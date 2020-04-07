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
#ifndef __NO_ASM__
#if CCAES_INTEL && (defined(__x86_64__))

    /*
            SupplementalSSE3 implementation of ccm-encrypt functions

            void ccm128_encrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
            void ccm192_encrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
            void ccm256_encrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
    */


    .align  6
ONE:
    .quad 1,0
.Lbswap_mask:
    .byte 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
    .quad   0x00ffff, 0         // ctr_len = 2
    .quad   0x00ffffff, 0       // ctr_len = 3
    .quad   0x00ffffffff, 0     // ctr_len = 4
    .quad   0x00ffffffffff, 0     // ctr_len = 5
    .quad   0x00ffffffffffff, 0     // ctr_len = 6 
    .quad   0x00ffffffffffffff, 0     // ctr_len = 7 

#define pin     %rdi
#define pout    %rsi
#define ptag    %rdx
#define nblocks %ecx
#define KS      %r8
#define pctr    %r9

#define in      %xmm0
#define tag     %xmm1
#define ctr     %xmm2
#define out     %xmm3
#define key     %xmm4
#define mask    %xmm5

    // aes_encrypt(ctr) and increment ctr by 1
    .macro  single_block_encrypt_head   // input in ctr and output in t
    movdqa  ctr, in
    movdqa  mask, ctr
    pandn   in, ctr                     // ctr now has part no need to update
    paddq   ONE(%rip), in
    pand    mask, in                    // in has the part of ctr_len bytes
    por     in, ctr
    movdqu  (pin), in
    movdqa  ctr, out
    pxor    in, tag                     // tag = tag ^ in;
    pshufb  .Lbswap_mask(%rip), out     // byte swap back for aes_encrypt
    movdqu  (KS), key
    pxor    key, tag
    pxor    key, out
    movdqu  16*1(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*2(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*3(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*4(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*5(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*6(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*7(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*8(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*9(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*10(KS), key
    .endm

    .macro  single_block_encrypt_tail
    aesenclast  key, tag
    aesenclast  key, out
    pxor        in, out
    movdqu  out, (pout)
    add     $$16, pin
    add     $$16, pout
    .endm

    .macro  single_block_encrypt_128
    single_block_encrypt_head
    single_block_encrypt_tail
    .endm

    .macro  single_block_encrypt_192
    single_block_encrypt_head
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*11(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*12(KS), key
    single_block_encrypt_tail
    .endm

    .macro  single_block_encrypt_256
    single_block_encrypt_head
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*11(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*12(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*13(KS), key
    aesenc  key, tag
    aesenc  key, out
    movdqu  16*14(KS), key
    single_block_encrypt_tail
    .endm

    .globl _ccm128_encrypt
    .align 4
_ccm128_encrypt:

    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp   

#if CC_KERNEL
    sub    $16+6*16, %rsp
    andq   $-16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
    movdqa  %xmm5, 5*16(%rsp)
#endif    

    movl    16(%rbp), %eax
    shll    $4, %eax
    leaq    ONE(%rip), %r10
    movdqa  (%r10, %rax), mask 
  
    // copy ctr/NR to registers
    movdqu  (pctr), ctr
    movdqu  (ptag), tag
  
    // byte swap ctr
    pshufb  .Lbswap_mask(%rip), ctr

0:      // Main_Loop:

    single_block_encrypt_128
    sub      $1, nblocks
    jg       0b     // Main_Loop

    // byte swap ctr and save to *ptr_ctx 
    pshufb  .Lbswap_mask(%rip), ctr
    movdqu  ctr, (pctr)
    movdqu  tag, (ptag)

#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    movdqa  5*16(%rsp), %xmm5
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret

    .globl _ccm192_encrypt
    .align 4
_ccm192_encrypt:

    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp   

#if CC_KERNEL
    sub    $16+6*16, %rsp
    andq   $-16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
    movdqa  %xmm5, 5*16(%rsp)
#endif    

    movl    16(%rbp), %eax
    shll    $4, %eax
    leaq    ONE(%rip), %r10
    movdqa  (%r10, %rax), mask 
  
    // copy ctr/NR to registers
    movdqu  (pctr), ctr
    movdqu  (ptag), tag
  
    // byte swap ctr
    pshufb  .Lbswap_mask(%rip), ctr

0:      // Main_Loop:

    single_block_encrypt_192
    sub      $1, nblocks
    jg       0b     // Main_Loop

    // byte swap ctr and save to *ptr_ctx 
    pshufb  .Lbswap_mask(%rip), ctr
    movdqu  ctr, (pctr)
    movdqu  tag, (ptag)

#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    movdqa  5*16(%rsp), %xmm5
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret



    .globl _ccm256_encrypt
    .align 4
_ccm256_encrypt:

    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp   

#if CC_KERNEL
    sub    $16+6*16, %rsp
    andq   $-16, %rsp
    movdqa  %xmm0, 0*16(%rsp)
    movdqa  %xmm1, 1*16(%rsp)
    movdqa  %xmm2, 2*16(%rsp)
    movdqa  %xmm3, 3*16(%rsp)
    movdqa  %xmm4, 4*16(%rsp)
    movdqa  %xmm5, 5*16(%rsp)
#endif    

    movl    16(%rbp), %eax
    shll    $4, %eax
    leaq    ONE(%rip), %r10
    movdqa  (%r10, %rax), mask 
  
    // copy ctr/NR to registers
    movdqu  (pctr), ctr
    movdqu  (ptag), tag
  
    // byte swap ctr
    pshufb  .Lbswap_mask(%rip), ctr

0:      // Main_Loop:

    single_block_encrypt_256
    sub      $1, nblocks
    jg       0b     // Main_Loop

    // byte swap ctr and save to *ptr_ctx 
    pshufb  .Lbswap_mask(%rip), ctr
    movdqu  ctr, (pctr)
    movdqu  tag, (ptag)

#if CC_KERNEL
    movdqa  0*16(%rsp), %xmm0
    movdqa  1*16(%rsp), %xmm1
    movdqa  2*16(%rsp), %xmm2
    movdqa  3*16(%rsp), %xmm3
    movdqa  4*16(%rsp), %xmm4
    movdqa  5*16(%rsp), %xmm5
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret

#endif  // __x86_64__
#endif /* __NO_ASM__ */
