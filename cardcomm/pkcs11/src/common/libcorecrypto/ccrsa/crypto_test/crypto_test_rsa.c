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

// static int verbose = 1;

#if (CCRSA == 0)
entryPoint(ccrsa,"ccrsa")
#else
#include <corecrypto/ccrsa.h>
#include <corecrypto/ccrsa_priv.h>
#include <corecrypto/ccrsa_test.h>
#import <corecrypto/ccrng_test.h>
#import <corecrypto/ccrng_sequence.h>
#import <corecrypto/ccrng_rsafips_test.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#include "crypto_test_rsa.h"
#include "crypto_test_rsapss.h"

#define RSA_STD_KEYGEN_STRESS 5
#define RSA_FIPS_KEYGEN_STRESS 5

static int saneKeySize(ccrsa_full_ctx_t fk, int public) {
    size_t keySize;
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);

    if(public) keySize = ccrsa_pubkeylength(pubk);
    else keySize = ccrsa_privkeylength(fk);
    return (keySize < 512 || keySize > 4097);
}

static int roundTripCrypt(ccrsa_full_ctx_t fk) {
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(fk);

    size_t keySizeUnits = ccrsa_priv_n(fk);
    cc_unit clear[keySizeUnits], cipher[keySizeUnits], decrypted[keySizeUnits];
    ccn_zero(keySizeUnits, clear);
    clear[0]=2; // Non-null message
    is(ccrsa_pub_crypt(pubk, cipher, clear),0,"Pub crypt");
    is(ccrsa_priv_crypt(privk, decrypted, cipher),0,"Priv crypt");
    ok(ccn_cmp(keySizeUnits, decrypted, clear) == 0, "Results are what we started with");
    return ccn_cmp(keySizeUnits, decrypted, clear);
}

static int oaep_decrypt_error_test(ccrsa_full_ctx_t fk) {
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    size_t keySizeBytes = (ccrsa_pubkeylength(fk)+7)/8;
    cc_size n=ccrsa_ctx_n(pubk);
    cc_unit tmp_u[n];
    uint8_t tmp[keySizeBytes];
    byteBuffer decryptedData = mallocByteBuffer(keySizeBytes);
    size_t decryptedData_len_bkp = decryptedData->len;
    int status = 0,expected_status;
    uint32_t test_status = 0;
    int test_index=0;
    bool run_test=true;
    struct ccrng_sequence_state rng_seq;

    // Arbitrary choices for the test
    struct ccdigest_info di_big = {
        .output_size = keySizeBytes/2+1,
    };
    const struct ccdigest_info *di_test;
    const struct ccdigest_info *di=ccsha256_di();
    const size_t PS_len=10;
    cc_assert(2*di->output_size+PS_len+2<keySizeBytes);
    const size_t M_len=keySizeBytes-(2*di->output_size+PS_len+2);
    uint8_t M[M_len];
    uint8_t *ptr;
    uint8_t seed[di->output_size];
    uint8_t seed_mask[di->output_size];
    size_t maskedDB_len=keySizeBytes-di->output_size-1;
    memset(seed,0xff,sizeof(seed));
    char* test_description[]=
    {
        "oaep_decrypt: Sanity",
        "oaep_decrypt: Y not zero",
        "oaep_decrypt: No Separator",
        "oaep_decrypt: Padding is not zero",
        "oaep_decrypt: lHash does not match",
        "oaep_decrypt: seedHash does not match",
        "oaep_decrypt: key vs hash length error",
        "oaep_decrypt: output is too small",
        "oaep_decrypt: ciphertext is too small",
    };

    // Message is all 0
    memset(M,0,M_len);

    // Seed is all 0xFF
    memset(seed,0xff,sizeof(seed));


    while(run_test)
    {
        keySizeBytes = (ccrsa_pubkeylength(fk)+7)/8;
        di_test=di;
        decryptedData->len=decryptedData_len_bkp;

        // Encode message, in little endian as an array of cc_unit
        ccrng_sequence_init(&rng_seq, sizeof(seed), seed);
        status=ccrsa_oaep_encode(di,(struct ccrng_state*)&rng_seq,keySizeBytes, tmp_u, M_len, M);
        cc_assert(status==0); (void) status; // Analyzer warning in release mode

        switch(test_index)
        {
            case 0:
                // Keep the encoded message good for sanity
                expected_status=0; // Pass
                break;
            case 1:
                // Y is not zero
                ptr=(uint8_t *)tmp_u;
                ptr[keySizeBytes-1]=0x01;
                expected_status=CCRSA_DECRYPTION_ERROR;
                break;
            case 2:
                // No separator 0x01
                ccn_swap(n,tmp_u);
                ptr=(uint8_t*)tmp_u + (ccn_sizeof_n(n)-keySizeBytes);
                ptr[keySizeBytes-M_len-1]^=1;
                ccmgf(di,di->output_size,seed_mask,maskedDB_len,&ptr[1+di->output_size]); // Recompute seed mask
                cc_xor(di->output_size,&ptr[1],seed,seed_mask); // Overwrite maskedSeed
                ccn_swap(n,tmp_u);
                expected_status=CCRSA_DECRYPTION_ERROR;
                break;
            case 3:
                // Padding is not zero
                ccn_swap(n,tmp_u);
                ptr=(uint8_t*)tmp_u + (ccn_sizeof_n(n)-keySizeBytes);
                ptr[keySizeBytes-M_len-2]^=2;
                ccmgf(di,di->output_size,seed_mask,maskedDB_len,&ptr[1+di->output_size]); // Recompute seed mask
                cc_xor(di->output_size,&ptr[1],seed,seed_mask); // Overwrite maskedSeed
                ccn_swap(n,tmp_u);
                expected_status=CCRSA_DECRYPTION_ERROR;
                break;
            case 4:
                // lHash does not match
                ccn_swap(n,tmp_u);
                ptr=(uint8_t*)tmp_u + (ccn_sizeof_n(n)-keySizeBytes);
                ptr[1+di->output_size]^=1;
                ccmgf(di,di->output_size,seed_mask,maskedDB_len,&ptr[1+di->output_size]); // Recompute seed mask
                cc_xor(di->output_size,&ptr[1],seed,seed_mask); // Overwrite maskedSeed
                ccn_swap(n,tmp_u);
                expected_status=CCRSA_DECRYPTION_ERROR;
                break;
            case 5:
                // seed does not match
                ccn_swap(n,tmp_u);
                ptr=(uint8_t*)tmp_u + (ccn_sizeof_n(n)-keySizeBytes);
                ptr[2]^=1;
                ccn_swap(n,tmp_u);
                expected_status=CCRSA_DECRYPTION_ERROR;
                break;
            case 6:
                // Padding test fails
                di_test=&di_big;
                expected_status=CCRSA_INVALID_CONFIG;
                break;
            case 7:
                // Output buffer is too small
                decryptedData->len=keySizeBytes-2*di->output_size-3;
                expected_status=CCRSA_INVALID_INPUT;
                break;
            case 8:
                // Ciphertext is too small
                keySizeBytes-=1;
                expected_status=CCRSA_INVALID_INPUT;
                break;
            default:
                run_test=false;
                expected_status=1;
                break;
        }

        if (run_test)
        {
            cc_assert(test_index<(int)(sizeof(test_description)/sizeof(test_description[0])));

            // Encrypt
            is(ccrsa_pub_crypt(pubk, tmp_u, tmp_u),0, "ccrsa_pub_crypt");

            // we need to write leading zeroes if necessary
            ccn_write_uint_padded(n, tmp_u, keySizeBytes, tmp);

            // Try to decrypt, expected to fail
            ok((status = ccrsa_decrypt_oaep(fk,di_test,
                                            &decryptedData->len, decryptedData->bytes,
                                            keySizeBytes,  tmp,
                                            0, NULL)) == expected_status,
                            test_description[test_index]);

            // Check return value
            if  (status==expected_status)   // Expect failures
            {
                test_status|=(1<<test_index);
            }
            test_index++;
        }

    }
    free(decryptedData);
    if (((1<<test_index)-1)==test_status)
    {
        return 0; // All tests passed
    }
    return -1;
}


static int pkcs1v15_decrypt_error_test(ccrsa_full_ctx_t fk) {
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    size_t keySizeBytes = (ccrsa_pubkeylength(fk)+7)/8;
    cc_size n=ccrsa_ctx_n(pubk);
    uint8_t tmp[keySizeBytes];
    cc_unit tmp_u[n];
    byteBuffer decryptedData = mallocByteBuffer(keySizeBytes);
    int status = 0,expected_status;
    uint32_t test_status = 0;
    int test_index=0;
    bool run_test=true;
    
    char* test_description[]=
    {
        "pkcs1v15_decrypt: Sanity",
        "pkcs1v15_decrypt: Padding first byte",
        "pkcs1v15_decrypt: Padding second byte",
        "pkcs1v15_decrypt: No separator",
        "pkcs1v15_decrypt: Padding length",
    };
    
    while(run_test)
    {
        switch(test_index)
        {
            // Expect format is 00:02:PS:00:Msg
            // PS has to be great or equal to 8 bytes.
            case 0:
                // Handcraft GOOD message for sanity
                tmp[0]=0x00;  // Prefix byte 1
                tmp[1]=0x02;  // Prefix byte 2
                memset(&tmp[2],0xff,keySizeBytes-2);
                tmp[10]=0x00; // Prefix separator
                expected_status=0; // Pass
                break;
            case 1:
                // Handcraft message
                tmp[0]=0x7f;  // Prefix byte 1: Not null => must fail
                tmp[1]=0x02;  // Prefix byte 2
                memset(&tmp[2],0xff,keySizeBytes-2);
                tmp[10]=0x00; // Prefix separator
                expected_status=-1; // Pass
                break;
            case 2:
                // Handcraft message
                tmp[0]=0x00;  // Prefix byte 1
                tmp[1]=0xff;  // Prefix byte 2: Not 0x02 => must fail
                memset(&tmp[2],0xff,keySizeBytes-2);
                tmp[10]=0x00; // Prefix separator
                expected_status=-1;
                break;
            case 3:
                // Handcraft message
                tmp[0]=0x00;  // Prefix byte 1
                tmp[1]=0x02;  // Prefix byte 2
                memset(&tmp[2],0xff,keySizeBytes-2);
                // No separator => must fail
                expected_status=-1;
                break;
            case 4:
                // Handcraft message
                tmp[0]=0x00;  // Prefix byte 1
                tmp[1]=0x02;  // Prefix byte 2
                memset(&tmp[2],0xff,keySizeBytes-2);
                tmp[9]=0x00; // Prefix separator
                expected_status=-1;
                break;
            default:
                run_test=false;
                expected_status=1;
                break;
        }
                
        if (run_test)
        {
            cc_assert(test_index<(int)(sizeof(test_description)/sizeof(test_description[0])));

            // Convert in uint
            ccn_zero(n, tmp_u);
            ccn_read_uint(n,tmp_u,keySizeBytes,tmp);
            
            // Encrypt
            is(ccrsa_pub_crypt(pubk, tmp_u, tmp_u),0,"ccrsa_pub_crypt");
            
            /* we need to write leading zeroes if necessary */
            ccn_write_uint_padded(n, tmp_u, keySizeBytes, tmp);
            
            // Try to decrypt, expected to fail
            decryptedData->len=keySizeBytes;
            ok((status = ccrsa_decrypt_eme_pkcs1v15(fk,
                                                    &decryptedData->len, decryptedData->bytes,
                                                    keySizeBytes,  tmp)) == expected_status,
                      test_description[test_index]);
            if  (status==expected_status)   // Expect failures
            {
                test_status|=(1<<test_index);
            }
            test_index++;
        }
        
    }
    free(decryptedData);
    if (((1<<test_index)-1)==test_status)
    {
        return 0; // All 5 tests passed
    }
    return -1;
}

static int wrapUnwrap(ccrsa_full_ctx_t fk, int padding, struct ccrng_state *rng) {
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    size_t keySizeBytes = ccrsa_privkeylength(fk)/8+64;
    byteBuffer keydata = hexStringToBytes("000102030405060708090a0b0c0d0e0f");
    byteBuffer decryptedKey = mallocByteBuffer(keySizeBytes);
    byteBuffer encryptedKey = mallocByteBuffer(keySizeBytes);
    int status = 1;

    switch(padding) {
        case PADDING_PKCS1: {
            ok((status = ccrsa_encrypt_eme_pkcs1v15(pubk, rng,
                                        &encryptedKey->len, encryptedKey->bytes,
                                        keydata->len, keydata->bytes)) == 0,
                        "Wrap Key Data with RSA Encryption");
            if(status) goto errout;
            ok((status = ccrsa_decrypt_eme_pkcs1v15(fk,
                                        &decryptedKey->len, decryptedKey->bytes,
                                        encryptedKey->len, encryptedKey->bytes)) == 0,
                        "Unwrap Key Data with RSA Encryption");
            if(status) goto errout;
        } break;
        case PADDING_OAEP: {
            ok((status = ccrsa_encrypt_oaep(pubk, ccsha1_di(), rng,
                                                    &encryptedKey->len, encryptedKey->bytes,
                                                    keydata->len, keydata->bytes, 0, NULL)) == 0,
               "Wrap Key Data with RSA Encryption");
            if(status) goto errout;
            ok((status = ccrsa_decrypt_oaep(fk, ccsha1_di(),
                                                    &decryptedKey->len, decryptedKey->bytes,
                                                    encryptedKey->len, encryptedKey->bytes, 0, NULL)) == 0,
               "Unwrap Key Data with RSA Encryption");
            if(status) goto errout;
        } break;
    }
    
    ok(bytesAreEqual(decryptedKey, keydata), "Round Trip wrap/unwrap");
    if(bytesAreEqual(decryptedKey, keydata)) status = 0;
errout:
    free(keydata);
    free(encryptedKey);
    free(decryptedKey);
    return status;
}

#define MAXKEYSPACE 512

static int sign_verify(ccrsa_full_ctx_t fk, int padding, struct ccrng_state *rng, const struct ccdigest_info *di)
{
    int status = 1;
    bool valid=false;
    byteBuffer signature = mallocByteBuffer(MAXKEYSPACE*2);
    byteBuffer hash = hexStringToBytes("000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f");
    
    hash->len = di->output_size;
    
    switch(padding) {
        case PADDING_PKCS1: {
            ok((status = ccrsa_sign_pkcs1v15(fk, di->oid.oid, hash->len, hash->bytes,
                                         &signature->len, signature->bytes)) == 0, "RSA Signing");
            if(status) goto errout;
            ok((status = ccrsa_verify_pkcs1v15(fk, di->oid.oid, hash->len, hash->bytes,
                                           signature->len, signature->bytes,
                                           &valid)) == 0, "RSA Verifying");
            if(status) goto errout;
        } break;
        case PADDING_OAEP: {
            ok((status = ccrsa_sign_oaep(fk, di, rng, hash->len, hash->bytes,
                                &signature->len, signature->bytes)) == 0, "RSA Signing");
            if(status) goto errout;
            ok((status = ccrsa_verify_oaep(fk, di, hash->len, hash->bytes,
                                  signature->len, signature->bytes,
                                  &valid)) == 0, "RSA Verifying");
            if(status) goto errout;
        } break;
        case PADDING_PSS: {
            size_t salt_len=hash->len;
            ok((status = ccrsa_sign_pss(fk, di, di, salt_len, rng, hash->len, hash->bytes, &signature->len, signature->bytes)) == 0, "RSA Signing");
            if(status)
                goto errout;
            
            ok((status = ccrsa_verify_pss(fk, di, di, hash->len, hash->bytes, signature->len, signature->bytes, salt_len, &valid)) == 0, "RSA Verifying");
            if(status)
                goto errout;
        } break;
    }
    ok(valid == true, "Signature verifies");
    if(!valid) goto errout;
    status = 0;
    
errout:
    free(signature);
    free(hash);
    return status;
}


#define TMPBUFSIZ 1024
static int export_import(ccrsa_full_ctx_t fk)
{
    int pubkeytest = 1;
    int privkeytest = 1;
    int status = 0;
    byteBuffer tmp=NULL;
    
    if(pubkeytest) {
        ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
        ccrsa_full_ctx_decl(ccn_sizeof(ccrsa_privkeylength(fk)), tmpkey);
        ccrsa_pub_ctx_t pubk2 = ccrsa_ctx_public(tmpkey);
        tmp = mallocByteBuffer(ccrsa_export_pub_size(pubk));
        
        // Public key test
        ok((status = ccrsa_export_pub(pubk, tmp->len, tmp->bytes)) == 0, "Exported Public Key");
        if(status) goto export_import_exit;
        ok((ccrsa_ctx_n(pubk2) = ccrsa_import_pub_n(tmp->len, tmp->bytes)) != 0, "Got Key N");;
        if(ccrsa_ctx_n(pubk2) == 0) goto export_import_exit;
        ok((status = ccrsa_import_pub(pubk2, tmp->len, tmp->bytes)) == 0, "Imported Public Key");
        if(status) goto export_import_exit;
        ok((status = saneKeySize(tmpkey, 1)) == 0, "Keysize is realistic");
        if(status) goto export_import_exit;
    }
    if(privkeytest) {
        ccrsa_full_ctx_decl(ccn_sizeof(ccrsa_privkeylength(fk)), key2);
        tmp = mallocByteBuffer(ccrsa_export_priv_size(fk));
        
        // Private key test
        ok((status = ccrsa_export_priv(fk, tmp->len, tmp->bytes)) == 0, "Exported Private Key");
        if(status) goto export_import_exit;
        ok((ccrsa_ctx_n(key2) = ccrsa_import_priv_n(tmp->len, tmp->bytes)) != 0, "Got Key N");;
        if(ccrsa_ctx_n(key2) == 0) goto export_import_exit;
        ok((status = ccrsa_import_priv(key2, tmp->len, tmp->bytes)) == 0, "Imported Private Key");
        if(status) goto export_import_exit;
        ok((status = saneKeySize(key2, 0)) == 0, "Keysize is realistic");
        if(status) goto export_import_exit;
        ok((status = roundTripCrypt(key2)) == 0, "Can round-trip re-imported key");
        if(status) goto export_import_exit;
    }
export_import_exit:
    if (tmp!=NULL) free(tmp);
    if (status==0)
    {
        return 0; // No error
    }
    return 1; // Error;
}


/* Nist CAVP vectors specifies the hash as strings - those are matching hashes implementations */
/* We picked the implementations that are on all platform, it does not matter since we are not testing the hash here */
#define di_SHA1 &ccsha1_eay_di
#define di_SHA224 &ccsha224_ltc_di
#define di_SHA256 &ccsha256_ltc_di
#define di_SHA384 &ccsha384_ltc_di
#define di_SHA512 &ccsha512_ltc_di

/* Nist CAVP vectors for verify specify the result as F (failed) or P (passed)
 those translate as true or false */

#define P true
#define F false

const struct ccrsa_verify_vector verify_vectors_pkcs1v15[]=
{
#include "../test_vectors/SigVer15.inc"
};


static int
test_verify_pkcs1v15_known_answer_test(void)
{
    uint32_t i;
    uint32_t nb_test_passed,nb_test;
    nb_test_passed=0;
    nb_test=0;
    // Run only tests for supported hash algos
    for(i=0;i<sizeof(verify_vectors_pkcs1v15)/sizeof(struct ccrsa_verify_vector);i++)
    {
        if (verify_vectors_pkcs1v15[i].di!=NULL)
        {
            // 1 bit is set to one when the test passed
            nb_test++;
            if (ccrsa_test_verify_pkcs1v15_vector(&verify_vectors_pkcs1v15[i])==0)
            {
              nb_test_passed+=1;
            }
        }
    }
    
    if ((nb_test_passed==nb_test) && (nb_test>0))
    {
        return 0;
    }
    return -1;
}


static int
test_rsa_sample(void)
{
    
    // Private-Key(1024 bit) (Big Endian)
    const uint8_t modulus[]={
        0xac,0x79,0xe8,0xb0,0xd2,0x11,0x64,0x9a,0x1c,0xe4,0x24,0xf6,0x3c,0xfe,
        0x7c,0x6a,0x3a,0x3e,0x0b,0x07,0xae,0xa9,0x79,0x6f,0x64,0x4e,0xf0,0x5c,0x4c,
        0xb4,0xa7,0x38,0xc9,0xde,0x4a,0x36,0x68,0x7f,0x98,0x05,0xe3,0x3c,0xf8,0xd6,
        0xd2,0xf1,0x9f,0xd9,0x88,0x9d,0xa7,0xcf,0x0d,0xe5,0x92,0x8d,0x2b,0x44,0x24,
        0xa3,0xa7,0x20,0xf4,0xd4,0xd7,0xe5,0xf8,0x07,0x24,0xd7,0xd2,0x32,0x2c,0x8f,
        0xcb,0xd8,0xf8,0xe0,0x97,0x69,0xcb,0xab,0x4c,0xfb,0xf3,0xa2,0xe5,0x43,0x8c,
        0xb1,0x9f,0xa6,0xac,0xe9,0x86,0x88,0x85,0x74,0xf2,0xb2,0xdc,0x87,0x56,0xf5,
        0x99,0x96,0x03,0x70,0xa2,0x5d,0x26,0x26,0x12,0x20,0x09,0x3e,0x5f,0xb0,0x3b,
        0xbd,0xee,0x19,0x9c,0x96,0xd8,0x82,0x22,0x91};
    const uint32_t publicExponent=65537; // (0x10001)
    const uint8_t privateExponent[]={
        0x03,0xe5,0xc9,0x5d,0x5d,0x91,0xe9,0x0d,0x16,0x84,0x0d,0x55,0xc7,0x31,0x15,
        0x0c,0xad,0x7e,0x43,0x6f,0x8c,0x01,0xe6,0x6d,0x9e,0xfd,0xad,0xae,0xd8,0x48,
        0xe8,0xd2,0x7e,0xb5,0x58,0x45,0xfc,0x7c,0x8d,0xa9,0xec,0x65,0xaf,0x55,0xe3,
        0x74,0x74,0x61,0x4d,0x16,0x0a,0xf9,0xc1,0xdd,0xa3,0x3f,0x2f,0x70,0x1d,0xc7,
        0xd8,0xfa,0x04,0xae,0x52,0x7d,0xe3,0x20,0xc6,0xb5,0x5b,0x6b,0xd7,0x0b,0x02,
        0x2a,0xcf,0x28,0xf4,0x34,0x7d,0x46,0x69,0x15,0xf0,0x95,0xd0,0x7b,0x9a,0xa4,
        0x24,0x9b,0x27,0x49,0x99,0x49,0x14,0x27,0xa9,0x95,0x89,0x6e,0xff,0x96,0x0c,
        0x02,0xb7,0x46,0xab,0x95,0x46,0x34,0x33,0xee,0xe1,0x1a,0x4c,0x3a,0x09,0x19,
        0xf3,0xda,0x2c,0x67,0x8e,0xcc,0x10,0x71};
    const uint8_t prime1[]={
        0x01,0xb5,0x09,0x1a,0x06,0xe7,0xfa,0x1f,0x6b,0x52,0x3f,0xe9,0x57,0x3c,0xd9,
        0xe0,0xdb,0x1a,0x32,0x05,0x0c,0xf8,0xba,0x84,0x55,0xc5,0x17,0x64,0xb6,0x02,
        0x4f,0xcf,0x30,0x07,0x3a,0x1c,0x13,0x14,0xfb,0x5f,0xf6,0xcf,0x4b,0xeb,0x4d,
        0x11,0x6e,0x78,0x93,0x2b,0x38,0xd9,0x8a,0x59,0x0f,0xba,0x57,0xb6,0xc5,0x50,
        0x20,0xf0,0xd5,0x23,0x55};
    const uint8_t prime2[]={
        0x65,0x07,0xcd,0x1e,0x84,0x0f,0x49,0x20,0xa8,0x61,0xff,0x94,0x00,0x2c,0x28,
        0xe9,0x5c,0x19,0xa1,0x11,0x16,0xf3,0xf2,0xbf,0x42,0x8c,0x03,0x66,0xeb,0x1e,
        0x5e,0xe6,0x28,0xb5,0xf5,0x03,0x2b,0x31,0x36,0x40,0xb7,0xd5,0xb6,0x68,0x8f,
        0x62,0xdf,0xca,0xe2,0x38,0xc4,0xac,0xb5,0x7f,0xf3,0xa8,0xc6,0x16,0x32,0xec,
        0x9f,0x50,0x7a,0x4d};
    const uint8_t exponent1[]={
        0x00,0xa2,0x5d,0x8b,0x49,0xdd,0x8d,0x53,0x76,0xef,0xcb,0xc6,0xc9,0x1e,0x56,
        0x63,0xef,0x82,0xbf,0xea,0x98,0x73,0x1f,0xf8,0x62,0x55,0x22,0xe7,0xcb,0xa6,
        0xf8,0x37,0xa5,0x44,0x4a,0x16,0x7c,0x10,0x63,0x83,0xb7,0x92,0x34,0x46,0x6b,
        0x0f,0x7a,0xd7,0x58,0xf5,0xc9,0xdd,0x28,0x45,0x06,0x4e,0xd8,0x9f,0x92,0x96,
        0xbe,0x66,0x3b,0x09,0x31};
    const uint8_t exponent2[]={
        0x38,0xf7,0xaf,0x27,0x97,0xdb,0x6e,0xa6,0xa5,0x8b,0xac,0xab,0x6d,0x75,0x79,
        0x14,0x2c,0xc4,0x9e,0xd7,0x9e,0x13,0xac,0x3b,0x40,0x70,0xe6,0xb2,0x2f,0xbd,
        0x8e,0x51,0x45,0x7f,0x64,0x4a,0x87,0x1e,0x56,0xb3,0x23,0x75,0xb4,0x47,0x3d,
        0x22,0xc9,0x82,0x03,0x11,0x73,0x84,0xd7,0x4a,0xf0,0xbf,0xa8,0x02,0x78,0x70,
        0x88,0x5c,0xbe,0xb9};
    const uint8_t coefficient[]={
        0x01,0xb1,0x42,0xc6,0xc3,0xa0,0x76,0x2c,0x68,0x0f,0x42,0xd1,0x7c,0xe3,0x63,
        0xc4,0xb9,0xb7,0x12,0x56,0xa7,0x59,0xe1,0x20,0xb0,0xd7,0xa4,0xb8,0xa0,0x20,
        0x75,0x99,0x8d,0xf3,0x66,0x4a,0x90,0x9e,0x0a,0xc9,0x93,0x23,0x9f,0xb5,0xbb,
        0x17,0xc1,0xf4,0xe5,0x2f,0x2a,0xfe,0xa3,0xd5,0x4b,0xf0,0xf3,0xec,0xd2,0x55,
        0xe3,0x24,0x7d,0x91,0x4e};
    
    byteBuffer digest=hexStringToBytes("2812fd163b700eaad52a82c2d330eb1d2b23c1db");
    byteBuffer oid=hexStringToBytes("06052b0e03021a");
    uint32_t status=0,test_step=0;
    byteBuffer encoded_message=hexStringToBytes("0001ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff003021300906052b0e03021a050004142812fd163b700eaad52a82c2d330eb1d2b23c1db");

    byteBuffer expected_sig=hexStringToBytes("203b16fdecf3989a60d161914b60c0459ff3f4925ca1298c1514f97a12086214647e7ff1162ea1b22e1a0133a60fcd9a6bd2efc91de7561c6c67e7b9f1b945cf51242c4169d84c29a1231decf292fe44b972090ebb057a10425f26962e755d76bee595e803d8f79b423af780a97b9d149f84d24a6623642e16ae013ec78f10c6");
    
    const cc_size n = ccn_nof_size(sizeof(modulus));
    ccrsa_full_ctx_decl(ccn_sizeof_n(n), fk);
    ccrsa_ctx_n(fk) = n;
    cc_unit tmp_u[n];
    bool valid=false;
    uint8_t result[sizeof(modulus)];
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(fk);
    
    // Set value
    CCZP_N(ccrsa_ctx_private_zp(privk)) = ccn_nof_size(sizeof(prime1));
    CCZP_N(ccrsa_ctx_private_zq(privk)) = ccn_nof_size(sizeof(prime2));
    CCZP_N(ccrsa_ctx_zm(pubk)) = n;
    
    ccn_seti(n, ccrsa_ctx_e(pubk), publicExponent);
    ccn_read_uint(n,CCZP_PRIME(ccrsa_ctx_zm(pubk)),sizeof(modulus),modulus);
    ccn_read_uint(n,ccrsa_ctx_d(fk),sizeof(privateExponent),privateExponent);
    ccn_read_uint(cczp_n(ccrsa_ctx_private_zp(privk)),CCZP_PRIME(ccrsa_ctx_private_zp(privk)),sizeof(prime1),prime1);
    ccn_read_uint(cczp_n(ccrsa_ctx_private_zp(privk)),ccrsa_ctx_private_dp(privk),sizeof(exponent1),exponent1);
    ccn_read_uint(cczp_n(ccrsa_ctx_private_zq(privk)),CCZP_PRIME(ccrsa_ctx_private_zq(privk)),sizeof(prime2),prime2);
    ccn_read_uint(cczp_n(ccrsa_ctx_private_zq(privk)),ccrsa_ctx_private_dq(privk),sizeof(exponent2),exponent2);
    ccn_read_uint(cczp_n(ccrsa_ctx_private_zp(privk)),ccrsa_ctx_private_qinv(privk),sizeof(coefficient),coefficient);
    
    // Perform initialization
    cczp_init(ccrsa_ctx_private_zp(privk));
    cczp_init(ccrsa_ctx_private_zq(privk));
    cczp_init(ccrsa_ctx_zm(pubk));
    
    // Convert in uint
    ccn_zero(n, tmp_u);
    ccn_read_uint(n,tmp_u,encoded_message->len,encoded_message->bytes);


    // "Sign"
    if (0==ccrsa_priv_crypt(privk, tmp_u, tmp_u))
    {
        status|=1<<test_step;
    }
    test_step++;
    
    // Export as byte array
    ccn_write_uint_padded(n, tmp_u, sizeof(result), result);
    if (0==memcmp(result,expected_sig->bytes,expected_sig->len))
    {
        status|=1<<test_step;
    }
    test_step++;
    
    if (0==ccrsa_verify_pkcs1v15(ccrsa_ctx_public(fk), oid->bytes,
                                  digest->len, digest->bytes,
                                  expected_sig->len,expected_sig->bytes,
                                  &valid) && (valid==true))
     {
         status|=1<<test_step;
     }
     test_step++;
    
    free(expected_sig);
    free(encoded_message);
    free(digest);
    free(oid);
    
    // Check the final status
    if (((1<<test_step)-1) == status)
    {
        return 0;
    }
    return -1;
}

static int
test_rsa_invalid_key(void)
{
    const int keysize=1536;
    ccrsa_full_ctx_decl(ccn_sizeof(keysize), full_key);
    uint8_t e[1]={0x3};
    int status = 1;
    struct ccrng_state *rng = global_test_rng;

    // Generate a key
    is(ccrsa_generate_fips186_key(keysize, full_key, 1, e, rng, rng),0, "RSA Key generation");

    // Change key so that exponents are 1:

    // Public exponent
    cc_unit *pub_exp = ccrsa_ctx_e(full_key);
    cc_size n = ccrsa_ctx_n(full_key);
    ccn_seti(n,pub_exp,1);

    // Private exponents
    ccrsa_priv_ctx_t priv_key=ccrsa_ctx_private(full_key);
    cczp_t zp=ccrsa_ctx_private_zp(priv_key);
    cczp_t zq=ccrsa_ctx_private_zq(priv_key);
    ccn_seti(cczp_n(zp),ccrsa_ctx_private_dp(priv_key),1);
    ccn_seti(cczp_n(zq),ccrsa_ctx_private_dq(priv_key),1);

    // =========================================================================
    // Test public key operations
    // =========================================================================
    cc_unit unit_tmp[ccrsa_ctx_n(full_key)];
    size_t modulus_size=CC_BITLEN_TO_BYTELEN(keysize);
    uint8_t *byte_tmp=(uint8_t*)unit_tmp;
    size_t output_size=modulus_size;
    bool valid;
    const struct ccdigest_info* di=ccsha256_di();

    // Encryptions
    is(ccrsa_pub_crypt(full_key,unit_tmp,unit_tmp),CCRSA_KEY_ERROR,"Key ok");
    is(ccrsa_encrypt_eme_pkcs1v15(full_key,rng,&output_size,byte_tmp,modulus_size-16,byte_tmp),CCRSA_KEY_ERROR,"Key ok");
    output_size=modulus_size;
    is(ccrsa_encrypt_oaep(full_key,di,rng,&output_size,byte_tmp,modulus_size-(2*di->output_size+2),byte_tmp,0,NULL),CCRSA_KEY_ERROR,"Key ok");

    // Verifications
    valid=true;
    is(ccrsa_verify_oaep(full_key,di,di->output_size, byte_tmp,modulus_size,byte_tmp,&valid),CCRSA_KEY_ERROR,"Key ok");
    is(valid,false,"Fail close");

    valid=true;
    is(ccrsa_verify_pkcs1v15(full_key,di->oid.oid,di->output_size, byte_tmp,modulus_size,byte_tmp,&valid),CCRSA_KEY_ERROR,"Key ok");
    is(valid,false,"Fail close");

    valid=true;
    is(ccrsa_verify_pss(full_key,di,di,di->output_size, byte_tmp,modulus_size,byte_tmp,20,&valid),CCRSA_KEY_ERROR,"Key ok");
    is(valid,false,"Fail close");

    // =========================================================================
    // Test private key operations
    // =========================================================================

    // Decryptions
    is(ccrsa_priv_crypt(priv_key,unit_tmp,unit_tmp),CCRSA_KEY_ERROR,"Key ok");
    is(ccrsa_decrypt_eme_pkcs1v15(full_key,&output_size,byte_tmp,modulus_size,byte_tmp),CCRSA_KEY_ERROR,"Key ok");
    output_size=modulus_size;
    is(ccrsa_decrypt_oaep(full_key,di,&output_size,byte_tmp,modulus_size,byte_tmp,0,NULL),CCRSA_KEY_ERROR,"Key ok");

    // Signature
    is(ccrsa_sign_oaep(full_key,di,rng,di->output_size, byte_tmp,&modulus_size,byte_tmp),CCRSA_KEY_ERROR,"Key ok");
    is(ccrsa_sign_pkcs1v15(full_key,di->oid.oid,di->output_size, byte_tmp,&modulus_size,byte_tmp),CCRSA_KEY_ERROR,"Key ok");
    is(ccrsa_sign_pss(full_key,di,di,20,rng,di->output_size, byte_tmp,&modulus_size,byte_tmp),CCRSA_KEY_ERROR,"Key ok");

    status=0;
    return status;
}

static int
RSARoundTripTests(ccrsa_full_ctx_t fk)
{
    int status = 1;
    struct ccrng_state *rng = global_test_rng;
    ok((status = saneKeySize(fk, 1)) == 0, "Keysize is realistic");
    ok((status = saneKeySize(fk, 0)) == 0, "Keysize is realistic");
    ok((status = roundTripCrypt(fk)) == 0, "Can perform round-trip encryption");
    ok((status = wrapUnwrap(fk, PADDING_PKCS1, rng)) == 0, "Can perform round-trip PKCS1 wrap/unwrap");
    ok((status = oaep_decrypt_error_test(fk)) == 0, "Can check invalid ciphertext padding");
    ok((status = pkcs1v15_decrypt_error_test(fk)) == 0, "Can check invalid ciphertext padding");
    ok((status = wrapUnwrap(fk, PADDING_OAEP, rng)) == 0, "Can perform round-trip OAEP wrap/unwrap");
    ok((status = sign_verify(fk, PADDING_PKCS1, rng, ccsha1_di())) == 0, "Can perform round-trip PKCS1Padding sign/verify");
    ok((status = sign_verify(fk, PADDING_OAEP, rng, ccsha1_di())) == 0, "Can perform round-trip OAEP sign/verify");
    ok((status = sign_verify(fk, PADDING_PSS, rng, ccsha1_di())) == 0, "Can perform round-trip PSS sign/verify");
    ok((status = export_import(fk)) == 0, "Can perform round-trip import/export");
    return status;    
}

static int
RSAStdGenTest(size_t keysize, uint32_t exponent)
{
    ccrsa_full_ctx_decl(ccn_sizeof(keysize), full_key);
    uint8_t e4[4];
    for(int i=0; i<4; i++) e4[3-i] = ((exponent >> (i*8)) & 0x000000ff);
    int status = 1;

    struct ccrng_state *rng = global_test_rng;
    ok((ccrsa_generate_key(keysize, full_key, 4, e4, rng)==0), "RSA Key generation");
    ok((ccn_bitlen(ccrsa_ctx_n(full_key),ccrsa_ctx_m(full_key))==keysize), "RSA expected keysize");
    ok((status = RSARoundTripTests(full_key)) == 0, "RSA Round-Trip Key Tests");
    return status;
}

/* Generation of random keys */
static int
RSAFIPS_Gen_Test(size_t keysize, uint32_t exponent)
{
    ccrsa_full_ctx_decl(ccn_sizeof(keysize), full_key);
    uint8_t e4[4];
    for(int i=0; i<4; i++) e4[3-i] = ((exponent >> (i*8)) & 0x000000ff);
    int status = 1;
    struct ccrng_state *rng = global_test_rng;
    is((status = ccrsa_generate_fips186_key(keysize, full_key, 4, e4, rng, rng)),0, "RSA FIPS Key generation");
    if (status) return 1;
    is(ccn_bitlen(ccrsa_ctx_n(full_key),ccrsa_ctx_m(full_key)),keysize, "RSA FIPS expected keysize");
    is((status = RSARoundTripTests(full_key)),0, "RSA FIPS Round-Trip Key Tests");
    if (status) return 1;
    return 0;
}

/* Known Answer Tests */
static int
RSAFIPS_KAT_Test(  char *estr,
                   char *xp1str, char *xp2str, char *xpstr,
                   char *xq1str, char *xq2str, char *xqstr,
                   char *pstr, char *qstr, char *mstr, char *dstr)
{
    int verbose = 0;
    byteBuffer e = hexStringToBytes(estr);
    ccnBuffer xp1 = hexStringToCcn(xp1str);
    ccnBuffer xp2 = hexStringToCcn(xp2str);
    ccnBuffer xp = hexStringToCcn(xpstr);
    ccnBuffer xq1 = hexStringToCcn(xq1str);
    ccnBuffer xq2 = hexStringToCcn(xq2str);
    ccnBuffer xq = hexStringToCcn(xqstr);
    ccnBuffer expectedP = hexStringToCcn(pstr);
    ccnBuffer expectedQ = hexStringToCcn(qstr);
    ccnBuffer expectedM = hexStringToCcn(mstr);
    ccnBuffer expectedD = hexStringToCcn(dstr);
    int retvalP,retvalQ,retvalM,retvalD;
    int status = -1;
    cc_size nbits = ccn_bitlen(expectedM->len, expectedM->units);
    cc_size n = ccn_sizeof(nbits);

    ccrsa_full_ctx_decl(ccn_sizeof_n(n), full_key);
    ccrsa_priv_ctx_t privk;

    struct ccrng_rsafips_test_state rng1;
    struct ccrng_rsafips_test_state rng2;

    // Rngs
    ccrng_rsafips_test_init(&rng1,xp1->len,xp1->units,xp2->len,xp2->units,xp->len,xp->units);
    ccrng_rsafips_test_init(&rng2,xq1->len,xq1->units,xq2->len,xq2->units, xq->len, xq->units);

    // Computations
    ok((ccrsa_generate_fips186_key(nbits, full_key, e->len, e->bytes,
                                   (struct ccrng_state *)&rng1,
                                   (struct ccrng_state *)&rng2)==0), "RSA FIPS Key generation");
    privk = ccrsa_ctx_private(full_key);
    ok((ccn_bitlen(ccrsa_ctx_n(full_key),ccrsa_ctx_m(full_key))==nbits), "RSA FIPS expected keysize");

    // Verify results
    if (  (ccn_cmp(expectedQ->len,cczp_prime(ccrsa_ctx_private_zp(privk)), expectedQ->units)==0)
       && (ccn_cmp(expectedP->len,cczp_prime(ccrsa_ctx_private_zq(privk)), expectedP->units)==0)) {
        printf("Swapped P and Q\n");
    }

    ok((retvalP=ccn_cmp(expectedP->len,cczp_prime(ccrsa_ctx_private_zp(privk)), expectedP->units))==0, "p is built correctly");
    ok((retvalQ=ccn_cmp(expectedQ->len,cczp_prime(ccrsa_ctx_private_zq(privk)), expectedQ->units))==0, "q is built correctly");
    ok((retvalD=ccn_cmp(expectedD->len,ccrsa_ctx_d(full_key), expectedD->units))==0, "d is built correctly");
    ok((retvalM=ccn_cmp(expectedM->len,ccrsa_ctx_m(full_key), expectedM->units))==0, "m is built correctly");
    if(verbose) {
        struct ccn_buf retP={cczp_n(ccrsa_ctx_private_zp(privk)),CCZP_PRIME(ccrsa_ctx_private_zp(privk))};
        struct ccn_buf retQ={cczp_n(ccrsa_ctx_private_zq(privk)),CCZP_PRIME(ccrsa_ctx_private_zq(privk))};
        struct ccn_buf retD={ccrsa_ctx_n(full_key),ccrsa_ctx_d(full_key)};
        struct ccn_buf retM={ccrsa_ctx_n(full_key),ccrsa_ctx_m(full_key)};
        retvalP=1;
        retvalQ=1;
        retvalD=1;
        retvalM=1;
        if (retvalP) printf("P\nreturned: %s\nexpected: %s\n\n", ccnToHexString(&retP), ccnToHexString(expectedP));
        else printf("P is correct\n");
        if (retvalQ) printf("Q\nreturned: %s\nexpected: %s\n\n", ccnToHexString(&retQ), ccnToHexString(expectedQ));
        else printf("Q is correct\n");
        if (retvalD) printf("D\nreturned: %s\nexpected: %s\n\n", ccnToHexString(&retD), ccnToHexString(expectedD));
        else printf("D is correct\n");
        if (retvalM) printf("M\nreturned: %s\nexpected: %s\n\n", ccnToHexString(&retM), ccnToHexString(expectedM));
        else printf("M is correct\n");
    }
    if (retvalP || retvalQ || retvalD || retvalM) goto errout;

    ok((status = RSARoundTripTests(full_key)) == 0, "RSA Round-Trip Key Tests");
errout:
    free(e);free(xp1); free(xp2); free(xp); free(xq1); free(xq2); free(xq);
    free(expectedP); free(expectedQ); free(expectedM); free(expectedD);
    return status;
}

/* Negative tests */
static int
RSAFIPS_Negative_Test(void)
{
    cc_size keysize=2048;
    ccrsa_full_ctx_decl(ccn_sizeof(keysize), full_key);
    uint8_t e1[4]={0,0,0,1};
    uint8_t e2[4]={0,0,0,2};
    uint8_t e65537[4]={0,1,0,1};
    uint8_t seq[3]={0x00,0xff,0xba};
    struct ccrng_sequence_state rng_seq;
    struct ccrng_rsafips_test_state rng1;
    struct ccrng_rsafips_test_state rng2;

    struct ccrng_state *trng=global_test_rng;

    // The keysize for generation is smaller than the required size
    ok((ccrsa_generate_fips186_key(1023, full_key, 4, e65537,
                    trng,
                    trng)!=0), "Fail for small size");

    // Exponent is one while it must be >1
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e1,
                                   trng,
                                   trng)!=0), "Fail for even exponent size");

    // Exponent is even while it must be odd
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e2,
                    trng,
                    trng)!=0), "Fail for even exponent size");

    // Rng fails
    ccrng_sequence_init(&rng_seq, 0, NULL);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                    (struct ccrng_state *)&rng_seq,
                    trng)!=0), "Fail for bad RNG P");
    ccrng_sequence_init(&rng_seq, 0, NULL);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                    trng,
                    (struct ccrng_state *)&rng_seq)!=0), "Fail for bad RNG Q");

    // Rng single values
    ccrng_sequence_init(&rng_seq, 1, &seq[0]);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                   (struct ccrng_state *)&rng_seq,
                                   trng)!=0), "RNG returns all 0x00");
    ccrng_sequence_init(&rng_seq, 1, &seq[1]);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                   (struct ccrng_state *)&rng_seq,
                                   trng)!=0), "RNG returns all 0xff");

    // Check identical P&Q are rejected
    // First run dry to check the fix seed works.
    ccrng_sequence_init(&rng_seq, 1, &seq[2]);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                   (struct ccrng_state *)&rng_seq,
                                   trng)==0), "P is seeded with 0xba");

    // This time both P&Q will be identical
    ccrng_sequence_init(&rng_seq, 1, &seq[2]);
    ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                   (struct ccrng_state *)&rng_seq,
                                   (struct ccrng_state *)&rng_seq)!=0), "P&Q are seeded with 0xba");


    // P&Q and Xp,Xq delta too small, using a 2048 key.
    {
        //cc_size n=ccn_nof(nbits);
        ccnBuffer ccn_xp1 = hexStringToCcn("1747cbbd8b16c4dbc259e53b8a5c7db1b9f5");
        ccnBuffer ccn_xp2 = hexStringToCcn("18946d3a6f5e3e088446dd0e04aa62bc87e8");
        ccnBuffer ccn_xp = hexStringToCcn("6fccd146d52a5b4adda4a45a45f2eabb41da13fe6de477dad87d361d69c2cbb79640e76ac7c28abbce096dbf2e638b2053fc39c503bfcdc64d0ae2d7d818bb984896f115a76a8edad23e996b536856f808c717999dbb3955c4213b001a6d9722ce8d69e6b57e103a2f24765da3a2a413254b0c388172ad2f2cd623a9ce296c99");
        cc_unit xq1[ccn_xp1->len];
        cc_unit xq2[ccn_xp2->len];
        cc_unit xq[ccn_xp->len];
        cc_unit tmp[ccn_xp->len];

        // |Xp-Xq|=2^(nbits/2-100) => fail
        // 2.q1.q2 > 2.r1.r2 so that |p-q|>2^(nbits/2-100)
        ccn_zero(ccn_xp->len,tmp);
        ccn_set_bit(tmp,((keysize/2)-100),1);
        ccn_add(ccn_xp->len,xq,ccn_xp->units,tmp);
        ccn_set(ccn_xp1->len,xq1,ccn_xp1->units);        // xq1=xp1
        ccn_add1(ccn_xp2->len,xq2,ccn_xp2->units,((cc_unit)1)<<31); // xq2=xp1+2^31
        ccrng_rsafips_test_init(&rng1,ccn_xp1->len,ccn_xp1->units,ccn_xp2->len,ccn_xp2->units,ccn_xp->len,ccn_xp->units);
        ccrng_rsafips_test_init(&rng2,ccn_xp1->len,xq1,ccn_xp2->len,xq2, ccn_xp->len, xq);
        ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                       (struct ccrng_state *)&rng1,
                                       (struct ccrng_state *)&rng2)!=0), "RSA FIPS Key generation");
        ccrng_rsafips_test_init(&rng1,ccn_xp1->len,ccn_xp1->units,ccn_xp2->len,ccn_xp2->units,ccn_xp->len,ccn_xp->units);
        ccrng_rsafips_test_init(&rng2,ccn_xp1->len,xq1,ccn_xp2->len,xq2, ccn_xp->len, xq);
        ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                       (struct ccrng_state *)&rng2,
                                       (struct ccrng_state *)&rng1)!=0), "RSA FIPS Key generation");

        // |Xp-Xq|=2^(nbits/2-100)+2 => pass
        // 2.q1.q2 > 2.r1.r2 so that |p-q|>2^(nbits/2-100)
        ccn_zero(ccn_xp->len,tmp);
        ccn_set_bit(tmp,((keysize/2)-100),1);
        ccn_set_bit(tmp,(cc_unit)1,1);
        ccn_add(ccn_xp->len,xq,ccn_xp->units,tmp);
        ccn_zero(ccn_xp1->len,xq1);
        ccn_zero(ccn_xp2->len,xq2);
        ccrng_rsafips_test_init(&rng1,ccn_xp1->len,xq1,ccn_xp2->len,xq2,ccn_xp->len,ccn_xp->units);
        ccrng_rsafips_test_init(&rng2,ccn_xp1->len,ccn_xp1->units,ccn_xp2->len,ccn_xp2->units, ccn_xp->len, xq);
        ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                       (struct ccrng_state *)&rng1,
                                       (struct ccrng_state *)&rng2)!=0), "RSA FIPS Key generation");
        ccrng_rsafips_test_init(&rng1,ccn_xp1->len,xq1,ccn_xp2->len,xq2,ccn_xp->len,ccn_xp->units);
        ccrng_rsafips_test_init(&rng2,ccn_xp1->len,ccn_xp1->units,ccn_xp2->len,ccn_xp2->units, ccn_xp->len, xq);
        ok((ccrsa_generate_fips186_key(keysize, full_key, 4, e65537,
                                       (struct ccrng_state *)&rng2,
                                       (struct ccrng_state *)&rng1)!=0), "RSA FIPS Key generation");

        // Free
        free(ccn_xp1); free(ccn_xp2); free(ccn_xp);
    }

    // Close rng fd.
    return 0;
}


/* Known answer tests with construct function */
static int
RSAFIPS_Build_Test(cc_unit e,
                 char *xp1str, char *xp2str, char *xpstr,
                 char *xq1str, char *xq2str, char *xqstr,
                 char *pstr, char *qstr, char *mstr, char *dstr)
{
    int verbose = 0;
    ccnBuffer xp1 = hexStringToCcn(xp1str);
    ccnBuffer xp2 = hexStringToCcn(xp2str);
    ccnBuffer xp = hexStringToCcn(xpstr);
    ccnBuffer xq1 = hexStringToCcn(xq1str);
    ccnBuffer xq2 = hexStringToCcn(xq2str);
    ccnBuffer xq = hexStringToCcn(xqstr);
    ccnBuffer expectedP = hexStringToCcn(pstr);
    ccnBuffer expectedQ = hexStringToCcn(qstr);
    ccnBuffer expectedM = hexStringToCcn(mstr);
    ccnBuffer expectedD = hexStringToCcn(dstr);
    ccnBuffer retP = mallocCcnBuffer(MAXKEYSPACE);
    ccnBuffer retQ = mallocCcnBuffer(MAXKEYSPACE);
    ccnBuffer retM = mallocCcnBuffer(MAXKEYSPACE);
    ccnBuffer retD = mallocCcnBuffer(MAXKEYSPACE);
    int status = -1;
    cc_size n = xp->len + xq->len;
    cc_size nbits = ccn_bitsof_n(n);

    ccrsa_full_ctx_decl(ccn_sizeof(nbits), full_key);
    
    ccrsa_make_931_key(nbits, 1, &e, xp1->len, xp1->units, xp2->len, xp2->units, xp->len, xp->units,
                                    xq1->len, xq1->units, xq2->len, xq2->units, xq->len, xq->units,
                                    full_key,
                                    &retP->len, retP->units, &retQ->len, retQ->units,
                                    &retM->len, retM->units, &retD->len, retD->units);

    
    if(ccnAreEqual(retP, expectedQ) && ccnAreEqual(retQ, expectedP)) {
        ccnBuffer tmp = retP;
        retP = retQ;
        retQ = tmp;
        printf("Swapped P and Q\n");
    }
    
    ok(ccnAreEqual(retP, expectedP), "p is built correctly");
    ok(ccnAreEqual(retQ, expectedQ), "q is built correctly");
    ok(ccnAreEqual(retD, expectedD), "d is built correctly");
    ok(ccnAreEqual(retM, expectedM), "m is built correctly");
    if(verbose) {
        if(!ccnAreEqual(retP, expectedP)) printf("P\nreturned: %s\nexpected: %s\n\n", ccnToHexString(retP), ccnToHexString(expectedP));
        else printf("P is correct\n");
        if(!ccnAreEqual(retQ, expectedQ)) printf("Q\nreturned: %s\nexpected: %s\n\n", ccnToHexString(retQ), ccnToHexString(expectedQ));
        else printf("Q is correct\n");
        if(!ccnAreEqual(retD, expectedD)) printf("D\nreturned: %s\nexpected: %s\n\n", ccnToHexString(retD), ccnToHexString(expectedD));
        else printf("D is correct\n");
        if(!ccnAreEqual(retM, expectedM)) printf("M\nreturned: %s\nexpected: %s\n\n", ccnToHexString(retM), ccnToHexString(expectedM));
        else printf("M is correct\n");
    }
    if(!ccnAreEqual(retP, expectedP) || !ccnAreEqual(retQ, expectedQ) || !ccnAreEqual(retD, expectedD) || !ccnAreEqual(retM, expectedM))
        goto errout;
    
    ok((status = RSARoundTripTests(full_key)) == 0, "RSA Round-Trip Key Tests");
errout:
    free(retP); free(retQ); free(retD); free(retM);
    free(xp1); free(xp2); free(xp); free(xq1); free(xq2); free(xq); free(expectedP); free(expectedQ); free(expectedM); free(expectedD);
    return status;
}

// Der Public key for EFI
static int test_import_der_pub_key(void) {
    uint8_t foo[256];
    int i;
    for(i=0; i<256; i++) foo[i] = i;
    static uint8_t derbuf[] = {
        0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
        0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
        0x00, 0xa5, 0xcd, 0xd7, 0xaf, 0xeb, 0x44, 0xd6, 0xa2, 0xe4, 0xe4, 0x4d, 0xb8, 0xc8, 0xd5, 0x80,
        0x51, 0xc1, 0x12, 0xe1, 0xc2, 0x0c, 0xc6, 0x89, 0x29, 0x3a, 0xe9, 0x6d, 0x7e, 0x7d, 0x9d, 0xc3,
        0x5a, 0xaf, 0xce, 0xdc, 0x1e, 0x92, 0x28, 0xbe, 0x00, 0x34, 0x05, 0xc8, 0x19, 0x4f, 0x7c, 0x23,
        0x00, 0x3d, 0x7b, 0xdb, 0x80, 0xd1, 0x82, 0xb9, 0xca, 0xb4, 0xe4, 0x01, 0x43, 0xdc, 0x11, 0xd3,
        0x20, 0xec, 0xd8, 0x44, 0xd6, 0xa8, 0x7e, 0xa7, 0xa3, 0xc2, 0x85, 0x61, 0xff, 0xd4, 0x88, 0x8e,
        0x40, 0x4b, 0x1c, 0xe9, 0x2f, 0x5e, 0x48, 0x26, 0x46, 0x79, 0x65, 0xf4, 0x4f, 0x52, 0x04, 0x09,
        0x0b, 0x1a, 0x05, 0x27, 0x18, 0xe9, 0x22, 0x6d, 0x10, 0xa6, 0x4b, 0xe3, 0x7a, 0x4b, 0x32, 0x8d,
        0x65, 0xbf, 0x1c, 0x8d, 0x24, 0x9c, 0x12, 0xfe, 0xd3, 0xc9, 0xd6, 0x3a, 0xb2, 0xca, 0x50, 0xab,
        0x37, 0x56, 0x79, 0x97, 0x79, 0xe6, 0xed, 0xf8, 0x3a, 0xc6, 0xf7, 0xec, 0x4d, 0x33, 0x9a, 0x63,
        0x9c, 0xc9, 0x14, 0x7d, 0x09, 0x41, 0xe2, 0x07, 0x91, 0x1d, 0xf6, 0xe8, 0x3f, 0xe5, 0x47, 0x26,
        0x6e, 0x4d, 0x20, 0x6c, 0x9e, 0x21, 0x60, 0xa0, 0xf6, 0xc8, 0x73, 0xc8, 0xa5, 0x3f, 0xbf, 0x74,
        0xd3, 0x2c, 0xc5, 0xce, 0xb0, 0x71, 0xa2, 0x11, 0xee, 0xe2, 0x88, 0x43, 0x87, 0x02, 0x96, 0xe0,
        0x76, 0xcb, 0x45, 0x2f, 0xe2, 0xe6, 0x01, 0xee, 0x6e, 0xab, 0x17, 0x4a, 0x20, 0xee, 0x9e, 0x7c,
        0x35, 0x81, 0xe5, 0xf4, 0x82, 0x74, 0xbf, 0xe4, 0x15, 0x1e, 0x2c, 0xf7, 0x5c, 0xf6, 0x3a, 0x14,
        0x16, 0xcd, 0x1a, 0xb2, 0x67, 0xfe, 0xbd, 0x34, 0x25, 0x56, 0xc1, 0x2c, 0xcd, 0xf5, 0xbf, 0x7f,
        0xae, 0x63, 0x8f, 0xdc, 0x37, 0xac, 0x09, 0x9d, 0xb4, 0x3f, 0x7f, 0x0e, 0x3e, 0xb6, 0xa4, 0xa2,
        0xdb, 0x02, 0x03, 0x01, 0x00, 0x01,
    };
    size_t derlen = sizeof(derbuf);
    cc_size n = ccrsa_import_pub_n(derlen, derbuf);
    ok((ccn_sizeof_n(n) == ccn_sizeof(2048)), "size is correct");
    ccrsa_pub_ctx_decl(ccn_sizeof_n(n), pubkey);
    ccrsa_ctx_n(pubkey) = n;
    if(ccrsa_import_pub(pubkey, derlen, derbuf) != 0) {
        printf("Internal Error importing pubkey\n");
        return 0;
    }
    return 1;
}

// Der blob use by FastSim
unsigned char derdat[] = {
    0x30, 0x82, 0x01, 0xdb, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xaf,
    0xb5, 0xc5, 0xc6, 0x7b, 0xc5, 0x3a, 0x34, 0x90, 0xa9, 0x54, 0xc0, 0x8f,
    0xb7, 0xeb, 0xa1, 0x54, 0xd2, 0x4f, 0x22, 0xde, 0x83, 0xf5, 0x03, 0xa6,
    0xc6, 0x68, 0x46, 0x9b, 0xc0, 0xb8, 0xc8, 0x6c, 0xdb, 0x26, 0xf9, 0x3c,
    0x49, 0x2f, 0x02, 0xe1, 0x71, 0xdf, 0x4e, 0xf3, 0x0e, 0xc8, 0xbf, 0x22,
    0x9d, 0x04, 0xcf, 0xbf, 0xa9, 0x0d, 0xff, 0x68, 0xab, 0x05, 0x6f, 0x1f,
    0x12, 0x8a, 0x68, 0x62, 0xeb, 0xfe, 0xc9, 0xea, 0x9f, 0xa7, 0xfb, 0x8c,
    0xba, 0xb1, 0xbd, 0x65, 0xac, 0x35, 0x9c, 0xa0, 0x33, 0xb1, 0xdd, 0xa6,
    0x05, 0x36, 0xaf, 0x00, 0xa2, 0x7f, 0xbc, 0x07, 0xb2, 0xdd, 0xb5, 0xcc,
    0x57, 0x5c, 0xdc, 0xc0, 0x95, 0x50, 0xe5, 0xff, 0x1f, 0x20, 0xdb, 0x59,
    0x46, 0xfa, 0x47, 0xc4, 0xed, 0x12, 0x2e, 0x9e, 0x22, 0xbd, 0x95, 0xa9,
    0x85, 0x59, 0xa1, 0x59, 0x3c, 0xc7, 0x83, 0x02, 0x03, 0x01, 0x00, 0x01,
    0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xec, 0xbe, 0xe5, 0x5b, 0x9e, 0x7a,
    0x50, 0x8a, 0x96, 0x80, 0xc8, 0xdb, 0xb0, 0xed, 0x44, 0xf2, 0xba, 0x1d,
    0x5d, 0x80, 0xc1, 0xc8, 0xb3, 0xc2, 0x74, 0xde, 0xee, 0x28, 0xec, 0xdc,
    0x78, 0xc8, 0x67, 0x53, 0x07, 0xf2, 0xf8, 0x75, 0x9c, 0x4c, 0xa5, 0x6c,
    0x48, 0x94, 0xc8, 0xeb, 0xad, 0xd7, 0x7d, 0xd2, 0xea, 0xdf, 0x74, 0x20,
    0x62, 0xc9, 0x81, 0xa8, 0x3c, 0x36, 0xb9, 0xea, 0x40, 0xfd, 0x02, 0x41,
    0x00, 0xbe, 0x00, 0x19, 0x76, 0xc6, 0xb4, 0xba, 0x19, 0xd4, 0x69, 0xfa,
    0x4d, 0xe2, 0xf8, 0x30, 0x27, 0x36, 0x2b, 0x4c, 0xc4, 0x34, 0xab, 0xd3,
    0xd9, 0x8c, 0xd6, 0xb8, 0x0d, 0x37, 0x5e, 0x59, 0x4b, 0x76, 0x70, 0x68,
    0x2b, 0x1f, 0x4c, 0x3d, 0x47, 0x5f, 0xa5, 0xb1, 0xcd, 0x74, 0x56, 0x88,
    0xfe, 0x7c, 0xf8, 0x3b, 0x30, 0x6f, 0xfd, 0xc3, 0xed, 0x87, 0x3c, 0xa1,
    0x53, 0x84, 0xc3, 0xd2, 0x7f, 0x02, 0x40, 0x60, 0x71, 0x9b, 0xe9, 0xe8,
    0xf3, 0x97, 0x1f, 0xfe, 0x13, 0xd4, 0xbf, 0x7a, 0xa2, 0x0d, 0xf6, 0x7b,
    0xcf, 0x3e, 0xaa, 0x17, 0x47, 0x75, 0xc3, 0x7f, 0xec, 0xd9, 0x44, 0x9e,
    0xc9, 0x6a, 0x02, 0xe9, 0xe4, 0xaf, 0x56, 0x51, 0xd5, 0x47, 0xa9, 0x09,
    0xb2, 0xc5, 0x16, 0xa7, 0x8b, 0x2b, 0x34, 0xa0, 0x33, 0x6e, 0x2f, 0x3d,
    0x95, 0x7b, 0xe8, 0xef, 0x02, 0xe4, 0x14, 0xbf, 0x44, 0x28, 0xd9, 0x02,
    0x40, 0x10, 0x0e, 0x2e, 0x18, 0xad, 0x5d, 0xe4, 0x43, 0xfe, 0x81, 0x1e,
    0x17, 0xaa, 0xd0, 0x52, 0x31, 0x5e, 0x10, 0x76, 0xa2, 0x35, 0xd9, 0x37,
    0x43, 0xb0, 0xf5, 0x0c, 0x04, 0x81, 0xe3, 0x45, 0x24, 0x6d, 0x53, 0xbe,
    0x59, 0xb6, 0x81, 0x58, 0xc4, 0x49, 0x3e, 0xd5, 0x31, 0x89, 0x5d, 0x2e,
    0xa2, 0x62, 0xa9, 0x0f, 0x47, 0x5e, 0x8f, 0x51, 0x19, 0x27, 0x4e, 0x66,
    0x4b, 0x8a, 0x72, 0x89, 0xbd, 0x02, 0x40, 0x3e, 0x53, 0x0a, 0xf4, 0x8e,
    0x75, 0xe1, 0x52, 0xc6, 0x24, 0xe9, 0xf7, 0xbb, 0xac, 0x3f, 0x22, 0x5f,
    0xe8, 0xe0, 0x79, 0x35, 0xff, 0x91, 0xee, 0x22, 0x56, 0xd2, 0x00, 0x68,
    0x32, 0xc4, 0xe1, 0x5f, 0xff, 0xf8, 0xb1, 0x1d, 0xee, 0xdc, 0x57, 0x81,
    0xd1, 0xab, 0x8b, 0x37, 0x22, 0xe3, 0x9f, 0xd0, 0xa1, 0xc1, 0xce, 0x1d,
    0xd0, 0x24, 0x23, 0xa0, 0x0e, 0xf7, 0xa6, 0xdb, 0xa3, 0xea, 0xd3
};

static int test_import_der_priv_key(void)
{
    int status = 0;
    cc_size n=ccrsa_import_priv_n(sizeof(derdat), derdat);
    ok(!(n==0),"Import size");
    ccrsa_full_ctx_decl(ccn_sizeof_n(n), tmpkey);
    ccrsa_ctx_n(tmpkey)=n;
    ok_or_fail((status = ccrsa_import_priv(tmpkey, sizeof(derdat), derdat)) == 0, "Imported Private Key");
    ok_or_fail((status = RSARoundTripTests(tmpkey)) == 0, "Can round-trip imported key");
    ccrsa_full_ctx_clear(ccn_sizeof_n(n),tmpkey);
    return 1; // No error
}


int ccrsa(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    int verbose = 1;
    int build931 = 1;
    int stdgen = 1;
    int fipsgen = 1;
    int fipskat = 1;
    size_t keystep = 1024;

    plan_tests(4060);

    ok(test_import_der_pub_key(), "Import RSA der public key");
    ok(test_import_der_priv_key(), "Import RSA der private key");

    if(verbose) diag("Sample RSA signature");
    ok(test_rsa_sample() == 0, "Sample RSA signature");
    
    if(verbose) diag("PKCS v1.5 Known Answer Tests");
    ok(test_verify_pkcs1v15_known_answer_test() == 0, "PKCS v1.5 Known Answer Tests");
 
    if(verbose) diag("RSAPSS Known Answer Tests");
    ok(test_rsa_pss_known_answer()==0, "RSAPSS Known Answer Tests");

    if(verbose) diag("Invalid keys");
    ok(test_rsa_invalid_key()==0, "Invalid keys");

    if(stdgen) {
        
        if(verbose) diag("Starting to generate STD keys stepping %lu", keystep);
        for (size_t i=0;i<RSA_STD_KEYGEN_STRESS;i++) {
            for(size_t keysize = 1024; keysize < 4096; keysize+=keystep) {
                if(verbose) diag("Iteration %lu: Generating %lu bit keypair", i, keysize);
                ok(RSAStdGenTest(keysize, 65537) == 0, "Generate Standard RSA Key Pair");
            }
        }

        // We should be able to work with keysizes % 8 - doing a couple here
        ok(RSAStdGenTest(1032, 65537) == 0, "Generate 1032 bit RSA Key Pair");
        ok(RSAStdGenTest(1056, 65537) == 0, "Generate 1056 bit RSA Key Pair");
        if(verbose) diag_linereturn();
    } /* stdgen */

    if(fipsgen) {
        if(verbose) diag("Negative tests for FIPS RSA KeyGen", keystep);
        ok(RSAFIPS_Negative_Test() == 0, "Negative test");

        if(verbose) diag("Starting to generate FIPS keys stepping %lu", keystep);
        for (size_t i=0;i<RSA_FIPS_KEYGEN_STRESS;i++) {
            for(size_t keysize = 1024; keysize <= 4096; keysize+=keystep) {
                if(verbose) diag("Iteration %lu: Generating %lu bit FIPS keypair", i, keysize);
                ok(RSAFIPS_Gen_Test(keysize, 65537) == 0, "Generate FIPS RSA Key Pair");
            }
        }

        // We should be able to work with keysizes % 8 - doing a couple here
        ok(RSAFIPS_Gen_Test(1032, 65537) == 0, "Generate 1032 bit RSA Key Pair");
        ok(RSAFIPS_Gen_Test(1056, 65537) == 0, "Generate 1056 bit RSA Key Pair");
        if(verbose) diag_linereturn();
    } /* stdgen */

    /* KAT where xp1, xp2, xq1, xq2 need to be at the exact expected size:
     * 101bits for 1024 key, 141bits for 2048 key and 171bits for 3072 key
     */
    if(fipskat) {
        if(verbose) diag("KAT tests with ccrsa_generate_fips186_key");
        char *xp1, *xp2, *xp, *xq1, *xq2, *xq, *p, *q, *m, *d, *e;
        int i = 1;

        xp1 = "16bf0a0a1ffb86ecc83b0811b4";
        xp2 = "1ff1fd983eca621b679339f637";
        xp  = "f065e44770423a1a42a2729480d6fadd2d3a5a776b1b2296ab2dde3a7b495b89fd41190a02c8077f33b31a350ea6bfe73684d97be6a358aa988a12e0d952e0b5";
        q   = "f065e44770423a1a42a2729480d6fadd2d3a5a776b1b2296ab2dde3a7b495b89fd41190a02cb5274d198e05fd1bbed9431e02f235a47b5cde34a83036e2d0313";
        xq1 = "1284a138ea6c6820e9afdc8212";
        xq2 = "148e0ab29e93dcc7d1f26eb5e3";
        xq  = "7242adc365806d8796c47a8fc1aeac797f9ace90f081b04074b6a4b58e82b984690a630abd3a2a14b06015a1a8da0ad7b5a3f3b73f27b6004970e14b95e70f87";
        p   = "f242adc365806d8796c47a8fc1aeac797f9ace90f081b04074b6a4b58e82b984690a630abd3a5c9e26598329e0bf22b49ab318478a2d75c2e0d3bcf550810d5f";
        m   = "e37eef3cbabb822419fd0298746375e011fa7bb259b7cfc2df6469505bed13e410debfef68774a848198c3239ff8895524d2749f256104a4228b36c66d5ed16784fecf3c5872beae02a9c3eb0dd9ca6396b73a5399f9eea59b7bab54d67581c9d175bc93e3e7c448222444f8bf5f8f2149ba08742c3ad9d0e07545b1a86f1b0d";
        e   = "010001";
        d   = "0515dc51363764d45bc45d1f7d528fdaff6be301fca1704281ede3b0c92d46898d9afd1d63ef6eba018fa1a515396010882f302e328eb5a4d9a4d507a080431c0b81f4023c52a5613f36e04837595d2337c336f1c0a6222dfe734592e344524aea3d5a8dbd069bc4c179b5f95e8769127bd1b6f4f726c31b69c6c438faf8b6bd";

        if(verbose) diag("FIPS KAT key pair %d", i++);
        ok(RSAFIPS_KAT_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

        xp1 = "1747cbbd8b16c4dbc259e53b8a5c7db1b9f5";
        xp2 = "18946d3a6f5e3e088446dd0e04aa62bc87e8";
        xp  = "6fccd146d52a5b4adda4a45a45f2eabb41da13fe6de477dad87d361d69c2cbb79640e76ac7c28abbce096dbf2e638b2053fc39c503bfcdc64d0ae2d7d818bb984896f115a76a8edad23e996b536856f808c717999dbb3955c4213b001a6d9722ce8d69e6b57e103a2f24765da3a2a413254b0c388172ad2f2cd623a9ce296c99";
        p   = "efccd146d52a5b4adda4a45a45f2eabb41da13fe6de477dad87d361d69c2cbb79640e76ac7c28abbce096dbf2e638b2053fc39c503bfcdc64d0ae2d7d818bb984896f115a76a8edad23e996b536856f808c717999dbb3955c4213b006440c777347d91c61db9c8362e47f98be3046ed928149472f50e6d5892066f6f85e4eeab";
        xq1 = "12b99070aad5f8eb5a1eb542cc7954964713";
        xq2 = "174fff7a7dbe800b669794255534fc85907c";
        xq  = "ceac510d2dbc456cc1bb7d960927c45da1556751e4922fe7efeee3e3c44f4a268872cd88e44682e4d0708971791707030e4f130078c4e8b5fa6989cee8dfb623a5e4adbf2f2010938ed46721e69e3dcadc7f1f14be39d33ee75bb1c51fc12f52686118044659b1b1801e9b7cd85ba5399a9f8c9f94af3c2db548a4c8eb7fe549";
        q   = "ceac510d2dbc456cc1bb7d960927c45da1556751e4922fe7efeee3e3c44f4a268872cd88e44682e4d0708971791707030e4f130078c4e8b5fa6989cee8dfb623a5e4adbf2f2010938ed46721e69e3dcadc7f1f14be39d33ee75bb1cbc87166c1b1a46868c7ff638f06d2b1878fa4587091c665b60ac1ff8d23db1f96da61d153";
        m   = "c19839efc194b43206772251c8f015c9dafabdcfba2f85fdac2d309e890de1185a0bae4fb1dc01015939023bb569276f747624e894df7332244275e88a5cf3a4f07b1f4f7206372a3a1851f6e131f3e1d85fc668522d8436d94e9e3df20fcec951b8d4837ad3bc8876fca31b442232ba1b843074b575f1409e7fa341e1f5f7ff6ebebe92e4cd0cdc3820e77569312999ea027f70a5862dd0ca7f5a49672b0bb84e5f77756fb158a7e9115bafb2000a04023612d8764f78b7167bc00074a94934aa2091ef72e3e3da800cb284cf419346e1308cb6f511a9b02bddab4805899f00e93081fff58edf7c9d16f0f878f3e447179035856693ff7b84aa69a65cddfc71";
        e   = "010001";
        d   = "06873d9cea283b935c48742dd1dad6c141d326c7747dd9c4de237ffa38a0f130430915cdb62fb710f6934791ffe215e45a4709a396319acedc0e29afb7cabea4973f6fa4dc317d76bd12c8e0cc6344d81ed5cbf9049c6852f92cff85133e99388cbaf0dfa62ab1b6e05c509bc0c66e737c2e0efc408a43a26c35dda981172c4fafbc0932cda7598a3a6475c4b14684c70915e093be794d89f763f2ccfcafecca78c51000143afe9cb655389c581c7c18650c23b5cc1a9c46236919d4728cca17562c8fce1beb9eb50eae58e4027244d6ea8dd26ce3cce5376232e0f9882c6baf32fb30754a5568840ccba307c0189457aeeb479f2c0ff84778fba8f3b02ee169";

        if(verbose) diag("FIPS KAT key pair %d", i++);
        ok(RSAFIPS_KAT_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");


        xp1 = "06758b895a2569a5f3750675ffa1ccf21a361772995d";
        xp2 = "04c8a6de0443deeb337153b22e1aaa37c6f1c1acc7a0";
        xp  = "f8db6812f6afb06d609ffe1e355a6d7a3fd0dd5cda682140f07192cd690bf584bba6424ea4741f7e792fc455b0c33fda7da63ff5b2a0372cc636ed3cc09e296073bdab77a13f1cb5fc7af05a9eb59abf1005dac7aab6d67868ce77fc86c5f2cd799c38a475156319587ed9b84e22187150120b9846df8805ec81e49e2224fd80b173116efce75d3eb178f3948bfbb73ff41080c3b5b9e0b672dba421f07165720d9ca1cacbb1288c06e0f50a337a22cff4e1c205f679f4ada5a965c37b97e0fa";
        p   = "f8db6812f6afb06d609ffe1e355a6d7a3fd0dd5cda682140f07192cd690bf584bba6424ea4741f7e792fc455b0c33fda7da63ff5b2a0372cc636ed3cc09e296073bdab77a13f1cb5fc7af05a9eb59abf1005dac7aab6d67868ce77fc86c5f2cd799c38a475156319587ed9b84e22187150120b9846df8805ec81e49e2224fd80b173116efce75d3eb178f3948bfbb73ff41080c46e5d93daf1d83a8874fd51981b2406206696fb055f151a8e6fab00a789a4967603ddeae5a2cab0ef0bde773d";
        xq1 = "07707e60b84e01ed744f4b2cc4d898d63115c50419b4";
        xq2 = "067e05048874d2c690293b7b02b488b450c267a54a22";
        xq  = "6506916267c5c26007953864284eda97d9f95c63952e33635204ff313f936d941f3413a4adebc1f963a895bac448526d2d1cc3b35e5579ccb90f72d3366bba676dae2214b385247154a8d411c33be2ff737b7297b4a9b3ab5670774ea8e8fe23c887de95caf1ae88cbc9fcfaf993d55a0b3f94e5ac2b1455d2859bb41ce9c27928d12809bc10cfdf8ef716cf608f5d88656bf3b6758d8196b80178d5758a8a36989f845ed7aa4e7b87c41a54409e15fc1337b4baeb635e8191ed1bb6d5dfa557";
        q   = "e506916267c5c26007953864284eda97d9f95c63952e33635204ff313f936d941f3413a4adebc1f963a895bac448526d2d1cc3b35e5579ccb90f72d3366bba676dae2214b385247154a8d411c33be2ff737b7297b4a9b3ab5670774ea8e8fe23c887de95caf1ae88cbc9fcfaf993d55a0b3f94e5ac2b1455d2859bb41ce9c27928d12809bc10cfdf8ef716cf608f5d88656bf3b690335fa2989e646e160b75e6b6bde76235a86b0d214f10f71d701feb2d2fb348f9eb9de8d73b1511d6331ad1";
        m   = "dea2a69155ee0a96428bd94475ae6d7e58a419f856bf06284b1f53ba8bbf09d893a10c9d7ea88a7664e23e5e3e9d1eb4fed6ef36aef29fef5e79f35b8c31b427741b74c8f8728fae3488ba96949d77f5e85e76c47fd24bdd955a07f71d03bdc3ea2fc299f20c5b334616a1ac3e4bdbfef34ee0d9c915b413bc178c17b7dcdfd142a0f865d0d29f822bcac419e30f7cc392b4531fa55652b62456ab7580dcc6b73006366f0f7c9355a86608caa4d3a801c3c6f35d48de60cc6f2fbc70fa68dbea90709f11811f139df5644dd8a3aec2301aed0cea43f6613c9d7ba57ec6a9494989af565a654f2e4d445c14bdb14134900101ddaf8f6935df7bb5185f6df81d69c4536d60a605545813f402f789b016936131674900a48eeec68f25e7145397bd1cc2663da56bc8bff2dc02a94866bfd39ca3eb6d3559adf17bcb8acba8949ad5512ee06adb0f888705054f541312d6aec39af31cea32316c8b37823802277938b2a1810afd8a5c5e3e91f9c1a92dd355715db37d68a15cd6509afb6907e28acd";
        e   = "010001";
        d   = "078ed6ffc26933d923b0dd5dda6ee2f9b1e6feee097d2fa77e3160901f2960f0b0c6515d7c0cc5b2923c84a88210a717b6e508b6e9b12e911b90769caa2c3a74f26c9cdb810fb13aaa943a5d5d4b368f806575e662d3cff92eeba104986abaee4e8cc8daf2b6cd0121c527ea3c827fc3ee72443b85d238f6f33636192207f0b333c35a30beaa3fcefff9eb6158ee257ffad828a594e1282aab1e241ba24071cb35e952bf6a1e57b41b3a3cdc80444d2fa37bfab6b52abe222d38c8cebf0ed5e9ac3cca78d14bc291573c52305417e0819496a03b8b21a8467879e4ce69cbfa9161860a9ef52b6edccf1da4e08d00371e4f35e559e8c0b6dcc5e3e3aee841db1958a620acbe5b6ab187938be40123e7caa62ca71c8c24792883fae04d0cdc8815b88f679847c0bd0a03d6c6dc71968f1d724b9463fdf372e1e09d7920e3b3af192f36ca6c6d902f181095b9583bbe9fc1ae383dcd4a68ad67bd35f9cbb6fb06f136a4da4c7c1d10326f10dd5a4e0babca3a3740573e07a358e6c5d1a128fe28a1";

        if(verbose) diag("FIPS KAT key pair %d", i++);
        ok(RSAFIPS_KAT_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");
        if(verbose) diag_linereturn();
    }

    /* KAT where xp1, xp2, xq1, xq2 can be of any arbitrary size
     * Allows to run the CAVs
     */
    if(build931) {
        if(verbose) diag("KAT Tests with ccrsa_make_931_key");
        char *xp1, *xp2, *xp, *xq1, *xq2, *xq, *p, *q, *m, *d;
        uint32_t e;
        int i = 1;

        e = 3;
        xp1 = "1eaa9ade4a0da46dd40824d814";
        xp2 = "17379044dc2c6105423da807f8";
        xp = "fd3f368d01a95944bc1578f8ae58a9b6c17f529da1599a8bcd361df6efede4176924944e30cbe5c2ddea5648019d2086b95c68588380b8725003b047db88f92a";
        xq1 = "1da08feb13d9fba526190d3756";
        xq2 = "10d93d84466d213a3e776c61f6";
        xq = "f67b5f051126a8956171561b62f572090cde4b09b13f73ee28a90bea2bfb4001fe7b16bd51266524684520e77941dddc56b892ae4bd09dd44acc08bf45dd0a58";
        
        
        p = "fd3f368d01a95944bc1578f8ae58a9b6c17f529da1599a8bcd361df6efede4176924944e30d114d4c767d573d1149e005267e6fe36c51d86968cf6f65afcb973";
        q = "f67b5f051126a8956171561b62f572090cde4b09b13f73ee28a90bea2bfb4001fe7b16bd5129f06dc6e1f8b4f739c7eb1eb8dcacca3b41cd484fc0c693367037";
        m = "f3d4c9ca2dca5d4b893919ae7bee0d174d1e7bd2190287f79a7db6f21366108e8b0aa37cc972989ff3730d629620076555884da0e895d4e426449c60e36fad1d0208dd4ade1c45fc90da5e76c9c89fd95d13ce76a97530ee83ea3cfbe96cf28f85c4756797cd0123683194b7b2fcd185c3ea984cb0ef90580f95d57a44b027b5";
        d = "28a376f707a1ba3741898447bf525783e22fbf4daed5c153ef14f3d3033bad6d172c7094cc3dc41aa8932ce5c3b0013b8e4162457c18f8d0b10b6f657b3d478482626149773760b0688ded3b1ebf16044273b2cd3924b068c2572dd9cceb4d13afb0cc64ae4da9facefbf66d271d11ef0dcc4e1af2a7dd80b2c984f4e3bf7fad";
        
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");
        
        e = 0x010001;
        xp1 = "155e67ddb99eefb13e4b77a7f0";
        xp2 = "17044df236c14e8ec333e92506";
        xp = "d4f2b30f4f062ad2d05fc742e91bc20ca3ee8a2d126aff592c7de19edb3b884550ddd6f99b0a6b2b785617b46c0995bc112176dbae9a5b7f0bec678e84d6f44c";
        p = "d4f2b30f4f062ad2d05fc742e91bc20ca3ee8a2d126aff592c7de19edb3b884550ddd6f99b13e5dd56ffb2ac1867030f385597e712f65ac8dd1de502857c1a41";
        xq1 = "1e2923b103c935e3788ebd10e4";
        xq2 = "11a2ccec655a8b362b5ec5fcc4";
        xq = "f7c6a68cff2467f300b82591e5123b1d1256546d999a37f4b18fe4896464df6987e7cc80efee3ce4e2f5c7a3cc085bbe33e4d375ed59cbc591f2b3302bd823bc";
        q = "f7c6a68cff2467f300b82591e5123b1d1256546d999a37f4b18fe4896464df6987e7cc80efeeb4c59165f7d1aec9be2b34889dbe221147e7ceefb5c9bd5cb945";
        m = "ce1b6904ec27f4a8f420414860704f4797a202ed16a9a35f63a16511a31675ccb046b02b192ef121b328385922f5faa032113332d42f84c70d4323133e216b0f339ebaf672f6214d0d7c13bea301174485ec44f44fae0e8a7f8d3c81ced5df77723331816158c3added7dc55f1436a7e5f14730be22cf3bebab1b62915c80c85";
        d = "18d16522721b5793169e61ae08eacd291641ac6f8718933313c8a5e66b487393dbb00f5b89334556e4ff5555aa678b2fca07972e2a2db4a3d15d81b639f7852ffe71657918d0280ff1be2f8f5d90b3e68195ab35e5069a3053540958bc6d58489fecf8baab0981f4af7b4db43550bcf01114e5ecdcb18f228db1c617b5d09781";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");
        
        e = 3;
        xp1 = "1c36bd0874761109bb0575ee16";
        xp2 = "1777c33935db08546dd66b6d96";
        xp = "d040fa5fe5e32eab84bac6cab4c512dae938cbbe4a29f972b78b149b0b5f6a639e29c0830fa13ca140ac83dda18a1ea7b25122d3c39a10effe7afad4a8b4e77ba42c7912399fcd4f1592a3059188bff536788fe6807e0df8e3d1e7350cf5dd69";
        p = "d040fa5fe5e32eab84bac6cab4c512dae938cbbe4a29f972b78b149b0b5f6a639e29c0830fa13ca140ac83dda18a1ea7b25122d3c39a10effe7afad4a8b4e77ba42c791239acf889977037a0efe181d54b93279b7e46a2fdcf674039fb11e89b";
        xq1 = "14c70e475b12870bc6efd3b944";
        xq2 = "1432548a4959eed65b858cd316";
        xq = "e4d222daf062a01a3a9ddfc82a229613403b772ff05fa9fab1fc77de51744af98b65d47bdb2e8f5091af66002550b1d3ca446738450f8f670045f8465a952a8942079c1e048228c86291bb0ae7665146782021262c49143b5ea37ce400240372";
        q = "e4d222daf062a01a3a9ddfc82a229613403b772ff05fa9fab1fc77de51744af98b65d47bdb2e8f5091af66002550b1d3ca446738450f8f670045f8465a952a8942079c1e04937b7eb94b8d322faefd691b6fa2b0ef4a2333ed791afe8ac3ac41";
        m = "ba24d0a5878c01f6ad9140b6271b42309887a6815d5ef1bc3415a381b7b511a42b8d2b8d9df59faa0b69456ff908e24b4ccb835420404ce449c9ce4ca65dc4ae4eb6bb8403b809d530ef4b37e5b211c13a03e2a69afb8c748b90c97d52023ae9a24c1f1f4b3b87685eaa649f54e41b6439e29700543f0747f09658ed392f96ee568a50ad7b5441c88ad37c581526ff296b1c6cc87e352d4f921960b6b630f8f546f1077a7586b839ee07717de84e0a19cd52eceb358ff2c69387b13a83e5335b";
        d = "1f0622c641420053c7983573b12f35b2c4169bc03a3a7d9f5e039b404948d846074231ecefa8eff1ac918b92a9817b0c8ccc95e35ab562260c4c4d0cc664f61d0d1e7496009eac4e32d28c8950f302f589ab507119d49768c1ed76ea3855b47bfcded5a6137e49706fe2f50213aa1313ad67b8adaef390a46bd7ccbdfa0f5042dcd4749d181613a3c9694314626207c7a7c125ca139742296de412449dd1267d6574d30c5e8bb60844e1f21c76ca41cf3bb805c521553218ce71390055029a6b";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");
        
        e = 3;
        xp1 = "1408766e2cb2d47ebfee7ea614";
        xp2 = "16292b77507cffd2f798b7c9f2";
        xp = "f74435451a7ddaa163c8c8ad03dfde97fe066360dfee52e3a9d8f41310fdb484e92e302de0b88c6c698a0b4af99ae001758441bbeb74be9d8047d104a9edb60e9e127c5d0cfd5d170ab84b314f71cbeea22006a2916a1dbc66c5be0357def520fd38445d0815f5ac3099afeb6f2d48666d22da9e3c961949459ce399829719c1";
        p = "f74435451a7ddaa163c8c8ad03dfde97fe066360dfee52e3a9d8f41310fdb484e92e302de0b88c6c698a0b4af99ae001758441bbeb74be9d8047d104a9edb60e9e127c5d0cfd5d170ab84b314f71cbeea22006a2916a1dbc66c5be0357def520fd38445d081fbe68dd24e14f0711cc0351fec8641d8ea7d22c4709f233e6349b";
        xq1 = "161d77eb77c6f257d8f8a3b0ca";
        xq2 = "152f11dfc70b78f0fc6c9137b8";
        xq = "c4d3feeb0e561be3727fd83dedaeaaecba01c798e917dd8bb11a03ce07fcf08f6f006ac6137d021912dffffc1aee981c395366fef05718e38aef69f0abf64f8b2cb9750826b8ec854dab1e1280c403169e3497ee9af08bd6d2b53a0d9c49e034220506f7719041f0cced1cc846b853a090ac42af0f699c2c3174606e02800952";
        q = "c4d3feeb0e561be3727fd83dedaeaaecba01c798e917dd8bb11a03ce07fcf08f6f006ac6137d021912dffffc1aee981c395366fef05718e38aef69f0abf64f8b2cb9750826b8ec854dab1e1280c403169e3497ee9af08bd6d2b53a0d9c49e034220506f771942204f0890fb5e617c580aa98a7482b5457215badc119f23b21c3";
        m = "be1cfc39868d8e9a8239f504482be60c01071cbdab4355b03c10edafe85d9ca10689d86036b6d35829a364a8a2b69f28743e50e5e27ac6b6fe8962809e1c2e0765b2d7508d61bfa538085dfb685595c6965bc5e0855a6dd8807a83e2ee7fc50b5b48f2d232195b672f2c325eb6649dee9758ce76f690107f3b0d10afef427777fb0bac0a41e23717fc54d9194a344d1823bdc18fa364e5373da39a3e41bcc4d88a688a711b56c6387b669d37c4fd7878559b93473869ae8190c46605f03cf25038bf771246fb81a27bc9d44ba67bfce94a3051856511661dbe0803d220809695ad707022c4acd24d40e011eb3752e39568f66cdd2d90369a67295e19dadb0d11";
        d = "1faf7f5eebc2426f15b45380b6b1fbacaad684ca4735e39d5f58279d5164ef702bc1a410091e788eb19b3b717073c53168b50d7ba5bf211e7fc1906ac504b25690f323e2c23af546340164ff3c0e43a1190f4ba56b8f124ec0146b507d154b81e48c28785daee49132875dba73bb6fa7c3e42269291802bfdf2cd81d528b13e90a7de94f042d0ac33102095d0ec64b433c9e43c3a4651e215072c5ba3175aff6085efd3f868589487fd4c2fd72be000f1bcb51c20f6fa3d56b97872d6f0ed21e67a896478336340105e6672bf90bb250ac4f487e0973ca17161781f58763f58ac25ddb77b7297da53dddb02661b18dad920fd4dd7b7233f125336dd79e1ef3c9";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");
        
        e = 3;
        xp1 = "164511563871556a9babc022c8";
        xp2 = "1ae2a7a04f23efe080f48a24b0";
        xp = "db5c4ccf412b17041b6e20b7e0cb45d807ef4da8282428e05e26782fef3251ea2f613d00a134842c6070aa6ebd2c38bb2a28c0f457601b159ae1f5af94dc8c9812f9b4e031ed1f08c64fdb6ffca71c0d3fc93c63596100b2dbce1d6cbf34fae84bccb859397f700114b4bba2e56678360f79c9df784e5f21e995f84fb8622543a48351520012ff80144653efc08ed49e62e17050fa4fc1c98cdd8e40c68f9512e3c687b4cfcc55eb8caeaa3fd44ab8ad00a8389c288eac128c4ee82832e3d0bb";
        p = "db5c4ccf412b17041b6e20b7e0cb45d807ef4da8282428e05e26782fef3251ea2f613d00a134842c6070aa6ebd2c38bb2a28c0f457601b159ae1f5af94dc8c9812f9b4e031ed1f08c64fdb6ffca71c0d3fc93c63596100b2dbce1d6cbf34fae84bccb859397f700114b4bba2e56678360f79c9df784e5f21e995f84fb8622543a48351520012ff80144653efc08ed49e62e17050fa4fc1c98cdd8e40c68f9512e3c687b4d000a836a83d21ea810c683a30e79e5fc8626e78961f076aef2f89ab";
        xq1 = "18ab1ad30607288890b387858a";
        xq2 = "19975a38d9368fa99deda7e986";
        xq = "bd7cc6c56616fb5b41f35d8de2a5c61d1894895dfa46aa95c2de4ea5dfe370eb4543d6670898431d29a9efbbb034347cfaeb8a4c55bcb52dca553dd93ae81fa9ad2bc2b5e6a42c3d3b237648a3907d8a11e6db8b008016064f94168f50fddd791c3d72f729c21e811e68db7ae5400a0f02906462241a33e8faa1c20f48aa12253a80ce75f87a81b37a80079a9ecc42d378ee0e19e913769b738628a14b772673b0fcbf777c55be99f974e1eff5bd8c9d190abff776f246e6614b2f8d81ed812c";
        q = "bd7cc6c56616fb5b41f35d8de2a5c61d1894895dfa46aa95c2de4ea5dfe370eb4543d6670898431d29a9efbbb034347cfaeb8a4c55bcb52dca553dd93ae81fa9ad2bc2b5e6a42c3d3b237648a3907d8a11e6db8b008016064f94168f50fddd791c3d72f729c21e811e68db7ae5400a0f02906462241a33e8faa1c20f48aa12253a80ce75f87a81b37a80079a9ecc42d378ee0e19e913769b738628a14b772673b0fcbf777c640b3b2f869336b823710bb296f32aaba903f90af79239c3d97279";
        m = "a25e0fbcc06a40ac879bba988e78b9df8f88b800077d580b615e3f2f663c9ce631eb0229ee7a4d5166122378bd055f686dd382e63c1564c96127ec191c88d1ba02fcf90f1efcfe29bdfab0fd6413dcb4027512d15c2e337f7111e7acc7679cd1b96581461466ca63af5fbfc0579d322ca02413b75a6dce25c529d6475fafbc5d07504a29039c0f567cbb9dff2938687a6e6d4633f9ae46383536060dc7efb90ff99a6e97449e8f8ad24853f70726953b3f1dc82222f8407f98250f2060777cbd05d0b2ed6abb99d86ac30974df41da16bc1e3abd610df6bcff49a2be932baeedf163911eec026dcbd5937734b47ceb48db97c27bd2a35338f90332b75374ae4404913ae82caf14bba7410c638676a544046aed0b6605562186a4ba6b3695ab25f900899bd03a8f3e68d548b4eadbd9a348a142618954b1b9d73245926d6c57e26454db887c6272280c2d0efff1b856762da7c8be77a0006da3ea589b21ee5efec36574c041d8e506af55de52083225242642cafdcdadfa9663e4424a2bb937d3";
        d = "1b0fad4a2011b5721699f46ec269744fed417400013f8eac903a5fdd3bb4c4d10851d5b1a7bf0ce2e6585b3eca2b8fe6bcf895d10a0390cc3adbfcaeda16cd9f007f7ed7da7f7fb19fa9c82a3b58a4c8ab138322e4b25dea92d85147769144cd9ee6403658bbcc65f28ff54ab944ddb21ab0adf3e467a25ba0dc4e613a9d4a0f81380c5c2b44ad3914c9efaa86debc1467bce108a99d0bb408de5657a1529ed7feef126e8b6fc297230c0dfe813118df352fa15b05d40abfeeb0d7dababe94c9e77e9a8ecb3eebe9823aec87d9f8225aef4465f3dfc5db367a60cf517603a7596a1fbf9e8b08f115b73ecf81b684bfad73c093df30ebc07e434caa87c09d55ab0b674b3858afa1939ba249c7265fd747731f2384d75b5fe6b9e06bbd3110787618290fb73cd42aca08f3f2ee855e393a5e6e835aa77cafc7d329c1dde7655abeeb8d74a015f8d2d36a3bc8939864dfd60da40c63435f76ac1b411af42d5145e95d1b0798a8e8b2ee23edb188228061fa60760993399b16b0cb2246c63ec809f3";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

        e = 3;
        xp1 = "1a02a180a22a37d3ab4d5523fe";
        xp2 = "1179fc502dbe82ff9946c00392";
        xp = "d94a30017127e43b0005e99016c2f4efb8e0c91e61805b52478e35fddf3918e7a3a6e68013e5be75fa246981f222f5862ae79fdc67b3f7e849343ef1d0fb13301e314f267f862d33a66bae633a813b8b91518c95bb3dca18c2b6f02c30b0777cd253329cbcf4779d8d437fdff4c60f27738658f163081d08397e1353073f8df24675588ad215e4dc3615a59d2ad9b9815aeecb9a69fa37e036f36f115e909dbb02fd8a96cad3be182947e944e3a281c3cdf1ad35d4fd62c9417dcb0b3c8beffe8e558e6bab154b78ef43117c2808af1255f7c56dadf8e4ebe384f1eca918cae473e32caf7dc2d5250f6fe5ef00f68a997968dce7fbd2066da370a75aad1f7895";
        p = "d94a30017127e43b0005e99016c2f4efb8e0c91e61805b52478e35fddf3918e7a3a6e68013e5be75fa246981f222f5862ae79fdc67b3f7e849343ef1d0fb13301e314f267f862d33a66bae633a813b8b91518c95bb3dca18c2b6f02c30b0777cd253329cbcf4779d8d437fdff4c60f27738658f163081d08397e1353073f8df24675588ad215e4dc3615a59d2ad9b9815aeecb9a69fa37e036f36f115e909dbb02fd8a96cad3be182947e944e3a281c3cdf1ad35d4fd62c9417dcb0b3c8beffe8e558e6bab154b78ef43117c2808af1255f7c56dadf8e4ebe384f1eca918cae473e32caf7dd1126fd14c73ebcce310791625550d6582891713c38ac374993099";
        xq1 = "1fb621dce29cbb6a66cc3bf7d6";
        xq2 = "122325102c2e57c27d462e1e06";
        xq = "facc7f5f089ed9267363bc23c6c7b8f73208a36f61fa8ea8084ff777bc154107068061c4b9ead9788318eab4c3bf05729a4684f845ce9700aa70811530c50440d4ac19e47a47e5e78047e912996a79bbd9416fa10c3720174ccf8f65d32de16b0dd81187f1bee5b992792105f1d0fa191681cd305f3e113617f58b2d4a54c0cfd88db075c956c137e034fa5573fa71d67a8c076ee5e952a53369db3640438ab55e515e75a81861a99303dcc9c6efc7382cec83234742ccacc7b3e9485b002565c7af8351370aae57d26b2f2b93b7e2885429ab172c516593fb5c1b2b43957b273a2c87cf1d368e88c6f65b41815bac0d1cc9e6113d1d06a1f8ebdba6a1097343";
        q = "facc7f5f089ed9267363bc23c6c7b8f73208a36f61fa8ea8084ff777bc154107068061c4b9ead9788318eab4c3bf05729a4684f845ce9700aa70811530c50440d4ac19e47a47e5e78047e912996a79bbd9416fa10c3720174ccf8f65d32de16b0dd81187f1bee5b992792105f1d0fa191681cd305f3e113617f58b2d4a54c0cfd88db075c956c137e034fa5573fa71d67a8c076ee5e952a53369db3640438ab55e515e75a81861a99303dcc9c6efc7382cec83234742ccacc7b3e9485b002565c7af8351370aae57d26b2f2b93b7e2885429ab172c516593fb5c1b2b43957b273a2c87cf1d710fc707b5e6d58b6f3cb377b286466c4da41f592c749ebf97fca3";
        m = "d4e0061c2150cdf177232b89266af9153902cbd434a39cab549d997ed6dadcb4e84bbac6d49658428728a01bd7036bab4b0003f7e6ccf69df1effad985185c4ab0756237e4be92b2f42085d4388a29f461af98649c700d6dad5e0fe352513b578b3bff5f19b144e6304defe1b4fb43b37ecb4ed7c0e97377802d9e79c6d742837b3b71fd101fcf5ead4a114d9419af008a421d8a4c5efd4e6da8cc3c967502bd4cc1bda09e87bf7a1d0badaf0783a6dbef5c98359c59d6bda1cc9bacfaa962c841ddfa3670211e38a68998508ea1a2be519718a168d09cc0d2c1d0f8d56ca1d7199b0c4fc78ddb595f6681e5b1b96309251c0714bf134d46f58419a0273bfaab3328b59d75d8ada5e6e2745e816d17ded27b52f0b5632088ee6bf9675793adc52591abc3eacbf3ae4b59871ac9c94e98708801f534ad0a99791827e91cbacf7afbbd72e162698aeba0380f74462b8dd097fb576a99d70ad2117efee8f6ef51d6afd6fb8ce9b6c234ebf00d24d44ad505305e48af1a8037fed9a2a44235980d395bf69489309d37a04b66f236d223b1af759232ecf9d6556a71cd74c4936fc6d3efe6efb3311eea1574e0cebd657a9d36142f0719b95c98900bd32b9cdb6702ff92a7eefc5ec99c6f12709cb3a118cdaf56284dd195e0633dd689889924c42d3e6579e403bb3ecb08310128c673de301c3bea248f3bd0f63cab3f2545da9f8d6b";
        d = "237aabaf5ae2ccfd93db31ec3111d42e342b21f8b3709a1c8e1a443fce79cf737c0c9f21236e640b1686c559f92b3c9c8c8000a95122291a52fd5479962eba0c72be3b0950ca6dc87e056ba35ec1b1a8baf299661a12ace79ce502a5e30d89e3ec89ffe52ef2e0d1080cfd5048d48b489521e2794ad1933e955cefbef67935c09489e854d8054d3a723702e243599d2ac1b5af970cba7f8d1246ccb4c3be2b1f8ccaf4f01a6bf53f04d7479d2beb4679fd3a195e44b9a3ca45a219f229c6e5cc0afa545e680585097116eeb817c59b1fb843d9703c22c4cacdcaf82978e7704e8444820d4becf9e43a9115a648499081862f5683752de2367e40aef00689ff1c3a83010a2a02fd60bde977c71b5066fea69851107da6b3c26fc24ca84a0b8df91491bb3fda29e49ff7af5dd0adfbe3454739a4dac131bf48163de6a5af29c957017aac4e66c493f81440beaa685ff96c323c0f334dbb057055a96a8e7dd8297d229c9e915f2b3b7a4cb33cb5279df74b710e5b178eb456f56c07d64afb55f513df7dec96c388184208da0db6088d410e9aae8ffb46fdcc7b813d5c6a28c49a65ed1956711fb321b89ec38172747c0e09aee2ce756f84bc2f00703e8c35f9d2448a1b24dfea1c45c50d75ba01fb8eb4ae1cabcf8cc9ee5974fe9c14958958fbddc93c5d40daaa1c22e3ffcd00d9eca5d29d030c3491aacc2bb50d30fb4667bab3";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

        e = 0xd5db07;
        xp1 = "5bb112dd0eea8da7ab88a74b47c3105f2b69ea75e7bf99f8bfd17ba3";
        xp2 = "04d50b31ce0cf62d6162f9225b03eef44b99d297e048f20784c175";
        xp = "baa459bb19f837a82313f70e75a6215e61ec91248bc39016c813673c08958ebe3768b3e56cf9b6bb50fa07d29f9097dde6ef29b94b635ac5a2b2b473f479c969adefd26a4b64f19d744c63a132dd2ff4cbd31cc4ff7b187f6b0cbd6f86cb52f561bf291c4ed68403783bf0865d2165ef8fce3479644051b850e4c4530ae63865e5c2b3dc7511a10925dd1f0fa6c8fde6fc1ad0b40ff847015745fc9ca6192277c38a2d17908150c7366664cc917f77fe06b5e81dc058080ab232a722f2b7d34e";
        p = "baa459bb19f837a82313f70e75a6215e61ec91248bc39016c813673c08958ebe3768b3e56cf9b6bb50fa07d29f9097dde6ef29b94b635ac5a2b2b473f479c969adefd26a4b64f19d744c63a132dd2ff4cbd31cc4ff7b187f6b0cbd6f86cb52f561bf291c4ed68403783bf0865d2165ef8fce3479644051b850e4c4530ae63865e5c2b3dc7511a1093ba3ef165ce324498f57436ebf8adef98375648a33fe7f67291ab1fd7c11727269c0e10c1d1fc9578432863344b2b89ac601d53f62f4dc87";
        xq1 = "765dae3f7b4535196f62eec5ee495b1a67d1e593e7004baf4dddd91080739f36e84bc37d8c440662900005b1a85356253af6302e3b924927aea579bd03c83ab096a7cbfea48e43";
        xq2 = "0d13d2881d687676894e7ad625bc590ec6b512b11d27";
        xq = "ddbe54ed64de7d7a0379de757bd14b100c9c051b5248f8f88ee513e3069c7ba472b642855dedefc370f378e6c6175efcb4b597cbdc703c94e5c41dcd6eb5296057478e0c18256b5fb162315bc29f437155ad70bbf279df29057a2c9fa4ba96157305f883d6de427119e55054b8270354f154b5aa4e067eb9f99877722e9afec0a104f4646526377c8929721121d75be4bef6f12e6554e5858ea75723b1df13c1c6f430f73ad1b9d4bd3c8565c40f82573c81175020062f7e2cba0e39cb272f2d";
        q = "ddbe54ed64de7d7a0379de757bd14b100c9c051b5248f8f88ee513e3069c7ba472b642855dedefc370f378e6c6175efcb4b597cbdc703c94e5c41dcd6eb5296057478e0c18256b5fb162315bc29f437155ad70bbf279df29057a2c9fa4ba9615730621268bd01abae191dc256f7ccdc1ec024b9020e5232900b1b63064039cd60cb6f9c8ee815815e3b674becf31a212999b99c3de13cd7b14db092995d8fe7dece98c9eb79dff7fefa7184301b06efa66c9eea0cb4e6bfb6a0af1a9f1c6f463";
        m = "a1aaa55c1f6b6e100d3258062bc20a316e8ce02ff9cfa1c9df110ee7be9289309090d7f9858ccf78a32949ed41f2a7ff1784689f7ed20a01f978e8c064f311f6f3f65fa23380e1618a62d04712a34d3be3b433b67434f0dade2cf333dcbc72c764708496c0b96532977e7f5f60bf921aa46cf586fb771b18dbbda728d8e61c45fffa9fdddefa2850379ae5c457d22ecd84f9385ff4c40ae1255f4abde4f926d25a6b681b7b703645d8bb418624bdd9cbb4e69ffdd02038266baefb5d9a76b2a218eba8f7aab54974a74059791446dbe31e2c2a8af162dd68c309452c66319d6005a8410f9001331b16a6882b733a1a73e7507b9d54162295af7ba3631134e12e8870fa462b6e5d4ea0ab3c5d495c64c76df92a1e2ec6d468d6314e0e6924a88497a2b08b3dcbfdb4884b8f66d586c18f8e3e16fad71c5e9f0a9e0ad6f0398e369847db01e1952ff81e8d36f851498eff1fb06da8d5c1ad0259b0b5732fea792239b24e21708eab8b303c63572652efefcf5871a205fa20fe47ab9af14e4bf435";
        d = "25adc60ffedb7f08c53a5661b495a09e5ef6d2250195875923e620c3d80afdc59b2131ce9832db9021422af41b8f37201a9c1f939743dda1702f72baeecb4db26b505932791432b02adadbaf42648396dc4b49fe90750795ed8bb9e2d4963a31e262fdd83b572cd96ae305c54fab42411e7afbfbce7f0c2a52d2508594155992d11681ec66f02a9107a0bcfb0bc4bf886823d0b1f63293b0a31fc9437a24815ef9ccc8f85596195d2d722d9f72252d2ffb5d93612bac999514dda544c2b7ebfa32318276d88f3e027067ba0ca84e1d3eba6112d0f03673b7fbf2a871de4de93d52f8a78657d7267cb99f675f00fce87c62bdda8d1c39409c3465bac3445dbb0439c7707b09cf2d7e75789bfb78dba14a4eae9579f73c5df1afc307dddfa29b67ea7a5a5c932699c2c4be7f40ca9150932f50db9374d02769601c1222481f61c3353cc8059b407a479af01dffbfeeabd8ba97c0a679d874d4cee9a2249dbdf47bb532a7cabb7abadc7a6b85bc4f605a07bba5b42dd24ed149d6f87ce54110f5e7";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

        e = 0x78963b;
        xp1 = "2733f044e950d97eaea4463ebb68718f6d";
        //p1 = 2733f044e950d97eaea4463ebb68718fd3
        xp2 = "12179c6fb04940edeea0223623";
        //p2 = 12179c6fb04940edeea0223661
        xp = "e8637d8cd28dfe250959eb0aa45d402fadf862c9b48674d0a9d572cc8d57bcc520e063140d36dd73dc94a70b5b1e275ebcf1601af3633987994ec2e3d0125ba3";
        p = "e8637d8cd28dfe250959eb0aa45d402fadf862c9b48674d0a9d572cc8d57bcc520e45427ae56bc1f304447870f5f4af7b927d3e060c05ea78c8e412080ece86d";
        xq1 = "c808b18d339b6124594c06b9c94841cf17";
        //q1 = c808b18d339b6124594c06b9c94841cf4d
        xq2 = "15dd1a787b3e9470cf7fb5df2d";
        //q2 = 15dd1a787b3e9470cf7fb5df69
        xq = "e95707ab2c5b717284195ec005079e1c48bd5744b4cbb916e002b809df7f5c4455dbb47fa2decbc078eaea54e03080a64ba9888c6239215c9235bcfb8ee83e90";
        q = "e95707ab2c5b717284195ec005079e1c48bd5744b4cbb916e002b809df7f5c4455e72cb48debd7915bda684ebdbe601a6299eb23163a5a6e215b8a99cf32eaa3";
        m = "d3d18e0af1a69d514ec770bf207dd43c0406b5c4ceb0dbd8c21b7f6da8252e3250859442b0b3a8f55581a0117253fcf0e4669950dbaf9012cc23fa539a84bd621b25431fde3b798b70d6dd0a936f187b618ae5e20f6f9c444588a9fc47b61a3c47ed860771fc55ecfc716bec55a7eb93921d741f25e2e689f55496e52b959f67";
        d = "34742945247a5740550f0805f02c774b5db8730d7f44159973952761cadacf40a5615a79e1e00a78b2b287785d09152f14b87a2491ffba7f9121384f063066835e6515e871e96b6b3e89be2f68b4b7aa490fc3c0d142b1a9366b1aee4127488c5be1a90d64e6b3aadaf98dc824291dc961c7ec4bacbca238533d29f1fd45c7eb";
        if(verbose) diag("Build FIPS key pair %d", i++);
        ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

        e = 0x3dadb7;
        xp1 = "36a675e92a9ced61319c98fa45ca6e141bab41a0225d7bbcc124d7b289783ac20bbcab363848d77a4f";
        //p1 = 36a675e92a9ced61319c98fa45ca6e141bab41a0225d7bbcc124d7b289783ac20bbcab363848d77b6d
        xp2 = "00aa64b0cb733c50d3666d41a302a956621bba634c3c19019f67ccd988ed528143b62799b1d548b919";
        //p2 = 00aa64b0cb733c50d3666d41a302a956621bba634c3c19019f67ccd988ed528143b62799b1d548b9ab
        xp = "ba5db7ce3d0dde423bec2ddbe4e1636425c30d0da51d3069ad8ce657157cded05f73713bb90ab3e48d80b9a8f5bc9a9f90683757498085bf9f7de8ce91a81c5a3351d0445257aac04bd4d2ef924c5780452fef01403a640edea40641e1531dced7cc3280305923281bb44a0f02e1e01b43884ccf97f6504499f2492376da0619301945269aadc7ad5007e948d1121430d36dc32e31a6837b6a7f5b0bbf28de709e885af5a3daf286abe0b2b26cb34caa99392f0e552450e41712526495f60c7d";
        p = "ba5db7ce3d0dde423bec2ddbe4e1636425c30d0da51d3069ad8ce657157cded05f73713bb90ab3e48d80b9a8f5bc9a9f90683757498085bf9f7de8ce91a81c5a3351d0445257aac04bd4d2ef924c5780452fef01403a640edea40641e1531dced7cc3280305923281bb44a0f02e29bc57a77a19bc173986d4f3991f4e78ff4f1b630c725fe1f6e3dd2318afbf9f111d8071305357f2bd10303166dad0334b98f40d53845793803f4788dc9ec568d1a3d08e0a227e4b740ee8d85b5dd06052d17";
        xq1 = "1d4f1147ca43912983c2c74caa928be0256b3b2b2570921b4dc0d7";
        //q1 = 1d4f1147ca43912983c2c74caa928be0256b3b2b2570921b4dc0e7
        xq2 = "07760dcd8ddc59657ac0794e1c6d598f94a2042292ebe7c7866bc24d1f";
        //q2 = 07760dcd8ddc59657ac0794e1c6d598f94a2042292ebe7c7866bc24d55
        xq = "ede277b3716d4ab749a068da372d316d510dbdbe33d4df4b6cf3b1d5aa9323c75e7c02a1dcaeb578d70324aef881cb39ba97a6d0aa648082aa88c1f3dbc30471c4256748c0dc46bd3b4f4704cf4137f360ab770d1c1168054169e3abbf4bf4c4edf48aba2a451ad69f579abef1b2d7798bd447e66d5c3d6a3a21f2113b2c45002cbf8206ae68fc799442b27ae4bf63e1892d0d08aa696e6b08aed00d9258c8756b08a862a84bad8d942ecf5e598a940b3824caad9114dec17860f185a7c777be";
        q = "ede277b3716d4ab749a068da372d316d510dbdbe33d4df4b6cf3b1d5aa9323c75e7c02a1dcaeb578d70324aef881cb39ba97a6d0aa648082aa88c1f3dbc30471c4256748c0dc46bd3b4f4704cf4137f360ab770d1c1168054169e3abbf4bf4c4edf48aba2a451ad69f579abef1b2d7798bd447e66d5c3d6a3a21f2113b2c45002cbf8206ae68fc7b7dbcc4f070cd5fa0bc0f66abb4dc968469acae8236c75e8184b4ccd18fc1d6157e057038d12be329917d476f0cbf23b67e70defdb827015f";
        m = "ad2da10a65230f057c725f87e33e3f0785f553370db587a35d274e5f5b7f0c88502e339149d491ae2ae812b47e7633fcb9d3b3a5271e94c00d1d28943b5a960d541122e55b1d7d322c22f9bc74edf87a49100d2b0ab2e270b31a8d98885d0eb540c879755d2e4a3fc614029ba4c1b9b73e7e2f17f70c0d1003af17efd9cdd70cb88847d7e141e4825916f77ea2a811892302a0ec58ccdc01fd491de1a463699c46128381f29020a4a436055e2f3f153841b1c57390a7fba65382d7e6f337fc672144326e59eef53b0f0155339eae5565a8e662fdad4bce3350decec593355a015a8416325e6f636c33427d317517d642fbee857300bb7664a83195d5f99e86abb7be0c499988d998e3f61c6e0cb44eb76a3bc25dc5a46b0443a71531a8a12b9d2c08bf2626522091c67f51063e7788ae8aa1fa14e63c49ff2efedd0794f5cc1e5fb0004bc4c2a201d03eb64cbd33b8b27578e51c7c215a6822b5dd8ae040b56d8d890440b54373c8096129554ffc2ad659fcd4aed0d679a0179b7ef8a799d289";
        d = "51ce416ca3cbd04124e26f739f8e7f0a74d879b2b99b31a06eb1e71627e13982542f9130942e3a6dcaba33321aaf4d5dd3d22b437c74182f54d4a6321131c1146043a591da8fa22b74da6ffbaa803993f3292910fe57236ca02b6dc14b531b894ded0dd814f6c5dc01e31a55319eef05610f0734a5a782135bfb60861e2f6f0fea663a89d681a9cab53382a27535c84563de40317b7155e8904195ca874f7be6e87e8fd16eee4264890e5b6236be09f01cdff1354751b94351bff7af5b07084295bd2381abe2e7b22d8514bfe959403d43fdb1593d2b04dbf983194f04f2e9faef9dfcc4815ae12f3b9a10c55aa985040ea42a657b8306cfac4cdb6199825bb8092c1149e9b002bb40948942e2b06e0ae478ce8202f99361fcaf78b0e01fc2006cf1c2e49f52d1715501abee47db1aeb32cf83b73ffd9f61b8a5d7bfb31bf6d71cc202e5bc5f835a92ed24946fdefe3089c6ceb68caac0f8c0bcca9589ef1cf9a4d8b8acbfc2235a1a81847546c9ce6af5b33ca66f5eb43f97155dff6c445933";

            if(verbose) diag("Build FIPS key pair %d\n", i++);
            ok(RSAFIPS_Build_Test(e, xp1, xp2, xp, xq1, xq2, xq, p, q, m, d) == 0, "Successfully Built RSA KeyPair");

    } /* build931 */

    return 0;
}
#endif

