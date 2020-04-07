# 
#  Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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

/* 	This files defines _aes_encrypt_key_hw and _aes_decrypt_key_hw --- Intel Westmere HW AES-based implementation
	of _aes_encrypt_key and _aes_decrypt_key. 

	These 2 functions SHOULD BE entried ONLY after the AES HW is verified to be available. 
	They SHOULD NOT be called without AES HW detection. It might cause xnu to crash.

	The AES HW is detected 1st thing in 
		_aes_encrypt_key (ExpandKeyForEncryption.s) 
		_aes_decrypt_key (ExpandKeyForDecryption.s)
	and, if AES HW is detected, branch without link (ie, jump) to the functions here.

	The implementation here follows the examples in an Intel White Paper
	"Intel Advanced Encryption Standard (AES) Instruction Set" Rev.2 01

	Note: Rev. 03 Final 2010 01 26 is available. Looks like some code change from Rev.2 01

*/

#include <corecrypto/cc_config.h>

#if !defined(__NO_ASM__) && CCAES_INTEL

#if (defined __i386__ || defined __x86_64__)

	.text	
	.align	4,0x90

	// hw_aes_encrypt_key(key, klen, hwectx);
	// klen = 16, 24, or 32, or (128/192/256)

	.globl	_aes_encrypt_key_hw
_aes_encrypt_key_hw:

	.globl _vng_aes_encrypt_aesni_key
_vng_aes_encrypt_aesni_key:

#ifdef	__i386__
	push	%ebp
	mov		%esp, %ebp
	push	%ebx
	push	%edi	
	mov		8(%ebp), %eax		// pointer to key
	mov		12(%ebp), %ebx		// klen
	mov		16(%ebp), %edi		// ctx
	#define	pkey	%eax
	#define	klen	%ebx
	#define	ctx		%edi
	#define	sp		%esp
	#define	cx		%ecx
#else
	#define	pkey	%rdi
	#define	klen	%rsi
	#define	ctx		%rdx
	#define	sp		%rsp
	#define	cx		%rcx
	push	%rbp
	mov		%rsp, %rbp
#endif

#if CC_KERNEL
	// for xmm registers save and restore
	sub		$(16*4), sp
#endif

	cmp		$32, klen
	jg		0f					// klen>32
	shl		$3, klen			// convert 16/24/32 to 128/192/256
0:

	cmp		$128, klen			// AES-128 ?
	je		L_AES_128_Encrypt_Key
	cmp		$192, klen			// AES-192 ?
	je		L_AES_192_Encrypt_Key
	cmp		$256, klen			// AES-256 ?
	je		L_AES_256_Encrypt_Key
	mov		$1, %eax			// return error for wrong klen 
L_Encrypt_Key_2_return:
#if CC_KERNEL
	add		$(16*4), sp
#endif
#ifdef	__i386__
	pop		%edi
	pop		%ebx
#endif
	leave
	ret

L_AES_128_Encrypt_Key:
#if CC_KERNEL
	// save xmm registers
	movaps	%xmm1, (sp)
	movaps	%xmm2, 16(sp)
	movaps	%xmm3, 32(sp)
#endif	// CC_KERNEL

	movl	$160, 240(ctx)		// write expanded key length to ctx
	xor		cx, cx

	movups	(pkey), %xmm1
	movups	%xmm1, (ctx)
	aeskeygenassist	$1, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$2, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$4, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$8, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x10, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x20, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x40, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x80, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x1b, %xmm1, %xmm2
	call	L_key_expansion_128
	aeskeygenassist	$0x36, %xmm1, %xmm2
	call	L_key_expansion_128

#if CC_KERNEL
	// restore xmm registers
	movaps	(sp), %xmm1
	movaps	16(sp), %xmm2
	movaps	32(sp), %xmm3
#endif	// CC_KERNEL
	xor		%eax, %eax			// return 0 for success
	jmp		L_Encrypt_Key_2_return

	.align	4, 0x90
L_key_expansion_128:
	pshufd	$0xff, %xmm2, %xmm2
	movaps	%xmm1, %xmm3
	pslldq	$4, %xmm3
	pxor	%xmm3, %xmm1
	movaps	%xmm1, %xmm3
	pslldq	$4, %xmm3
	pxor	%xmm3, %xmm1
	movaps	%xmm1, %xmm3
	pslldq	$4, %xmm3
	pxor	%xmm3, %xmm1
	pxor	%xmm2, %xmm1
	add		$16, cx
	movups	%xmm1, (ctx, cx)
	ret

L_AES_192_Encrypt_Key:
#if CC_KERNEL
	// save xmm registers
	movaps	%xmm1, (sp)
	movaps	%xmm2, 16(sp)
	movaps	%xmm3, 32(sp)
	movaps	%xmm4, 48(sp)
#endif	// CC_KERNEL
	movl	$192, 240(ctx)		// write expanded key length to ctx

	movups	(pkey), %xmm1
	movq	16(pkey), %xmm3

	movups	%xmm1, (ctx)
	movq	%xmm3, 16(ctx)

	lea		24(ctx), cx

	aeskeygenassist	$1, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$2, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$4, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$8, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$0x10, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$0x20, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$0x40, %xmm3, %xmm2
	call	L_key_expansion_192
	aeskeygenassist	$0x80, %xmm3, %xmm2
	call	L_key_expansion_192

#if CC_KERNEL
	// restore xmm registers
	movaps	(sp), %xmm1
	movaps	16(sp), %xmm2
	movaps	32(sp), %xmm3
	movaps	48(sp), %xmm4
#endif	// CC_KERNEL
	xor		%eax, %eax			// return 0 for success
	jmp		L_Encrypt_Key_2_return

	.align	4, 0x90
L_key_expansion_192:
	pshufd	$0x55, %xmm2, %xmm2

	movaps	%xmm1, %xmm4
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pxor	%xmm2, %xmm1

	pshufd	$0xff, %xmm1, %xmm2

	movaps	%xmm3, %xmm4
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm3
	pxor	%xmm2, %xmm3

	movups	%xmm1, (cx)
	movq	%xmm3, 16(cx)

	add		$24, cx
	ret

L_AES_256_Encrypt_Key:
#if CC_KERNEL
	// save xmm registers
	movaps	%xmm1, (sp)
	movaps	%xmm2, 16(sp)
	movaps	%xmm3, 32(sp)
	movaps	%xmm4, 48(sp)
#endif	// CC_KERNEL
	movl	$224, 240(ctx)		// write expanded key length to ctx

	movups	(pkey), %xmm1
	movups	16(pkey), %xmm3
	movups	%xmm1, (ctx)
	movups	%xmm3, 16(ctx)

	lea		32(ctx), cx

	aeskeygenassist	$1, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$2, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$4, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$8, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$0x10, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$0x20, %xmm3, %xmm2
	call	L_key_expansion_256
	aeskeygenassist	$0x40, %xmm3, %xmm2
	call	L_key_expansion_256_final

#if CC_KERNEL
	// restore xmm registers
	movaps	(sp), %xmm1
	movaps	16(sp), %xmm2
	movaps	32(sp), %xmm3
	movaps	48(sp), %xmm4
#endif	// CC_KERNEL
	xor		%eax, %eax			// return 0 for success
	jmp		L_Encrypt_Key_2_return

	.align	4, 0x90
L_key_expansion_256:

	pshufd	$0xff, %xmm2, %xmm2

	movaps	%xmm1, %xmm4
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pxor	%xmm2, %xmm1

	movups	%xmm1, (cx)

	aeskeygenassist	$0, %xmm1, %xmm4

	pshufd	$0xaa, %xmm4, %xmm2

	movaps	%xmm3, %xmm4
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm3
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm3
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm3
	pxor	%xmm2, %xmm3

	movups	%xmm3, 16(cx)

	add		$32, cx
	ret

	.align	4, 0x90
L_key_expansion_256_final:

	pshufd	$0xff, %xmm2, %xmm2

	movaps	%xmm1, %xmm4
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4

	pxor	%xmm4, %xmm1
	pxor	%xmm2, %xmm1

	movups	%xmm1, (cx)
	ret 

// _aes_decrypt_key_hw is implemented as
// 	1. call _aes_encrypt_key_hw
// 	2. use aesimc to convert the expanded round keys (except the 1st and last round keys)

	.text	
	.align	4, 0x90
	.globl	_aes_decrypt_key_hw
_aes_decrypt_key_hw:

	.globl _vng_aes_decrypt_aesni_key
_vng_aes_decrypt_aesni_key:

#ifdef	__i386__

	push	%ebp
	mov		%esp, %ebp
	sub		$(8+16), %esp

	// copy input arguments for calling aes_decrypt_key_hw

	mov		8(%ebp), %eax
	mov		%eax, (%esp)
	mov		12(%ebp), %eax
	mov		%eax, 4(%esp)
	mov		16(%ebp), %eax
	mov		%eax, 8(%esp)

#else

	push	%rbp
	mov		%rsp, %rbp
	sub		$16, %rsp

	// calling arguments %rdi/%rsi/%rdx will be used for encrypt_key 
	// %rdx (ctx) will return unchanged
	// %rsi (klen) will (<<3) if <= 32

#endif
	call	_aes_encrypt_key_hw
	cmp		$0, %eax
	je		L_decrypt_inv
L_decrypt_almost_done:
#ifdef	__i386__
	add		$(8+16), %esp
#else
	add		$16, %rsp
#endif
	leave
	ret

L_decrypt_inv:
#if CC_KERNEL
	movaps	%xmm0, (sp)
#endif

#ifdef	__i386__	
	#undef	klen
	#undef	ctx	
	mov     12(%ebp), %eax      // klen
    mov     16(%ebp), %edx      // ctx
	#define	klen	%eax
	#define	ctx		%edx
	cmp		$32, klen
	jg		0f					// klen>32
	shl		$3, klen			// convert 16/24/32 to 128/192/256
0:
#endif

	mov		$9, cx				// default is AES-128
	cmp		$128, klen
	je		L_Decrypt_Key
	add		$2, cx
	cmp		$192, klen
	je		L_Decrypt_Key
	add		$2, cx 

L_Decrypt_Key:
	add		$16, ctx
	movups	(ctx), %xmm0
	aesimc	%xmm0, %xmm0
	movups	%xmm0, (ctx)
	sub		$1, cx
	jg		L_Decrypt_Key

#if CC_KERNEL
	movaps	(sp), %xmm0
#endif
#ifdef	__i386__
	xor		%eax, %eax
#endif
	jmp		L_decrypt_almost_done

#endif /* x86 based build */
#endif /* __NO_ASM__ */
