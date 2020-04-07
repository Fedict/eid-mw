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

int ccrsa_emsa_pss_encode(const struct ccdigest_info* di, const struct ccdigest_info* MgfDi,
                    size_t sSize, const uint8_t *salt,
                    size_t hSize, const uint8_t *mHash,
                    size_t emBits, uint8_t *EM)
{
    const size_t emSize = cc_ceiling(emBits,8);
    int rc=0;
    
    
    //0.
    if(emBits< 8*hSize + 8*sSize + 9)
        return CCRSA_ENCODING_ERROR;
    //1.
    
    //2. This functoin get the hash of the input message in mHash
    //3.
    if(emSize < hSize + sSize + 2)
        return CCRSA_ENCODING_ERROR;
    
    //4. This function expects to get salt in salt input

    
    //5., 6. H = hash(00 00 00 00 00 00 00 00 || mHash ||salt)
    cc_assert(hSize==di->output_size); //or there will be a buffer overrun
    uint8_t *H = EM+emSize-hSize-1; //store H=hash() directly to the output EM
    const uint64_t zero = 0;
    ccdigest_di_decl(di, dc);
    ccdigest_init(di, (ccdigest_ctx_t)dc);
    ccdigest_update(di, (ccdigest_ctx_t)dc, sizeof(uint64_t), &zero);
    ccdigest_update(di, (ccdigest_ctx_t)dc, hSize, mHash);
    if(sSize!=0) ccdigest_update(di, (ccdigest_ctx_t)dc, sSize, salt); //sLen=0 can be passed to the function, which means no salt
    ccdigest_final(di, (ccdigest_ctx_t)dc, H);
    ccdigest_di_clear(di, dc);
    
    //7.,8.
    //we create PS and DB on the fly. See step 10
    
    //9. mask generation function
    uint8_t *dbMask=EM; //use EM as temp buffer for dbMask
    //if( emSize-hSize-1 < MgfDi->output_size) ccmgf returns error. this is not part of the spec but not acceptable for MGF
    rc = ccmgf(MgfDi, emSize-hSize-1, dbMask, hSize, H); //take H and produce dbMask with the length of emLen-hLen-1

    //10.
    size_t i, j;
    const size_t len=emSize-sSize-hSize-2;
    uint8_t *maskedDB=EM; //store directly to EM
    //wachout maskedDB, dbMask and EM point to the same location
    for(i=0; i<len; i++) //len can be zero
        maskedDB[i] = 0 ^ dbMask[i];

    maskedDB[i] = 0x01 ^ dbMask[i]; i++;
    for(j=0; j<sSize; i++, j++)
         maskedDB[i] = salt[j] ^ dbMask[i];
        
    //11. this makes sure encoded message is smaller than modulus
    const size_t n=8*emSize-emBits;
    uint8_t mask;
    if(n<8) // 0<=n<8,  n==0 means emBits fits in an array of bytes and modBits has one extra bits ie modBits=1 mod 8
        mask = (uint8_t)0xff>>n;
    else{
        mask=0xff; //there is an error and mask value is irrelevant
        rc=-1;
    }
    maskedDB[0] &=mask;
    
    //12., 13.
    //EM <--- maskedDB length is emLen-hLen-1 this has been done in step 10.
    //EM+emLen-hLen-1 <--- H length is  hLen, this has been done at step 5.,6.
    EM[emSize-1] = 0xbc;
    return rc;
}


