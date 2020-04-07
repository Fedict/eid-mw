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


#include <corecrypto/ccn.h>
#include <corecrypto/cczp.h>
#if 0
/* q*t + r == s [e.g. s/t, q=quotient, r=remainder]. */
void ccn_divmod(cc_size n, cc_unit *q, cc_unit *r, const cc_unit *s, const cc_unit *t) {
    if (ccn_is_zero(n, t)) {
        /* Division by zero is illegal. */
        return;
    }
    
    /* If s < t then q = 0, r = s */
    if (ccn_cmp(n, s, t) < 0) {
        if (r) ccn_set(n, r, s);
        if (q) ccn_zero(n, q);
        return;
    }
    
    cc_unit tr[n], tt[n], ta[n], tq[n];
    ccn_zero(n, tr);
    ccn_zero(n, tt);
    ccn_zero(n, ta);
    ccn_zero(n, tq);

    cc_size k = ccn_bitlen(n, s) - ccn_bitlen(n, t);
    ccn_seti(n, ta, 1);
    ccn_shift_left_multi(n, ta, ta, k);
    
    ccn_set(n, tr, s);
    
    ccn_shift_left_multi(n, tt, t, k);
        
    for (;;) {
        if (ccn_cmp(n, tr, tt) >= 0) {
            ccn_sub(n, tr, tr, tt);
            ccn_add(n, tq, tq, ta);
        }
        if (!k--)
            break;
        
        ccn_shift_left(n, tt, tt, 1);
        ccn_shift_left(n, ta, ta, 1);
    }
        
    if (r) {
        ccn_set(n, r, tr);
    }
    
    if (q) {
        ccn_set(n, q, tq);
    }
}
#else
void ccn_divmod(cc_size n, cc_unit *q, cc_unit *r, const cc_unit *s_2n, const cc_unit *t) {
    cczp_decl_n(n, zd);
    CCZP_N(zd) = n;
    ccn_set(n, CCZP_PRIME(zd), t);
    cczp_init(zd);
    
    cc_unit rtmp[n];
    cc_unit qtmp[n];
    
    if(!r) { r = rtmp; ccn_zero(n, r); }
    if(!q) { q = qtmp; ccn_zero(n, q); }
    
    cczp_div(zd, q, r, s_2n);
}

#endif
