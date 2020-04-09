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


#include "ccsymmetric.h"

int
cc_get_ciphermode(cc_cipher_select cipher, cc_mode_select mode, cc_direction direction, cc_ciphermode_descriptor desc) {
    desc->cipher = cipher;
    desc->mode = mode;
    desc->direction = direction;
    desc->ciphermode.data = NULL;
    int op = direction == cc_Encrypt; // save editting flip-flop logic
    switch(cipher) {
        case cc_cipherAES:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb = (op) ? ccaes_ecb_encrypt_mode(): ccaes_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? ccaes_cbc_encrypt_mode(): ccaes_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? ccaes_cfb_encrypt_mode(): ccaes_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  ccaes_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  ccaes_ofb_crypt_mode(); break;
                case cc_ModeXTS: desc->ciphermode.xts =  (op) ? ccaes_xts_encrypt_mode(): ccaes_xts_decrypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? ccaes_cfb8_encrypt_mode(): ccaes_cfb8_decrypt_mode(); break;
                case cc_ModeGCM: desc->ciphermode.gcm =  (op) ? ccaes_gcm_encrypt_mode(): ccaes_gcm_decrypt_mode(); break;
                case cc_ModeCCM: desc->ciphermode.ccm =  (op) ? ccaes_ccm_encrypt_mode(): ccaes_ccm_decrypt_mode(); break;
            }  break;
        case cc_cipherDES:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb =  (op) ? ccdes_ecb_encrypt_mode(): ccdes_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? ccdes_cbc_encrypt_mode(): ccdes_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? ccdes_cfb_encrypt_mode(): ccdes_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  ccdes_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  ccdes_ofb_crypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? ccdes_cfb8_encrypt_mode(): ccdes_cfb8_decrypt_mode(); break;
            }  break;
        case cc_cipher3DES:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb =  (op) ? ccdes3_ecb_encrypt_mode(): ccdes3_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? ccdes3_cbc_encrypt_mode(): ccdes3_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? ccdes3_cfb_encrypt_mode(): ccdes3_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  ccdes3_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  ccdes3_ofb_crypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? ccdes3_cfb8_encrypt_mode(): ccdes3_cfb8_decrypt_mode(); break;
            }  break;
        case cc_cipherCAST:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb =  (op) ? cccast_ecb_encrypt_mode(): cccast_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? cccast_cbc_encrypt_mode(): cccast_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? cccast_cfb_encrypt_mode(): cccast_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  cccast_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  cccast_ofb_crypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? cccast_cfb8_encrypt_mode(): cccast_cfb8_decrypt_mode(); break;
            }  break;
        case cc_cipherRC2:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb =  (op) ? ccrc2_ecb_encrypt_mode(): ccrc2_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? ccrc2_cbc_encrypt_mode(): ccrc2_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? ccrc2_cfb_encrypt_mode(): ccrc2_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  ccrc2_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  ccrc2_ofb_crypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? ccrc2_cfb8_encrypt_mode(): ccrc2_cfb8_decrypt_mode(); break;
            }  break;
        case cc_cipherBlowfish:
            switch(mode) {
                case cc_ModeECB: desc->ciphermode.ecb =  (op) ? ccblowfish_ecb_encrypt_mode(): ccblowfish_ecb_decrypt_mode(); break;
                case cc_ModeCBC: desc->ciphermode.cbc =  (op) ? ccblowfish_cbc_encrypt_mode(): ccblowfish_cbc_decrypt_mode(); break;
                case cc_ModeCFB: desc->ciphermode.cfb =  (op) ? ccblowfish_cfb_encrypt_mode(): ccblowfish_cfb_decrypt_mode(); break;
                case cc_ModeCTR: desc->ciphermode.ctr =  ccblowfish_ctr_crypt_mode(); break;
                case cc_ModeOFB: desc->ciphermode.ofb =  ccblowfish_ofb_crypt_mode(); break;
                case cc_ModeCFB8: desc->ciphermode.cfb8 =  (op) ? ccblowfish_cfb8_encrypt_mode(): ccblowfish_cfb8_decrypt_mode(); break;
            }  break;
            
    }
    if(desc->ciphermode.data == NULL) return CC_FAILURE;
    return CC_SUCCESS;
}

int
cc_get_C_ciphermode(cc_cipher_select cipher, cc_mode_select mode, cc_direction direction, cc_ciphermode_descriptor desc) {
    desc->cipher = cipher;
    desc->mode = mode;
    desc->direction = direction;
    desc->ciphermode.data = NULL;
    return CC_UNIMPLEMENTED;
}

int
cc_symmetric_setup(cc_ciphermode_descriptor cm, const void *key, size_t keylen, const void *iv, cc_symmetric_context_p ctx) {
    switch(cm->mode) {
        case cc_ModeECB: cm->ciphermode.ecb->init(cm->ciphermode.ecb, ctx->ctx.ecb, keylen, key); break;
        case cc_ModeCBC: cm->ciphermode.cbc->init(cm->ciphermode.cbc, ctx->ctx.cbc, keylen, key); break;
        case cc_ModeCFB: cm->ciphermode.cfb->init(cm->ciphermode.cfb, ctx->ctx.cfb, keylen, key, iv); break;
        case cc_ModeCTR: cm->ciphermode.ctr->init(cm->ciphermode.ctr, ctx->ctx.ctr, keylen, key, iv); break;
        case cc_ModeOFB: cm->ciphermode.ofb->init(cm->ciphermode.ofb, ctx->ctx.ofb, keylen, key, iv); break;
        case cc_ModeCFB8: cm->ciphermode.cfb8->init(cm->ciphermode.cfb8, ctx->ctx.cfb8, keylen, key, iv); break;
        default: return CC_FAILURE;
    }
    return CC_SUCCESS;
}

int
cc_symmetric_setup_tweaked(cc_ciphermode_descriptor cm, const void *key, size_t keylen,
                           const void *tweak, CC_UNUSED const void *iv, cc_symmetric_context_p ctx) {
    switch(cm->mode) {
        case cc_ModeXTS: cm->ciphermode.xts->init(cm->ciphermode.xts, ctx->ctx.xts, keylen, key, tweak); break;
        default: return CC_FAILURE;
    }
    return CC_SUCCESS;
}

int
cc_symmetric_setup_authenticated(cc_ciphermode_descriptor cm, const void *key, size_t keylen,
                                 const void *iv, size_t iv_len,
                                 const void *adata, size_t adata_len,
                                 size_t data_len,
                                 size_t tag_len,
                                 cc_symmetric_context_p ctx) {
    switch(cm->mode) {
        case cc_ModeGCM:
            cm->ciphermode.gcm->init(cm->ciphermode.gcm, ctx->ctx.gcm, keylen, key);
            cm->ciphermode.gcm->set_iv(ctx->ctx.gcm, iv_len, iv);
            cm->ciphermode.gcm->gmac(ctx->ctx.gcm, adata_len, adata);
            break;
        case cc_ModeCCM:
            cm->ciphermode.ccm->init(cm->ciphermode.ccm, ctx->ctx.ccm, keylen, key);
            cm->ciphermode.ccm->set_iv(ctx->ctx.ccm, ctx->xtra_ctx.ccm_nonce, iv_len, iv, tag_len,adata_len, data_len);
            cm->ciphermode.ccm->cbcmac(ctx->ctx.ccm, ctx->xtra_ctx.ccm_nonce, adata_len, adata);
            break;
        default: return CC_FAILURE;
    }
    return CC_SUCCESS;
}


int
cc_symmetric_crypt(cc_symmetric_context_p ctx, const void *iv, const void *in, void *out, size_t len) {
    switch(ctx->mode_desc->mode) {
        case cc_ModeECB: ctx->mode_desc->ciphermode.ecb->ecb(ctx->ctx.ecb, len / ctx->mode_desc->ciphermode.ecb->block_size, in, out); break;
        case cc_ModeCBC: {
            cccbc_iv_decl(ctx->mode_desc->ciphermode.cbc->block_size, cbciv);
            cccbc_set_iv(ctx->mode_desc->ciphermode.cbc, cbciv, iv);
            ctx->mode_desc->ciphermode.cbc->cbc(ctx->ctx.cbc, cbciv, len / ctx->mode_desc->ciphermode.cbc->block_size, in, out);
        } break;
        case cc_ModeCFB: ctx->mode_desc->ciphermode.cfb->cfb(ctx->ctx.cfb, len / ctx->mode_desc->ciphermode.cfb->block_size, in, out); break;
        case cc_ModeCTR: ctx->mode_desc->ciphermode.ctr->ctr(ctx->ctx.ctr, len / ctx->mode_desc->ciphermode.ctr->block_size, in, out); break;
        case cc_ModeOFB: ctx->mode_desc->ciphermode.ofb->ofb(ctx->ctx.ofb, len / ctx->mode_desc->ciphermode.ofb->block_size, in, out); break;
        case cc_ModeCFB8: ctx->mode_desc->ciphermode.cfb8->cfb8(ctx->ctx.cfb8, len / ctx->mode_desc->ciphermode.cfb8->block_size, in, out); break;
        case cc_ModeXTS: {
            ccxts_tweak_decl(ctx->mode_desc->ciphermode.xts->tweak_size, xts_iv);
            ccxts_set_tweak(ctx->mode_desc->ciphermode.xts, ctx->ctx.xts, xts_iv, iv);
            ctx->mode_desc->ciphermode.xts->xts(ctx->ctx.xts, xts_iv, len / ctx->mode_desc->ciphermode.xts->block_size, in, out);
        } break;
        case cc_ModeGCM: ctx->mode_desc->ciphermode.gcm->gcm(ctx->ctx.gcm, len / ctx->mode_desc->ciphermode.gcm->block_size, in, out); break;
        case cc_ModeCCM: {
            ctx->mode_desc->ciphermode.ccm->ccm(ctx->ctx.ccm, ctx->xtra_ctx.ccm_nonce, len / ctx->mode_desc->ciphermode.ccm->block_size, in, out);

        } break;
        default: return CC_FAILURE;
    }
    return CC_SUCCESS;
}


void
cc_symmetric_final(CC_UNUSED cc_symmetric_context_p ctx) {
}

void
cc_symmetric_authenticated_finalize(cc_symmetric_context_p ctx, char *tag, size_t tag_len) {
    switch(ctx->mode_desc->mode) {
        case cc_ModeGCM: ctx->mode_desc->ciphermode.gcm->finalize(ctx->ctx.gcm, tag_len, tag);break;
        case cc_ModeCCM: ctx->mode_desc->ciphermode.ccm->finalize(ctx->ctx.ccm, ctx->xtra_ctx.ccm_nonce, tag);break;
        default:
            cc_assert(0);
    }
}



int
cc_symmetric_oneshot(cc_ciphermode_descriptor cm, const void *key, size_t keylen,
                     const void *iv, const void *in, void *out, size_t len) {
    switch(cm->mode) {
        case cc_ModeECB: ccecb_one_shot(cm->ciphermode.ecb, keylen, key, len / cm->ciphermode.ecb->block_size, in, out); break;
        case cc_ModeCBC: cccbc_one_shot(cm->ciphermode.cbc, keylen, key, iv, len / cm->ciphermode.cbc->block_size, in, out); break;
        case cc_ModeCFB: cccfb_one_shot(cm->ciphermode.cfb, keylen, key, iv, len / cm->ciphermode.cfb->block_size, in, out); break;
        case cc_ModeCTR: ccctr_one_shot(cm->ciphermode.ctr, keylen, key, iv, len / cm->ciphermode.ctr->block_size, in, out); break;
        case cc_ModeOFB: ccofb_one_shot(cm->ciphermode.ofb, keylen, key, iv, len / cm->ciphermode.ofb->block_size, in, out); break;
        case cc_ModeCFB8: cccfb8_one_shot(cm->ciphermode.cfb8, keylen, key, iv, len / cm->ciphermode.cfb8->block_size, in, out); break;
        default: return CC_FAILURE;
    }
    return CC_SUCCESS;
}

unsigned long
cc_symmetric_bloc_size(cc_ciphermode_descriptor cm) {
    
    unsigned long block_size=0;
    
    switch(cm->mode) {
        case cc_ModeECB: block_size=cm->ciphermode.ecb->block_size; break;
        case cc_ModeCBC: block_size=cm->ciphermode.cbc->block_size; break;
        case cc_ModeCFB: block_size=cm->ciphermode.cfb->block_size; break;
        case cc_ModeCTR: block_size=cm->ciphermode.ctr->block_size; break;
        case cc_ModeOFB: block_size=cm->ciphermode.ofb->block_size; break;
        case cc_ModeCFB8: block_size=cm->ciphermode.cfb8->block_size; break;
        default: return 0;
    }
    return block_size;
}
