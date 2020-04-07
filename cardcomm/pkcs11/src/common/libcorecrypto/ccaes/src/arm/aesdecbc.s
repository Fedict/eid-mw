# 
#  Copyright (c) 2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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

#if defined(__arm64__)
#include "ccarm_intrinsic_compatability.h"
	// decrypt C code
/*
	aes_rqal aes_decrypt_cbc(const __m128 *ibuf, __m128 *iq, int num_blk, __m128 *obuf, const aes_decrypt_ctx *ctx)
    {
        while(num_blk--) {
            aes_decrypt(ibuf, obuf, ctx);
            *obuf++ ^= *iq;
            *iq = *ibuf++;
        }
        return 0;
    }
*/

	#define	ctx		x0
	#define	iq		x1
	#define	num_blk	x2
	#define	ibuf	x3
	#define	obuf	x4
	#define	keylen	x5
	#define	keylenw	w5
	#define	t		x6

	.text
	.align	4
	.globl	_ccaes_arm_decrypt_cbc

_ccaes_arm_decrypt_cbc:

	// early exit if input number of blocks is zero
	cbnz		num_blk, 1f
	ret			lr
1:


	ldr			keylenw, [ctx, #240]

	cmp     	keylen, #160
    b.eq   		2f
    cmp     	keylen, #192
    b.eq   		2f
    cmp     	keylen, #224
    b.eq     	2f

	mov     	x0, #-1     // Return error.
	ret			lr

2:
#if CC_KERNEL
    // save used vector registers
	sub		x6, sp, #11*16
    sub     sp, sp, #11*16
    st1.4s      {v0,v1,v2,v3}, [x6], #4*16
    st1.4s      {v4,v5,v6,v7}, [x6], #4*16
    st1.4s      {v16,v17,v18}, [x6], #3*16
#endif


	mov			t, keylen
	ldr			q4, [iq]				// a copy of *iq
	subs		num_blk, num_blk, #4
	b.lt		L_lessthan_4


L_loop:			// per 4 blocks


	ldr			q16, [ctx, t]		// expanded key[10]
	sub			t, t, #16

	ld1.4s		{v0,v1,v2,v3}, [ibuf], #4*16

	orr.16b 	v5, v0, v0
	orr.16b 	v6, v1, v1
	orr.16b 	v7, v2, v2
	orr.16b 	v18, v3, v3

0:
	AESD		0, 16					// 	xor/SubByte/ShiftRows
	AESIMC      0, 0					// MixColumns
	AESD		1, 16					// 	xor/SubByte/ShiftRows
	AESIMC      1, 1					// MixColumns
	AESD		2, 16					// 	xor/SubByte/ShiftRows
	AESIMC      2, 2					// MixColumns
	AESD		3, 16					// 	xor/SubByte/ShiftRows
	AESIMC      3, 3					// MixColumns
	ldr			q16, [ctx, t]			// expanded key[t]
	subs		t, t, #16
	b.gt		0b

	mov			t, keylen

	ldr			q17, [ctx]				// expanded key[0]
	AESD		0, 16					// 	xor/SubByte/ShiftRows
	eor.16b		v0, v0, v17				// out = state ^ key[0];
	AESD		1, 16					// 	xor/SubByte/ShiftRows
	eor.16b		v1, v1, v17				// out = state ^ key[0];
	AESD		2, 16					// 	xor/SubByte/ShiftRows
	eor.16b		v2, v2, v17				// out = state ^ key[0];
	AESD		3, 16					// 	xor/SubByte/ShiftRows
	eor.16b		v3, v3, v17				// out = state ^ key[0];

	eor.16b		v0, v0, v4
	eor.16b		v1, v1, v5
	eor.16b		v2, v2, v6
	eor.16b		v3, v3, v7

	orr.16b		v4, v18, v18

	st1.4s		{v0,v1,v2,v3}, [obuf], #4*16
	subs		num_blk, num_blk, #4
	b.ge		L_loop

L_lessthan_4:

	adds		num_blk, num_blk, #4
	b.le		L_done

L_scalar:

	ldr			q16, [ctx, t]		// expanded key[10]
	sub			t, t, #16

	ldr			q0, [ibuf], #16			// state = in
	orr.16b 	v18, v0, v0				// next iv

0:
	AESD		0, 16					// 	xor/SubByte/ShiftRows
	AESIMC		0, 0					// MixColumns
	ldr			q16, [ctx, t]			// expanded key[t]
	subs		t, t, #16
	b.gt		0b

	ldr			q17, [ctx]				// expanded key[0]
	AESD		0, 16					// 	xor/SubByte/ShiftRows
	eor.16b		v0, v0, v17				// out = state ^ key[0];

	eor.16b		v0, v0, v4
	orr.16b		v4, v18, v18

	mov			t, keylen
	str			q0, [obuf], #16
	subs		num_blk, num_blk, #1
	b.gt		L_scalar

L_done:
	mov			x0, #0
	str			q18, [iq]
#if CC_KERNEL
    // restore used vector registers
    ld1.4s      {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s      {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s      {v16,v17,v18}, [sp], #3*16
#endif
	ret			lr


#else
	#define	Select	1		// Select=1 to define aes_decrypt_cbc from aes_cbc.s
	#include "aes_cbc.s"
	#undef	Select
#endif

#endif /* __NO_ASM__ */
