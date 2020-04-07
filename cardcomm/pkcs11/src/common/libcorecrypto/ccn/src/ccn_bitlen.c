/*
 * Copyright (c) 2010,2011,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccn.h>

/*  Counts leading zeros in data, under the assumption that data is non-zero. */
CC_INLINE CC_CONST cc_unit cc_clz_nonzero(cc_unit data) {
    assert(data != 0);
#if defined __x86_64__ || defined __i386__
/*  On i386 and x86_64, we know clang and GCC will generate BSR for 
 *  __builtin_clzl.  This instruction IS NOT constant time on all micro-
 *  architectures, but it *is* constant time on all micro-architectures that
 *  have been used by Apple, and we expect that to continue to be the case.
 *
 *  When building for x86_64h with clang, this produces LZCNT, which is exactly
 *  what we want.                                                             */
    return __builtin_clzl(data);
#elif defined __arm64__ || defined __arm__
/*  On arm and arm64, we know that clang and GCC generate the constant-time CLZ
 *  instruction from __builtin_clzl( ).                                       */
    return __builtin_clzl(data);
#elif defined __GNUC__
#   warning Using __builtin_clzl( ) on an unknown architecture; it may not be constant-time.
/*  If you find yourself seeing this warning, file a radar for someone to 
 *  check whether or not __builtin_clzl( ) generates a constant-time
 *  implementation on the architecture you are targeting.  If it does, append
 *  the name of that architecture to the list of "safe" architectures above.  */
    return __builtin_clzl(data);
#else
#   warning Using a non-constant time implementation of cc_clz_nonzero( ).
/*  If you find yourself seeing this warning, the function ccn_bitlen will not
 *  be constant time in the resulting build of corecrypto.  Please file a radar
 *  for someone to implement a constant-time cc_clz_nonzero for your targeted
 *  architecture and compiler pair.                                           */
    cc_unit mask = CC_UNIT_C(1) << (CCN_UNIT_BITS - 1);
    size_t b = 0;
    while ((data & mask >> b) == 0) b++;
    return b;
#endif
}

size_t ccn_bitlen(cc_size count, const cc_unit *s)
{
    cc_size ix = ccn_n(count, s);
	if (ix == 0) return 0; // value is 0, no bits set.
    assert(s[ix-1] != 0);
    return CCN_UNIT_BITS * ix - cc_clz_nonzero(s[ix - 1]);
}
