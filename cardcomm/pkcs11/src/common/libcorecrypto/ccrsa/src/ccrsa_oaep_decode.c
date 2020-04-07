/*
 * Copyright (c) 2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccrng.h>
#include <corecrypto/ccrsa_priv.h>

/*
 The s_size argument is really meant to be a size_t rather than a cc_size.  It's the size
 in bytes of the key for which this decoding is being done.  's' on the other hand is a
 cc_unit array large enough to contain the blocksize of the key.  We need to start the
 decoding "right justified" within s for s_size bytes.

 written from PKCS #1 v2.2
 */

int ccrsa_oaep_decode_parameter(const struct ccdigest_info* di,
                                size_t *r_len, uint8_t *r,
                                size_t s_size, cc_unit* s,
                                size_t parameter_data_len, const uint8_t *parameter_data)
{
    size_t encoded_len = s_size - 1;
    uint8_t lHash[di->output_size];
    
    size_t  DB_len = encoded_len - di->output_size;
    cc_unit DB[ccn_nof_size(DB_len)];
    cc_unit dbMask[ccn_nof_size(DB_len)];
    cc_unit seed[ccn_nof_size(di->output_size)];
    cc_unit seedMask[ccn_nof_size(di->output_size)];
    volatile size_t  retval=0;

    ccn_swap(ccrsa_n_from_size(s_size), s);
    uint8_t *encoded = ccrsa_block_start(s_size, s, 0);
    uint8_t *ptr;
    size_t ptr_increment;

    // Independent of the the content of s so ok to early abort.
    if (encoded_len < 2 * di->output_size + 1) {
        return CCRSA_INVALID_CONFIG;
    }

    // a) Hash the label
    ccdigest(di, parameter_data_len, parameter_data, lHash);

    // b) Encoded message is broken down into:
    // EM = Y || maskedSeed || maskedDB

    // Make a local copy in buffer aligned on cc_unit for the verification
    CC_MEMCPY(seed, &encoded[1], di->output_size);
    CC_MEMCPY(DB, &encoded[1+di->output_size], DB_len);

    // c) seedMask = MGF(maskedDB,hLen);
    ccmgf(di, di->output_size, seedMask, DB_len, DB);

    // d) seed = maskedSeed XOR seedMask
    ccn_xor(ccn_nof_size(sizeof(seedMask)), seed, seed, seedMask);

    // e) dbMask=MGF(seed,k-hLen - 1);
    ccmgf(di, DB_len, dbMask, di->output_size, seed);

    // f) DB = maskDB XOR dbMask
    ccn_xor(ccn_nof_size(sizeof(dbMask)), DB, DB, dbMask);


    // g) Separate DB into an octet string
    // DB = lHash' || PS || 0x01 || M

    // Y == 0 ?
    retval |= (encoded[0] ^ 0x00);

    // lHash==lHash' ?
    retval |= cc_cmp_safe(sizeof(lHash), lHash, DB);

    // -- Walk through the PS until 01 separator is found
    ptr=((uint8_t *)DB) + di->output_size;
    *r_len=0;
    ptr_increment=0;
    for(size_t i=0; i<(DB_len-di->output_size);i++)
    {
        uint8_t flag=ptr[i]^0x01;   // 0 iff ptr[i]==0x01. Any value !=0 otherwise
        HEAVISIDE_STEP_UINT8(flag); // 0 iff ptr[i]==0x01. 1 otherwise
        *r=ptr[i];               // The loop writes in r[0] until separator is found
        r+=ptr_increment;
        *r_len+=ptr_increment;
        ptr_increment|=(1^flag); // ptr_increment is 0 until 0x01 is found, remains 1 after that
        // until we found 0x01, it is an error to see a byte !=0.
        retval|= ((1^ptr_increment)             // 1 while still iterating on PS
                  & ((uint16_t)ptr[i]+0xff)>>8); // 1 is ptr[i]>0
    }

    // Error if ptr_increment==1 (separator 0x01 not found)
    retval |= (ptr_increment^1);

    // Clear our context
    ccn_clear(ccn_nof_size(DB_len), DB);
    ccn_clear(ccn_nof_size(DB_len), dbMask);
    ccn_clear(ccn_nof_size(di->output_size), seedMask);
    ccn_clear(ccn_nof_size(di->output_size), seed);
    cc_clear(sizeof(lHash),lHash);

    if (retval!=0) {
        return CCRSA_DECRYPTION_ERROR;
    }
    return (int)retval;
}
