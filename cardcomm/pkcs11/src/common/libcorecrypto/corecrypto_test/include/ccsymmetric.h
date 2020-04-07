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


#ifndef corecrypto_ccsymmetric_h
#define corecrypto_ccsymmetric_h

#include <corecrypto/ccmode.h>
#include <corecrypto/ccmode_impl.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccdes.h>
#include <corecrypto/cccast.h>
#include <corecrypto/ccrc2.h>
#include <corecrypto/ccblowfish.h>


#define CC_SUCCESS 0
#define CC_FAILURE -1
#define CC_UNIMPLEMENTED -2
typedef int cc_status;

// Ciphers
enum {
    cc_cipherAES        = 0,
    cc_cipherDES        = 1,
    cc_cipher3DES       = 2,
    cc_cipherCAST       = 3,
    cc_cipherRC2        = 4,
    cc_cipherBlowfish   = 5,
    cc_NCiphers         = 6,
};
typedef uint32_t cc_cipher_select;

// Modes
enum {
	cc_ModeECB		= 0,
	cc_ModeCBC		= 1,
	cc_ModeCFB		= 2,
	cc_ModeCTR		= 3,
	cc_ModeOFB		= 4,
	cc_ModeXTS		= 5,
	cc_ModeCFB8		= 6,
	cc_ModeGCM		= 7,
    cc_ModeCCM		= 8,
    cc_NModes      = 9,
};
typedef uint32_t cc_mode_select;

// Directions
enum {
	cc_Encrypt		= 0,
	cc_Decrypt		= 1,
    cc_NDirections = 2,
};
typedef uint32_t cc_direction;

typedef union  {
    const void *data;
    const struct ccmode_ecb *ecb;
    const struct ccmode_cbc *cbc;
    const struct ccmode_cfb *cfb;
    const struct ccmode_cfb8 *cfb8;
    const struct ccmode_ctr *ctr;
    const struct ccmode_ofb *ofb;
    const struct ccmode_xts *xts;
    const struct ccmode_gcm *gcm;
    const struct ccmode_ccm *ccm;
} ciphermode_t;

typedef struct cc_ciphermode_descriptor_t {
    cc_cipher_select cipher;
    cc_mode_select mode;
    cc_direction direction;
    ciphermode_t ciphermode;
} cc_ciphermode_descriptor_s, *cc_ciphermode_descriptor;

cc_aligned_struct(16) cc_aligned_ctx;

typedef union {
    cc_aligned_ctx *data;
    ccecb_ctx *ecb;
    cccbc_ctx *cbc;
    cccfb_ctx *cfb;
    cccfb8_ctx *cfb8;
    ccctr_ctx *ctr;
    ccofb_ctx *ofb;
    ccxts_ctx *xts;
    ccgcm_ctx *gcm;
    ccccm_ctx *ccm;
} mode_ctx;

typedef union {
    cc_aligned_ctx *data;
    ccccm_nonce *ccm_nonce;
} extra_ctx;

typedef struct cc_symmetric_context_t {
    cc_ciphermode_descriptor mode_desc;
    mode_ctx  ctx;
    extra_ctx xtra_ctx;
} cc_symmetric_context, *cc_symmetric_context_p;

int
cc_get_ciphermode(cc_cipher_select cipher, cc_mode_select mode, cc_direction direction, cc_ciphermode_descriptor desc);

int
cc_get_C_ciphermode(cc_cipher_select cipher, cc_mode_select mode, cc_direction direction, cc_ciphermode_descriptor desc);

int
cc_symmetric_setup(cc_ciphermode_descriptor cm, const void *key, size_t keylen, const void *iv, cc_symmetric_context_p ctx);

int
cc_symmetric_setup_tweaked(cc_ciphermode_descriptor cm, const void *key, size_t keylen, const void *tweak, const void *iv, cc_symmetric_context_p ctx);

int
cc_symmetric_setup_authenticated(cc_ciphermode_descriptor cm, const void *key, size_t keylen,
                                 const void *iv, size_t iv_len,
                                 const void *adata, size_t adata_len,
                                 size_t data_len,
                                 size_t tag_len,
                                 cc_symmetric_context_p ctx);

int
cc_symmetric_crypt(cc_symmetric_context_p ctx, const void *iv, const void *in, void *out, size_t len);

void
cc_symmetric_final(cc_symmetric_context_p ctx);

void
cc_symmetric_authenticated_finalize(cc_symmetric_context_p ctx, char *tag, size_t tag_len);

int
cc_symmetric_oneshot(cc_ciphermode_descriptor cm, const void *key, size_t keylen,
                     const void *iv, const void *in, void *out, size_t len);

static inline size_t get_context_size_in_bytes(cc_ciphermode_descriptor cm) {
    return (cm->ciphermode.ecb->size);
                                    // This assumes the compiler always puts the size field first
                                    // if not we're going to have to build a switch statement and
                                    // go through all the modes.
}

static inline size_t get_extra_context_size_in_bytes(cc_ciphermode_descriptor cm) {
    // Allocate an extra context (ex: nonce in CCM)
    if (cm->mode==cc_ModeCCM) {
        return cm->ciphermode.ccm->nonce_size;
    }
    return 1; // To prevent warning about declaring a zero length variable
}

// Generic mode context (cc_symmetric_ctx) aligned on 16bytes.
#define MAKE_GENERIC_MODE_CONTEXT(_name_,_descriptor_) \
    cc_ctx_decl(cc_aligned_ctx, get_context_size_in_bytes(_descriptor_), __ctx_##_name_); \
    cc_ctx_decl(cc_aligned_ctx, get_extra_context_size_in_bytes(_descriptor_), __extra_ctx_##_name_);\
    cc_symmetric_context __##_name_={_descriptor_,{__ctx_##_name_},{__extra_ctx_##_name_}}; \
    cc_symmetric_context_p _name_=&__##_name_;

unsigned long
cc_symmetric_bloc_size(cc_ciphermode_descriptor cm);

#endif /* corecrypto_ccsymmetric_h */
