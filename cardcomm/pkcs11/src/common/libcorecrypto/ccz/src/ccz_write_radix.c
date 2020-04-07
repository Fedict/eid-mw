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


#include <corecrypto/ccz_priv.h>

const char *ccn_radix_digit_map = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

/* q*t + r == s [e.g. s/t, q=quotient, r=remainder]. */
static void ccn_divn(cc_size nqs, cc_unit *q, cc_unit *r, const cc_unit *s, size_t nrt, cc_unit *t) {
    if (ccn_is_zero(nrt, t)) {
        /* Division by zero is illegal. */
        return;
    }

    /* If s < t then q = 0, r = s */
    if (ccn_cmpn(nqs, s, nrt, t) < 0) {
        if (r) ccn_setn(nrt, r, CC_MIN(nrt, nqs), s);
        if (q) ccn_zero(nqs, q);
        return;
    }

    /* s >= t => k >= 0 */
    size_t k = ccn_bitlen(nqs, s);
    size_t l = ccn_bitlen(nrt, t);
    assert(k >= l);
    k -= l;

    cc_unit tr[nqs];
    cc_unit tt[nqs];
    cc_unit tq[nqs];

    ccn_set(nqs, tr, s);

    ccn_setn(nqs, tt, nrt, t);
    ccn_shift_left_multi(nqs, tt, tt, k);

    ccn_zero(nqs, tq);

    for (;;) {
        if (ccn_cmp(nqs, tr, tt) >= 0) {
            ccn_sub(nqs, tr, tr, tt);
            ccn_set_bit(tq, k, 1);
        }
        if (!k)
            break;

        --k;
        ccn_shift_right(nqs, tt, tt, 1);
    }

    if (r) {
        ccn_setn(nrt, r, CC_MIN(nrt, nqs), tr);
    }
    if (q) {
        ccn_set(nqs, q, tq);
    }
}

static void ccn_div1(cc_size n, cc_unit *q, cc_unit *r, const cc_unit *s, cc_unit v) {
    if (n == 0) {
        *r = 0;
        return;
    }

    size_t k = ccn_bitlen(1, &v) - 1;
    size_t l = ccn_trailing_zeros(1, &v);
    if (k == l) {
        /* Divide by power of 2, remainder in *r. */
        *r = s[0] & (v - 1);
        ccn_shift_right(n, q, s, k);
    } else {
        ccn_divn(n, q, r, s, 1, &v);
    }
}

static size_t ccn_write_radix_size(cc_size n, const cc_unit *s,
                                   unsigned radix) {
    if (ccn_is_zero(n, s)) {
        return 1;
    }

    /* digs is the digit count */
    cc_unit uradix[1] = { radix };
    size_t k = ccn_bitlen(1, uradix) - 1;
    size_t l = ccn_trailing_zeros(1, uradix);
    if (k == l) {
        /* Radix is 2**k. */
        return (ccn_bitlen(n, s) + k - 1) / k;
    } else {
        size_t size = 0;
        n = ccn_n(n, s);
        cc_unit t[n];
        ccn_set(n, t, s);
        cc_unit v[1];
        while (n) {
            ccn_div1(n, t, v, t, radix);
            n = ccn_n(n, t);
            ++size;
        }
        return size;
    }
}

static void ccn_write_radix(cc_size n, const cc_unit *s,
                            size_t out_size, char *out, unsigned radix) {
    assert(radix <= strlen(ccn_radix_digit_map));
    cc_unit t[n];
    cc_unit v[1]={0};
    ccn_set(n, t, s);
    /* Write from the end of the buffer backwards. */
    for (char *p = out, *q = p + out_size; p < q;) {
        ccn_div1(n, t, v, t, radix);
        n = ccn_n(n, t);
        *--q = ccn_radix_digit_map[v[0]];
        if (ccn_is_zero(n, t)) {
            /* Pad remaining space with zeros. */
            while (p < q) {
                *--q = '0';
            }
            break;
        }
    }
}

size_t ccz_write_radix_size(const ccz *s, unsigned radix) {
    return ccn_write_radix_size(ccz_n(s), s->u, radix) + (ccz_sign(s) < 0 ? 1 : 0);
}

void ccz_write_radix(const ccz *s, size_t out_size, void *out, unsigned radix) {
    char *p = out;
    if (ccz_sign(s) < 0) {
        *p++ = '-';
        --out_size;
    }
    ccn_write_radix(ccz_n(s), s->u, out_size, p, radix);
}
