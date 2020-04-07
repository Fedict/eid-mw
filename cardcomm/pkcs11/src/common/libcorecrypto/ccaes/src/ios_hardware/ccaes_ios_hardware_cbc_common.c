/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
 * 
 * corecrypto Internal Use License Agreement
 * 
 * IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
 * in consideration of your agreement to the following terms, and your download or use
 * of this Apple software constitutes acceptance of these terms.  If you do not agree
 * with these terms, please do not download or use this Apple software.
 * 
 * 1.	As used in this Agreement, the term "Apple Software" collectively means and
 * includes all of the Apple corecrypto materials provided by Apple here, including
 * but not limited to the Apple corecrypto software, frameworks, libraries, documentation
 * and other Apple-created materials. In consideration of your agreement to abide by the
 * following terms, conditioned upon your compliance with these terms and subject to
 * these terms, Apple grants you, for a period of ninety (90) days from the date you
 * download the Apple Software, a limited, non-exclusive, non-sublicensable license
 * under Apple’s copyrights in the Apple Software to make a reasonable number of copies
 * of, compile, and run the Apple Software internally within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software; provided
 * that you must retain this notice and the following text and disclaimers in all
 * copies of the Apple Software that you make. You may not, directly or indirectly,
 * redistribute the Apple Software or any portions thereof. The Apple Software is only
 * licensed and intended for use as expressly stated above and may not be used for other
 * purposes or in other contexts without Apple's prior written permission.  Except as
 * expressly stated in this notice, no other rights or licenses, express or implied, are
 * granted by Apple herein.
 * 
 * 2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
 * OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 * THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
 * SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
 * SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
 * OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
 * WILL CREATE A WARRANTY. 
 * 
 * 3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
 * IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
 * SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * 4.	This Agreement is effective until terminated. Your rights under this Agreement will
 * terminate automatically without notice from Apple if you fail to comply with any term(s)
 * of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
 * and destroy all copies, full or partial, of the Apple Software. This Agreement will be
 * governed and construed in accordance with the laws of the State of California, without
 * regard to its choice of law rules.
 * 
 * You may report security issues about Apple products to product-security@apple.com,
 * as described here:  https://www.apple.com/support/security/.  Non-security bugs and
 * enhancement requests can be made via https://bugreport.apple.com as described
 * here: https://developer.apple.com/bug-reporting/
 *
 * EA1350 
 * 10/5/15
 */


#include <corecrypto/cc_config.h>

#if CCAES_MUX

#include <corecrypto/cc_debug.h>
#include <unistd.h>
#include "ccaes_ios_hardware_cbc.h"
#include <IOKit/IOKitLib.h>
#include <Kernel/IOKit/crypto/IOAESTypes.h>
#include <sys/ioctl.h>


/*
 ccaes_hardware_threshold is being set to a constant of 1 so that hardware FIPS tests
 can directly call into this with a low threshold of bytes.  This shouldn't matter since
 iOS clients use this interface through the ccaes_ios_mux interface; which will use
 the 16K value.
 */

static int ccaes_device = -1;
static size_t ccaes_hardware_quantum = ((256*4096) / CCAES_BLOCK_SIZE);
static size_t ccaes_hardware_threshold = 1;

static void ccaes_ios_hardware_connect(void) {
    static dispatch_once_t	aesinit;
    dispatch_once(&aesinit, ^{
        struct IOAESAcceleratorInfo aesInfo;
        ccaes_device = open("/dev/aes_0", O_RDWR | O_NONBLOCK, 0);
        if(ccaes_device < 0) return;
        if(ioctl(ccaes_device, IOAES_GET_INFO, &aesInfo) != -1) {
            ccaes_hardware_quantum =  aesInfo.maxBytesPerCall / CCAES_BLOCK_SIZE;
            // For right now we're going to set the minimum to 1 block - allowing this
            // to function like any other aes-cbc modeObj. It can be tested while in this
            // configuration with the normal tests, although the round trips through the
            // kernel boundary are painfully slow for small block counts.
            ccaes_hardware_threshold = 1; // aesInfo.minBytesPerCall / CCAES_BLOCK_SIZE;
        }
    });
}


void
ccaes_ios_hardware_common_init(const struct ccmode_cbc *cbc CC_UNUSED, cccbc_ctx *cbcctx,
                               unsigned long rawkey_len, const void *rawkey)
{
    struct ccaes_hardware_aes_key* ctx = (struct ccaes_hardware_aes_key*) cbcctx;
    CC_MEMCPY(ctx->keyBytes, rawkey, ctx->keyLength = rawkey_len);
}

unsigned long ccaes_ios_hardware_crypt(int operation, ccaes_hardware_aes_cbc_ctx ctx, cccbc_iv *iv,
                            const void *in, void *out, unsigned long nblocks)
{
    uint8_t *pt8, *ct8;
	struct IOAESAcceleratorRequest aesRequest;
	
    if(nblocks < ccaes_hardware_threshold) return 0;
	size_t remaining = nblocks;
	size_t chunk;

    ccaes_ios_hardware_connect();
    if(ccaes_device < 0) return 0;

    if(operation) {
        aesRequest.operation = IOAESOperationEncrypt;
        pt8 = __DECONST(uint8_t *,in);
        ct8 = (uint8_t *) out;
    } else {
        aesRequest.operation = IOAESOperationDecrypt;
        pt8 = (uint8_t *) out;
        ct8 = __DECONST(uint8_t *,in);
    }
    
	CC_MEMCPY(aesRequest.iv.ivBytes, iv, CCAES_BLOCK_SIZE);
	aesRequest.keyData.key.keyLength = (UInt32) (ctx->keyLength << 3); //Hardware needs it in bits.
	CC_MEMCPY(aesRequest.keyData.key.keyBytes, ctx->keyBytes, ctx->keyLength);
	aesRequest.keyData.keyHandle = kIOAESAcceleratorKeyHandleExplicit;

    
	while (remaining) {
        for(chunk = remaining; chunk > ccaes_hardware_quantum; chunk >>= 1);
        aesRequest.plainText = pt8;
        aesRequest.cipherText = ct8;
        aesRequest.textLength = (IOByteCount32) (chunk * CCAES_BLOCK_SIZE); //The hardware needs textLength in bytes.
        if(ioctl(ccaes_device, IOAES_ENCRYPT_DECRYPT, &aesRequest) == -1) {
            break;
        }
        remaining -= chunk;
        pt8 += (chunk*CCAES_BLOCK_SIZE);
        ct8 += (chunk*CCAES_BLOCK_SIZE);
	}
	//Copy the IV back.
	CC_MEMCPY(iv, aesRequest.iv.ivBytes, CCAES_BLOCK_SIZE);
    cc_clear(ctx->keyLength,aesRequest.keyData.key.keyBytes); // zero key bytes
	return (nblocks - remaining);
} 

#endif /* CCAES_MUX */

