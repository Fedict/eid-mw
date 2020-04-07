/*
 * Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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

#ifndef __NO_ASM__

#include <corecrypto/ccmode.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/cc_config.h>
#include <corecrypto/cc_priv.h>

#if CCAES_INTEL

#include "vng_aesPriv.h"

struct ccaes_intel_encrypt_wrapper_ctx
{
    vng_aes_encrypt_ctx cx[1];
};

#define CBC_CTX_SIZE sizeof(struct ccaes_intel_encrypt_wrapper_ctx)

/* ==========================================================================
	VNG Optimized AES implementation.  This implementation is optimized but
	does not use the AESNI instructions
   ========================================================================== */

/* Initialize a context with the key and iv*/
static void init_wrapper_opt(const struct ccmode_cbc *cbc CC_UNUSED, cccbc_ctx *key,
                             unsigned long rawkey_len, const void *rawkey)
{
	struct ccaes_intel_encrypt_wrapper_ctx *ctx = (struct ccaes_intel_encrypt_wrapper_ctx *) key;
	
	vng_aes_encrypt_opt_key((const unsigned char *)rawkey, (int) rawkey_len, ctx->cx);
}

/* cbc encrypt or decrypt nblocks from in to out, iv will be used and updated. */
static void cbc_wrapper_opt(const cccbc_ctx *key, cccbc_iv *iv, unsigned long nblocks,
                            const void *in, void *out)
{
    const struct ccaes_intel_encrypt_wrapper_ctx *ctx = (const struct ccaes_intel_encrypt_wrapper_ctx *) key;
    
    if (0 == nblocks)
    {
        return;
    }

	vng_aes_encrypt_opt_cbc((const unsigned char *) in, (unsigned char *) iv, (unsigned int) nblocks, (unsigned char *) out, ctx->cx);

    /* TODO: the encrypt function should update the iv, for now we copy it here */
    CC_MEMCPY(iv, out+(nblocks-1)*CCAES_BLOCK_SIZE, CCAES_BLOCK_SIZE);
}

const struct ccmode_cbc ccaes_intel_cbc_encrypt_opt_mode = {
    .size = CBC_CTX_SIZE,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_opt,
    .cbc = cbc_wrapper_opt,
    .custom = NULL,
};

/* ==========================================================================
	VNG AESNI implementation.  This implementation uses the AESNI 
	instructions
   ========================================================================== */

/* Initialize a context with the key and iv*/
static void init_wrapper_aesni(const struct ccmode_cbc *cbc CC_UNUSED, cccbc_ctx *key,
                               unsigned long rawkey_len, const void *rawkey)
{
	vng_aes_encrypt_ctx *ctx = (vng_aes_encrypt_ctx *) key;
	
	vng_aes_encrypt_aesni_key((const unsigned char *)rawkey, (int) rawkey_len, ctx);
}

/* cbc encrypt or decrypt nblocks from in to out, iv will be used and updated. */
static void cbc_wrapper_aesni(const cccbc_ctx *key, cccbc_iv *iv, unsigned long nblocks,
                              const void *in, void *out)
{
    const vng_aes_encrypt_ctx *ctx = (const vng_aes_encrypt_ctx *) key;
    
    if (0 == nblocks)
    {
        return;
    }

	vng_aes_encrypt_aesni_cbc(in, (unsigned char *) iv, (unsigned int) nblocks, out, ctx);

    /* TODO: the encrypt function should update the iv, for now we copy it here */
    CC_MEMCPY(iv, out+(nblocks-1)*CCAES_BLOCK_SIZE, CCAES_BLOCK_SIZE);
}

const struct ccmode_cbc ccaes_intel_cbc_encrypt_aesni_mode = {
    .size = CBC_CTX_SIZE,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_aesni,
    .cbc = cbc_wrapper_aesni,
    .custom = NULL,
};

#endif /* CCAES_INTEL */
#endif /* __NO_ASM__ */
