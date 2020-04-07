/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
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

#include "testmore.h"
#include "testbyteBuffer.h"
#include <corecrypto/ccaes.h>
#include <corecrypto/ccmode.h>
#include "ccsymmetric.h"
#include "crypto_test_modes.h"
#include "cc_debug.h"

static int verbose = 0;

typedef struct duplex_cryptor_t {
    ciphermode_t encrypt_ciphermode;
    ciphermode_t decrypt_ciphermode;
    cc_cipher_select cipher;
    cc_mode_select mode;
} duplex_cryptor_s, *duplex_cryptor;


typedef struct ccsymmetric_test_t {
    char *keyStr;
    char *twkStr;
    char *init_ivStr;
    char *block_ivStr;
    char *aDataStr;
    char *ptStr;
    char *ctStr;
    char *tagStr;
} ccsymmetric_test_vector;

// These are stock keys/IVs/blocks to encode - don't change them - add if you
// need more.
#define keystr64     "0001020304050607"
#define keystr128    "000102030405060708090a0b0c0d0e0f"
#define keystr192    "000102030405060708090a0b0c0d0e0f1011121314151617"
#define keystr256    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
#define twkstr128    "000102030405060708090a0b0c0d0e0f"
#define ivstr64      "0f0e0d0c0b0a0908"
#define ivstr128     "0f0e0d0c0b0a09080706050403020100"
#define ivstrff64    "ffffffffffffffff"
#define ivstrff128   "ffffffffffffffffffffffffffffffff"
#define zeroX1       "00"
#define zeroX16      "00000000000000000000000000000000"
#define zeroX32      "0000000000000000000000000000000000000000000000000000000000000000"
#define zeroX33      "000000000000000000000000000000000000000000000000000000000000000000"
#define zeroX64      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define aDataTest    "TEST"
#define END_VECTOR   { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }

static void report_cipher_mode(duplex_cryptor cryptor) {
    char *cipherStr, *modeStr;
    
    switch(cryptor->cipher) {
        case cc_cipherAES: cipherStr = "AES-"; break;
        case cc_cipherDES: cipherStr = "DES-"; break;
        case cc_cipher3DES: cipherStr = "3DES-"; break;
        case cc_cipherCAST: cipherStr = "CAST-"; break;
        case cc_cipherRC2: cipherStr = "RC2-"; break;
        case cc_cipherBlowfish: cipherStr = "Blowfish-"; break;
        default: cipherStr = "UnknownCipher-"; break;
    }
    switch(cryptor->mode) {
        case cc_ModeECB: modeStr = "ECB\n"; break;
        case cc_ModeCBC: modeStr = "CBC\n"; break;
        case cc_ModeCFB: modeStr = "CFB\n"; break;
        case cc_ModeCTR: modeStr = "CTR\n"; break;
        case cc_ModeOFB: modeStr = "OFB\n"; break;
        case cc_ModeXTS: modeStr = "XTS\n"; break;
        case cc_ModeCFB8: modeStr = "CFB8\n"; break;
        case cc_ModeGCM: modeStr = "GCM\n"; break;
        case cc_ModeCCM: modeStr = "CCM\n"; break;
        default: modeStr = "UnknownMode\n"; break;
    }
    diag("%s%s", cipherStr, modeStr);
}

ccsymmetric_test_vector aes_ecb_vectors[] = {
    { keystr128, NULL, NULL, NULL, NULL, zeroX16, "c6a13b37878f5b826f4f8162a1c8d879", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX32, "c6a13b37878f5b826f4f8162a1c8d879c6a13b37878f5b826f4f8162a1c8d879", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX64, "c6a13b37878f5b826f4f8162a1c8d879c6a13b37878f5b826f4f8162a1c8d879c6a13b37878f5b826f4f8162a1c8d879", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_cbc_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX16, "20a9f992b44c5be8041ffcdc6cae996a", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX32, "20a9f992b44c5be8041ffcdc6cae996ae40e2d6f4762a0c584042b8bd534704b", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX64, "20a9f992b44c5be8041ffcdc6cae996ae40e2d6f4762a0c584042b8bd534704b8b9c1f12376c87fdb08b354e40418f9d", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_cfb_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX1, "20", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX16, "20a9f992b44c5be8041ffcdc6cae996a", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX33, "20a9f992b44c5be8041ffcdc6cae996ae40e2d6f4762a0c584042b8bd534704b8b", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_ctr_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX1, "20", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX16, "20a9f992b44c5be8041ffcdc6cae996a", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX33, "20a9f992b44c5be8041ffcdc6cae996a47a6a4a5755e60446eb291ec4939015fbb", NULL },
#if 0
    /* For reference / debug */
    /* Counter rolls over 32bit */
    { keystr128, NULL, ivstrff128, ivstrff128, NULL, zeroX32, "3c441f32ce07822364d7a2990e50bb13dd94a22c83d419e0f9e7dcda9b8da9d4", NULL },
    /* Counter rolls over 128bit */
    { keystr128, NULL, ivstrff128, ivstrff128, NULL, zeroX32, "3c441f32ce07822364d7a2990e50bb13c6a13b37878f5b826f4f8162a1c8d879", NULL },
#else
    /* corecrypto implementation: counter rolls over 64bit */
    { keystr128, NULL, ivstrff128, ivstrff128, NULL, zeroX32, "3c441f32ce07822364d7a2990e50bb1325d4e948bd5e1296afc0bf87095a7248", NULL },
#endif
    END_VECTOR
};

ccsymmetric_test_vector aes_ofb_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX1, "20", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX16, "20a9f992b44c5be8041ffcdc6cae996a", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX33, "20a9f992b44c5be8041ffcdc6cae996ae40e2d6f4762a0c584042b8bd534704b8b", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_xts_vectors[] = {
    { keystr128, twkstr128, ivstr128, ivstr128, NULL, zeroX16, "c4a7d4fdf32efb2d801bd757b99ae921", NULL },
    { keystr128, twkstr128, ivstr128, ivstr128, NULL, zeroX32, "c4a7d4fdf32efb2d801bd757b99ae921f9125f42a92b0e14d6596cc0e6b4b4a1", NULL },
    { keystr128, twkstr128, ivstr128, ivstr128, NULL, zeroX64, "c4a7d4fdf32efb2d801bd757b99ae921f9125f42a92b0e14d6596cc0e6b4b4a1ebb4c4592b47f182be6212f8d0d21ce4", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_cfb8_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX1, "20", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX16, "20850f3e23fb3645d633538f3bedcecc", NULL },
    { keystr128, NULL, ivstr128, ivstr128, NULL, zeroX33, "20850f3e23fb3645d633538f3bedceccf85db3f7e68cb4b72bb029404c755a0399", NULL },
    END_VECTOR
};

ccsymmetric_test_vector aes_gcm_vectors[] = {
    { keystr128, NULL, ivstr128, ivstr128, aDataTest, zeroX1, "9e", "c0b7b834467c01300eb37a2d94593a48" },
    { keystr128, NULL, ivstr128, ivstr128, aDataTest, zeroX16, "9ea5433afafdaca2ac376b736ae44152", "bdaa4f10af822c61178783033143eed1" },
    { keystr128, NULL, ivstr128, ivstr128, aDataTest, zeroX33, "9ea5433afafdaca2ac376b736ae44152a3f8f8f5d378d1a378be3175d28162b686", "1b11adfcfbec6fd1300a0290c3784049" },
    END_VECTOR
};

ccsymmetric_test_vector aes_ccm_vectors[] = {
    // From NIST CAV tests.
    // AES 128
    { "4bb3c4a4f893ad8c9bdc833c325d62b3", NULL, "5a8aa485c316e9403aff", ivstr128, NULL, "00", "a1", "246d32b7" },
    { "4bb3c4a4f893ad8c9bdc833c325d62b3", NULL, "5a8aa485c316e9403aff85", ivstr128, NULL, "00", "89", "086723fd" },
    { "4ae701103c63deca5b5a3939d7d05992", NULL, "5a8aa485c316e9", ivstr128, NULL, "", "", "02209f55" },
    { "4bb3c4a4f893ad8c9bdc833c325d62b3", NULL, "5a8aa485c316e9", ivstr128, NULL, "", "", "75d582db43ce9b13ab4b6f7f14341330" },
    { "4bb3c4a4f893ad8c9bdc833c325d62b3", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "", "", "90156f3f" },
    { "19ebfde2d5468ba0a3031bde629b11fd", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "", "", "fb04dc5a44c6bb000f2440f5154364b4" },
    { "19ebfde2d5468ba0a3031bde629b11fd", NULL, "5a8aa485c316e9", ivstr128, NULL, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22", "a90e8ea44085ced791b2fdb7fd44b5cf0bd7d27718029bb7", "03e1fa6b" },
    { "197afb02ffbd8f699dacae87094d5243", NULL, "5a8aa485c316e9", ivstr128, NULL, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22", "24ab9eeb0e5508cae80074f1070ee188a637171860881f1f", "2d9a3fbc210595b7b8b1b41523111a8e" },
    { "197afb02ffbd8f699dacae87094d5243", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697", "4a550134f94455979ec4bf89ad2bd80d25a77ae94e456134", "a3e138b9" },
    { "90929a4b0ac65b350ad1591611fe4829", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697", "4bfe4e35784f0a65b545477e5e2f4bae0e1e6fa717eaf2cb", "6a9a970b9beb2ac1bd4fd62168f8378a" },
    { "90929a4b0ac65b350ad1591611fe4829", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "", "", "782e4318" },
    { "6a798d7c5e1a72b43e20ad5c7b08567b", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "", "", "41b476013f45e4a781f253a6f3b1e530" },
    { "6a798d7c5e1a72b43e20ad5c7b08567b", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "", "", "9f69f24f" },
    { "f9fdca4ac64fe7f014de0f43039c7571", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "", "", "1859ac36a40a6b28b34266253627797a" },
    { "f9fdca4ac64fe7f014de0f43039c7571", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768", "6be31860ca271ef448de8f8d8b39346daf4b81d7e92d65b3", "38f125fa" },
    { "a7aa635ea51b0bb20a092bd5573e728c", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768", "b351ab96b2e45515254558d5212673ee6c776d42dbca3b51", "2cf3a20b7fd7c49e6e79bef475c2906f" },
    { "a7aa635ea51b0bb20a092bd5573e728c", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
        "934f893824e880f743d196b22d1f340a52608155087bd28a", "c25e5329" },
    { "26511fb51fcfa75cb4b44da75a6e5a0e", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5", "50038b5fdd364ee747b70d00bd36840ece4ea19998123375", "c0a458bfcafa3b2609afe0f825cbf503" },
    // AES 192
    { "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c", NULL, "5a8aa485c316e9", ivstr128, NULL, "", "", "9d4b7f3b" },
    { "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65", NULL, "5a8aa485c316e9", ivstr128, NULL, "", "", "17223038fa99d53681ca1beabe78d1b4" },
    { "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "", "", "fe69ed84" },
    { "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "", "", "0c66a8e547ed4f8c2c9a9a1eb5d455b9" },
    { "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa", NULL, "5a8aa485c316e9", ivstr128, NULL, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22", "411986d04d6463100bff03f7d0bde7ea2c3488784378138c", "ddc93a54" },
    { "197afb02ffbd8f699dacae87094d524324576b99844f75e1", NULL, "5a8aa485c316e9", ivstr128, NULL, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22", "cba4b4aeb85f0492fd8d905c4a6d8233139833373ef188a8", "c5a5ebecf7ac8607fe412189e83d9d20" },
    { "197afb02ffbd8f699dacae87094d524324576b99844f75e1", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697",
        "042653c674ef2a90f7fb11d30848e530ae59478f1051633a", "34fad277" },
    { "90929a4b0ac65b350ad1591611fe48297e03956f6083e451", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, NULL, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697",
        "a5b7d8cca2069908d1ed88e6a9fe2c9bede3131dad54671e", "a7ade30a07d185692ab0ebdf4c78cf7a" },
    { "90929a4b0ac65b350ad1591611fe48297e03956f6083e451", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "", "", "1d089a5f" },
    { "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "", "", "5280a2137fee3deefcfe9b63a1199fb3" },
    { "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "", "", "5e0eaebd" },
    { "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "", "", "d07ccf9fdc3d33aa94cda3d230da707c" },
    { "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4", NULL, "5a8aa485c316e9", ivstr128, "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c", "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768", "9f6ca4af9b159148c889a6584d1183ea26e2614874b05045", "75dea8d1" },
    { "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5", "16e543d0e20615ff0df15acd9927ddfe40668a54bb854ccc", "c25e9fce" },
    { "26511fb51fcfa75cb4b44da75a6e5a0eb8d9c8f3b906f886", NULL, "5a8aa485c316e9403aff859fbb", ivstr128, "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982", "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5", "c5b0b2ef17498c5570eb335df4588032958ba3d69bf6f317", "8464a6f7fa2b76744e8e8d95691cecb8" },
    // AES 256
    { "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6", NULL, "a544218dadd3c1", ivstr128, NULL, "", "", "469c90bb" },
    { "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8", NULL, "a544218dadd3c1", ivstr128, NULL, "", "", "8207eb14d33855a52acceed17dbcbf6e" },
    { "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8", NULL, "a544218dadd3c10583db49cf39", ivstr128, NULL, "", "", "8a19a133" },
    { "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569", NULL, "a544218dadd3c10583db49cf39", ivstr128, NULL, "", "", "97e1a8dd4259ccd2e431e057b0397fcf" },
    { "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569", NULL, "a544218dadd3c1", ivstr128, NULL, "d3d5424e20fbec43ae495353ed830271515ab104f8860c98", "64a1341679972dc5869fcf69b19d5c5ea50aa0b5e985f5b7", "22aa8d59" },
    { "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453", NULL, "a544218dadd3c1", ivstr128, NULL, "d3d5424e20fbec43ae495353ed830271515ab104f8860c98", "bc51c3925a960e7732533e4ef3a4f69ee6826de952bcb0fd", "374f3bb6db8377ebfc79674858c4f305" },
    { "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453", NULL, "a544218dadd3c10583db49cf39", ivstr128, NULL, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e",
        "63e00d30e4b08fd2a1cc8d70fab327b2368e77a93be4f412", "3d14fb3f" },
    { "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4", NULL, "a544218dadd3c10583db49cf39", ivstr128, NULL, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e",
        "f0050ad16392021a3f40207bed3521fb1e9f808f49830c42", "3a578d179902f912f9ea1afbce1120b3" },
    { "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4", NULL, "a544218dadd3c1", ivstr128, "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab", "", "", "92d00fbe" },
    { "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088", NULL, "a544218dadd3c1", ivstr128, "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab", "", "", "93af11a08379eb37a16aa2837f09d69d" },
    { "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088", NULL, "a544218dadd3c10583db49cf39", ivstr128, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907", "", "", "866d4227" },
    { "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a", NULL, "a544218dadd3c10583db49cf39", ivstr128, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907", "", "", "867b0d87cf6e0f718200a97b4f6d5ad5" },
    { "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a", NULL, "a544218dadd3c1", ivstr128, "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab", "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3", "c2fe12658139f5d0dd22cadf2e901695b579302a72fc5608", "3ebc7720" },
    { "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe", NULL, "a544218dadd3c1", ivstr128, "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab", "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3", "3341168eb8c48468c414347fb08f71d2086f7c2d1bd581ce", "1ac68bd42f5ec7fa7e068cc0ecd79c2a" },
    { "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe", NULL, "a544218dadd3c10583db49cf39", ivstr128, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907", "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3", "c0ea400b599561e7905b99262b4565d5c3dc49fad84d7c69", "ef891339" },
    { "314a202f836f9f257e22d8c11757832ae5131d357a72df88f3eff0ffcee0da4e", NULL, "a544218dadd3c10583db49cf39", ivstr128, "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907", "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3", "8d34cdca37ce77be68f65baf3382e31efa693e63f914a781", "367f30f2eaad8c063ca50795acd90203" },
    END_VECTOR
};

ccsymmetric_test_vector des_ecb_vectors[] = {
    { keystr64, NULL, NULL, NULL, NULL, zeroX16, "a5173ad5957b4370a5173ad5957b4370", NULL },
    { keystr64, NULL, NULL, NULL, NULL, zeroX32, "a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370", NULL },
    { keystr64, NULL, NULL, NULL, NULL, zeroX64, "a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370a5173ad5957b4370", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des_cbc_vectors[] = {
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX16, "af342d1acd53c72120a127bead351d12", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX32, "af342d1acd53c72120a127bead351d125afe64feb410e48667671ed946a622a6", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX64, "af342d1acd53c72120a127bead351d125afe64feb410e48667671ed946a622a619e7c39e2e725fd9338b3d69b8ddd450", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des_cfb_vectors[] = {
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX1, "af", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX16, "af342d1acd53c72120a127bead351d12", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX33, "af342d1acd53c72120a127bead351d125afe64feb410e48667671ed946a622a619", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des_ctr_vectors[] = {
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX1, "af", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX16, "af342d1acd53c72146f42ae448a624b6", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX33, "af342d1acd53c72146f42ae448a624b68e03b9d1e04cff967a4261be15103f910a", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des_ofb_vectors[] = {
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX1, "af", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX16, "af342d1acd53c72120a127bead351d12", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX33, "af342d1acd53c72120a127bead351d125afe64feb410e48667671ed946a622a619", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des_cfb8_vectors[] = {
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX1, "af", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX16, "af445cfa60ae8dc4545ac82c1cf7015e", NULL },
    { keystr64, NULL, ivstr64, ivstr64, NULL, zeroX33, "af445cfa60ae8dc4545ac82c1cf7015e53b2c0ccbcbd94da460087a074525d6e3d", NULL },
    END_VECTOR
};


ccsymmetric_test_vector des3_ecb_vectors[] = {
    { keystr192, NULL, NULL, NULL, NULL, zeroX16, "894bc3085426a441894bc3085426a441", NULL },
    { keystr192, NULL, NULL, NULL, NULL, zeroX32, "894bc3085426a441894bc3085426a441894bc3085426a441894bc3085426a441", NULL },
    { keystr192, NULL, NULL, NULL, NULL, zeroX64, "894bc3085426a441894bc3085426a441894bc3085426a441894bc3085426a441894bc3085426a441894bc3085426a441", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des3_cbc_vectors[] = {
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX16, "a38feed967ba6cfe6f2417e54f7b5260", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX32, "a38feed967ba6cfe6f2417e54f7b5260ed6430b817a27ce3746407017aa59d77", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX64, "a38feed967ba6cfe6f2417e54f7b5260ed6430b817a27ce3746407017aa59d776abe8fff018b09bec17d5938c4f23100", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des3_cfb_vectors[] = {
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX1, "a3", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX16, "a38feed967ba6cfe6f2417e54f7b5260", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX33, "a38feed967ba6cfe6f2417e54f7b5260ed6430b817a27ce3746407017aa59d776a", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des3_ctr_vectors[] = {
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX1, "a3", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX16, "a38feed967ba6cfe6488374dff61b9fd", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX33, "a38feed967ba6cfe6488374dff61b9fd4362abfc77bb2e0c8c5592fedbbd5b5930", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des3_ofb_vectors[] = {
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX1, "a3", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX16, "a38feed967ba6cfe6f2417e54f7b5260", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX33, "a38feed967ba6cfe6f2417e54f7b5260ed6430b817a27ce3746407017aa59d776a", NULL },
    END_VECTOR
};

ccsymmetric_test_vector des3_cfb8_vectors[] = {
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX1, "a3", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX16, "a3d313e297cd33235df0692276ec1aeb", NULL },
    { keystr192, NULL, ivstr64, ivstr64, NULL, zeroX33, "a3d313e297cd33235df0692276ec1aeb578b180d77557126d9beb8eb5bacba56de", NULL },
    END_VECTOR
};



ccsymmetric_test_vector cast_ecb_vectors[] = {
    { keystr128, NULL, NULL, NULL, NULL, zeroX16, "98ed0a15f0337b1b98ed0a15f0337b1b", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX32, "98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX64, "98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b98ed0a15f0337b1b", NULL },
    END_VECTOR
};

ccsymmetric_test_vector cast_cbc_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "c5546bdc50a400f7722c685d84ec285f", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX32, "c5546bdc50a400f7722c685d84ec285fe6bab3d5f479bd6312dae235be573946", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX64, "c5546bdc50a400f7722c685d84ec285fe6bab3d5f479bd6312dae235be57394603d3e828a3e1e54785e3e594c0649fb5", NULL },
    END_VECTOR
};

ccsymmetric_test_vector cast_cfb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "c5", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "c5546bdc50a400f7722c685d84ec285f", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "c5546bdc50a400f7722c685d84ec285fe6bab3d5f479bd6312dae235be57394603", NULL },
    END_VECTOR
};

ccsymmetric_test_vector cast_ctr_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "c5", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "c5546bdc50a400f798ab6de151e19203", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "c5546bdc50a400f798ab6de151e19203d74a3e9a0d82be553dbe9089fcf2a94b61", NULL },
    END_VECTOR
};

ccsymmetric_test_vector cast_ofb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "c5", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "c5546bdc50a400f7722c685d84ec285f", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "c5546bdc50a400f7722c685d84ec285fe6bab3d5f479bd6312dae235be57394603", NULL },
    END_VECTOR
};

ccsymmetric_test_vector cast_cfb8_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "c5", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "c5f272819cbc9557d5921753269e5020", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "c5f272819cbc9557d5921753269e50205a6d7ff7b07737391493963e5b10bf78b8", NULL },
    END_VECTOR
};



ccsymmetric_test_vector rc2_ecb_vectors[] = {
    { keystr128, NULL, NULL, NULL, NULL, zeroX16, "9c4bfe6dfe739c2b9c4bfe6dfe739c2b", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX32, "9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX64, "9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b9c4bfe6dfe739c2b", NULL },
    END_VECTOR
};

ccsymmetric_test_vector rc2_cbc_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "b2df22134258d3566c964020e5918809", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX32, "b2df22134258d3566c964020e59188099e072f766fcf49e63eddcd81de64da42", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX64, "b2df22134258d3566c964020e59188099e072f766fcf49e63eddcd81de64da42b5f45750d48f3d191a2e806ba027d624", NULL },
    END_VECTOR
};

ccsymmetric_test_vector rc2_cfb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "b2", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "b2df22134258d3566c964020e5918809", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "b2df22134258d3566c964020e59188099e072f766fcf49e63eddcd81de64da42b5", NULL },
    END_VECTOR
};

ccsymmetric_test_vector rc2_ctr_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "b2", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "b2df22134258d35691a96187855ad58d", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "b2df22134258d35691a96187855ad58d2d28e7df3551b65866ff40405cdc572b92", NULL },
    END_VECTOR
};

ccsymmetric_test_vector rc2_ofb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "b2", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "b2df22134258d3566c964020e5918809", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "b2df22134258d3566c964020e59188099e072f766fcf49e63eddcd81de64da42b5", NULL },
    END_VECTOR
};

ccsymmetric_test_vector rc2_cfb8_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "b2", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "b211de15aecd4e5331065b799f763d09", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "b211de15aecd4e5331065b799f763d098d3d8beefa35fc83cc6a4ed9ad1a08a7ac", NULL },
    END_VECTOR
};



ccsymmetric_test_vector blowfish_ecb_vectors[] = {
    { keystr128, NULL, NULL, NULL, NULL, zeroX16, "b995f24ddfe87bf0b995f24ddfe87bf0", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX32, "b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0", NULL },
    { keystr128, NULL, NULL, NULL, NULL, zeroX64, "b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0b995f24ddfe87bf0", NULL },
    END_VECTOR
};

ccsymmetric_test_vector blowfish_cbc_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "16c5049f3e3ad3562cac4e5b98dbed08", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX32, "16c5049f3e3ad3562cac4e5b98dbed08e901bc04d1b2d84f00e215b262c917dd", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX64, "16c5049f3e3ad3562cac4e5b98dbed08e901bc04d1b2d84f00e215b262c917ddfbf81ba83106bb09f2ae30aeffa6a91f", NULL },
    END_VECTOR
};

ccsymmetric_test_vector blowfish_cfb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "16", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "16c5049f3e3ad3562cac4e5b98dbed08", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "16c5049f3e3ad3562cac4e5b98dbed08e901bc04d1b2d84f00e215b262c917ddfb", NULL },
    END_VECTOR
};

ccsymmetric_test_vector blowfish_ctr_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "16", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "16c5049f3e3ad356a3ed4d1f897c44f5", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "16c5049f3e3ad356a3ed4d1f897c44f5189eef6feafcd0c959f52c8eaa2a7c7fa8", NULL },
    END_VECTOR
};

ccsymmetric_test_vector blowfish_ofb_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "16", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "16c5049f3e3ad3562cac4e5b98dbed08", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "16c5049f3e3ad3562cac4e5b98dbed08e901bc04d1b2d84f00e215b262c917ddfb", NULL },
    END_VECTOR
};

ccsymmetric_test_vector blowfish_cfb8_vectors[] = {
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX1, "16", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX16, "16ac9f958a247fb9aa5058b45b86eb80", NULL },
    { keystr128, NULL, ivstr64, ivstr64, NULL, zeroX33, "16ac9f958a247fb9aa5058b45b86eb80013303a7eb91747a3387b9dbc18787b497", NULL },
    END_VECTOR
};



ccsymmetric_test_vector *vectors[cc_NCiphers][cc_NModes] = {
    { aes_ecb_vectors, aes_cbc_vectors, aes_cfb_vectors, aes_ctr_vectors, aes_ofb_vectors, aes_xts_vectors, aes_cfb8_vectors, aes_gcm_vectors, aes_ccm_vectors }, // AES
    { des_ecb_vectors, des_cbc_vectors, des_cfb_vectors, des_ctr_vectors, des_ofb_vectors, NULL, des_cfb8_vectors, NULL }, // DES
    { des3_ecb_vectors, des3_cbc_vectors, des3_cfb_vectors, des3_ctr_vectors, des3_ofb_vectors, NULL, des3_cfb8_vectors, NULL }, // 3DES
    { cast_ecb_vectors, cast_cbc_vectors, cast_cfb_vectors, cast_ctr_vectors, cast_ofb_vectors, NULL, cast_cfb8_vectors, NULL }, // CAST
    { rc2_ecb_vectors, rc2_cbc_vectors, rc2_cfb_vectors, rc2_ctr_vectors, rc2_ofb_vectors, NULL, rc2_cfb8_vectors, NULL }, // RC2
    { blowfish_ecb_vectors, blowfish_cbc_vectors, blowfish_cfb_vectors, blowfish_ctr_vectors, blowfish_ofb_vectors, NULL, blowfish_cfb8_vectors, NULL }, // Blowfish
};


static cc_status
ccsymmetric_tests(duplex_cryptor cryptor, ccsymmetric_test_vector test) {
    byteBuffer key = hexStringToBytes(test.keyStr);
    byteBuffer twk = hexStringToBytes(test.twkStr);
    byteBuffer init_iv = hexStringToBytes(test.init_ivStr);
    byteBuffer block_iv = hexStringToBytes(test.block_ivStr);
    byteBuffer adata = hexStringToBytes(test.aDataStr);
    byteBuffer pt = hexStringToBytes(test.ptStr);
    byteBuffer ct = hexStringToBytes(test.ctStr);
    byteBuffer tag = hexStringToBytes(test.tagStr);
    size_t len = pt->len;
    cc_status status = 1;
    
    
    cc_ciphermode_descriptor_s encrypt_desc;
    cc_ciphermode_descriptor_s decrypt_desc;
    
    encrypt_desc.cipher = decrypt_desc.cipher = cryptor->cipher;
    encrypt_desc.mode = decrypt_desc.mode = cryptor->mode;
    encrypt_desc.direction = cc_Encrypt;
    decrypt_desc.direction = cc_Decrypt;
    encrypt_desc.ciphermode = cryptor->encrypt_ciphermode;
    decrypt_desc.ciphermode = cryptor->decrypt_ciphermode;
    
    MAKE_GENERIC_MODE_CONTEXT(encrypt_ctx, &encrypt_desc);
    MAKE_GENERIC_MODE_CONTEXT(decrypt_ctx, &decrypt_desc);

    if(verbose) report_cipher_mode(cryptor);
    
    switch(cryptor->mode) {
        case cc_ModeECB:
        case cc_ModeCBC:
        case cc_ModeCFB:
        case cc_ModeCTR:
        case cc_ModeOFB:
        case cc_ModeCFB8:
            ok_or_fail((cc_symmetric_setup(&encrypt_desc, key->bytes, key->len, init_iv->bytes, encrypt_ctx) == 0), "cipher-mode is initted");
            ok_or_fail((cc_symmetric_setup(&decrypt_desc, key->bytes, key->len, init_iv->bytes, decrypt_ctx) == 0), "cipher-mode is initted");
            break;
        case cc_ModeXTS:
            ok_or_fail((cc_symmetric_setup_tweaked(&encrypt_desc, key->bytes, key->len, twk->bytes, init_iv->bytes, encrypt_ctx) == 0), "cipher-mode is initted");
            ok_or_fail((cc_symmetric_setup_tweaked(&decrypt_desc, key->bytes, key->len, twk->bytes, init_iv->bytes, decrypt_ctx) == 0), "cipher-mode is initted");
            break;
        case cc_ModeCCM:
        case cc_ModeGCM:
            ok_or_fail((cc_symmetric_setup_authenticated(&encrypt_desc, key->bytes, key->len, init_iv->bytes, init_iv->len, adata->bytes, adata->len, len, tag->len, encrypt_ctx) == 0), "cipher-mode is initted");
            ok_or_fail((cc_symmetric_setup_authenticated(&decrypt_desc, key->bytes, key->len, init_iv->bytes, init_iv->len, adata->bytes, adata->len, len, tag->len, decrypt_ctx) == 0), "cipher-mode is initted");
            break;
        default:
            break;
    }
    
    uint8_t in[len], out[len];
    cc_symmetric_crypt((cc_symmetric_context_p) encrypt_ctx, block_iv->bytes, pt->bytes, out, len);
    cc_symmetric_crypt((cc_symmetric_context_p) decrypt_ctx, block_iv->bytes, out, in, len);
    
    if(test.ctStr) {
        ok_memcmp_or_fail(out, ct->bytes, len, "ciphertext as expected");
    } else if(verbose) {
        byteBuffer result = bytesToBytes(out, len);
        diag("Round Trip Results\n");
        printByteBufferAsCharAssignment(pt, "pt");
        printByteBufferAsCharAssignment(result, "ct");
        free(result);
        return 1;
    }
    ok_memcmp_or_fail(in, pt->bytes, len, "plaintext as expected");
    
    if ((cryptor->mode == cc_ModeGCM)
        || (cryptor->mode == cc_ModeCCM)){
        size_t len = tag->len;
        char encrypt_returned_tag[len], decrypt_returned_tag[len];
        cc_zero(len,encrypt_returned_tag);
        cc_zero(len, decrypt_returned_tag);
        cc_symmetric_authenticated_finalize((cc_symmetric_context_p) encrypt_ctx, encrypt_returned_tag, len);
        cc_symmetric_authenticated_finalize((cc_symmetric_context_p) decrypt_ctx, decrypt_returned_tag, len);
        ok_memcmp_or_fail(encrypt_returned_tag, decrypt_returned_tag, len, "encrypt and decrypt tags match");
        if(test.tagStr) {
            ok_memcmp_or_fail(encrypt_returned_tag, tag->bytes, len, "computed and expected tags match");
        } else {
            byteBuffer result = bytesToBytes(encrypt_returned_tag, len);
            diag("Round Trip Tags\n");
            printByteBufferAsCharAssignment(result, "tagStr");
            free(result);
        }
    }
    
    free(key);
    free(init_iv);
    free(block_iv);
    free(pt);
    if(ct) free(ct);
    return status;
}


static int
run_symmetric_vectors(duplex_cryptor cryptor) {
    ccsymmetric_test_vector *run_vector = vectors[cryptor->cipher][cryptor->mode];
    for(int i=0; run_vector[i].keyStr != NULL; i++) {
        ccsymmetric_test_vector test = run_vector[i];
        ok_or_fail(ccsymmetric_tests(cryptor, test), "Test Vector Passed");
    }
    return 1;
}

int test_mode(ciphermode_t encrypt_ciphermode, ciphermode_t decrypt_ciphermode, cc_cipher_select cipher, cc_mode_select mode) {
    duplex_cryptor_s cryptor;
    
    cryptor.cipher = cipher;
    cryptor.mode = mode;
    cryptor.encrypt_ciphermode = encrypt_ciphermode;
    cryptor.decrypt_ciphermode = decrypt_ciphermode;
    ok_or_fail(run_symmetric_vectors(&cryptor), "Cipher-Mode Test");
    return 1;
}

