/*
 * Copyright (c) 2012,2013,2015 Apple Inc. All rights reserved.
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


/*
 * Parts of this code adapted from LibTomCrypt vng_aes.c
 *
 * LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

#include <corecrypto/cc_config.h>

#if !defined(__NO_ASM__) && CCAES_INTEL
#if (defined (__i386__) || defined (__x86_64__))

#include <string.h>
#include "vng_aesPriv.h"

/*! XTS Encryption
 @param pt     [in]  Plaintext
 @param ptlen  Length of plaintext (and ciphertext)
 @param ct     [out] Ciphertext
 @param tweak  [in] The 128--bit encryption tweak (e.g. sector number)
 @param xts    The XTS structure
 @param useAESNI [in] If 1 then use the AESNI instructions.
 Returns CRYPT_OK upon success
 */

int 
vng_aes_xts_encrypt_opt(
                   const uint8_t *pt, unsigned long ptlen,
                   uint8_t *ct,
                   const uint8_t *T,
                   const void *ctx)
{
    const vng_aes_encrypt_ctx *xts = ctx;
    uint8_t PP[16], CC[16];
    uint64_t i, m, mo, lim;
    int   err;
    
    /* get number of blocks */
    m  = ptlen >> 4;
    mo = ptlen & 15;
    
    /* must have at least one full block */
    if (m == 0) {
        return CRYPT_INVALID_ARG;
    }
    
    /* for i = 0 to m-2 do */
    if (mo == 0) {
        lim = m;
    } else {
        lim = m - 1;
    }
    
    
    if (lim>0) {
        err = aesxts_tweak_crypt_group_opt(pt, ct, T, xts, (uint32_t)lim);
        ct += (lim<<4);
        pt += (lim<<4);
    }
    
    /* if ptlen not divide 16 then */
    if (mo > 0) {
        /* CC = tweak encrypt block m-1 */
        if ((err = aesxts_tweak_crypt_opt(pt, CC, T, xts)) != 0) {
            return err;
        }
        
        /* Cm = first ptlen % 16 bytes of CC */
        for (i = 0; i < mo; i++) {
            PP[i] = pt[16+i];
            ct[16+i] = CC[i];
        }
        
        for (; i < 16; i++) {
            PP[i] = CC[i];
        }
        
        /* Cm-1 = Tweak encrypt PP */
        if ((err = aesxts_tweak_crypt_opt(PP, ct, T, xts)) != 0) {
            return err;
        }
    }
    
    return err;
}

int 
vng_aes_xts_encrypt_aesni(
                   const uint8_t *pt, unsigned long ptlen,
                   uint8_t *ct,
                   const uint8_t *T,
                   const void *ctx)
{
    const vng_aes_encrypt_ctx *xts = ctx;
    uint8_t PP[16], CC[16];
    uint64_t i, m, mo, lim;
    int   err;
    
    /* get number of blocks */
    m  = ptlen >> 4;
    mo = ptlen & 15;
    
    /* must have at least one full block */
    if (m == 0) {
        return CRYPT_INVALID_ARG;
    }
    
    /* for i = 0 to m-2 do */
    if (mo == 0) {
        lim = m;
    } else {
        lim = m - 1;
    }
    
    
    if (lim>0) {
        err = aesxts_tweak_crypt_group_aesni(pt, ct, T, xts, (uint32_t)lim);
        ct += (lim<<4);
        pt += (lim<<4);
    }
    
    /* if ptlen not divide 16 then */
    if (mo > 0) {
        /* CC = tweak encrypt block m-1 */
        if ((err = aesxts_tweak_crypt_aesni(pt, CC, T, xts)) != 0) {
            return err;
        }
        
        /* Cm = first ptlen % 16 bytes of CC */
        for (i = 0; i < mo; i++) {
            PP[i] = pt[16+i];
            ct[16+i] = CC[i];
        }
        
        for (; i < 16; i++) {
            PP[i] = CC[i];
        }
        
        /* Cm-1 = Tweak encrypt PP */
        if ((err = aesxts_tweak_crypt_aesni(PP, ct, T, xts)) != 0) {
            return err;
        }
    }
    
    return err;
}


/*! XTS Decryption
 @param ct     [in] Ciphertext
 @param ptlen  Length of plaintext (and ciphertext)
 @param pt     [out]  Plaintext
 @param tweak  [in] The 128--bit encryption tweak (e.g. sector number)
 @param xts    The XTS structure
 @param useAESNI [in] If 1 then use the AESNI instructions.
 Returns CRYPT_OK upon success
 */

int 
vng_aes_xts_decrypt_opt(
                   const uint8_t *ct, unsigned long ptlen,
                   uint8_t *pt,
                   const uint8_t *T,
                   const void *ctx)
{

    const vng_aes_decrypt_ctx *decrypt_ctx = ctx;
    uint8_t PP[16], CC[16];
    uint64_t i, m, mo, lim;
    int err;
    
    /* check inputs */
    if((pt == NULL) || (ct == NULL)|| (decrypt_ctx == NULL)) return 1;
    
    /* get number of blocks */
    m  = ptlen >> 4;
    mo = ptlen & 15;
    
    /* must have at least one full block */
    if (m == 0) {
        return CRYPT_INVALID_ARG;
    }

    
    /* for i = 0 to m-2 do */
    if (mo == 0) {
        lim = m;
    } else {
        lim = m - 1;
    }
    
    if (lim>0) {
        if ((err = aesxts_tweak_uncrypt_group_opt(ct, pt, T, decrypt_ctx,(uint32_t)lim)) != CRYPT_OK) {
            return err;
        }
        ct += (lim<<4);
        pt += (lim<<4);
    }

    /* if ptlen not divide 16 then */
    if (mo > 0) {
        memcpy(CC, T, 16);
        aesxts_mult_x(CC);
        
        /* PP = tweak decrypt block m-1 */
        if ((err = aesxts_tweak_uncrypt_opt(ct, PP, CC, decrypt_ctx)) != CRYPT_OK) {
            return err;
        }
        
        /* Pm = first ptlen % 16 bytes of PP */
        for (i = 0; i < mo; i++) {
            CC[i]    = ct[16+i];
            pt[16+i] = PP[i];
        }
        for (; i < 16; i++) {
            CC[i] = PP[i];
        }
        
        /* Pm-1 = Tweak uncrypt CC */
        if ((err = aesxts_tweak_uncrypt_opt(CC, pt, T, decrypt_ctx)) != CRYPT_OK) {
            return err;
        }
    }
    
    return CRYPT_OK;
}


int 
vng_aes_xts_decrypt_aesni(
                   const uint8_t *ct, unsigned long ptlen,
                   uint8_t *pt,
                   const uint8_t *T,
                   const void *ctx)
{

    const vng_aes_decrypt_ctx *decrypt_ctx = ctx;
    uint8_t PP[16], CC[16];
    uint64_t i, m, mo, lim;
    int err;
    
    /* check inputs */
    if((pt == NULL) || (ct == NULL)|| (decrypt_ctx == NULL)) return 1;
    
    /* get number of blocks */
    m  = ptlen >> 4;
    mo = ptlen & 15;
    
    /* must have at least one full block */
    if (m == 0) {
        return CRYPT_INVALID_ARG;
    }

    
    /* for i = 0 to m-2 do */
    if (mo == 0) {
        lim = m;
    } else {
        lim = m - 1;
    }
    
    if (lim>0) {
        if ((err = aesxts_tweak_uncrypt_group_aesni(ct, pt, T, decrypt_ctx,(uint32_t)lim)) != CRYPT_OK) {
            return err;
        }
        ct += (lim<<4);
        pt += (lim<<4);
    }
    
    /* if ptlen not divide 16 then */
    if (mo > 0) {
        memcpy(CC, T, 16);
        aesxts_mult_x(CC);
        
        /* PP = tweak decrypt block m-1 */
        if ((err = aesxts_tweak_uncrypt_aesni(ct, PP, CC, decrypt_ctx)) != CRYPT_OK) {
            return err;
        }
        
        /* Pm = first ptlen % 16 bytes of PP */
        for (i = 0; i < mo; i++) {
            CC[i]    = ct[16+i];
            pt[16+i] = PP[i];
        }
        for (; i < 16; i++) {
            CC[i] = PP[i];
        }
        
        /* Pm-1 = Tweak uncrypt CC */
        if ((err = aesxts_tweak_uncrypt_aesni(CC, pt, T, decrypt_ctx)) != CRYPT_OK) {
            return err;
        }
    }
    
    return CRYPT_OK;
}

#endif /* X86 */

#endif /* __NO_ASM__ */
