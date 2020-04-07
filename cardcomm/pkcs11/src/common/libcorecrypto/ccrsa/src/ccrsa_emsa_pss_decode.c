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

/*!
 @function  ccrsa_emsa_pss_decode() PKCS calls it EMSA-PSS-Verify (M, EM, emBits)
 @param Options:	Hash	hash function (hLen denotes the length in octets of the hash function output)
 @param MGF	mask generation function
 @param sLen	intended length in octets of the salt
 @param M	message to be verified, an octet string
 @param EM	encoded message, an octet string of length emLen = ⎡emBits/8⎤
 @param emBits	maximal bit length of the integer OS2IP (EM) (see Section 4.2), at least 8hLen + 8sLen + 9
 @result	0=consistent or non zero= inconsistent
 */

int ccrsa_emsa_pss_decode(const struct ccdigest_info* di, const struct ccdigest_info* MgfDi,
                    size_t sSize,
                    size_t mSize,  const uint8_t *mHash,
                    size_t emBits, const uint8_t *EM)
{
    const cc_size emSize = cc_ceiling(emBits, 8); //In theory, emLen can be one byte less than modBytes
    const size_t hSize=di->output_size;
    int rc=0;
    
    //1.
    if(mSize != hSize) return CCRSA_DECODING_ERROR;
     
    
    //3.
    if (emSize < hSize + sSize + 2) return  CCRSA_DECODING_ERROR;

    //4.
    rc = EM[emSize-1]^0xbc; // EM[emLen-1] must be 0xbc
    
    //5.
    const uint8_t *maskedDB=EM; //get directly from EM
    
    //6.
    //stndard: If the leftmost 8emLen – emBits bits of the leftmost octet in maskedDB are not all equal to zero, output “inconsistent” and stop.
    const size_t n=8*emSize-emBits;
    uint8_t mask;
    if(n<8)
        mask = ~((uint8_t)0xff>>n); //n==0 generates mask==0, that meanS no mask is required and sets rc to zero
    else{
        mask=0xff; //there is an error. mask value is irrelevant
        rc=-1;
    }
    rc |= maskedDB[0] & mask;
    
    //7.
    const size_t len= emSize-hSize-1;
    uint8_t dbMask[len];
    const uint8_t *H=EM+len;
    
    rc |= ccmgf(MgfDi, len, dbMask, hSize, H);

    //8.
    //9. knock off n number of bits
    size_t i=0;
    rc |= (maskedDB[i]^dbMask[i]) & ~mask;
    i++;
    
    //10.
    cc_assert(emSize>=sSize+hSize+2);
    for(; i<emSize-sSize-hSize-2; i++){ // maskedDB and dbMask must be equal
        rc |= maskedDB[i]^dbMask[i]; //let it continue, although there might be an error
    }
    
    //10.
    rc |= (maskedDB[i] ^ dbMask[i]) != 1; //must be one
    i++;
    
    //11.
    size_t j;
    uint8_t salt[sSize]; //sLen can be 0
    for(j=0; j<sSize; i++, j++)
        salt[j] = maskedDB[i] ^ dbMask[i];
    
    //12. 13.
    uint8_t H2[hSize];
    const uint64_t zero = 0;
    ccdigest_di_decl(di, dc);
    ccdigest_init(di, (ccdigest_ctx_t)dc);
    ccdigest_update(di, (ccdigest_ctx_t)dc, sizeof(uint64_t), &zero);
    ccdigest_update(di, (ccdigest_ctx_t)dc, mSize, mHash);
    if(sSize!=0) ccdigest_update(di, (ccdigest_ctx_t)dc, sSize, salt); //sLen=0 can be passed to the function, which means no salt
    ccdigest_final(di, (ccdigest_ctx_t)dc, H2);
    
    //14.
    rc |= cc_cmp_safe(hSize, H2, H);
    
    rc = rc!=0? CCRSA_DECODING_ERROR : 0;

    return rc;
}
