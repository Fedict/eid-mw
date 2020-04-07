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


#include <stdio.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccmode.h>
#include "crypto_test_modes.h"
#include "testbyteBuffer.h"
#include "testmore.h"


// static int kTestTestCount = 16;
static int verbose = 0;


typedef struct ccgcm_test_t {
    char *keyStr;
    char *adataStr;
    char *ivStr;
    char *ptStr;
    char *ctStr;
    char *tagStr;
} ccgcm_test_vector;

ccgcm_test_vector gcm_vectors[]={
    {
        "00000000000000000000000000000000",
        "",
        "000000000000000000000000",
        "",
        "",
        "58e2fccefa7e3061367f1d57a4e7455a"
    },{
        "00000000000000000000000000000000",
        "",
        "000000000000000000000000",
        "00000000000000000000000000000000",
        "0388dace60b6a392f328c2b971b2fe78",
        "ab6e47d42cec13bdf53a67b21257bddf"
    },{
        "feffe9928665731c6d6a8f9467308308",
        "",
        "cafebabefacedbaddecaf888",
    "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
    "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985",
        "4d5c2af327cd64a62cf35abd2ba6fab4",
    },{
        "feffe9928665731c6d6a8f9467308308",
        "feedfacedeadbeeffeedfacedeadbeefabaddad2",
        "cafebabefacedbaddecaf888",
        "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
        "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
        "5bc94fbc3221a5db94fae95ae7121a47",
    },{
        "feffe9928665731c6d6a8f9467308308",
        "feedfacedeadbeeffeedfacedeadbeefabaddad2",
        "cafebabefacedbad",
        "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
        "61353b4c2806934a777ff51fa22a4755699b2a714fcdc6f83766e5f97b6c742373806900e49f24b22b097544d4896b424989b5e1ebac0f07c23f4598",
        "3612d2e79e3b0785561be14aaca2fccb",
    },{
        "feffe9928665731c6d6a8f9467308308",
        "feedfacedeadbeeffeedfacedeadbeefabaddad2",
        "9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0c95156809539fcf0e2429a6b525416aedbf5a0de6a57a637b39b",
        "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
        "8ce24998625615b603a033aca13fb894be9112a5c3a211a8ba262a3cca7e2ca701e4a9a4fba43c90ccdcb281d48c7c6fd62875d2aca417034c34aee5",
        "619cc5aefffe0bfa462af43c1699d050",
    },{
        "00000000000000000000000000000000",
        "688e1aa984de926dc7b4c47f44",
        "b72138b5a05ff5070e8cd94183f761d8",
        "a2aab3ad8b17acdda288426cd7c429b7ca86b7aca05809c70ce82db25711cb5302eb2743b036f3d750d6cf0dc0acb92950d546db308f93b4ff244afa9dc72bcd758d2c",
        "cbc8d2f15481a4cc7dd1e19aaa83de5678483ec359ae7dec2ab8d534e0906f4b4663faff58a8b2d733b845eef7c9b331e9e10eb2612c995feb1ac15a6286cce8b297a8",
        "8d2d2a9372626f6bee8580276a6366bf",
    },{
        "00000000000000000000000000000000",
        "",
        "",
        "",
        "",
        "66e94bd4ef8a2c3b884cfa59ca342b2e",
    }
};

size_t nvectors = sizeof(gcm_vectors) / sizeof(ccgcm_test_vector);

static void ccgcm_discreet(const struct ccmode_gcm *mode,
                              size_t key_len, const void *key,
                              size_t iv_len, const void *iv,
                              size_t adata_len, const void *adata,
                              size_t nbytes, const void *in, void *out,
                              size_t tag_len, void *tag)
{
	ccgcm_ctx_decl(mode->size, ctx);
	mode->init(mode, ctx, key_len, key);
	mode->set_iv(ctx, iv_len, iv);
    if(adata_len > 0 && adata != NULL) {
        mode->gmac(ctx, adata_len, adata);
    } else {
        if(verbose) printf("Skipping added gmac\n");
    }
	mode->gcm(ctx, nbytes, in, out);
	mode->finalize(ctx, tag_len, tag);
	ccgcm_ctx_clear(mode->size, ctx);
}

static int memeq(const void *a, const void *b, size_t len) {
    return memcmp(a, b, len) == 0;
}


static int gcm_test_discreet(const struct ccmode_gcm *em, const struct ccmode_gcm *dm,
                              size_t key_len, const void *key,
                              size_t iv_len, const void *iv,
                              size_t adata_len, const void *adata,
                              size_t nbytes, const void *plaintext, void *ciphertext,
                              size_t tag_len, void *tag) {
    
    uint8_t cipher_result[nbytes], plain_result[nbytes];
    uint8_t cipher_tag[tag_len], plain_tag[tag_len];
    
    ccgcm_discreet(em, key_len, key, iv_len, iv, adata_len, adata, nbytes, plaintext, cipher_result, tag_len, cipher_tag);
    ccgcm_discreet(dm, key_len, key, iv_len, iv, adata_len, adata, nbytes, cipher_result, plain_result, tag_len, plain_tag);
    
    ok_or_fail(memeq(plaintext, plain_result, nbytes), "Round Trip Encrypt/Decrypt works");
    
    if(tag) {
        ok_or_fail(memeq(tag, cipher_tag, tag_len), "tags match on encrypt");
        ok_or_fail(memeq(tag, plain_tag, tag_len), "tags match on decrypt");
    } else {
        byteBuffer result = bytesToBytes(cipher_tag, tag_len);
        diag("Round Trip Tags\n");
        printByteBufferAsCharAssignment(result, "tagStr");
        free(result);
    }
    
    if(ciphertext) {
        ok_or_fail(memeq(ciphertext, cipher_result, nbytes), "Ciphertext matches known answer");
    } else {
        byteBuffer result = bytesToBytes(ciphertext, nbytes);
        diag("ciphertext produced\n");
        printByteBufferAsCharAssignment(result, "ctStr");
        free(result);
    }
    return 1;
}

static int gcm_test_oneshot(const struct ccmode_gcm *em, const struct ccmode_gcm *dm,
                              size_t key_len, const void *key,
                              size_t iv_len, const void *iv,
                              size_t adata_len, const void *adata,
                              size_t nbytes, const void *plaintext, void *ciphertext,
                              size_t tag_len, void *tag) {
     
    uint8_t cipher_result[nbytes], plain_result[nbytes];
    uint8_t cipher_tag[tag_len], plain_tag[tag_len];
    
    ccgcm_one_shot(em, key_len, key, iv_len, iv, adata_len, adata, nbytes, plaintext, cipher_result, tag_len, cipher_tag);
    ccgcm_one_shot(dm, key_len, key, iv_len, iv, adata_len, adata, nbytes, cipher_result, plain_result, tag_len, plain_tag);
    
    ok_or_fail(memeq(plaintext, plain_result, nbytes), "Round Trip Encrypt/Decrypt works");
    
    if(tag) {
        ok_or_fail(memeq(tag, cipher_tag, tag_len), "tags match on encrypt");
        ok_or_fail(memeq(tag, plain_tag, tag_len), "tags match on decrypt");
    } else {
        byteBuffer result = bytesToBytes(cipher_tag, tag_len);
        diag("Round Trip Tags\n");
        printByteBufferAsCharAssignment(result, "tagStr");
        free(result);
    }
    
    if(ciphertext) {
        ok_or_fail(memeq(ciphertext, cipher_result, nbytes), "Ciphertext matches known answer");
    } else {
        byteBuffer result = bytesToBytes(ciphertext, nbytes);
        diag("ciphertext produced\n");
        printByteBufferAsCharAssignment(result, "ctStr");
        free(result);
    }
    return 1;
}


static int gcm_testcase(const struct ccmode_gcm *encrypt_ciphermode, const struct ccmode_gcm *decrypt_ciphermode,
                        size_t casenum, char *keyStr, char *adataStr, char *ivStr,
                        char *ptStr, char *ctStr, char *tagStr) {
    byteBuffer key = hexStringToBytes(keyStr);
    byteBuffer iv = hexStringToBytes(ivStr);
    byteBuffer adata = hexStringToBytes(adataStr);
    byteBuffer plaintext = hexStringToBytes(ptStr);
    byteBuffer ciphertext = hexStringToBytes(ctStr);
    byteBuffer tag = hexStringToBytes(tagStr);
    
    if(verbose) printf("GCM Case %lu\n", casenum);
    
    ok_or_fail(gcm_test_oneshot(encrypt_ciphermode, decrypt_ciphermode,
                        key->len, key->bytes,
                        iv->len, iv->bytes,
                        adata->len, adata->bytes,
                        plaintext->len,
                        plaintext->bytes, ciphertext->bytes,
                        tag->len, tag->bytes), "Successful Round Trip Encrypt/Validate");
    
     
    ok_or_fail(gcm_test_discreet(encrypt_ciphermode, decrypt_ciphermode,
                        key->len, key->bytes,
                        iv->len, iv->bytes,
                        adata->len, adata->bytes,
                        plaintext->len,
                        plaintext->bytes, ciphertext->bytes,
                        tag->len, tag->bytes), "Successful Round Trip Encrypt/Validate");
    return 1;
    
}

int test_gcm(const struct ccmode_gcm *encrypt_ciphermode, const struct ccmode_gcm *decrypt_ciphermode) {
	CC_UNUSED int retval; // In case of debug

	// plan_tests(kTestTestCount);
    
    for(size_t i = 0; i < nvectors; i++) {
        retval = gcm_testcase(  encrypt_ciphermode, decrypt_ciphermode, i,
                                gcm_vectors[i].keyStr,
                                gcm_vectors[i].adataStr,
                                gcm_vectors[i].ivStr,
                                gcm_vectors[i].ptStr,
                                gcm_vectors[i].ctStr,
                                gcm_vectors[i].tagStr);
    }

    return 1;
}


