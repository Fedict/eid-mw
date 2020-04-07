# 
#  Copyright (c) 2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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

#if defined(__arm64__)
#include "ccarm_intrinsic_compatability.h"

	// per block
	#define in      x0
    #define out     x1
    #define key     x2
    #define keylen  x3
    #define t       x5


	.text	
	.align	4	
	    .globl _ccaes_arm_decrypt
_ccaes_arm_decrypt:

#if CC_KERNEL
	// save used vector registers
	sub		sp, sp, #3*16
	st1.4s		{v0,v1,v2}, [sp]
#endif

    ldr     w3, [key, #240]         // keylength = 32-bit
    ldr     q0, [in]                // plain data
    mov     t, keylen
    ldr     q1, [key, t]		        // expanded key
	sub		t, t, #16
    ldr     q2, [key]               // expanded key
0:
    AESD    0, 1
    AESIMC   0, 0
    ldr     q1, [key, t]				// expanded key
    subs    t, t, #16
    b.gt    0b
    AESD    0, 1
    eor.16b v0, v0, v2
    str     q0, [out]

#if CC_KERNEL
	// restore used vector registers
	ld1.4s		{v0,v1,v2}, [sp], #48
#endif

    ret     lr

	#undef in
    #undef out
    #undef key
    #undef keylen


	// ecb mode

    #define key     x0
	#define	nblocks	w1
	#define in      x2
    #define out     x3
    #define keylen  x4


	    .globl _ccaes_arm_decrypt_ecb
	.align	4
_ccaes_arm_decrypt_ecb:

#if CC_KERNEL
    // save used vector registers
    sub     x4, sp, #6*16
    sub     sp, sp, #6*16
    st1.4s      {v0,v1,v2,v3}, [x4], #4*16
    st1.4s      {v4,v5}, [x4], #2*16
#endif

    ldr     w4, [key, #240]             // keylength = 32-bit
    ldr     q5, [key]               // expanded key
	subs	nblocks, nblocks, #4
	b.lt	L_lessthan4

L_4blocks:
    mov     t, keylen
	ld1.4s	{v0,v1,v2,v3}, [in], #4*16
    ldr     q4, [key, t]	        // expanded key
    sub     t, t, #16
0:
    AESD     0, 4
    AESIMC   0, 0
    AESD     1, 4
    AESIMC   1, 1
    AESD     2, 4
    AESIMC   2, 2
    AESD     3, 4
    AESIMC   3, 3
    ldr         q4, [key, t]				// expanded key
    subs        t, t, #16
    b.gt        0b
    AESD     0, 4
    eor.16b v0, v0, v5
    AESD     1, 4
    eor.16b v1, v1, v5
    AESD     2, 4
    eor.16b v2, v2, v5
    AESD     3, 4
    eor.16b v3, v3, v5

	st1.4s		{v0,v1,v2,v3}, [out], #4*16

	subs	nblocks, nblocks, #4
	b.ge	L_4blocks

L_lessthan4:
	ands	nblocks, nblocks, #3
	b.eq	9f

L_1block:
    mov     t, keylen
    ldr     q0, [in], #16          // plain data
    ldr     q4, [key, t]	        // expanded key
    sub     t, t, #16
0:
    AESD    0, 4
    AESIMC   0, 0
    ldr     q4, [key, t]			// expanded key
    subs        t, t, #16
    b.gt        0b

    AESD    0, 4
    eor.16b v0, v0, v5

    str     q0, [out], #16
	subs	nblocks, nblocks, #1
	b.gt	L_1block

9:
#if CC_KERNEL
	// restore used vector registers
	ld1.4s		{v0,v1,v2,v3}, [sp], #4*16
	ld1.4s		{v4,v5}, [sp], #2*16
#endif

    ret     lr

	#undef in
    #undef out
    #undef key
	#undef nblocks
    #undef keylen

#else

	#define	Select	1		// Select=1 to define aes_decryptc from EncryptDecrypt.s
	#include "EncryptDecrypt.s"
	#undef	Select

#endif
#endif /* __NO_ASM__ */
