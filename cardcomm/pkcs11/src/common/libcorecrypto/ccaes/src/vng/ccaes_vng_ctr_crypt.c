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


#include <corecrypto/cc_runtime_config.h>
#include <corecrypto/ccaes.h>
#include "ccaes_vng_ctr.h"

#if !defined(__NO_ASM__) && CCMODE_CTR_VNG_SPEEDUP

#if defined(__x86_64__) || defined(__arm64__)

// In assembly
extern void aes_ctr_crypt(const void *pt, void *ct, int, void *ctr, void *);

// for arm64 or intel aesni
void ccaes_vng_ctr_crypt(ccctr_ctx *key,
                      size_t nbytes, const void *in, void *out) {
    const struct ccmode_ecb *ecb = CCMODE_CTR_KEY_ECB(key);
    ccecb_ctx *ecb_key = CCMODE_CTR_KEY_ECB_KEY(key);
    uint8_t *ctr = (uint8_t *)CCMODE_CTR_KEY_CTR(key);
    uint8_t *pad = (uint8_t *)CCMODE_CTR_KEY_PAD(key);
    cc_size pad_len = CCMODE_CTR_KEY_PAD_LEN(key);
    const uint8_t *pt = in;
    uint8_t *ct = out;

    while (nbytes) {
        if (pad_len == CCAES_BLOCK_SIZE) {

#ifdef  __x86_64__
            if ( CC_HAS_AESNI() && CC_HAS_SupplementalSSE3() )
#endif  // __x86_64__
            {
                if (nbytes >= CCAES_BLOCK_SIZE) {
                    unsigned int j = (unsigned int) (nbytes & (-CCAES_BLOCK_SIZE));
                    aes_ctr_crypt((const void*) pt, (void*) ct, j, (void *) ctr, (void *) ecb_key);
                    ct += j;    pt += j;    nbytes -= j;
                }
            }
            ecb->ecb(ecb_key, 1, ctr, pad);
            pad_len = 0;

            /* increment the big endian counter (64bit) */
            for (size_t x = CCAES_BLOCK_SIZE; x-- > CCAES_BLOCK_SIZE-8;) {
                ctr[x] = (ctr[x] + (unsigned char)1) & (unsigned char)255;
                if (ctr[x] != (unsigned char)0) {
                    break;
                }
            }

            if (nbytes==0) break;
        }

        do {
            *ct++ = *pt++ ^ pad[pad_len++];
            --nbytes;
        } while ((nbytes>0)&&(pad_len<CCAES_BLOCK_SIZE));
    }
    CCMODE_CTR_KEY_PAD_LEN(key) = pad_len;
}

#else

// for arm32 with __ARM_NEON__

typedef __attribute__((neon_vector_type(8))) uint8_t uint8x8_t;
typedef __attribute__((neon_vector_type(8))) int8_t int8x8_t;
typedef __attribute__((neon_vector_type(1))) uint64_t uint64x1_t;
typedef __attribute__((neon_vector_type(16))) uint8_t uint8x16_t;

void ccaes_vng_ctr_crypt(ccctr_ctx *key,
                      size_t nbytes, const void *in, void *out) {
    const struct ccmode_ecb *ecb = CCMODE_CTR_KEY_ECB(key);
    const ccecb_ctx *ecb_key = CCMODE_CTR_KEY_ECB_KEY(key);
    uint8_t *ctr = (uint8_t *)CCMODE_CTR_KEY_CTR(key);
    uint8_t *pad = (uint8_t *)CCMODE_CTR_KEY_PAD(key);
    cc_size pad_len = CCMODE_CTR_KEY_PAD_LEN(key);
    const uint8_t *pt = in;
    // Counter is 64bit wide for cipher with block size of 64bit or more
    // This is to match the assembly
    // const size_t counter_size=(CC_MIN(ecb->block_size,(typeof(ecb->block_size))8));
    uint8_t *ct = out;

    uint8x8_t ctr_vec = (uint8x8_t) __builtin_neon_vld1_v(ctr+8, 16);
    uint8x8_t rev_ctr = __builtin_shufflevector(ctr_vec, ctr_vec, 7, 6, 5, 4, 3, 2, 1, 0);
    uint8x8_t rev_one = (const uint8x8_t) { 1, 0, 0, 0, 0, 0, 0, 0 };

    while (nbytes) {

        if (pad_len == CCAES_BLOCK_SIZE) {
            ecb->ecb(ecb_key, 1, ctr, pad);
            pad_len = 0;

            /* increment the big endian counter */
           rev_ctr = ((uint64x1_t)rev_ctr + (uint64x1_t)rev_one);
            ctr_vec = __builtin_shufflevector(rev_ctr, rev_ctr, 7, 6, 5, 4, 3, 2, 1, 0);
            __builtin_neon_vst1_v(ctr+8, (int8x8_t)ctr_vec, 16);
        }

        if (pad_len == 0 && nbytes >= CCAES_BLOCK_SIZE) {
            uint8x16_t __v0 = (uint8x16_t) __builtin_neon_vld1q_v(pt, 48);
            uint8x16_t __v1 = (uint8x16_t) __builtin_neon_vld1q_v(pad, 48);
            __v0 = __v0 ^ __v1;
            __builtin_neon_vst1q_v(ct, __v0, 48);
            pad_len = CCAES_BLOCK_SIZE;
            pt += pad_len;
            ct += pad_len;
            nbytes -= pad_len;
        }

        while ((nbytes>0)&&(pad_len<CCAES_BLOCK_SIZE)) {
            *ct++ = *pt++ ^ pad[pad_len++];
            --nbytes;
        } 

    }
    CCMODE_CTR_KEY_PAD_LEN(key) = pad_len;
}


#endif  // arm32 with NEON)

#endif  // CCMODE_CTR_VNG_SPEEDUP
