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


#import "ECValidation.h"
#import "ccec_unit.h"
#import <corecrypto/cc_memory.h>
#import <corecrypto/ccec_priv.h>
#import <corecrypto/cczp_priv.h>

@implementation ECValidation

- (void) cczpModTest: (cczp_const_t) zp : (NSString *)cname
{
    cc_unit r[cczp_n(zp)],
            one[cczp_n(zp)];
    cc_unit s[2 * cczp_n(zp)],
            t[2 * cczp_n(zp)];
    CC_DECL_WORKSPACE(ws, CCZP_MOD_WORKSPACE_N(cczp_n(zp)));
    ccn_zero(cczp_n(zp), s + cczp_n(zp));
    ccn_set(cczp_n(zp), s, cczp_prime(zp));

    if (cczp_is_montgomery(zp)) {
        ccn_seti(2*cczp_n(zp), t, 1);
        cczp_mod_prime(zp)(zp, one, t, ws); // R^-1
    }
    else
    {
        ccn_seti(cczp_n(zp), one, 1);
    }

    cczp_mod_prime(zp)(zp, r, s, ws);
    XCTAssertTrue(ccn_is_zero(cczp_n(zp), r), @"%@ cczp_mod(p) == 0", cname);

    ccn_add1(2 * cczp_n(zp), s, s, 1);
    cczp_mod_prime(zp)(zp, r, s, ws);
    XCAssertCCNEquals(cczp_n(zp), r, one, @"%@ cczp_mod(p + 1) == 1", cname);

    ccn_sub1(2 * cczp_n(zp), s, s, 2);
    ccn_sub(cczp_n(zp), t, cczp_prime(zp), one); // p - 1
    cczp_mod_prime(zp)(zp, r, s, ws);
    XCAssertCCNEquals(cczp_n(zp), r, t, @"%@ cczp_mod(p - 1) == p - 1", cname);

    ccn_sqr(cczp_n(zp), t, s);
    cczp_mod_prime(zp)(zp, r, t, ws);
    XCAssertCCNEquals(cczp_n(zp), r, one, @"%@ cczp_mod((p-1)^2) == 1", cname);

    CC_FREE_WORKSPACE(ws);
}

- (void) evaluateCurve:(ccec_const_cp_t)cp : (ccec_const_affine_point_t) sa : (ccec_const_affine_point_t) ta : (ccec_const_affine_point_t) radd : (ccec_const_affine_point_t) rsub : (ccec_const_affine_point_t) rdbl : (const cc_unit *) d : (ccec_const_affine_point_t) rmul : (const cc_unit *) e : (ccec_const_affine_point_t) rtmul : (NSString *)cname
{
    /* Validate that cczp_init generates the same recipricol that we hardcoded in the cp. */
    cczp_decl_n(ccec_cp_n(cp), zq);
    CCZP_N(zq) = ccec_cp_n(cp);
    ccn_set(ccec_cp_n(cp), CCZP_PRIME(zq), cczp_prime(ccec_cp_zq(cp)));
    cczp_init(zq);
    XCAssertCCNEquals(1 + ccec_cp_n(cp), cczp_recip(ccec_cp_zq(cp)), cczp_recip(zq), @"%@ cczq recip matches computed recip", cname);

    /* Next let's test the mod function for zp and zq. */
    [self cczpModTest: ccec_cp_zq(cp) : cname];

    ccec_projective_point *sp = calloc(3, ccec_ccn_size(cp));
    ccec_projective_point *tp = calloc(3, ccec_ccn_size(cp));
    ccec_projective_point *rp = calloc(3, ccec_ccn_size(cp));
    ccec_affine_point *ra = calloc(2, ccec_ccn_size(cp));

    //ccec_alprint(cp, "s", sa);
    //ccec_alprint(cp, "t", ta);

    ccec_projectify(cp, sp, sa, NULL);
    ccec_projectify(cp, tp, ta, NULL);

    //ccec_plprint(cp, "s", sp);
    //ccec_plprint(cp, "t", tp);

    /* test ccec_projectify -> ccec_affinify */
    ccec_affinify(cp, ra, sp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(sa, cp), @"%@ ccec_affinify(ccec_projectify(s)) == s", cname);

    /* test ccec_full_add */
    ccec_full_add(cp, rp, sp, tp);
    ccec_affinify(cp, ra, rp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(radd, cp), @"%@ full_add R = S + T", cname);

    /* test ccec_full_sub */
    ccec_full_sub(cp, rp, sp, tp);
    ccec_affinify(cp, ra, rp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(rsub, cp), @"%@ full_sub R = S - T", cname);

    /* test ccec_double */
    ccec_double(cp, rp, sp);
    ccec_affinify(cp, ra, rp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(rdbl, cp), @"%@ double R = 2S", cname);

    /* test ccec_mult */
    ccec_mult(cp, rp, d, sp,NULL);
    ccec_affinify(cp, ra, rp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(rmul, cp), @"%@ mult R = dS", cname);

    /* test ccec_twin_mult */
    ccec_twin_mult(cp, rp, d, sp, e, tp);
    ccec_affinify(cp, ra, rp);
    XCAssertECPEquals(2 * ccec_cp_n(cp), ccec_point_x(ra, cp), ccec_const_point_x(rtmul, cp), @"%@ twin mult R = dS + eT", cname);

    free(ra);
    free(rp);
    free(tp);
    free(sp);
}

- (void) testP192 {
    static const cc_unit sa[2 * CCN192_N] = {
        CCN192_C(d4,58,e7,d1,27,ae,67,1b,0c,33,02,66,d2,46,76,93,53,a0,12,07,3e,97,ac,f8),
        CCN192_C(32,59,30,50,0d,85,1f,33,6b,dd,c0,50,cf,7f,b1,1b,56,73,a1,64,50,86,df,3b)
    },
    ta[2 * CCN192_N] = {
        CCN192_C(f2,2c,43,95,21,3e,9e,be,67,dd,ec,dd,87,fd,bd,01,be,16,fb,05,9b,97,53,a4),
        CCN192_C(26,44,24,09,6a,f2,b3,59,77,96,db,48,f8,df,b4,1f,a9,ce,cc,97,69,1a,9c,79)
    },
    /* Full add radd = S + T */
    radd[2 * CCN192_N] = {
        CCN192_C(48,e1,e4,09,6b,9b,8e,5c,a9,d0,f1,f0,77,b8,ab,f5,8e,84,38,94,de,4d,02,90),
        CCN192_C(40,8f,a7,7c,79,7c,d7,db,fb,16,aa,48,a3,64,8d,3d,63,c9,41,17,d7,b6,aa,4b)
    },
    /* Full subtract R = S − T */
    rsub[2 * CCN192_N] = {
        CCN192_C(fc,96,83,cc,5a,bf,b4,fe,0c,c8,cc,3b,c9,f6,1e,ab,c4,68,8f,11,e9,f6,4a,2e),
        CCN192_C(09,3e,31,d0,0f,b7,82,69,73,2b,1b,d2,a7,3c,23,cd,d3,17,45,d0,52,3d,81,6b)
    },
    /* Double R = 2S */
    rdbl[2 * CCN192_N] = {
        CCN192_C(30,c5,bc,6b,8c,7d,a2,53,54,b3,73,dc,14,dd,8a,0e,ba,42,d2,5a,3f,6e,69,62),
        CCN192_C(0d,de,14,bc,42,49,a7,21,c4,07,ae,db,f0,11,e2,dd,bb,cb,29,68,c9,d8,89,cf)
    },
    /* Scalar multiply R = dS */
    d[CCN192_N] = {
        CCN192_C(a7,8a,23,6d,60,ba,ec,0c,5d,d4,1b,33,a5,42,46,3a,82,55,39,1a,f6,4c,74,ee)
    },
    rmul[2 * CCN192_N] = {
        CCN192_C(1f,ae,e4,20,5a,4f,66,9d,2d,0a,8f,25,e3,bc,ec,9a,62,a6,95,29,65,bf,6d,31),
        CCN192_C(5f,f2,cd,fa,50,8a,25,81,89,23,67,08,7c,69,6f,17,9e,7a,4d,7e,82,60,fb,06)
    },
    /* Joint scalar multiply R = dS + eT (d as above) */
    e[CCN192_N] = {
        CCN192_C(c4,be,3d,53,ec,30,89,e7,1e,4d,e8,ce,ab,7c,ce,88,9b,c3,93,cd,85,b9,72,bc),
    },
    rtmul[2 * CCN192_N] = {
        CCN192_C(01,9f,64,ee,d8,fa,9b,72,b7,df,ea,82,c1,7c,9b,fa,60,ec,b9,e1,77,8b,5b,de),
        CCN192_C(16,59,0c,5f,cd,86,55,fa,4c,ed,33,fb,80,0e,2a,7e,3c,61,f3,5d,83,50,36,44)
    };
    [self evaluateCurve: ccec_cp_192() : (const cc_unit *)sa : (const cc_unit *)ta : (const cc_unit *)radd : (const cc_unit *)rsub : (const cc_unit *)rdbl : (const cc_unit *)d : (const cc_unit *)rmul : (const cc_unit *)e : (const cc_unit *)rtmul : @"p192"];
}

- (void) testP224 {
    static const cc_unit sa[2 * CCN224_N] = {
        CCN224_C(6e,ca,81,4b,a5,9a,93,08,43,dc,81,4e,dd,6c,97,da,95,51,8d,f3,c6,fd,f1,6e,9a,10,bb,5b),
        CCN224_C(ef,4b,49,7f,09,63,bc,8b,6a,ec,0c,a0,f2,59,b8,9c,d8,09,94,14,7e,05,dc,6b,64,d7,bf,22)
    },
    ta[2 * CCN224_N] = {
        CCN224_C(b7,2b,25,ae,a5,cb,03,fb,88,d7,e8,42,00,29,69,64,8e,6e,f2,3c,5d,39,ac,90,38,26,bd,6d),
        CCN224_C(c4,2a,8a,4d,34,98,4f,0b,71,b5,b4,09,1a,f7,dc,eb,33,ea,72,9c,1a,2d,c8,b4,34,f1,0c,34)
    },
    /* Full add radd = S + T */
    radd[2 * CCN224_N] = {
        CCN224_C(23,6f,26,d9,e8,4c,2f,7d,77,6b,10,7b,d4,78,ee,0a,6d,2b,cf,ca,a2,16,2a,fa,e8,d2,fd,15),
        CCN224_C(e5,3c,c0,a7,90,4c,e6,c3,74,6f,6a,97,47,12,97,a0,b7,d5,cd,f8,d5,36,ae,25,bb,0f,da,70)
    },
    /* Full subtract R = S − T */
    rsub[2 * CCN224_N] = {
        CCN224_C(db,41,12,bc,c8,f3,4d,4f,0b,36,04,7b,ca,10,54,f3,61,54,13,85,2a,79,31,33,52,10,b3,32),
        CCN224_C(90,c6,e8,30,4d,a4,81,38,78,c1,54,0b,23,96,f4,11,fa,cf,78,7a,52,0a,0f,fb,55,a8,d9,61)
    },
    /* Double R = 2S */
    rdbl[2 * CCN224_N] = {
        CCN224_C(a9,c9,6f,21,17,de,e0,f2,7c,a5,68,50,eb,b4,6e,fa,d8,ee,26,85,2f,16,5e,29,cb,5c,df,c7),
        CCN224_C(ad,f1,8c,84,cf,77,ce,d4,d7,6d,49,30,41,7d,95,79,20,78,40,bf,49,bf,bf,58,37,df,dd,7d)
    },
    /* Scalar multiply R = dS */
    d[CCN224_N] = {
        CCN224_C(a7,8c,cc,30,ea,ca,0f,cc,8e,36,b2,dd,6f,bb,03,df,06,d3,7f,52,71,1e,63,63,aa,f1,d7,3b)
    },
    rmul[2 * CCN224_N] = {
        CCN224_C(96,a7,62,5e,92,a8,d7,2b,ff,11,13,ab,db,95,77,7e,73,6a,14,c6,fd,aa,cc,39,27,02,bc,a4),
        CCN224_C(0f,8e,57,02,94,2a,3c,5e,13,cd,2f,d5,80,19,15,25,8b,43,df,ad,c7,0d,15,db,ad,a3,ed,10)
    },
    /* Joint scalar multiply R = dS + eT (d as above) */
    e[CCN224_N] = {
        CCN224_C(54,d5,49,ff,c0,8c,96,59,25,19,d7,3e,71,e8,e0,70,3f,c8,17,7f,a8,8a,a7,7a,6e,d3,57,36)
    },
    rtmul[2 * CCN224_N] = {
        CCN224_C(db,fe,29,58,c7,b2,cd,a1,30,2a,67,ea,3f,fd,94,c9,18,c5,b3,50,ab,83,8d,52,e2,88,c8,3e),
        CCN224_C(2f,52,1b,83,ac,3b,05,49,ff,48,95,ab,cc,7f,0c,5a,86,1a,ac,b8,7a,cb,c5,b8,14,7b,b1,8b)
    };
    [self evaluateCurve: ccec_cp_224() : (const cc_unit *)sa : (const cc_unit *)ta : (const cc_unit *)radd : (const cc_unit *)rsub : (const cc_unit *)rdbl : (const cc_unit *)d : (const cc_unit *)rmul : (const cc_unit *)e : (const cc_unit *)rtmul : @"p224"];
}

- (void) testP256 {
    static const cc_unit sa[2 * CCN256_N] = {
        CCN256_C(de,24,44,be,bc,8d,36,e6,82,ed,d2,7e,0f,27,15,08,61,75,19,b3,22,1a,8f,a0,b7,7c,ab,39,89,da,97,c9),
        CCN256_C(c0,93,ae,7f,f3,6e,53,80,fc,01,a5,aa,d1,e6,66,59,70,2d,e8,0f,53,ce,c5,76,b6,35,0b,24,30,42,a2,56)
    },
    ta[2 * CCN256_N] = {
        CCN256_C(55,a8,b0,0f,8d,a1,d4,4e,62,f6,b3,b2,53,16,21,2e,39,54,0d,c8,61,c8,95,75,bb,8c,f9,2e,35,e0,98,6b),
        CCN256_C(54,21,c3,20,9c,2d,6c,70,48,35,d8,2a,c4,c3,dd,90,f6,1a,8a,52,59,8b,9e,7a,b6,56,e9,d8,c8,b2,43,16)
    },
    /* Full add radd = S + T */
    radd[2 * CCN256_N] = {
        CCN256_C(72,b1,3d,d4,35,4b,6b,81,74,51,95,e9,8c,c5,ba,69,70,34,91,91,ac,47,6b,d4,55,3c,f3,5a,54,5a,06,7e),
        CCN256_C(8d,58,5c,bb,2e,13,27,d7,52,41,a8,a1,22,d7,62,0d,c3,3b,13,31,5a,a5,c9,d4,6d,01,30,11,74,4a,c2,64)
    },
    /* Full subtract R = S − T */
    rsub[2 * CCN256_N] = {
        CCN256_C(c0,9c,e6,80,b2,51,bb,1d,2a,ad,1d,bf,61,29,de,ab,83,74,19,f8,f1,c7,3e,a1,3e,7d,c6,4a,d6,be,60,21),
        CCN256_C(1a,81,5b,f7,00,bd,88,33,6b,2f,9b,ad,4e,da,b1,72,34,14,a0,22,fd,f6,c3,f4,ce,30,67,5f,b1,97,5e,f3)
    },
    /* Double R = 2S */
    rdbl[2 * CCN256_N] = {
        CCN256_C(76,69,e6,90,16,06,ee,3b,a1,a8,ee,f1,e0,02,4c,33,df,6c,22,f3,b1,74,81,b8,2a,86,0f,fc,db,61,27,b0),
        CCN256_C(fa,87,81,62,18,7a,54,f6,c3,9f,6e,e0,07,2f,33,de,38,9e,f3,ee,cd,03,02,3d,e1,0c,a2,c1,db,61,d0,c7)
    },
    /* Scalar multiply R = dS */
    d[CCN256_N] = {
        CCN256_C(c5,1e,47,53,af,de,c1,e6,b6,c6,a5,b9,92,f4,3f,8d,d0,c7,a8,93,30,72,70,8b,65,22,46,8b,2f,fb,06,fd)
    },
    rmul[2 * CCN256_N] = {
        CCN256_C(51,d0,8d,5f,2d,42,78,88,29,46,d8,8d,83,c9,7d,11,e6,2b,ec,c3,cf,c1,8b,ed,ac,c8,9b,a3,4e,ec,a0,3f),
        CCN256_C(75,ee,68,eb,8b,f6,26,aa,5b,67,3a,b5,1f,6e,74,4e,06,f8,fc,f8,a6,c0,cf,30,35,be,ca,95,6a,7b,41,d5)
    },
    /* Joint scalar multiply R = dS + eT (d as above) */
    e[CCN256_N] = {
        CCN256_C(d3,7f,62,8e,ce,72,a4,62,f0,14,5c,be,fe,3f,0b,35,5e,e8,33,2d,37,ac,dd,83,a3,58,01,6a,ea,02,9d,b7)
    },
    rtmul[2 * CCN256_N] = {
        CCN256_C(d8,67,b4,67,92,21,00,92,34,93,92,21,b8,04,62,45,ef,cf,58,41,3d,aa,cb,ef,f8,57,b8,58,83,41,f6,b8),
        CCN256_C(f2,50,40,55,c0,3c,ed,e1,2d,22,72,0d,ad,69,c7,45,10,6b,66,07,ec,7e,50,dd,35,d5,4b,d8,0f,61,52,75)
    };
    [self evaluateCurve: ccec_cp_256() : (const cc_unit *)sa : (const cc_unit *)ta : (const cc_unit *)radd : (const cc_unit *)rsub : (const cc_unit *)rdbl : (const cc_unit *)d : (const cc_unit *)rmul : (const cc_unit *)e : (const cc_unit *)rtmul : @"p256"];
}

- (void) testP384 {
    static const cc_unit sa[2 * CCN384_N] = {
        CCN384_C(fb,a2,03,b8,1b,bd,23,f2,b3,be,97,1c,c2,39,97,e1,ae,4d,89,e6,9c,b6,f9,23,85,dd,a8,27,68,ad,a4,15,eb,ab,41,67,45,9d,a9,8e,62,b1,33,2d,1e,73,cb,0e),
        CCN384_C(5f,fe,db,ae,fd,eb,a6,03,e7,92,3e,06,cd,b5,d0,c6,5b,22,30,14,29,29,33,76,d5,c6,94,4e,3f,a6,25,9f,16,2b,47,88,de,69,87,fd,59,ae,d5,e4,b5,28,5e,45)
    },
    ta[2 * CCN384_N] = {
        CCN384_C(aa,cc,05,20,2e,7f,da,6f,c7,3d,82,f0,a6,62,20,52,7d,a8,11,7e,e8,f8,33,0e,ad,7d,20,ee,6f,25,5f,58,2d,8b,d3,8c,5a,7f,2b,40,bc,db,68,ba,13,d8,10,51),
        CCN384_C(84,00,9a,26,3f,ef,ba,7c,2c,57,cf,fa,5d,b3,63,4d,28,61,31,af,c0,fc,a8,d2,5a,fa,22,a7,b5,dc,e0,d9,47,0d,a8,92,33,ce,e1,78,59,2f,49,b6,fe,cb,50,92)
    },
    /* Full add radd = S + T */
    radd[2 * CCN384_N] = {
        CCN384_C(12,dc,5c,e7,ac,df,c5,84,4d,93,9f,40,b4,df,01,2e,68,f8,65,b8,9c,32,13,ba,97,09,0a,24,7a,2f,c0,09,07,5c,f4,71,cd,2e,85,c4,89,97,9b,65,ee,0b,5e,ed),
        CCN384_C(16,73,12,e5,8f,e0,c0,af,a2,48,f2,85,4e,3c,dd,cb,55,7f,98,3b,31,89,b6,7f,21,ee,e0,13,41,e7,e9,fe,67,f6,ee,81,b3,69,88,ef,a4,06,94,5c,88,04,a4,b0)
    },
    /* Full subtract R = S − T */
    rsub[2 * CCN384_N] = {
        CCN384_C(6a,fd,af,8d,a8,b1,1c,98,4c,f1,77,e5,51,ce,e5,42,cd,a4,ac,2f,25,cd,52,2d,0c,d7,10,f8,80,59,c6,56,5a,ef,78,f6,b5,ed,6c,c0,5a,66,66,de,f2,a2,fb,59),
        CCN384_C(7b,ed,0e,15,8a,e8,cc,70,e8,47,a6,03,47,ca,15,48,c3,48,de,cc,63,09,f4,8b,59,bd,5a,fc,9a,9b,80,4e,7f,78,76,17,8c,b5,a7,eb,4f,69,40,a9,c7,3e,8e,5e)
    },
    /* Double R = 2S */
    rdbl[2 * CCN384_N] = {
        CCN384_C(2a,21,11,b1,e0,aa,8b,2f,c5,a1,97,55,16,bc,4d,58,01,7f,f9,6b,25,e1,bd,ff,3c,22,9d,5f,ac,3b,ac,c3,19,dc,be,c2,9f,94,78,f4,2d,ee,59,7b,46,41,50,4c),
        CCN384_C(fa,2e,3d,9d,c8,4d,b8,95,4c,e8,08,5e,f2,8d,71,84,fd,df,d1,34,4b,4d,47,97,34,3a,f9,b5,f9,d8,37,52,0b,45,0f,72,64,43,e4,11,4b,d4,e5,bd,b2,f6,5d,dd)
    },
    /* Scalar multiply R = dS */
    d[CCN384_N] = {
        CCN384_C(a4,eb,ca,e5,a6,65,98,34,93,ab,3e,62,60,85,a2,4c,10,43,11,a7,61,b5,a8,fd,ac,05,2e,d1,f1,11,a5,c4,4f,76,f4,56,59,d2,d1,11,a6,1b,5f,dd,97,58,34,80)
    },
    rmul[2 * CCN384_N] = {
        CCN384_C(e4,f7,7e,7f,fe,b7,f0,95,89,10,e3,a6,80,d6,77,a4,77,19,1d,f1,66,16,0f,f7,ef,6b,b5,26,1f,79,1a,a7,b4,5e,3e,65,3d,15,1b,95,da,d3,d9,3c,a0,29,0e,f2),
        CCN384_C(ac,7d,ee,41,d8,c5,f4,a7,d5,83,69,60,a7,73,cf,c1,37,62,89,d3,37,3f,8c,f7,41,7b,0c,62,07,ac,32,e9,13,85,66,12,fc,9f,f2,e3,57,eb,2e,e0,5c,f9,66,7f)
    },
    /* Joint scalar multiply R = dS + eT (d as above) */
    e[CCN384_N] = {
        CCN384_C(af,cf,88,11,9a,3a,76,c8,7a,cb,d6,00,8e,13,49,b2,9f,4b,a9,aa,0e,12,ce,89,bc,fc,ae,21,80,b3,8d,81,ab,8c,f1,50,95,30,1a,18,2a,fb,c6,89,3e,75,38,5d)
    },
    rtmul[2 * CCN384_N] = {
        CCN384_C(91,7e,a2,8b,cd,64,17,41,ae,5d,18,c2,f1,bd,91,7b,a6,8d,34,f0,f0,57,73,87,dc,81,26,04,62,ae,a6,0e,24,17,b8,bd,c5,d9,54,fc,72,9d,21,1d,b2,3a,02,dc),
        CCN384_C(1a,29,f7,ce,6d,07,46,54,d7,7b,40,88,8c,73,e9,25,46,c8,f1,6a,5f,f6,bc,bd,30,7f,75,8d,4a,ee,68,4b,ef,f2,6f,67,42,f5,97,e2,58,5c,86,da,90,8f,71,86)
    };
    [self evaluateCurve: ccec_cp_384() : (const cc_unit *)sa : (const cc_unit *)ta : (const cc_unit *)radd : (const cc_unit *)rsub : (const cc_unit *)rdbl : (const cc_unit *)d : (const cc_unit *)rmul : (const cc_unit *)e : (const cc_unit *)rtmul : @"p384"];
}

- (void) testP521 {
    static const cc_unit sa[2 * CCN521_N] = {
        CCN528_C(01,d5,c6,93,f6,6c,08,ed,03,ad,0f,03,1f,93,74,43,45,8f,60,1f,d0,98,d3,d0,22,7b,4b,f6,28,73,af,50,74,0b,0b,b8,4a,a1,57,fc,84,7b,cf,8d,c1,6a,8b,2b,8b,fd,8e,2d,0a,7d,39,af,04,b0,89,93,0e,f6,da,d5,c1,b4),
        CCN528_C(01,44,b7,77,09,63,c6,3a,39,24,88,65,ff,36,b0,74,15,1e,ac,33,54,9b,22,4a,f5,c8,66,4c,54,01,2b,81,8e,d0,37,b2,b7,c1,a6,3a,c8,9e,ba,a1,1e,07,db,89,fc,ee,5b,55,6e,49,76,4e,e3,fa,66,ea,7a,e6,1a,c0,18,23)
    },
    ta[2 * CCN521_N] = {
        CCN528_C(00,f4,11,f2,ac,2e,b9,71,a2,67,b8,02,97,ba,67,c3,22,db,a4,bb,21,ce,c8,b7,00,73,bf,88,fc,1c,a5,fd,e3,ba,09,e5,df,6d,39,ac,b2,c0,76,2c,03,d7,bc,22,4a,3e,19,7f,ea,f7,60,d6,32,40,06,fe,3b,e9,a5,48,c7,d5),
        CCN528_C(01,fd,f8,42,76,9c,70,7c,93,c6,30,df,6d,02,ef,f3,99,a0,6f,1b,36,fb,96,84,f0,b3,73,ed,06,48,89,62,9a,bb,92,b1,ae,32,8f,db,45,53,42,68,38,49,43,f0,e9,22,2a,fe,03,25,9b,32,27,4d,35,d1,b9,58,4c,65,e3,05)
    },
    /* Full add radd = S + T */
    radd[2 * CCN521_N] = {
        CCN528_C(01,26,4a,e1,15,ba,9c,bc,2e,e5,6e,6f,00,59,e2,4b,52,c8,04,63,21,60,2c,59,a3,39,cf,b7,57,c8,9a,59,c3,58,a9,a8,e1,f8,6d,38,4b,3f,3b,25,5e,a3,f7,36,70,c6,dc,9f,45,d4,6b,6a,19,6d,c3,7b,be,0f,6b,2d,d9,e9),
        CCN528_C(00,62,a9,c7,2b,8f,9f,88,a2,71,69,0b,fa,01,7a,64,66,c3,1b,9c,ad,c2,fc,54,47,44,ae,b8,17,07,23,49,cf,dd,c5,ad,0e,81,b0,3f,18,97,bd,9c,8c,6e,fb,df,68,23,7d,c3,bb,00,44,59,79,fb,37,3b,20,c9,a9,67,ac,55)
    },
    /* Full subtract R = S − T */
    rsub[2 * CCN521_N] = {
        CCN528_C(01,29,2c,b5,8b,17,95,ba,47,70,63,fe,f7,cd,22,e4,2c,20,f5,7a,e9,4c,ea,ad,86,e0,d2,1f,f2,29,18,b0,dd,3b,07,6d,63,be,25,3d,e2,4b,c2,0c,6d,a2,90,fa,54,d8,37,71,a2,25,de,ec,f9,14,9f,79,a8,e6,14,c3,c4,cd),
        CCN528_C(01,69,5e,38,21,e7,2c,7c,ac,aa,dc,f6,29,09,cd,83,46,3a,21,c6,d0,33,93,c5,27,c6,43,b3,62,39,c4,6a,f1,17,ab,7c,7a,d1,9a,4c,8c,f0,ae,95,ed,51,72,98,85,46,1a,a2,ce,27,00,a6,36,5b,ca,37,33,d2,92,0b,22,67)
    },
    /* Double R = 2S */
    rdbl[2 * CCN521_N] = {
        CCN528_C(01,28,79,44,2f,24,50,c1,19,e7,11,9a,5f,73,8b,e1,f1,eb,a9,e9,d7,c6,cf,41,b3,25,d9,ce,6d,64,31,06,e9,d6,11,24,a9,1a,96,bc,f2,01,30,5a,9d,ee,55,fa,79,13,6d,c7,00,83,1e,54,c3,ca,4f,f2,64,6b,d3,c3,6b,c6),
        CCN528_C(01,98,64,a8,b8,85,5c,24,79,cb,ef,e3,75,ae,55,3e,23,93,27,1e,d3,6f,ad,fc,44,94,fc,05,83,f6,bd,03,59,88,96,f3,98,54,ab,ea,e5,f9,a6,51,5a,02,1e,2c,0e,ef,13,9e,71,de,61,01,43,f5,33,82,f4,10,4d,cc,b5,43)
    },
    /* Scalar multiply R = dS */
    d[CCN521_N] = {
        CCN528_C(01,eb,7f,81,78,5c,96,29,f1,36,a7,e8,f8,c6,74,95,71,09,73,55,54,11,1a,2a,86,6f,a5,a1,66,69,94,19,bf,a9,93,6c,78,b6,26,53,96,4d,f0,d6,da,94,0a,69,5c,72,94,d4,1b,2d,66,00,de,6d,fc,f0,ed,cf,c8,9f,dc,b1)
    },
    rmul[2 * CCN521_N] = {
        CCN528_C(00,91,b1,5d,09,d0,ca,03,53,f8,f9,6b,93,cd,b1,34,97,b0,a4,bb,58,2a,e9,eb,ef,a3,5e,ee,61,bf,7b,7d,04,1b,8e,c3,4c,6c,00,c0,c0,67,1c,4a,e0,63,31,8f,b7,5b,e8,7a,f4,fe,85,96,08,c9,5f,0a,b4,77,4f,8c,95,bb),
        CCN528_C(01,30,f8,f8,b5,e1,ab,b4,dd,94,f6,ba,af,65,4a,2d,58,10,41,1e,77,b7,42,39,65,e0,c7,fd,79,ec,1a,e5,63,c2,07,bd,25,5e,e9,82,8e,b7,a0,3f,ed,56,52,40,d2,cc,80,dd,d2,ce,cb,b2,eb,50,f0,95,1f,75,ad,87,97,7f)
    },
    /* Joint scalar multiply R = dS + eT (d as above) */
    e[CCN521_N] = {
        CCN528_C(01,37,e6,b7,3d,38,f1,53,c3,a7,57,56,15,81,26,08,f2,ba,b3,22,9c,92,e2,1c,0d,1c,83,cf,ad,92,61,db,b1,7b,b7,7a,63,68,20,00,03,1b,91,22,c2,f0,cd,ab,2a,f7,23,14,be,95,25,4d,e4,29,1a,8f,85,f7,c7,04,12,e3)
    },
    rtmul[2 * CCN521_N] = {
        CCN528_C(00,9d,38,02,64,2b,3b,ea,15,2b,eb,9e,05,fb,a2,47,79,0f,7f,c1,68,07,2d,36,33,40,13,34,02,f2,58,55,88,dc,13,85,d4,0e,bc,b8,55,2f,8d,b0,2b,23,d6,87,ca,e4,61,85,b2,75,28,ad,b1,bf,97,29,71,6e,4e,ba,65,3d),
        CCN528_C(00,0f,e4,43,44,e7,9d,a6,f4,9d,87,c1,06,37,44,e5,95,7d,9a,c0,a5,05,ba,fa,82,81,c9,ce,9f,f2,5a,d5,3f,8d,a0,84,a2,de,b0,92,3e,46,50,1d,e5,79,78,50,c6,1b,22,90,23,dd,9c,f7,fc,7f,04,cd,35,eb,b0,26,d8,9d)
    };
    [self evaluateCurve: ccec_cp_521() : (const cc_unit *)sa : (const cc_unit *)ta : (const cc_unit *)radd : (const cc_unit *)rsub : (const cc_unit *)rdbl : (const cc_unit *)d : (const cc_unit *)rmul : (const cc_unit *)e : (const cc_unit *)rtmul : @"p521"];
}

@end
