/*
 * Copyright (c) 2013,2015 Apple Inc. All rights reserved.
 * 
 * corecrypto Internal Use License Agreement
 * 
 * IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
 * in consideration of your agreement to the following terms, and your download or use
 * of this Apple software constitutes acceptance of these terms.  If you do not agree
 * with these terms, please do not download or use this Apple software.
 * 
 * 1.	As used in this Agreement, the term "Apple Software" collectively means and
 * includes all of the Apple corecrypto materials provided by Apple here, including
 * but not limited to the Apple corecrypto software, frameworks, libraries, documentation
 * and other Apple-created materials. In consideration of your agreement to abide by the
 * following terms, conditioned upon your compliance with these terms and subject to
 * these terms, Apple grants you, for a period of ninety (90) days from the date you
 * download the Apple Software, a limited, non-exclusive, non-sublicensable license
 * under Apple’s copyrights in the Apple Software to make a reasonable number of copies
 * of, compile, and run the Apple Software internally within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software; provided
 * that you must retain this notice and the following text and disclaimers in all
 * copies of the Apple Software that you make. You may not, directly or indirectly,
 * redistribute the Apple Software or any portions thereof. The Apple Software is only
 * licensed and intended for use as expressly stated above and may not be used for other
 * purposes or in other contexts without Apple's prior written permission.  Except as
 * expressly stated in this notice, no other rights or licenses, express or implied, are
 * granted by Apple herein.
 * 
 * 2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
 * OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 * THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
 * SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
 * SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
 * OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
 * WILL CREATE A WARRANTY. 
 * 
 * 3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
 * IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
 * SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * 4.	This Agreement is effective until terminated. Your rights under this Agreement will
 * terminate automatically without notice from Apple if you fail to comply with any term(s)
 * of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
 * and destroy all copies, full or partial, of the Apple Software. This Agreement will be
 * governed and construed in accordance with the laws of the State of California, without
 * regard to its choice of law rules.
 * 
 * You may report security issues about Apple products to product-security@apple.com,
 * as described here:  https://www.apple.com/support/security/.  Non-security bugs and
 * enhancement requests can be made via https://bugreport.apple.com as described
 * here: https://developer.apple.com/bug-reporting/
 *
 * EA1350 
 * 10/5/15
 */

#ifndef __NO_ASM__

#ifndef corecrypto_arm_aes_compatability_h
#define corecrypto_arm_aes_compatability_h

// #include <Availability.h>
#include <sys/cdefs.h>

#if defined(__clang__) && ((defined(__apple_build_version__) && __apple_build_version__ > 5010000))
#define __USES_V_CRYPTO_INTRINSICS 1
#else
#define __USES_V_CRYPTO_INTRINSICS 0
#endif


//  AES INSTRUCTIONS
// aese.16b	v0, v1
// aesd.16b	v0, v1
// aesmc.16b	v0, v1
// aesimc.16b	v0, v1

// SHA1 INTRINSICS
// sha1su0.4s	v0, v1, v2
// sha1su1.4s	v0, v1
// sha1c.4s	v0, v1, v2		// or q0, s1, v2.4s
// sha1m.4s	v0, v1, v2		// or q0, s1, v2.4s
// sha1p.4s	v0, v1, v2		// or q0, s1, v2.4s
// sha1h.4s	v0, v1		// or s0, s1

// SHA256 INTRINSICS
// sha256su0.4s	v0, v1
// sha256su1.4s	v0, v1, v2
// sha256h.4s		v0, v1, v2		// or q0, q1, v2.4s
// sha256h2.4s	v0, v1, v2		// or q0, q1, v2.4s


#if __USES_V_CRYPTO_INTRINSICS == 1
.macro	AESE
aese.16b v$0, v$1
.endm

.macro	AESD
aesd.16b v$0, v$1
.endm

.macro	AESMC
aesmc.16b v$0, v$1
.endm

.macro	AESIMC
aesimc.16b v$0, v$1
.endm


#else

.macro	AESE
aese q$0, q$1
.endm

.macro	AESD
aesd q$0, q$1
.endm

.macro	AESMC
aesmc q$0, q$1
.endm

.macro	AESIMC
aesimc q$0, q$1
.endm

#endif

#if __USES_V_CRYPTO_INTRINSICS == 1

.macro SHA1SU0
sha1su0	v$0.4s, v$1.4s, v$2.4s
.endm

.macro SHA1SU1
sha1su1	v$0.4s, v$1.4s
.endm

.macro SHA1C
sha1c	q$0, s$1, v$2.4s
.endm

.macro SHA1M
sha1m	q$0, s$1, v$2.4s
.endm

.macro SHA1P
sha1p	q$0, s$1, v$2.4s
.endm

.macro SHA1H
sha1h	s$0, s$1
.endm

.macro SHA256SU0
sha256su0    v$0.4s, v$1.4s
.endm

.macro SHA256SU1
sha256su1    v$0.4s, v$1.4s, v$2.4s
.endm

.macro SHA256H
sha256h    q$0, q$1, v$2.4s
.endm

.macro SHA256H2
sha256h2    q$0, q$1, v$2.4s
.endm

#else

.macro SHA1SU0
sha1su0	q$0, q$1, q$2
.endm

.macro SHA1SU1
sha1su1	q$0, q$1
.endm

.macro SHA1C
sha1c	q$0, q$1, q$2
.endm

.macro SHA1M
sha1m	q$0, q$1, q$2
.endm

.macro SHA1P
sha1p	q$0, q$1, q$2
.endm

.macro SHA1H
sha1h	q$0, q$1
.endm

.macro SHA256SU0
sha256su0    q$0, q$1
.endm

.macro SHA256SU1
sha256su1    q$0, q$1, q$2
.endm

.macro SHA256H
sha256h    q$0, q$1, q$2
.endm

.macro SHA256H2
sha256h2    q$0, q$1, q$2
.endm

#endif
#endif /*corecrypto_arm_aes_compatability_h*/



#endif /* __NO_ASM__ */
