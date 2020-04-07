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


#include <corecrypto/ccperf.h>
#include <corecrypto/ccsrp.h>
#include <corecrypto/ccsrp_gp.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>

static ccsrp_ctx *
create_context(const struct ccdigest_info *di, cc_size bits)
{
    ccsrp_ctx * srp;
    ccdh_const_gp_t gp;
    
    if (bits == 1024)
        gp = ccsrp_gp_rfc5054_1024();
    else if (bits == 2048)
        gp = ccsrp_gp_rfc5054_2048();
    else if (bits == 3072)
        gp = ccsrp_gp_rfc5054_3072();
    else if (bits == 4096)
        gp = ccsrp_gp_rfc5054_4096();
    else {
        printf("unknow bits: %d\n", (int)bits);
        abort();
    }

    srp = malloc(ccsrp_sizeof_srp(di, gp));
    if (srp == NULL)
        return NULL;

    ccsrp_ctx_init(srp, di, gp);
    return srp;
}


static double internal_ccsrp_generate_verifier(const struct ccdigest_info *di, unsigned long loops, cc_size nbits)
{
    ccsrp_ctx *srp = create_context(di, nbits);
    char *password = "password";
    size_t password_len = strlen(password);
    char *salt = "random-salt-1234567890";
    size_t salt_len = strlen(salt);
    uint8_t verifier[ccsrp_ctx_sizeof_n(srp)];
    double t;

    perf_start();
    do {
        ccsrp_generate_verifier(srp, "username", password_len, password, salt_len, salt, verifier);
    } while (--loops != 0);
    t = perf_time();
    free(srp);
    return t;
}

static double perf_ccsrp_generate_verifier_sha1(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_generate_verifier(ccsha1_di(), loops, nbits);
}

static double perf_ccsrp_generate_verifier_sha256(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_generate_verifier(ccsha256_di(), loops, nbits);
}

static double perf_ccsrp_generate_verifier_sha512(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_generate_verifier(ccsha512_di(), loops, nbits);
}

static double internal_ccsrp_client_start(const struct ccdigest_info *di, unsigned long loops, cc_size nbits)
{
    ccsrp_ctx *srp = create_context(di, nbits);
    char *username = "username";
    char *password = "password";
    size_t password_len = strlen(password);
    char *salt = "random-salt-1234567890";
    size_t salt_len = strlen(salt);
    uint8_t verifier[ccsrp_ctx_sizeof_n(srp)];
    uint8_t A[ccsrp_ctx_sizeof_n(srp)];
    double t;
    
    ccsrp_generate_verifier(srp, username, password_len, password, salt_len, salt, verifier);
    
    perf_start();
    do {
        ccsrp_client_start_authentication(srp, rng, A);
    } while (--loops != 0);
    t = perf_time();
    free(srp);
    return t;
}

static double perf_ccsrp_client_start_sha1(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_client_start(ccsha1_di(), loops, nbits);
}

static double perf_ccsrp_client_start_sha256(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_client_start(ccsha256_di(), loops, nbits);
}

static double perf_ccsrp_client_start_sha512(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_client_start(ccsha512_di(), loops, nbits);
}


static double internal_ccsrp_validate_verifier(const struct ccdigest_info *di, unsigned long loops, cc_size nbits)
{
    ccsrp_ctx *client_srp = create_context(di, nbits);
    ccsrp_ctx *server_srp = create_context(di, nbits);
    char *username = "username";
    char *password = "password";
    size_t password_len = strlen(password);
    char *salt = "random-salt-1234567890";
    size_t salt_len = strlen(salt);
    uint8_t verifier[ccsrp_ctx_sizeof_n(client_srp)];
    uint8_t A[ccsrp_ctx_sizeof_n(client_srp)];
    uint8_t B[ccsrp_ctx_sizeof_n(client_srp)];
    double t;
    
    ccsrp_generate_verifier(server_srp, username, password_len, password, salt_len, salt, verifier);
    ccsrp_client_start_authentication(client_srp, rng, A);

    perf_start();
    do {
        ccsrp_server_start_authentication(server_srp, rng, username, salt_len, salt, verifier, A, B);
    } while (--loops != 0);
    t = perf_time();
    free(client_srp);
    free(server_srp);
    return t;
}

static double perf_ccsrp_validate_verifier_sha1(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_validate_verifier(ccsha1_di(), loops, nbits);
}

static double perf_ccsrp_validate_verifier_sha256(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_validate_verifier(ccsha256_di(), loops, nbits);
}

static double perf_ccsrp_validate_verifier_sha512(unsigned long loops, cc_size nbits)
{
    return internal_ccsrp_validate_verifier(ccsha512_di(), loops, nbits);
}



#define _TEST(_x) { .name = #_x, .func = perf_ ## _x}
static struct ccsrp_perf_test {
    const char *name;
    double(*func)(unsigned long loops, cc_size nbits);
} ccsrp_perf_tests[] = {
    _TEST(ccsrp_generate_verifier_sha1),
    _TEST(ccsrp_generate_verifier_sha256),
    _TEST(ccsrp_generate_verifier_sha512),
    _TEST(ccsrp_client_start_sha1),
    _TEST(ccsrp_client_start_sha256),
    _TEST(ccsrp_client_start_sha512),
    _TEST(ccsrp_validate_verifier_sha1),
    _TEST(ccsrp_validate_verifier_sha256),
    _TEST(ccsrp_validate_verifier_sha512),
};

static double perf_ccsrp(unsigned long loops, unsigned long size, const void *arg)
{
    const struct ccsrp_perf_test *test=arg;
    return test->func(loops, size);
}

static struct ccperf_family family;

struct ccperf_family *ccperf_family_ccsrp(int argc, char *argv[])
{
    F_GET_ALL(family, ccsrp);
    family.nsizes=3;
    family.sizes=malloc(family.nsizes*sizeof(unsigned long));
    family.sizes[0]=2048;
    family.sizes[1]=3072;
    family.sizes[2]=4096;
    family.size_kind=ccperf_size_bits;
    return &family;
}
