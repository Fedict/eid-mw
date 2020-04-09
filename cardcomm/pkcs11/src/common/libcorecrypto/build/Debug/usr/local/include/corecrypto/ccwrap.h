/*
 * Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CCWRAP_H_
#define _CORECRYPTO_CCWRAP_H_

#include <corecrypto/ccmode.h>

#define CCWRAP_SEMIBLOCK 8

CC_INLINE size_t ccwrap_wrapped_size(const size_t data_size)
{
    return ((data_size + CCWRAP_SEMIBLOCK /*integrity block*/ +
             (CCWRAP_SEMIBLOCK - 1) /*round up*/) & ~(CCWRAP_SEMIBLOCK - 1));
}

CC_INLINE size_t ccwrap_unwrapped_size(const size_t data_size)
{
    return (data_size - CCWRAP_SEMIBLOCK);
}

/* attempt to unwrap; -1 is returned in case of a mac failure */
int ccwrap_auth_decrypt(const struct ccmode_ecb *ecb_mode, ccecb_ctx *ctx,
                        size_t nbytes, const void *in,
                        size_t *obytes, void *out);

/* attempt to wrap; -1 is returned in case of illegal input size */
int ccwrap_auth_encrypt(const struct ccmode_ecb *ecb_mode, ccecb_ctx *ctx,
                        size_t nbytes, const void *in,
                        size_t *obytes, void *out);

/*
	This was originally implemented according to the “AES Key Wrap Specification”
	formalized in RFC 3394.

	The following publications track changes made over time:

   [AES-KW1] National Institute of Standards and Technology, AES Key
         Wrap Specification, 17 November 2001.
         http://csrc.nist.gov/groups/ST/toolkit/documents/kms/
         AES_key_wrap.pdf

   [AES-KW2] Schaad, J. and R. Housley, "Advanced Encryption Standard
             (AES) Key Wrap Algorithm", RFC 3394, September 2002.

	Follow up:

	   This document specifies an extension of the Advanced Encryption
   Standard (AES) Key Wrap algorithm [AES-KW1, AES-KW2].  Without this
   extension, the input to the AES Key Wrap algorithm, called the key
   data, must be a sequence of two or more 64-bit blocks.

	http://tools.ietf.org/html/rfc5649

	   Advanced Encryption Standard (AES) Key Wrap with Padding Algorithm

	   This document specifies a padding convention for use with the AES Key
	   Wrap algorithm specified in RFC 3394.  This convention eliminates the
	   requirement that the length of the key to be wrapped be a multiple of
	   64 bits, allowing a key of any practical length to be wrapped.

	The general idea is to pad the messages with zeroes until its a multiple
	of 8 bytes and encode the size in the "iv", where the upper 32 bits are
	fixed to 0xA65959A6 and the lower 32 bits the "message length indicator" (MLI).

	The 32-bit MLI, which occupies the low-order half of the AIV,
	is an unsigned binary integer equal to the octet length of the
	plaintext key data, in network order -- that is, with the most
	significant octet first.

	So the check during unwrap needs to validate the upper 32 bits, then validate
	the message length div 8 is the same as the lower 32 bits div 8, then check
	the message has zeroes for the last length mod 8 bytes.

	NIST SP800-38F provides a generalized form KW (key wrap) and KWP (with padding)

	Note: block size is required to be 128 bits

*/

#endif /* _CORECRYPTO_CCWRAP_H_ */
