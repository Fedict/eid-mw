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


#include <corecrypto/ccz_priv.h>

static void ccz_swap(ccz *a, ccz *b) {
    CC_SWAP(a->n, b->n);
    CC_SWAP(a->isa, b->isa);
    CC_SWAP(a->sac, b->sac);
    CC_SWAP(a->u, b->u);
}

/* q*t + r == s [e.g. s/t, q=quotient, r=remainder]. */
void ccz_divmod(ccz *q, ccz *r, const ccz *s, const ccz *t) {
    if (ccz_is_zero(t)) {
        /* Division by zero is illegal. */
        return;
    }

    /* If s < t then q = 0, r = s */
    if (ccn_cmpn(ccz_n(s), s->u, ccz_n(t), t->u) < 0) {
        if (r) ccz_set(r, s);
        if (q) ccz_zero(q);
        return;
    }

    ccz tr, tt, ta, tq;
    ccz_init(s->isa, &ta);
    ccz_init(s->isa, &tq);
    ccz_init(s->isa, &tr);
    ccz_init(s->isa, &tt);

    size_t k = ccz_bitlen(s) - ccz_bitlen(t);
    ccz_seti(&ta, 1);
    ccz_lsl(&ta, &ta, k);

    ccz_set(&tr, s);
    ccz_set_sign(&tr, 1);

    ccz_lsl(&tt, t, k);
    ccz_set_sign(&tt, 1);

    for (;;) {
        if (ccz_cmp(&tr, &tt) >= 0) {
            ccz_sub(&tr, &tr, &tt);
            ccz_add(&tq, &tq, &ta);
        }
        if (!k--)
            break;

        ccz_lsr(&tt, &tt, 1);
        ccz_lsr(&ta, &ta, 1);
    }

    int rs  = ccz_sign(s);
    int qs = rs == ccz_sign(t) ? 1 : -1;
    if (r) {
        ccz_swap(r, &tr);
        ccz_set_sign(r, ccz_is_zero(q) ? 1 : rs);
    }
    if (q) {
        ccz_swap(q, &tq);
        ccz_set_sign(q, ccz_is_zero(q) ? 1 : qs);
    }

    ccz_free(&ta);
    ccz_free(&tq);
    ccz_free(&tr);
    ccz_free(&tt);
}
