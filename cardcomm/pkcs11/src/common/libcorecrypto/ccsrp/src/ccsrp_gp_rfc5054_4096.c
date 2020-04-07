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


/* Autogenerated file - Use scheme ccdh_gen_gp */
#include <corecrypto/ccdh_priv.h>
#include <corecrypto/ccsrp_gp.h>

static const ccdh_gp_decl_static(4096) _ccsrp_gp_rfc5054_4096 =
{
    .zp = {
        .n = ccn_nof(4096),
        .options = 0,
        .mod_prime = cczp_mod
    },
    .p = {
        /* prime */
        CCN64_C(ff,ff,ff,ff,ff,ff,ff,ff),CCN64_C(4d,f4,35,c9,34,06,31,99),
        CCN64_C(86,ff,b7,dc,90,a6,c0,8f),CCN64_C(93,b4,ea,98,8d,8f,dd,c1),
        CCN64_C(d0,06,91,27,d5,b0,5a,a9),CCN64_C(b8,1b,dd,76,21,70,48,1c),
        CCN64_C(1f,61,29,70,ce,e2,d7,af),CCN64_C(23,3b,a1,86,51,5b,e7,ed),
        CCN64_C(99,b2,96,4f,a0,90,c3,a2),CCN64_C(28,7c,59,47,4e,6b,c0,5d),
        CCN64_C(2e,8e,fc,14,1f,be,ca,a6),CCN64_C(db,bb,c2,db,04,de,8e,f9),
        CCN64_C(25,83,e9,ca,2a,d4,4c,e8),CCN64_C(1a,94,68,34,b6,15,0b,da),
        CCN64_C(99,c3,27,18,6a,f4,e2,3c),CCN64_C(88,71,9a,10,bd,ba,5b,26),
        CCN64_C(1a,72,3c,12,a7,87,e6,d7),CCN64_C(4b,82,d1,20,a9,21,08,01),
        CCN64_C(43,db,5b,fc,e0,fd,10,8e),CCN64_C(08,e2,4f,a0,74,e5,ab,31),
        CCN64_C(77,09,88,c0,ba,d9,46,e2),CCN64_C(bb,e1,17,57,7a,61,5d,6c),
        CCN64_C(52,1f,2b,18,17,7b,20,0c),CCN64_C(d8,76,02,73,3e,c8,6a,64),
        CCN64_C(f1,2f,fa,06,d9,8a,08,64),CCN64_C(ce,e3,d2,26,1a,d2,ee,6b),
        CCN64_C(1e,8c,94,e0,4a,25,61,9d),CCN64_C(ab,f5,ae,8c,db,09,33,d7),
        CCN64_C(b3,97,0f,85,a6,e1,e4,c7),CCN64_C(8a,ea,71,57,5d,06,0c,7d),
        CCN64_C(ec,fb,85,04,58,db,ef,0a),CCN64_C(a8,55,21,ab,df,1c,ba,64),
        CCN64_C(ad,33,17,0d,04,50,7a,33),CCN64_C(15,72,8e,5a,8a,aa,c4,2d),
        CCN64_C(15,d2,26,18,98,fa,05,10),CCN64_C(39,95,49,7c,ea,95,6a,e5),
        CCN64_C(de,2b,cb,f6,95,58,17,18),CCN64_C(b5,c5,5d,f0,6f,4c,52,c9),
        CCN64_C(9b,27,83,a2,ec,07,a2,8f),CCN64_C(e3,9e,77,2c,18,0e,86,03),
        CCN64_C(32,90,5e,46,2e,36,ce,3b),CCN64_C(f1,74,6c,08,ca,18,21,7c),
        CCN64_C(67,0c,35,4e,4a,bc,98,04),CCN64_C(9e,d5,29,07,70,96,96,6d),
        CCN64_C(1c,62,f3,56,20,85,52,bb),CCN64_C(83,65,5d,23,dc,a3,ad,96),
        CCN64_C(69,16,3f,a8,fd,24,cf,5f),CCN64_C(98,da,48,36,1c,55,d3,9a),
        CCN64_C(c2,00,7c,b8,a1,63,bf,05),CCN64_C(49,28,66,51,ec,e4,5b,3d),
        CCN64_C(ae,9f,24,11,7c,4b,1f,e6),CCN64_C(ee,38,6b,fb,5a,89,9f,a5),
        CCN64_C(0b,ff,5c,b6,f4,06,b7,ed),CCN64_C(f4,4c,42,e9,a6,37,ed,6b),
        CCN64_C(e4,85,b5,76,62,5e,7e,c6),CCN64_C(4f,e1,35,6d,6d,51,c2,45),
        CCN64_C(30,2b,0a,6d,f2,5f,14,37),CCN64_C(ef,95,19,b3,cd,3a,43,1b),
        CCN64_C(51,4a,08,79,8e,34,04,dd),CCN64_C(02,0b,be,a6,3b,13,9b,22),
        CCN64_C(29,02,4e,08,8a,67,cc,74),CCN64_C(c4,c6,62,8b,80,dc,1c,d1),
        CCN64_C(c9,0f,da,a2,21,68,c2,34),CCN64_C(ff,ff,ff,ff,ff,ff,ff,ff)
    },
    .recip = {
        /* recip */
        CCN64_C(c1,4a,b0,dd,cc,03,aa,20),CCN64_C(07,20,53,62,9d,93,12,52),
        CCN64_C(8e,27,da,72,bf,17,72,12),CCN64_C(59,1a,07,21,ad,fe,a4,21),
        CCN64_C(07,f6,d0,9e,26,90,60,e5),CCN64_C(ae,1c,f6,70,c7,e2,8d,82),
        CCN64_C(97,25,ef,ec,54,ae,db,55),CCN64_C(8e,1c,d7,1e,a5,18,f6,6b),
        CCN64_C(ef,bd,df,3f,30,60,e3,54),CCN64_C(ed,fd,73,74,a1,d9,ca,9a),
        CCN64_C(5d,46,80,fe,70,fc,2a,3f),CCN64_C(18,b7,f0,a5,56,4a,16,16),
        CCN64_C(e5,86,ab,ca,ef,7f,a1,2e),CCN64_C(39,9a,4e,cb,4b,05,f3,6f),
        CCN64_C(ed,e8,43,f9,0f,5d,a8,34),CCN64_C(c1,45,e0,9d,ae,0a,5c,7e),
        CCN64_C(0c,25,8e,7e,91,55,6c,e5),CCN64_C(52,94,f5,e1,27,28,13,91),
        CCN64_C(f3,45,a3,15,c4,76,87,65),CCN64_C(67,ed,59,09,17,2f,b4,d7),
        CCN64_C(29,96,4a,fa,53,e3,c1,b9),CCN64_C(74,6d,18,ce,ed,b5,65,ff),
        CCN64_C(76,39,00,49,8b,0f,03,08),CCN64_C(1e,c9,7a,0b,25,20,1c,17),
        CCN64_C(49,8f,de,c9,d5,4b,d0,71),CCN64_C(de,c1,4c,c9,55,69,81,1e),
        CCN64_C(6b,78,e8,20,43,04,17,16),CCN64_C(6d,ab,25,69,61,1b,ad,db),
        CCN64_C(ee,4c,d8,60,ee,0b,64,50),CCN64_C(2e,b5,90,41,e1,44,78,3a),
        CCN64_C(ba,72,c5,9b,f5,cc,f3,26),CCN64_C(a8,99,c5,99,99,bf,87,7d),
        CCN64_C(9a,3e,0b,c1,0e,10,0c,e0),CCN64_C(90,6e,ef,7d,26,ca,f0,52),
        CCN64_C(e6,24,91,05,f5,b1,95,fe),CCN64_C(14,b6,36,e6,0f,eb,c2,92),
        CCN64_C(8b,b4,93,16,c1,06,e4,e0),CCN64_C(9d,5d,89,d7,de,4a,75,c8),
        CCN64_C(40,e4,06,9d,55,6e,9d,d0),CCN64_C(ea,c9,be,0b,31,eb,31,85),
        CCN64_C(be,85,74,37,04,94,a3,54),CCN64_C(bf,23,31,e9,c9,4d,e9,1f),
        CCN64_C(2b,ce,3e,51,90,b8,91,ab),CCN64_C(11,15,f0,24,a6,e9,76,bd),
        CCN64_C(f5,e4,f0,7a,b8,b2,86,e4),CCN64_C(e7,50,2d,2f,5f,6a,7b,65),
        CCN64_C(6f,c7,fa,a8,b2,bd,ca,9b),CCN64_C(a8,8d,0d,2f,78,a7,7a,8a),
        CCN64_C(c4,b8,73,9c,be,a0,38,aa),CCN64_C(19,fc,25,8d,79,bc,21,7a),
        CCN64_C(8e,db,2d,e1,89,93,41,37),CCN64_C(7a,a5,cc,40,e0,20,35,58),
        CCN64_C(86,c5,81,76,47,b0,88,d1),CCN64_C(5c,0e,13,d1,68,04,9b,bc),
        CCN64_C(54,75,db,33,db,7b,83,bb),CCN64_C(c8,c9,d3,d9,22,ee,ce,9a),
        CCN64_C(d4,80,2f,b8,d3,29,55,0d),CCN64_C(96,89,fc,09,03,a8,01,e3),
        CCN64_C(b1,f9,fb,b5,bf,16,fb,e7),CCN64_C(64,cf,ca,c5,f1,87,2e,51),
        CCN64_C(a6,db,0f,58,84,48,b6,11),CCN64_C(47,03,ce,7e,2e,81,51,97),
        CCN64_C(36,f0,25,5d,de,97,3d,cb),CCN64_C(00,00,00,00,00,00,00,00),
        CCN8_C(01)
    },
    .g = {
        /* g */
        CCN64_C(00,00,00,00,00,00,00,05),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN8_C(00)
    },
    .q = {
        /* q */
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN64_C(00,00,00,00,00,00,00,00),
        CCN64_C(00,00,00,00,00,00,00,00),CCN8_C(00)
    },
    .l = 384,
};

ccdh_const_gp_t ccsrp_gp_rfc5054_4096(void)
{
    return (ccdh_const_gp_t)(cczp_const_t)(const cc_unit *)&_ccsrp_gp_rfc5054_4096;
}

