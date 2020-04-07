/*
 * Copyright (c) 2011,2014,2015 Apple Inc. All rights reserved.
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
#include <corecrypto/cczp.h>
#include <corecrypto/cczp_priv.h>

static double perf_cczp_init(unsigned long loops, cc_size nbits)
{
    cczp_decl_n(ccn_nof(nbits), zp);
    CCZP_N(zp) = ccn_nof(nbits);

    ccn_random_bits(nbits, CCZP_PRIME(zp), rng);
    perf_start();
    do {
        cczp_init(zp);
    } while (--loops != 0);
    return perf_time();
}

static double perf_cczp_power_fast(unsigned long loops, cc_size nbits, cc_unit *exponent)
{
    cczp_decl_n(ccn_nof(nbits), zp);
    cc_unit r[ccn_nof(nbits)], msg[ccn_nof(nbits)];
    CCZP_N(zp) = ccn_nof(nbits);

    ccn_random_bits(nbits, msg, rng);
    ccn_random_bits(nbits, CCZP_PRIME(zp), rng);
    cczp_init(zp);
    perf_start();
    do {
        cczp_power_fast(zp, r, msg, exponent);
    } while (--loops != 0);
    return perf_time();
}

static double perf_cczp_power(unsigned long loops, cc_size nbits, cc_unit *exponent)
{
    cczp_decl_n(ccn_nof(nbits), zp);
    cc_unit r[ccn_nof(nbits)], msg[ccn_nof(nbits)];
    CCZP_N(zp) = ccn_nof(nbits);

    ccn_random_bits(nbits, msg, rng);
    ccn_random_bits(nbits, CCZP_PRIME(zp), rng);
    cczp_init(zp);
    perf_start();
    do {
        cczp_power(zp, r, msg, exponent);
    } while (--loops != 0);
    return perf_time();
}

static double perf_cczp_power_rnd(unsigned long loops, cc_size nbits)
{
    cc_unit exponent[ccn_nof(nbits)];

    ccn_random_bits(nbits, exponent, rng);
    return perf_cczp_power(loops, nbits, exponent);
}

static double perf_cczp_power_fast_3(unsigned long loops, cc_size nbits)
{
    cc_unit exponent[ccn_nof(nbits)];
    ccn_seti(ccn_nof(nbits), exponent, 3);
    return perf_cczp_power_fast(loops, nbits, exponent);
}

static double perf_cczp_power_fast_65537(unsigned long loops, cc_size nbits)
{
    cc_unit exponent[ccn_nof(nbits)];
    ccn_seti(ccn_nof(nbits), exponent, 65537);
    return perf_cczp_power_fast(loops, nbits, exponent);
}

#define _TEST(_x) { .name = #_x, .func = perf_ ## _x}
static struct cczp_perf_test {
    const char *name;
    double(*func)(unsigned long loops, cc_size n);
} cczp_perf_tests[] = {
    _TEST(cczp_init),
    _TEST(cczp_power_rnd),
    _TEST(cczp_power_fast_3),
    _TEST(cczp_power_fast_65537),
};

static double perf_cczp(unsigned long loops, unsigned long size, const void *arg)
{
    const struct cczp_perf_test *test=arg;
    return test->func(loops, size);
}

static struct ccperf_family family;

struct ccperf_family *ccperf_family_cczp(int argc, char *argv[])
{
    F_GET_ALL(family, cczp);
    F_SIZES(family, 5, 512);
    family.size_kind=ccperf_size_bits;
    return &family;
}
