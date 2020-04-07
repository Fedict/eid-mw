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


#include <corecrypto/ccaes.h>
#include "ccmode_internal.h"

#if	!defined(__NO_ASM__) && \
    ((CCAES_INTEL && defined(__x86_64__)) || (CCAES_ARM && defined(__ARM_NEON__)))
#define	CCMODE_GCM_VNG_SPEEDUP	1
#else
#define	CCMODE_GCM_VNG_SPEEDUP	0
#endif


#if CCMODE_GCM_VNG_SPEEDUP
void ccaes_vng_gcm_init(const struct ccmode_gcm *gcm, ccgcm_ctx *key,
                        size_t rawkey_len, const void *rawkey);


void ccaes_vng_gcm_set_iv(ccgcm_ctx *key, size_t iv_size, const void *iv);

void ccaes_vng_gcm_gmac(ccgcm_ctx *key, size_t nbytes, const void *in);

void ccaes_vng_gcm_decrypt(ccgcm_ctx *key, size_t nbytes,
                           const void *in, void *out);

void ccaes_vng_gcm_encrypt(ccgcm_ctx *key, size_t nbytes,
                           const void *in, void *out);

void ccaes_vng_gcm_finalize(ccgcm_ctx *key, size_t tag_size, void *tag);

void ccaes_vng_gcm_mult_h(ccgcm_ctx *key, unsigned char *I);

void ccaes_vng_gcm_gf_mult(const unsigned char *a, const unsigned char *b,
                           unsigned char *c);

extern void gcm_init(void *Htable, void *H);
extern void gcm_gmult(const void *X, const void *Htable, void *out);
extern void gcm_ghash(void *X, void *Htable, const void *in, size_t len);
#ifdef  __x86_64__
extern void gcmEncrypt_SupplementalSSE3(const void*, void*, void*, unsigned int, void*, void*);
extern void gcmDecrypt_SupplementalSSE3(const  void*, void*, void*, unsigned int, void*, void*);
extern void gcmEncrypt_avx1(const void*, void*, void*, unsigned int, void*, void*);
extern void gcmDecrypt_avx1(const void*, void*, void*, unsigned int, void*, void*);
#else
extern void gcmEncrypt(const void*, void*, void*, unsigned int, void*, void*);
extern void gcmDecrypt(const void*, void*, void*, unsigned int, void*, void*);
#endif

struct _ccaes_vng_gcm_tables {
#if !defined(__arm64__) && defined(__ARM_NEON__)
    unsigned char Htable[8*2] __attribute__((aligned(16)));
#else
    unsigned char Htable[16*8*2] __attribute__((aligned(16)));
#endif
};
#define VNG_GCM_TABLE_SIZE sizeof(struct _ccaes_vng_gcm_tables)

#define CCMODE_GCM_KEY_Htable(K) (((struct _ccaes_vng_gcm_tables*)&_CCMODE_GCM_KEY(K)->u[0])->Htable)

/* Use this to statically initialize a ccmode_gcm object for decryption. */
#define CCAES_VNG_GCM_DECRYPT(ECB_ENCRYPT) { \
.size = ccn_sizeof_size(sizeof(struct _ccmode_gcm_key))  \
+ GCM_ECB_KEY_SIZE(ECB_ENCRYPT)  \
+ VNG_GCM_TABLE_SIZE, \
.block_size = 1, \
.init = ccaes_vng_gcm_init, \
.set_iv = ccaes_vng_gcm_set_iv, \
.gmac = ccaes_vng_gcm_gmac, \
.gcm = ccaes_vng_gcm_decrypt, \
.finalize = ccaes_vng_gcm_finalize, \
.reset = ccmode_gcm_reset, \
.custom = (ECB_ENCRYPT) \
}

/* Use this to statically initialize a ccmode_gcm object for encryption. */
#define CCAES_VNG_GCM_ENCRYPT(ECB_ENCRYPT) { \
.size = ccn_sizeof_size(sizeof(struct _ccmode_gcm_key))  \
+ GCM_ECB_KEY_SIZE(ECB_ENCRYPT)  \
+ VNG_GCM_TABLE_SIZE, \
.block_size = 1, \
.init = ccaes_vng_gcm_init, \
.set_iv = ccaes_vng_gcm_set_iv, \
.gmac = ccaes_vng_gcm_gmac, \
.gcm = ccaes_vng_gcm_encrypt, \
.finalize = ccaes_vng_gcm_finalize, \
.reset = ccmode_gcm_reset, \
.custom = (ECB_ENCRYPT) \
}

/* Use these function to runtime initialize a ccmode_gcm decrypt object (for
 example if it's part of a larger structure). For GCM you always pass a
 ecb encrypt mode implementation of some underlying algorithm as the ecb
 parameter. */
CC_INLINE
void ccaes_vng_gcm_decrypt_mode_setup(struct ccmode_gcm *gcm) {
    struct ccmode_gcm gcm_decrypt = CCAES_VNG_GCM_DECRYPT(ccaes_ecb_encrypt_mode());
    *gcm = gcm_decrypt;
}

/* Use these function to runtime initialize a ccmode_gcm encrypt object (for
 example if it's part of a larger structure). For GCM you always pass a
 ecb encrypt mode implementation of some underlying algorithm as the ecb
 parameter. */
CC_INLINE
void ccaes_vng_gcm_encrypt_mode_setup(struct ccmode_gcm *gcm) {
    struct ccmode_gcm gcm_encrypt = CCAES_VNG_GCM_ENCRYPT(ccaes_ecb_encrypt_mode());
    *gcm = gcm_encrypt;
}

#endif /* CCMODE_GCM_VNG_SPEEDUP */

