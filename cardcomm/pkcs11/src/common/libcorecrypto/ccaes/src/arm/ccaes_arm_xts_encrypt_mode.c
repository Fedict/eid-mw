/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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

#ifndef __NO_ASM__

#include <corecrypto/cc_config.h>

#if CCAES_ARM
#include "ccmode_internal.h"
#include "arm_aes.h"
#include <corecrypto/ccaes.h>

extern void ccaes_xts_encrypt_vng_vector(void *out, const void *in, cc_unit *tweak, unsigned long nblocks, const void *);

static void *ccaes_xts_encrypt_vng(const ccxts_ctx *key, ccxts_tweak *tweak,
                            unsigned long nblocks, const void *in, void *out)
{
    unsigned long numBlocks = CCMODE_XTS_TWEAK_BLOCK_PROCESSED(tweak);
    numBlocks += nblocks;
    if (numBlocks > (1 << 20))
    {
        return NULL;
    }
    CCMODE_XTS_TWEAK_BLOCK_PROCESSED(tweak) = numBlocks;
    cc_unit *t=CCMODE_XTS_TWEAK_VALUE(tweak);
#if defined(__arm__) && CC_KERNEL
    /*
     The armv7 implementation of ccaes_arm_encrypt needs in/out to be 4-bytes aligned in kernel mode.
     */
    int aligned_in[4], aligned_out[4];
    if ((((int)in&0x03)==0) && (((int)out&0x03)==0)) {  // both in and out are 4-byte aligned
        if (nblocks)
            ccaes_xts_encrypt_vng_vector(out, in, t, nblocks, ccmode_xts_key_data_key(key));
    } else {
        while (nblocks) {
            memcpy((void*)aligned_in, in, CCAES_BLOCK_SIZE);
            ccaes_xts_encrypt_vng_vector(aligned_out, aligned_in, t, 1, ccmode_xts_key_data_key(key));
            memcpy(out, (void*)aligned_out, CCAES_BLOCK_SIZE);
            in += CCAES_BLOCK_SIZE;
            out += CCAES_BLOCK_SIZE;
            nblocks--;
        }
    }
#else
    const cc_unit *input = in;
    cc_unit *output = out;
    if (nblocks) {
        ccaes_xts_encrypt_vng_vector(output, input, t, nblocks, ccmode_xts_key_data_key(key));
    }
#endif
    return t;
}

const struct ccmode_xts ccaes_arm_xts_encrypt_mode = { \
    .size = ccn_sizeof_size(sizeof(struct _ccmode_xts_key)) + 2 * ccn_sizeof_size(sizeof(ccaes_arm_encrypt_ctx)), \
    .tweak_size = ccn_sizeof_size(sizeof(struct _ccmode_xts_tweak)) + ccn_sizeof_size(16), \
    .block_size = 16, \
    .init = ccmode_xts_init, \
    .set_tweak = ccmode_xts_set_tweak, \
    .xts = ccaes_xts_encrypt_vng, \
    .custom = (&ccaes_arm_ecb_encrypt_mode), \
    .custom1 = (&ccaes_arm_ecb_encrypt_mode) \
};


#endif

#endif /* __NO_ASM__ */
