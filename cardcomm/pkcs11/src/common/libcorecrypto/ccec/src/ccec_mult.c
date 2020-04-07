/*
 * Copyright (c) 2010,2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccec_priv.h>

#if defined(DEBUG) && 0
#define CCEC_MULT_DEBUG 1
#else
#define CCEC_MULT_DEBUG 0
#endif

// Configuration
#define EC_CURVE_SUPPORT_ONLY_A_MINUS_3

/*!
 @function   XYCZadd
 @abstract   (X,Y)-only co-Z addition with update

 @param      P         Input: X:Y Jacobian coordinate for P
                        Output: X:Y Jacobian coordinate for (P + Q)
 @param      Q        Input: X:Y Jacobian coordinate for Q
                        Output: X:Y Jacobian coordinate for P'
 @param      ws        Worskpace for internal computations
                        To be cleaned up by the caller.
 @result
            Given the twos points P and Q and a curve cp,
            Compute P' and P+Q where
            P' ~= P (same point in the equivalence class)
            P' and (P+Q) have the same Z coordinate
            Z coordinate omitted in output
 */
#define CCEC_XYCZadd_WORKSPACE_N(n) (2*(n))
static void XYCZadd(ccec_const_cp_t cp,
                   cc_unit *P,
                   cc_unit *Q,
                   cc_ws_t ws)
{
    cc_size n=ccec_cp_n(cp);
    cc_unit *t1=&P[0],*t2=&P[n],
            *t3=&Q[0],*t4=&Q[n];
    cc_unit *t5=ws->start;
    cc_unit *t6=ws->start+n;
    ws->start+=2*n;
    cc_assert((ws->start)<=ws->end); // Check that provided workspace is sufficient;

 /*
    Algo 18
    modified to have input and output in same buffer
    use more RAM but less than XYCZaddC so that it does not matter
    Cost: 2S + 4M + 7sub
 */
    cczp_sub_ws(cp.zp,  t5, t3, t1, ws);        //  X2-X1
    cczp_sqr_ws(cp.zp,  t5, t5, ws);        // (X2-X1)^2=A
    cczp_mul_ws(cp.zp,  t6, t3, t5, ws);    // X2.A=C
    cczp_mul_ws(cp.zp,  t3, t1, t5, ws);    // X1.A=B
    cczp_sub_ws(cp.zp,  t5, t4, t2, ws);        // Y2-Y1
    cczp_sqr_ws(cp.zp,  t1, t5, ws);        // (Y2-Y1)^2 = D
    cczp_sub_ws(cp.zp,  t1, t1, t3, ws);        // D - B

    cczp_sub_ws(cp.zp,  t1, t1, t6, ws);        // X3
    cczp_sub_ws(cp.zp,  t6, t6, t3, ws);        // C - B
    cczp_mul_ws(cp.zp,  t4, t2, t6, ws);    // Y1 (C - B)
    cczp_sub_ws(cp.zp,  t2, t3, t1, ws);        // B - X3
    cczp_mul_ws(cp.zp,  t2, t5, t2, ws);    // (Y2-Y1) (B - X3)
    cczp_sub_ws(cp.zp,  t2, t2, t4, ws);        // (Y2-Y1)(B - X3) - Y1 (C - B)

    ws->start=t5; // restore workspace starting point. 
}

/*!
 @function   XYCZadd
 @abstract   (X,Y)-only co-Z conjugate addition with update

 @param      P        Input: X:Y Jacobian coordinate for P
                        Output: X:Y Jacobian coordinate for (P+Q)
 @param      Q        Input: X:Y Jacobian coordinate for Q
                        Output: X:Y Jacobian coordinate for (P-Q)
 @param      ws        Worskpace for internal computations
                        To be cleaned up by the caller.
 @result
             Given the twos points P and Q and a curve cp,
             Compute P' and P+Q where
             P' ~= P (same point in the equivalence class)
             (P-Q) and (P+Q) have the same Z coordinate
             Z coordinate omitted in output
 */

#define CCEC_XYCZaddC_WORKSPACE_N(n) (7*(n))
static void XYCZaddC(ccec_const_cp_t cp,
                   cc_unit *P,
                   cc_unit *Q,
                   cc_ws_t ws)
{
    cc_size n=ccec_cp_n(cp);
    cc_unit *t1=&P[0],*t2=&P[n],
        *t3=&Q[0],*t4=&Q[n];

    cc_unit *t5=ws->start;
    cc_unit *t6=ws->start+n;
    cc_unit *t7=ws->start+2*n;
    ws->start+=3*n;
    cc_assert((ws->start)<=ws->end); // Check that provided workspace is sufficient;

    /*
     Algo 19
     Modified to have same input and output buffers
     Cost: 3S + 5M + 11add/sub
     */
    cczp_sub_ws(cp.zp,  t5, t3, t1, ws);        //  X2-X1
    cczp_sqr_ws(cp.zp,  t5, t5, ws);        // (X2-X1)^2=A
    cczp_mul_ws(cp.zp,  t6, t1, t5, ws);    // X1 * A = B
    cczp_mul_ws(cp.zp,  t1, t3, t5, ws);    // X2 * A = C
    cczp_add_ws(cp.zp,  t5, t4, t2, ws);        // Y2+Y1
    cczp_sub_ws(cp.zp,  t4, t4, t2, ws);        // Y2-Y1
    cczp_sub_ws(cp.zp,  t3, t1, t6, ws);        // C - B
    cczp_mul_ws(cp.zp,  t7, t2, t3, ws);    // Y1 * (C-B)
    cczp_add_ws(cp.zp,  t3, t1, t6, ws);        // C + B

    cczp_sqr_ws(cp.zp,  t1, t4, ws);        // (Y2-Y1)^2
    cczp_sub_ws(cp.zp,  t1, t1, t3, ws);        // X3 = (Y2-Y1)^2 - (C+B)
    cczp_sub_ws(cp.zp,  t2, t6, t1, ws);        // B - X3
    cczp_mul_ws(cp.zp,  t2, t4, t2, ws);    // (Y2-Y1) * (B-X3)

    cczp_sub_ws(cp.zp,  t2, t2, t7, ws);        // Y3 = (Y2-Y1)*(B-X3) - Y1*(C-B)
    cczp_sqr_ws(cp.zp,  t4, t5, ws);        // F = (Y2+Y1)^2
    cczp_sub_ws(cp.zp,  t3, t4, t3, ws);        // X3' = F - (C+B)
    cczp_sub_ws(cp.zp,  t4, t3, t6, ws);        // X3' - B
    cczp_mul_ws(cp.zp,  t4, t4, t5, ws);    // (X3'-B) * (Y2+Y1)
    cczp_sub_ws(cp.zp,  t4, t4, t7, ws);        // Y3' = (X3'-B)*(Y2+Y1) - Y1*(C-B)

    ws->start=t5; // restore workspace starting point.
}


/*!
 @function   XYCZdblJac
 @abstract   Point Doubling in Jacobian with Co-Z output

 @param      twoP      Output: X:Y Jacobian coordinate for 2P
 @param      P         Output: X:Y Jacobian coordinate for P'
 @param      p         Input: P in Jacobian coordinates
 @param      ws        Worskpace for internal computations
                       To be cleaned up by the caller.
 @result
            Given a point P and a curve cp,
            Compute 2P and P' where
            P' ~= P (same point in the equivalence class)
            2P and P' have the same Z coordinate
            Z coordinate omitted in output
 */
#define CCEC_XYCZdblJac_WORKSPACE_N(n) (3*(n))
static void XYCZdblJac(ccec_const_cp_t cp,
                   cc_unit *twoP,
                   cc_unit *P,
                   ccec_const_projective_point_t p,
                   cc_ws_t ws)
{
    cc_size n=ccec_cp_n(cp);
    cc_unit *t1=&twoP[0],*t2=&twoP[n],
        *t3=&P[0],*t4=&P[n];
    cc_unit *t5=ws->start;
    cc_unit *t6=ws->start+n;
    cc_unit *t7=ws->start+2*n;
    ws->start+=3*n;
    cc_assert((ws->start)<=ws->end); // Check that provided workspace is sufficient;
    /*
    Cost (a=-3)     : 6S + 2M + 12add/sub
    Cost (generic)  : 6S + 3M + 10add/sub
     */

    cczp_sqr_ws(cp.zp,  t7, ccec_const_point_x(p,cp), ws);        //  X1^2 
    cczp_add_ws(cp.zp,  t4, t7, t7, ws);        //  2*X1^2
    cczp_add_ws(cp.zp,  t7, t7, t4, ws);        //  3*X1^2
    cczp_sqr_ws(cp.zp,  t3, ccec_const_point_z(p,cp), ws);        //  Z1^2 
    cczp_sqr_ws(cp.zp,  t3, t3, ws);        //  Z1^4 

#ifdef EC_CURVE_SUPPORT_ONLY_A_MINUS_3
    cczp_add_ws(cp.zp,  t5, t3, t3, ws);        //  2*Z1^4
    cczp_add_ws(cp.zp,  t5, t5, t3, ws);        //  3*Z1^4
    cczp_sub_ws(cp.zp,  t7, t7, t5, ws);        //  B = 3*X1^2 - 3.Z1^4
#else
    cczp_mul_ws(cp.zp,  t5, ccec_cp_a(cp), t3, ws); //  a.Z1^4 
    cczp_add_ws(cp.zp,  t7, t7, t5, ws);        //  B = 3*X1^2 + a.Z1^4
#endif
    cczp_sqr_ws(cp.zp,  t4, ccec_const_point_y(p,cp), ws);        //  Y1^2 
    cczp_add_ws(cp.zp,  t4, t4, t4, ws);        //  2Y1^2
    cczp_add_ws(cp.zp,  t5, t4, t4, ws);        //  4Y1^2
    cczp_mul_ws(cp.zp,  t3, t5, ccec_const_point_x(p,cp), ws);    //  A = 4Y1^2.X1 
    cczp_sqr_ws(cp.zp,  t6, t7, ws);        //  B^2 

    cczp_sub_ws(cp.zp,  t6, t6, t3, ws);        //  B^2 - A
    cczp_sub_ws(cp.zp,  t1, t6, t3, ws);        //  X2 = B^2 - 2.A
    cczp_sub_ws(cp.zp,  t6, t3, t1, ws);        //  A - X2

    cczp_mul_ws(cp.zp,  t6, t6, t7, ws);    //  (A - X2)*B 
    cczp_sqr_ws(cp.zp,  t4, t4, ws);        //  (2Y1^2)^2 
    cczp_add_ws(cp.zp,  t4, t4, t4, ws);        //  8.Y1^4 = Y1'
    cczp_sub_ws(cp.zp,  t2, t6, t4, ws);        //  Y2 = (A - X2)*B - 8.Y1^4

    ws->start=t5; // restore workspace starting point.
}

/*!
 @function   XYCZrecoverCoeffJac
 @abstract   Recover Z and lambdaX, lambdaY coefficients for the result point
    if b=0 => R1 - R0 = -P
    if b=1 => R1 - R0 = P

 @param      lambdaX    Output: Correcting coefficient for X
 @param      lambdaY    Output: Correcting coefficient for Y
 @param      Z          Output: Z coordinate
 @param      R0         Input: X:Y Jacobian coordinates for P
 @param      R1         Input: X:Y Jacobian coordinates for Q
 @param      b          Input: Last bit of the scalar
 @param       p          Input: input point to the scalar multiplication
 @param      ws         Worskpace for internal computations
                        To be cleaned up by the caller.
 @result
    {lambaX, lambdaY, Z} so that the result point is recovered from R0
    after the last iteration.
 */
#define CCEC_XYCZrecoverCoeffJac_WORKSPACE_N(n) (0)
static void XYCZrecoverCoeffJac(ccec_const_cp_t cp,
                                cc_unit *lambdaX, cc_unit *lambdaY,
                                cc_unit *Z,
                                const cc_unit *R0,
                                const cc_unit *R1,
                                int bit,
                                ccec_const_projective_point_t p,
                                cc_ws_t ws) {
    cc_size n=ccec_cp_n(cp);
    cc_unit *t1=lambdaX,*t2=lambdaY, *t3=Z;

    cczp_sub_ws(cp.zp,  t3, R1, R0, ws); // X1 - X0
    cczp_mul_ws(cp.zp,  t3, cc_muxp(bit, &R1[n], &R0[n])  , t3, ws); // Yb * (X1-X0)
    cczp_mul_ws(cp.zp,  t3, ccec_const_point_x(p,cp), t3, ws); // XP * Yb*(X1-X0)
    cczp_mul_ws(cp.zp,  t3, ccec_const_point_z(p,cp), t3, ws); // ZP * XP*Yb*(X1-X0)

    cczp_mul_ws(cp.zp,  t2, cc_muxp(bit, R1, R0)  , ccec_const_point_y(p,cp), ws); // Xb*YP
    cczp_sqr_ws(cp.zp,  t1, t2, ws);        // (Xb*YP)^2
    cczp_mul_ws(cp.zp,  t2, t2, t1, ws);    // (Xb*YP)^3

     // {T1,T2,T3}
}


/*!
 @function   ccec_mult_ws
 @abstract   Scalar multiplication on the curve cp

 @param      cp    Curve parameter
 @param      r     Output point d.s
 @param      d     Scalar of size ccec_cp_n(cp) cc_units. 
                    Required to verify d<=q where q is the order of the curve
 @param      s     Input point in Jacobian projective representation
 @param      rng   Random for randomization
 @param      ws         Worskpace for internal computations
            To be cleaned up by the caller.
 @result
 */

#define CCEC_MULT_WORKSPACE_SIZE(n) (11*(n))
static int ccec_mult_ws(ccec_const_cp_t cp, ccec_projective_point_t r, const cc_unit *d, ccec_const_projective_point_t s, struct ccrng_state *rng, cc_ws_t ws) {
    cc_size n=ccec_cp_n(cp);
    size_t dbitlen=ccn_bitlen(n,d);

    cc_unit *R0=ws->start;          // R0 and R1 are full points:
    cc_unit *R1=ws->start+2*n;      // X in [0..n-1] and Y in [n..2n-1]
    ws->start+=4*n;
    cc_assert((ws->start)<=ws->end); // Check that provided workspace is sufficient;

    // Check edge cases

    // Scalar d must be <= q to
    // prevent intermediary results to be the point at infinity
    // corecrypto to take care to meet this requirement
    cc_assert(ccn_cmp(n,d,cczp_prime(ccec_cp_zq(cp)))<=0); // d <= q
    ccn_sub1(n,R0,cczp_prime(ccec_cp_zq(cp)),1); // q-1
    if (dbitlen < 1) {
        ccn_clear(n, ccec_point_x(r, cp));
        ccn_clear(n, ccec_point_y(r, cp));
        ccn_clear(n, ccec_point_z(r, cp));
    } else if (dbitlen == 1) {
        // If d=1 => r=s
        ccn_set(n, ccec_point_x(r, cp), ccec_const_point_x(s, cp));
        ccn_set(n, ccec_point_y(r, cp), ccec_const_point_y(s, cp));
        ccn_set(n, ccec_point_z(r, cp), ccec_const_point_z(s, cp));
    } else if (ccn_cmp(n,d,R0)==0) {
        // If d=(q-1) => r=-s
        // Case not handle by Montgomery Ladder because R1-R0 = s.
        // On the last iteration r=R0 => R1 is equal to infinity which is not supported
        ccn_set(n, ccec_point_x(r, cp), ccec_const_point_x(s, cp));
        ccn_sub(n, ccec_point_y(r, cp), cczp_prime(cp.zp), ccec_const_point_y(s, cp));
        ccn_set(n, ccec_point_z(r, cp), ccec_const_point_z(s, cp));
    }
    else {
        // Randomize buffer to harden against cache attacks
        // TODO: scalar randomization.
        cc_unit c=1;
        if (rng) ccn_random_bits(1,&c,rng);

        // Core of the EC scalar multiplication
        cc_unit dbit; // Bit of d at index i
        cc_unit *R[2] __attribute__((aligned(16)))={R0,R1};

        XYCZdblJac(cp,R[c^1],R[c^0],s,ws);

        // Main loop
        for (unsigned long i = dbitlen - 2; i>0; --i) {
            dbit=c^(ccn_bit(d, i));
            XYCZaddC(cp,R[dbit],R[1^dbit],ws);
            XYCZadd(cp,R[dbit],R[1^dbit],ws);
            // Per Montgomery Ladder:
            // Invariably, R1 - R0 = P at this point of the loop
        }

        // Last iteration
        dbit=c^(ccn_bit(d, 0));
        XYCZaddC(cp,R[dbit],R[1^dbit],ws);
        // If d0 =      0           1
        //          R1-R0=-P     R1-R0=P
        // Therefore we can reconstruct the Z coordinate
        // To save an inversion and keep the result in Jacobian projective coordinates,
        //  we compute coefficient for X and Y.
        XYCZrecoverCoeffJac(cp,
                            ccec_point_x(r, cp),
                            ccec_point_y(r, cp),
                            ccec_point_z(r, cp),
                            R[c^1],R[c^0],
                            (int)(dbit^c),
                            s,
                            ws);
        XYCZadd(cp,R[dbit],R[1^dbit],ws);

        // Apply coefficients
        cczp_mul_ws(cp.zp,  ccec_point_x(r, cp), ccec_point_x(r, cp), &(R[c][0]), ws); // X0 * lambdaX
        cczp_mul_ws(cp.zp,  ccec_point_y(r, cp), ccec_point_y(r, cp), &(R[c][n]), ws); // Y0 * lambdaY
    }
#if CCEC_MULT_DEBUG
    ccn_lprint(n, "Result X:", ccec_point_x(r, cp));
    ccn_lprint(n, "Result Y:", ccec_point_y(r, cp));
    ccn_lprint(n, "Result Z:", ccec_point_z(r, cp));
#endif
    ws->start-=4*n;

    return 0;
}

// Requires the point s to have been generated by "ccec_projectify"
int ccec_mult(ccec_const_cp_t cp, ccec_projective_point_t r, const cc_unit *d,
                                    ccec_const_projective_point_t s,
                                    struct ccrng_state *rng) {
    int status;
    cc_size n=ccec_cp_n(cp);
    CC_DECL_WORKSPACE(ws,CCEC_MULT_WORKSPACE_SIZE(n));
    status=ccec_mult_ws(cp,r,d,s,rng,ws);
    CC_CLEAR_AND_FREE_WORKSPACE(ws);
    return status;
}
