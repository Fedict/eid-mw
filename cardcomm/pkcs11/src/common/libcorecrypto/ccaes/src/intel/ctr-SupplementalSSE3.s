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

#if !defined(__NO_ASM__) && CCAES_INTEL && \
    defined(__x86_64__)

    /*
            SupplementalSSE3 implementation of ctr-crypt functions
    */

    .align  6
ONE:
    .quad 1,0
TWO:
    .quad 2,0
.Lbswap_mask:
    .byte 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

#define PT %rdi
#define CT %rsi
#define len %rdx
#define pctr %rcx
#define KS  %r8
#define NR %r9d

#define t3 %xmm4
#define t4 %xmm5
#define t5 %xmm6
#define ctr0 %xmm7
#define ctr1 %xmm8
#define ctr2 %xmm9
#define ctr3 %xmm10
#define ctr4 %xmm11
#define ctr5 %xmm12
#define ctr6 %xmm13
#define ctr7 %xmm14
#define ctr %xmm15


    /*
        perform i-th aes_encrypt round on ctr0-ctr7
    */
    .macro ROUND i
    movdqu  \i*16(KS), t3
    aesenc  t3, ctr0
    aesenc  t3, ctr1
    aesenc  t3, ctr2
    aesenc  t3, ctr3
    aesenc  t3, ctr4
    aesenc  t3, ctr5
    aesenc  t3, ctr6
    aesenc  t3, ctr7
    .endm


    // aes_encrypt(ctr) and increment ctr by 1
    .macro  single_block_encrypt      // input in ctr and output in t5

    movdqa  ctr, t5
    pshufb  .Lbswap_mask(%rip), t5
    paddq   ONE(%rip), ctr
    movdqu (KS), ctr0

    movdqu 16*1(KS), ctr1
    movdqu 16*2(KS), ctr2
    movdqu 16*3(KS), ctr3
    movdqu 16*4(KS), ctr4
    movdqu 16*5(KS), ctr5
    movdqu 16*6(KS), ctr6
    movdqu 16*7(KS), ctr7

    pxor    ctr0, t5

    aesenc  ctr1, t5
    aesenc  ctr2, t5
    movdqu 16*8(KS), ctr1
    movdqu 16*9(KS), ctr2
    aesenc  ctr3, t5
    aesenc  ctr4, t5
    aesenc  ctr5, t5
    aesenc  ctr6, t5
    aesenc  ctr7, t5
    aesenc  ctr1, t5
    aesenc  ctr2, t5
    movdqu  16*10(KS), t3
    cmp      $$160, NR
    jbe      1f
    movdqu 16*11(KS), ctr2
    aesenc  t3, t5
    aesenc  ctr2, t5
    movdqu  16*12(KS), t3
    cmp      $$192, NR
    jbe      1f
    movdqu  16*13(KS), ctr2
    aesenc  t3, t5
    aesenc  ctr2, t5
    movdqu  16*14(KS), t3
1:  aesenclast t3, t5
    .endm

    .globl _aes_ctr_crypt
    .align 4
_aes_ctr_crypt:

    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp   


#if CC_KERNEL

    sub    $16+12*16, %rsp
    andq   $-16, %rsp

    movdqa  %xmm4, 0*16(%rsp)
    movdqa  %xmm5, 1*16(%rsp)
    movdqa  %xmm6, 2*16(%rsp)
    movdqa  %xmm7, 3*16(%rsp)
    movdqa  %xmm8, 4*16(%rsp)
    movdqa  %xmm9, 5*16(%rsp)
    movdqa  %xmm10, 6*16(%rsp)
    movdqa  %xmm11, 7*16(%rsp)
    movdqa  %xmm12, 8*16(%rsp)
    movdqa  %xmm13, 9*16(%rsp)
    movdqa  %xmm14, 10*16(%rsp)
    movdqa  %xmm15, 11*16(%rsp)
#endif    
  
    // copy ctr/NR to registers
    movdqu  (pctr), ctr
    mov      240(KS), NR
  
    // byte swap ctr
    pshufb  .Lbswap_mask(%rip), ctr

    // if len < 128, skip to single-block processing
    sub      $128, len
    jb       L_Decrypt_Single
    jmp      Main_Decrypt_Loop
      
    .align 6
Main_Decrypt_Loop:
   
    /* interleaved 
        - updated ctr0:ctr7
        - byte swap ctr0:ctr7 (all hash computation should be in byte swapped domain)
        - xor ctr0:ctr7 with 1st expanded key
    */
      
    movdqa TWO(%rip), t4
    movdqa  .Lbswap_mask(%rip), t3
    movdqu (KS), t5
    movdqa  ctr, ctr1
    movdqa  ctr, ctr2
    paddq  ONE(%rip), ctr1
    paddq  t4, ctr2
    movdqa  ctr, ctr0
    pshufb t3, ctr0
    movdqa  ctr1, ctr3
    paddq  t4, ctr3
    pshufb t3, ctr1
    movdqa  ctr2, ctr4
    paddq  t4, ctr4
    pshufb t3, ctr2
    pxor  t5, ctr0
    movdqa  ctr3, ctr5
    paddq  t4, ctr5
    pshufb t3, ctr3
    pxor  t5, ctr1
    movdqa  ctr4, ctr6
    paddq  t4, ctr6
    pshufb t3, ctr4
    pxor  t5, ctr2
    movdqa  ctr5, ctr7
    paddq  t4, ctr7
    pshufb t3, ctr5
    pxor  t5, ctr3
    movdqa  ctr6, ctr
    paddq  t4, ctr
    pshufb t3, ctr6
    pxor  t5, ctr4
    pshufb t3, ctr7
    pxor  t5, ctr5
    pxor  t5, ctr6
    pxor  t5, ctr7

/* 9 aes_encrypt rounds for ctr0:ctr7 */
    ROUND 1
    ROUND 2
    ROUND 3
    ROUND 4
    ROUND 5
    ROUND 6
    ROUND 7
    ROUND 8
    ROUND 9

    /* if AES-128, load final expanded key and branch for aes_encrypt_last */
    movdqu  160(KS), t5
    cmp  $160, NR
    jbe  1f

    /* 2 more aes_encrypt rounds, and if AES-192 load final expanded key and branch for aes_encrypt_last */
    ROUND 10
    ROUND 11
    movdqu  192(KS), t5
    cmp  $192, NR
    jbe  1f

    /* 2 more aes_encrypt rounds, and load final expanded key to fall through to aes_encrypt_last */
    ROUND 12
    ROUND 13
    movdqu  224(KS), t5

1:

    .macro   dec_read_write i, x  
    movdqu \i*16(PT), t3
    pxor   t5, t3
    aesenclast t3, \x
    movdqu \x, \i*16(CT)
    .endm

    dec_read_write  0, ctr0
    dec_read_write  1, ctr1
    dec_read_write  2, ctr2
    dec_read_write  3, ctr3
    dec_read_write  4, ctr4
    dec_read_write  5, ctr5
    dec_read_write  6, ctr6
    dec_read_write  7, ctr7

    add     $128, CT
    add     $128, PT

    sub      $128, len
    jge       Main_Decrypt_Loop

#Here we decrypt and hash any remaining whole block

L_Decrypt_Single:
    add     $(128-16), len      // post-increment by 128 and pre-decrement by 16 of len
    jl    L_Decrypt_done

L_Decrypt_Single_Loop:
  
    // t5 = aes_encrypt(ctr); ctr++;
    single_block_encrypt

    // *pt++ = *ct++ xor aes_encrypt(ctr);
    movdqu (PT), t3     
    pxor    t3, t5
    movdqu  t5, (CT)
    addq     $16, PT
    addq     $16, CT

    sub   $16, len
    jge   L_Decrypt_Single_Loop

L_Decrypt_done:

    // byte swap ctr and save to *ptr_ctx 
    pshufb  .Lbswap_mask(%rip), ctr
    movdqu  ctr, (pctr)

#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm4
    movdqa  1*16(%rsp), %xmm5
    movdqa  2*16(%rsp), %xmm6
    movdqa  3*16(%rsp), %xmm7
    movdqa  4*16(%rsp), %xmm8
    movdqa  5*16(%rsp), %xmm9
    movdqa  6*16(%rsp), %xmm10
    movdqa  7*16(%rsp), %xmm11
    movdqa  8*16(%rsp), %xmm12
    movdqa  9*16(%rsp), %xmm13
    movdqa  10*16(%rsp), %xmm14
    movdqa  11*16(%rsp), %xmm15
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret


#endif  // __x86_64__
