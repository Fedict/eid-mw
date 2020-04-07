/*
 * Copyright (c) 2014,2015 Apple Inc. All rights reserved.
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
#include <corecrypto/ccn_priv.h>

#if CCN_DEDICATED_SQR

typedef cc_unit cc_sqrw;
typedef cc_dunit cc_sqrd;
#define CCSQRW_BITS  CCN_UNIT_BITS

#if !CCN_MUL1_UINT128_SUPPORT_FOR_64BIT_ARCH
#error "ccn_sqr requires support of cc_dunit for 64bit architecture"
#endif

/* Do r = s^2, r is 2 * n cc_units in size, s is n * cc_units in size. */
void ccn_sqr_ws(cc_size n, cc_unit *r, const cc_unit *s, cc_ws_t ws) {
    cc_sqrd prod,sum;
    cc_sqrw si, c,cadd;
    cc_unit *t=ws->start;
    cc_assert((ws->start+2*n)<=ws->end); // Check that provided workspace is sufficient;

    // First iteration separately since t[i]==0;
    t[0]=0;
    c=0;

    // Set of s0*sj
    si=s[0];
    for (cc_size j = 1; j < n; j++) {
        prod= ((cc_sqrd) si * s[j]) + c;  // Implicit C integer promotion
        t[j]=(cc_sqrw)prod;
        c = prod >> CCSQRW_BITS;
    }
    t[n]=c;

    // Set of s0^2
    prod= (cc_sqrd) si * si;
    r[0]= (cc_sqrw)prod;
    sum=(cc_sqrd)2*t[1]+(prod >> CCSQRW_BITS);
    r[1]=(cc_sqrw)sum;
    cadd=(sum >> CCN_UNIT_BITS);

    // Main loop
    for (cc_size i = 1; i < n; i++) {
        // Set of si^2
        c=0;
        si=s[i];

        // Set of si*sj
        for (cc_size j = (i+1); j < n; j++) {
            prod= ((cc_sqrd) si * s[j] + t[i+j]) + c;  // Implicit C integer promotion
            t[i+j]=(cc_sqrw)prod;
            c = prod >> CCSQRW_BITS;
        }
        t[i+n]=c;

        // 2t + r
        prod= (cc_sqrd) si * si + cadd;
        sum=(cc_sqrd)2*t[2*i]
                + (cc_sqrw)prod;
        r[2*i]= (cc_sqrw)sum;
        sum=(cc_sqrd)2*t[2*i+1]
                + (prod >> CCSQRW_BITS)
                + (sum >> CCSQRW_BITS);
        r[2*i+1]= (cc_sqrw)sum;
        cadd=(sum >> CCSQRW_BITS);
    }

#if 0
    // Debug
    cc_unit r_expected[2*n];
    ccn_mul(n, r_expected, s, s);
    if (ccn_cmp(2*n,r,r_expected)) {
        ccn_lprint(n,"Input: ", s);
        ccn_lprint(2*n,"Expected: ", r_expected);
        ccn_lprint(2*n,"Computed: ", r);
        cc_unit delta[2*n];
        ccn_sub(2*n,delta,r_expected,r);
        ccn_lprint(2*n,"Delta:    ", delta);
    }
    cc_assert(ccn_cmp(2*n,r,r_expected)==0);
#endif // 0 debug
}

/* Do r = s^2, r is 2 * count cc_units in size, s are n * cc_units in size. */
void ccn_sqr(cc_size n, cc_unit *r, const cc_unit *s) {
    CC_DECL_WORKSPACE(ws,CCN_SQR_WS_WORKSPACE_N(n));
    ccn_sqr_ws(n,r,s,ws);
    CC_FREE_WORKSPACE(ws);
}

#endif // CCN_DEDICATED_SQR

