# 
#  Copyright (c) 2014,2015 Apple Inc. All rights reserved.
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

/*
	the order of 2nd and 3rd calling arguments is different from the xnu implementation
 */

#include <corecrypto/cc_config.h>

#if CCSHA2_VNG_INTEL

/*
	This file provides x86_64/i386 hand implementation of the following function

	sha2_void sha256_compile(sha256_ctx ctx[1]);

	which is a C function in CommonCrypto Source/Digest/sha2.c

	The implementation here is modified from another sha256 x86_64/i386 implementation for sha256 in the xnu.
	To modify to fit the new API,
		the old ctx (points to ctx->hashes) shoule be changed to ctx->hashes, 8(ctx).
		the old data (points to ctx->wbuf), should be changed to ctx->wbuf, 40(ctx).

	sha256_compile handles 1 input block (64 bytes) per call.


	The following is comments for the initial xnu-sha256.s.

	void SHA256_Transform(SHA256_ctx *ctx, char *data, unsigned int num_blocks);

	which is a C function in sha2.c (from xnu).

	sha256 algorithm per block description:

		1. W(0:15) = big-endian (per 4 bytes) loading of input data (64 byte)
		2. load 8 digests a-h from ctx->state
		3. for r = 0:15
				T1 = h + Sigma1(e) + Ch(e,f,g) + K[r] + W[r];
				d += T1;
				h = T1 + Sigma0(a) + Maj(a,b,c)
				permute a,b,c,d,e,f,g,h into h,a,b,c,d,e,f,g
		4. for r = 16:63
				W[r] = W[r-16] + sigma1(W[r-2]) + W[r-7] + sigma0(W[r-15]);
				T1 = h + Sigma1(e) + Ch(e,f,g) + K[r] + W[r];
				d += T1;
				h = T1 + Sigma0(a) + Maj(a,b,c)
				permute a,b,c,d,e,f,g,h into h,a,b,c,d,e,f,g

	In the assembly implementation:
		- a circular window of message schedule W(r:r+15) is updated and stored in xmm0-xmm3
		- its corresponding W+K(r:r+15) is updated and stored in a stack space circular buffer
		- the 8 digests (a-h) will be stored in GPR or m32 (all in GPR for x86_64, and some in m32 for i386)

	the implementation per block looks like

	----------------------------------------------------------------------------

	load W(0:15) (big-endian per 4 bytes) into xmm0:xmm3
	pre_calculate and store W+K(0:15) in stack

	load digests a-h from ctx->state;

	for (r=0;r<48;r+=4) {
		digests a-h update and permute round r:r+3
		update W([r:r+3]%16) and WK([r:r+3]%16) for the next 4th iteration
	}

	for (r=48;r<64;r+=4) {
		digests a-h update and permute round r:r+3
	}

	ctx->states += digests a-h;

	----------------------------------------------------------------------------

	our implementation (allows multiple blocks per call) pipelines the loading of W/WK of a future block
	into the last 16 rounds of its previous block:

	----------------------------------------------------------------------------

	load W(0:15) (big-endian per 4 bytes) into xmm0:xmm3
	pre_calculate and store W+K(0:15) in stack

L_loop:

	load digests a-h from ctx->state;

	for (r=0;r<48;r+=4) {
		digests a-h update and permute round r:r+3
		update W([r:r+3]%16) and WK([r:r+3]%16) for the next 4th iteration
	}

	num_block--;
	if (num_block==0)	jmp L_last_block;

	for (r=48;r<64;r+=4) {
		digests a-h update and permute round r:r+3
		load W([r:r+3]%16) (big-endian per 4 bytes) into xmm0:xmm3
		pre_calculate and store W+K([r:r+3]%16) in stack
	}

	ctx->states += digests a-h;

	jmp	L_loop;

L_last_block:

	for (r=48;r<64;r+=4) {
		digests a-h update and permute round r:r+3
	}

	ctx->states += digests a-h;

	------------------------------------------------------------------------

	Apple CoreOS vector & numerics
*/
#if defined __x86_64__


/*
	I removed the cpu capabilities check for the CC_KERNEL version of this code
	This code is being used in a KEXT and the ability to load the address of
	__cpu_capabilities@GOTPCREL(%rip) is not available.  The fix for this
	was to add entry points into this routine for the SSE3 and NO_SSE3 
	processing and have a C function in the KEXT to the capability 
	checking.  
	
	The C code is as follows:
	
	void ccsha256_vng_intel_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
	{
	    // I need to add code here of the ilk
	    if (((cpuid_features() & CPUID_FEATURE_SSE3) != 0))
	    {
	        ccsha256_vng_intel_sse3_compress(state, nblocks, in);
	    }
	    else
	    {
	        ccsha256_vng_intel_nossse3_compress(state, nblocks, in);
	    }    
	}
*/

	// associate variables with registers or memory

	#define	sp			%rsp
	#define	ctx			%rdi
	#define num_blocks	%rsi
	#define	data        %rdx

	#define	a			%r8d
	#define	b			%r9d
	#define	c			%r10d
	#define	d			%r11d
	#define	e			%r12d
	#define	f			%r13d
	#define	g			%r14d
	#define	h			%r15d

	#define	K			%rbx
	#define stack_size	(8+16*8+16+64)	// 8 (align) + xmm0:xmm7 + L_aligned_bswap + WK(0:15)

	#define	L_aligned_bswap	64(sp)		// bswap : big-endian loading of 4-byte words
	#define	xmm_save	80(sp)			// starting address for xmm save/restore

	// 2 local variables
	#define	s	%eax
	#define	t	%ecx
	#define	u	%ebp

	// a window (16 words) of message scheule
	#define	W0	%xmm0
	#define	W1	%xmm1
	#define	W2	%xmm2
	#define	W3	%xmm3

	// circular buffer for WK[(r:r+15)%16]
	#define WK(x)   (x&15)*4(sp)

// #define Ch(x,y,z)   (((x) & (y)) ^ ((~(x)) & (z)))

	.macro Ch
	mov		$0, t		// x
	mov		$0, s		// x
	not		t			// ~x
	and		$1, s		// x & y
	and		$2, t		// ~x & z
	xor		s, t		// t = ((x) & (y)) ^ ((~(x)) & (z));
	.endm

// #define Maj(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

	.macro	Maj
#if 1		// steve's suggestion
	mov	 	$1,	t // y
	mov		$2,	s // z
	xor		$2,	t // y^z
	and		$1,	s // y&z
	and		$0, 	t // x&(y^z)
	xor		s,	t // Maj(x,y,z)
#else
	mov		$0, t		// x
	mov		$1, s		// y
	and		s, t		// x&y
	and		$2, s		// y&z
	xor		s, t		// (x&y) ^ (y&z)
	mov		$2, s		// z
	and		$0, s		// (x&z)
	xor		s, t		// t = (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#endif
	.endm

// #define sigma0_256(x)   (S32(7,  (x)) ^ S32(18, (x)) ^ R(3 ,   (x)))

	// performs sigma0_256 on 4 words on an xmm registers
	// use xmm6/xmm7 as intermediate registers
	.macro	sigma0
	movdqa	$0, %xmm6
	movdqa	$0, %xmm7
	psrld	$$3, $0			// SHR3(x)
	psrld	$$7, %xmm6		// part of ROTR7
	pslld	$$14, %xmm7		// part of ROTR18
	pxor	%xmm6, $0
	pxor	%xmm7, $0
	psrld	$$11, %xmm6		// part of ROTR18
	pslld	$$11, %xmm7		// part of ROTR7
	pxor	%xmm6, $0
	pxor	%xmm7, $0
	.endm

// #define sigma1_256(x)   (S32(17, (x)) ^ S32(19, (x)) ^ R(10,   (x)))

	// performs sigma1_256 on 4 words on an xmm registers
	// use xmm6/xmm7 as intermediate registers
	.macro	sigma1
	movdqa	$0, %xmm6
	movdqa	$0, %xmm7
	psrld	$$10, $0		// SHR10(x)
	psrld	$$17, %xmm6		// part of ROTR17
	pxor	%xmm6, $0
	pslld	$$13, %xmm7		// part of ROTR19
	pxor	%xmm7, $0
	psrld	$$2, %xmm6		// part of ROTR19
	pxor	%xmm6, $0
	pslld	$$2, %xmm7		// part of ROTR17
	pxor	%xmm7, $0
	.endm

// #define Sigma0_256(x)   (S32(2,  (x)) ^ S32(13, (x)) ^ S32(22, (x)))

	.macro	Sigma0
	mov		$0, t			// x
	mov		$0, s			// x
	ror		$$2, t			// S32(2,  (x))
	ror		$$13, s			// S32(13,  (x))
	xor		s, t			// S32(2,  (x)) ^ S32(13, (x))
	ror		$$9, s			// S32(22,  (x))
	xor		s, t			// t = (S32(2,  (x)) ^ S32(13, (x)) ^ S32(22, (x)))
	.endm

// #define Sigma1_256(x)   (S32(6,  (x)) ^ S32(11, (x)) ^ S32(25, (x)))

	.macro	Sigma1
	mov		$0, s			// x
	ror		$$6, s			// S32(6,  (x))
	mov		s, t			// S32(6,  (x))
	ror		$$5, s			// S32(11, (x))
	xor		s, t			// S32(6,  (x)) ^ S32(11, (x))
	ror		$$14, s			// S32(25, (x))
	xor		s, t			// t = (S32(6,  (x)) ^ S32(11, (x)) ^ S32(25, (x)))
	.endm

	// per round digests update
	.macro	round_ref
	Sigma1	$4				// t = T1
	add		t, $7			// use h to store h+Sigma1(e)
	Ch		$4, $5, $6		// t = Ch (e, f, g);
	add		$7, t			// t = h+Sigma1(e)+Ch(e,f,g);
	add		WK($8), t		// h = T1
	add		t, $3			// d += T1;
	mov		t, $7			// h = T1
	Sigma0	$0				// t = Sigma0(a);
	add		t, $7			// h = T1 + Sigma0(a);
	Maj		$0, $1, $2		// t = Maj(a,b,c)
	add		t, $7			// h = T1 + Sigma0(a) + Maj(a,b,c);
	.endm

	.macro	round
	mov		$4, s
	mov		$0, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		$4, s
	mov		$5, u
	xor		$0, t
	ror		$$(11-6), s
	xor		$6, u
	xor		$4, s
	and		$4, u
	ror		$$(13-2), t
	xor		$0, t
	xor		$6, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK($8), u
	mov		$0, s
	add		u, $7
	mov		$0, u
	or		$2, s
	add		$7, $3
	and		$2, u
	and		$1, s
	add		t, $7
	or		u, s
	add		s, $7	
	.endm

	// per 4 rounds digests update and permutation
	// permutation is absorbed by rotating the roles of digests a-h
	.macro	rounds
	round	$0, $1, $2, $3, $4, $5, $6, $7, 0+$8
	round	$7, $0, $1, $2, $3, $4, $5, $6, 1+$8
	round	$6, $7, $0, $1, $2, $3, $4, $5, 2+$8
	round	$5, $6, $7, $0, $1, $2, $3, $4, 3+$8
	.endm

	.macro	roundsA
	round	a, b, c, d, e, f, g, h, 0+$0
	round	h, a, b, c, d, e, f, g, 1+$0
	round	g, h, a, b, c, d, e, f, 2+$0
	round	f, g, h, a, b, c, d, e, 3+$0
	.endm

	.macro	roundsE
	round	e, f, g, h, a, b, c, d, 0+$0
	round	d, e, f, g, h, a, b, c, 1+$0
	round	c, d, e, f, g, h, a, b, 2+$0
	round	b, c, d, e, f, g, h, a, 3+$0
	.endm


	// update the message schedule W and W+K (4 rounds) 16 rounds ahead in the future
	.macro	message_schedule
	movdqu	(K), %xmm5
	addq	$$16, K				// K points to next K256 word for next iteration
	movdqa	$1, %xmm4 			// W7:W4
	palignr	$$4, $0, %xmm4		// W4:W1
	movdqa	%xmm4, %xmm6
	movdqa	%xmm4, %xmm7
	psrld	$$3, %xmm4			// SHR3(x)
	psrld	$$7, %xmm6		// part of ROTR7
	pslld	$$14, %xmm7		// part of ROTR18
	pxor	%xmm6, %xmm4
	pxor	%xmm7, %xmm4
	psrld	$$11, %xmm6		// part of ROTR18
	pslld	$$11, %xmm7		// part of ROTR7
	pxor	%xmm6, %xmm4
	pxor	%xmm7, %xmm4
	movdqa	$3, %xmm6 			// W15:W12
	paddd	%xmm4, $0			// $0 = W3:W0 + sigma0(W4:W1)
	palignr	$$4, $2, %xmm6		// W12:W9
	paddd	%xmm6, $0			// $0 = W12:W9 + sigma0(W4:W1) + W3:W0
	movdqa	$3, %xmm4			// W15:W12
	psrldq	$$8, %xmm4			// 0,0,W15,W14
	movdqa	%xmm4, %xmm6
	movdqa	%xmm4, %xmm7
	psrld	$$10, %xmm4		// SHR10(x)
	psrld	$$17, %xmm6		// part of ROTR17
	pxor	%xmm6, %xmm4
	pslld	$$13, %xmm7		// part of ROTR19
	pxor	%xmm7, %xmm4
	psrld	$$2, %xmm6		// part of ROTR19
	pxor	%xmm6, %xmm4
	pslld	$$2, %xmm7		// part of ROTR17
	pxor	%xmm7, %xmm4
	paddd	%xmm4, $0			// sigma1(0,0,W15,W14) + W12:W9 + sigma0(W4:W1) + W3:W0
	movdqa	$0, %xmm4			// W19-sigma1(W17), W18-sigma1(W16), W17, W16
	pslldq	$$8, %xmm4			// W17, W16, 0, 0
	movdqa	%xmm4, %xmm6
	movdqa	%xmm4, %xmm7
	psrld	$$10, %xmm4		// SHR10(x)
	psrld	$$17, %xmm6		// part of ROTR17
	pxor	%xmm6, %xmm4
	pslld	$$13, %xmm7		// part of ROTR19
	pxor	%xmm7, %xmm4
	psrld	$$2, %xmm6		// part of ROTR19
	pxor	%xmm6, %xmm4
	pslld	$$2, %xmm7		// part of ROTR17
	pxor	%xmm7, %xmm4
	paddd	%xmm4, $0			// W19:W16
	paddd	$0, %xmm5			// WK
	movdqa	%xmm5, WK($4)
	.endm

	// this macro is used in the last 16 rounds of a current block
	// it reads the next message (16 4-byte words), load it into 4 words W[r:r+3], computes WK[r:r+3]
	// and save into stack to prepare for next block

	.macro	update_W_WK
	movdqu	$0*16(data), $1		// read 4 4-byte words
	pshufb	L_aligned_bswap, $1	// big-endian of each 4-byte word, W[r:r+3]
	movdqu	$0*16(K), %xmm4		// K[r:r+3]
	paddd	$1, %xmm4			// WK[r:r+3]
	movdqa	%xmm4, WK($0*4)		// save WK[r:r+3] into stack circular buffer
	.endm

	.macro	roundsA_schedule
	// round	a, b, c, d, e, f, g, h, 0+$4
		movdqu	(K), %xmm5
		addq	$$16, K				// K points to next K256 word for next iteration
	mov		e, s
	mov		a, t
		movdqa	$1, %xmm4 			// W7:W4
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		e, s
		palignr	$$4, $0, %xmm4		// W4:W1
	mov		f, u
	xor		a, t
	ror		$$(11-6), s
	xor		g, u
		movdqa	%xmm4, %xmm6
	xor		e, s
		movdqa	%xmm4, %xmm7
	and		e, u
		psrld	$$3, %xmm4			// SHR3(x)
	ror		$$(13-2), t
	xor		a, t
		psrld	$$7, %xmm6		// part of ROTR7
	xor		g, u
		pslld	$$14, %xmm7		// part of ROTR18
	ror		$$6, s
	add		s, u
	ror		$$2, t
		pxor	%xmm6, %xmm4
	add		WK(0+$4), u
	mov		a, s
		pxor	%xmm7, %xmm4
	add		u, h
		psrld	$$11, %xmm6		// part of ROTR18
	mov		a, u
		pslld	$$11, %xmm7		// part of ROTR7
	or		c, s
	add		h, d
	and		c, u
	and		b, s
		pxor	%xmm6, %xmm4
	add		t, h
	or		u, s
	add		s, h	
		pxor	%xmm7, %xmm4

	// round	h, a, b, c, d, e, f, g, 1+$4
	mov		d, s
		movdqa	$3, %xmm6 			// W15:W12
	mov		h, t
	ror		$$(25-11), s
	ror		$$(22-13), t
		paddd	%xmm4, $0			// $0 = W3:W0 + sigma0(W4:W1)
	xor		d, s
	mov		e, u
	xor		h, t
		palignr	$$4, $2, %xmm6		// W12:W9
	ror		$$(11-6), s
	xor		f, u
		movdqa	$3, %xmm4			// W15:W12
	xor		d, s
	and		d, u
		paddd	%xmm6, $0			// $0 = W12:W9 + sigma0(W4:W1) + W3:W0
	ror		$$(13-2), t
	xor		h, t
	xor		f, u
		psrldq	$$8, %xmm4			// 0,0,W15,W14
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(1+$4), u
		movdqa	%xmm4, %xmm6
	mov		h, s
		movdqa	%xmm4, %xmm7
	add		u, g
		psrld	$$10, %xmm4		// SHR10(x)
	mov		h, u
	or		b, s
		psrld	$$17, %xmm6		// part of ROTR17
	add		g, c
	and		b, u
		pslld	$$13, %xmm7		// part of ROTR19
	and		a, s
		pxor	%xmm6, %xmm4
	add		t, g
	or		u, s
		psrld	$$2, %xmm6		// part of ROTR19
	add		s, g	
		pxor	%xmm7, %xmm4


	// round	g, h, a, b, c, d, e, f, 2+$4
	mov		c, s
	mov		g, t
		pslld	$$2, %xmm7		// part of ROTR17
	ror		$$(25-11), s
	ror		$$(22-13), t
		pxor	%xmm6, %xmm4
	xor		c, s
	mov		d, u
	xor		g, t
	ror		$$(11-6), s
		pxor	%xmm7, %xmm4
	xor		e, u
	xor		c, s
	and		c, u
	ror		$$(13-2), t
		paddd	%xmm4, $0			// sigma1(0,0,W15,W14) + W12:W9 + sigma0(W4:W1) + W3:W0
	xor		g, t
	xor		e, u
		movdqa	$0, %xmm4			// W19-sigma1(W17), W18-sigma1(W16), W17, W16
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(2+$4), u
		pslldq	$$8, %xmm4			// W17, W16, 0, 0
	mov		g, s
	add		u, f
	mov		g, u
	or		a, s
		movdqa	%xmm4, %xmm6
	add		f, b
		movdqa	%xmm4, %xmm7
	and		a, u
	and		h, s
		psrld	$$10, %xmm4		// SHR10(x)
	add		t, f
		pslld	$$13, %xmm7		// part of ROTR19
	or		u, s
	add		s, f	
		psrld	$$17, %xmm6		// part of ROTR17

	// round	f, g, h, a, b, c, d, e, 3+$4
	mov		b, s
	mov		f, t
	ror		$$(25-11), s
		pxor	%xmm7, %xmm4
	ror		$$(22-13), t
	xor		b, s
		pxor	%xmm6, %xmm4
	mov		c, u
	xor		f, t
		psrld	$$2, %xmm6		// part of ROTR19
	ror		$$(11-6), s
	xor		d, u
		pslld	$$2, %xmm7		// part of ROTR17
	xor		b, s
	and		b, u
		pxor	%xmm6, %xmm4
	ror		$$(13-2), t
	xor		f, t
	xor		d, u
	ror		$$6, s
	add		s, u
		pxor	%xmm7, %xmm4
	ror		$$2, t
	add		WK(3+$4), u
	mov		f, s
	add		u, e
			paddd	%xmm4, $0			// W19:W16
	mov		f, u
	or		h, s
	add		e, a
	and		h, u
			paddd	$0, %xmm5			// WK
	and		g, s
	add		t, e
	or		u, s
	add		s, e	
			movdqa	%xmm5, WK($4)
	.endm

	.macro	roundsE_schedule
	// round	e, f, g, h, a, b, c, d, 0+$4
		movdqu	(K), %xmm5
		addq	$$16, K				// K points to next K256 word for next iteration
	mov		a, s
	mov		e, t
		movdqa	$1, %xmm4 			// W7:W4
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		a, s
		palignr	$$4, $0, %xmm4		// W4:W1
	mov		b, u
	xor		e, t
	ror		$$(11-6), s
	xor		c, u
		movdqa	%xmm4, %xmm6
	xor		a, s
		movdqa	%xmm4, %xmm7
	and		a, u
		psrld	$$3, %xmm4			// SHR3(x)
	ror		$$(13-2), t
	xor		e, t
		psrld	$$7, %xmm6		// part of ROTR7
	xor		c, u
		pslld	$$14, %xmm7		// part of ROTR18
	ror		$$6, s
	add		s, u
	ror		$$2, t
		pxor	%xmm6, %xmm4
	add		WK(0+$4), u
	mov		e, s
		pxor	%xmm7, %xmm4
	add		u, d
		psrld	$$11, %xmm6		// part of ROTR18
	mov		e, u
		pslld	$$11, %xmm7		// part of ROTR7
	or		g, s
	add		d, h
	and		g, u
	and		f, s
		pxor	%xmm6, %xmm4
	add		t, d
	or		u, s
	add		s, d	
		pxor	%xmm7, %xmm4

	// round	d, e, f, g, h, a, b, c, 1+$4
	mov		h, s
		movdqa	$3, %xmm6 			// W15:W12
	mov		d, t
	ror		$$(25-11), s
	ror		$$(22-13), t
		paddd	%xmm4, $0			// $0 = W3:W0 + sigma0(W4:W1)
	xor		h, s
	mov		a, u
	xor		d, t
		palignr	$$4, $2, %xmm6		// W12:W9
	ror		$$(11-6), s
	xor		b, u
		movdqa	$3, %xmm4			// W15:W12
	xor		h, s
	and		h, u
		paddd	%xmm6, $0			// $0 = W12:W9 + sigma0(W4:W1) + W3:W0
	ror		$$(13-2), t
	xor		d, t
	xor		b, u
		psrldq	$$8, %xmm4			// 0,0,W15,W14
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(1+$4), u
		movdqa	%xmm4, %xmm6
	mov		d, s
		movdqa	%xmm4, %xmm7
	add		u, c
		psrld	$$10, %xmm4		// SHR10(x)
	mov		d, u
	or		f, s
		psrld	$$17, %xmm6		// part of ROTR17
	add		c, g
	and		f, u
		pslld	$$13, %xmm7		// part of ROTR19
	and		e, s
		pxor	%xmm6, %xmm4
	add		t, c
	or		u, s
		psrld	$$2, %xmm6		// part of ROTR19
	add		s, c	
		pxor	%xmm7, %xmm4

	//round	c, d, e, f, g, h, a, b, 2+$4
	mov		g, s
	mov		c, t
		pslld	$$2, %xmm7		// part of ROTR17
	ror		$$(25-11), s
	ror		$$(22-13), t
		pxor	%xmm6, %xmm4
	xor		g, s
	mov		h, u
	xor		c, t
	ror		$$(11-6), s
		pxor	%xmm7, %xmm4
	xor		a, u
	xor		g, s
	and		g, u
	ror		$$(13-2), t
		paddd	%xmm4, $0			// sigma1(0,0,W15,W14) + W12:W9 + sigma0(W4:W1) + W3:W0
	xor		c, t
	xor		a, u
		movdqa	$0, %xmm4			// W19-sigma1(W17), W18-sigma1(W16), W17, W16
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(2+$4), u
		pslldq	$$8, %xmm4			// W17, W16, 0, 0
	mov		c, s
	add		u, b
	mov		c, u
	or		e, s
		movdqa	%xmm4, %xmm6
	add		b, f
		movdqa	%xmm4, %xmm7
	and		e, u
	and		d, s
		psrld	$$10, %xmm4		// SHR10(x)
	add		t, b
	or		u, s
		pslld	$$13, %xmm7		// part of ROTR19
	add		s, b	
		psrld	$$17, %xmm6		// part of ROTR17


	// round	b, c, d, e, f, g, h, a, 3+$4
	mov		f, s
	mov		b, t
	ror		$$(25-11), s
		pxor	%xmm7, %xmm4
	ror		$$(22-13), t
	xor		f, s
		pxor	%xmm6, %xmm4
	mov		g, u
	xor		b, t
		psrld	$$2, %xmm6		// part of ROTR19
	ror		$$(11-6), s
	xor		h, u
		pslld	$$2, %xmm7		// part of ROTR17
	xor		f, s
	and		f, u
		pxor	%xmm6, %xmm4
	ror		$$(13-2), t
	xor		b, t
	xor		h, u
	ror		$$6, s
	add		s, u
		pxor	%xmm7, %xmm4
	ror		$$2, t
	add		WK(3+$4), u
	mov		b, s
	add		u, a
		paddd	%xmm4, $0			// W19:W16
	mov		b, u
	or		d, s
	add		a, e
	and		d, u
		paddd	$0, %xmm5			// WK
	and		c, s
	add		t, a
	or		u, s
	add		s, a	

		movdqa	%xmm5, WK($4)

	.endm

	.macro	roundsA_update
	// round	a, b, c, d, e, f, g, h, 0+$0
		movdqu	(($0&12)*4)(data), $1		// read 4 4-byte words
	mov		e, s
	mov		a, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		e, s
	mov		f, u
	xor		a, t
	ror		$$(11-6), s
	xor		g, u
	xor		e, s
	and		e, u
	ror		$$(13-2), t
	xor		a, t
	xor		g, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(0+$0), u
	mov		a, s
	add		u, h
	mov		a, u
	or		c, s
	add		h, d
	and		c, u
	and		b, s
	add		t, h
	or		u, s
	add		s, h	

	// round	h, a, b, c, d, e, f, g, 1+$0
		pshufb	L_aligned_bswap, $1	// big-endian of each 4-byte word, W[r:r+3]
	mov		d, s
	mov		h, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		d, s
	mov		e, u
	xor		h, t
	ror		$$(11-6), s
	xor		f, u
	xor		d, s
	and		d, u
	ror		$$(13-2), t
	xor		h, t
	xor		f, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(1+$0), u
	mov		h, s
	add		u, g
	mov		h, u
	or		b, s
	add		g, c
	and		b, u
	and		a, s
	add		t, g
	or		u, s
	add		s, g	

	// round	g, h, a, b, c, d, e, f, 2+$0
	movdqu	(($0&12)*4)(K), %xmm4		// K[r:r+3]
	mov		c, s
	mov		g, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		c, s
	mov		d, u
	xor		g, t
	ror		$$(11-6), s
	xor		e, u
	xor		c, s
	and		c, u
	ror		$$(13-2), t
	xor		g, t
	xor		e, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(2+$0), u
	mov		g, s
	add		u, f
	mov		g, u
	or		a, s
	add		f, b
	and		a, u
	and		h, s
	add		t, f
	or		u, s
	add		s, f	

	// round	f, g, h, a, b, c, d, e, 3+$0
		paddd	$1, %xmm4			// WK[r:r+3]
	mov		b, s
	mov		f, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		b, s
	mov		c, u
	xor		f, t
	ror		$$(11-6), s
	xor		d, u
	xor		b, s
	and		b, u
	ror		$$(13-2), t
	xor		f, t
	xor		d, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(3+$0), u
	mov		f, s
	add		u, e
	mov		f, u
	or		h, s
	add		e, a
	and		h, u
	and		g, s
	add		t, e
	or		u, s
	add		s, e	

		movdqa	%xmm4, WK($0&12)		// save WK[r:r+3] into stack circular buffer

	.endm

	.macro	roundsE_update
	// round	e, f, g, h, a, b, c, d, 0+$0
		movdqu	(($0&12)*4)(data), $1		// read 4 4-byte words
	mov		a, s
	mov		e, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		a, s
	mov		b, u
	xor		e, t
	ror		$$(11-6), s
	xor		c, u
	xor		a, s
	and		a, u
	ror		$$(13-2), t
	xor		e, t
	xor		c, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(0+$0), u
	mov		e, s
	add		u, d
	mov		e, u
	or		g, s
	add		d, h
	and		g, u
	and		f, s
	add		t, d
	or		u, s
	add		s, d	

	// round	d, e, f, g, h, a, b, c, 1+$0
		pshufb	L_aligned_bswap, $1	// big-endian of each 4-byte word, W[r:r+3]
	mov		h, s
	mov		d, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		h, s
	mov		a, u
	xor		d, t
	ror		$$(11-6), s
	xor		b, u
	xor		h, s
	and		h, u
	ror		$$(13-2), t
	xor		d, t
	xor		b, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(1+$0), u
	mov		d, s
	add		u, c
	mov		d, u
	or		f, s
	add		c, g
	and		f, u
	and		e, s
	add		t, c
	or		u, s
	add		s, c	

	//round	c, d, e, f, g, h, a, b, 2+$0
		movdqu	(($0&12)*4)(K), %xmm4		// K[r:r+3]
	mov		g, s
	mov		c, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		g, s
	mov		h, u
	xor		c, t
	ror		$$(11-6), s
	xor		a, u
	xor		g, s
	and		g, u
	ror		$$(13-2), t
	xor		c, t
	xor		a, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(2+$0), u
	mov		c, s
	add		u, b
	mov		c, u
	or		e, s
	add		b, f
	and		e, u
	and		d, s
	add		t, b
	or		u, s
	add		s, b	

	// round	b, c, d, e, f, g, h, a, 3+$0
		paddd	$1, %xmm4			// WK[r:r+3]
	mov		f, s
	mov		b, t
	ror		$$(25-11), s
	ror		$$(22-13), t
	xor		f, s
	mov		g, u
	xor		b, t
	ror		$$(11-6), s
	xor		h, u
	xor		f, s
	and		f, u
	ror		$$(13-2), t
	xor		b, t
	xor		h, u
	ror		$$6, s
	add		s, u
	ror		$$2, t
	add		WK(3+$0), u
	mov		b, s
	add		u, a
	mov		b, u
	or		d, s
	add		a, e
	and		d, u
	and		c, s
	add		t, a
	or		u, s
	add		s, a	

	// update_W_WK	(($0/4)&3), $1
		movdqa	%xmm4, WK($0&12)		// save WK[r:r+3] into stack circular buffer
	.endm

	.text
    .globl	_ccsha256_vng_intel_ssse3_compress
_ccsha256_vng_intel_ssse3_compress:

	// push callee-saved registers
	push	%rbp
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	// allocate stack space
	sub		$stack_size, sp

	// if kernel code, save used xmm registers
#if CC_KERNEL
	movdqa	%xmm0, 0*16+xmm_save
	movdqa	%xmm1, 1*16+xmm_save
	movdqa	%xmm2, 2*16+xmm_save
	movdqa	%xmm3, 3*16+xmm_save
	movdqa	%xmm4, 4*16+xmm_save
	movdqa	%xmm5, 5*16+xmm_save
	movdqa	%xmm6, 6*16+xmm_save
	movdqa	%xmm7, 7*16+xmm_save
#endif

	// set up bswap parameters in the aligned stack space and pointer to table K256[]
	lea		_ccsha256_K(%rip), K
	lea		L_bswap(%rip), %rax
	movdqa	(%rax), %xmm0
	movdqa	%xmm0, L_aligned_bswap

	// load W[0:15] into xmm0-xmm3
	movdqu	0*16(data), W0
	movdqu	1*16(data), W1
	movdqu	2*16(data), W2
	movdqu	3*16(data), W3
	addq	$64, data

	pshufb	L_aligned_bswap, W0
	pshufb	L_aligned_bswap, W1
	pshufb	L_aligned_bswap, W2
	pshufb	L_aligned_bswap, W3

	// compute WK[0:15] and save in stack
	movdqu	0*16(K), %xmm4
	movdqu	1*16(K), %xmm5
	movdqu	2*16(K), %xmm6
	movdqu	3*16(K), %xmm7
    addq	$64, K
	paddd	%xmm0, %xmm4
	paddd	%xmm1, %xmm5
	paddd	%xmm2, %xmm6
	paddd	%xmm3, %xmm7
	movdqa	%xmm4, WK(0)
	movdqa	%xmm5, WK(4)
	movdqa	%xmm6, WK(8)
	movdqa	%xmm7, WK(12)

L_loop:

	// digests a-h = ctx->states;
	mov		0*4(ctx), a
	mov		1*4(ctx), b
	mov		2*4(ctx), c
	mov		3*4(ctx), d
	mov		4*4(ctx), e
	mov		5*4(ctx), f
	mov		6*4(ctx), g
	mov		7*4(ctx), h

	// rounds 0:47 interleaved with W/WK update for rounds 16:63
	roundsA_schedule W0,W1,W2,W3,16
	roundsE_schedule W1,W2,W3,W0,20
	roundsA_schedule W2,W3,W0,W1,24
	roundsE_schedule W3,W0,W1,W2,28
	roundsA_schedule W0,W1,W2,W3,32
	roundsE_schedule W1,W2,W3,W0,36
	roundsA_schedule W2,W3,W0,W1,40
	roundsE_schedule W3,W0,W1,W2,44
	roundsA_schedule W0,W1,W2,W3,48
	roundsE_schedule W1,W2,W3,W0,52
	roundsA_schedule W2,W3,W0,W1,56
	roundsE_schedule W3,W0,W1,W2,60

	// revert K to the beginning of K256[]
	subq		$256, K
	subq		$1, num_blocks				// num_blocks--

	je		L_final_block				// if final block, wrap up final rounds

	// rounds 48:63 interleaved with W/WK initialization for next block rounds 0:15
	roundsA_update	48, W0
	roundsE_update	52, W1
	roundsA_update	56, W2
	roundsE_update	60, W3

	addq	$64, K
	addq	$64, data

	// ctx->states += digests a-h
	add		a, 0*4(ctx)
	add		b, 1*4(ctx)
	add		c, 2*4(ctx)
	add		d, 3*4(ctx)
	add		e, 4*4(ctx)
	add		f, 5*4(ctx)
	add		g, 6*4(ctx)
	add		h, 7*4(ctx)

	jmp		L_loop				// branch for next block

	// wrap up digest update round 48:63 for final block
L_final_block:
	roundsA	48
	roundsE	52
	roundsA	56
	roundsE	60

	// ctx->states += digests a-h
	add		a, 0*4(ctx)
	add		b, 1*4(ctx)
	add		c, 2*4(ctx)
	add		d, 3*4(ctx)
	add		e, 4*4(ctx)
	add		f, 5*4(ctx)
	add		g, 6*4(ctx)
	add		h, 7*4(ctx)

	// if kernel, restore xmm0-xmm7
#if CC_KERNEL
	movdqa	0*16+xmm_save, %xmm0
	movdqa	1*16+xmm_save, %xmm1
	movdqa	2*16+xmm_save, %xmm2
	movdqa	3*16+xmm_save, %xmm3
	movdqa	4*16+xmm_save, %xmm4
	movdqa	5*16+xmm_save, %xmm5
	movdqa	6*16+xmm_save, %xmm6
	movdqa	7*16+xmm_save, %xmm7
#endif

	// free allocated stack memory
	add		$stack_size, sp

	// restore callee-saved registers
	pop		%r15
	pop		%r14
	pop		%r13
	pop		%r12
	pop		%rbx
	pop		%rbp

	// return
	ret

	// data for using ssse3 pshufb instruction (big-endian loading of data)
    .const
    .align  4, 0x90

L_bswap:
    .long   0x00010203
    .long   0x04050607
    .long   0x08090a0b
    .long   0x0c0d0e0f


#endif      // x86_64

#endif /* CCSHA2_VNG_INTEL */
#endif /* __NO_ASM__ */
