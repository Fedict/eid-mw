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


#include <corecrypto/ccperf.h>
#include <corecrypto/ccec.h>
#include <corecrypto/ccec_priv.h>

static ccec_const_cp_t ccec_cp(unsigned long size) {
    switch (size) {
        case (192):
            return ccec_cp_192();
        case (224):
            return ccec_cp_224();
        case (256):
            return ccec_cp_256();
        case (384):
            return ccec_cp_384();
        case (521): /* -- 544 = 521 rounded up to the nearest multiple of 32*/
            return ccec_cp_521();
        default:
            return (ccec_const_cp_t)(const struct cczp *)0;
    }
}

static double perf_ccec_compact_import_pub(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);
    if (ccec_cp_prime_bitlen(cp)==224) {
        return 0; // not supported
    }
    ccec_generate_key_internal_fips(cp, rng, key);
    
    size_t  export_pubsize = ccec_compact_export_size(0, key);
    uint8_t exported_pubkey[export_pubsize];
    ccec_pub_ctx_decl_cp(ccec_ctx_cp(key), reconstituted_pub);
    
    ccec_compact_export(0, exported_pubkey, key);
    
    perf_start();
    do {
        ccec_compact_import_pub(ccec_ctx_cp(key), export_pubsize, exported_pubkey, reconstituted_pub);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_generate_key_legacy(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);

    perf_start();
    do {
        ccec_generate_key_legacy(cp, rng, key);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_generate_key_fips(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);

    perf_start();
    do {
        ccec_generate_key_fips(cp, rng, key);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_compact_generate_key(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);

    perf_start();
    do {
        ccec_compact_generate_key(cp, rng, key);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_generate_key_internal_fips(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);

    perf_start();
    do {
        ccec_generate_key_internal_fips(cp, rng, key);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_sign(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);
    size_t siglen = ccec_sign_max_size(cp);
    uint8_t sig[siglen];
    uint8_t digest[24] = "012345678912345678901234";

    ccec_generate_key_internal_fips(cp, rng, key);

    perf_start();
    do {
        ccec_sign(key, sizeof(digest), digest, &siglen, sig, rng);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccec_verify(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key);
    size_t siglen = ccec_sign_max_size(cp);
    uint8_t sig[siglen];
    uint8_t digest[24] = "012345678912345678901234";
    bool ok;

    ccec_generate_key_internal_fips(cp, rng, key);
    ccec_sign(key, sizeof(digest), digest, &siglen, sig, rng);

    perf_start();
    do {
        ccec_verify(key, sizeof(digest), digest, siglen, sig, &ok);
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccecdh_compute_shared_secret(unsigned long loops, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, key1);
    ccec_full_ctx_decl_cp(cp, key2);
    uint8_t out1[ccec_ccn_size(cp)];
    size_t out1_len;

    ccec_generate_key_internal_fips(cp, rng, key1);
    ccec_generate_key_internal_fips(cp, rng, key2);

    perf_start();
    do {
        ccecdh_compute_shared_secret(key1, key2, &out1_len, out1,rng);
    } while (--loops != 0);
    return perf_time();
}

#define _TEST(_x) { .name = #_x, .func = perf_ ## _x}
static struct ccec_perf_test {
    const char *name;
    double(*func)(unsigned long loops, ccec_const_cp_t cp);
} ccec_perf_tests[] = {

    _TEST(ccec_generate_key_internal_fips),
    _TEST(ccec_generate_key_fips),
    _TEST(ccec_generate_key_legacy),
    _TEST(ccec_compact_generate_key),
    _TEST(ccec_sign),
    _TEST(ccec_verify),
    _TEST(ccec_compact_import_pub),
    _TEST(ccecdh_compute_shared_secret),
};

static double perf_ccec(unsigned long loops, unsigned long size, const void *arg)
{
    const struct ccec_perf_test *test=arg;
    return test->func(loops, ccec_cp(size));
}

static struct ccperf_family family;

struct ccperf_family *ccperf_family_ccec(int argc, char *argv[])
{
    F_GET_ALL(family, ccec);
    family.nsizes=5;
    family.sizes=malloc(family.nsizes*sizeof(unsigned long));
    family.sizes[0]=192;
    family.sizes[1]=224;
    family.sizes[2]=256;
    family.sizes[3]=384;
    family.sizes[4]=521;
    family.size_kind=ccperf_size_bits;
    return &family;
}
