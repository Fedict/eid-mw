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


/*
 * Parts of this code adapted from LibTomCrypt
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


#include <corecrypto/ccaes.h>
#include <corecrypto/cc_priv.h>
#include "ccaes_private_types.h"

#if	defined(__i386__) || defined(__x86_64__)

static int ccaes_aesni_init(const unsigned char *key, int keylen, int num_rounds,
                          ccecb_ctx *skey)
{
   	int i, j;
    ltc_rijndael_keysched *rijndael;

    rijndael = (ltc_rijndael_keysched *)skey;

    if (keylen != 16 && keylen != 24 && keylen != 32) {
        return -1; //CRYPT_INVALID_KEYSIZE;
    }

    if (num_rounds != 0 && num_rounds != (10 + ((keylen/8)-2)*2)) {
        return -1; //CRYPT_INVALID_ROUNDS;
    }

    rijndael->Nr = 10 + ((keylen/8)-2)*2;

	// encrypt key expansion

	if (keylen == 16) // aes-128
	{			
		__asm__ volatile(

			"movups		(%[key]), %%xmm1\n\t"
			"movups		%%xmm1, (%[ctx])\n\t"
			"aeskeygenassist	$1, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$2, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$4, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$8, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x10, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x20, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x40, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x80, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x1b, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x36, %%xmm1, %%xmm2\n\t"
			"call		1f\n\t"
			"jmp		2f\n\t"
			"1:\n\t"		// L_key_expansion_128
           
			"pshufd  $0xff, %%xmm2, %%xmm2\n\t"
			"movaps  %%xmm1, %%xmm3\n\t"
			"pslldq  $4, %%xmm3\n\t"
			"pxor    %%xmm3, %%xmm1\n\t"
			"movaps  %%xmm1, %%xmm3\n\t"
			"pslldq  $4, %%xmm3\n\t"
			"pxor    %%xmm3, %%xmm1\n\t"
			"movaps  %%xmm1, %%xmm3\n\t"
			"pslldq  $4, %%xmm3\n\t"
			"pxor    %%xmm3, %%xmm1\n\t"
			"pxor    %%xmm2, %%xmm1\n\t"
			"add     $16, %[ctx]\n\t"
			"movups  %%xmm1, (%[ctx])\n\t"
			"ret\n\t"
			"2:\n\t"
		:
		: [key] "r" (key), [ctx] "r" (rijndael->eK)
		: "xmm1", "xmm2", "xmm3"
		);

	} 
	else if (keylen == 24) // aes-192
	{		

		__asm__ volatile(
			"movups		(%[key]), %%xmm1\n\t"
			"movq		16(%[key]), %%xmm3\n\t"
			"movups		%%xmm1, (%[ctx])\n\t"
			"movq		%%xmm3, 16(%[ctx])\n\t"
			"aeskeygenassist	$1, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$2, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$4, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$8, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x10, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x20, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x40, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x80, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"jmp		2f\n\t"

			"1:\n\t"			// L_key_expansion_192

			"pshufd  $0x55, %%xmm2, %%xmm2\n\t"
			"movaps  %%xmm1, %%xmm4\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pxor    %%xmm2, %%xmm1\n\t"

			"pshufd  $0xff, %%xmm1, %%xmm2\n\t"
			"movaps  %%xmm3, %%xmm4\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm3\n\t"
			"pxor    %%xmm2, %%xmm3\n\t"

			"add     $24, %[ctx]\n\t"
			"movups  %%xmm1, (%[ctx])\n\t"
			"movq	 %%xmm3, 16(%[ctx])\n\t"
			"ret\n\t"
			"2:\n\t"
		:
		: [key] "r" (key), [ctx] "r" (rijndael->eK)
		: "xmm1", "xmm2", "xmm3", "xmm4"
		);

	} 
	else if (keylen == 32)  // aes-256
	{

		__asm__ volatile(
			"movups		(%[key]), %%xmm1\n\t"
			"movups		16(%[key]), %%xmm3\n\t"
			"movups		%%xmm1, (%[ctx])\n\t"
			"movups		%%xmm3, 16(%[ctx])\n\t"
			"aeskeygenassist	$1, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$2, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$4, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$8, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x10, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x20, %%xmm3, %%xmm2\n\t"
			"call		1f\n\t"
			"aeskeygenassist	$0x40, %%xmm3, %%xmm2\n\t"
			"jmp		2f\n\t"

			"1:\n\t"			// L_key_expansion_256

			"pshufd  $0xff, %%xmm2, %%xmm2\n\t"
			"movaps  %%xmm1, %%xmm4\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pxor    %%xmm2, %%xmm1\n\t"
			"add     $32, %[ctx]\n\t"
			"movups  %%xmm1, (%[ctx])\n\t"

			"aeskeygenassist $0, %%xmm1, %%xmm4\n\t"
		

			"pshufd  $0xaa, %%xmm4, %%xmm2\n\t"
			"movaps  %%xmm3, %%xmm4\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm3\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm3\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm3\n\t"
			"pxor    %%xmm2, %%xmm3\n\t"
			"movups  %%xmm3, 16(%[ctx])\n\t"
			"ret\n\t"

			"2:\n\t"			// L_key_expansion_256_final
			"pshufd  $0xff, %%xmm2, %%xmm2\n\t"
			"movaps  %%xmm1, %%xmm4\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pslldq  $4, %%xmm4\n\t"
			"pxor    %%xmm4, %%xmm1\n\t"
			"pxor    %%xmm2, %%xmm1\n\t"
			"movups  %%xmm1, 32(%[ctx])\n\t"
		:
		: [key] "r" (key), [ctx] "r" (rijndael->eK)
		: "xmm1", "xmm2", "xmm3", "xmm4"
		);


	} 
	else 
	{
    	/* this can't happen */
    	return -1; //CRYPT_ERROR;
	}

#ifndef ENCRYPT_ONLY
	if (keylen == 16) { i = 9; j = 40;}
	else if (keylen == 24) { i = 11; j = 48;}
	else { i = 13; j = 56;}

        __asm__ volatile(
		"movups	(%[ekey]), %%xmm0\n\t"
		"movups	%%xmm0, (%[dkey])\n\t"
		"0:\n\t"
		"sub	$16, %[ekey]\n\t"
		"add	$16, %[dkey]\n\t"
		"movups	(%[ekey]), %%xmm0\n\t"
		"aesimc	%%xmm0, %%xmm0\n\t"
		"movups	%%xmm0, (%[dkey])\n\t"
		"dec	%[Ni]\n\t"
		"jg		0b\n\t"
		"movups	-16(%[ekey]), %%xmm0\n\t"
		"movups	%%xmm0, 16(%[dkey])\n\t"
		:
        : [dkey] "r" (rijndael->dK), [ekey] "r" (&rijndael->eK[j]), [Ni] "r" (i)
        : "xmm0"
        );

#endif

	return 0; //CRYPT_OK;
}

static void ccaes_ecb_encrypt_aesni_init(const struct ccmode_ecb *ecb CC_UNUSED, ccecb_ctx *key,
                                   unsigned long rawkey_len, const void *rawkey) {
    ccaes_aesni_init(rawkey, (int)rawkey_len, 0, key);
}

static void ccaes_aesni_ecb_encrypt(const ccecb_ctx *skey, const unsigned char *pt,
                                  unsigned char *ct)
{
	
	const uint32_t *rk;
    int Nr;
    const ltc_rijndael_keysched *rijndael;

    rijndael = (const ltc_rijndael_keysched *)skey;

    Nr = rijndael->Nr;
    rk = rijndael->dK;
   	if (Nr == 10) 
	{
        __asm__ volatile(
		"movups	(%[pt]), %%xmm0\n\t"
		"movups	(%[key]), %%xmm1\n\t"

		"pxor	%%xmm1, %%xmm0\n\t"		

		"movups	16(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	32(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	48(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	64(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	80(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	96(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	112(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	128(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	144(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	160(%[key]), %%xmm1\n\t"
		"aesenclast	%%xmm1, %%xmm0\n\t"

		"movups	%%xmm0, (%[ct])\n\t"

		:
        : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK)
        : "xmm0", "xmm1"
		);


	} 
	else if (Nr == 12) 
	{

        __asm__ volatile(
		"movups	(%[pt]), %%xmm0\n\t"
		"movups	(%[key]), %%xmm1\n\t"

		"pxor	%%xmm1, %%xmm0\n\t"		

		"movups	16(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	32(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	48(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	64(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	80(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	96(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	112(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	128(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	144(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	160(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	176(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	192(%[key]), %%xmm1\n\t"
		"aesenclast	%%xmm1, %%xmm0\n\t"

        "movups	%%xmm0, (%[ct])\n\t"

		:
        : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK)
        : "xmm0", "xmm1"
		);


	} 
	else 
	{

        __asm__ volatile(
		"movups	(%[pt]), %%xmm0\n\t"
		"movups	(%[key]), %%xmm1\n\t"

		"pxor	%%xmm1, %%xmm0\n\t"		

		"movups	16(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	32(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	48(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	64(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	80(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	96(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	112(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	128(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	144(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	160(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	176(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	192(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	208(%[key]), %%xmm1\n\t"
		"aesenc	%%xmm1, %%xmm0\n\t"

		"movups	224(%[key]), %%xmm1\n\t"
		"aesenclast	%%xmm1, %%xmm0\n\t"

        "movups	%%xmm0, (%[ct])\n\t"

		:
        : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK)
        : "xmm0", "xmm1"
		);


	}

	return;
}
    
static void ccaes_aesni_ecb_encrypt_4x_blocks(const ccecb_ctx *skey, const unsigned char *pt, unsigned char *ct, uint32_t n4blocks) {
    int Nr;
    const ltc_rijndael_keysched *rijndael;
    rijndael = (const ltc_rijndael_keysched *)skey;
    Nr = rijndael->Nr;
	if (Nr == 10) {

            __asm__ volatile(

			"movups	(%[key]), %%xmm4\n\t"
			"movups	16(%[key]), %%xmm5\n\t"

			"0:\n\t"

			"movups	(%[pt]), %%xmm0\n\t"
			"movups	16(%[pt]), %%xmm1\n\t"
			"movups	32(%[pt]), %%xmm2\n\t"
			"movups	48(%[pt]), %%xmm3\n\t"

			"pxor	%%xmm4, %%xmm0\n\t"		
			"pxor	%%xmm4, %%xmm1\n\t"		
			"pxor	%%xmm4, %%xmm2\n\t"		
			"pxor	%%xmm4, %%xmm3\n\t"		
			"movups	32(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	48(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	64(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	80(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	96(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	112(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	128(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	144(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	160(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	16(%[key]), %%xmm5\n\t"

			"aesenclast	%%xmm4, %%xmm0\n\t"		
			"aesenclast	%%xmm4, %%xmm1\n\t"		
			"aesenclast	%%xmm4, %%xmm2\n\t"		
			"aesenclast	%%xmm4, %%xmm3\n\t"		
			"movups	0(%[key]), %%xmm4\n\t"

			"movups	%%xmm0, (%[ct])\n\t"
			"movups	%%xmm1, 16(%[ct])\n\t"
			"movups	%%xmm2, 32(%[ct])\n\t"
			"movups	%%xmm3, 48(%[ct])\n\t"

			"add	$64, %[ct]\n\t"
			"add	$64, %[pt]\n\t"
			"dec	%[Nb]\n\t"
			"jg		0b\n\t"

			:
            : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK), [Nb] "r" (n4blocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
			);

	} else if (Nr == 12) {

            __asm__ volatile(

			"movups	(%[key]), %%xmm4\n\t"
			"movups	16(%[key]), %%xmm5\n\t"

			"0:\n\t"

			"movups	(%[pt]), %%xmm0\n\t"
			"movups	16(%[pt]), %%xmm1\n\t"
			"movups	32(%[pt]), %%xmm2\n\t"
			"movups	48(%[pt]), %%xmm3\n\t"

			"pxor	%%xmm4, %%xmm0\n\t"		
			"pxor	%%xmm4, %%xmm1\n\t"		
			"pxor	%%xmm4, %%xmm2\n\t"		
			"pxor	%%xmm4, %%xmm3\n\t"		
			"movups	32(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	48(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	64(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	80(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	96(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	112(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	128(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	144(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	160(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	176(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	192(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	16(%[key]), %%xmm5\n\t"

			"aesenclast	%%xmm4, %%xmm0\n\t"		
			"aesenclast	%%xmm4, %%xmm1\n\t"		
			"aesenclast	%%xmm4, %%xmm2\n\t"		
			"aesenclast	%%xmm4, %%xmm3\n\t"		
			"movups	0(%[key]), %%xmm4\n\t"

			"movups	%%xmm0, (%[ct])\n\t"
			"movups	%%xmm1, 16(%[ct])\n\t"
			"movups	%%xmm2, 32(%[ct])\n\t"
			"movups	%%xmm3, 48(%[ct])\n\t"

			"add	$64, %[ct]\n\t"
			"add	$64, %[pt]\n\t"
			"dec	%[Nb]\n\t"
			"jg		0b\n\t"

			:
            : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK), [Nb] "r" (n4blocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
			);

	} else {

            __asm__ volatile(

			"movups	(%[key]), %%xmm4\n\t"
			"movups	16(%[key]), %%xmm5\n\t"

			"0:\n\t"

			"movups	(%[pt]), %%xmm0\n\t"
			"movups	16(%[pt]), %%xmm1\n\t"
			"movups	32(%[pt]), %%xmm2\n\t"
			"movups	48(%[pt]), %%xmm3\n\t"

			"pxor	%%xmm4, %%xmm0\n\t"		
			"pxor	%%xmm4, %%xmm1\n\t"		
			"pxor	%%xmm4, %%xmm2\n\t"		
			"pxor	%%xmm4, %%xmm3\n\t"		
			"movups	32(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	48(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	64(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	80(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	96(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	112(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	128(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	144(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	160(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	176(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	192(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	208(%[key]), %%xmm5\n\t"

			"aesenc	%%xmm4, %%xmm0\n\t"		
			"aesenc	%%xmm4, %%xmm1\n\t"		
			"aesenc	%%xmm4, %%xmm2\n\t"		
			"aesenc	%%xmm4, %%xmm3\n\t"		
			"movups	224(%[key]), %%xmm4\n\t"

			"aesenc	%%xmm5, %%xmm0\n\t"		
			"aesenc	%%xmm5, %%xmm1\n\t"		
			"aesenc	%%xmm5, %%xmm2\n\t"		
			"aesenc	%%xmm5, %%xmm3\n\t"		
			"movups	16(%[key]), %%xmm5\n\t"

			"aesenclast	%%xmm4, %%xmm0\n\t"		
			"aesenclast	%%xmm4, %%xmm1\n\t"		
			"aesenclast	%%xmm4, %%xmm2\n\t"		
			"aesenclast	%%xmm4, %%xmm3\n\t"		
			"movups	0(%[key]), %%xmm4\n\t"

			"movups	%%xmm0, (%[ct])\n\t"
			"movups	%%xmm1, 16(%[ct])\n\t"
			"movups	%%xmm2, 32(%[ct])\n\t"
			"movups	%%xmm3, 48(%[ct])\n\t"

			"add	$64, %[ct]\n\t"
			"add	$64, %[pt]\n\t"
			"dec	%[Nb]\n\t"
			"jg		0b\n\t"

			:
            : [ct] "r" (ct), [pt] "r" (pt), [key] "r" (rijndael->eK), [Nb] "r" (n4blocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
			);
	}
}

static void ccaes_ecb_aesni_encrypt(const ccecb_ctx *key, unsigned long nblocks,
                              const void *in, void *out) 
{
    if (nblocks) 
	{
        const unsigned char *p = in;
        unsigned char *c = out;

   	 	if ((nblocks>=4))  // if aesni available and more than 4 blocks to decrypt
		{               
			uint32_t        n4blocks = nblocks>>2;
			ccaes_aesni_ecb_encrypt_4x_blocks(key, p, c, n4blocks);   // do 4 blocks decrypt in an iteration
			n4blocks <<= 2;
			nblocks -= n4blocks;
			n4blocks *= CCAES_BLOCK_SIZE;
			p += n4blocks;
			c += n4blocks;
	    }
	    if (nblocks<=0) return;

	    for (;;) 
		{
			ccaes_aesni_ecb_encrypt(key, p, c);
			if (--nblocks) 
			{
				p += CCAES_BLOCK_SIZE;
				c += CCAES_BLOCK_SIZE;
			} 
			else 
			{
				break;
			}
	    }
	}
}
const struct ccmode_ecb ccaes_aesni_ecb_encrypt_mode = {
    .size = sizeof(ltc_rijndael_keysched),
    .block_size = CCAES_BLOCK_SIZE,
    .init = ccaes_ecb_encrypt_aesni_init,
    .ecb = ccaes_ecb_aesni_encrypt,
};

#endif // defined(__i386__) || defined(__x86_64__)

