/*
 * Copyright (c) 2010,2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CCMODE_INTERNAL_H_
#define _CORECRYPTO_CCMODE_INTERNAL_H_

#include <corecrypto/ccmode.h>
#include <corecrypto/ccmode_factory.h>
#include <corecrypto/cc_priv.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/cc_debug.h>


/* Helper function used.  TODO: Probably not specific to xts, since
   gcm uses it too */
void ccmode_xts_mult_alpha(cc_unit *tweak);

/* Macros for accessing a CCMODE_CBC_KEY.
 {
     const struct ccmode_ecb *ecb
     ccn_unit ecb_key[ecb->n]
 } */
#define _CCMODE_CBC_KEY(K)       ((struct _ccmode_cbc_key *)(K))
#define _CCMODE_CBC_KEY_CONST(K) ((const struct _ccmode_cbc_key *)(K))
#define CCMODE_CBC_KEY_ECB(K) (_CCMODE_CBC_KEY(K)->ecb)
#define CCMODE_CBC_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_CBC_KEY(K)->u[0])

CC_CONST CC_INLINE
const struct ccmode_ecb * ccmode_cbc_key_ecb(const cccbc_ctx *K) {
    return ((const struct _ccmode_cbc_key *)K)->ecb;
}

CC_CONST CC_INLINE
const ccecb_ctx * ccmode_cbc_key_ecb_key(const cccbc_ctx *K) {
    return (const ccecb_ctx *)&((const struct _ccmode_cbc_key *)K)->u[0];
}

/* Macros for accessing a CCMODE_CFB_KEY.
{
    const struct ccmode_ecb *ecb
    cc_size pad_len;
    ccn_unit pad[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit iv[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit ecb_key[ecb->n]
} */
#define _CCMODE_CFB_KEY(K) ((struct _ccmode_cfb_key *)(K))
#define CCMODE_CFB_KEY_ECB(K) (_CCMODE_CFB_KEY(K)->ecb)
#define CCMODE_CFB_KEY_PAD_LEN(K) (_CCMODE_CFB_KEY(K)->pad_len)
#define CCMODE_CFB_KEY_PAD(K) (&_CCMODE_CFB_KEY(K)->u[0])
#define CCMODE_CFB_KEY_IV(K) (&_CCMODE_CFB_KEY(K)->u[ccn_nof_size(CCMODE_CFB_KEY_ECB(K)->block_size)])
#define CCMODE_CFB_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_CFB_KEY(K)->u[2 * ccn_nof_size(CCMODE_CFB_KEY_ECB(K)->block_size)])

/* Macros for accessing a CCMODE_CFB8_KEY.
{
    const struct ccmode_ecb *ecb
    ccn_unit pad[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit iv[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit ecb_key[ecb->n]
} */
#define _CCMODE_CFB8_KEY(K) ((struct _ccmode_cfb8_key *)(K))
#define CCMODE_CFB8_KEY_ECB(K) (_CCMODE_CFB8_KEY(K)->ecb)
#define CCMODE_CFB8_KEY_PAD(K) (&_CCMODE_CFB8_KEY(K)->u[0])
#define CCMODE_CFB8_KEY_IV(K) (&_CCMODE_CFB8_KEY(K)->u[ccn_nof_size(CCMODE_CFB8_KEY_ECB(K)->block_size)])
#define CCMODE_CFB8_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_CFB8_KEY(K)->u[2 * ccn_nof_size(CCMODE_CFB8_KEY_ECB(K)->block_size)])


/* Macros for accessing a CCMODE_CTR_KEY.
{
    const struct ccmode_ecb *ecb
    cc_size pad_len;
    ccn_unit pad[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit ctr[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit ecb_key[ecb->n]
} */
#define _CCMODE_CTR_KEY(K) ((struct _ccmode_ctr_key *)(K))
#define CCMODE_CTR_KEY_ECB(K) (_CCMODE_CTR_KEY(K)->ecb)
#define CCMODE_CTR_KEY_PAD_LEN(K) (_CCMODE_CTR_KEY(K)->pad_len)
#define CCMODE_CTR_KEY_PAD(K) (&_CCMODE_CTR_KEY(K)->u[0])
#define CCMODE_CTR_KEY_CTR(K) (&_CCMODE_CTR_KEY(K)->u[ccn_nof_size(CCMODE_CFB8_KEY_ECB(K)->block_size)])
#define CCMODE_CTR_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_CTR_KEY(K)->u[2 * ccn_nof_size(CCMODE_CFB8_KEY_ECB(K)->block_size)])


/* Macros for accessing a CCMODE_OFB_KEY.
{
    const struct ccmode_ecb *ecb
    cc_size pad_len;
    ccn_unit iv[ecb->block_size / CCN_UNIT_SIZE];
    ccn_unit ecb_key[ecb->n]
} */
#define _CCMODE_OFB_KEY(K) ((struct _ccmode_ofb_key *)(K))
#define CCMODE_OFB_KEY_ECB(K) (_CCMODE_OFB_KEY(K)->ecb)
#define CCMODE_OFB_KEY_PAD_LEN(K) (_CCMODE_OFB_KEY(K)->pad_len)
#define CCMODE_OFB_KEY_IV(K) (&_CCMODE_OFB_KEY(K)->u[0])
#define CCMODE_OFB_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_OFB_KEY(K)->u[ccn_nof_size(CCMODE_OFB_KEY_ECB(K)->block_size)])


/* Macros for accessing a CCMODE_XTS_KEY.
{
    const struct ccmode_ecb *ecb
    const struct ccmode_ecb *ecb_encrypt
    ccn_unit data_key[ecb->size]
    ccn_unit tweak_key[ecb_encrypt->size]
} */
#define _CCMODE_XTS_KEY(K) ((struct _ccmode_xts_key *)(K))
#define CCMODE_XTS_KEY_ECB(K) (_CCMODE_XTS_KEY(K)->ecb)
#define CCMODE_XTS_KEY_ECB_ENCRYPT(K) (_CCMODE_XTS_KEY(K)->ecb_encrypt)
#define CCMODE_XTS_KEY_DATA_KEY(K) ((ccecb_ctx *)&_CCMODE_XTS_KEY(K)->u[0])
#define CCMODE_XTS_KEY_TWEAK_KEY(K) ((ccecb_ctx *)&_CCMODE_XTS_KEY(K)->u[ccn_nof_size(CCMODE_XTS_KEY_ECB(K)->size)])

CC_CONST CC_INLINE
const struct ccmode_ecb * ccmode_xts_key_ecb(const ccxts_ctx *K) {
    return ((const struct _ccmode_xts_key *)K)->ecb;
}

CC_CONST CC_INLINE
const struct ccmode_ecb * ccmode_xts_key_ecb_encrypt(const ccxts_ctx *K) {
    return ((const struct _ccmode_xts_key *)K)->ecb_encrypt;
}

CC_CONST CC_INLINE
const ccecb_ctx * ccmode_xts_key_data_key(const ccxts_ctx *K) {
    return (const ccecb_ctx *)&((const struct _ccmode_xts_key *)K)->u[0];
}

CC_CONST CC_INLINE
const ccecb_ctx * ccmode_xts_key_tweak_key(const ccxts_ctx *K) {
    return (const ccecb_ctx *)&((const struct _ccmode_xts_key *)K)->u[ccn_nof_size(ccmode_xts_key_ecb(K)->size)];
}

/* Macros for accessing a CCMODE_XTS_TWEAK.
{
 unsigned long  blocks_processed;
 uint8_t value[16]; 
} */
#define _CCMODE_XTS_TWEAK(T) ((struct _ccmode_xts_tweak *)(T))
#define CCMODE_XTS_TWEAK_BLOCK_PROCESSED(T)(_CCMODE_XTS_TWEAK(T)->blocks_processed)
#define CCMODE_XTS_TWEAK_VALUE(T) (_CCMODE_XTS_TWEAK(T)->u)


/* Macros for accessing a CCMODE_GCM_KEY.
 Common to the generic (factory) and the VNG implementation
*/

#define _CCMODE_GCM_KEY(K) ((struct _ccmode_gcm_key *)(K))
#define CCMODE_GCM_KEY_H(K) (_CCMODE_GCM_KEY(K)->H)
#define CCMODE_GCM_KEY_X(K) (_CCMODE_GCM_KEY(K)->X)
#define CCMODE_GCM_KEY_Y(K) (_CCMODE_GCM_KEY(K)->Y)
#define CCMODE_GCM_KEY_Y_0(K) (_CCMODE_GCM_KEY(K)->Y_0)
#define CCMODE_GCM_KEY_PAD_LEN(K) (_CCMODE_GCM_KEY(K)->buflen)
#define CCMODE_GCM_KEY_PAD(K) (_CCMODE_GCM_KEY(K)->buf)

#define _CCMODE_GCM_ECB_MODE(K) ((struct _ccmode_gcm_key *)(K))
#define CCMODE_GCM_KEY_ECB(K) (_CCMODE_GCM_ECB_MODE(K)->ecb)
#define CCMODE_GCM_KEY_ECB_KEY(K) ((ccecb_ctx *)_CCMODE_GCM_ECB_MODE(K)->ecb_key)  // set in init function

#define CCMODE_GCM_MODE_IV    0
#define CCMODE_GCM_MODE_AAD   1
#define CCMODE_GCM_MODE_TEXT  2

void ccmode_gcm_gf_mult(const unsigned char *a, const unsigned char *b,
                        unsigned char *c);
void ccmode_gcm_mult_h(ccgcm_ctx *key, unsigned char *I);

/* Macros for accessing a CCMODE_CCM_KEY. */
#define _CCMODE_CCM_KEY(K) ((struct _ccmode_ccm_key *)(K))
#define CCMODE_CCM_KEY_ECB(K) (_CCMODE_CCM_KEY(K)->ecb)
#define CCMODE_CCM_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_CCM_KEY(K)->u[0])

#define _CCMODE_CCM_NONCE(N) ((struct _ccmode_ccm_nonce *)(N))
#define CCMODE_CCM_KEY_MAC(N) (_CCMODE_CCM_NONCE(N)->MAC)
#define CCMODE_CCM_KEY_A_I(N) (_CCMODE_CCM_NONCE(N)->A_i)
#define CCMODE_CCM_KEY_B_I(N) (_CCMODE_CCM_NONCE(N)->B_i)
#define CCMODE_CCM_KEY_PAD_LEN(N) (_CCMODE_CCM_NONCE(N)->buflen)
#define CCMODE_CCM_KEY_PAD(N) (_CCMODE_CCM_NONCE(N)->buf)
#define CCMODE_CCM_KEY_MAC_LEN(N) (_CCMODE_CCM_NONCE(N)->mac_size)
#define CCMODE_CCM_KEY_NONCE_LEN(N) (_CCMODE_CCM_NONCE(N)->nonce_size)
#define CCMODE_CCM_KEY_AUTH_LEN(N) (_CCMODE_CCM_NONCE(N)->b_i_len)

#define CCMODE_CCM_MODE_IV    0
#define CCMODE_CCM_MODE_AAD   1
#define CCMODE_CCM_MODE_TEXT  2


/* Macros for accessing a CCMODE_OMAC_KEY.
{
    const struct ccmode_ecb *ecb
    cc_size tweak_size;
    ccn_unit ecb_key1[ecb->n]
    ccn_unit ecb_key2[ecb->n]
} */
#define _CCMODE_OMAC_KEY(K) ((struct _ccmode_omac_key *)(K))
#define CCMODE_OMAC_KEY_ECB(K) (_CCMODE_OMAC_KEY(K)->ecb)
#define CCMODE_OMAC_KEY_TWEAK_LEN(K) (_CCMODE_OMAC_KEY(K)->tweak_len)
#define CCMODE_OMAC_KEY_ECB_KEY(K) ((ccecb_ctx *)&_CCMODE_OMAC_KEY(K)->u[0])

#endif /* _CORECRYPTO_CCMODE_INTERNAL_H_ */
