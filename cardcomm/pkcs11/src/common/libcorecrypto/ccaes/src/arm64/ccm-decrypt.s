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

    /*
            arm64 implementation of ccm-decrypt functions

            void ccm128_decrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
            void ccm192_decrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
            void ccm256_decrypt(void *in, void *out, void *tag, int nblocks, void *key, void *ctr, int ctr_len);
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

#define pin     x0
#define pout    x1
#define ptag    x2
#define nblocks w3
#define KS      x4
#define pctr    x5
#define ctr_len w6

#define in      v0
#define tag     v1
#define ctr     v2
#define out     v3
#define key     v4
#define mask    v5
#define one     v6
#define Lbswap  v7

#define key0    v16
#define key1    v17
#define key2    v18
#define key3    v19
#define key4    v20
#define key5    v21
#define key6    v22
#define key7    v23
#define key8    v24
#define key9    v25
#define key10   v26
#define key11   v27
#define key12   v28
#define key13   v29
#define key14   v30

    .macro  aesenc1
    aese.16b    $0, $1
    aesmc.16b   $0, $0
    .endm

    .macro  aesenclast1
    aese.16b    $0, $1
    eor.16b     $0, $0, $2
    .endm

    .macro  aesenc
    aese.16b    out, $0
    aesmc.16b   out, out
    aese.16b    tag, $0
    aesmc.16b   tag, tag
    .endm

    .macro  aesenclast
    aese.16b    out, $0
    eor.16b     out, out, $1
    aese.16b    tag, $0
    eor.16b     tag, tag, $1
    .endm

    .macro  load_keys_128
    ld1.4s  {key0,key1,key2,key3}, [KS], #64
    ld1.4s  {key4,key5,key6,key7}, [KS], #64
    ld1.4s  {key8,key9,key10}, [KS], #48
    .endm

    .macro  load_keys_192
    ld1.4s  {key0,key1,key2,key3}, [KS], #64
    ld1.4s  {key4,key5,key6,key7}, [KS], #64
    ld1.4s  {key8,key9,key10,key11}, [KS], #64
    ld1.4s  {key12}, [KS], #16
    .endm

    .macro  load_keys_256
    ld1.4s  {key0,key1,key2,key3}, [KS], #64
    ld1.4s  {key4,key5,key6,key7}, [KS], #64
    ld1.4s  {key8,key9,key10,key11}, [KS], #64
    ld1.4s  {key12,key13,key14}, [KS], #48
    .endm

    .macro  aesenc1_128
    aesenc1  $0, key0
    aesenc1  $0, key1
    aesenc1  $0, key2
    aesenc1  $0, key3
    aesenc1  $0, key4
    aesenc1  $0, key5
    aesenc1  $0, key6
    aesenc1  $0, key7
    aesenc1  $0, key8
    aesenclast1  $0, key9, key10
    .endm
    .macro  aesenc1_192
    aesenc1  $0, key0
    aesenc1  $0, key1
    aesenc1  $0, key2
    aesenc1  $0, key3
    aesenc1  $0, key4
    aesenc1  $0, key5
    aesenc1  $0, key6
    aesenc1  $0, key7
    aesenc1  $0, key8
    aesenc1  $0, key9
    aesenc1  $0, key10
    aesenclast1  $0, key11, key12
    .endm
    .macro  aesenc1_256
    aesenc1  $0, key0
    aesenc1  $0, key1
    aesenc1  $0, key2
    aesenc1  $0, key3
    aesenc1  $0, key4
    aesenc1  $0, key5
    aesenc1  $0, key6
    aesenc1  $0, key7
    aesenc1  $0, key8
    aesenc1  $0, key9
    aesenc1  $0, key10
    aesenc1  $0, key11
    aesenc1  $0, key12
    aesenclast1  $0, key13, key14
    .endm

    .macro  ucao_head
    add.2d  out, ctr, one
    bic.16b ctr, ctr, mask  
    and.16b out, out, mask
    orr.16b ctr, out, ctr
    ld1.4s  {in}, [pin], #16
    tbl.16b out, {ctr}, Lbswap
    .endm

    .macro  ucao_tail
    eor.16b out, in, out
    st1.4s  {out}, [pout], #16
    .endm

    .macro  update_ctr_and_output_128
    ucao_head
    aesenc1_128 out
    ucao_tail
    .endm
    .macro  update_ctr_and_output_192
    ucao_head
    aesenc1_192 out
    ucao_tail
    .endm
    .macro  update_ctr_and_output_256
    ucao_head
    aesenc1_256 out
    ucao_tail
    .endm

    .macro  aesenc_128
    aesenc  key0
    aesenc  key1
    aesenc  key2
    aesenc  key3
    aesenc  key4
    aesenc  key5
    aesenc  key6
    aesenc  key7
    aesenc  key8
    aesenclast  key9, key10
    .endm
    .macro  aesenc_192
    aesenc  key0
    aesenc  key1
    aesenc  key2
    aesenc  key3
    aesenc  key4
    aesenc  key5
    aesenc  key6
    aesenc  key7
    aesenc  key8
    aesenc  key9
    aesenc  key10
    aesenclast  key11, key12
    .endm
    .macro  aesenc_256
    aesenc  key0
    aesenc  key1
    aesenc  key2
    aesenc  key3
    aesenc  key4
    aesenc  key5
    aesenc  key6
    aesenc  key7
    aesenc  key8
    aesenc  key9
    aesenc  key10
    aesenc  key11
    aesenc  key12
    aesenclast  key13, key14
    .endm

    .macro  single_block_decrypt_128
    eor.16b tag, tag, out
    ucao_head
    aesenc_128
    ucao_tail
    .endm
    .macro  single_block_decrypt_192
    eor.16b tag, tag, out
    ucao_head
    aesenc_192
    ucao_tail
    .endm
    .macro  single_block_decrypt_256
    eor.16b tag, tag, out
    ucao_head
    aesenc_256
    ucao_tail
    .endm

    .macro  final_tag_update_128
    eor.16b tag, tag, out
    aesenc1_128 tag
    .endm   
    .macro  final_tag_update_192
    eor.16b tag, tag, out
    aesenc1_192 tag
    .endm   
    .macro  final_tag_update_256
    eor.16b tag, tag, out
    aesenc1_256 tag
    .endm   

    .globl _ccm128_decrypt
    .align 4
_ccm128_decrypt:


/* set up often used constants in registers */
    adrp        x7, L_ONE@page
    add         x7, x7, L_ONE@pageoff

#if CC_KERNEL
    sub     sp, sp, #19*16
    mov     x8, sp
    st1.4s  {v0,v1,v2,v3}, [x8], #4*16
    st1.4s  {v4,v5,v6,v7}, [x8], #4*16
    st1.4s  {v16,v17,v18,v19}, [x8], #4*16
    st1.4s  {v20,v21,v22,v23}, [x8], #4*16
    st1.4s  {v24,v25,v26}, [x8], #3*16
#endif
    ld1.4s      {one,Lbswap}, [x7]
    
    // read mask 
    ldr     q5, [x7, ctr_len, uxtw #4] 

    ld1.4s  {ctr}, [pctr] 
    ld1.4s  {tag}, [ptag] 

    load_keys_128
  
    // byte swap ctr
    tbl.16b ctr, {ctr}, Lbswap 

    update_ctr_and_output_128
    subs    nblocks, nblocks, #1
    b.eq    9f      // L_Decrypt_done

0:      // L_Main_Loop:

    single_block_decrypt_128
    subs    nblocks, nblocks, #1
    b.gt    0b          // L_Main_Loop

9:      // L_Decrypt_done:

    final_tag_update_128

    // byte swap ctr and save to *ptr_ctx 
    tbl.16b ctr, {ctr}, Lbswap 
    st1.4s  {ctr}, [pctr]
    st1.4s  {tag}, [ptag]

#if CC_KERNEL
    ld1.4s  {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s  {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s  {v16,v17,v18,v19}, [sp], #4*16
    ld1.4s  {v20,v21,v22,v23}, [sp], #4*16
    ld1.4s  {v24,v25,v26}, [sp], #3*16
#endif    

    ret     lr

    .globl _ccm192_decrypt
    .align 4
_ccm192_decrypt:


/* set up often used constants in registers */
    adrp        x7, L_ONE@page
    add         x7, x7, L_ONE@pageoff

#if CC_KERNEL
    sub     sp, sp, #21*16
    mov     x8, sp
    st1.4s  {v0,v1,v2,v3}, [x8], #4*16
    st1.4s  {v4,v5,v6,v7}, [x8], #4*16
    st1.4s  {v16,v17,v18,v19}, [x8], #4*16
    st1.4s  {v20,v21,v22,v23}, [x8], #4*16
    st1.4s  {v24,v25,v26,v27}, [x8], #4*16
    st1.4s  {v28}, [x8], #1*16
#endif
    ld1.4s      {one,Lbswap}, [x7]
    
    // read mask 
    ldr     q5, [x7, ctr_len, uxtw #4] 

    ld1.4s  {ctr}, [pctr] 
    ld1.4s  {tag}, [ptag] 

    load_keys_192
  
    // byte swap ctr
    tbl.16b ctr, {ctr}, Lbswap 

    update_ctr_and_output_192
    subs    nblocks, nblocks, #1
    b.eq    9f      // L_Decrypt_done

0:      // L_Main_Loop:

    single_block_decrypt_192
    subs    nblocks, nblocks, #1
    b.gt    0b          // L_Main_Loop

9:      // L_Decrypt_done:

    final_tag_update_192

    // byte swap ctr and save to *ptr_ctx 
    tbl.16b ctr, {ctr}, Lbswap 
    st1.4s  {ctr}, [pctr]
    st1.4s  {tag}, [ptag]

#if CC_KERNEL
    ld1.4s  {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s  {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s  {v16,v17,v18,v19}, [sp], #4*16
    ld1.4s  {v20,v21,v22,v23}, [sp], #4*16
    ld1.4s  {v24,v25,v26,v27}, [sp], #4*16
    ld1.4s  {v28}, [sp], #1*16
#endif    

    ret     lr


    .globl _ccm256_decrypt
    .align 4
_ccm256_decrypt:


/* set up often used constants in registers */
    adrp        x7, L_ONE@page
    add         x7, x7, L_ONE@pageoff

#if CC_KERNEL
    sub     sp, sp, #23*16
    mov     x8, sp
    st1.4s  {v0,v1,v2,v3}, [x8], #4*16
    st1.4s  {v4,v5,v6,v7}, [x8], #4*16
    st1.4s  {v16,v17,v18,v19}, [x8], #4*16
    st1.4s  {v20,v21,v22,v23}, [x8], #4*16
    st1.4s  {v24,v25,v26,v27}, [x8], #4*16
    st1.4s  {v28,v29,v30}, [x8], #3*16
#endif
    ld1.4s      {one,Lbswap}, [x7]
    
    // read mask 
    ldr     q5, [x7, ctr_len, uxtw #4] 

    ld1.4s  {ctr}, [pctr] 
    ld1.4s  {tag}, [ptag] 

    load_keys_256
  
    // byte swap ctr
    tbl.16b ctr, {ctr}, Lbswap 

    update_ctr_and_output_256
    subs    nblocks, nblocks, #1
    b.eq    9f      // L_Decrypt_done

0:      // L_Main_Loop:

    single_block_decrypt_256
    subs    nblocks, nblocks, #1
    b.gt    0b          // L_Main_Loop

9:      // L_Decrypt_done:

    final_tag_update_256

    // byte swap ctr and save to *ptr_ctx 
    tbl.16b ctr, {ctr}, Lbswap 
    st1.4s  {ctr}, [pctr]
    st1.4s  {tag}, [ptag]

#if CC_KERNEL
    ld1.4s  {v0,v1,v2,v3}, [sp], #4*16
    ld1.4s  {v4,v5,v6,v7}, [sp], #4*16
    ld1.4s  {v16,v17,v18,v19}, [sp], #4*16
    ld1.4s  {v20,v21,v22,v23}, [sp], #4*16
    ld1.4s  {v24,v25,v26,v27}, [sp], #4*16
    ld1.4s  {v28,v29,v30}, [sp], #3*16
#endif    

    ret     lr

#endif  // __arm64__
#endif /* __NO_ASM__ */
