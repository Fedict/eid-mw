/*
 * Copyright (c) 2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccrsa_priv.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/cc_debug.h>

/*
 VERIFY_BY_ENCODE_THEN_MEMCMP:

 With the current implementation, defining this produce overall smaller code because the emsa_*_encode function
 is smaller than the emsa_*_verify. Even though this function will get a bit bigger, there is a total gain.
 
 Also the encode function is also used for rsa sign so when doing both sign and verify, you only need one the encode.
 so thats an additional size gain in the common case.

 One downside is it will use extra stack space (the size of the modulus). 
 
 Performance difference not tested, but assumed to be negligible compared to the actual rsa crypt.
 
 It should be possible to further optimize the direct verify version to make it smaller than
 the encode + memcmp, in particular if directly comparing the cc_unit array resulting from 
 the ccrsa_pub_crypt call and avoid the conversion to a byte string.
 
 Having the emsa_*_encode or emsa_*_verify as static would also reduce the total size in the case you
 only need verify.
 
 Currently left undefined, so at least the verify code is tested.
 
*/

//#define VERIFY_BY_ENCODE_THEN_MEMCMP

int ccrsa_verify_pkcs1v15(ccrsa_pub_ctx_t key, const uint8_t *oid,
                          size_t digest_len, const uint8_t *digest,
                          size_t sig_len, const uint8_t *sig,
                          bool *valid)
{
    size_t m_size = ccn_write_uint_size(ccrsa_ctx_n(key), ccrsa_ctx_m(key));
    cc_size n=ccrsa_ctx_n(key);
    cc_unit s[n];
    *valid = false;
    int err;

    cc_require_action(sig_len==m_size,errOut,err=CCRSA_INVALID_INPUT);

    ccn_read_uint(n, s, sig_len, sig);
    cc_require((err=ccrsa_pub_crypt(key, s, s))==0,errOut);

    {
        unsigned char em[m_size];
        ccn_write_uint_padded(n, s, m_size, em);
    
#ifdef VERIFY_BY_ENCODE_THEN_MEMCMP
        unsigned char em2[m_size];
   
        cc_require((err=ccrsa_emsa_pkcs1v15_encode(m_size, em2, digest_len, digest, oid))==0,errOut); /* digest len is too big ?*/

        if(memcmp(em, em2, m_size)==0)
            *valid = true;
#else
        if(ccrsa_emsa_pkcs1v15_verify(m_size, em, digest_len, digest, oid)==0)
            *valid = true;
#endif
    }
errOut:
    return err;
}
