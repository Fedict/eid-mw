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

    .align  6
ONE:
    .quad 1,0
TWO:
    .quad 2,0
.Lbswap_mask:
    .byte 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
L_poly:
    .quad 0xc200000000000000, 0x1

/*

    Encrypt and Authenticate vectorized computation engine

    void gcmEncrypt(uint8_t* PT, uint8_t* CT, GCM128_CONTEXT *Gctx, uint64_t len, void Htbl, void *KS);

    PT : pointer to input plain text
    CT : pointer to output cipher text
    Gctx : pointer to AES-GCM context
            16(Gctx)  ->  T         message authentication code/tag
            32(Gctx)  ->  ctr       GCM counter
    len : message length
    Htbl : extended table (of H = aesenc(0) based on the key)
    KS  : pointer to AES expanded key structures, 240(KS) -> number of AES rounds : 160/192/224

    aes-gcm encrypt operation can be described as follows (omitting implementation detail for byte swaping):
    0. Given 128-bit T and 128-bit ctr, an input plain text of len bytes
    1. while (len>=16) {
            *cipher = *plain++ xor encrypt(ctr++);
            T = (T + *cipher++) * H;
        }

    to vectorize, note that 
        - multiple encrypt blocks can be pre-computed before adding to plain text blocks to derive ciphers
        - multiple block hash update can be vectorized (e.g. 8 blocks) as follows
            T = hash(T,Vcipher) = cipher[7]*H^1 + cipher[6]*H^2 + ... + cipher[1]*H^7 + (T + cipher[0])*H^8;

    the step 1 can be vectorized and scheduled as follows

                                                                Vctr = aes_encrypt(ctr0:7); ctr+=8;
        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);           Vctr = aes_encrypt(ctr0:7); ctr+=8;
        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);           Vctr = aes_encrypt(ctr0:7); ctr+=8;
        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);           Vctr = aes_encrypt(ctr0:7); ctr+=8;
        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);           Vctr = aes_encrypt(ctr0:7); ctr+=8;
        ...
        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);

    the key implementation issue is to maximize the parellelism of the operation

        T = hash(T, *Vcipher++ = *Vplain++ xor Vctr);           Vctr = aes_encrypt(ctr0:7); ctr+=8;

    That is, 

        - derive ciphers from plains xor Vctr (which is aes_encrypt(ctr) from previous iteration)
        - update Vctr = aes_encrypt(ctr) for next iteration
        - hash update T = hash(T, Vcipher);

*/

    .globl _gcmEncrypt_avx1
    .align 4
_gcmEncrypt_avx1:

#define PT %rdi
#define CT %rsi
#define Gctx %rdx
#define len %rcx
#define Htbl %r8
#define KS %r9
#define NR %r10d

#define T %xmm0
#define t0 %xmm1
#define t1 %xmm2
#define t2 %xmm3
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

/* -----------------------------------------------------------------------------------

    AES-GCM encrypt macro definitions

    ROUND i : 
    KARATSUBA i, x :
    ROUNDMUL i : 
    encrypt8_final :
    reduce_to_128 :
    single_block_encrypt :

    -------------------------------------------------------------------------------- */

    /*
        perform i-th aes_encrypt round on ctr0-ctr7
    */
    .macro ROUND i
    vmovdqu  \i*16(KS), t3
    vaesenc  t3, ctr0, ctr0
    vaesenc  t3, ctr1, ctr1
    vaesenc  t3, ctr2, ctr2
    vaesenc  t3, ctr3, ctr3
    vaesenc  t3, ctr4, ctr4
    vaesenc  t3, ctr5, ctr5
    vaesenc  t3, ctr6, ctr6
    vaesenc  t3, ctr7, ctr7
    .endm

    /*
        Karatsuba Multiplication Method
            Ah:Al   *   Hh:Hl

            Ah*Hh            Al*Hl
               (Ah+Al)*(Hh+Hl)
                    Ah*Hh
                    Al*Hl

            \i*16(%rsp)     : Ah:Al 
            \i*16(Htbl)     : Hh:Hl
            128+\i*16(Htbl) : (Hh+Hl)

            t1 : accumulated Ah*Hh
            t2 : accumulated Al*Hl
            t0 : accumulated (Ah+Al)*(Hh+Hl)
    */
    .macro KARATSUBA i, x
    vpshufd     $78, \x, t5                       // t5 = Al:Ah
    vpclmulqdq  $0x11, \i*16(Htbl), \x, t3        // t3 = Ah*Hh
    vpclmulqdq  $0x00, \i*16(Htbl), \x, t4        // t4 = Al*Hl
    vpxor       \x, t5, t5                        // t5 = (Ah+Al):(Ah*Al)
    vpclmulqdq  $0x00, 128+\i*16(Htbl), t5, t5    // t5 = (Ah+Al)*(Hh+Hl)
    vpxor       t3, t1, t1                        // t1 = updated accumulated Ah*Hh
    vpxor       t4, t2, t2                        // t2 = updated accumulated Al*Hl 
    vpxor       t5, t0, t0                        // t0 = updated accumulated (Ah+Al)*(Hh+Hl)
    .endm

    /*
        combined i-th aes_encrypt round (of 8 counters) and Karatsuba hash update

        Note : T has been saved (in stack) before all invoke of this macro, and will be restored afterwards
                T is used as a temp register to reduce execution pipe stalls
    */
    .macro ROUNDMUL i
    vmovdqu  \i*16(KS), t3                                      // aes encrypt expanded key for i-th round
                vmovdqu  \i*16(%rsp), t5                        // ctr[7-i] = Ah:Al
    vaesenc  t3, ctr0, ctr0
    vaesenc  t3, ctr1, ctr1
                vpclmulqdq  $0x11, \i*16(Htbl), t5, T           // Ah*Hh
                vpshufd  $78, t5, t4                            // Al:Ah
    vaesenc  t3, ctr2, ctr2
    vaesenc  t3, ctr3, ctr3
                vpxor    T, t1, t1                              // t1 = updated accumulated Ah*Hh
                vpclmulqdq  $0x00, \i*16(Htbl), t5, T           // Al*Hl
    vaesenc  t3, ctr4, ctr4
                vpxor    t5, t4, t4                             // (Ah+Al)
                vpclmulqdq  $0x00, 128+\i*16(Htbl), t4, t5      // (Ah+Al)*(Hh+Hl)
    vaesenc  t3, ctr5, ctr5
    vaesenc  t3, ctr6, ctr6
                vpxor       T, t2, t2                           // t2 = updated accumulated Al*Hl
    vaesenc  t3, ctr7, ctr7
                vpxor       t5, t0, t0                          // t0 = updated accumulated (Ah+Al)*(Hh+Hl)
    .endm

    /* read i-th plain text block (it can be NOT 16-byte aligned), xor with ctr, finished up aes_encrypt */
    .macro  enc_final i,x
    vmovdqu \i*16(PT), t3
    vpxor       t3, t5, t3
    vaesenclast t3, \x, \x
    .endm

    /* save output ciphers and byte swap ctr0:ctr7 */  
    .macro  save_swap i, x
    vmovdqu  \x, \i*16(CT)
    vpshufb  t3, \x, \x
    .endm

    .macro  encrypt8_final
    /*
        for 8 counters ctr0:ctr7
            - finished up final aes_encrypt
            - xor with input plain text, save to output ciphers
            - byte swap ctr0:ctr7 (all hash computation are in the byte swapped domain)
            - advanced CT/PT by 128
    */
    enc_final   0, ctr0
    enc_final   1, ctr1
    enc_final   2, ctr2
    enc_final   3, ctr3
    enc_final   4, ctr4
    enc_final   5, ctr5
    enc_final   6, ctr6
    enc_final   7, ctr7
    vmovdqu   .Lbswap_mask(%rip), t3
    save_swap   0, ctr0
    save_swap   1, ctr1
    save_swap   2, ctr2
    save_swap   3, ctr3
    save_swap   4, ctr4
    save_swap   5, ctr5
    save_swap   6, ctr6
    save_swap   7, ctr7
    add     $$128, PT
    add     $$128, CT
    .endm

    // reduce 256-bit (t0,t1,t2) to a new 128-bit tag (in T)
    .macro  reduce_to_128
    vpxor       t1, t0, t0
    vpxor       t2, t0, t0
    vpsrldq     $$8, t0, T
    vpslldq     $$8, t0, t5
    vpxor       T, t1, t4
    vpxor       t5, t2, T
    vpshufd     $$78, T, t1
    vpclmulqdq  $$0x00, L_poly(%rip), T, T
    vpxor       T, t1, T
    vpshufd     $$78, T, t1
    vpclmulqdq  $$0x00, L_poly(%rip), T, T
    vpxor       T, t1, T
    vpxor       t4, T, T
    .endm

    // aes_encrypt(ctr) and increment ctr by 1
    .macro  single_block_encrypt      // input in ctr and output in t5
    vpshufb  .Lbswap_mask(%rip), ctr, t5
    vpaddd   ONE(%rip), ctr, ctr
    vmovdqu (KS), ctr0
    vmovdqu 16*1(KS), ctr1
    vmovdqu 16*2(KS), ctr2
    vmovdqu 16*3(KS), ctr3
    vmovdqu 16*4(KS), ctr4
    vmovdqu 16*5(KS), ctr5
    vmovdqu 16*6(KS), ctr6
    vmovdqu 16*7(KS), ctr7

    vpxor    ctr0, t5, t5
    vaesenc  ctr1, t5, t5
    vaesenc  ctr2, t5, t5
    vmovdqu 16*8(KS), ctr1
    vmovdqu 16*9(KS), ctr2
    vaesenc  ctr3, t5, t5
    vaesenc  ctr4, t5, t5
    vaesenc  ctr5, t5, t5
    vaesenc  ctr6, t5, t5
    vaesenc  ctr7, t5, t5
    vaesenc  ctr1, t5, t5
    vaesenc  ctr2, t5, t5
    vmovdqu  16*10(KS), t3
    cmp      $$160, NR
    jbe      1f
    vmovdqu 16*11(KS), ctr2
    vaesenc  t3, t5, t5
    vaesenc  ctr2, t5, t5
    vmovdqu  16*12(KS), t3
    cmp      $$192, NR
    jbe      1f
    vmovdqu  16*13(KS), ctr2
    vaesenc  t3, t5, t5
    vaesenc  ctr2, t5, t5
    vmovdqu  16*14(KS), t3
1:  vaesenclast t3, t5, t5
    .endm

/* -------------  End of encrypt macro definitions ------------------ */


    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp   

    sub    $128+16*16, %rsp
    andq   $-16, %rsp

#if CC_KERNEL
    vmovdqa  %xmm0, 8*16(%rsp)
    vmovdqa  %xmm1, 9*16(%rsp)
    vmovdqa  %xmm2, 10*16(%rsp)
    vmovdqa  %xmm3, 11*16(%rsp)
    vmovdqa  %xmm4, 12*16(%rsp)
    vmovdqa  %xmm5, 13*16(%rsp)
    vmovdqa  %xmm6, 14*16(%rsp)
    vmovdqa  %xmm7, 15*16(%rsp)
    vmovdqa  %xmm8, 16*16(%rsp)
    vmovdqa  %xmm9, 17*16(%rsp)
    vmovdqa  %xmm10, 18*16(%rsp)
    vmovdqa  %xmm11, 19*16(%rsp)
    vmovdqa  %xmm12, 20*16(%rsp)
    vmovdqa  %xmm13, 21*16(%rsp)
    vmovdqa  %xmm14, 22*16(%rsp)
    vmovdqa  %xmm15, 23*16(%rsp)
#endif    


    /* read counter/Tag/NR into registers */
    vmovdqu  32(Gctx), ctr
    vmovdqu  16(Gctx), T
    mov      240(KS), NR
  
    /* byte swap counter/Tag for hash computation */ 
    vpshufb  .Lbswap_mask(%rip), ctr, ctr
    vpshufb  .Lbswap_mask(%rip), T, T
 
    /* if less than 128 bytes, branch to deal with single vector 16-byte */ 
    cmp      $128, len
    jb       L_Encrypt_Singles

    /* ----------------------------------------------------------------
        aes_encrypt(8 counters) for the first 8 blocks 
       ----------------------------------------------------------------  */   

    /* interleaved 
        - updated ctr0:ctr7
        - byte swap ctr0:ctr7 (all hash computation should be in byte swapped domain)
        - xor ctr0:ctr7 with 1st expanded key
    */
    vmovdqa  TWO(%rip), t4
    vmovdqa  .Lbswap_mask(%rip), t5
    vmovdqa  (KS), t3
    vpaddd   ONE(%rip), ctr, ctr1
    vpaddd   t4, ctr, ctr2
    vpshufb  t5, ctr, ctr0
    vpaddd   t4, ctr1, ctr3
    vpshufb  t5, ctr1, ctr1
    vpxor    t3, ctr0, ctr0
    vpaddd   t4, ctr2, ctr4
    vpshufb  t5, ctr2, ctr2
    vpxor    t3, ctr1, ctr1
    vpaddd   t4, ctr3, ctr5
    vpshufb  t5, ctr3, ctr3
    vpxor    t3, ctr2, ctr2
    vpaddd   t4, ctr4, ctr6
    vpshufb  t5, ctr4, ctr4
    vpxor    t3, ctr3, ctr3
    vpaddd   t4, ctr5, ctr7
    vpshufb  t5, ctr5, ctr5
    vpxor    t3, ctr4, ctr4
    vpaddd   t4, ctr6, ctr
    vpshufb  t5, ctr6, ctr6
    vpshufb  t5, ctr7, ctr7
    vpxor    t3, ctr5, ctr5
    vpxor    t3, ctr6, ctr6
    vpxor    t3, ctr7, ctr7

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
    vmovdqu  160(KS), t5
    cmp  $160, NR
    jbe  1f
 
    /* 2 more aes_encrypt rounds, and if AES-192 load final expanded key and branch for aes_encrypt_last */ 
    ROUND 10
    ROUND 11
    vmovdqu  192(KS), t5
    cmp  $192, NR
    jbe  1f

    /* 2 more aes_encrypt rounds, and load final expanded key to fall through to aes_encrypt_last */ 
    ROUND 12
    ROUND 13
    vmovdqu  224(KS), t5
  
1:

    /*
        for 8 counters ctr0:ctr7
            - finished up final aes_encrypt
            - xor with input plain text, save to output ciphers
            - byte swap ctr0:ctr7 (all hash computation are in the byte swapped domain)
            - advanced CT/PT by 128
    */
    encrypt8_final

    sub     $128+128, len                   // 1st 128-byte and pre-decrement len by another 128 bytes
    jb      .End_Main_Encrypt_Loop          // if no more than another 128 bytes input plain text, branch to End of 8 vectors

    jmp     Main_Encrypt_Loop
   
# Encrypt 8 blocks each time while hashing previous 8 blocks
    .align 6
Main_Encrypt_Loop:

    // xor T and ctr0 and save to stack so we can use T as an extra scratch register
    vpxor   ctr0, T, T

    /* interleaved 
        - saved ctr0:ctr7 to stack, which will be used in ROUNDMUL for hash computation
        - updated ctr0:ctr7
        - byte swap ctr0:ctr7 (all hash computation should be in byte swapped domain)
        - xor ctr0:ctr7 with 1st expanded key
        - ctr7 * H^1 (intermediate results in t0/t1/t2)
    */
        vmovdqa  TWO(%rip), t4
    vmovdqa  T, 7*16(%rsp)
        vmovdqu  (KS), T
    vmovdqa  ctr1, 6*16(%rsp)
        vpaddd   ONE(%rip), ctr, ctr1
    vmovdqa  ctr2, 5*16(%rsp)
        vpaddd   t4, ctr, ctr2
        vpshufb  t3, ctr, ctr0
    vmovdqa  ctr3, 4*16(%rsp)
        vpaddd   t4, ctr1, ctr3
        vpshufb  t3, ctr1, ctr1
    vmovdqa  ctr4, 3*16(%rsp)
        vpaddd   t4, ctr2, ctr4
        vpshufb  t3, ctr2, ctr2
        vpxor    T, ctr0, ctr0
    vmovdqa  ctr5, 2*16(%rsp)
        vpaddd   t4, ctr3, ctr5
        vpshufb  t3, ctr3, ctr3
        vpxor    T, ctr1, ctr1
    vmovdqa  ctr6, 1*16(%rsp)
        vpaddd   t4, ctr4, ctr6
        vpshufb  t3, ctr4, ctr4
        vpxor    T, ctr2, ctr2
    vmovdqa  ctr7, t5
        vpaddd   t4, ctr5, ctr7
        vpshufb  t3, ctr5, ctr5
        vpxor    T, ctr3, ctr3
        vpaddd   t4, ctr6, ctr
    vmovdqu     16*0(Htbl), t4
        vpshufb  t3, ctr6, ctr6
    vpclmulqdq  $0x11, t4, t5, t1
        vpxor    T, ctr4, ctr4
    vpclmulqdq  $0x00, t4, t5, t2      
    vmovdqu     128+0*16(Htbl), t4
        vpshufb  t3, ctr7, ctr7
    vpshufd     $78, t5, t3
        vpxor    T, ctr5, ctr5
    vpxor       t5, t3, t5
        vpxor    T, ctr6, ctr6
        vpxor    T, ctr7, ctr7
    vpclmulqdq  $0x00, t4, t5, t0

    /* 
        encrypt : 7 rounds aes_encrypt(ctr0:ctr7) 
        hashing : (t0/t1/t2) += ctr6*H^2 + ctr5*H^3 + ... + ctr1*H^7 + (ctr0+T)*H^8
        note : ctr0+T is already processed ahead and stored in 7*16(%rsp)
    */  
    ROUNDMUL 1
    ROUNDMUL 2
    ROUNDMUL 3
    ROUNDMUL 4
    ROUNDMUL 5
    ROUNDMUL 6
    ROUNDMUL 7

    /* 
        2 aes_encrypt rounds + reduce 256-bit t0/t1/t2 to 128-bit tag T
    */  
            vmovdqu  128(KS), t3
    vpxor    t1, t0, t0
            vaesenc  t3, ctr0, ctr0
    vpxor    t2, t0, t0
            vaesenc  t3, ctr1, ctr1
            vaesenc  t3, ctr2, ctr2
    vpsrldq  $8, t0, T
    vpslldq  $8, t0, t0
            vaesenc  t3, ctr3, ctr3
            vaesenc  t3, ctr4, ctr4
    vpxor    T, t1, t4
    vpxor    t0, t2, T
            vaesenc  t3, ctr5, ctr5
    vpclmulqdq  $0x00, L_poly(%rip), T, t1
    vpshufd   $78, T, T
            vaesenc  t3, ctr6, ctr6
            vaesenc  t3, ctr7, ctr7
            vmovdqu  144(KS), t3
    vpxor       T, t1, T
            vaesenc  t3, ctr0, ctr0
            vaesenc  t3, ctr1, ctr1
    vpclmulqdq  $0x00, L_poly(%rip), T, t1
    vpshufd   $78, T, T
            vaesenc  t3, ctr2, ctr2
            vaesenc  t3, ctr3, ctr3
            vaesenc  t3, ctr4, ctr4
            vaesenc  t3, ctr5, ctr5
            vaesenc  t3, ctr6, ctr6
    vpxor   T, t1, T
            vaesenc  t3, ctr7, ctr7
    vpxor   t4, T, T

    // branch if aes-128
    vmovdqu  160(KS), t5
    cmp      $160, NR
    jbe  1f
      
    ROUND 10
    ROUND 11

    // branch if aes-192
    vmovdqu  192(KS), t5
    cmp      $192, NR       
    jbe  1f
     
    ROUND 12
    ROUND 13

    // aes-256 fall through 
    vmovdqu  224(KS), t5

1:

    encrypt8_final  

    sub      $128, len
    jge     Main_Encrypt_Loop

.End_Main_Encrypt_Loop:

    add      $128, len          // post-increment len by 128
    vpxor    T, ctr0, ctr0      // this will free T and reuse KARATSUBA for (ctr0+T)*H^8

    // initialize (t0,t1,t2) = ctr7*H^1
    vpshufd     $78, ctr7, t5
    vpclmulqdq  $0x11, 16*0(Htbl), ctr7, t1
    vpclmulqdq  $0x00, 16*0(Htbl), ctr7, t2      
    vpxor       ctr7, t5, t5
    vpclmulqdq  $0x00, 128+0*16(Htbl), t5, t0
  
    // update (t0,t1,t2) += ctr6*H^2 + ctr5*H^3 + ... + ctr0*H^8; 
    KARATSUBA 1, ctr6
    KARATSUBA 2, ctr5
    KARATSUBA 3, ctr4      
    KARATSUBA 4, ctr3
    KARATSUBA 5, ctr2
    KARATSUBA 6, ctr1
    KARATSUBA 7, ctr0

    // reduce 256-bit (t0,t1,t2) to a new 128-bit tag (in T)
    reduce_to_128

    /* encrypt any remaining whole block */

L_Encrypt_Singles:

    // if len < 16, nothing to do */
    sub  $16, len
    jb    L_Encrypt_done

    // aes_encrypt(ctr) and increment ctr by 1, output at t5
    single_block_encrypt

    // cipher = plain + aes_encrypt(ctr) 
    vmovdqu  (PT), t3
    vpxor    t3, t5, t5
    vmovdqu  t5, (CT)
    addq     $16, CT
    addq     $16, PT
 
    // byte swap ctr and compute (t0,t1,t2) = (T+ctr)*H^1 
    vpshufb  .Lbswap_mask(%rip), t5, t5
    vpxor    t5, T, t5
    vpshufd     $78, t5, t3
    vpclmulqdq  $0x11, (Htbl, len), t5, t1
    vpxor       t5, t3, t3
    vpclmulqdq  $0x00, (Htbl, len), t5, t2
    vpclmulqdq  $0x00, 128(Htbl, len), t3, t0

    sub   $16, len
    jb    L_Encrypt_final_reduce
    jmp   L_Encrypt_Singles_Loop

    .align  6
L_Encrypt_Singles_Loop:

    // aes_encrypt(ctr) and increment ctr by 1, output at t5
    single_block_encrypt

    // cipher = plain + aes_encrypt(ctr) 
    vmovdqu  (PT), t3
    vpxor    t3, t5, t5
    vmovdqu  t5, (CT)
    addq     $16, CT
    addq     $16, PT
 
    // byte swap ctr and update (t0,t1,t2) += (T+ctr)*H^1 
    vpshufb  .Lbswap_mask(%rip), t5, t5   
    vpshufd     $78, t5, T
    vpclmulqdq  $0x11, (Htbl, len), t5, t3
    vpxor       t5, T, T
    vpclmulqdq  $0x00, (Htbl, len), t5, t4
    vpclmulqdq  $0x00, 128(Htbl, len), T, t5
    vpxor       t3, t1, t1
    vpxor       t4, t2, t2
    vpxor       t5, t0, t0

    sub   $16, len
    jge   L_Encrypt_Singles_Loop

L_Encrypt_final_reduce:

    reduce_to_128
   
L_Encrypt_done:

    // byte swap T and ctr and save to GCM ctx
    vpshufb  .Lbswap_mask(%rip), T, T
    vpshufb  .Lbswap_mask(%rip), ctr, ctr
    vmovdqu  T, 16(Gctx)
    vmovdqu  ctr, 32(Gctx)

#if CC_KERNEL
    vmovdqa  8*16(%rsp), %xmm0
    vmovdqa  9*16(%rsp), %xmm1
    vmovdqa  10*16(%rsp), %xmm2
    vmovdqa  11*16(%rsp), %xmm3
    vmovdqa  12*16(%rsp), %xmm4
    vmovdqa  13*16(%rsp), %xmm5
    vmovdqa  14*16(%rsp), %xmm6
    vmovdqa  15*16(%rsp), %xmm7
    vmovdqa  16*16(%rsp), %xmm8
    vmovdqa  17*16(%rsp), %xmm9
    vmovdqa  18*16(%rsp), %xmm10
    vmovdqa  19*16(%rsp), %xmm11
    vmovdqa  20*16(%rsp), %xmm12
    vmovdqa  21*16(%rsp), %xmm13
    vmovdqa  22*16(%rsp), %xmm14
    vmovdqa  23*16(%rsp), %xmm15
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret
   
   
/*

    Deccrypt and Authenticate vectorized computation engine

    void gcmDecrypt(uint8_t* CT, uint8_t* PT, Gctx *Gctx, uint64_t len, void Htbl, void *KS);

    CT : pointer to input cipher text
    PT : pointer to output decipher text
    Gctx : pointer to AES-GCM context
            16(Gctx)  ->  T         message authentication code/tag
            32(Gctx)  ->  ctr       GCM counter
    len : message length
    Htbl : extended table (of H = aesenc(0) based on the key)
    KS  : pointer to AES expanded key structures, 240(KS) -> number of AES rounds : 160/192/224

    aes-gcm decrypt operation can be described as follows (omitting implementation detail for byte swaping):
    0. Given 128-bit T and 128-bit ctr, an input plain text of len bytes
    1. while (len>=16) {
            *plain++ = *cipher xor encrypt(ctr++);
            T = (T + *cipher++) * H;
        }

    to vectorize, note that 
        - multiple encrypt blocks (of counters) can work together in parallel with the hash update function
            T = hash(T,Vcipher) = cipher[7]*H^1 + cipher[6]*H^2 + ... + cipher[1]*H^7 + (T + cipher[0])*H^8;

    the step 1 can be vectorized as follows

        T = hash(T, *Vcipher);  Vctr = aes_encrypt(ctr0:7); ctr+=8; *Vplain++ = *Vcipher++ xor Vctr;

    the key implementation issue is to maximize the parellelism of the operation

        - hash update T = hash(T, Vcipher);
        - Vctr = aes_encrypt(ctr);
        - derive deciphers from ciphers xor Vctr;

*/

#undef  CT
#undef  PT

#define CT %rdi
#define PT %rsi

.macro DEC_KARATSUBA i
   vmovdqu     (7-\i)*16(CT), t5
   vpshufb     .Lbswap_mask(%rip), t5, t5
   vpshufd     $78, t5, T
   vpclmulqdq  $0x11, 16*\i(Htbl), t5, t3
   vpxor       t5, T, T
   vpclmulqdq  $0x00, 16*\i(Htbl), t5, t4
   vpclmulqdq  $0x00, 128+\i*16(Htbl), T, t5
   vpxor       t3, t1, t1
   vpxor       t4, t2, t2
   vpxor       t5, t0, t0
.endm

.macro ROUND_DEC_KARATSUBA i    // reuse ctr as temp register
    vmovdqu  \i*16(KS), ctr
        vmovdqu     (7-\i)*16(CT), t5
    vaesenc  ctr, ctr0, ctr0
        vpshufb     .Lbswap_mask(%rip), t5, t5
    vaesenc  ctr, ctr1, ctr1
    vaesenc  ctr, ctr2, ctr2
        vpshufd     $78, t5, T
    vaesenc  ctr, ctr3, ctr3
        vpclmulqdq  $0x11, 16*\i(Htbl), t5, t3
        vpxor       t5, T, T
    vaesenc  ctr, ctr4, ctr4
        vpclmulqdq  $0x00, 16*\i(Htbl), t5, t4
    vaesenc  ctr, ctr5, ctr5
        vpclmulqdq  $0x00, 128+\i*16(Htbl), T, t5
    vaesenc  ctr, ctr6, ctr6
        vpxor       t3, t1, t1
    vaesenc  ctr, ctr7, ctr7
        vpxor       t4, t2, t2
        vpxor       t5, t0, t0
.endm

.macro ROUND_DEC_KARATSUBA_final i    // reuse ctr as temp register
    vmovdqu  \i*16(KS), ctr
        vmovdqu     (7-\i)*16(CT), t5
    vaesenc  ctr, ctr0, ctr0
        vpshufb     .Lbswap_mask(%rip), t5, t5
    vaesenc  ctr, ctr1, ctr1
        vpxor       T, t5, t5
    vaesenc  ctr, ctr2, ctr2
        vpshufd     $78, t5, T
    vaesenc  ctr, ctr3, ctr3
        vpclmulqdq  $0x11, 16*\i(Htbl), t5, t3
        vpxor       t5, T, T
    vaesenc  ctr, ctr4, ctr4
        vpclmulqdq  $0x00, 16*\i(Htbl), t5, t4
    vaesenc  ctr, ctr5, ctr5
        vpclmulqdq  $0x00, 128+\i*16(Htbl), T, t5
    vaesenc  ctr, ctr6, ctr6
        vpxor       t3, t1, t1
    vaesenc  ctr, ctr7, ctr7
        vpxor       t4, t2, t2
        vpxor       t5, t0, t0
.endm

    .globl _gcmDecrypt_avx1
    .align 4
_gcmDecrypt_avx1:

    /* allocate stack memory */
    pushq %rbp
    movq   %rsp, %rbp

    sub    $16+16*16, %rsp
    andq   $-16, %rsp

#if CC_KERNEL
    vmovdqa  %xmm0, 0*16(%rsp)
    vmovdqa  %xmm1, 1*16(%rsp)
    vmovdqa  %xmm2, 2*16(%rsp)
    vmovdqa  %xmm3, 3*16(%rsp)
    vmovdqa  %xmm4, 4*16(%rsp)
    vmovdqa  %xmm5, 5*16(%rsp)
    vmovdqa  %xmm6, 6*16(%rsp)
    vmovdqa  %xmm7, 7*16(%rsp)
    vmovdqa  %xmm8, 8*16(%rsp)
    vmovdqa  %xmm9, 9*16(%rsp)
    vmovdqa  %xmm10, 10*16(%rsp)
    vmovdqa  %xmm11, 11*16(%rsp)
    vmovdqa  %xmm12, 12*16(%rsp)
    vmovdqa  %xmm13, 13*16(%rsp)
    vmovdqa  %xmm14, 14*16(%rsp)
    vmovdqa  %xmm15, 15*16(%rsp)
#endif    

  
    // copy ctr/T/NR to registers
    vmovdqu  32(Gctx), ctr
    vmovdqu  16(Gctx), T
    mov      240(KS), NR
  
    // byte swap ctr/T for hashing computation 
   vpshufb  .Lbswap_mask(%rip), ctr, ctr
   vpshufb  .Lbswap_mask(%rip), T, T

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
      
    vmovdqa TWO(%rip), t4
    vmovdqa  .Lbswap_mask(%rip), t3
    vmovdqu (KS), t5
    vpaddd  ONE(%rip), ctr, ctr1
    vpaddd  TWO(%rip), ctr, ctr2
    vpshufb t3, ctr, ctr0
    vpaddd  TWO(%rip), ctr1, ctr3
    vpshufb t3, ctr1, ctr1
    vpaddd  TWO(%rip), ctr2, ctr4
    vpshufb t3, ctr2, ctr2
    vpxor  t5, ctr0, ctr0
    vpaddd  TWO(%rip), ctr3, ctr5
    vpshufb t3, ctr3, ctr3
    vpxor  t5, ctr1, ctr1
    vpaddd  TWO(%rip), ctr4, ctr6
    vpshufb t3, ctr4, ctr4
    vpxor  t5, ctr2, ctr2
    vpaddd  TWO(%rip), ctr5, ctr7
    vpshufb t3, ctr5, ctr5
    vpxor  t5, ctr3, ctr3
    vpaddd  TWO(%rip), ctr6, ctr
    vpshufb t3, ctr6, ctr6
    vpxor  t5, ctr4, ctr4
    vpshufb t3, ctr7, ctr7
    vpxor  t5, ctr5, ctr5
    vpxor  t5, ctr6, ctr6
    vpxor  t5, ctr7, ctr7

    // save T/ctr to reuse them
    vmovdqu T, 16(Gctx)
    vmovdqu ctr, 32(Gctx)

    // ciphers[7] * H^1 (intermediate results in t0/t1/t2)   
    vmovdqu     7*16(CT), t5
    vpshufb     t3, t5, t5
    vpclmulqdq  $0x11, 16*0(Htbl), t5, t1
    vpshufd     $78, t5, t3
    vpclmulqdq  $0x00, 16*0(Htbl), t5, t2      
    vpxor       t5, t3, t3
    vpclmulqdq  $0x00, 128+0*16(Htbl), t3, t0

    
    // 6 aes_encrypt rounds for ctr0:ctr7, and update (t0,t1,t2) += ciphers[6]*H^2 + ciphers[5]*H^3 + ... + ciphers[1]*H^7;  
    ROUND_DEC_KARATSUBA 1
    ROUND_DEC_KARATSUBA 2
    ROUND_DEC_KARATSUBA 3
    ROUND_DEC_KARATSUBA 4
    ROUND_DEC_KARATSUBA 5
    ROUND_DEC_KARATSUBA 6

    // 1 aes_encrypt round for ctr0:ctr7, and update (t0,t1,t2) += (ciphers[0]+T)*H^8; 
    vmovdqu 16(Gctx), T             // restore T, it is needed in ROUND_DEC_KARATSUBA_final
    ROUND_DEC_KARATSUBA_final 7

    // interleaved 2 aes_encrypt(ctr0:ctr7) rounds with reducion of 256-bit (t0/t1/t2) to 128-bit (in T)      
        vmovdqu  8*16(KS), ctr
    vpxor       t1, t0, t0
        vaesenc  ctr, ctr0, ctr0
    vpxor       t2, t0, t0
        vaesenc  ctr, ctr1, ctr1
    vpsrldq     $8, t0, t3
        vaesenc  ctr, ctr2, ctr2
    vpslldq     $8, t0, t0
        vaesenc  ctr, ctr3, ctr3
    vpxor       t3, t1, t4
        vaesenc  ctr, ctr4, ctr4
    vpxor       t0, t2, T
        vaesenc  ctr, ctr5, ctr5
        vaesenc  ctr, ctr6, ctr6
    vpshufd   $78, T, t1
    vpclmulqdq  $0x00, L_poly(%rip), T, T
        vaesenc  ctr, ctr7, ctr7
        vmovdqu  9*16(KS), ctr
        vaesenc  ctr, ctr0, ctr0
    vpxor       T, t1, T
        vaesenc  ctr, ctr1, ctr1
        vaesenc  ctr, ctr2, ctr2
    vpshufd  $78, T, t1
    vpclmulqdq  $0x00, L_poly(%rip), T, T
        vaesenc  ctr, ctr3, ctr3
        vaesenc  ctr, ctr4, ctr4
        vaesenc  ctr, ctr5, ctr5
    vpxor       T, t1, T
        vaesenc  ctr, ctr6, ctr6
        vaesenc  ctr, ctr7, ctr7
    vpxor    t4, T, T

    // if aes-128, branch to final aes encrypt round
    vmovdqu     160(KS), t5
    cmp         $160, NR
    jbe  1f
      
    ROUND 10
    ROUND 11

    // if aes-192, branch to final aes encrypt round
    vmovdqu     192(KS), t5
    cmp         $192, NR       
    jbe  1f
    
    // aes-256, fall through to last aes encrypt round 
    ROUND 12
    ROUND 13
    vmovdqu  224(KS), t5

1:      
    
   .macro   dec_read_write i, x  
    vmovdqu \i*16(CT), t3
    vpxor   t3, t5, t3
    vaesenclast t3, \x, \x 
    vmovdqu \x, \i*16(PT)
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

    // restore ctr as it was reused in the main loop
    vmovdqu 32(Gctx), ctr

    sub      $128, len
    jge       Main_Decrypt_Loop

#Here we decrypt and hash any remaining whole block

L_Decrypt_Single:
    add     $(128-16), len      // post-increment by 128 and pre-decrement by 16 of len
    jl    L_Decrypt_done
  
    // T + cipher[0] 
    vmovdqu  (CT), t5
    vpshufb  .Lbswap_mask(%rip), t5, t5
    vpxor    t5, T, t5
  
    // (t0,t1,t2) = (T+cipher[0])*H^1 
    vpshufd     $78, t5, t3
    vpclmulqdq  $0x11, (Htbl, len), t5, t1
    vpxor       t5, t3, t3
    vpclmulqdq  $0x00, (Htbl, len), t5, t2
    vpclmulqdq  $0x00, 128(Htbl, len), t3, t0

    // t5 = aes_encrypt(ctr); ctr++;
    single_block_encrypt
  

    // *pt++ = *ct++ xor aes_encrypt(ctr);
    vmovdqu (CT), t3     
    vpxor    t3, t5, t5
    vmovdqu  t5, (PT)
    addq     $16, CT
    addq     $16, PT

    sub   $16, len
    jl    L_Decrypt_reduce_final

L_Decrypt_Single_Loop:

    // update (t0,t1,t2) += (*cipher)*H^1;
    vmovdqu  (CT), t5
    vpshufb  .Lbswap_mask(%rip), t5, t5   
    vpclmulqdq  $0x11, (Htbl, len), t5, t3
    vpshufd     $78, t5, T
    vpclmulqdq  $0x00, (Htbl, len), t5, t4
    vpxor       t5, T, T
    vpclmulqdq  $0x00, 128(Htbl, len), T, t5
    vpxor       t3, t1, t1
    vpxor       t4, t2, t2
    vpxor       t5, t0, t0

    // t5 = aes_encrypt(ctr); ctr++;
    single_block_encrypt

    // *pt++ = *ct++ xor aes_encrypt(ctr);
    vmovdqu (CT), t3     
    vpxor    t3, t5, t5
    vmovdqu  t5, (PT)
    addq     $16, CT
    addq     $16, PT

    sub     $16, len
    jge   L_Decrypt_Single_Loop

L_Decrypt_reduce_final:

    reduce_to_128

L_Decrypt_done:

    // byte swap T and ctr and save to GCM ctx 
    vpshufb  .Lbswap_mask(%rip), T, T
    vpshufb  .Lbswap_mask(%rip), ctr, ctr
    vmovdqu  T, 16(Gctx)
    vmovdqu  ctr, 32(Gctx)

#if CC_KERNEL
    vmovdqa  0*16(%rsp), %xmm0
    vmovdqa  1*16(%rsp), %xmm1
    vmovdqa  2*16(%rsp), %xmm2
    vmovdqa  3*16(%rsp), %xmm3
    vmovdqa  4*16(%rsp), %xmm4
    vmovdqa  5*16(%rsp), %xmm5
    vmovdqa  6*16(%rsp), %xmm6
    vmovdqa  7*16(%rsp), %xmm7
    vmovdqa  8*16(%rsp), %xmm8
    vmovdqa  9*16(%rsp), %xmm9
    vmovdqa  10*16(%rsp), %xmm10
    vmovdqa  11*16(%rsp), %xmm11
    vmovdqa  12*16(%rsp), %xmm12
    vmovdqa  13*16(%rsp), %xmm13
    vmovdqa  14*16(%rsp), %xmm14
    vmovdqa  15*16(%rsp), %xmm15
#endif    

    // restore rsp and return
    movq   %rbp, %rsp
    popq   %rbp
    ret


#endif  // __x86_64__
#endif /* __NO_ASM__ */
