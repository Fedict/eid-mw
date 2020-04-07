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

#include <corecrypto/cc_config.h>
#if !defined(__NO_ASM__) && CCAES_INTEL
#if defined(__x86_64__)

	.text	
	.align	4


	.macro  karatsuba_reduce_to_128
    /* Karatsuba method produces t0 in %xmm3, t1 in %xmm1, t2 in %xmm0 */
    /* %xmm2 = H<<1 mod g(x) */
    movdqa      %xmm0,%xmm1
    pshufd      $$78,%xmm0,%xmm3
    pshufd      $$78,%xmm2,%xmm4
    pxor        %xmm0,%xmm3
    pxor        %xmm2,%xmm4
    pclmulqdq   $$0x0, %xmm2, %xmm0
    pclmulqdq   $$0x11, %xmm2, %xmm1
    pclmulqdq   $$0x0, %xmm4, %xmm3

    /* reduce to 128-bit in %xmm0 */
    pxor       %xmm1, %xmm3
    pxor       %xmm0, %xmm3
    movdqa     %xmm3, %xmm4
    pslldq     $$8, %xmm3
    psrldq     $$8, %xmm4
    pxor       %xmm3, %xmm0
    pxor       %xmm1, %xmm4
    pshufd     $$78, %xmm0, %xmm1
    pclmulqdq  $$0x10, L0x1c2_polynomial(%rip), %xmm0
    pxor       %xmm1, %xmm0
    pshufd     $$78, %xmm0, %xmm1
    pclmulqdq  $$0x10, L0x1c2_polynomial(%rip), %xmm0
    pxor       %xmm1, %xmm4
    pxor       %xmm4, %xmm0
    .endm

	.macro  write_Htable
    movdqu  %xmm0,$0
    pshufd  $$78,%xmm0,%xmm3
    pxor    %xmm0,%xmm3
    movdqu  %xmm3,$1
    .endm

/*
        void gcm_init(u128 Htable[16], u128 *H);

        the following equation will be used in the computation of A*H

            reflected (A)*reflected (H<<1 mod g(x)) = reflected (A*H) mod g(x)

        this function pre-computes (H^i << 1) mod g(x) for i=1:8
        it also precomputes the corresponding constants that are used in the Karatsuba algorithm. 
*/
.globl	_gcm_init

_gcm_init:

#if CC_KERNEL
    push    %rbp
    mov     %rsp, %rbp
    sub     $5*16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
#endif    
	movdqu	(%rsi),%xmm2                    // H = aes_encrypt(0);
	pshufb  L$bswap_mask(%rip), %xmm2       // reflected(H)

    // compute %xmm2 = reflected( H<<1 mod g(x) )
	pshufd	$255,%xmm2,%xmm4                
	movdqa	%xmm2,%xmm3
	psllq	$1,%xmm2
	psrlq	$63,%xmm3
	psrad	$31, %xmm4                       
	pslldq	$8,%xmm3
	por	    %xmm3,%xmm2
	pand	L0x1c2_polynomial(%rip),%xmm4
	pxor	%xmm4,%xmm2                     // reflected(H<<1 mod g(x))

	movdqa	        %xmm2,%xmm0
    write_Htable    0(%rdi), 128(%rdi)
    karatsuba_reduce_to_128
    write_Htable    16(%rdi), 144(%rdi)
    karatsuba_reduce_to_128
    write_Htable    32(%rdi), 160(%rdi)
    karatsuba_reduce_to_128
    write_Htable    48(%rdi), 176(%rdi)
    karatsuba_reduce_to_128
    write_Htable    64(%rdi), 192(%rdi)
    karatsuba_reduce_to_128
    write_Htable    80(%rdi), 208(%rdi)
    karatsuba_reduce_to_128
    write_Htable    96(%rdi), 224(%rdi)
    karatsuba_reduce_to_128
    write_Htable    112(%rdi), 240(%rdi)
#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    mov     %rbp, %rsp
    pop     %rbp
#endif    
	ret

    .globl	_gcm_gmult
    .p2align	4
_gcm_gmult:
#if CC_KERNEL
    push    %rbp
    mov     %rsp, %rbp
    sub     $5*16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
#endif    
	movdqu	(%rdi),%xmm0
	movdqu	(%rsi),%xmm2
	pshufb  L$bswap_mask(%rip), %xmm0

    karatsuba_reduce_to_128

	pshufb  L$bswap_mask(%rip), %xmm0
	movdqu	%xmm0,(%rdx)
#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    mov     %rbp, %rsp
    pop     %rbp
#endif    
	ret

.globl	_gcm_ghash

	#define	X		%rdi
	#define	Htable	%rsi
	#define	input	%rdx
	#define	len		%rcx

    /*
        compute (t0,t1,t2) = data[7]*H^1;
        t0 in xmm7, t1 in xmm6, t2 in xmm5
    */
	.macro	initial_Karatsuba
	movdqu	112(input),%xmm4
	pshufb	L$bswap_mask(%rip),%xmm4
	pshufd	$$78,%xmm4,%xmm7
    movdqa  %xmm4, %xmm5
    movdqa  %xmm4, %xmm6
	pxor	%xmm4,%xmm7
	pclmulqdq	$$0,(Htable),%xmm5
	pclmulqdq	$$17,(Htable),%xmm6
	pclmulqdq	$$0,128(Htable),%xmm7
	.endm

    /*
        update (t0,t1,t2) += data[7-i]*H^(1+i);
    */
	.macro	Karatsuba i
	movdqu	112-\i*16(input),%xmm4
	pshufb	L$bswap_mask(%rip),%xmm4
	pshufd	$78,%xmm4,%xmm3
    movdqa  %xmm4, %xmm1
    movdqa  %xmm4, %xmm2
	pxor	%xmm4, %xmm3
	pclmulqdq	$0,16*\i(Htable),%xmm1
	pclmulqdq	$17,16*\i(Htable),%xmm2
	pclmulqdq	$0,128+\i*16(Htable),%xmm3
	pxor	%xmm1,%xmm5
	pxor	%xmm2,%xmm6
	pxor	%xmm3,%xmm7
	.endm

    /*
        update (t0,t1,t2) += (data[0]+T)*H^(1+i);
        T in xmm0
    */
	.macro	final_Karatsuba i
	movdqu	112-\i*16(input),%xmm4
	pshufb	L$bswap_mask(%rip),%xmm4
	pxor	%xmm0,%xmm4
	pshufd	$78,%xmm4,%xmm3
    movdqa  %xmm4, %xmm1
    movdqa  %xmm4, %xmm2
	pxor	%xmm4, %xmm3
	pclmulqdq	$0,16*\i(Htable),%xmm1
	pclmulqdq	$17,16*\i(Htable),%xmm2
	pclmulqdq	$0,128+\i*16(Htable),%xmm3
	pxor	%xmm1,%xmm5
	pxor	%xmm2,%xmm6
	pxor	%xmm3,%xmm7
	.endm

    /*
        reduce Karatsuba intermediate 256-bit (t0,t1,t2) to 128 bit T (in xmm0)
    */
    .macro  reduce
	pxor	%xmm5,%xmm7
	pxor	%xmm6,%xmm7
    movdqa  %xmm7, %xmm0
    movdqa  %xmm7, %xmm1
	pslldq	$$8,%xmm0
	psrldq	$$8,%xmm1
    pxor	%xmm6,%xmm1
	pxor	%xmm5,%xmm0
    pshufd  $$78, L0x1c2_polynomial(%rip), %xmm2
	pclmulqdq	$$0,%xmm0,%xmm2
	pshufd		$$78,%xmm0,%xmm0
	pxor	%xmm2,%xmm0
    pshufd  $$78, L0x1c2_polynomial(%rip), %xmm2
	pclmulqdq	$$0,%xmm0,%xmm2
	pshufd		$$78,%xmm0,%xmm0
	pxor	%xmm2,%xmm0
	pxor	%xmm1,%xmm0
    .endm

    .align	4
_gcm_ghash:

#if CC_KERNEL
    push    %rbp
    mov     %rsp, %rbp
    sub     $8*16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
    movdqa  %xmm5, 5*16(%rsp)
    movdqa  %xmm6, 6*16(%rsp)
    movdqa  %xmm7, 7*16(%rsp)
#endif    
    // read T and byte swap for hash computation
	movdqu	(X), %xmm0
	pshufb	L$bswap_mask(%rip),%xmm0

    sub     $128, len
    jl      L_singles
    jmp     L_8blocks_loop

    .align  6
L_8blocks_loop:
    /*
        per 8 blocks computation loop

        (t0,t1,t2) = data[7]*H^1 + data[6]*H^2 + ... + data[1]*H^7 + (data[0]+T)*H^8;
        update T = reduce(t0,t1,t2);

    */

	initial_Karatsuba
	Karatsuba	1
	Karatsuba	2
	Karatsuba	3
	Karatsuba	4
	Karatsuba	5
	Karatsuba	6
	final_Karatsuba	7
    reduce

    add     $128, input
    sub     $128, len
    jge     L_8blocks_loop

L_singles:
    add     $(128-16), len
    jl      L_done

    /*
        (t0,t1,t2) = (data[len/16-1]+T)*H^(len/16);
        len-=16;
    */
	movdqu	(input),%xmm4
	pshufb	L$bswap_mask(%rip),%xmm4
	pxor	%xmm0,%xmm4
	movdqa	%xmm4,%xmm5
	movdqa	%xmm4,%xmm6
	pshufd	$78,%xmm4,%xmm7
	pxor	%xmm4,%xmm7
	pclmulqdq	$0,(Htable,len), %xmm5
	pclmulqdq	$17,(Htable,len), %xmm6
	pclmulqdq	$0,16*8(Htable,len), %xmm7

	add     $16, input
    sub     $16, len
    jl      L_reduce
    jmp     L_single_loop
    .align  6
L_single_loop:

    /*
        (t0,t1,t2) += (data[len/16-1])*H^(len/16);
        len-=16;
    */
	movdqu	    (input),%xmm4
	pshufb	    L$bswap_mask(%rip),%xmm4
    movdqa      %xmm4, %xmm2
	pclmulqdq	$0,(Htable,len),%xmm2
	pxor	    %xmm2,%xmm5
    movdqa      %xmm4, %xmm2
	pclmulqdq	$17,(Htable,len),%xmm2
	pxor	    %xmm2,%xmm6
	pshufd	    $78,%xmm4,%xmm3
	pxor	    %xmm4,%xmm3
	pclmulqdq	$0,128(Htable,len),%xmm3
	pxor	    %xmm3,%xmm7

	add     $16, input
    sub     $16, len
    jge     L_single_loop

L_reduce:

    /*
        update T = reduce(t0,t1,t2);
    */
    reduce

L_done:

    /*
        byte swap T and save
    */
	pshufb	L$bswap_mask(%rip),%xmm0
	movdqu	%xmm0,(X)
#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    movdqa  5*16(%rsp), %xmm5
    movdqa  6*16(%rsp), %xmm6
    movdqa  7*16(%rsp), %xmm7
    mov     %rbp, %rsp
    pop     %rbp
#endif    
    ret

	.align	4
L$bswap_mask:
.byte	15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
L0x1c2_polynomial:
.byte	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc2

#endif	// __x86_64__
#endif /* __NO_ASM__ */
