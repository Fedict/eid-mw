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


#include <corecrypto/ccpbkdf2.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/cc.h>
#include <corecrypto/cc_priv.h>
/* Will write hLen bytes into dataPtr according to PKCS #5 2.0 spec.
   See: ../docs/pkcs5v2_1.pdf for details (cached copy of RSA's PKCS5v2)
*/
static void
F (const struct ccdigest_info *di,
   cchmac_ctx_t hc,
   const cc_unit *istate,
   size_t saltLen, const void *salt,
   unsigned long iterationCount,
   unsigned long blockNumber,
   unsigned long dataLen,
   void *data)
{
    uint8_t *inBlock = cchmac_data(di, hc);
	/* Set up inBlock to contain Salt || INT (blockNumber). */
    uint32_t bn;

    ccdigest_copy_state(di, cchmac_istate32(di, hc), istate);
    cchmac_nbits(di, hc) = di->block_size * 8;
    cchmac_num(di, hc)=0;
    ccdigest_update(di, cchmac_digest_ctx(di, hc), saltLen, salt);

    CC_STORE32_BE((uint32_t)blockNumber, &bn);
    ccdigest_update(di, cchmac_digest_ctx(di, hc), 4, &bn);

    /* Caculate U1 (result goes to outBlock) and copy it to resultBlockPtr. */
	cchmac_final(di,  hc, inBlock);

	CC_MEMCPY(data, inBlock, dataLen);

    /* Calculate U2 though UiterationCount. */
	for (unsigned long iteration = 2; iteration <= iterationCount; iteration++)
	{
        /* Now inBlock conatins Uiteration-1.  Calculate Uiteration into outBlock. */
        ccdigest_copy_state(di, cchmac_istate32(di, hc), istate);
        cchmac_nbits(di, hc) = di->block_size * 8;
        cchmac_num(di, hc) = (uint32_t)di->output_size;
        cchmac_final(di,  hc, inBlock);

        /* Xor data in dataPtr (U1 \xor U2 \xor ... \xor Uiteration-1) with
		   outBlock (Uiteration). */
		cc_xor(dataLen, data, data, inBlock);
    }
}


int ccpbkdf2_hmac(const struct ccdigest_info *di,
                   unsigned long passwordLen, const void *password,
                   unsigned long saltLen, const void *salt,
                   unsigned long iterations,
                   unsigned long dkLen, void *dk)
{
	// FIPS required check
    //  Specification is If dkLen > (2^32 – 1) × hLen, output “derived key too long” and stop.
    //  (^) is not "power of" in C.  It is in spec-speak.  Really this is a max uint32 number
    //  as the limit.
	if ((dkLen / di->output_size) > UINT32_MAX)
	{
		return -1;
	}
	
    cchmac_di_decl(di, hc);
    cc_unit istate[ccn_nof_size(di->state_size)];
    cchmac_init(di, hc, passwordLen, password);
    ccdigest_copy_state(di, istate, cchmac_istate32(di, hc));

    const unsigned long hLen = di->output_size;

	unsigned long completeBlocks = dkLen / hLen;
	unsigned long partialBlockSize = dkLen % hLen;

	/* First calculate all the complete hLen sized blocks required. */
	unsigned long blockNumber = 1;
	uint8_t *dataPtr = dk;
	
	// For FIPS the output needs to be concatenated not just xor'd
	for (; blockNumber <= completeBlocks; blockNumber++, dataPtr += hLen)
	{
		F (di, hc, istate, saltLen, salt, iterations, blockNumber, hLen, dataPtr);
    }

    /* Finally if the requested output size was not an even multiple of hLen,
       calculate the final block and copy the first partialBlockSize bytes of
       it to the output. */
	if (partialBlockSize > 0)
	{
		F (di, hc, istate, saltLen, salt, iterations, blockNumber, partialBlockSize, dataPtr);
	}

	cchmac_di_clear(di, hc);
	ccn_clear(ccn_nof_size(di->state_size), istate);
	return 0;
}
