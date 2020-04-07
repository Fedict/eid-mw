# 
#  Copyright (c) 2015 Apple Inc. All rights reserved.
#  
#  corecrypto Internal Use License Agreement
#  
#  IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
#  in consideration of your agreement to the following terms, and your download or use
#  of this Apple software constitutes acceptance of these terms.  If you do not agree
#  with these terms, please do not download or use this Apple software.
#  
#  1.	As used in this Agreement, the term "Apple Software" collectively means and
#  includes all of the Apple corecrypto materials provided by Apple here, including
#  but not limited to the Apple corecrypto software, frameworks, libraries, documentation
#  and other Apple-created materials. In consideration of your agreement to abide by the
#  following terms, conditioned upon your compliance with these terms and subject to
#  these terms, Apple grants you, for a period of ninety (90) days from the date you
#  download the Apple Software, a limited, non-exclusive, non-sublicensable license
#  under Apple’s copyrights in the Apple Software to make a reasonable number of copies
#  of, compile, and run the Apple Software internally within your organization only on
#  devices and computers you own or control, for the sole purpose of verifying the
#  security characteristics and correct functioning of the Apple Software; provided
#  that you must retain this notice and the following text and disclaimers in all
#  copies of the Apple Software that you make. You may not, directly or indirectly,
#  redistribute the Apple Software or any portions thereof. The Apple Software is only
#  licensed and intended for use as expressly stated above and may not be used for other
#  purposes or in other contexts without Apple's prior written permission.  Except as
#  expressly stated in this notice, no other rights or licenses, express or implied, are
#  granted by Apple herein.
#  
#  2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
#  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
#  OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
#  THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
#  SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
#  REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
#  ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
#  SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
#  OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
#  WILL CREATE A WARRANTY. 
#  
#  3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
#  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
#  IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
#  SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
#  NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#  
#  4.	This Agreement is effective until terminated. Your rights under this Agreement will
#  terminate automatically without notice from Apple if you fail to comply with any term(s)
#  of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
#  and destroy all copies, full or partial, of the Apple Software. This Agreement will be
#  governed and construed in accordance with the laws of the State of California, without
#  regard to its choice of law rules.
#  
#  You may report security issues about Apple products to product-security@apple.com,
#  as described here:  https://www.apple.com/support/security/.  Non-security bugs and
#  enhancement requests can be made via https://bugreport.apple.com as described
#  here: https://developer.apple.com/bug-reporting/
#  
#  EA1350
#  10/5/15
# 

#ifndef __NO_ASM__

#include <corecrypto/cc_config.h>

#if CCAES_ARM && defined(__arm64__)
#include "ccarm_intrinsic_compatability.h"

/*
	aes_rqal aes_encrypt_cfb(const aes_encrypt_ctx *ctx, __m128 *iv, int num_blk, const __m128 *ibuf, __m128 *obuf)
    {

		while (num_blk--) {
              aes_encrypt(iv, iv, ctx);
              *iv ^= *ibuf++;
              *obuf++ = *iv;
	}

        return 0;
    }
*/

	#define	ctx		x0
	#define	iv		x1
	#define	num_blk	x2
	#define	ibuf	x3
	#define	obuf	x4
	#define	keylen	x5
	#define	keylenw	w5
	#define	t		x6

	.text


	.align	4
	.globl	_ccaes_cfb_encrypt_vng_vector

_ccaes_cfb_encrypt_vng_vector:

	// early exit if input number of blocks is zero
	cbnz		num_blk, 1f
	ret			lr
1:

	ldr			keylenw, [ctx, #240]

	cmp     	keylenw, #160
    b.eq   		2f
    cmp     	keylenw, #192
    b.eq   		2f
    cmp     	keylenw, #224
    b.eq     	2f

	mov     	x0, #-1     // Return error.
	ret			lr

2:

#if CC_KERNEL
    // save used vector registers
    sub     sp, sp, #4*16
    st1.4s      {v0,v1,v2,v3}, [sp]
#endif

	ldr			q0, [iv]				// initial *iv

L_scalar:

    ld1.4s      {v2}, [ctx], #16    
	sub			t, keylen, #16

	// aes_encrypt(iv, iv, ctx);
0:
	AESE		0, 2					// 	xor/SubByte/ShiftRows
	AESMC		0, 0					// MixColumns
    ld1.4s      {v2}, [ctx], #16    
	subs		t, t, #16
	b.gt		0b
	ldr			q3, [ctx]				// expanded key[0]
	sub			ctx, ctx, keylen
    ld1.4s      {v1}, [ibuf], #16
	AESE		0, 2					// 	xor/SubByte/ShiftRows
	eor.16b		v0, v0, v3				// v0 now is the final *iv
	eor.16b		v0, v0, v1				// *iv ^= *ibuf++;
	st1.4s      {v0}, [obuf], #16
	subs		num_blk, num_blk, #1
	b.gt		L_scalar

L_done:
	mov			x0, #0
	str			q0, [iv]
#if CC_KERNEL
    // restore used vector registers
    st1.4s      {v0,v1,v2,v3}, [sp], #4*16
#endif
	ret			lr

#endif
#endif /* __NO_ASM__ */
