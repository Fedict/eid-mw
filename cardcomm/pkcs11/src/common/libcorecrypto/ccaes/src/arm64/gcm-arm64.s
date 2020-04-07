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
#if defined(__arm64__)
#include <corecrypto/cc_config.h>
    .align  6
L_ONE:
    .quad 1,0
L_TWO:
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

#define PT x0
#define CT x1
#define Gctx x2
#define len x3
#define Htbl x4
#define KS x5
#define NR w6

#define T       v0
#define qT      q0
#define t0    v1
#define t1    v2
#define t2    v3
#define KEY     v20
#define qKEY    q20
#define FINALKEY    v21
#define qFINALKEY   q21
#define t4    v6
#define qt4   q6
#define ctr0    v22
#define ctr1    v23
#define ctr2    v24
#define ctr3    v25
#define ctr4    v26
#define ctr5    v27
#define ctr6    v28
#define ctr7    v29
#define qctr0   q22
#define qctr1   q23
#define qctr2   q24
#define qctr3   q25
#define qctr4   q26
#define qctr5   q27
#define qctr6   q28
#define qctr7   q29
#define ctr     v30
#define qctr    q30

#define ONE     v16
#define TWO     v17
#define Lbswap  v18
#define Lpoly   v19
#define Zero    v31

/* -----------------------------------------------------------------------------------

                         AES-GCM encrypt macro definitions

    -------------------------------------------------------------------------------- */

    /*
        general round aes_encrypt
        $1 should be the i-th expanded key
    */
    .macro  aesenc
    aese.16b    $0, $1  
    aesmc.16b   $0, $0
    .endm

    /*
        last 2 rounds aes_encrypt
    */
    .macro  aeslast
    aese.16b    $0, KEY
    eor.16b     $0, $0, FINALKEY
    .endm

    /* 
        swap the high/low quad registers in $1 and write to $0
    */
    .macro  transpose
    ext.16b $0, $1, $1, #8  
    .endm

    /*
        byte swap $1 and write to $0
    */
    .macro  byteswap
    tbl.16b $0, {$1}, Lbswap
    .endm

    /*
        (t0,t1,t2) += cipher[7-i]*H^(i+1); i-th round aes_encrypt(ctr0:ctr7);
        we also used this for i=7, we pre-process cipher[0]+T and save that in stack
    */
    .macro ROUNDMUL i
    /*    HASHING                       ENCRYPTION            */
    ldr     q7, [sp, #(\i*16)]
    ldr     q4, [Htbl,#(128+\i*16)]
                                    aesenc  ctr0, KEY
    transpose   v6, v7
                                    aesenc  ctr1, KEY
    eor.16b v6, v7, v6
                                    aesenc  ctr2, KEY
    mov.16b     v5, t0
    pmull.1q    t0, v4, v6
    eor.16b     t0, v5, t0
    ldr     q5, [Htbl,#(\i*16)]
                                    aesenc  ctr3, KEY
                                    aesenc  ctr4, KEY
    mov.16b     v4, t1
    pmull2.1q   t1, v5, v7
    eor.16b     t1, v4, t1
                                    aesenc  ctr5, KEY
                                    aesenc  ctr6, KEY
    mov.16b     v4, t2
    pmull.1q    t2, v5, v7
    eor.16b     t2, v4, t2
                                    aesenc  ctr7, KEY
                                    ldr     qKEY, [KS, #(\i*16)]
    .endm

    /*
        i-th round aes_encrypt(ctr0:ctr7);
        exit with reading the next expanded key in KEY
    */
    .macro ROUND i
    aesenc  ctr0, KEY
    aesenc  ctr1, KEY
    aesenc  ctr2, KEY
    aesenc  ctr3, KEY
    aesenc  ctr4, KEY
    aesenc  ctr5, KEY
    aesenc  ctr6, KEY
    aesenc  ctr7, KEY
    ldr     qKEY, [KS, #(\i*16)]
    .endm

    /*
        (t0,t1,t2) += cipher[7-i] * H^(i+1);
    */
    .macro KARATSUBA i
    ldr         q6, [sp, #(\i*16)]
    ldr         q5, [Htbl, #(\i*16)]
    ldr         q7, [Htbl,#(128+\i*16)]
    mov.16b     v4, t1
    pmull2.1q   t1, v5, v6
    eor.16b     t1, v4, t1

    mov.16b     v4, t2
    pmull.1q    t2, v5, v6
    eor.16b     t2, v4, t2

    transpose   v5, v6
    eor.16b     v6, v6, v5

    mov.16b     v5, t0
    pmull.1q    t0, v7, v6
    eor.16b     t0, v5, t0
    .endm

    /*
        v7 = aes_encrypt(ctr); ctr++;
    */
    .macro  single_block_encrypt
    mov     x7, KS
    byteswap  v7, ctr
    ld1.4s  {v4,v5,v6}, [x7], #48
    add.4s   ctr, ONE, ctr
    aesenc  v7, v4
    aesenc  v7, v5
    aesenc  v7, v6
    ld1.4s  {v4,v5,v6}, [x7], #48
    aesenc  v7, v4
    aesenc  v7, v5
    aesenc  v7, v6
    ld1.4s  {v4,v5,v6}, [x7], #48
    aesenc  v7, v4
    aesenc  v7, v5
    aesenc  v7, v6
    ld1.4s  {KEY,FINALKEY}, [x7], #32
    cmp      NR, #160
    b.le      1f
    aesenc  v7, KEY
    aesenc  v7, FINALKEY
    ld1.4s  {KEY,FINALKEY}, [x7], #32
    cmp      NR, #192
    b.le     1f
    aesenc  v7, KEY
    aesenc  v7, FINALKEY
    ld1.4s  {KEY,FINALKEY}, [x7], #32
1:  aese.16b  v7, KEY
    eor.16b v7, v7, FINALKEY
    .endm

    /*
        finished up the last 2 rounds of aes_encrypt for 4 ctr
        *Vciper = *Vctr++ XOR *Vplain++;
        byte swap *Vcipher++ for hash computation
     */
    .macro  Encrypt_RW4
    ld1.4s  {v4, v5, v6, v7}, [PT], #64
    aese.16b    $0, KEY
    eor.16b     $0, $0, FINALKEY
    aese.16b    $1, KEY
    eor.16b     $1, $1, FINALKEY
    aese.16b    $2, KEY
    eor.16b     $2, $2, FINALKEY
    aese.16b    $3, KEY
    eor.16b     $3, $3, FINALKEY
    eor.16b     $0, $0, v4
    eor.16b     $1, $1, v5
    eor.16b     $2, $2, v6
    eor.16b     $3, $3, v7
    st1.4s  {$0, $1, $2, $3}, [CT], #64
    tbl.16b $0, {$0}, Lbswap
    tbl.16b $1, {$1}, Lbswap
    tbl.16b $2, {$2}, Lbswap
    tbl.16b $3, {$3}, Lbswap
    .endm

    /*
        reduce Karatsuba temp results in (t0,t1,t2) to 128-bit ib T
    */
    .macro  reduce_to_128bit 
    eor.16b      t0, t1, t0
    eor.16b      t0, t2, t0
    ext.16b      v5, t0, Zero, #8
    eor.16b      t4, v5, t1
    ext.16b      v5, Zero, t0, #8
    eor.16b      T, v5, t2
    transpose    t1, T
    pmull.1q     T, Lpoly, T
    eor.16b      T, t1, T
    transpose    t1, T
    pmull.1q     T, Lpoly, T
    eor.16b      T, t1, T
    eor.16b      T, t4, T
    .endm

    .globl _gcmEncrypt
    .align 4
_gcmEncrypt:

    // set up often used registers 
    adrp    x7, L_ONE@page
    add     x7, x7, L_ONE@pageoff
#if CC_KERNEL
    sub     sp, sp, #24*16
    mov     x8, sp
    st1.4s  {v0,v1,v2,v3}, [x8], #4*16
    st1.4s  {v4,v5,v6,v7}, [x8], #4*16
    st1.4s  {v16,v17,v18,v19}, [x8], #4*16
    st1.4s  {v20,v21,v22,v23}, [x8], #4*16
    st1.4s  {v24,v25,v26,v27}, [x8], #4*16
    st1.4s  {v28,v29,v30,v31}, [x8], #4*16
#endif
    sub     sp, sp, #128
    ld1.4s  {ONE,TWO,Lbswap,Lpoly}, [x7]
    eor.16b Zero,Zero,Zero  

    /* read counter/Tag/NR into registers */
    ldr     qctr, [Gctx, #32]
    ldr     qT, [Gctx, #16]
    ldr     NR, [KS, #240]
  
    /* byte swap counter/Tag for hash computation */ 
    byteswap    ctr, ctr
    byteswap    T, T
 
    /* if less than 128 bytes, branch to deal with single vector 16-byte */ 
    cmp        len, #128
    b.lt       L_Encrypt_Singles
  
    /*
        ctr -> ctr0:ctr7;   ctr+=8;
        ctr0:ctr7 = byteswap(ctr0:ctr7);
    */ 
    mov.16b     ctr0, ctr
    add.4s      ctr1, ONE, ctr0
    add.4s      ctr2, TWO, ctr0
    byteswap    ctr0, ctr0
    add.4s      ctr3, TWO, ctr1
    byteswap    ctr1, ctr1
    add.4s      ctr4, TWO, ctr2
    byteswap    ctr2, ctr2
    add.4s      ctr5, TWO, ctr3
    byteswap    ctr3, ctr3
    add.4s      ctr6, TWO, ctr4
    byteswap    ctr4, ctr4
    add.4s      ctr7, TWO, ctr5
    byteswap    ctr5, ctr5
    add.4s      ctr,  TWO, ctr6
    byteswap    ctr6, ctr6
    ldr         qKEY, [KS]
    byteswap    ctr7, ctr7

    /*
        9 rounds of aes_encrypt(ctr0:ctr7);
    */
    ROUND 1
    ROUND 2
    ROUND 3
    ROUND 4
    ROUND 5
    ROUND 6
    ROUND 7
    ROUND 8
    ROUND 9
   
    ldr qFINALKEY, [KS, #160]
    cmp  NR, #160
    b.le  1f
  
    ROUND 10
    ROUND 11
  
    ldr qFINALKEY, [KS, #192]
    cmp  NR, #192
    b.le  1f

    ROUND 12
    ROUND 13

    ldr qFINALKEY, [KS, #224]
  
1:
    /*
        *Vciper = *Vctr++ XOR *Vplain++;
        byte swap *Vcipher++ for hash computation
     */
    Encrypt_RW4 ctr0, ctr1, ctr2, ctr3
    Encrypt_RW4 ctr4, ctr5, ctr6, ctr7

    subs        len, len, #(128+128)        // 1st 128-byte and pre-decrement len by another 128 bytes
    b.lt        Encrypt_Main_Loop_End      // if no more than another 128 bytes input, branch to End of 8 vectors
    b           Encrypt_Main_Loop
 
/*
    ------------------------------------------------------
            AES-GCM encrypt main loop START
    ------------------------------------------------------ 
*/  
    .align 6
    .globl  Encrypt_Main_Loop
Encrypt_Main_Loop:

    /*         HASHING                ENCRYPTION                                 */  
    /*
        (t0,t1,t2) = cipher[7]*H^1;     ctr-> ctr0:ctr7;  ctr0:ctr7 = byteswap(ctr0:ctr7); ctr+=8;
    */
    eor.16b     T, ctr0, T                                      // pre-processed ctr0 XOR T
    ldr         q4, [Htbl]
    str         qT, [sp, #16*7]                                 // so we can reuse ROUNDMUL for i=7 
                                    mov.16b ctr0, ctr           // counter 0
    ldr         q5, [Htbl, #128]
    str         qctr1, [sp, #16*6]

                                    add.4s  ctr1, ONE, ctr0
    str         qctr2, [sp, #16*5]
                                    add.4s  ctr2, TWO, ctr0
    pmull2.1q   t1, v4, ctr7

    str         qctr3, [sp, #16*4]
                                    add.4s  ctr3, TWO, ctr1
                                    byteswap  ctr0, ctr0
    pmull.1q    t2, v4, ctr7

    str         qctr4, [sp, #16*3]
                                    add.4s  ctr4, TWO, ctr2
                                    byteswap  ctr1, ctr1
    transpose   v4, ctr7

    str         qctr5, [sp, #16*2]
                                    add.4s  ctr5, TWO, ctr3
                                    byteswap  ctr2, ctr2
    eor.16b     v7, v4, ctr7

    str         qctr6, [sp, #16*1]
                                    add.4s  ctr6, TWO, ctr4
                                    byteswap  ctr3, ctr3
    pmull.1q    t0, v5, v7

                                    add.4s  ctr7, TWO, ctr5
                                    byteswap  ctr4, ctr4
                                    add.4s  ctr, TWO, ctr6

                                    ldr qKEY, [KS]
                                    byteswap  ctr5, ctr5
                                    byteswap  ctr6, ctr6
                                    byteswap  ctr7, ctr7
    /*
        for i = 1 to 7
           (t0,t1,t2) += cipher[7-i]*H^(1+i);     i-th round aes_encrypt(ctr0:ctr7);
    */
      
      ROUNDMUL 1
      ROUNDMUL 2
      ROUNDMUL 3
      ROUNDMUL 4
      ROUNDMUL 5
      ROUNDMUL 6
      ROUNDMUL 7

    /*
        interleaved reduce (t0/t1/t2) to 128-bit T and ROUND 8/9
    */     
    eor.16b     t0, t1, t0
                                            aesenc  ctr0, KEY
    eor.16b     t0, t2, t0
                                            aesenc  ctr1, KEY
    ext.16b     v4, t0, Zero, #8
    ext.16b     v5, Zero, t0, #8
                                            aesenc  ctr2, KEY
                                            aesenc  ctr3, KEY
    eor.16b     t4, v4, t1
    eor.16b     T, v5, t2

                                            aesenc  ctr4, KEY
    transpose   t1, T
    pmull.1q    T, Lpoly, T
    eor.16b     T, t1, T
                                            aesenc  ctr5, KEY
                                            aesenc  ctr6, KEY
                                            aesenc  ctr7, KEY
                                            ldr     qKEY, [KS, #(8*16)]
      
                                            aesenc  ctr0, KEY
                                            aesenc  ctr1, KEY
    transpose   t1, T
    pmull.1q    T, Lpoly, T
    eor.16b     T, t1, T
                                            aesenc  ctr2, KEY
                                            aesenc  ctr3, KEY
                                            aesenc  ctr4, KEY
                                            aesenc  ctr5, KEY
                                            aesenc  ctr6, KEY
                                            aesenc  ctr7, KEY
    eor.16b     T, t4, T
                                            ldr     qKEY, [KS, #(9*16)]
                                            ldr     qFINALKEY, [KS,#160]

      cmp   NR, #160
      b.le  1f

      ROUND 10
      ROUND 11
      ldr   qFINALKEY, [KS, #192]
      cmp      NR, #192
      b.le  1f
     
      ROUND 12
      ROUND 13
      ldr   qFINALKEY, [KS, #224]

1:


    /*
        *Vciper = *Vctr++ XOR *Vplain++;
        byte swap *Vcipher++ for hash computation
    */
    Encrypt_RW4 ctr0, ctr1, ctr2, ctr3
    Encrypt_RW4 ctr4, ctr5, ctr6, ctr7

    subs    len, len, #128
    b.ge    Encrypt_Main_Loop

/*
    ------------------------------------------------------
            AES-GCM encrypt main loop END
    ------------------------------------------------------ 
*/  

Encrypt_Main_Loop_End:

    // post-increment len by 128
    add         len, len, #128
    eor.16b     ctr0, T, ctr0           // do this so we can reuse KARATSUBA for i=7
    
    mov.16b     v7, ctr7
    str         qctr6, [sp, #16*1]
    str         qctr5, [sp, #16*2]
    str         qctr4, [sp, #16*3]
    str         qctr3, [sp, #16*4]
    str         qctr2, [sp, #16*5]
    str         qctr1, [sp, #16*6]
    str         qctr0, [sp, #16*7]
  
    /*
        (t0,t1,t2) = cipher[7]*H^1;
    */ 
    ldr         q5, [Htbl, #0]
    pmull2.1q   t1, v5, v7
    pmull.1q    t2, v5, v7
    transpose   v5, v7
    eor.16b     v7, v5, v7
    ldr         q5, [Htbl, #128]
    pmull.1q    t0, v5, v7
  
    /*
        for i = 1 to 7
            (t0,t1,t2) += cipher[7-i]*H^(1+i);
    */ 
    KARATSUBA 1
    KARATSUBA 2
    KARATSUBA 3      
    KARATSUBA 4
    KARATSUBA 5
    KARATSUBA 6
    KARATSUBA 7

    /*
        reduce Karatsuba temp result (t0,t1,t2) to 128-bit T
    */
    reduce_to_128bit

L_Encrypt_Singles:

    subs    len, len, #16
    b.lt    L_Encrypt_done

    /*
        v7 = aes_encrypt(ctr); ctr++;
    */
    single_block_encrypt

    /* 
        *cipher++ = v7 XOR *plain++;
    */
    ld1.4s      {v4}, [PT], #16
    eor.16b     v7, v4, v7
    st1.4s      {v7}, [CT], #16
  
    /*
        (t0,t1,t2) = (*cipher + T)*H^(len/16);  len-=16;
    */ 
    byteswap    v7, v7
    eor.16b     v7, T, v7
    add         x7, Htbl, len 
    ldr         q4, [x7]      
    pmull2.1q   t1, v4, v7
    pmull.1q    t2, v4, v7
    ldr         q4, [x7, #128]      
    transpose   v5, v7
    eor.16b     v7, v5, v7
    pmull.1q    t0, v4, v7

    subs     len, len, #16
    b.lt    L_Encrypt_reduce

L_Encrypt_Singles_Loop:

    /*
        v7 = aes_encrypt(ctr); ctr++;
    */
    single_block_encrypt

    /* 
        *cipher++ = v7 XOR *plain++;
    */
    ld1.4s  {v4}, [PT], #16
    eor.16b v7, v4, v7
    st1.4s  {v7}, [CT], #16

    /*
        (t0,t1,t2) += (*cipher)*H^(len/16);  len-=16;
    */ 
    byteswap    v7, v7   
    add         x7, Htbl, len 
    ldr         q4, [x7]      
    mov.16b     v5, t1
    pmull2.1q   t1, v4, v7
    eor.16b     t1, v5, t1

    mov.16b     v5, t2
    pmull.1q    t2, v4, v7
    eor.16b     t2, v5, t2

    ldr         q4, [x7, #128]      
    transpose   v5, v7
    eor.16b     v7, v5, v7

    mov.16b     v5, t0
    pmull.1q    t0, v4, v7
    eor.16b     t0, v5, t0

    subs     len, len, #16
    b.ge     L_Encrypt_Singles_Loop

L_Encrypt_reduce:

    reduce_to_128bit
   
L_Encrypt_done:

    byteswap    T, T
    byteswap    ctr, ctr
    str         qT, [Gctx, #16]
    str         qctr, [Gctx, #32]
    add         sp, sp, #128
#if CC_KERNEL
    ld1.4s  {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s  {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s  {v16,v17,v18,v19}, [sp], #4*16
    ld1.4s  {v20,v21,v22,v23}, [sp], #4*16
    ld1.4s  {v24,v25,v26,v27}, [sp], #4*16
    ld1.4s  {v28,v29,v30,v31}, [sp], #4*16
#endif
    ret         lr


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
#define CT x0
#define PT x1

    /*
        Decrypt Karatsuba to update (t0,t1,t2) += cipher[7-i]*H^(i+1) 
        This macro is integrated with macro ROUND into ROUND_DEC_KARATSUBA given in the following
    */
    .macro DEC_KARATSUBA i
    ldr          q7, [CT,#(7-\i)*16]
    byteswap     v7, v7
    ldr          q4, [Htbl, #16*\i] 

    mov.16b      v5, t1
    pmull2.1q    t1, v4, v7
    eor.16b      t1, v5, t1

    mov.16b      v4, t2
    pmull.1q     t2, v4, v7
    eor.16b      t2, v4, t2

    ldr          q4, [Htbl, #128+16*\i] 
    transpose    v5, v7
    eor.16b      v7, v5, v7

    mov.16b      v5, t0
    pmull.1q     t0, v4, v7
    eor.16b      t0, v5, t0
    .endm

    /*
        Decrypt Karatsuba to update (t0,t1,t2) += cipher[7-i]*H^(i+1) + i-th round aes_encrypt(ctr0:ctr7)
        this is used for i = 1 to 6
    */
    .macro ROUND_DEC_KARATSUBA i
    /*    HASHING                           Encrypt                     */
    ldr         q7, [CT,#(7-\i)*16]
                                        aesenc  ctr0, KEY
    ldr         q4, [Htbl, #16*\i] 
                                        aesenc  ctr1, KEY
    byteswap     v7, v7
    ldr         q6, [Htbl, #128+16*\i] 
                                        aesenc  ctr2, KEY
    mov.16b     v5, t1
    pmull2.1q   t1, v4, v7
    eor.16b     t1, v5, t1
                                        aesenc  ctr3, KEY
    mov.16b     v5, t2
    pmull.1q    t2, v4, v7
    eor.16b     t2, v5, t2
                                        aesenc  ctr4, KEY
    transpose     v5, v7
                                        aesenc  ctr5, KEY
    eor.16b     v5, v7, v5
                                        aesenc  ctr6, KEY
    mov.16b     v4, t0
    pmull.1q    t0, v6, v5
    eor.16b     t0, v4, t0
                                        aesenc  ctr7, KEY
                                        ldr     qKEY, [KS, #(\i*16)]
    .endm

    /*
        Decrypt Karatsuba to update (t0,t1,t2) += (cipher[0]+T)*H^8 + 8-th round aes_encrypt(ctr0:ctr7)
    */
    .macro ROUND0_DEC_KARATSUBA i
    /*    HASHING                           Encrypt                     */
    ldr         q7, [CT,#(7-\i)*16]
                                        aesenc  ctr0, KEY
    byteswap     v7, v7
    ldr         q4, [Htbl, #16*\i] 
                                        aesenc  ctr1, KEY
    eor.16b     v7, v7, T
    ldr         q6, [Htbl, #128+16*\i] 
                                        aesenc  ctr2, KEY
    mov.16b     v5, t1
    pmull2.1q   t1, v4, v7
    eor.16b     t1, v5, t1
                                        aesenc  ctr3, KEY
    mov.16b     v5, t2
    pmull.1q    t2, v4, v7
    eor.16b     t2, v5, t2
                                        aesenc  ctr4, KEY
    transpose     v5, v7
                                        aesenc  ctr5, KEY
    eor.16b     v5, v7, v5
                                        aesenc  ctr6, KEY
    mov.16b     v4, t0
    pmull.1q    t0, v6, v5
    eor.16b     t0, v4, t0
                                        aesenc  ctr7, KEY
                                        ldr     qKEY, [KS, #(\i*16)]
    .endm


    .globl _gcmDecrypt
    .align 4
_gcmDecrypt:

    /* set up often used constants in registers */
    adrp        x7, L_ONE@page
    add         x7, x7, L_ONE@pageoff
#if CC_KERNEL
    sub     sp, sp, #24*16
    mov     x8, sp
    st1.4s  {v0,v1,v2,v3}, [x8], #4*16
    st1.4s  {v4,v5,v6,v7}, [x8], #4*16
    st1.4s  {v16,v17,v18,v19}, [x8], #4*16
    st1.4s  {v20,v21,v22,v23}, [x8], #4*16
    st1.4s  {v24,v25,v26,v27}, [x8], #4*16
    st1.4s  {v28,v29,v30,v31}, [x8], #4*16
#endif
    ld1.4s      {ONE,TWO,Lbswap,Lpoly}, [x7]
    eor.16b     Zero,Zero,Zero  
  
    /* initiate ctr/T/NR in registers */ 
    ldr      qctr, [Gctx, #32]
    ldr      qT, [Gctx, #16]
    ldr      NR, [KS, #240]
 
    /* 
        byte swap T for hash computation
        byte swap ctr to add constants 0:7 to derive ctr0:ctr7, which needs to be byte swap back for aes_encrypt 
        
    */ 
    byteswap  ctr, ctr
    byteswap  T, T

    subs        len, len, #128
    b.lt        Decrypt_Main_Loop_End
    b           Decrypt_Main_Loop
      
    .align 6
Decrypt_Main_Loop:
    

    /* hashing based on ciphers, and aes-encrypt(counters) interleaved                     */
    /*          Hashing                           encrypt                                  */
    /*   (t0,t1,t2) = cipher[7]*H^1;       ctr -> ctr0:ctr7; ctr+=8;  byteswap(ctr0:ctr7); */

      ldr           q7, [CT,#7*16]
                                            byteswap    ctr0, ctr
      ldr           q4, [Htbl,#16*0]
                                            add.4s      ctr1, ONE, ctr      // ctr1 = ctr + 1
                                            add.4s      ctr2, TWO, ctr
      ldr           q6, [Htbl,#128+16*0]
      byteswap      v7, v7
                                            add.4s      ctr3, TWO, ctr1
                                            byteswap    ctr1, ctr1          // byte swap ctr1 for aes_encrypt
                                            add.4s      ctr4, TWO, ctr2
      pmull2.1q     t1, v4, v7
                                            byteswap    ctr2, ctr2
      transpose     v5, v7
                                            add.4s      ctr5, TWO, ctr3
      pmull.1q      t2, v4, v7
                                            byteswap    ctr3, ctr3
                                            add.4s      ctr6, TWO, ctr4
      eor.16b       v7, v5, v7
                                            byteswap    ctr4, ctr4
                                            add.4s      ctr7, TWO, ctr5
                                            byteswap    ctr5, ctr5
                                            add.4s      ctr, TWO, ctr6
      pmull.1q      t0, v6, v7
                                            ldr     qKEY, [KS]
                                            byteswap    ctr6, ctr6
                                            byteswap    ctr7, ctr7
      

    /*
        for i=1 to 6
        (t0,t1,t2) += cipher[7-i]*H^(i+1);       i-th round aes_encrypt(ctr0:ctr7);
    */
                            ROUND_DEC_KARATSUBA 1
      
                            ROUND_DEC_KARATSUBA 2
      
                            ROUND_DEC_KARATSUBA 3
      
                            ROUND_DEC_KARATSUBA 4
      
                            ROUND_DEC_KARATSUBA 5
      
                            ROUND_DEC_KARATSUBA 6

    /*
        (t0,t1,t2) += (cipher[0]+T)*H^(i+1);       7-th round aes_encrypt(ctr0:ctr7);
    */
                            ROUND0_DEC_KARATSUBA 7
      
    /*
        interleaved reduce (t0/t1/t2) to 128-bit T and ROUND 8/9
    */     
    eor.16b       t0, t1, t0
                                            aesenc  ctr0, KEY
    eor.16b       t0, t2, t0
                                            aesenc  ctr1, KEY
    ext.16b v4, t0, Zero, #8
    ext.16b v5, Zero, t0, #8
                                            aesenc  ctr2, KEY
                                            aesenc  ctr3, KEY
    eor.16b       t4, v4, t1
    eor.16b       T, v5, t2

                                            aesenc  ctr4, KEY
    transpose   t1, T
    pmull.1q    T, Lpoly, T
    eor.16b       T, t1, T
                                            aesenc  ctr5, KEY
                                            aesenc  ctr6, KEY
                                            aesenc  ctr7, KEY
                                            ldr     qKEY, [KS, #(8*16)]
      
                                            aesenc  ctr0, KEY
                                            aesenc  ctr1, KEY
    transpose   t1, T
    pmull.1q    T, Lpoly, T
    eor.16b       T, t1, T
                                            aesenc  ctr2, KEY
                                            aesenc  ctr3, KEY
                                            aesenc  ctr4, KEY
                                            aesenc  ctr5, KEY
                                            aesenc  ctr6, KEY
                                            aesenc  ctr7, KEY
    eor.16b    T, t4, T
                                            ldr     qKEY, [KS, #(9*16)]
                                            ldr     qFINALKEY, [KS,#160]

    cmp         NR, #160    /* aes-128 ? */
    b.le  1f                
      
                                            ROUND 10
                                            ROUND 11
                                            ldr     qFINALKEY, [KS, #192]
    cmp         NR, #192    /* aes-192 ? */   
    b.le  1f
     
                                            ROUND 12
                                            ROUND 13
                                            ldr     qFINALKEY, [KS, #224]

1:      

    /* 
        aeslast needs KEY/FINALKEY to finish final 2 rounds
        Vdecipher++ = Vciphers++ XOR ctr0:ctr7;
    */
     
    ld1.4s  {v4,v5,v6,v7}, [CT], #64
    aeslast ctr0
    aeslast ctr1
    aeslast ctr2
    aeslast ctr3
    eor.16b  ctr0, v4, ctr0
    eor.16b  ctr1, v5, ctr1
    eor.16b  ctr2, v6, ctr2
    eor.16b  ctr3, v7, ctr3
    ld1.4s  {v4,v5,v6,v7}, [CT], #64
    aeslast ctr4
    aeslast ctr5
    aeslast ctr6
    aeslast ctr7
    st1.4s  {ctr0, ctr1, ctr2, ctr3}, [PT], #64 
    eor.16b  ctr4, v4, ctr4
    eor.16b  ctr5, v5, ctr5
    eor.16b  ctr6, v6, ctr6
    eor.16b  ctr7, v7, ctr7
    st1.4s  {ctr4, ctr5, ctr6, ctr7}, [PT], #64 

    subs    len, len, #128
    b.ge    Decrypt_Main_Loop

Decrypt_Main_Loop_End:

    /* dealing with single block */

    adds    len, len, #(128-16)
    b.lt    L_Decrypt_done
  
    /*
        (t0,t1,t2) = (*cipher + T)*H^(len/16);  len-=16;
    */ 
    ldr         q7, [CT]
    add         x7, Htbl, len 
    byteswap    v7, v7
    ldr q4,     [Htbl, len]  
    eor.16b     v7, T, v7
    ldr         qT, [x7, #128]  
    transpose   v5, v7
    pmull2.1q   t1, v4, v7
    eor.16b     v5, v5, v7
    pmull.1q    t2, v4, v7
    pmull.1q    t0, T, v5

    /*
        v7 = aes_encrypt(ctr); ctr++;
    */
    single_block_encrypt

    /*
        *decipher++ = *cipher++ XOR v7;
    */
    ld1.4s  {v4},[CT],#16 
    eor.16b    v7, v4, v7
    st1.4s  {v7},[PT],#16

    subs   len, len, #16
    b.lt    L_Decrypt_reduce

L_Decrypt_block_Loop:

    /*
        (t0,t1,t2) += (*cipher)*H^(len/16);  len-=16;
    */ 
    ldr         q7, [CT]
    byteswap    v7, v7
    add         x7, Htbl, len
    ldr         q4, [x7]
    mov.16b     v5, t1
    pmull2.1q   t1, v4, v7
    eor.16b     t1, v5, t1

    mov.16b     v5, t2
    pmull.1q    t2, v4, v7
    eor.16b     t2, v5, t2

    ldr         q4, [x7, #128]
    transpose   v5, v7
    eor.16b     v7, v5, v7

    mov.16b     v5, t0
    pmull.1q    t0, v4, v7
    eor.16b     t0, v5, t0

    /*
        v7 = aes_encrypt(ctr); ctr++;
    */
    single_block_encrypt

    /*
        *decipher++ = *cipher++ XOR v7;
    */
    ld1.4s  {v4},[CT],#16 
    eor.16b    v7, v4, v7
    st1.4s  {v7},[PT],#16

    subs   len, len, #16
    b.ge   L_Decrypt_block_Loop

L_Decrypt_reduce:

    /*
        reduce 256-bit (t0,t1,t2) to 128-bit T
    */
    reduce_to_128bit

L_Decrypt_done:

    byteswap    T, T
    byteswap    ctr, ctr
    str         qT, [Gctx, #16]
    str         qctr, [Gctx, #32]
#if CC_KERNEL
    ld1.4s  {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s  {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s  {v16,v17,v18,v19}, [sp], #4*16
    ld1.4s  {v20,v21,v22,v23}, [sp], #4*16
    ld1.4s  {v24,v25,v26,v27}, [sp], #4*16
    ld1.4s  {v28,v29,v30,v31}, [sp], #4*16
#endif
    ret         lr

#endif  // __arm64__
#endif /* __NO_ASM__ */
