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
/*
	This (armv5 isa) assembly file was derived by porting from the i386/x86_64 EncryptDecrypt.s, plus 
	adding the cbc mode function wrapper.
	
    This file is a code template to define the CBC mode AES functions
		_aes_encrypt_cbc
		_aes_decrypt_cbc
	depending on the value of the Select preprocessor symbol. 
	It can be used to define _aes_encrypt_cbc with Select=0.
    Otherwise, it defines _aes_decrypt_cbc. 

	The atomic functions aes_encrypt/aes_decrypt can be similarly defined from EncryptDecrypt.s

	By calling directly these cbc mode functions, instead of writing C functions and calling the
	atomic functions aes_encrypt/aes_decrypt per 16-byte block, significant amount of overhead is saved.
	This improves the cost from ~ 36 cycles/byte to ~ 29 cycles/byte in cbc mode applications on cortex-a8.

    One important change from i386/x86_64 is that the fact that the 2nd operand for arm architecture
    comes from the rotated/shifted output of the barrel shifter is exploited to save usage of GPU
    and to save the size of the lookup tables. For all the 4 used lookup tables, here we only
    need the 1st quarter of the original tables in the original i386/Data.s

*/

#if CCAES_ARM

// AES 16-byte state sits on r0-r3
#define	S0	r0
#define	S1	r1
#define	S2	r2
#define	S3	r3

#if Select == 0
	#define	Name		_ccaes_arm_encrypt_cbc		// Routine name.
	#define	MTable		_AESEncryptTable			// Main table.
	#define	FTable		_AESSubBytesWordTable		// Final table.
	#define	P0			S0							// State permutation.
	#define	P1			S1
	#define	P2			S2
	#define	P3			S3
	#define	Increment	+16							// ExpandedKey increment.
#else
	#define	Name		_ccaes_arm_decrypt_cbc		// Routine name.
	#define	MTable		_AESDecryptTable			// Main table.
	#define	FTable		_AESInvSubBytesWordTable	// Final table.
	#define	P0			S2							// State permutation.
	#define	P1			S3
	#define	P2			S0
	#define	P3			S1
	#define	Increment	-16							// ExpandedKey increment.

#endif	// Select

/*

	our cbc-mode level implementation of aes_encrypt_cbc(*ibuf,*iv_in,nb,*obuf,*ctx) is as follows:

	// all variables represent 16-byte blocks.
	*iv = *iv_in;         // *iv_in is declared const, so we should not change its value
	while (nb--) {
		*iv ^= *ibuf;
		aes_encrypt(iv,iv,ctx);
		memcpy(obuf,iv,AES_BLOCK_SIZE);
		ibuf += AES_BLOCK_SIZE;
		obuf += AES_BLOCK_SIZE;
	}

	our cbc-mode level implementation of aes_decrypt_cbc(*ibuf,*in_iv,nb,*obuf,*ctx) is as follows:

	memcpy(iv,in_iv,AES_BLOCK_SIZE);
	while (nb--) {
		memcpy(tmp,ibuf,AES_BLOCK_SIZE);
		aes_decrypt(ibuf,obuf,ctx);
		*obuf ^= *iv;
		memcpy(iv,tmp,AES_BLOCK_SIZE);
		ibuf += AES_BLOCK_SIZE;
		obuf += AES_BLOCK_SIZE;
	}
	
*/

#define	ExpandedKey			r11
#define	ExpandedKeyEnd		lr
#define	ContextKeyLength	240	
#define	t					r12

	.text
	.align	4
	.globl Name
Name:

	ldr		t, [sp, #0]			// load the 5th calling argument (ctx) before we move the stack pointer

	// set up debug trace frame pointer
	push	{r7,lr}				// set up frame pointer for debug tracing
	mov		r7, sp

	// now setup the stack for the current function
	push	{r0-r6,r8,r10-r11}		// saved ibuf,nb,obuf in the stack, need to use r0-r3 for the 16-byte state
	sub		sp, #64

	// set up the destination ExpandedKey[],
	mov		ExpandedKey, t		// the 1st thing inside ctx is ExpandedKey[] 
	
	// Get and check "key length".
	ldr		t, [ExpandedKey, #ContextKeyLength]

	cmp		t, #160
	beq		2f
	cmp		t, #192
	beq		2f
	cmp		t, #224
	beq		2f
	
	mov		r0, #-1		// Return error.
	b		9f
2:
	str		t, [sp, #32]		// save a copy of keylength for future reference
	
	#if (Select == 0)
		// For encryption, prepare to iterate forward through expanded key.
		add		ExpandedKeyEnd, ExpandedKey, t
	#else
		// For decryption, prepare to iterate backward through expanded key.
		mov		ExpandedKeyEnd, ExpandedKey
		add		ExpandedKey, t
	#endif

	mov		r9, #0xff			// to be used as a byte mask

#if (Select == 0)
    // copy iv_in to local tmp (encrypt)
    add     r10, sp, #16
	ldr		r4, [r1]
	ldr		r5, [r1, #4]
	ldr		r6, [r1, #8]
	ldr		r8, [r1, #12]
#else
    // memcpy(iv,in_iv,16);
    add     r10, sp, #48
	ldr		r4, [r1]
	ldr		r5, [r1, #4]
	ldr		r6, [r1, #8]
	ldr		r8, [r1, #12]
#endif
	stmia	r10, {r4-r6,r8}
	
L_loop_head:

	// while (num_blocks--) {

	ldr		t, [sp, #72]			// num_blocks
	subs	t, #1					// num_blocks--
	eorlt	r0, r0					// in case num_blocks <= 0, to return 0	
	blt		9f
	str		t, [sp, #72]			// save updated num_block	

	// reset *ExpandedKey based on the unchanged *ExpandedKeyEnd

	ldr		t, [sp, #32]			// load keylength
	#if (Select == 0)
		sub		ExpandedKey, ExpandedKeyEnd, t
	#else
		add		ExpandedKey, ExpandedKeyEnd, t
	#endif

#if (Select == 0)

	// tmp ^= ibuf

	add		r10, sp, #16			// *tmp
	ldr 	t, [sp, #64]			// *ibuf	
	ldmia	r10, {r0-r3}
	ldr		r4, [t], #4
	ldr		r5, [t], #4
	ldr		r6, [t], #4
	ldr		r8, [t], #4
	str		t, [sp, #64]			// update *ibuf so it points to next block
	
	// aes state now sits in r0-r3
	eor		r0, r4
	eor		r1, r5
	eor		r2, r6
	eor		r3, r8
	

#else				// decrypt_cbc

	// tmp = ibuf
	ldr 	t, [sp, #64]			// *ibuf	
	add		r10, sp, #16			// *tmp
	ldr		r0, [t], #4
	ldr		r1, [t], #4
	ldr		r2, [t], #4
	ldr		r3, [t], #4
	str		t, [sp, #64]			// update *ibuf so it points to next block
	stmia	r10, {r0-r3}			// tmp = *ibuf

#endif

	// the following is performing aes_encrypt(tmp, tmp, ctx); or aes_decrypt(ibuf, obuf, ctx);

	// 1st expanded round key.
	ldmia	ExpandedKey, {r4-r6,r8}
	add		ExpandedKey, #Increment

	// 1st Add round key.
	eor		S0, r4
	eor		S1, r5
	eor		S2, r6
	eor		S3, r8
	
	// Set up Main Table address.
#ifdef CC_STATIC
	adr		r6, L_table1
	ldr		r6, [r6]
#else
    ldr		r6, L_table1
L_table0:	
    ldr		r6, [pc, r6]
#endif

	// save S0-S3 in STACK
	stmia	sp, {S0-S3}

	// Get round key.
	ldmia	ExpandedKey,{S0-S3}
	add		ExpandedKey, #Increment 

	/*
		the following macro defines the per round operation for aes
		the state computed from the previous round is now saved in sp[0:15]
		and r0-r3 has been initialized with the next expanded round key
		the macro reads those 16 bytes in sp[0:15] and for each byte does a table look up
		the result (4-byte) word is xor-ed to one of r0-r3
		the final r0-r3 is the aes state
		r6 : points to Main or Final table
		r9 : 0xff is used as a byte mask
	*/
	
	.macro	aes_per_round
#if defined (__ARM_ARCH_7S__)
    // the following implementation is better for swift (and the old cortex-a8)
	ldr		t, [sp, #0]					// 1st 4 state bytes
	and		r4, r9, t					// 1st byte a
	and		r5, r9, t, lsr #8			// 1st byte b
	ldr		r4, [r6, r4, lsl #2]		// table(a)
	and		r8, r9, t, lsr #16			// 1st byte c
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	and		r10, r9, t, lsr #24			// 1st byte(d)
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	eor		S0, r4						// S0 ^= table(a)
	ldr		r10, [r6, r10, lsl #2]		// table(d)
	eor		P3, r5, ror #24				// P3 ^= ror(table(b),24)
	ldr		t, [sp, #4]					// 2nd 4 state bytes
	eor		S2, r8, ror #16				// S2 ^= ror(table(c),16)
	eor		P1, r10, ror #8				// P1 ^= ror(table(d),8)
	
	and		r4, r9, t					// 2nd a
	and		r5, r9, t, lsr #8			// 2nd b
	ldr		r4, [r6, r4, lsl #2]		// table(a)
	and		r8, r9, t, lsr #16			// 2nd c
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	and		r10, r9, t, lsr #24			// 2nd d
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	eor		S1, r4						// S1 ^= table(a)
	ldr		r10, [r6, r10, lsl #2]		// table(d)
	eor		P0, r5, ror #24				// P0 ^= ror(table(b),24)
	ldr		t, [sp, #8]					// 3rd 4 state bytes
	eor		S3, r8, ror #16				// S3 ^= ror(table(c),16)
	eor		P2, r10, ror #8				// P2 ^= ror(table(d),8)

	and		r4, r9, t					// 3rd a
	and		r5, r9, t, lsr #8			// 3rd b
	ldr		r4, [r6, r4, lsl #2]		// table(a)
	and		r8, r9, t, lsr #16			// 3rd c
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	and		r10, r9, t, lsr #24			// 3rd d
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	eor		S2, r4						// S2 ^= table(a)
	ldr		r10, [r6, r10, lsl #2]		// table(d)
	eor		P1, r5, ror #24				// P1 ^= ror(table(b),24) 
	ldr		t, [sp, #12]				// last 4 state bytes
	eor		S0, r8, ror #16				// S0 ^= ror(table(c),16)
	eor		P3, r10, ror #8				// P3 ^= ror(table(d),8)

	and		r4, r9, t					// 4th a
	and		r5, r9, t, lsr #8			// 4th b
	ldr		r4, [r6, r4, lsl #2]		// table(a)
	and		r8, r9, t, lsr #16			// 4th c
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	and		r10, r9, t, lsr #24			// 4th d
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	eor		S3, r4						// S3 ^= table(a)
	ldr		r10, [r6, r10, lsl #2]		// table(d)
	eor		P2, r5, ror #24				// P2 ^= ror(table(b),24)
	eor		S1, r8, ror #16				// S1 ^= ror(table(c),16)
	eor		P0, r10, ror #8				// P0 ^= ror(table(d),8)

#else
    // the following implementation is better for cortex-a7 and cortex-a9
	ldrb	r4, [sp, #0]				// 1st byte a
	ldrb	r5, [sp, #1]				// 1st byte(b)
	ldrb	r8, [sp, #2]				// 1st byte c
	ldrb	r10, [sp, #3]				// 1st byte(d)

	ldr		r4, [r6, r4, lsl #2]		// table(a)
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	ldr		r10, [r6, r10, lsl #2]		// table(d)

	eor		S0, r4						// S0 ^= table(a)
	ldrb	r4, [sp, #4]				// 2nd a
	eor		P3, r5, ror #24				// P3 ^= ror(table(b),24)
	ldrb	r5, [sp, #5]				// 2nd b
	eor		S2, r8, ror #16				// S2 ^= ror(table(c),16)
	ldrb	r8, [sp, #6]				// 2nd c
	eor		P1, r10, ror #8				// P1 ^= ror(table(d),8)
	ldrb	r10, [sp, #7]				// 2nd d

	ldr		r4, [r6, r4, lsl #2]		// table(a)
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	ldr		r10, [r6, r10, lsl #2]		// table(d)

	eor		S1, r4						// S1 ^= table(a)
	ldrb	r4, [sp, #8]				// 3rd a
	eor		P0, r5, ror #24				// P0 ^= ror(table(b),24)
	ldrb	r5, [sp, #9]				// 3rd b
	eor		S3, r8, ror #16				// S3 ^= ror(table(c),16)
	ldrb	r8, [sp, #10]				// 3rd c
	eor		P2, r10, ror #8				// P2 ^= ror(table(d),8)
	ldrb	r10, [sp, #11]				// 3rd d

	ldr		r4, [r6, r4, lsl #2]		// table(a)
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	ldr		r10, [r6, r10, lsl #2]		// table(d)

	eor		S2, r4						// S2 ^= table(a)
	ldrb	r4, [sp, #12]				// 4th a
	eor		P1, r5, ror #24				// P1 ^= ror(table(b),24) 
	ldrb	r5, [sp, #13]				// 4th b
	eor		S0, r8, ror #16				// S0 ^= ror(table(c),16)
	ldrb	r8, [sp, #14]				// 4th c
	eor		P3, r10, ror #8				// P3 ^= ror(table(d),8)
	ldrb	r10, [sp, #15]				// 4th d

	ldr		r4, [r6, r4, lsl #2]		// table(a)
	ldr		r5, [r6, r5, lsl #2]		// table(b)
	ldr		r8, [r6, r8, lsl #2]		// table(c)
	ldr		r10, [r6, r10, lsl #2]		// table(d)

	eor		S3, r4						// S3 ^= table(a)
	eor		P2, r5, ror #24				// P2 ^= ror(table(b),24)
	eor		S1, r8, ror #16				// S1 ^= ror(table(c),16)
	eor		P0, r10, ror #8				// P0 ^= ror(table(d),8)
#endif
	.endm

	.macro	aes_last_round
#if defined (__ARM_ARCH_7S__)
    // the following implementation is better for swift (and the old cortex-a8)
	ldr		t, [sp, #0]					// 1st 4 state bytes
	and		r4, r9, t					// 1st byte a
	and		r5, r9, t, lsr #8			// 1st byte b
	ldrb	r4, [r6, r4]		// table(a)
	and		r8, r9, t, lsr #16			// 1st byte c
	ldrb	r5, [r6, r5]		// table(b)
	and		r10, r9, t, lsr #24			// 1st byte(d)
	ldrb	r8, [r6, r8]		// table(c)
	eor		S0, r4						// S0 ^= table(a)
	ldrb	r10, [r6, r10]		// table(d)
	eor		P3, r5, ror #24				// P3 ^= ror(table(b),24)
	ldr		t, [sp, #4]					// 2nd 4 state bytes
	eor		S2, r8, ror #16				// S2 ^= ror(table(c),16)
	eor		P1, r10, ror #8				// P1 ^= ror(table(d),8)
	
	and		r4, r9, t					// 2nd a
	and		r5, r9, t, lsr #8			// 2nd b
	ldrb	r4, [r6, r4]		// table(a)
	and		r8, r9, t, lsr #16			// 2nd c
	ldrb	r5, [r6, r5]		// table(b)
	and		r10, r9, t, lsr #24			// 2nd d
	ldrb	r8, [r6, r8]		// table(c)
	eor		S1, r4						// S1 ^= table(a)
	ldrb	r10, [r6, r10]		// table(d)
	eor		P0, r5, ror #24				// P0 ^= ror(table(b),24)
	ldr		t, [sp, #8]					// 3rd 4 state bytes
	eor		S3, r8, ror #16				// S3 ^= ror(table(c),16)
	eor		P2, r10, ror #8				// P2 ^= ror(table(d),8)

	and		r4, r9, t					// 3rd a
	and		r5, r9, t, lsr #8			// 3rd b
	ldrb	r4, [r6, r4]		// table(a)
	and		r8, r9, t, lsr #16			// 3rd c
	ldrb	r5, [r6, r5]		// table(b)
	and		r10, r9, t, lsr #24			// 3rd d
	ldrb	r8, [r6, r8]		// table(c)
	eor		S2, r4						// S2 ^= table(a)
	ldrb	r10, [r6, r10]		// table(d)
	eor		P1, r5, ror #24				// P1 ^= ror(table(b),24) 
	ldr		t, [sp, #12]				// last 4 state bytes
	eor		S0, r8, ror #16				// S0 ^= ror(table(c),16)
	eor		P3, r10, ror #8				// P3 ^= ror(table(d),8)

	and		r4, r9, t					// 4th a
	and		r5, r9, t, lsr #8			// 4th b
	ldrb	r4, [r6, r4]		// table(a)
	and		r8, r9, t, lsr #16			// 4th c
	ldrb	r5, [r6, r5]		// table(b)
	and		r10, r9, t, lsr #24			// 4th d
	ldrb	r8, [r6, r8]		// table(c)
	eor		S3, r4						// S3 ^= table(a)
	ldrb	r10, [r6, r10]		// table(d)
	eor		P2, r5, ror #24				// P2 ^= ror(table(b),24)
	eor		S1, r8, ror #16				// S1 ^= ror(table(c),16)
	eor		P0, r10, ror #8				// P0 ^= ror(table(d),8)
#else
    // the following implementation is better for cortex-a7 and cortex-a9
	ldrb	r4, [sp, #0]				// 1st byte a
	ldrb	r5, [sp, #1]				// 1st byte(b)
	ldrb	r8, [sp, #2]				// 1st byte c
	ldrb	r10, [sp, #3]				// 1st byte(d)

	ldrb	r4, [r6, r4]			// table(a)
	ldrb	r5, [r6, r5]			// table(b)
	ldrb	r8, [r6, r8]			// table(c)
	ldrb	r10, [r6, r10]			// table(d)

	eor		S0, r4						// S0 ^= table(a)
	ldrb	r4, [sp, #4]				// 2nd a
	eor		P3, r5, ror #24				// P3 ^= ror(table(b),24)
	ldrb	r5, [sp, #5]				// 2nd b
	eor		S2, r8, ror #16				// S2 ^= ror(table(c),16)
	ldrb	r8, [sp, #6]				// 2nd c
	eor		P1, r10, ror #8				// P1 ^= ror(table(d),8)
	ldrb	r10, [sp, #7]				// 2nd d

	ldrb	r4, [r6, r4]		// table(a)
	ldrb	r5, [r6, r5]		// table(b)
	ldrb	r8, [r6, r8]		// table(c)
	ldrb	r10, [r6, r10]		// table(d)

	eor		S1, r4						// S1 ^= table(a)
	ldrb	r4, [sp, #8]				// 3rd a
	eor		P0, r5, ror #24				// P0 ^= ror(table(b),24)
	ldrb	r5, [sp, #9]				// 3rd b
	eor		S3, r8, ror #16				// S3 ^= ror(table(c),16)
	ldrb	r8, [sp, #10]				// 3rd c
	eor		P2, r10, ror #8				// P2 ^= ror(table(d),8)
	ldrb	r10, [sp, #11]				// 3rd d

	ldrb	r4, [r6, r4]		// table(a)
	ldrb	r5, [r6, r5]		// table(b)
	ldrb	r8, [r6, r8]		// table(c)
	ldrb	r10, [r6, r10]		// table(d)

	eor		S2, r4						// S2 ^= table(a)
	ldrb	r4, [sp, #12]				// 4th a
	eor		P1, r5, ror #24				// P1 ^= ror(table(b),24) 
	ldrb	r5, [sp, #13]				// 4th b
	eor		S0, r8, ror #16				// S0 ^= ror(table(c),16)
	ldrb	r8, [sp, #14]				// 4th c
	eor		P3, r10, ror #8				// P3 ^= ror(table(d),8)
	ldrb	r10, [sp, #15]				// 4th d

	ldrb	r4, [r6, r4]		// table(a)
	ldrb	r5, [r6, r5]		// table(b)
	ldrb	r8, [r6, r8]		// table(c)
	ldrb	r10, [r6, r10]		// table(d)

	eor		S3, r4						// S3 ^= table(a)
	eor		P2, r5, ror #24				// P2 ^= ror(table(b),24)
	eor		S1, r8, ror #16				// S1 ^= ror(table(c),16)
	eor		P0, r10, ror #8				// P0 ^= ror(table(d),8)
#endif
	.endm
1:
	aes_per_round

	// Save state for next iteration 
	stmia	sp,{S0-S3}
	
	// load next round key.
	ldmia	ExpandedKey,{S0-S3}

	// repeat a round if ExpandedKey != ExpandedKeyEnd
	
	cmp		ExpandedKeyEnd, ExpandedKey
	add		ExpandedKey, #Increment 
	bne		1b
	
	// Set up Final Table address.
#ifdef CC_STATIC
	adr		r6, L_table3
	ldr		r6, [r6]
#else
    ldr		r6, L_table3
L_table2:	
    ldr		r6, [pc, r6]
#endif

	// perform the final round
	aes_last_round

#if (Select==0)

	add		r10, sp, #16
	ldr		r4, [sp, #76]			// *obuf
	stmia	r10, {S0-S3}			// write local iv
	str		S0, [r4], #4
	str		S1, [r4], #4
	str		S2, [r4], #4
	str		S3, [r4], #4
	str		r4, [sp, #76]			// update obuf++

#else

	add		r10, sp, #48			// *iv
	ldmia	r10, {r4-r6,r8}
	eor		r0, r4
	eor		r1, r5
	eor		r2, r6
	eor		r3, r8
	ldr		r4, [sp, #76]			// *obuf
	str		S0, [r4], #4
	str		S1, [r4], #4
	str		S2, [r4], #4
	str		S3, [r4], #4
	add		r5, sp, #16 			// tmp
	str		r4, [sp, #76]			// update obuf++
	ldmia	r5, {r0-r3}
	stmia	r10, {r0-r3}			// iv = tmp

#endif

	b		L_loop_head

9:

	add		sp, #80				// 64 + 16 (no need to pop r0-r3)
	pop		{r4-r6,r8,r10-r11}
	pop		{r7,pc}

#ifdef CC_STATIC
	.align 	2
L_table1:
	.long	MTable
L_table3:
	.long	FTable	
#else
	.align 	2
L_table1:
    .long   L_Tab$non_lazy_ptr-(L_table0+8)

	.align 	2
L_table3:
    .long   L_Tab$non_lazy_ptr2-(L_table2+8)

    .section    __DATA,__nl_symbol_ptr,non_lazy_symbol_pointers
    .align  2
L_Tab$non_lazy_ptr:
    .indirect_symbol    MTable
    .long   0

    .align  2
L_Tab$non_lazy_ptr2:
    .indirect_symbol    FTable
    .long   0
#endif

#undef	S0
#undef	S1
#undef	S2
#undef	S3
#undef	Name
#undef	MTable
#undef	FTable
#undef	P0
#undef	P1
#undef	P2
#undef	P3
#undef	Increment

#endif /* CCAES_ARM */
#endif /* __NO_ASM__ */
