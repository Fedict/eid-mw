/*
 * Copyright (c) 2013,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccnistkdf.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/cc.h>

/*
 
 From:
 
 NIST Special Publication 800-108
 Recommendation for Key Derivation
 Using Pseudorandom Functions
 
 http://csrc.nist.gov/publications/nistpubs/800-108/sp800-108.pdf
 Section 5.1
 KDF in Counter Mode

 Fixed values:
 1. h - The length of the output of the PRF in bits, and
 2. r - The length of the binary representation of the counter i.
 
 Input: KI, Label, Context, and L.
 
 Process:
 1. n := ⎡L/h⎤.
 2. If n > 2r-1, then indicate an error and stop.
 3. result(0):= ∅.
 4. For i = 1 to n, do
    a. K(i) := PRF (KI, [i]2 || Label || 0x00 || Context || [L]2) 12
    b. result(i) := result(i-1) || K(i). 5. Return: KO := the leftmost L bits of result(n).
 
 Output: KO.
 
 In each iteration, the fixed input data is the string Label || 0x00 || Context || [L]2. The
 counter [i]2 is the iteration variable and is represented as a binary string of r bits.
 
 Implementation note:
 
 r is 32 (sizeof(uint32_t))
 h is dependent upon the passed-in digest_info output size.

*/

#include "ccnistkdf_priv.h"

static void
F (const struct ccdigest_info *di, cchmac_ctx_t hc, const cc_unit *istate,
   size_t counter, size_t fixedDataLen, const void *fixedData, void *result) {
    cchmac_reset_from_cache(di, hc, istate);
    ccdigest_update_uint32_t(di, cchmac_digest_ctx(di, hc), (uint32_t) counter);
    ccdigest_update(di, cchmac_digest_ctx(di, hc), fixedDataLen, fixedData);
    cchmac_final(di, hc, result);
}

int ccnistkdf_ctr_hmac_fixed(const struct ccdigest_info *di,
                       size_t kdkLen, const void *kdk,
                       size_t fixedDataLen, const void *fixedData,
                       size_t dkLen, void *dk) {
    size_t h = di->output_size;
    size_t n = cc_div_ceiling(dkLen, h);
    uint8_t result_buf[n*h];
    uint8_t *result = result_buf;
    
    if(n > UINT32_MAX) return -1;
    if(kdkLen == 0 || kdk == NULL) return -1;
    if(dkLen == 0 || dk == NULL) return -1;
    
    cchmac_di_decl(di, hc);
    cchmac_state_cache(di, istate);
    cchmac_init(di, hc, kdkLen, kdk);
    cchmac_cache_state(di, hc, istate);
    
    for(size_t i = 1; i <= n; i++, result += h) {
        F(di, hc, istate, i, fixedDataLen, fixedData, result);
    }
    
    CC_MEMCPY(dk, result_buf, dkLen);
    cc_clear(n*h,result_buf);
	cchmac_di_clear(di, hc);
	cc_clear(di->state_size, istate);
    
    return 0;
}

int ccnistkdf_ctr_hmac(const struct ccdigest_info *di,
                       size_t kdkLen, const void *kdk,
                       size_t labelLen, const void *label,
                       size_t contextLen, CC_UNUSED const void *context,
                       size_t dkLen, void *dk) {
    size_t fixedDataLen = labelLen + contextLen + 5;
    uint8_t fixedData[fixedDataLen];
    construct_fixed_data(labelLen, label, contextLen, context, dkLen, fixedData);
    int retval = ccnistkdf_ctr_hmac_fixed(di, kdkLen, kdk, fixedDataLen, fixedData, dkLen, dk);
    cc_clear(fixedDataLen,fixedData);
    return retval;
}

