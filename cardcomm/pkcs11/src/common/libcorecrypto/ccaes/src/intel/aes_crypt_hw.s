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


/* 	This files defines _aes_encrypt_hw and _aes_decrypt_hw --- Intel Westmere HW AES-based implementation
	of _aes_encrypt and _aes_decrypt. 

	These 2 functions SHOULD BE entried ONLY after the AES HW is verified to be available. 
	They SHOULD NOT be called without AES HW detection. It might cause xnu to crash.

	The AES HW is detected 1st thing in 
		_aes_encrypt (EncryptDecrypt.s) 
		_aes_decrypt (EncryptDecrypt.s)
	and, if AES HW is detected, branch without link (ie, jump) to the functions here.

	The implementation here follows the examples in an Intel White Paper
	"Intel Advanced Encryption Standard (AES) Instruction Set" Rev.2 01

	Note: Rev. 03 Final 2010 01 26 is available. Looks like some code change from Rev.2 01

*/

#include <corecrypto/cc_config.h>

#if !defined(__NO_ASM__) && CCAES_INTEL && \
    (defined __i386__ || defined __x86_64__)

    .text
    .align 4,0x90
.globl	_aes_encrypt_hw
_aes_encrypt_hw:

.globl _vng_aes_encrypt_aesni
_vng_aes_encrypt_aesni:


#if	defined	__i386__	
	movl	4(%esp), %eax	// in
	movl	12(%esp), %edx	// ctx
	movl	8(%esp), %ecx	// out

	#define	LOCAL_SIZE	(12+16+16)		// 16-byte align (-4 for return address) + 16 (xmm0) + 16 (xmm1)
	#define	in		%eax
	#define	ctx		%edx
	#define	out		%ecx
	#define	r13		%esp

#else		// x86_64

	#define	LOCAL_SIZE	(8+16+16)		// 16-byte align (-8 for return address) + 16 (xmm0) + 16 (xmm1)
	#define	in			%rdi
	#define	ctx			%rdx
	#define	out			%rsi
	#define	r13			%rsp

#endif		// i386 or x86_64

#if CC_KERNEL
	sub		$LOCAL_SIZE, r13
	movaps	%xmm0, (r13)
#endif
	movups	(in), %xmm0

	// key length identification
	movl	240(ctx), %eax			// key length
	cmp		$160, %eax
	je		L_AES_128
	cmp		$192, %eax
	je		L_AES_192
	cmp		$224, %eax
	je		L_AES_256
	mov		$-1, %eax					// return ERROR
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret

L_AES_128:
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		0f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	(ctx), %xmm0
	aesenc	16(ctx), %xmm0
	aesenc	32(ctx), %xmm0
	aesenc	48(ctx), %xmm0
	aesenc	64(ctx), %xmm0
	aesenc	80(ctx), %xmm0
	aesenc	96(ctx), %xmm0
	aesenc	112(ctx), %xmm0
	aesenc	128(ctx), %xmm0
	aesenc	144(ctx), %xmm0
	aesenclast	160(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
0:										// special case expanded key is not 16-byte aligned	
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	160(ctx), %xmm1
	aesenclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1
	add		$LOCAL_SIZE, r13
#endif
	ret

L_AES_192:
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		0f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	(ctx), %xmm0
	aesenc	16(ctx), %xmm0
	aesenc	32(ctx), %xmm0
	aesenc	48(ctx), %xmm0
	aesenc	64(ctx), %xmm0
	aesenc	80(ctx), %xmm0
	aesenc	96(ctx), %xmm0
	aesenc	112(ctx), %xmm0
	aesenc	128(ctx), %xmm0
	aesenc	144(ctx), %xmm0
	aesenc	160(ctx), %xmm0
	aesenc	176(ctx), %xmm0
	aesenclast	192(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
0:										// special case expanded key is not 16-byte aligned	
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	160(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	176(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	192(ctx), %xmm1
	aesenclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1
	add		$LOCAL_SIZE, r13
#endif
	ret

L_AES_256:
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		0f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	(ctx), %xmm0
	aesenc	16(ctx), %xmm0
	aesenc	32(ctx), %xmm0
	aesenc	48(ctx), %xmm0
	aesenc	64(ctx), %xmm0
	aesenc	80(ctx), %xmm0
	aesenc	96(ctx), %xmm0
	aesenc	112(ctx), %xmm0
	aesenc	128(ctx), %xmm0
	aesenc	144(ctx), %xmm0
	aesenc	160(ctx), %xmm0
	aesenc	176(ctx), %xmm0
	aesenc	192(ctx), %xmm0
	aesenc	208(ctx), %xmm0
	aesenclast	224(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
0:										// special case expanded key is not 16-byte aligned	
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	160(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	176(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	192(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	208(ctx), %xmm1
	aesenc	%xmm1, %xmm0
	movups	224(ctx), %xmm1
	aesenclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1
	add		$LOCAL_SIZE, r13
#endif
	ret


    .text
    .align 4,0x90
.globl	_aes_decrypt_hw
_aes_decrypt_hw:

.globl _vng_aes_decrypt_aesni
_vng_aes_decrypt_aesni:

#if	defined	__i386__	
	movl	4(%esp), %eax	// in
	movl	12(%esp), %edx	// ctx
	movl	8(%esp), %ecx	// out

#endif

#if CC_KERNEL
	sub		$LOCAL_SIZE, r13
	movaps	%xmm0, (r13)
#endif
	movups	(in), %xmm0

	// key length identification
	movl	240(ctx), %eax			// key length
	cmp		$160, %eax
	je		0f						// AES-128
	cmp		$192, %eax
	je		1f						// AES-192
	cmp		$224, %eax
	je		2f						// AES-256
	mov		$-1, %eax				// return ERROR
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret

0:									// AES-128
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		9f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	160(ctx), %xmm0
	aesdec	144(ctx), %xmm0
	aesdec	128(ctx), %xmm0
	aesdec	112(ctx), %xmm0
	aesdec	96(ctx), %xmm0
	aesdec	80(ctx), %xmm0
	aesdec	64(ctx), %xmm0
	aesdec	48(ctx), %xmm0
	aesdec	32(ctx), %xmm0
	aesdec	16(ctx), %xmm0
	aesdeclast	(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
9:										// AES-128 Decrypt : special case expanded key is not 16-byte aligned 
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	160(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	(ctx), %xmm1
	aesdeclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1	
	add		$LOCAL_SIZE, r13
#endif
	ret

1:								// AES-192
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		9f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	192(ctx), %xmm0
	aesdec	176(ctx), %xmm0
	aesdec	160(ctx), %xmm0
	aesdec	144(ctx), %xmm0
	aesdec	128(ctx), %xmm0
	aesdec	112(ctx), %xmm0
	aesdec	96(ctx), %xmm0
	aesdec	80(ctx), %xmm0
	aesdec	64(ctx), %xmm0
	aesdec	48(ctx), %xmm0
	aesdec	32(ctx), %xmm0
	aesdec	16(ctx), %xmm0
	aesdeclast	(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
9:										// AES-192 Decrypt : special case expanded key is not 16-byte aligned 
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	192(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	176(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	160(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	(ctx), %xmm1
	aesdeclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1	
	add		$LOCAL_SIZE, r13
#endif
	ret

2:							// AES-256
	testb	$15, %dl					// check whether expanded key is 16-byte aligned
	jne		9f							// if not 16-byte aligned, aesenc xmm, m128 won't work	
	pxor	224(ctx), %xmm0
	aesdec	208(ctx), %xmm0
	aesdec	192(ctx), %xmm0
	aesdec	176(ctx), %xmm0
	aesdec	160(ctx), %xmm0
	aesdec	144(ctx), %xmm0
	aesdec	128(ctx), %xmm0
	aesdec	112(ctx), %xmm0
	aesdec	96(ctx), %xmm0
	aesdec	80(ctx), %xmm0
	aesdec	64(ctx), %xmm0
	aesdec	48(ctx), %xmm0
	aesdec	32(ctx), %xmm0
	aesdec	16(ctx), %xmm0
	aesdeclast	(ctx), %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	add		$LOCAL_SIZE, r13
#endif
	ret
9:										// AES-256 Decrypt : special case expanded key is not 16-byte aligned 
#if CC_KERNEL
	movaps	%xmm1, 16(r13)				// save xmm1 into stack
#endif
	movups	224(ctx), %xmm1
	pxor	%xmm1, %xmm0
	movups	208(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	192(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	176(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	160(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	144(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	128(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	112(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	96(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	80(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	64(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	48(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	32(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	16(ctx), %xmm1
	aesdec	%xmm1, %xmm0
	movups	(ctx), %xmm1
	aesdeclast	%xmm1, %xmm0
	xorl	%eax, %eax
	movups	%xmm0, (out)
#if CC_KERNEL
	movaps	(r13), %xmm0	
	movaps	16(r13), %xmm1	
	add		$LOCAL_SIZE, r13
#endif
	ret

#endif /* x86 based build */
