/*
 * Copyright (c) 2013,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_cccmac_H_
#define _CORECRYPTO_cccmac_H_

#include <corecrypto/cc.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/ccaes.h>

#define CMAC_BLOCKSIZE 16

struct cccmac_ctx {
    uint8_t b[8];
} __attribute__((aligned(8)));

typedef struct cccmac_ctx_hdr {
    uint8_t k1[16];
    uint8_t k2[16];
    uint8_t ctx[8];
} __attribute__((aligned(8))) cccmac_ctx_hdr;

typedef union {
    struct cccmac_ctx *b;
    cccmac_ctx_hdr *hdr;
} cccmac_ctx_t __attribute__((transparent_union));

#define cccmac_hdr_size sizeof(struct cccmac_ctx_hdr)
#define cccmac_iv_size(_mode_)  ((_mode_)->block_size)
#define cccmac_cbc_size(_mode_) ((_mode_)->size)

#define cccmac_ctx_size(_mode_) (cccmac_hdr_size + cccmac_iv_size(_mode_) + cccmac_cbc_size(_mode_))
#define cccmac_ctx_n(_mode_)  ccn_nof_size(cccmac_ctx_size(_mode_))

#define cccmac_mode_decl(_mode_, _name_) cc_ctx_decl(struct cccmac_ctx, cccmac_ctx_size(_mode_), _name_)
#define cccmac_mode_clear(_mode_, _name_) cc_clear(cccmac_ctx_size(_mode_), _name_)

/* Return a cccbc_ctx * which can be accesed with the macros in ccmode.h */
#define cccmac_mode_ctx_start(_mode_, HC)     (((HC).hdr)->ctx)
#define cccmac_mode_sym_ctx(_mode_, HC)     (cccbc_ctx *)(cccmac_mode_ctx_start(_mode_, HC))
#define cccmac_mode_iv(_mode_, HC)     (cccbc_iv *)(cccmac_mode_ctx_start(_mode_, HC)+cccmac_cbc_size(_mode_))
#define CCCMAC_HDR(HC)      (((cccmac_ctx_t)(HC)).hdr)
#define cccmac_k1(HC)       (CCCMAC_HDR(HC)->k1)
#define cccmac_k2(HC)       (CCCMAC_HDR(HC)->k2)

void cccmac_init(const struct ccmode_cbc *cbc, cccmac_ctx_t ctx, const void *key);


void cccmac_block_update(const struct ccmode_cbc *cbc, cccmac_ctx_t cmac,
                                       size_t nblocks, const void *data);


void cccmac_final(const struct ccmode_cbc *cbc, cccmac_ctx_t ctx,
                  size_t nbytes, const void *in, void *out);

void cccmac(const struct ccmode_cbc *cbc, const void *key,
            size_t data_len, const void *data,
            void *mac);


#endif /* _CORECRYPTO_cccmac_H_ */
