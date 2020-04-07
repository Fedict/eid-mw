/*
 * Copyright (c) 2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#include <mach/mach_types.h>
#define USE_SUPER_COOL_NEW_CCOID_T


#include <libkern/crypto/register_crypto.h>

kern_return_t corecrypto_kext_start(kmod_info_t * ki, void *d);
kern_return_t corecrypto_kext_stop(kmod_info_t *ki, void *d);
extern void	panic(const char *, ...);

#include <corecrypto/ccdigest.h>
#include <corecrypto/ccmd5.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccdes.h>
#include <corecrypto/ccpad.h>
#include <corecrypto/ccblowfish.h>
#include <corecrypto/cccast.h>
#include "kext_fipspost.h"

#include <pexpert/pexpert.h>


#include <libkern/libkern.h>
#include <pexpert/pexpert.h>

static struct crypto_functions kpis;

kern_return_t corecrypto_kext_start(kmod_info_t * ki, void *d)
{
#ifdef SKIP_KEXT_FIPS_TESTS
#pragma unused (ki, d)
#endif
    /* TODO: Call the corecrypto FIPS POST here before registering */

	kprintf("corecrypto_kext_start called\n");
    int verbose = 0;
    
/* ========================================================================
    SKIP_KEXT_FIPS_TESTS is defined when things like InnsbruckWire is being
	built.  There is no need to run a FIPS test for the wire.
   ======================================================================== */
    
#ifndef SKIP_KEXT_FIPS_TESTS
	int fips_mode = 2;
	 
	int boot_arg_fips_mode = 0; 
	
	if (PE_parse_boot_argn("fips_mode", &boot_arg_fips_mode, sizeof(boot_arg_fips_mode)))
	{
		fips_mode = boot_arg_fips_mode;
	}

	if (fips_mode)
	{
		// Limit the values for fips_mode to 1 or 2
		if (1 != fips_mode && 2 != fips_mode)
		{
			// if the fips_mode value is not 1 or 2 pin
			// value at compliant meaning fips_mode = 2;
			kprintf("fips_mode forced to 2!\n");
			fips_mode = 2;
		}
		
		// fips_mode = 1 should only be used for debugging
		// purposes.  Setting fips_mode = 1 make the module
		// NOT be FIPS compliant.  On the other hand, it 
		// will set the verbose flag and there will be
		// a log of the FIPS POST tests.

		if (1 == fips_mode)
		{
			kprintf("fips_mode being set to 1, verbose is set to 1!\n");
			verbose = 1;
		}
		
        kprintf("kext fips mode set to %d\n", fips_mode);

		// Note: If verbose is set to 0 and the FIPS POSTs are
		// sucessful, then NO logging is done.
		
		if (KEXT_FIPSPost(ki, d, verbose))
	    {
            if (2 == fips_mode)
            {
				panic("FIPS Kernel POST Failed!");
            }
			else if (1 == fips_mode)
			{
				
				kprintf("KEXT_FIPS Post failed but the failure is being ignored\n");
			}
	    }
	}
	else 
	{
		kprintf("Bypassing FIPS mode for kernel space!\n");
	}
#endif // !SKIP_KEXT_FIPS_TESTS

    /* Register KPIs */

    /* digests common functions */
    kpis.ccdigest_init_fn = &ccdigest_init;
    kpis.ccdigest_update_fn = &ccdigest_update;
    kpis.ccdigest_fn = &ccdigest;
    /* digest implementations */
    kpis.ccmd5_di = ccmd5_di();
    kpis.ccsha1_di = ccsha1_di();
    kpis.ccsha256_di = ccsha256_di();
    kpis.ccsha384_di = ccsha384_di();
    kpis.ccsha512_di = ccsha512_di();

    /* hmac common function */
    kpis.cchmac_init_fn = &cchmac_init;
    kpis.cchmac_update_fn = &cchmac_update;
    kpis.cchmac_final_fn = &cchmac_final;
    kpis.cchmac_fn = &cchmac;

    /* ciphers modes implementations */
    /* AES, ecb, cbc and xts */
    kpis.ccaes_ecb_encrypt = ccaes_ecb_encrypt_mode();
    kpis.ccaes_ecb_decrypt = ccaes_ecb_decrypt_mode();
    kpis.ccaes_cbc_encrypt = ccaes_cbc_encrypt_mode();
    kpis.ccaes_cbc_decrypt = ccaes_cbc_decrypt_mode();
    kpis.ccaes_gcm_encrypt = ccaes_gcm_encrypt_mode();
    kpis.ccaes_gcm_decrypt = ccaes_gcm_decrypt_mode();
    kpis.ccaes_xts_encrypt = ccaes_xts_encrypt_mode();
    kpis.ccaes_xts_decrypt = ccaes_xts_decrypt_mode();
    /* DES, ecb and cbc */
    kpis.ccdes_ecb_encrypt = ccdes_ecb_encrypt_mode();
    kpis.ccdes_ecb_decrypt = ccdes_ecb_decrypt_mode();
    kpis.ccdes_cbc_encrypt = ccdes_cbc_encrypt_mode();
    kpis.ccdes_cbc_decrypt = ccdes_cbc_decrypt_mode();
    /* TDES, ecb and cbc */
    kpis.cctdes_ecb_encrypt = ccdes3_ecb_encrypt_mode();
    kpis.cctdes_ecb_decrypt = ccdes3_ecb_decrypt_mode();
    kpis.cctdes_cbc_encrypt = ccdes3_cbc_encrypt_mode();
    kpis.cctdes_cbc_decrypt = ccdes3_cbc_decrypt_mode();
    /* RC4 */
    kpis.ccrc4_info = ccrc4();
    /* Blowfish - ECB only */
    kpis.ccblowfish_ecb_encrypt = ccblowfish_ecb_encrypt_mode();
    kpis.ccblowfish_ecb_decrypt = ccblowfish_ecb_decrypt_mode();
	/* CAST - ECB only */
    kpis.cccast_ecb_encrypt = cccast_ecb_encrypt_mode();
    kpis.cccast_ecb_decrypt = cccast_ecb_decrypt_mode();
    /* DES key helper functions */
    kpis.ccdes_key_is_weak_fn = &ccdes_key_is_weak;
    kpis.ccdes_key_set_odd_parity_fn = &ccdes_key_set_odd_parity;
    /* XTS padding */
    kpis.ccpad_xts_encrypt_fn = NULL; //&ccpad_xts_encrypt;
    kpis.ccpad_xts_decrypt_fn = NULL; // &ccpad_xts_decrypt;

    register_crypto_functions(&kpis);

	if (verbose)
	{
		kprintf("corecrypto_kext_start completed sucessfully\n");
	}
    return KERN_SUCCESS;
}

kern_return_t corecrypto_kext_stop(kmod_info_t *ki CC_UNUSED, void *d CC_UNUSED)
{

    return KERN_SUCCESS;
}
