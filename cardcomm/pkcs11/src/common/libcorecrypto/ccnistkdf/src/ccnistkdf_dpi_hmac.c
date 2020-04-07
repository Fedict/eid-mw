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



#include <corecrypto/ccnistkdf.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/cc.h>

/*
 
 From:
 
 NIST Special Publication 800-108
 Recommendation for Key Derivation
 Using Pseudorandom Functions
 
 http://csrc.nist.gov/publications/nistpubs/800-108/sp800-108.pdf
 Section 5.3
 KDF in Double-Pipeline Iteration Mode
 
 Fixed values:
 1. h - The length of the output of the PRF in bits, and
 2. r - The length of the binary representation of the counter i. r is specified 
        only when a counter is used as an input.
 
 Input: KI, Label, Context, and L.
 
 Process:
 1. n: = ⎡L/h⎤.
 2. If n > 232 -1, then indicate an error and stop.
 3. result(0):= ∅
 4. A(0):= IV = Label || 0x00 || Context || [L]2.
 5. For i = 1 to n, do
 a. A(i) := PRF (KI, A(i-1))
 b. K(i) := PRF (KI, A(i){|| [i]2}|| Label || 0x00 || Context || [L]2) c. result (i) := result(i-1) || K(i)
 7. Return: KO, i.e., the leftmost L bits of result(n).
 
 Output: KO.
 The first iteration pipeline uses a feedback mode with an initial value of 
    A(0)= IV = Label || 0x00 || Context || [L]2.
 Each second pipeline iteration generates K(i) using A(i) and, optionally, a 
 counter [i]2 as the iteration variable.
 
 */


int ccnistkdf_dpi_hmac(CC_UNUSED const struct ccdigest_info *di,
                       CC_UNUSED size_t kdkLen, CC_UNUSED const void *kdk,
                       CC_UNUSED size_t labelLen, CC_UNUSED const void *label,
                       CC_UNUSED size_t contextLen, CC_UNUSED const void *context,
                       CC_UNUSED size_t dkLen, CC_UNUSED void *dk) {
    return 0;
}
