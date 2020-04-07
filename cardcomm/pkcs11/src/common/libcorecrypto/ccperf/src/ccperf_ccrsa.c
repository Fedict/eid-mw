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


#include <corecrypto/ccperf.h>
#include <corecrypto/ccrsa.h>
#include <corecrypto/ccsha1.h>

static cc_size grsakey_nbits = 0;
static ccrsa_full_ctx_t grsakey;

const uint8_t e65537[] = { 0x01, 0x00, 0x01 };
static double internal_ccrsa_generate_key(int do931, unsigned long loops, cc_size nbits)
{
    if (grsakey_nbits != nbits) {
        grsakey_nbits = nbits;
        grsakey.full = (ccrsa_full_ctx *)realloc(grsakey.full, ccrsa_full_ctx_size(ccn_sizeof(nbits)));
    }
    perf_start();
    do {
        if(!do931) ccrsa_generate_key(nbits, grsakey, sizeof(e65537), e65537, rng);
        else ccrsa_generate_931_key(nbits, grsakey, sizeof(e65537), e65537, rng, rng);
    } while (--loops != 0);
    return perf_time();
}

static double internal_ccrsa_sign(int do931, unsigned long loops, cc_size nbits)
{
    uint8_t sig[ccn_sizeof(nbits)];
    size_t siglen = sizeof(sig);
    uint8_t digest[CCSHA1_OUTPUT_SIZE] = "01234567890123456789";
    int ret;
    
    if (grsakey_nbits != nbits) {
        grsakey_nbits = nbits;
        grsakey.full = (ccrsa_full_ctx *)realloc(grsakey.full, ccrsa_full_ctx_size(ccn_sizeof(nbits)));
        if(!do931) ret = ccrsa_generate_key(nbits, grsakey, sizeof(e65537), e65537, rng);
        else ret = ccrsa_generate_931_key(nbits, grsakey, sizeof(e65537), e65537, rng, rng);
        if (ret) abort();
    }

    perf_start();
    do {
        ret = ccrsa_sign_pkcs1v15(grsakey, ccoid_sha1, CCSHA1_OUTPUT_SIZE, digest, &siglen, sig);
        if (ret) abort();
    } while (--loops != 0);
    return perf_time();
}

static double internal_ccrsa_verify(int do931, unsigned long loops, cc_size nbits)
{
    uint8_t sig[ccn_sizeof(nbits)];
    size_t siglen = sizeof(sig);
    uint8_t digest[CCSHA1_OUTPUT_SIZE] = "01234567890123456789";
    int ret;

    if (grsakey_nbits != nbits) {
        grsakey_nbits = nbits;
        grsakey.full = (ccrsa_full_ctx *)realloc(grsakey.full, ccrsa_full_ctx_size(ccn_sizeof(nbits)));
        if(!do931) ret = ccrsa_generate_key(nbits, grsakey, sizeof(e65537), e65537, rng);
        else ret = ccrsa_generate_931_key(nbits, grsakey, sizeof(e65537), e65537, rng, rng);
        if (ret) abort();
    }
    bool ok;

    ret = ccrsa_sign_pkcs1v15(grsakey, ccoid_sha1, CCSHA1_OUTPUT_SIZE, digest, &siglen, sig);
    if (ret) abort();

    perf_start();
    do {
        ret = ccrsa_verify_pkcs1v15(ccrsa_ctx_public(grsakey), ccoid_sha1, CCSHA1_OUTPUT_SIZE, digest, siglen, sig, &ok);
        if (ret || !ok) abort();
    } while (--loops != 0);
    return perf_time();
}

static double perf_ccrsa_generate_key(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_generate_key(0, loops, nbits);
}

static double perf_ccrsa_sign(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_sign(0, loops, nbits);
}

static double perf_ccrsa_verify(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_verify(0, loops, nbits);
}

static double perf_ccrsa931_generate_key(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_generate_key(0, loops, nbits);
}

static double perf_ccrsa931_sign(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_sign(0, loops, nbits);
}

static double perf_ccrsa931_verify(unsigned long loops, cc_size nbits)
{
    return internal_ccrsa_verify(0, loops, nbits);
}


#define _TEST(_x) { .name = #_x, .func = perf_ ## _x}
static struct ccrsa_perf_test {
    const char *name;
    double(*func)(unsigned long loops, cc_size nbits);
} ccrsa_perf_tests[] = {
    _TEST(ccrsa_generate_key),
    _TEST(ccrsa_sign),
    _TEST(ccrsa_verify),
    _TEST(ccrsa931_generate_key),
    _TEST(ccrsa931_sign),
    _TEST(ccrsa931_verify),
};

static double perf_ccrsa(unsigned long loops, unsigned long size, const void *arg)
{
    const struct ccrsa_perf_test *test=arg;
    return test->func(loops, size);
}

static struct ccperf_family family;

struct ccperf_family *ccperf_family_ccrsa(int argc, char *argv[])
{
    F_GET_ALL(family, ccrsa);
    family.nsizes=6;
    family.sizes=malloc(family.nsizes*sizeof(unsigned long));
    family.sizes[0]=1024;
    family.sizes[1]=1152;
    family.sizes[2]=1280;
    family.sizes[3]=1536;
    family.sizes[4]=2048;
    family.sizes[5]=4096;
    family.size_kind=ccperf_size_bits;
    return &family;
}
