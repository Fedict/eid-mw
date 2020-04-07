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
    gcm_encrypt((void*) pt, (void*) ct, (void*)  _CCMODE_GCM_KEY(key), j, (void*) HTable, (void*) CCMODE_GCM_KEY_ECB_KEY(key));

        CT = PT ^ AES_Encrypt_output;
        key->X ^= CT;
        key->X = key->X * key->H (ccmode_gcm_mult_h);
        key->Y++;
        AES_Encrypt_output = AES_Encrypt(key->Y);
        
*/

    #define     in  r4
    #define     out r5
    #define     gcm_key r6
    #define     nsize   r8
    #define     HTable  r10
    #define     ecb_key r11

    #define     key_pad     q1
    #define     X           q2
    #define     Y           q3

    .align      6
L_bswap:
    .byte   15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
    .word   1, 0, 0, 0

    .extern     _gcm_gmult              // r0 -> X r1 -> HTable
    .extern     _ccaes_arm_encrypt

    .text   
    .align  4
    .globl  _gcmEncrypt

_gcmEncrypt:

    mov     r12, sp
    push    {r4-r6,r8-r11,lr}
    vstmdb  sp!, {q4-q7}
#if CC_KERNEL
    vstmdb  sp!, {q8-q10}
    vstmdb  sp!, {q0-q3}
#endif

    mov     in, r0
    mov     out, r1
    mov     gcm_key, r2
    mov     nsize, r3
    ldr     HTable, [r12, #0]
    ldr     ecb_key, [r12, #4]
    adr     r9, L_bswap
    add     r12, gcm_key, #16
    vldmia  r9, {q2-q3}
    vldmia  r12, {q4-q5}           // q4 = X, q5 = Y
    vmov    q7, q3                // one  

    vtbl.8  d12, {q5}, d4         // q6 is byte reversed Y
    vtbl.8  d13, {q5}, d5
    vmov    q5, q2
    
0:
    // Q0 = PT
    vld1.8      {q0}, [in]!

    // Q1 = AES_Encrypt_output
    vldr        d2, [gcm_key, #64]
    vldr        d3, [gcm_key, #72]

    veor    q0, q0, q1          // CT =  PT ^ AES_Encrypt_output;
    veor    q4, q4, q0          // key->X ^= CT

    add     r0, gcm_key, #16    // r0 -> key->X;
    mov     r1, HTable          // r1 -> HTable;

    vst1.8  {q4}, [r0]          // update key->X;
    vst1.8  {q0}, [out]!        // write output ciphertext;

    blx     _gcm_gmult          // call gcm_gmult to update key->X;

    add     r12, gcm_key, #16

    vadd.i32    q6, q6, q7      // Y++;
    add         r0, gcm_key, #32
    vld1.8      {q4}, [r12]     // load updated key->X
    vtbl.8      d2, {q6}, d10   // byte swapped back to little-endian (to call _ccaes_arm_encrypt)
    add         r1, r12, #48    // output in gcm ctx KEY_PAD
    vtbl.8      d3, {q6}, d11
    mov         r2, ecb_key     // expanded AES Key

    // update Y in the gcm ctx
    vstr        d2, [r12, #16] 
    vstr        d3, [r12, #24] 

    blx         _ccaes_arm_encrypt

    subs        nsize, nsize, #16
    bgt         0b

#if CC_KERNEL
    vldmia  sp!, {q0-q3}
    vldmia  sp!, {q8-q10}
#endif
    vldmia  sp!, {q4-q7}

    pop     {r4-r6,r8-r11,pc}

/*
    gcm_decrypt((void*) ct, (void*) pt, (void*)  _CCMODE_GCM_KEY(key), j, (void*) HTable, (void*) CCMODE_GCM_KEY_ECB_KEY(key));

        key->X ^= CT;
        PT = CT ^ AES_Encrypt_output;
        key->X = key->X * key->H (ccmode_gcm_mult_h);
        key->Y++;
        AES_Encrypt_output = AES_Encrypt(key->Y);
        
*/
    .align  4
    .globl  _gcmDecrypt

_gcmDecrypt:

    mov     r12, sp
    push    {r4-r6,r8-r11,lr}
    vstmdb  sp!, {q4-q7}
#if CC_KERNEL
    vstmdb  sp!, {q8-q10}
    vstmdb  sp!, {q0-q3}
#endif

    mov     in, r0
    mov     out, r1
    mov     gcm_key, r2
    mov     nsize, r3
    ldr     HTable, [r12, #0]
    ldr     ecb_key, [r12, #4]
    adr     r9, L_bswap
    add     r12, gcm_key, #16
    vldmia  r9, {q2-q3}
    vldmia  r12, {q4-q5}           // q4 = X, q5 = Y
    vmov    q7, q3                // one  

    vtbl.8  d12, {q5}, d4         // q6 is byte reversed Y
    vtbl.8  d13, {q5}, d5
    vmov    q5, q2
    
0:
    // Q0 = CT;
    vld1.8  {q0}, [in]!

    // Q1 = AES_Encrypt_output;
    vldr        d2, [gcm_key, #64]
    vldr        d3, [gcm_key, #72]

    veor    q4, q4, q0          // key->X ^= CT;
    veor    q0, q0, q1          // PT = CT ^ AES_Encrypt_output;

    add     r0, gcm_key, #16    // r0 -> key->X;
    mov     r1, HTable          // r1 -> HTable

    vst1.8  {q4}, [r0]          // write to key->X (input to _gcm_gmult)
    vst1.8  {q0}, [out]!        // write output vector

    blx     _gcm_gmult

    add         r12, gcm_key, #16   // r12 -> key->X
    vadd.i32    q6, q6, q7          // Y++;
    add         r0, gcm_key, #32    // r0 -> key->Y
    vld1.8      {q4}, [r12]         // reload Q4 = key->X
    vtbl.8      d2, {q6}, d10       // byte swap Y and save to key->Y, to call _ccaes_arm_encrypt to compute AES_Encrypt(key->Y);
    add         r1, r12, #48        // r1 -> key->KEY_PAD (AES_Encrypt_output);
    vtbl.8      d3, {q6}, d11
    mov         r2, ecb_key         // r2 -> expanded AES Key

    // update Y in the gcm ctx
    vstr        d2, [r12, #16] 
    vstr        d3, [r12, #24] 

    blx         _ccaes_arm_encrypt

    subs        nsize, nsize, #16
    bgt         0b

#if CC_KERNEL
    vldmia  sp!, {q0-q3}
    vldmia  sp!, {q8-q10}
#endif
    vldmia  sp!, {q4-q7}

    pop     {r4-r6,r8-r11,pc}


#endif

#endif  // CCAES_ARM
#endif /* __NO_ASM__ */
