/*
 * Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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

#if !defined(__NO_ASM__) && CCAES_INTEL

#ifndef VNG_AES_PRIV_H_
#define VNG_AES_PRIV_H_
#if defined(__cplusplus)
extern "C"
{
#endif
    
#include "vng_aes.h"
// Assembly level interfaces for basic AES.

#define kUseAESNI 1
#define kUseOPT 0

//int 
//vng_aes_encrypt_key(const unsigned char *key, int key_len, vng_aes_encrypt_ctx cx[1]);

int
vng_aes_encrypt_opt_key(const unsigned char *key, int key_len, vng_aes_encrypt_ctx cx[1]);

int
vng_aes_encrypt_aesni_key(const unsigned char *key, int key_len, vng_aes_encrypt_ctx cx[1]);



//int 
//vng_aes_decrypt_key(const unsigned char *key, int key_len, vng_aes_decrypt_ctx cx[1]);

int
vng_aes_decrypt_opt_key(const unsigned char *key, int key_len, vng_aes_decrypt_ctx cx[1]);

int
vng_aes_decrypt_aesni_key(const unsigned char *key, int key_len, vng_aes_decrypt_ctx cx[1]);



//int
//vng_aes_encrypt(const unsigned char *Plaintext, unsigned char *Ciphertext, vng_aes_encrypt_ctx *ctx);

int 
vng_aes_encrypt_opt(const unsigned char *Plaintext, unsigned char *Ciphertext, const vng_aes_encrypt_ctx *ctx);

int 
vng_aes_encrypt_aesni(const unsigned char *Plaintext, unsigned char *Ciphertext, const vng_aes_encrypt_ctx *ctx);


//int
//vng_aes_decrypt(const unsigned char *Ciphertext, unsigned char *Plaintext, vng_aes_decrypt_ctx *ctx);

int
vng_aes_decrypt_opt(const unsigned char *Ciphertext, unsigned char *Plaintext, const vng_aes_decrypt_ctx *ctx);

int 
vng_aes_decrypt_aesni(const unsigned char *Ciphertext, unsigned char *Plaintext, const vng_aes_decrypt_ctx *ctx);


    
//extern int vng_aes_encrypt_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, vng_aes_encrypt_ctx *encrypt);
extern int vng_aes_encrypt_opt_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, const vng_aes_encrypt_ctx *encrypt);
extern int vng_aes_encrypt_aesni_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, const vng_aes_encrypt_ctx *encrypt);

//extern int vng_aes_decrypt_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, vng_aes_decrypt_ctx *decript);
extern int vng_aes_decrypt_opt_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, const vng_aes_decrypt_ctx *decript);
extern int vng_aes_decrypt_aesni_cbc(const unsigned char *ibuf, const unsigned char *in_iv, unsigned int num_blk, unsigned char *obuf, const vng_aes_decrypt_ctx *decript);

// Prototypes for assembly functions used in optimized AES-XTS for x86.

extern void aesxts_mult_x(uint8_t *I);
extern int aesxts_tweak_crypt_opt(const uint8_t *P, uint8_t *C, const uint8_t *T, const vng_aes_encrypt_ctx *ctx);
extern int aesxts_tweak_crypt_aesni(const uint8_t *P, uint8_t *C, const uint8_t *T, const vng_aes_encrypt_ctx *ctx);

extern int aesxts_tweak_crypt_group_aesni(const uint8_t *P, uint8_t *C, const uint8_t *T, const vng_aes_encrypt_ctx *ctx, uint32_t lim);
extern int aesxts_tweak_crypt_group_opt(const uint8_t *P, uint8_t *C, const uint8_t *T, const vng_aes_encrypt_ctx *ctx, uint32_t lim);

extern int aesxts_tweak_uncrypt_opt(const uint8_t *C, uint8_t *P, const uint8_t *T, const vng_aes_decrypt_ctx *ctx);
extern int aesxts_tweak_uncrypt_aesni(const uint8_t *C, uint8_t *P, const uint8_t *T, const vng_aes_decrypt_ctx *ctx);


extern int aesxts_tweak_uncrypt_group_aesni(const uint8_t *C, uint8_t *P, const uint8_t *T, const vng_aes_decrypt_ctx *ctx, uint32_t lim);
extern int aesxts_tweak_uncrypt_group_opt(const uint8_t *C, uint8_t *P, const uint8_t *T, const vng_aes_decrypt_ctx *ctx, uint32_t lim);


#if defined(__cplusplus)
}
#endif

#endif /* VNG_AES_PRIV_H_ */
#endif /* __NO_ASM__ */
