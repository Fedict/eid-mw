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


#include <corecrypto/ccrsa_priv.h>

/*
    The r_size argument is really meant to be a size_t rather than a cc_size.  It's the size
    in bytes of the key for which this encoding is being done.  'r' on the other hand is a
    cc_unit array large enough to contain the blocksize of the key.  We need to build up the
    encoding "right justified" within r for r_size bytes.  We'll zero-pad the front and then
    at the end of this routine we'll use ccn_swap() to make it a big number.
 */

int ccrsa_oaep_encode_parameter(const struct ccdigest_info* di,
                                struct ccrng_state *rng,
                                size_t r_size, cc_unit *r,
                                size_t message_len, const uint8_t *message,
                                size_t parameter_data_len, const uint8_t *parameter_data)
{
   	const size_t encoded_len = r_size - 1;
    const size_t DB_len = encoded_len - di->output_size;
    const size_t seedMask_len = di->output_size;
    cc_unit DB[ccn_nof_size(DB_len)];
    cc_unit dbMask[ccn_nof_size(DB_len)];
    cc_unit seedMask[ccn_nof_size(seedMask_len)];
    
    if (message_len > encoded_len - 2 * di->output_size - 1)
        return CCRSA_INVALID_INPUT;
    
    ccn_zero(ccn_nof_size(sizeof(DB)), DB);
    ccdigest(di, parameter_data_len, parameter_data, DB);
    
    uint8_t *DB_bytes = (uint8_t*)DB;
    
    DB_bytes[DB_len - 1 - message_len] = 1;
    
    CC_MEMCPY(DB_bytes + DB_len - message_len, message, message_len);
    
    // here we use the return buffer for generating the seed
    cc_unit *seed = r;
    if(ccrng_generate(rng, di->output_size, seed) != 0) return -1;
    ccmgf(di, DB_len, dbMask, di->output_size, seed);
    ccn_xor(ccn_nof_size(DB_len), DB, DB, dbMask);
    
    ccmgf(di, seedMask_len, seedMask, DB_len, DB);
    ccn_xor(ccn_nof_size(seedMask_len), seed, seed, seedMask);
    
    // get the block start non-destructively so we don't mess seed
    uint8_t *encoded = ccrsa_block_start(r_size, r, 0);
    // Copy the seed out before zeroing the leading zeros.
    CC_MEMMOVE(encoded+1, seed, di->output_size);
    // clear the beginning of the block if necessary
    encoded = ccrsa_block_start(r_size, r, 1);
    encoded[0] = 0;
    encoded++;
    CC_MEMCPY(encoded + di->output_size, DB, DB_len);
    
    ccn_clear(ccn_nof_size(DB_len), DB);
    ccn_clear(ccn_nof_size(DB_len), dbMask);
    ccn_clear(ccn_nof_size(seedMask_len), seedMask);
    
    ccn_swap(ccn_nof_size(r_size), r);
    return 0;
}
