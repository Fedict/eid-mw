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


#define USE_SUPER_COOL_NEW_CCOID_T
#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

#if (CCSRP == 0)
entryPoint(ccsrp_test,"ccdigest test")
#else


#include <corecrypto/ccsrp.h>
#include <corecrypto/ccsrp_gp.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>

/*
 Appendix B.  SRP Test Vectors
 
 The following test vectors demonstrate calculation of the verifier
 and premaster secret.
 */
static int srp_test_vector() {
    const struct ccdigest_info *di = ccsha1_di();
    ccsrp_const_gp_t gp = ccsrp_gp_rfc5054_1024();
    const char *I = "alice";
    const char *P = "password123";
    byteBuffer salt = hexStringToBytes("BEB25379D1A8581EB5A727673A2441EE");
    byteBuffer k = hexStringToBytes("7556AA045AEF2CDD07ABAF0F665C3E818913186F");
    byteBuffer x = hexStringToBytes("94B7555AABE9127CC58CCF4993DB6CF84D16C124");
    byteBuffer v =    hexStringToBytes("7E273DE8696FFC4F4E337D05B4B375BEB0DDE1569E8FA00A9886D812"\
    "9BADA1F1822223CA1A605B530E379BA4729FDC59F105B4787E5186F5"\
    "C671085A1447B52A48CF1970B4FB6F8400BBF4CEBFBB168152E08AB5"\
    "EA53D15C1AFF87B2B9DA6E04E058AD51CC72BFC9033B564E26480D78"\
    "E955A5E29E7AB245DB2BE315E2099AFB");
    byteBuffer a = hexStringToBytes("60975527035CF2AD1989806F0407210BC81EDC04E2762A56AFD529DDDA2D4393");
    byteBuffer b = hexStringToBytes("E487CB59D31AC550471E81F00F6928E01DDA08E974A004F49E61F5D105284D20");
    byteBuffer A = hexStringToBytes("61D5E490F6F1B79547B0704C436F523DD0E560F0C64115BB72557EC4"\
    "4352E8903211C04692272D8B2D1A5358A2CF1B6E0BFCF99F921530EC"\
    "8E39356179EAE45E42BA92AEACED825171E1E8B9AF6D9C03E1327F44"\
    "BE087EF06530E69F66615261EEF54073CA11CF5858F0EDFDFE15EFEA"\
    "B349EF5D76988A3672FAC47B0769447B");
    byteBuffer B = hexStringToBytes("BD0C61512C692C0CB6D041FA01BB152D4916A1E77AF46AE105393011"\
    "BAF38964DC46A0670DD125B95A981652236F99D9B681CBF87837EC99"\
    "6C6DA04453728610D0C6DDB58B318885D7D82C7F8DEB75CE7BD4FBAA"\
    "37089E6F9C6059F388838E7A00030B331EB76840910440B1B27AAEAE"\
    "EB4012B7D7665238A8E3FB004B117B58");
    byteBuffer u = hexStringToBytes("CE38B9593487DA98554ED47D70A7AE5F462EF019");
    byteBuffer S =  hexStringToBytes("B0DC82BABCF30674AE450C0287745E7990A3381F63B387AAF271A10D"\
    "233861E359B48220F7C4693C9AE12B0A6F67809F0876E2D013800D6C"\
    "41BB59B6D5979B5C00A172B4A2A5903A0BDCAF8A709585EB2AFAFA8F"\
    "3499B200210DCC1F10EB33943CD67FC88A2F39A4BE5BEC4EC0A3212D"\
    "C346D7E474B29EDE8A469FFECA686E5A");
    return ccsrp_test_calculations(di, gp, I, strlen(P), P,
                            salt->len, salt->bytes,
                            k->len, k->bytes,
                            x->len, x->bytes,
                            v->len, v->bytes,
                            a->len, a->bytes,
                            b->len, b->bytes,
                            A->len, A->bytes,
                            B->len, B->bytes,
                            u->len, u->bytes,
                            S->len, S->bytes);

}


static int verbose = 0;


#define NITER          100
#define TEST_HASH      SRP_SHA1
#define TEST_NG        SRP_NG_1024


static int test_srp(const struct ccdigest_info *di, ccsrp_const_gp_t gp, struct ccrng_state *rng) {
    ccsrp_ctx_decl(di, gp, client_srp);
    ccsrp_ctx_decl(di, gp, server_srp);
    ccsrp_ctx_init(client_srp, di, gp);
    ccsrp_ctx_init(server_srp, di, gp);
    size_t pki_size = ccsrp_ctx_sizeof_n(client_srp);
    size_t digest_size = ccsrp_ctx_keysize(client_srp);
    const char *username = "testuser";
    const char *password = "password";
    uint8_t salt[64];
    uint8_t verifier[pki_size];
    uint8_t A[pki_size];
    uint8_t B[pki_size];
    uint8_t M[digest_size];
    uint8_t bytes_HAMK[digest_size];
    
    size_t salt_len, password_len;
    
    salt_len = 64;
        
    password_len = strlen(password);
    
    if(verbose) diag("test_srp.0\n");
    
    ok_or_fail(ccsrp_generate_salt_and_verification(client_srp, rng, username, password_len, password, salt_len, salt, verifier) == 0, "Generate Salt and Verifier");
    
    // Generate a and A
    if(verbose) diag("test_srp.2\n");
    ok_or_fail(ccsrp_client_start_authentication(client_srp, rng, A) == 0, "Start client authentication");
    
    // Client sends A to server
    
    // Generate b and B using A
    if(verbose) diag("test_srp.3\n");
    ok_or_fail(ccsrp_server_start_authentication(server_srp, rng, username, salt_len, salt, verifier, A, B) == 0,
               "Verifier SRP-6a safety check" );
    
    // Client uses s and B to generate M to answer challenge
    if(verbose) diag("test_srp.4\n");
    ok_or_fail(ccsrp_client_process_challenge(client_srp, username, password_len, password, salt_len, salt, B, M) == 0,
               "User SRP-6a safety check" );
    
    void *ck = ccsrp_ctx_K(client_srp);
    void *sk = ccsrp_ctx_K(server_srp);
    ok(memcmp(ck, sk, digest_size) == 0, "Session Keys don't match");
    
    // Verify M was generated correctly - generate HAMK
    if(verbose) diag("test_srp.5\n");
    ok_or_fail(ccsrp_server_verify_session(server_srp, M, bytes_HAMK),
               "User authentication");
     
    // Client verifies correct HAMK
    if(verbose) diag("test_srp.6\n");
    ok_or_fail(ccsrp_client_verify_session(client_srp, bytes_HAMK ), "Server Authentication");
    
    if(verbose) diag("test_srp.7\n");
    ok(ccsrp_is_authenticated(client_srp), "Server Authentication");
    
    return 1;
}



int ccsrp_test(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    struct ccrng_state *rng = global_test_rng;
	plan_tests(226);

    diag("SRP KAT Test");
    ok(srp_test_vector() == 0, "SRP KAT Test");

    diag("SRP 1024 tests");
    ok(test_srp(ccsha1_di(), ccsrp_gp_rfc5054_1024(), rng), "SHA1/GP1024");
    ok(test_srp(ccsha224_di(), ccsrp_gp_rfc5054_1024(), rng), "SHA224/GP1024");
    ok(test_srp(ccsha256_di(), ccsrp_gp_rfc5054_1024(), rng), "SHA256/GP1024");
    ok(test_srp(ccsha384_di(), ccsrp_gp_rfc5054_1024(), rng), "SHA384/GP1024");
    ok(test_srp(ccsha512_di(), ccsrp_gp_rfc5054_1024(), rng), "SHA512/GP1024");
 
    diag("SRP 2048 tests");
    ok(test_srp(ccsha1_di(), ccsrp_gp_rfc5054_2048(), rng), "SHA1/GP2048");
    ok(test_srp(ccsha224_di(), ccsrp_gp_rfc5054_2048(), rng), "SHA224/GP2048");
    ok(test_srp(ccsha256_di(), ccsrp_gp_rfc5054_2048(), rng), "SHA256/GP2048");
    ok(test_srp(ccsha384_di(), ccsrp_gp_rfc5054_2048(), rng), "SHA384/GP2048");
    ok(test_srp(ccsha512_di(), ccsrp_gp_rfc5054_2048(), rng), "SHA512/GP2048");

    diag("SRP 3072 tests");
    ok(test_srp(ccsha1_di(), ccsrp_gp_rfc5054_3072(), rng), "SHA1/GP3072");
    ok(test_srp(ccsha224_di(), ccsrp_gp_rfc5054_3072(), rng), "SHA224/GP3072");
    ok(test_srp(ccsha256_di(), ccsrp_gp_rfc5054_3072(), rng), "SHA256/GP3072");
    ok(test_srp(ccsha384_di(), ccsrp_gp_rfc5054_3072(), rng), "SHA384/GP3072");
    ok(test_srp(ccsha512_di(), ccsrp_gp_rfc5054_3072(), rng), "SHA512/GP3072");

    diag("SRP 4096 tests");
    ok(test_srp(ccsha1_di(), ccsrp_gp_rfc5054_4096(), rng), "SHA1/GP4096");
    ok(test_srp(ccsha224_di(), ccsrp_gp_rfc5054_4096(), rng), "SHA224/GP4096");
    ok(test_srp(ccsha256_di(), ccsrp_gp_rfc5054_4096(), rng), "SHA256/GP4096");
    ok(test_srp(ccsha384_di(), ccsrp_gp_rfc5054_4096(), rng), "SHA384/GP4096");
    ok(test_srp(ccsha512_di(), ccsrp_gp_rfc5054_4096(), rng), "SHA512/GP4096");
    
    diag("SRP 8192 tests");
    ok(test_srp(ccsha1_di(), ccsrp_gp_rfc5054_8192(), rng), "SHA1/GP8192");
    ok(test_srp(ccsha224_di(), ccsrp_gp_rfc5054_8192(), rng), "SHA224/GP8192");
    ok(test_srp(ccsha256_di(), ccsrp_gp_rfc5054_8192(), rng), "SHA256/GP8192");
    ok(test_srp(ccsha384_di(), ccsrp_gp_rfc5054_8192(), rng), "SHA384/GP8192");
    ok(test_srp(ccsha512_di(), ccsrp_gp_rfc5054_8192(), rng), "SHA512/GP8192");

    return 0;
}

#endif
