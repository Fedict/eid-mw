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

#ifndef __NO_ASM__
/*	AES.s -- Core AES routines for Intel processors.

	Written by Eric Postpischil, January 30, 2008.
*/
#include <corecrypto/cc_config.h>
#if CCAES_INTEL && (defined __i386__ || defined __x86_64__)


/*	We build these AES routines as a single module because the routines refer
	to labels in Data.s and it is easier and faster to refer to them as local
	labels.  In my implementations of AES for CommonCrypto, both i386 and
	x86_64 use position-independent code.  For this in-kernel implementation,
	i386 has been converted to absolute addressing, but x86_64 still uses PIC.

	A local label can be referred to with position-independent assembler
	expressions such as "label-base(register)", where <base> is a local label
	whose address has been loaded into <register>.  (On i386, this is typically
	done with the idiom of a call to the next instruction and a pop of that
	return address into a register.)  Without local labels, the references must
	be done using spaces for addresses of "lazy symbols" that are filled in by
	the dynamic loader and loaded by the code that wants the address.

	So the various routines in other files are assembled here via #include
	directives.
*/
#include "Data.s"


#define	TableSize	(256*4)
	/*	Each of the arrays defined in Data.s except for the round constants
		in _AESRcon is composed of four tables of 256 entries of four bytes
		each.  TableSize is the number of bytes in one of those four tables.
	*/


// Include constants describing the AES context structures.
#include "Context.h"


/*	Define a macro to select a value based on architecture.  This reduces
	some of the architecture conditionalization later in the source.
*/
#if defined __i386__
	#define	Arch(i386, x86_64)	i386
#elif defined __x86_64__
	#define	Arch(i386, x86_64)	x86_64
#endif


// Define an instruction for moving pointers.
#define	movp	Arch(movd, movd)
	// Latter argument should be "movq", but the assembler uses "movd".


/*	Rename the general registers.  This makes it easier to keep track of them
	and provides names for the "whole register" that are uniform between i386
	and x86_64.
*/
#if defined __i386__
	#define	r0	%eax	// Available for any use.
	#define	r1	%ecx	// Available for any use, some special purposes (loop).
	#define	r2	%edx	// Available for any use.
	#define	r3	%ebx	// Must be preserved by called routine.
	#define	r4	%esp	// Stack pointer.
	#define	r5	%ebp	// Frame pointer, must preserve, no bare indirect.
	#define	r6	%esi	// Must be preserved by called routine.
	#define	r7	%edi	// Must be preserved by called routine.
#elif defined __x86_64__
	#define	r0	%rax	// Available for any use.
	#define	r1	%rcx	// Available for any use.
	#define	r2	%rdx	// Available for any use.
	#define	r3	%rbx	// Must be preserved by called routine.
	#define	r4	%rsp	// Stack pointer.
	#define	r5	%rbp	// Frame pointer.  Must be preserved by called routine.
	#define	r6	%rsi	// Available for any use.
	#define	r7	%rdi	// Available for any use.
	#define	r8	%r8		// Available for any use.
	#define	r9	%r9		// Available for any use.
	#define	r10	%r10	// Available for any use.
	#define	r11	%r11	// Available for any use.
	#define	r12	%r12	// Must be preserved by called routine.
	#define	r13	%r13	// Must be preserved by called routine.
	#define	r14	%r14	// Must be preserved by called routine.
	#define	r15	%r15	// Must be preserved by called routine.
#else
	#error "Unknown architecture."
#endif

// Define names for parts of registers.

#define	r0d		%eax	// Low 32 bits of r0.
#define	r1d		%ecx	// Low 32 bits of r1.
#define	r2d		%edx	// Low 32 bits of r2.
#define	r3d		%ebx	// Low 32 bits of r3.
#define	r5d		%ebp	// Low 32 bits of r5.
#define	r6d		%esi	// Low 32 bits of r6.
#define	r7d		%edi	// Low 32 bits of r7.
#define	r8d		%r8d	// Low 32 bits of r8.
#define	r9d		%r9d	// Low 32 bits of r9.
#define	r11d	%r11d	// Low 32 bits of r11.

#define	r0l		%al		// Low byte of r0.
#define	r1l		%cl		// Low byte of r1.
#define	r2l		%dl		// Low byte of r2.
#define	r3l		%bl		// Low byte of r3.
#define	r5l		%bpl	// Low byte of r5.

#define	r0h		%ah		// Second lowest byte of r0.
#define	r1h		%ch		// Second lowest byte of r1.
#define	r2h		%dh		// Second lowest byte of r2.
#define	r3h		%bh		// Second lowest byte of r3.


	.text


// Define encryption routine, _AESEncryptWithExpandedKey
#define	Select	0
#include "EncryptDecrypt.s"
#undef	Select


// Define decryption routine, _AESDecryptWithExpandedKey
#define	Select	1
#include "EncryptDecrypt.s"
#undef	Select

// Define encryption routine, _AESEncryptWithExpandedKey
#define	Select	2
#include "EncryptDecrypt.s"
#undef	Select


// Define decryption routine, _AESDecryptWithExpandedKey
#define	Select	3
#include "EncryptDecrypt.s"
#undef	Select


// Define key expansion routine for encryption, _AESExpandKeyForEncryption.
#include "ExpandKeyForEncryption.s"


// Define key expansion for decryption routine, _AESExpandKeyForDecryption.
#include "ExpandKeyForDecryption.s"
#endif /* x86 based build */
#endif /* __NO_ASM__ */
