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
#include <corecrypto/ccn.h>
#include <corecrypto/ccn_priv.h>

#define CCN_PERF_FUNCTION_N_R_X_Y_WS(func) \
static double perf_ ## func(unsigned long loops, cc_size count) \
{                                       \
    cc_unit x[count];                   \
    cc_unit y[count];                   \
    cc_unit r[2 * count];               \
    double perf_res;                    \
    ccn_random(count, x, rng);          \
    ccn_random(count, y, rng);          \
    CC_DECL_WORKSPACE(ws,20*count);     \
    perf_start();                       \
    do                                  \
        func(count, r, x, y,ws);        \
    while (--loops != 0);               \
    perf_res = perf_time();             \
    CC_FREE_WORKSPACE(ws);              \
    return perf_res;                    \
}

#define CCN_PERF_FUNCTION_N_R_X_Y(func) \
static double perf_ ## func(unsigned long loops, cc_size count) \
{                                       \
    cc_unit x[count];                   \
    cc_unit y[count];                   \
    cc_unit r[2 * count];               \
    ccn_random(count, x, rng);          \
    ccn_random(count, y, rng);          \
    perf_start();                       \
    do                                  \
        func(count, r, x, y);           \
    while (--loops != 0);               \
    return perf_time();                 \
}

#define CCN_PERF_FUNCTION_N_R_X(func)   \
static double perf_ ## func(unsigned long loops, cc_size count) \
{                                       \
    cc_unit x[count];                   \
    cc_unit r[2*count];                 \
    ccn_random(count, x, rng);          \
    perf_start();                       \
    do                                  \
        func(count, r, x);              \
    while (--loops != 0);               \
    return perf_time();                 \
}

#define CCN_PERF_FUNCTION_N_X(func)     \
static double perf_ ## func(unsigned long loops, cc_size count) \
{                                       \
    cc_unit x[count];                   \
    ccn_random(count, x, rng);          \
    perf_start();                       \
    do                                  \
        (void)func(count, x);           \
    while (--loops != 0);               \
    return perf_time();                 \
}

#define CCN_PERF_FUNCTION_N_R_X_t(func,_type) \
static double perf_ ## func(unsigned long loops, cc_size count) \
{                                       \
    cc_unit x[count];                   \
    cc_unit r[count];                   \
    _type b;                          \
    ccn_random(count, x, rng);          \
    ccn_random(1, &b, rng);             \
/* typecast: Number of bits in a cc_unit will always fit into a cc_unit */ \
    b&=(_type)(CCN_UNIT_BITS-1);      \
    if (b == 0) b = 1;                  \
    perf_start();                       \
    do                                  \
        func(count, r, x, b);           \
    while (--loops != 0);               \
    return perf_time();                 \
}

#define CCN_PERF_FUNCTION_N_R_X_B(func) CCN_PERF_FUNCTION_N_R_X_t(func,cc_unit)
#define CCN_PERF_FUNCTION_N_R_X_S(func) CCN_PERF_FUNCTION_N_R_X_t(func,size_t)

CCN_PERF_FUNCTION_N_R_X_Y(ccn_add)
CCN_PERF_FUNCTION_N_R_X_Y(ccn_sub)
CCN_PERF_FUNCTION_N_R_X_Y(ccn_mul)
CCN_PERF_FUNCTION_N_R_X_Y_WS(ccn_mul_ws)
CCN_PERF_FUNCTION_N_R_X(ccn_sqr)
CCN_PERF_FUNCTION_N_R_X_Y(ccn_gcd)

CCN_PERF_FUNCTION_N_R_X(ccn_set)

// CCN_PERF_FUNCTION_N_X(ccn_n)
CCN_PERF_FUNCTION_N_X(ccn_bitlen)

CCN_PERF_FUNCTION_N_R_X_B(ccn_add1)
CCN_PERF_FUNCTION_N_R_X_S(ccn_shift_left)
CCN_PERF_FUNCTION_N_R_X_S(ccn_shift_right)
CCN_PERF_FUNCTION_N_R_X_S(ccn_shift_left_multi)
CCN_PERF_FUNCTION_N_R_X_S(ccn_shift_right_multi)


/* this test the comparaison of identicals (worst case) */
/* putting this as a global so that compiler dont optimize away the actual calls we are trying to measure */
static int r_for_cmp;
static double perf_ccn_cmp(unsigned long loops, cc_size count)
{
    cc_unit s[count];
    cc_unit t[count];
    ccn_random(count, s, rng);
    ccn_set(count, t, s);
    perf_start();
    do {
        r_for_cmp=ccn_cmp(count, s, t);
    } while (--loops != 0);
    return perf_time();
}

//PERF_FUNCTION_N_R_D_L(ccn_read_uint)
//PERF_FUNCTION_N_R_D_L(ccn_write_uint)
/*
#if CCN_MUL_ASM
PERF_FUNCTION_N_R_X_B(ccn_mul1, 0xFFFFFFFF)
PERF_FUNCTION_N_R_X_B(ccn_addmul1, 0xFFFFFFFF)
#endif
*/

#define CCN_TEST(_op) { .name="ccn_"#_op, .di=&_di, .keylen=_keylen }

#define _TEST(_x) { .name = #_x, .func = perf_ ## _x}
static struct ccn_perf_test {
    const char *name;
    double(*func)(unsigned long loops, cc_size count);
} ccn_perf_tests[] = {
    _TEST(ccn_gcd),

    _TEST(ccn_mul),
    _TEST(ccn_mul_ws),
    _TEST(ccn_sqr),
    _TEST(ccn_add),
    _TEST(ccn_sub),

    _TEST(ccn_shift_left),
    _TEST(ccn_shift_right),
    _TEST(ccn_shift_left_multi),
    _TEST(ccn_shift_right_multi),

    _TEST(ccn_add1),
    _TEST(ccn_set),
    _TEST(ccn_cmp),
    _TEST(ccn_bitlen),
};

static double perf_ccn(unsigned long loops, unsigned long size, const void *arg)
{
    const struct ccn_perf_test *test=arg;
    cc_size count=ccn_nof(size);
    return test->func(loops, count);
}

static struct ccperf_family family;

struct ccperf_family *ccperf_family_ccn(int argc, char *argv[])
{
    F_GET_ALL(family, ccn);
    F_SIZES(family, 6, 128);
    family.loops=100;
    family.size_kind=ccperf_size_bits;
    return &family;
}
