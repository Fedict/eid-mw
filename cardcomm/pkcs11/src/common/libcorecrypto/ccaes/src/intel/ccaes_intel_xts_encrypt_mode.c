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


#if CCAES_INTEL
#include "vng_aesPriv.h"
struct ccaes_intel_encrypt_wrapper_ctx
{
	vng_aes_encrypt_ctx	cx[1];
	vng_aes_encrypt_ctx	tweak_cx[1];
};

#define XTS_CTX_SIZE sizeof(struct ccaes_intel_encrypt_wrapper_ctx)

/* ==========================================================================
	VNG Optimized AES implementation.  This implementation is optimized but
	does not use the AESNI instructions
   ========================================================================== */

/* Create a xts key from a xts mode object.  The tweak_len here
 determines how long the tweak is in bytes, for each subsequent call to
 ccmode_xts->xts(). */
static void init_wrapper_opt (const struct ccmode_xts *xts_not CC_UNUSED, ccxts_ctx *ctx,
                          unsigned long key_len, const void *key,
                          const void *tweak_key)
{

	struct ccaes_intel_encrypt_wrapper_ctx *xts = (struct ccaes_intel_encrypt_wrapper_ctx *) ctx;

	vng_aes_encrypt_opt_key(key, (int)key_len, xts->cx);
	vng_aes_encrypt_opt_key(tweak_key, (int)key_len, xts->tweak_cx);

}

/* Set the tweak (sector number), the block within the sector zero. */
static void set_tweak_wrapper_opt(const ccxts_ctx *ctx, ccxts_tweak *tweak, const void *iv)
{
	const struct ccaes_intel_encrypt_wrapper_ctx *xts = (const struct ccaes_intel_encrypt_wrapper_ctx *) ctx;
    uint8_t *T=(uint8_t*)tweak;

    /* encrypt the tweak */
    vng_aes_encrypt_opt(iv, T, xts->tweak_cx);

}

/* Encrypt blocks for a sector, clients must call set_tweak before calling
 this function.  Return a pointer to the current tweak (used by ccpad_xts). */
static void *xts_wrapper_opt(const ccxts_ctx *ctx, ccxts_tweak *tweak, unsigned long nblocks,
                            const void *in, void *out)
{
	const struct ccaes_intel_encrypt_wrapper_ctx *xts = (const struct ccaes_intel_encrypt_wrapper_ctx *) ctx;
    uint8_t *T=(uint8_t*)tweak;

	vng_aes_xts_encrypt_opt(in, nblocks*16, out, T, xts->cx);
    
    return T;
}

const struct ccmode_xts ccaes_intel_xts_encrypt_opt_mode = {
    .size = XTS_CTX_SIZE,
    .tweak_size = 16,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_opt,
    .set_tweak = set_tweak_wrapper_opt,
    .xts = xts_wrapper_opt,
    .custom = NULL,
    .custom1 = NULL,
};

/* ==========================================================================
	VNG AESNI implementation.  This implementation uses the AESNI 
	instructions
   ========================================================================== */

/* Create a xts key from a xts mode object.  The tweak_len here
 determines how long the tweak is in bytes, for each subsequent call to
 ccmode_xts->xts(). */
static void init_wrapper_aesni (const struct ccmode_xts *xts_not CC_UNUSED, ccxts_ctx *ctx,
                          unsigned long key_len, const void *key,
                          const void *tweak_key)
{

	struct ccaes_intel_encrypt_wrapper_ctx *xts = (struct ccaes_intel_encrypt_wrapper_ctx *) ctx;

	vng_aes_encrypt_aesni_key(key, (int)key_len, xts->cx);
	vng_aes_encrypt_aesni_key(tweak_key, (int)key_len, xts->tweak_cx);

}

/* Set the tweak (sector number), the block within the sector zero. */
static void set_tweak_wrapper_aesni(const ccxts_ctx *ctx, ccxts_tweak *tweak, const void *iv)
{
	const struct ccaes_intel_encrypt_wrapper_ctx *xts = (const struct ccaes_intel_encrypt_wrapper_ctx *) ctx;
    uint8_t *T=(uint8_t *)tweak;

    /* encrypt the tweak */
    vng_aes_encrypt_aesni(iv, T, xts->tweak_cx);

}

/* Encrypt blocks for a sector, clients must call set_tweak before calling
 this function.  Return a pointer to the current tweak (used by ccpad_xts). */
static void *xts_wrapper_aesni(const ccxts_ctx *ctx, ccxts_tweak *tweak, unsigned long nblocks,
                         const void *in, void *out)
{
	const struct ccaes_intel_encrypt_wrapper_ctx *xts = (const struct ccaes_intel_encrypt_wrapper_ctx *) ctx;
    uint8_t *T=(uint8_t *)tweak;

	vng_aes_xts_encrypt_aesni(in, nblocks*16, out, T, xts->cx);

    return T;
}

const struct ccmode_xts ccaes_intel_xts_encrypt_aesni_mode = {
    .size = XTS_CTX_SIZE,
    .tweak_size = 16,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_aesni,
    .set_tweak = set_tweak_wrapper_aesni,
    .xts = xts_wrapper_aesni,
    .custom = NULL,
    .custom1 = NULL,
};
#endif /* CCAES_INTEL */
#endif /* __NO_ASM__ */
