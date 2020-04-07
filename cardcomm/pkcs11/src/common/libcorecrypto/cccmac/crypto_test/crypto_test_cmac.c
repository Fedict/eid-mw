/*
 * Copyright (c) 2013,2015 Apple Inc. All rights reserved.
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

static int verbose = 0;

#if (CCCMACTEST == 0)
entryPoint(cccmactest,"cccmac test")
#else
#include <corecrypto/ccaes.h>
#include <corecrypto/cccmac.h>

/* Currently, cccmac and friends won't work when length == 0 and the
 * data pointer is NULL.
 */

#define keystr128    "000102030405060708090a0b0c0d0e0f"


#define cmac_DATA_POINTER_NULL_TOLERANT 0

typedef struct test_vector_t {
    int cnt;
    char *keyStr;
    char *aes128_K_0;
    char *k1Str;
    char *k2Str;
    char *inStr;
    char *outStr;
} test_vector;

static int showBytesAreEqual(byteBuffer bb1, byteBuffer bb2, char *label) {
        ok(bytesAreEqual(bb1, bb2), label);
    if(bytesAreEqual(bb1, bb2) == 0) {
        printByteBuffer(bb1, "Want");
        printByteBuffer(bb2, "Got ");
        return 0;
    }
    return 1;
}



static int test_answer(char *mode_name, test_vector *vector, void*answer, char *test_type) {
    byteBuffer answer_bb = bytesToBytes(answer, CMAC_BLOCKSIZE);
    if(vector->outStr == NULL) {
        diag("/* CMAC-128 test %d */", vector->cnt);
        diag("\t\t\"%s\",\n", bytesToHexString(answer_bb));
        return 1;
    }
    byteBuffer correct_answer_bb = hexStringToBytes((char *) vector->outStr);
    ok(bytesAreEqual(correct_answer_bb, answer_bb), "compare memory of answer");
    if(bytesAreEqual(correct_answer_bb, answer_bb) == 0) {
        diag("Failed Test (%d) for CMAC-128-%s %s\n", vector->cnt, mode_name, test_type);
        printByteBuffer(correct_answer_bb, "Correct Answer");
        printByteBuffer(answer_bb, "Provided Answer");
    }
    free(correct_answer_bb);
    free(answer_bb);
    return 1;
}

static int guard_ok(uint8_t *p, int chr, size_t len) {
    for(size_t i=0; i<len; i++) if(p[i] != chr) return 0;
    return 1;
}

static int test_discreet(const struct ccmode_cbc *cbc, char *mode_name, test_vector *vector) {

    uint8_t answer[CMAC_BLOCKSIZE];
    uint8_t ctxfrontguard[4096];
    cccmac_mode_decl(cbc, cmac);
    uint8_t ctxrearguard[4096];
    memset(ctxfrontguard, 0xee, 4096);
    memset(ctxrearguard, 0xee, 4096);
    
    byteBuffer key = hexStringToBytes(vector->keyStr);
    byteBuffer in = hexStringToBytes(vector->inStr);
    size_t nblocks = in->len/CMAC_BLOCKSIZE;
    size_t partial = in->len%CMAC_BLOCKSIZE;
    uint8_t *data = in->bytes;
    
    if(nblocks < 2) nblocks = 0; // must have >= 1 block for final
    else if(!partial) nblocks--; // have to have data for final
    cccmac_init(cbc, cmac, key->bytes);
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    byteBuffer correct_answer_k1 = hexStringToBytes(vector->k1Str);
    byteBuffer correct_answer_k2 = hexStringToBytes(vector->k2Str);
    byteBuffer answer_k1 = bytesToBytes(cccmac_k1(cmac), 16);
    byteBuffer answer_k2 = bytesToBytes(cccmac_k2(cmac), 16);
    showBytesAreEqual(correct_answer_k1, answer_k1, "Subkey K1 is correct");
    showBytesAreEqual(correct_answer_k2, answer_k2, "Subkey K2 is correct");

    for(size_t i=0; i<nblocks; i++) {
        cccmac_block_update(cbc, cmac, 1, data);
        data+=CMAC_BLOCKSIZE;
    }
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    
    cccmac_final(cbc, cmac, in->len - nblocks * CMAC_BLOCKSIZE, data, answer);
    
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    ok(test_answer(mode_name, vector, answer, "discreet calls"), "check answer");
    return 1;
}

static int test_oneshot(const struct ccmode_cbc *cbc, char *mode_name, test_vector *vector) {
    uint8_t answer[CMAC_BLOCKSIZE];
    byteBuffer key = hexStringToBytes(vector->keyStr);
    byteBuffer in = hexStringToBytes(vector->inStr);
    cccmac(cbc, key->bytes, in->len, in->bytes, answer);
    ok(test_answer(mode_name, vector, answer, "one-shot"), "check answer");
    return 1;
}


static int test_cmac(const struct ccmode_cbc *cbc, char *mode_name) {
    static test_vector vector[] = {
        {
            1, // cnt
            "2b7e151628aed2a6abf7158809cf4f3c", // keyStr
            "7df76b0c1ab899b33e42f047b91b546f", // aes128_K_0
            "fbeed618357133667c85e08f7236a8de", // k1Str
            "f7ddac306ae266ccf90bc11ee46d513b", // k2Str
            "", // in
            "bb1d6929e95937287fa37d129b756746" // out
        },
        {
            2, // cnt
            "2b7e151628aed2a6abf7158809cf4f3c", // keyStr
            "7df76b0c1ab899b33e42f047b91b546f", // aes128_K_0
            "fbeed618357133667c85e08f7236a8de", // k1Str
            "f7ddac306ae266ccf90bc11ee46d513b", // k2Str
            "6bc1bee22e409f96e93d7e117393172a", // in
            "070a16b46b4d4144f79bdd9dd04a287c" // out
        },
        {
            3, // cnt
            "2b7e151628aed2a6abf7158809cf4f3c", // keyStr
            "7df76b0c1ab899b33e42f047b91b546f", // aes128_K_0
            "fbeed618357133667c85e08f7236a8de", // k1Str
            "f7ddac306ae266ccf90bc11ee46d513b", // k2Str
            "6bc1bee22e409f96e93d7e117393172a"\
            "ae2d8a571e03ac9c9eb76fac45af8e51"\
            "30c81c46a35ce411", // in
            "dfa66747de9ae63030ca32611497c827" // out
        },
        {
            4, // cnt
            "2b7e151628aed2a6abf7158809cf4f3c", // keyStr
            "7df76b0c1ab899b33e42f047b91b546f", // aes128_K_0
            "fbeed618357133667c85e08f7236a8de", // k1Str
            "f7ddac306ae266ccf90bc11ee46d513b", // k2Str
            "6bc1bee22e409f96e93d7e117393172a"\
            "ae2d8a571e03ac9c9eb76fac45af8e51"\
            "30c81c46a35ce411e5fbc1191a0a52ef"\
            "f69f2445df4f9b17ad2b417be66c3710", // in
            "51f0bebf7e3b9d92fc49741779363cfe" // out
        },
        
    };
    size_t vector_size = sizeof (vector) / sizeof (test_vector);

    for(size_t i = 0; i < vector_size; i++) {
        ok(test_oneshot(cbc, mode_name, &vector[i]), "test one-shot AES128_CMAC");
        ok(test_discreet(cbc, mode_name, &vector[i]), "test discreet AES128_CMAC");
    }
    return 1;
}

int cccmactest(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
	plan_tests(57);
    if(verbose) diag("Starting cmac tests");
    ok(test_cmac(ccaes_cbc_encrypt_mode(), "system cbc di"), "CMAC Tests");
    return 0;
}
#endif
