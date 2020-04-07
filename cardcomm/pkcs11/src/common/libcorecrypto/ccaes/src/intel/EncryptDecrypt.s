# 
#  Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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
/*	This file defines _vng_aes_encrypt or _vng_aes_decrypt, according to the value of
	the Select preprocessor symbol.  This file is designed to be included in
	another assembly file using the preprocessor #include directive, to benefit
	from some assembly-time calculations.

	These two routines are nearly identical.  They differ only in the tables
	they use, the direction they iterate through the key, and the permutation
	performed on part of the state.

	Written by Eric Postpischil, January 2008.
*/

// Jimmur:  I changed the name of the routines for Select == 0 and Select == 1 to
// allow for splitting out the VNG optimized AES and the VNG AESNI AES.

#if Select == 0
	#define	Name		_vng_aes_encrypt_opt		// Routine name.
	#define	MTable		_AESEncryptTable			// Main table.
	#define	FTable		_AESSubBytesWordTable		// Final table.
	#define	P0			S0							// State permutation.
	#define	P1			S1
	#define	P2			S2
	#define	P3			S3
	#define	Increment	+16							// ExpandedKey increment.
#elif Select == 1
	#define	Name		_vng_aes_decrypt_opt		// Routine name.
	#define	MTable		_AESDecryptTable			// Main table.
	#define	FTable		_AESInvSubBytesWordTable	// Final table.
	#define	P0			S2							// State permutation.
	#define	P1			S3
	#define	P2			S0
	#define	P3			S1
	#define	Increment	-16							// ExpandedKey increment.
#elif Select == 2
	#define	Name		_aes_encrypt_xmm_no_save	// Routine name.
	#define	MTable		_AESEncryptTable			// Main table.
	#define	FTable		_AESSubBytesWordTable		// Final table.
	#define	P0			S0							// State permutation.
	#define	P1			S1
	#define	P2			S2
	#define	P3			S3
	#define	Increment	+16							// ExpandedKey increment.
#elif Select == 3
	#define	Name		_aes_decrypt_xmm_no_save	// Routine name.
	#define	MTable		_AESDecryptTable			// Main table.
	#define	FTable		_AESInvSubBytesWordTable	// Final table.
	#define	P0			S2							// State permutation.
	#define	P1			S3
	#define	P2			S0
	#define	P3			S1
	#define	Increment	-16							// ExpandedKey increment.
#endif	// Select


/*	Routine:

		_AESEncryptWithExpandedKey (if Select is 0) or
		_AESDecryptWithExpandedKey (if Select is 1).

	Function:

		Perform the AES cipher or its inverse as defined in Federal Information
		Processing Standards Publication 197 (FIPS-197), November 26, 2001.

		The inverse cipher here is the "Equivalent Inverse Cipher" in FIPS-197.

	Input:

		Constant data:

			The following names must be locally defined so the assembler
			can calculate certain offsets.
				
			For encryption:

				static const Word _AESEncryptTable[4][256].

					_AESEncryptTable[i] contains the tables T[i] defined in AES
					Proposal: Rijndael, version 2, 03/09/99, by Joan Daemen and
					Vincent Rijmen, section 5.2.1, page 18.  These tables
					combine the SubBytes and MixColumns operations.

				static const Word _AESSubBytesWordTable[256].

					_AESSubBytesWordTable[i][j] = SubBytes(j) << 8*i, where
					SubBytes is defined in FIPS-197.  _AESSubBytesWordTable
					differs from _AESEncryptTable in that it does not include
					the MixColumn operation.  It is used in performing the last
					round, which differs fromm the previous rounds in that it
					does not include the MixColumn operation.

			For decryption:

				static const Word _AESDecryptTable[4][256].

					The analog of _AESEncryptTable for decryption.

				static const Word _AESSubBytesWordTable[256].

					_AESInvSubBytesWordTable[i][j] = InvSubBytes(j) << 8*i,
					where InvSubBytes is defined in FIPS-197.
					_AESInvSubBytesWordTable differs from _AESDecryptTable in
					that it does not include the InvMixColumn operation.  It is
					used in performing the last round, which differs from the
					previous rounds in that it does not include the
					InvMixColumn operation.

		Arguments:

			const Byte *InputText.

				Address of input, 16 bytes.  Best if four-byte aligned.

			Byte *OutputText.

				Address of output, 16 bytes.  Best if four-byte aligned.

			vng_aes_encrypt_ctx *Context or vng_aes_decrypt_ctx *Context

				vng_aes_encrypt_ctx and vng_aes_decrypt_ctx are identical except the
				former is used for encryption and the latter for decryption.

				Each is a structure containing the expanded key beginning at
				offset ContextKey and a four-byte "key length" beginning at
				offset ContextKeyLength.  The "key length" is the number of
				bytes from the start of the first round key to the start of the
				last round key.  That is 16 less than the number of bytes in
				the entire key.

	Output:

		Encrypted or decrypted data is written to *OutputText.

	Return:

		aes_rval	// -1 if "key length" is invalid.  0 otherwise.
*/

	.text
	.globl Name
Name:

// Jimmur removed the capabilities check and the just to the AESNI code.  This
// will be handled by the C code.

	// Push new stack frame.
	push	r5

	/*	Save registers and set SaveSize to the number of bytes pushed onto the
		stack so far, including the caller's return address.
	*/
	push	r3
	#if defined __i386__
		push	r6
		push	r7
		#define	SaveSize	(5*4)
	#else
		#define	SaveSize	(3*8)
	#endif

	/*	Number of bytes used for local variables:

			4 (i386) or 0 (x86_64) bytes for ExpandedKeyEnd.

			5 (i386) or 3 (x86_64) 16-byte spaces to save XMM registers.
	*/
	#define	LocalsSize	(Arch(4, 0) + Arch(5, 3)*16)

	#if 0 < LocalsSize
		// Padding to position stack pointer at a multiple of 16 bytes.
		#define	Padding	(15 & -(SaveSize + LocalsSize))
		sub		$Padding + LocalsSize, r4	// Allocate space on stack.
	#else
		#define	Padding	0
	#endif

#if CC_KERNEL
#if	Select < 2
	// Save XMM registers.
	movaps	%xmm0, 0*16(r4)
	movaps	%xmm1, 1*16(r4)
	movaps	%xmm2, 2*16(r4)
#if defined __i386__
	movaps	%xmm3, 3*16(r4)
	movaps	%xmm4, 4*16(r4)
#endif
#endif	// Select
#endif	// CC_KERNEL

#if defined __i386__

	// Number of bytes from caller's stack pointer to ours.
	#define	StackFrame	(SaveSize + Padding + LocalsSize)

	// Define location of argument i (presuming 4-byte arguments).
	#define	Argument(i)	StackFrame+4*(i)(%esp)

	#define	ArgInputText	Argument(0)
	#define	ArgOutputText	Argument(1)
	#define	ArgContext		Argument(2)

#elif defined __x86_64__

	// Arguments.
	#define	InputText		r7	// Used early then overwritten for other use.
	#define	OutputText		r6	// Needed near end of routine.
	#define	ArgContext		r2
		/*	The argument passed in r2 overlaps registers we need for other
		 	work, so it must be moved early in the routine.
		*/

#endif

#define	BaseP		Arch(r6, r9)	// Base pointer for addressing global data.
#define	ExpandedKey	Arch(t0, r10)	// Address of expanded key.

/*	The Work registers defined below are used to hold parts of the AES state
	while we dissect or assemble it.  They must be assigned to the A, B, C, and
	D registers so that we can access the bytes in %al, %ah, and so on.
*/
#define	Work0d	r0d
#define	Work0l	r0l
#define	Work0h	r0h
#define	Work1d	r3d
#define	Work1l	r3l
#define	Work1h	r3h
#define	Work2d	r1d
#define	Work2l	r1l
#define	Work2h	r1h
#define	Work3d	r2d
#define	Work3l	r2l
#define	Work3h	r2h

#define	t0		r5
#define	t0d		r5d		// Low 32 bits of t0.
#define	t0l		r5l		// Low byte of t0.

#define	t1		r7

/*	S0, S1, S2, and S3 are where we assemble the new AES state when computing
	a regular round.  S1, S2, and S3 are assigned to the Work registers, but
	S0 needs to go somewhere else because Work0 holds part of the old state.
*/
#define	S0		Arch(t1, r8d)
#define	S1		Work1d
#define	S2		Work2d
#define	S3		Work3d

/*	These XMM registers are used as holding space, because it is faster to
	spill to these registers than to the stack.  (On x86_64, we do not need
	to spill, because there are additional general registers available.
	However, using more general registers requires saving them to the stack
	and restoring them.  I timed it, and no time was saved.)
*/
#define	vS1		%xmm0
#define	vS2		%xmm1
#define	vS3		%xmm2
#if defined __i386__
	#define	vExpandedKey	%xmm3
	#define	vIncrement		%xmm4
#endif

	// Get address of expanded key.
	mov	ArgContext, ExpandedKey
	#if 0 != ContextKey
		add		$ContextKey, ExpandedKey
	#endif

/*	Store sentinel value of ExpandedKey on the stack on i386, a register on
 	x86_64.
*/
#define	ExpandedKeyEnd	Arch(5*16(r4), r11)

	// Get and check "key length".
	movzb	ContextKeyLength(ExpandedKey), r0
	cmp		$160, r0
	je		2f
	cmp		$192, r0
	je		2f
	cmp		$224, r0
	je		2f
	mov		$-1, r0		// Return error.
	jmp		9f
2:

	#if (Select == 0 || Select == 2)
		// For encryption, prepare to iterate forward through expanded key.
		add		ExpandedKey, r0
		mov		r0, ExpandedKeyEnd
	#else
		// For decryption, prepare to iterate backward through expanded key.
		mov		ExpandedKey, ExpandedKeyEnd
		add		r0, ExpandedKey
	#endif

	// Initialize State from input text.
	#if defined __i386__
		mov		ArgInputText, BaseP
		#define	InputText	BaseP
	#endif
	mov		0*4(InputText), Work0d
	mov		1*4(InputText), S1
	mov		2*4(InputText), S2
	mov		3*4(InputText), S3
#undef	InputText	// Register is reused after this for other purposes.

	// Add round key and save results.
	xor		0*4(ExpandedKey), Work0d	// S0 is in dissection register.
	xor		1*4(ExpandedKey), S1
	movd	S1, vS1						// Save S1 to S3 in vector registers.
	xor		2*4(ExpandedKey), S2
	movd	S2, vS2
	xor		3*4(ExpandedKey), S3
	movd	S3, vS3

	add		$Increment, ExpandedKey		 // Advance to next round key.

	#if defined __i386__
		// Save expanded key address and increment in vector registers.
		mov		$Increment, t1
		movp	ExpandedKey, vExpandedKey
		movp	t1, vIncrement
	#endif

	// Set up relative addressing.
	#if defined __i386__

		// Get address of 0 in BaseP.
			call	0f				// Push program counter onto stack.
		0:
			pop		BaseP			// Get program counter.

		// Define macros to help address data.
#define	LookupM(table, index)	MTable-0b+(table)*TableSize(BaseP, index, 4)
#define LookupF(table, index)	FTable-0b+(table)*TableSize(BaseP, index, 4)

	#elif defined __x86_64__

		lea	MTable(%rip), BaseP

		// Define macros to help address data.
		#define	LookupM(table, index)	(table)*TableSize(BaseP, index, 4)
		#define	LookupF(table, index)	(table)*TableSize(BaseP, index, 4)

/*	With these definitions of LookupM and LookupF, BaseP must be loaded with
	the address of the table at the point where it is used.  So we need an
	instruction to change BaseP after we are done with MTable and before we
	start using FTable.  I would prefer to use something like:

		.set	FMinusM, FTable - MTable
		#define LookupF(table, index)	\
			FMinusM+(table)*TableSize(BaseP, index, 4)

	Then BaseP would not need to change.  However, this fails due to an
	assembler/linker bug, <rdar://problem/5683882>.
*/

	#endif

	// Get round key.
	mov		0*4(ExpandedKey), S0
	mov		1*4(ExpandedKey), S1
	mov		2*4(ExpandedKey), S2
	mov		3*4(ExpandedKey), S3

1:
	/*	Word 0 of the current state must be in Work0 now, and the next round
		key must be in S0 to S3.
	*/

	// Process previous S0.
	movzb	Work0l, t0
	xor		LookupM(0, t0), S0
	movzb	Work0h, t0d
	xor		LookupM(1, t0), P3
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupM(2, t0), S2
	movzb	Work0h, t0d
	xor		LookupM(3, t0), P1

	// Process previous S1.
	movd	vS1, Work0d
	movzb	Work0l, t0d
	xor		LookupM(0, t0), S1
	movzb	Work0h, t0d
	xor		LookupM(1, t0), P0
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupM(2, t0), S3
	movzb	Work0h, t0d
	xor		LookupM(3, t0), P2

	// Process previous S2.
	movd	vS2, Work0d
	movzb	Work0l, t0d
	xor		LookupM(0, t0), S2
	movzb	Work0h, t0d
	xor		LookupM(1, t0), P1
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupM(2, t0), S0
	movzb	Work0h, t0d
	xor		LookupM(3, t0), P3

	// Process previous S3.
	movd	vS3, Work0d
	movzb	Work0l, t0d
	xor		LookupM(0, t0), S3
	movzb	Work0h, t0d
	xor		LookupM(1, t0), P2
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupM(2, t0), S1
	movzb	Work0h, t0d
	xor		LookupM(3, t0), P0

	#if defined __i386__
		paddd	vIncrement, vExpandedKey
		movp	vExpandedKey, ExpandedKey
	#else
		add		$Increment, ExpandedKey
	#endif

	// Save state for next iteration and load next round key.
	mov		S0, Work0d
	mov		0*4(ExpandedKey), S0
	movd	S1, vS1
	mov		1*4(ExpandedKey), S1
	movd	S2, vS2
	mov		2*4(ExpandedKey), S2
	movd	S3, vS3
	mov		3*4(ExpandedKey), S3

	cmp		ExpandedKeyEnd, ExpandedKey
	jne		1b

	/*	Word 0 of the current state must be in Work0 now, and the next round
		key must be in S0 to S3.
	*/

	// Work around assembler bug.  See comments above about Radar 5683882.
	#if defined __x86_64__
		lea	FTable(%rip), BaseP
	#endif

	// Process previous S0.
	movzb	Work0l, t0
	xor		LookupF(0, t0), S0
	movzb	Work0h, t0d
	xor		LookupF(1, t0), P3
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupF(2, t0), S2
	movzb	Work0h, t0d
	xor		LookupF(3, t0), P1

	// Process previous S1.
	movd	vS1, Work0d
	movzb	Work0l, t0d
	xor		LookupF(0, t0), S1
	movzb	Work0h, t0d
	xor		LookupF(1, t0), P0
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupF(2, t0), S3
	movzb	Work0h, t0d
	xor		LookupF(3, t0), P2

	// Process previous S2.
	movd	vS2, Work0d
	movzb	Work0l, t0d
	xor		LookupF(0, t0), S2
	movzb	Work0h, t0d
	xor		LookupF(1, t0), P1
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupF(2, t0), S0
	movzb	Work0h, t0d
	xor		LookupF(3, t0), P3

	// Process previous S3.
	movd	vS3, Work0d
	movzb	Work0l, t0d
	xor		LookupF(0, t0), S3
	movzb	Work0h, t0d
	xor		LookupF(1, t0), P2
	shr		$16, Work0d
	movzb	Work0l, t0d
	xor		LookupF(2, t0), S1
	movzb	Work0h, t0d
	xor		LookupF(3, t0), P0

	#if defined __i386__	// Architecture.
		// Get OutputText address.
		#define	OutputText	BaseP
		mov		ArgOutputText, OutputText
	#endif	// Architecture.

	// Write output.
	mov		S0, 0*4(OutputText)
	mov		S1, 1*4(OutputText)
	mov		S2, 2*4(OutputText)
	mov		S3, 3*4(OutputText)

	xor		r0, r0		// Return success.

9:
	// Pop stack and restore registers.
#if CC_KERNEL
#if	Select < 2
#if defined __i386__
	movaps	4*16(r4), %xmm4
	movaps	3*16(r4), %xmm3
#endif
	movaps	2*16(r4), %xmm2
	movaps	1*16(r4), %xmm1
	movaps	0*16(r4), %xmm0
#endif	// Select
#endif	// CC_KERNEL
	#if 0 < LocalsSize
		add		$Padding + LocalsSize, r4
	#endif
	#if defined __i386__
		pop		r7
		pop		r6
	#elif defined __x86_64__
	#endif
	pop		r3
	pop		r5

	ret


#undef	ArgExpandedKey
#undef	ArgInputText
#undef	ArgNr
#undef	ArgOutputText
#undef	Argument
#undef	BaseP
#undef	ExpandedKey
#undef	ExpandedKeyEnd
#undef	FTable
#undef	InputText
#undef	LocalsSize
#undef	LookupM
#undef	LookupF
#undef	MTable
#undef	OutputText
#undef	Padding
#undef	SaveSize
#undef	S0
#undef	S1
#undef	S2
#undef	S3
#undef	StackFrame
#undef	Work0d
#undef	Work0h
#undef	Work0l
#undef	Work1d
#undef	Work1h
#undef	Work1l
#undef	Work2d
#undef	Work2h
#undef	Work2l
#undef	Work3d
#undef	Work3h
#undef	Work3l
#undef	t0
#undef	t0d
#undef	t0l
#undef	t1
#undef	vExpandedKey
#undef	vS1
#undef	vS2
#undef	vS3

#undef	Name
#undef	MTable
#undef	FTable
#undef	P0
#undef	P1
#undef	P2
#undef	P3
#undef	Increment
#endif /* __NO_ASM__ */
