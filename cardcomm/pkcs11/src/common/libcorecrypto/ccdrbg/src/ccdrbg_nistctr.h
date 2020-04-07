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


#ifndef _CORECRYPTO_CCDRBG_NISTCTR_H_
#define _CORECRYPTO_CCDRBG_NISTCTR_H_

/*!
    @header     ccdrbg_nistctr.h
    @abstract   Interface to a NIST SP 800-90 AES-CTR DRBG
    @discussion This is an implementation of the NIST SP 800-90 AES-CTR DRBG
 */

#include <corecrypto/ccdrbg.h>
#include <corecrypto/ccmode.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NO_OF_INTS(size)              ((size)/sizeof(uint32_t))
    
#define CCADRBG_KEYLEN(drbg)          ((drbg)->keylen)
#define CCADRBG_KEYLEN_INTS(drbg)     NO_OF_INTS(CCADRBG_KEYLEN(drbg))

#define CCADRBG_BLOCKSIZE(drbg)       ((drbg)->ecb->block_size)
#define CCADRBG_BLOCKSIZE_INTS(drbg)  NO_OF_INTS(CCADRBG_BLOCKSIZE(drbg))
    
#define CCADRBG_OUTLEN(drbg)          CCADRBG_BLOCKSIZE(drbg)
#define CCADRBG_OUTLEN_INTS(drbg)	  NO_OF_INTS(CCADRBG_OUTLEN(drbg))

#define CCADRBG_SEEDLEN(drbg)         (CCADRBG_KEYLEN(drbg)+CCADRBG_OUTLEN(drbg))
#define CCADRBG_SEEDLEN_INTS(drbg)    NO_OF_INTS(CCADRBG_SEEDLEN(drbg))
   
#define CCADRBG_TEMPLEN_BLOCKS(drbg)  (CCADRBG_SEEDLEN(drbg)+CCADRBG_OUTLEN(drbg)-1)/CCADRBG_OUTLEN(drbg)
#define CCADRBG_TEMPLEN(drbg)         CCADRBG_TEMPLEN_BLOCKS(drbg)*CCADRBG_OUTLEN(drbg)
#define CCADRBG_TEMPLEN_INTS(drbg)	  NO_OF_INTS(CCADRBG_TEMPLEN(drbg))
    
//limits
#define CCADRBG_MAX_KEYBITS		256
#define CCADRBG_MAX_KEYLEN	(CCADRBG_MAX_KEYBITS / 8)
    
//required memory size
#define CCDRBG_NISTCTR_SIZE(ecb, keylen)    \
    ((ecb)->block_size +                    \
    (((keylen)+(ecb)->block_size*2-1)/(ecb)->block_size)*(ecb)->block_size +            \
    (ecb)->block_size +                     \
    (ecb)->block_size + (keylen) +          \
    (ecb)->block_size +                     \
    (2*(ecb)->size))

typedef struct {
	uint32_t *S; /*[CCADRBG_OUTLEN_BYTES/sizeof(uint32_t)]; */
	unsigned long index;
} _CCADRBG_BCC;

struct ccdrbg_nistctr_state {
    const struct ccmode_ecb *ecb;
    unsigned long   keylen;
	uint8_t			*encryptedIV; /* [CCADRBG_SEEDLEN / CCADRBG_OUTLEN][CCADRBG_OUTLEN_BYTES]; */
	uint32_t		*V; /* [CCADRBG_OUTLEN_INTS]; */
	uint32_t		*nullInput; /* [CCADRBG_SEEDLEN_INTS]; */
    ccecb_ctx       *key;
    ccecb_ctx       *df_key;
	_CCADRBG_BCC	bcc;
	uint64_t		reseed_counter; //fits max NIST requirement of 2^48
	int             strictFIPS;
    int             use_df;
};

#ifdef __cplusplus
}
#endif
#endif /*  _CORECRYPTO_CCDRBG_NISTCTR_H_ */
