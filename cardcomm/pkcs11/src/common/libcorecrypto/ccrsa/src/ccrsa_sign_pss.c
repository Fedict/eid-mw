/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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

int ccrsa_sign_pss(const ccrsa_full_ctx_t key,
                   const struct ccdigest_info* hashAlgorithm,
                   const struct ccdigest_info* MgfHashAlgorithm,
                   size_t saltSize,struct ccrng_state *rng,
                   size_t hSize, const uint8_t *mHash,
                   size_t *sigSize, uint8_t *sig)
{
    const cc_size modBits =ccn_bitlen(ccrsa_ctx_n(key), ccrsa_ctx_m(key));
    const cc_size modBytes = cc_ceiling(modBits, 8);
    const cc_size emBits = modBits-1; //as defined in §8.1.1 of PKCS1-V2
    const cc_size emLen = cc_ceiling(emBits, 8); //In theory, emLen can be one byte less than modBytes
    int rc=0;
    
    //two FIPS 186-4 imposed conditions
    if(modBits==1024 && hashAlgorithm->output_size==512 && saltSize>hSize-2) return CCRSA_INVALID_INPUT;
    if(saltSize>hSize) return CCRSA_INVALID_INPUT;
    
    //input validation checks
    if(*sigSize<modBytes) return CCRSA_INVALID_INPUT;
    if(hSize!= hashAlgorithm->output_size)return CCRSA_INVALID_INPUT;
    
    *sigSize=modBytes;
    
    uint8_t salt[saltSize];
    int rc_rng;
    if (saltSize>0) {
        rc_rng=ccrng_generate(rng, saltSize, salt); //continue, although we know there is an error
    } else {
        rc_rng=0; // no error
    }
    
    const cc_size modWords=ccrsa_ctx_n(key);
    cc_unit EM[modWords]; //max length of EM in bytes is emLen. But since we pass EM to RSA exponentiation routine, we must have the length in modWords. In 64 bit machine, EM can be 7 bytes longer than what is needed in theory
    
    cc_assert(modWords*sizeof(cc_unit)>=emLen);
    EM[0]=EM[modWords-1] = 0; //in case emLen<modWord* sizeof(cc_unit), zeroize
    const size_t ofs = modWords*sizeof(cc_unit)-emLen;
    cc_assert(ofs<=sizeof(cc_unit)); //EM can only be one cc_unit larger
    rc|=ccrsa_emsa_pss_encode(hashAlgorithm, MgfHashAlgorithm, saltSize, salt, hSize, mHash, emBits, (uint8_t *)EM+ofs);     //let it continue, although we know there might be an error
    ccn_swap(modWords, EM);

    rc|=ccrsa_priv_crypt(ccrsa_ctx_private(key), EM, EM);
    
    /* we need to write leading zeroes if necessary */
    if(rc==0 && rc_rng==0)
        ccn_write_uint_padded(modWords,  EM, *sigSize, sig);
    else{
        ccn_clear(modWords, EM); //ccrsa_emsa_pss_encode() directly writes to EM. EM is cleared incase there is an error
        if(rc_rng!=0)
            rc = rc_rng;
    }
    
    return rc;
}
