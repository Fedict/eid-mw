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


#define CAVP_VERBOSE  0

#define LTC_EC_VERIFY  0

#if LTC_EC_VERIFY
#define LTC_SOURCE  1
#define LTM_DESC  1
#include <tomcrypt/tomcrypt.h>
#endif

#import "CAVP.h"
#import <corecrypto/ccec_priv.h>
#import <corecrypto/ccasn1.h>
#import <corecrypto/ccsha1.h>
#import <Foundation/Foundation.h>

@implementation CAVP

static uint8_t hex_nibble(uint8_t hex) {
    if ('0' <= hex && hex <= '9')
        return hex - '0';
    else if ('a' <= hex && hex <= 'f')
        return hex + 10 - 'a';
    else if ('A' <= hex && hex <= 'F')
        return hex + 10 - 'A';
    else {
        NSLog(@"invalid hex digit: %d[%c]\n", hex, hex);
        return 0;
    }
}

static NSData *hex2binary(NSString *hex) {
    NSData *hex_d = [hex dataUsingEncoding: NSASCIIStringEncoding];
    NSUInteger hex_l = [hex_d length];
    NSMutableData *binary = [NSMutableData dataWithLength: (hex_l + 1) / 2];

    const uint8_t *s = (const uint8_t *)[hex_d bytes];
    uint8_t *d = (uint8_t *)[binary mutableBytes];
    const uint8_t *p = s + hex_l;
    d += (hex_l + 1) / 2;
    while (p > s + 1) {
        *--d = hex_nibble(*--p);
        *d |= (uint8_t)(hex_nibble(*--p) << 4);
    }
    if (p > s)
        *--d = hex_nibble(*--p);

    return binary;
}

static void cc_read_string(cc_size count, cc_unit *r, NSString *s) {
    NSData *data = hex2binary(s);
    ccn_read_uint(count, r, [data length], [data bytes]);
}

#if CAVP_VERBOSE
static void hexDumpData(const char *label, size_t len, const uint8_t *s) {
    size_t ix;
    printf("%s", label);
    for (ix = 0; ix < len; ++ix) {
        printf("%.02x", s[ix]);
    }
    printf("\n");
}
#endif

- (void) sigVer: (ccec_const_cp_t)cp msg: (NSString *)msg_s
             Qx: (NSString *)qx_s Qy: (NSString *)qy_s r: (NSString *)r_s
              s: (NSString *)s_s result: (NSString *)result_s {
    //NSLog(@"%d: Msg = %@\nQx = %@\nQy = %@\nR = %@\nS = %@\n", ccec_cp_n(cp), msg_s, qx_s, qy_s, r_s, s_s);
    cc_unit r[ccec_cp_n(cp)], s[ccec_cp_n(cp)];
    uint8_t e[ccec_ccn_size(cp)];
    ccec_pub_ctx_decl_cp(cp, q);
    ccec_ctx_init(cp, q);
    NSData *msg_d = hex2binary(msg_s);
    cc_read_string(ccec_cp_n(cp), ccec_ctx_x(q), qx_s);
    cc_read_string(ccec_cp_n(cp), ccec_ctx_y(q), qy_s);
    ccn_seti(ccec_cp_n(cp), ccec_ctx_z(q), 1);
    cc_read_string(ccec_cp_n(cp), r, r_s);
    cc_read_string(ccec_cp_n(cp), s, s_s);

    ccn_zero(ccec_cp_n(cp), (cc_unit *)e);
    ccdigest(ccsha1_di(), [msg_d length], [msg_d bytes], e);

#if CAVP_VERBOSE
    hexDumpData("Msg = ", (size_t)[msg_d length], (const uint8_t *)[msg_d bytes]);
    ccn_lprint(ccec_cp_n(cp), "Qx = ", ccec_ctx_x(q));
    ccn_lprint(ccec_cp_n(cp), "Qy = ", ccec_ctx_y(q));
    ccn_lprint(ccec_cp_n(cp), "R = ", r);
    ccn_lprint(ccec_cp_n(cp), "S = ", s);
    hexDumpData("E = ", 20, e);
#endif

    /* Encode resulting r, s into SEQUENCE { r, s -- integer } */
    uint8_t tl, rl, sl;
    rl = (uint8_t)ccn_write_uint_size(ccec_cp_n(cp), r);
    sl = (uint8_t)ccn_write_uint_size(ccec_cp_n(cp), s);
    tl = rl + sl + 4;
    size_t sig_len = tl + 2;
    uint8_t sig[sig_len];
    sig[0] = CCASN1_CONSTRUCTED_SEQUENCE;
    sig[1] = tl;
    sig[2] = CCASN1_INTEGER;
    sig[3] = rl;
    size_t len = rl;
    ccn_write_uint(ccec_cp_n(cp), r, rl, sig + 4);
    sig[4 + rl] = CCASN1_INTEGER;
    sig[5 + rl] = sl;
    len = sl;
    ccn_write_uint(ccec_cp_n(cp), s, sl, sig + rl + 6);

	//CFAbsoluteTime t0, t1, t2;
	CFAbsoluteTime t1, t2;
#if LTC_EC_VERIFY
    /* determine the idx */
    int x;
    for (x = 0; ltc_ecc_sets[x].size != 0; x++) {
        if ((unsigned)ltc_ecc_sets[x].size >= rl) {
            break;
        }
    }
    if (ltc_ecc_sets[x].size == 0) {
        printf("error curve  of size %d not found", rl);
        return;
    }

    ecc_key ltc_key = { .type = PK_PUBLIC, .idx = x, .dp = &ltc_ecc_sets[x] };
    cc_init_multi(&ltc_key.pubkey.x, &ltc_key.pubkey.y, &ltc_key.pubkey.z, &ltc_key.k, NULL);
    NSData *data = hex2binary(qx_s);
    cc_read_unsigned_bin(ltc_key.pubkey.x, (unsigned char *)[data bytes], [data length]);
    data = hex2binary(qy_s);
    cc_read_unsigned_bin(ltc_key.pubkey.y, (unsigned char *)[data bytes], [data length]);
    cc_set(ltc_key.pubkey.z, 1);

    int stat = 0;
	t0 = CFAbsoluteTimeGetCurrent();
    int res = ecc_verify_hash(sig,  sig_len, e, 20, &stat, &ltc_key);
	t1 = CFAbsoluteTimeGetCurrent();
    t0 = t1 - t0;

    ecc_free(&ltc_key);

    //printf("res: %d %s\n", res, (stat ? "P" : "F"));
#endif
    bool valid;
	t1 = CFAbsoluteTimeGetCurrent();
    ccec_verify(q, 20, e, sig_len, sig, &valid);
	t2 = CFAbsoluteTimeGetCurrent();
    t1 = t2 - t1;
    XCTAssertEqualObjects(result_s, (valid ? @"P" : @"F"), @"ccec_verify ok");
    //printf("verify libtom: %fms nistecc: %fms\n", t0 * 1000.0, t1 * 1000.0);
}

- (void) testSigVer {
#if LTC_EC_VERIFY
    set_ltc_mp(&ltm_desc);
#endif

    NSBundle *bundle = [NSBundle bundleWithIdentifier:@"com.apple.validation"];
    NSString *path = [bundle pathForResource: @"SigVer.rsp" ofType: @"correct"];
    NSString *sigVer = [NSString stringWithContentsOfFile: path
                                                 encoding: NSASCIIStringEncoding
                                                    error: NULL];

    __block ccec_const_cp_t cp;
    __block NSString *msg = NULL, *Qx = NULL, *Qy = NULL, *r = NULL, *s = NULL,
    *result = NULL;
    cp.zp = NULL;
    __block int line_num = 0;
    [sigVer enumerateLinesUsingBlock: ^(NSString *line, BOOL *stop) {
        line_num++;
        if ([line hasPrefix: @"#"])
            return;
        if ([line hasPrefix: @"["]) {
            if ([line isEqualToString: @"[P-192]"]) {
                cp = ccec_cp_192();
            } else if ([line isEqualToString: @"[P-224]"]) {
                cp = ccec_cp_224();
            } else if ([line isEqualToString: @"[P-256]"]) {
                cp = ccec_cp_256();
            } else if ([line isEqualToString: @"[P-384]"]) {
                cp = ccec_cp_384();
            } else if ([line isEqualToString: @"[P-521]"]) {
                cp = ccec_cp_521();
            } else {
                NSLog(@"Skipping curve %@", line);
                cp.zp = NULL;
            }
        } else if ([line hasPrefix: @"Msg ="]) {
            msg = [line substringFromIndex: 5];
        } else if ([line hasPrefix: @"Qx = "]) {
            Qx = [line substringFromIndex: 5];
        } else if ([line hasPrefix: @"Qy = "]) {
            Qy = [line substringFromIndex: 5];
        } else if ([line hasPrefix: @"R = "]) {
            r = [line substringFromIndex: 4];
        } else if ([line hasPrefix: @"S = "]) {
            s = [line substringFromIndex: 4];
        } else if ([line hasPrefix: @"Result = "]) {
            result = [line substringFromIndex: 9];
            if (cp.zp) {
                [self sigVer: cp msg: msg Qx: Qx Qy: Qy r: r s: s result: result];
            } else {
                //NSLog(@"Skipping test for unsupported curve on line %d", line_num);
            }
        } else {
            if (msg) {
                msg = [msg stringByAppendingString: line];
            } else {
                NSLog(@"error garbage on line %d: %@", line_num, line);
            }
        }

        *stop = NO;
    }];
}

@end
