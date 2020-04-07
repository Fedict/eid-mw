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
#if CCAES_ARM
#if !defined(__arm64__) && defined(__ARM_NEON__)

    /*
            armv7 implementation of ccm-encrypt functions

            void ccm_encrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
    */



    .align  6
L_ONE:
    .quad 1,0
.Lbswap_mask:
    .byte 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
    .quad   0x00ffff, 0         // ctr_len = 2
    .quad   0x00ffffff, 0       // ctr_len = 3
    .quad   0x00ffffffff, 0     // ctr_len = 4
    .quad   0x00ffffffffff, 0     // ctr_len = 5
    .quad   0x00ffffffffffff, 0     // ctr_len = 6 
    .quad   0x00ffffffffffffff, 0     // ctr_len = 7 

#define pin     r8
#define pout    r9
#define ptag    r10
#define nblocks r11
#define KS      r4
#define pctr    r5
#define ctr_len r6

#define in      q0
#define out     q1
#define ctr     q3
#define tag     q11

#define key     q8
#define mask    q9
#define one     q10
#define Lbswap  q2


/*
    ccaes_arm_encrypt(const unsigned char *in, unsigned char *out, const ccaes_arm_encrypt_ctx cx[1]);
*/
    .extern _ccaes_arm_encrypt

    .macro  encrypt
    vst1.8  {$0}, [sp]
    mov     r0, sp          // in
    add     r1, sp, #16     // out
    mov     r2, KS          // expanded key
    bl      _ccaes_arm_encrypt
    add     r12, sp, #16
    vld1.8  {$0}, [r12]
    .endm

    .macro  update_ctr_read_in
    // ctr++;
    vadd.i64 out, ctr, one
    vbic    ctr, ctr, mask
    vand    out, out, mask
    vorr    ctr, out, ctr

    // out = reflected(ctr); tag ^= *in++;
    vld1.8  {in}, [pin]!
    vtbl.8  d2, {ctr}, d4 
    vtbl.8  d3, {ctr}, d5 
    veor    tag, tag, in
    .endm

    .macro  wrap_up_write_out
    veor    out, in, out
    vst1.8  {out}, [pout]!
    .endm

    .globl _ccm_encrypt
    .align 4
_ccm_encrypt:

/* set up often used constants in registers */
    mov     r12, sp
    push    {r4-r6,r8-r11,lr}
#if CC_KERNEL
    vstmdb  sp!, {q8-q11}
    vstmdb  sp!, {q0-q3}
#endif
    sub     sp, sp, #32
    mov     pin, r0
    mov     pout, r1
    mov     ptag, r2
    mov     nblocks, r3
    adr     lr, L_ONE
    ldmia   r12, {KS,pctr,ctr_len}

    add     r12, lr, ctr_len, lsl #4
    vld1.64 {one}, [lr :128]!
    vld1.64 {Lbswap}, [lr :128]!
    vld1.8  {tag}, [ptag]
    vld1.8  {ctr}, [pctr]
    vld1.64 {mask}, [r12 :128]

    // byte swap ctr
    vmov       in, ctr 
    vtbl.8  d6, {in}, d4 
    vtbl.8  d7, {in}, d5 

0:      // L_Main_Loop

    update_ctr_read_in
    encrypt out
    encrypt tag
    wrap_up_write_out

    subs    nblocks, nblocks, #1
    bgt     0b      // L_Main_Loop

    vmov       in, ctr 
    vtbl.8  d6, {in}, d4 
    vtbl.8  d7, {in}, d5 

    vst1.8  {ctr}, [pctr]
    vst1.8  {tag}, [ptag]

    add     sp, sp, #32
#if CC_KERNEL
    vldmia  sp!, {q0-q3}
    vldmia  sp!, {q8-q11}
#endif    
    pop     {r4-r6,r8-r11,pc}

#endif  // __armv7__ w __ARM_NEON__
#endif  // CCAES_ARM
#endif /* __NO_ASM__ */
