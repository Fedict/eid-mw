/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccrsa_priv.h>
#include <corecrypto/ccrsa.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccrng_sequence.h>
#include <corecrypto/ccsha2.h>
#include "crypto_test_rsapss.h"
#include "testmore.h"

/*
 http://www.emc.com/emc-plus/rsa-labs/standards-initiatives/pkcs-rsa-cryptography-standard.htm#
 ftp://ftp.rsasecurity.com/pub/pkcs/pkcs-1/pkcs-1v2-1-vec.zip
 the test vectors are from RSA lab (above links)
 A python scrips tranforms  pss-vect.txt to SigPSS.inc
 These test vectors donot check the the salt generation function.
 
 The CAVP test vectors are not particularly useful, because they just check the RSA signing operation and not the PSS padding.
 http://csrc.nist.gov/groups/STM/cavp/documents/components/RSA2SP1VS.pdf
 
 */

#define TEST_VECT_HASH_LEN 20
#define N_RSA_PARAMS 10
#define N_EXAMPLES 6
struct rsapss_sig_test_t {
    char * modulus; size_t modulus_len;
    char *e; size_t e_len;
    char *d; size_t d_len;
    char *p; size_t p_len;
    char *q; size_t q_len;
    char *dp; size_t dp_len;
    char *dq; size_t dq_len;
    char *qinv; size_t qinv_len;
    
    struct {
        char * msg; size_t msg_len;
        char *salt; size_t salt_len;
        char *sig; size_t sig_len;
    } example[N_EXAMPLES];
    
} rsapss_sig_test_vect [N_RSA_PARAMS]= {
    //The initializer file must have exactly 10 set of RSA parameters and 6 example per each RSA parameters.
#include "../test_vectors/pss-vect.inc"
};

//this is for debugging. It is not meant for testing the library
struct rsapss_sig_test_t rsapss_sig_test2 =
{
#include "../test_vectors/pss-int-vect.inc"
};

//this is for debugging. It is not meant for testing the library
struct rsapss_sig_intermediate_t {
    char *mHash; size_t mHash_len;
    char *salt; size_t salt_len;
    char *Mp; size_t Mp_len;
    char *H; size_t H_len;
    char *DB; size_t DB_len;
    char *dbMask; size_t dbMask_len;
    char *maskedDB; size_t maskedDB_len;
    char *EM; size_t EM_len;
} rsapss_sig_int = {
#include "../test_vectors/pss-int.inc"
};

static int read_fullkey(ccrsa_full_ctx_t fk, const struct rsapss_sig_test_t *v)
{
    size_t n = ccrsa_ctx_n(fk);
    
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    CCZP_N(ccrsa_ctx_zm(pubk)) = n;
    
    cc_unit exponent[n];
    cc_unit modulus[n];
    ccn_read_uint(n, modulus, (cc_unit)v->modulus_len, (unsigned char*)v->modulus);
    ccn_read_uint(n, exponent, v->e_len, (unsigned char*)v->e);
    ccrsa_init_pub(fk, modulus, exponent);
    cczp_init(ccrsa_ctx_zm(pubk)); //not needed
    
    //set the private key
    size_t np, nq;
    
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(fk);
    CCZP_N(ccrsa_ctx_private_zp(privk)) = np=ccn_nof(v->p_len*8);
    CCZP_N(ccrsa_ctx_private_zq(privk)) = nq=ccn_nof(v->q_len*8);
    ccn_read_uint(np, CCZP_PRIME(ccrsa_ctx_private_zp(privk)), v->p_len, (unsigned char *)v->p);
    ccn_read_uint(nq, CCZP_PRIME(ccrsa_ctx_private_zq(privk)), v->q_len, (unsigned char *)v->q);
    ccn_read_uint(np, ccrsa_ctx_private_dp(privk), v->dp_len, (unsigned char *)v->dp);
    ccn_read_uint(nq, ccrsa_ctx_private_dq(privk), v->dq_len, (unsigned char *)v->dq);
    ccn_read_uint(np, ccrsa_ctx_private_qinv(privk), v->qinv_len, (unsigned char *)v->qinv);
    
    //need to initialize reciprocals of zp and zq
    cczp_init(ccrsa_ctx_private_zp(privk));
    cczp_init(ccrsa_ctx_private_zq(privk));
    
    return 0;
    
    //not exported, linker error
    //return ccrsa_pairwise_consistency_check(fk)==true?0:-1;
}

//same as ok(). Print integer i as well as the string s
#define oki(cond, s, i) {char buf[132+1]; snprintf(buf, sizeof(buf), "%s: vect#%i ", (s), i); ok((cond), buf);}
static int test_rsapss_sig(const struct rsapss_sig_test_t *v, int nex)
{
    const cc_size n = ccn_nof((cc_size)v->modulus_len*8);
    ccrsa_full_ctx_decl(ccn_sizeof_n(n), fk);
    ccrsa_ctx_n(fk) = n;
    read_fullkey(fk, v);
    
    const struct ccdigest_info *di = ccsha1_di();
    ok_or_goto(TEST_VECT_HASH_LEN == di->output_size, "RSA-PSS: test vector hash len mismatch\n", fail);
    
    int rc=0, i;
    for(i=0; i<nex; i++){ //round trip signature check
        size_t siglen = v->example[i].sig_len;
        uint8_t sig[siglen];
        unsigned char mHash[di->output_size];
        
        //compute the message hash
        ccdigest(di, v->example[i].msg_len, v->example[i].msg, mHash);
        
        //set a dummy rng, and pass to signature function for salt generation
        struct ccrng_sequence_state seq_rng;
        ccrng_sequence_init(&seq_rng, v->example[i].salt_len, (uint8_t*)v->example[i].salt);
        struct ccrng_state *rng = (struct ccrng_state *)&seq_rng;
        
        rc = ccrsa_sign_pss(fk, di,di, v->example[i].salt_len, rng, TEST_VECT_HASH_LEN, mHash, &siglen, sig);
        oki(rc==0,  "signing error", i);
        
        rc = memcmp(sig, v->example[i].sig, siglen); oki(rc==0,  "wrong signature generated", i);
        
        //sig verification for the same data
        bool valid=false;
        rc=ccrsa_verify_pss(ccrsa_ctx_public(fk),
                            di, di,
                            di->output_size, mHash,
                            v->example[i].sig_len, (uint8_t *)v->example[i].sig,
                            v->example[i].salt_len, &valid);  ok(rc==0 && valid==true, "generated signature doesn't verify");
    }//for nex
    return 0;
fail:
    return -1;
    
}


static int pss_round_trip(const ccrsa_full_ctx_t fk,
                          const struct ccdigest_info* hashAlgorithm,
                          const struct ccdigest_info* MgfHashAlgorithm,
                          size_t saltLen,struct ccrng_state *rng,
                          size_t hLen, const uint8_t *mHash,
                          size_t *sigLen, uint8_t *sig)
{
    int rc;
    bool valid=false;
    
    rc = ccrsa_sign_pss(fk, hashAlgorithm, MgfHashAlgorithm, saltLen, rng, hLen, mHash, sigLen, sig);
    
    rc|=ccrsa_verify_pss(ccrsa_ctx_public(fk),
                         hashAlgorithm, MgfHashAlgorithm,
                         hLen, mHash,
                         *sigLen, sig,
                         saltLen, &valid);
    
    return (valid==true && rc==0)?0 : -1;
    
}


static void flipbit(unsigned char *s, int k)
{
    int k7 = 1<<(k&7);
    int bit= k7 & s[k/8];
    
    if(!bit)
        s[k/8] |= k7;
    else
        s[k/8] &= ~k7;
}

#define okrc(cond, s, rc) {(rc)|=(cond)?0:-1; ok((cond),(s));}
static int test_rsapss_misc(const struct rsapss_sig_test_t *v)
{
    int rc=0, rc2=0;
    size_t saltLen, hLen, sigLen;
    const struct ccdigest_info *di;
    
    const cc_size n = ccn_nof((cc_size)v->modulus_len*8);
    ccrsa_full_ctx_decl(ccn_sizeof_n(n), fk);
    ccrsa_ctx_n(fk) = n;
    read_fullkey(fk, v);
    
    const struct ccdigest_info *di160 = ccsha1_di();
    const struct ccdigest_info *di512 = ccsha512_di();
    struct ccrng_state *rng=global_test_rng;
    uint8_t mHash[di512->output_size]; //big enough for all hashes
    ccdigest(di512, 30, "The quick brown fox jumps over the lazy dog.", mHash); //don't want to use strlen()
    
    //TEST: saltLen>hLen
    const size_t modBits = ccn_bitlen(ccrsa_ctx_n(fk) , ccrsa_ctx_m(fk) );
    const size_t modBytes = cc_ceiling(modBits, 8);
    uint8_t sig[n*sizeof(cc_unit)];
    
    //TEST: saltlen>hlen, fails
    di = di160;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = hLen+1;
    rc= ccrsa_sign_pss(fk, di, di512, saltLen, rng, hLen, mHash, &sigLen, sig);
    okrc(rc!=0, "negative test failed:  saltLen>hLen, msg=sha1, MGF=sha512", rc2);
    
    //TEST: saltLen=0, passes
    di = di160;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = 0;
    rc=pss_round_trip(fk,di, di512, saltLen, rng, hLen, mHash, &sigLen, sig);
    okrc(rc==0, "test failed:  saltLen==0", rc2);
    
    //TEST: saltLen=0 with different hashes, passes
    di = di512;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = 0;
    rc=pss_round_trip(fk,di, di160, saltLen, rng, hLen, mHash, &sigLen, sig);
    okrc(rc==0,"\n test failed:  saltLen==0, msg=sha512, MGF=sha1", rc2);
    
    //TEST:
    //len=emLen-sLen-hLen-2; len==0, should pass but fails, because doesn't specify what to do if MGF output width is smaller than input width
    di = di512;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = 62;
    rc=pss_round_trip(fk,di, di512, saltLen, rng, hLen, mHash, &sigLen, sig);
    okrc(rc!=0, "\n test failed:  saltLen==0", rc2);
    
    //TEST: flip a random bit in the signature pass, big message hash, small MGF hash, fails
    di = di512;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = hLen-3;
    rc=pss_round_trip(fk,di, di160, saltLen, rng, hLen, mHash, &sigLen, sig);
    okrc(rc==0, "pss_round_trip test failed", rc2);
    uint16_t bit_num;
    bool valid;
    rc=ccrng_generate(rng, 2,&bit_num);
    okrc(rc==0, "random generation failed", rc2);
    cc_assert(n*sizeof(cc_unit)*8>0x02FF);
    flipbit(sig, bit_num & 0x02FF);
    rc=ccrsa_verify_pss(ccrsa_ctx_public(fk), di, di160, hLen, mHash, sigLen, sig, saltLen, &valid);
    okrc(rc!=0, "ccrsa_verify_pss() flipbit negative test failed", rc2);
    
    //TEST: rng returns error
    struct ccrng_sequence_state rng_seq;
    ccrng_sequence_init(&rng_seq, 0, NULL); // length=0 => make it generate error
    di = di160;
    hLen = di->output_size;
    sigLen=modBytes;
    saltLen = hLen;
    rc= ccrsa_sign_pss(fk, di, di512, saltLen, (struct ccrng_state *)&rng_seq, hLen, mHash, &sigLen, sig);
    okrc(rc!=0, "rng negative test failed", rc2);

    return rc2;
}

int test_rsa_pss_known_answer(void)
{
    int i, rc;
    
    rc=test_rsapss_misc(&rsapss_sig_test_vect[1]);
    for(i=0; i<N_RSA_PARAMS; i++)
        rc|=test_rsapss_sig(&rsapss_sig_test_vect[i], N_EXAMPLES);
    
    return rc;
}
