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


#include <corecrypto/ccmode.h>
#include <corecrypto/ccpad.h>
#include <corecrypto/cctest.h>
#include <corecrypto/ccmode_test.h>
#include <corecrypto/cc_priv.h>

//#define _INTERNAL_DEBUG_  1
//#define USE_COMMONCRYPTO_GCM  1

#ifdef USE_COMMONCRYPTO_GCM
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonCryptorSPI.h>


CCCryptorStatus CCCryptorGCM(
                             CCOperation 	op,				/* kCCEncrypt, kCCDecrypt */
                             CCAlgorithm		alg,
                             const void 		*key,			/* raw key material */
                             size_t 			keyLength,
                             const void 		*iv,
                             size_t 			ivLen,
                             const void 		*aData,
                             size_t 			aDataLen,
                             const void 		*dataIn,
                             size_t 			dataInLength,
                             void 			*dataOut,
                             const void 		*tag,
                             size_t 			*tagLength)
__OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_5_0);


#endif

#ifdef _INTERNAL_DEBUG_
#include <stdio.h>
#endif

/* does one encryption or decryption in ECB mode and compare result */
int ccmode_ecb_test_one(const struct ccmode_ecb *ecb, size_t keylen, const void *keydata,
                        unsigned long nblocks, const void *in, const void *out)
{
    unsigned char temp[nblocks*ecb->block_size];
    ccecb_ctx_decl(ecb->size, key);
    ecb->init(ecb, key, keylen, keydata);
    ecb->ecb(key, nblocks, in, temp);

#ifdef _INTERNAL_DEBUG_
    {
        char k[keylen*2+1];
        char i[ecb->block_size*nblocks*2+1];
        char o[ecb->block_size*nblocks*2+1];
        char t[ecb->block_size*nblocks*2+1];

        cc_bin2hex(keylen, k, keydata);
        cc_bin2hex(ecb->block_size*nblocks, i, in);
        cc_bin2hex(ecb->block_size*nblocks, o, out);
        cc_bin2hex(ecb->block_size*nblocks, t, temp);

        fprintf(stderr, "k: %s, i: %s, o:%s, t:%s\n", k, i, o, t);
    }
#endif

    return memcmp(out, temp, ecb->block_size*nblocks);
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_ecb_test_one_vector(const struct ccmode_ecb *ecb, const struct ccmode_ecb_vector *v, int dec)
{
    if (dec)
        return ccmode_ecb_test_one(ecb, v->keylen, v->key, v->nblocks, v->ct, v->pt);
    else
        return ccmode_ecb_test_one(ecb, v->keylen, v->key, v->nblocks, v->pt, v->ct);
}

/* Initialize a block of 'nblocks' of zeroes,
 Does 'loops' consecutive encryption (ECB) in place,
 then 'loops' decryption (ECB) in place,
 result should be zeroes. */
int ccmode_ecb_test_key_self(const struct ccmode_ecb *encrypt, const struct ccmode_ecb *decrypt, unsigned long nblocks,
                             size_t keylen, const void *keydata, unsigned long loops)
{
    unsigned char temp[nblocks*encrypt->block_size];
    unsigned char zeroes[nblocks*encrypt->block_size];
    ccecb_ctx_decl(decrypt->size, dkey);
    ccecb_ctx_decl(encrypt->size, ekey);

    cc_zero(nblocks*encrypt->block_size,temp);
    cc_zero(nblocks*encrypt->block_size,zeroes);

    encrypt->init(encrypt, ekey, keylen, keydata);
    decrypt->init(decrypt, dkey, keylen, keydata);

    for (unsigned long i=0; i<loops; i++)
        encrypt->ecb(ekey, nblocks, temp, temp);

    for (unsigned long i=0; i<loops; i++)
        decrypt->ecb(dkey, nblocks, temp, temp);

    return memcmp(zeroes, temp, encrypt->block_size*nblocks);
}

/* does one CBC encryption or decryption and compare result */
int ccmode_cbc_test_one(const struct ccmode_cbc *cbc, size_t keylen, const void *keydata,
                        const void *iv, unsigned long nblocks, const void *in, const void *out)
{
    unsigned char temp[nblocks*cbc->block_size];

    cccbc_one_shot(cbc, keylen, keydata, iv, nblocks, in, temp);

    return memcmp(out, temp, sizeof(temp));
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_cbc_test_one_vector(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec)
{
    if (dec)
        return ccmode_cbc_test_one(cbc, v->keylen, v->key, v->iv, v->nblocks, v->ct, v->pt);
    else
        return ccmode_cbc_test_one(cbc, v->keylen, v->key, v->iv, v->nblocks, v->pt, v->ct);
}

/* Test one test vector, with unaligned data */
int ccmode_cbc_test_one_vector_unaligned(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec)
{
    uint8_t pt[v->nblocks*cbc->block_size+1];
    uint8_t ct[v->nblocks*cbc->block_size+1];

    CC_MEMCPY(pt+1, v->pt, v->nblocks*cbc->block_size);
    CC_MEMCPY(ct+1, v->ct, v->nblocks*cbc->block_size);

    if (dec)
        return ccmode_cbc_test_one(cbc, v->keylen, v->key, v->iv, v->nblocks, ct+1, v->pt);
    else
        return ccmode_cbc_test_one(cbc, v->keylen, v->key, v->iv, v->nblocks, pt+1, v->ct);
}

/* Test one test vector, 1 block at a time */
int ccmode_cbc_test_one_chained(const struct ccmode_cbc *cbc, size_t keylen, const void *keydata,
                                const void *iv, unsigned long nblocks, const void *in, const void *out)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char temp[nblocks*cbc->block_size];
    cccbc_ctx_decl(cbc->size, key);
    cccbc_iv_decl(cbc->block_size, iv_ctx);
    cccbc_init(cbc, key, keylen, keydata);
    cccbc_set_iv(cbc, iv_ctx, iv);
    for (i=0; i<nblocks; i++) {
        cccbc_update(cbc, key, iv_ctx, 1, &input[i*cbc->block_size], &temp[i*cbc->block_size]);
    }

    return memcmp(out, temp, cbc->block_size*nblocks);
}

int ccmode_cbc_test_one_vector_chained(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec)
{
    if (dec)
        return ccmode_cbc_test_one_chained(cbc, v->keylen, v->key, v->iv, v->nblocks, v->ct, v->pt);
    else
        return ccmode_cbc_test_one_chained(cbc, v->keylen, v->key, v->iv, v->nblocks, v->pt, v->ct);
}


/* Initialize a block of 'nblocks' of zeroes,
 Does 'loops' consecutive encryption (CBC) in place,
 then 'loops' decryption (CBC) in place,
 using 0 for iv in each loop, result should be zeroes. */
int ccmode_cbc_test_key_self(const struct ccmode_cbc *encrypt,
                             const struct ccmode_cbc *decrypt,
                             unsigned long nblocks, size_t keylen,
                             const void *keydata, unsigned long loops)
{
    unsigned char temp[nblocks*encrypt->block_size];
    unsigned char zeroes[nblocks*encrypt->block_size];
    cccbc_iv_decl(encrypt->block_size, iv); // we can use the same iv context for encrypt and decrypt
                                            // as long as we are not chaining concurrently for both.
    cccbc_ctx_decl(encrypt->size, ekey);
    cccbc_ctx_decl(decrypt->size, dkey);

    cc_zero(nblocks*encrypt->block_size,temp);
    cc_zero(nblocks*encrypt->block_size,zeroes);

    cccbc_init(encrypt, ekey, keylen, keydata);

    for (unsigned long i=0; i<loops; i++) {
        cccbc_set_iv(encrypt, iv, NULL);
        cccbc_update(encrypt, ekey, iv, nblocks, temp, temp);
    }

    cccbc_init(decrypt, dkey, keylen, keydata);

    for (unsigned long i=0; i<loops; i++) {
        cccbc_set_iv(decrypt, iv, NULL);
        cccbc_update(decrypt, dkey, iv, nblocks, temp, temp);
    }

    return memcmp(zeroes, temp, encrypt->block_size*nblocks);
}

/*
 Encrypt and decrypt 'nblocks*loop' blocks of zeroes,
 'nblocks' at a time.
 */
int ccmode_cbc_test_chaining_self(const struct ccmode_cbc *encrypt,
                                  const struct ccmode_cbc *decrypt,
                                  unsigned long nblocks, size_t keylen,
                                  const void *keydata, unsigned long loops)
{
    unsigned char temp[nblocks*encrypt->block_size];
    unsigned char zeroes[nblocks*encrypt->block_size];
    cccbc_ctx_decl(encrypt->size, ekey);
    cccbc_ctx_decl(decrypt->size, dkey);

    /* here we have to use two iv contexts */
    cccbc_iv_decl(encrypt->block_size, eiv);
    cccbc_iv_decl(decrypt->block_size, div);

    cc_zero(nblocks*encrypt->block_size,temp);
    cc_zero(nblocks*encrypt->block_size,zeroes);

    cccbc_init(encrypt, ekey, keylen, keydata);
    cccbc_init(decrypt, dkey, keylen, keydata);

    cccbc_set_iv(encrypt, eiv, NULL);
    cccbc_set_iv(decrypt, div, NULL);

    for (unsigned long i=0; i<loops; i++) {
        cccbc_update(encrypt, ekey, eiv, nblocks, temp, temp);
        cccbc_update(decrypt, dkey, div, nblocks, temp, temp);
    }

    return memcmp(zeroes, temp, sizeof(temp));
}


/* OFB */

/* does one OFB encryption or decryption and compare result */
int ccmode_ofb_test_one(const struct ccmode_ofb *ofb, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned char temp[nbytes];
    ccofb_ctx_decl(ofb->size, key);
    ofb->init(ofb, key, keylen, keydata, iv);
    ofb->ofb(key, nbytes, in, temp);

    return memcmp(out, temp, nbytes);
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_ofb_test_one_vector(const struct ccmode_ofb *ofb, const struct ccmode_ofb_vector *v, int dec)
{
    if (dec)
        return ccmode_ofb_test_one(ofb, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_ofb_test_one(ofb, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}

/* Test one test vector, 1 byte at a time */
int ccmode_ofb_test_one_chained(const struct ccmode_ofb *ofb, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char temp[nbytes];
    ccofb_ctx_decl(ofb->size, key);
    ofb->init(ofb, key, keylen, keydata, iv);
    for (i=0; i<nbytes; i++) {
        ofb->ofb(key, 1, &input[i], &temp[i]);
    }

    return memcmp(out, temp, nbytes);
}

int ccmode_ofb_test_one_vector_chained(const struct ccmode_ofb *ofb, const struct ccmode_ofb_vector *v, int dec)
{
    if(dec)
        return ccmode_ofb_test_one_chained(ofb, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_ofb_test_one_chained(ofb, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}

/* does one CFB encryption or decryption and compare result */
int ccmode_cfb_test_one(const struct ccmode_cfb *cfb, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned char temp[nbytes];
    cccfb_ctx_decl(cfb->size, key);
    cfb->init(cfb, key, keylen, keydata, iv);
    cfb->cfb(key, nbytes, in, temp);

    return memcmp(out, temp, nbytes);
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_cfb_test_one_vector(const struct ccmode_cfb *cfb, const struct ccmode_cfb_vector *v, int dec)
{
    if (dec)
        return ccmode_cfb_test_one(cfb, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_cfb_test_one(cfb, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}

/* Test one test vector, 1 block at a time */
int ccmode_cfb_test_one_chained(const struct ccmode_cfb *cfb, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char temp[nbytes];
    cccfb_ctx_decl(cfb->size, key);
    cfb->init(cfb, key, keylen, keydata, iv);
    for (i=0; i<nbytes; i++) {
        cfb->cfb(key, 1, &input[i], &temp[i]);
    }

    return memcmp(out, temp, nbytes);
}

int ccmode_cfb_test_one_vector_chained(const struct ccmode_cfb *cfb, const struct ccmode_cfb_vector *v, int dec)
{
    if (dec)
        return ccmode_cfb_test_one_chained(cfb, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_cfb_test_one_chained(cfb, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}


/* CFB8 */

/* does one CFB8 encryption or decryption and compare result */
int ccmode_cfb8_test_one(const struct ccmode_cfb8 *cfb8, size_t keylen, const void *keydata,
                         const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned char temp[nbytes];
    cccfb8_ctx_decl(cfb8->size, key);
    cfb8->init(cfb8, key, keylen, keydata, iv);
    cfb8->cfb8(key, nbytes, in, temp);

    return memcmp(out, temp, nbytes);
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_cfb8_test_one_vector(const struct ccmode_cfb8 *cfb8, const struct ccmode_cfb8_vector *v, int dec)
{
    if(dec)
        return ccmode_cfb8_test_one(cfb8, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_cfb8_test_one(cfb8, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}

/* Test one test vector, 1 byte at a time */
int ccmode_cfb8_test_one_chained(const struct ccmode_cfb8 *cfb8, size_t keylen, const void *keydata,
                                 const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char temp[nbytes];
    cccfb8_ctx_decl(cfb8->size, key);
    cfb8->init(cfb8, key, keylen, keydata, iv);
    for (i=0; i<nbytes; i++) {
        cfb8->cfb8(key, 1, &input[i], &temp[i]);
    }

    return memcmp(out, temp, nbytes);
}

int ccmode_cfb8_test_one_vector_chained(const struct ccmode_cfb8 *cfb8, const struct ccmode_cfb8_vector *v, int dec)
{
    if(dec)
        return ccmode_cfb8_test_one_chained(cfb8, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_cfb8_test_one_chained(cfb8, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}


/* CTR */

/* does one CTR encryption or decryption and compare result */
int ccmode_ctr_test_one(const struct ccmode_ctr *ctr, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned char temp[nbytes];
    ccctr_ctx_decl(ctr->size, key);
    ctr->init(ctr, key, keylen, keydata, iv);
    ctr->ctr(key, nbytes, in, temp);

    return memcmp(out, temp, nbytes);
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_ctr_test_one_vector(const struct ccmode_ctr *ctr, const struct ccmode_ctr_vector *v, int dec)
{
    if (dec)
        return ccmode_ctr_test_one(ctr, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_ctr_test_one(ctr, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}

/* Test one test vector, 1 byte at a time */
int ccmode_ctr_test_one_chained(const struct ccmode_ctr *ctr, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char temp[nbytes];
    ccctr_ctx_decl(ctr->size, key);
    ctr->init(ctr, key, keylen, keydata, iv);
    for (i=0; i<nbytes; i++) {
        ctr->ctr(key, 1, &input[i], &temp[i]);
    }

    return memcmp(out, temp, nbytes);
}

int ccmode_ctr_test_one_vector_chained(const struct ccmode_ctr *ctr, const struct ccmode_ctr_vector *v, int dec)
{
    if (dec)
        return ccmode_ctr_test_one_chained(ctr, v->keylen, v->key, v->iv, v->nbytes, v->ct, v->pt);
    else
        return ccmode_ctr_test_one_chained(ctr, v->keylen, v->key, v->iv, v->nbytes, v->pt, v->ct);
}


/* GCM */

int ccmode_gcm_test_one(const struct ccmode_gcm *gcm,
                        size_t keylen, const void *keydata,
                        size_t ivlen, const void *iv,
                        size_t adalen, const void *ada,
                        size_t nbytes, const void *in, const void *out,
                        size_t taglen, const void *tag)
{
    unsigned char temp[nbytes];
    unsigned char temptag[taglen];
    ccgcm_ctx_decl(gcm->size, key);
    gcm->init(gcm, key, keylen, keydata);
    gcm->set_iv(key, ivlen, iv);
    gcm->gmac(key, adalen, ada);
    gcm->gcm(key, nbytes, in, temp);
    gcm->finalize(key, taglen, temptag);

#ifdef _INTERNAL_DEBUG_
    int r1, r2;
    r1 = memcmp(out, temp, nbytes);
    r2 = memcmp(tag, temptag, taglen);
    if (r1 || r2)
        cc_printf("ivlen: %lu adalen: %lu nbytes: %lu taglen: %lu crypt: %d tag: %d\n",
               ivlen, adalen, nbytes, taglen, r1, r2);

    return r1 != 0 ? r1 : r2;
#else
    return memcmp(out, temp, nbytes) || memcmp(tag, temptag, taglen);
#endif
}

int ccmode_gcm_test_one_chained(const struct ccmode_gcm *gcm,
                                size_t keylen, const void *keydata,
                                size_t ivlen, const void *iv,
                                size_t adalen, const void *ada,
                                size_t nbytes, const void *in, const void *out,
                                size_t taglen, const void *tag)
{
    /* mac and crypt one byte at a time */
    unsigned long i;
    unsigned char temp[nbytes];
    unsigned char temptag[taglen];
    ccgcm_ctx_decl(gcm->size, key);
    gcm->init(gcm, key, keylen, keydata);
    gcm->set_iv(key, ivlen, iv);
    const unsigned char *p = ada;
    if (adalen) {
        for (i = 0; i < adalen; i++) {
            gcm->gmac(key, 1, &p[i]);
        }
    } else {
        gcm->gmac(key, 0, NULL);
    }

    p = in;
    if (nbytes) {
        for (i = 0; i < nbytes; i++) {
            gcm->gcm(key, 1, &p[i], &temp[i]);
        }
    } else {
        gcm->gcm(key, 0, NULL, NULL);
    }
    gcm->finalize(key, taglen, temptag);

#ifdef _INTERNAL_DEBUG_
    int r1, r2;
    r1 = memcmp(out, temp, nbytes);
    r2 = memcmp(tag, temptag, taglen);
    if (r1 || r2)
        cc_printf("ivlen: %lu adalen: %lu nbytes: %lu taglen: %lu crypt: %d tag: %d\n",
               ivlen, adalen, nbytes, taglen, r1, r2);

    return r1 != 0 ? r1 : r2;
#else
    return memcmp(out, temp, nbytes) || memcmp(tag, temptag, taglen);
#endif
}


int ccmode_gcm_test_one_vector(const struct ccmode_gcm *gcm, const struct ccmode_gcm_vector *v, int dec)
{
    if (v->ptlen!=v->ctlen)
        return -1;

#ifdef USE_COMMONCRYPTO_GCM
    unsigned char temp[v->ptlen];
    unsigned char temptag[v->taglen];
    CCCryptorStatus st;
    size_t taglen = v->taglen;
    st = CCCryptorGCM(dec ? kCCDecrypt : kCCEncrypt,
                      kCCAlgorithmAES128,
                      v->key, v->keylen,
                      v->iv, v->ivlen,
                      v->ada, v->adalen,
                      dec ? v->ct : v->pt, v->ptlen,
                      temp,
                      temptag, &taglen);
#ifdef _INTERNAL_DEBUG_
    int r1, r2;
    r1 = memcmp(dec ? v->pt : v->ct, temp, v->ptlen);
    r2 = memcmp(v->tag, temptag, v->taglen);
    if (r1 || r2)
        cc_printf("ivlen: %lu adalen: %lu nbytes: %lu taglen: %lu crypt: %d tag: %d\n",
               v->ivlen, v->adalen, v->ptlen, v->taglen, r1, r2);

    return r1 != 0 ? r1 : r2;
#else
    return memcmp(dec ? v->pt : v->ct, temp, v->ptlen) || memcmp(v->tag, temptag, v->taglen);
#endif

#else
    if (dec)
        return ccmode_gcm_test_one(gcm, v->keylen, v->key, v->ivlen, v->iv, v->adalen, v->ada, v->ptlen, v->ct, v->pt, v->taglen, v->tag);
    else
        return ccmode_gcm_test_one(gcm, v->keylen, v->key, v->ivlen, v->iv, v->adalen, v->ada, v->ptlen, v->pt, v->ct, v->taglen, v->tag);
#endif
}

int ccmode_gcm_test_one_vector_chained(const struct ccmode_gcm *gcm, const struct ccmode_gcm_vector *v, int dec)
{
    if (v->ptlen!=v->ctlen)
        return -1;

#ifdef USE_COMMONCRYPTO_GCM
    unsigned char temp[v->ptlen];
    unsigned char temptag[v->taglen];
    CCCryptorStatus st;
    size_t taglen = v->taglen;
    CCCryptorRef cryptor = NULL;
    st = CCCryptorCreateWithMode(dec ? kCCDecrypt : kCCEncrypt, 11 /* kCCModeGCM */, kCCAlgorithmAES128, 0, NULL, v->key, v->keylen, NULL, 0, 0, 0, &cryptor);
    st = CCCryptorGCMAddIV(cryptor, v->iv, v->ivlen);
    unsigned long i;
#if 0
    for (i = 0; i < v->adalen; ++i) {
        st = CCCryptorGCMAddADD(cryptor, &v->ada[i], 1);
    }
#else
    st = CCCryptorGCMAddADD(cryptor, v->ada, v->adalen);
#endif

#if 0
    for (i = 0; i < v->ptlen; ++i) {
        st = (dec ? CCCryptorGCMDecrypt(cryptor, &v->ct[i], 1, &temp[i])
              : CCCryptorGCMEncrypt(cryptor, &v->pt[i], 1, &temp[i]));
    }
#else
    st = (dec ? CCCryptorGCMDecrypt(cryptor, v->ct, v->ptlen, temp)
          : CCCryptorGCMEncrypt(cryptor, v->pt, v->ptlen, temp));
#endif
    st = CCCryptorGCMFinal(cryptor, temptag, &taglen);
    CCCryptorRelease(cryptor);

#ifdef _INTERNAL_DEBUG_
    int r1, r2;
    r1 = memcmp(dec ? v->pt : v->ct, temp, v->ptlen);
    r2 = memcmp(v->tag, temptag, v->taglen);
    if (r1 || r2)
    cc_printf("ivlen: %lu adalen: %lu nbytes: %lu taglen: %lu crypt: %d tag: %d\n",
           v->ivlen, v->adalen, v->ptlen, v->taglen, r1, r2);

    return r1 != 0 ? r1 : r2;
#else
    return memcmp(dec ? v->pt : v->ct, temp, v->ptlen) || memcmp(v->tag, temptag, v->taglen);
#endif

#else
    if (dec)
        return ccmode_gcm_test_one_chained(gcm, v->keylen, v->key, v->ivlen, v->iv, v->adalen, v->ada, v->ptlen, v->ct, v->pt, v->taglen, v->tag);
    else
        return ccmode_gcm_test_one_chained(gcm, v->keylen, v->key, v->ivlen, v->iv, v->adalen, v->ada, v->ptlen, v->pt, v->ct, v->taglen, v->tag);
#endif
}



/* does one XTS encryption or decryption and compare result */
void ccmode_xts_test_one(const struct ccmode_xts *xts, size_t keylen,
                         const void *dkey, const void *tkey, const void *iv,
                         size_t nbytes, const void *in, void *out,
                         int dec)
{
    ccxts_ctx_decl(xts->size, key);
    ccxts_tweak_decl(xts->tweak_size, tweak);
    xts->init(xts, key, keylen, dkey, tkey);
    xts->set_tweak(key, tweak, iv);
    /* Use raw xex mode when nbytes is a multiple of the blocksize. */
    if ((nbytes & 15) == 0) {
        xts->xts(key, tweak, nbytes >> 4, in, out);
    } else {
        if (dec)
            ccpad_xts_decrypt(xts, key, tweak, nbytes, in, out);
        else
            ccpad_xts_encrypt(xts, key, tweak, nbytes, in, out);
    }
}

/* Test one test vector - use dec=1 to reverse pt and ct */
void ccmode_xts_test_one_vector(const struct ccmode_xts *xts,
                                const struct ccmode_xts_vector *v, void *out,
                                int dec)
{
    if (dec)
        return ccmode_xts_test_one(xts, v->keylen, v->dkey, v->tkey, v->tweak,
                                   v->nbytes, v->ct, out, dec);
    else
        return ccmode_xts_test_one(xts, v->keylen, v->dkey, v->tkey, v->tweak,
                                   v->nbytes, v->pt, out, dec);
}

/* Test one test vector, 1 block at a time */
void ccmode_xts_test_one_chained(const struct ccmode_xts *xts,
                                 size_t keylen, const void *dkey,
                                 const void *tkey, const void *iv,
                                 size_t nbytes, const void *in,
                                 void *out, int dec)
{
    unsigned long i;
    const unsigned char *input=in;
    unsigned char *output=out;
    ccxts_ctx_decl(xts->size, key);
    ccxts_tweak_decl(xts->tweak_size, tweak);
    xts->init(xts, key, keylen, dkey, tkey);
    xts->set_tweak(key, tweak, iv);
    unsigned long nblocks = nbytes >> 4;
    if (nbytes & 15) {
        nblocks -= 1;
    }

    for (i=0; i < nblocks; i++) {
        xts->xts(key, tweak, 1, &input[i*xts->block_size], &output[i*xts->block_size]);
    }

    if (nbytes & 15) {
        if (dec)
            ccpad_xts_decrypt(xts, key, tweak, nbytes-nblocks*xts->block_size, &input[nblocks*xts->block_size],
                              &output[nblocks*xts->block_size]);
        else
            ccpad_xts_encrypt(xts, key, tweak, nbytes-nblocks*xts->block_size, &input[nblocks*xts->block_size],
                              &output[nblocks*xts->block_size]);
    }
}

void ccmode_xts_test_one_vector_chained(const struct ccmode_xts *xts,
                                        const struct ccmode_xts_vector *v,
                                        void *out, int dec)
{
    if (dec)
        ccmode_xts_test_one_chained(xts, v->keylen, v->dkey, v->tkey, v->tweak,
                                    v->nbytes, v->ct, out, dec);
    else
        ccmode_xts_test_one_chained(xts, v->keylen, v->dkey, v->tkey, v->tweak,
                                    v->nbytes, v->pt, out, dec);
}


/* CCM */

int ccmode_ccm_test_one(const struct ccmode_ccm *ccm, size_t keylen, const void *keydata,
                        unsigned nonce_len, const void *nonce, unsigned long adalen, const void *ada,
                        unsigned long nbytes, const void *in, const void *out,
                        unsigned mac_size, const void *mac, int chained)
{
    unsigned char temp[nbytes];
    unsigned char tempmac[mac_size];
    ccccm_ctx_decl(ccm->size, key);
    ccccm_nonce_decl(ccm->nonce_size, nonce_ctx);

    ccm->init(ccm, key, keylen, keydata);
    ccm->set_iv(key, nonce_ctx, nonce_len, nonce, mac_size, adalen, nbytes);

    /* mac and crypt one byte at a time */
    if (chained) {
        const unsigned char *p = ada;
        if (adalen) {
            for(unsigned i = 0; i < adalen; i++) {
                ccm->cbcmac(key, nonce_ctx, 1, &p[i]);
            }
        } else {
            ccm->cbcmac(key, nonce_ctx, 0, NULL);
        }

        p = in;
        if (nbytes) {
            for(unsigned i = 0; i < nbytes; i++) {
                ccm->ccm(key, nonce_ctx, 1, &p[i], &temp[i]);
            }
        } else {
            ccm->ccm(key, nonce_ctx, 0, NULL, NULL);
        }
    } else {
        ccm->cbcmac(key, nonce_ctx, adalen, ada);
        ccm->ccm(key, nonce_ctx, nbytes, in, temp);
    }

    ccm->finalize(key, nonce_ctx, tempmac);

#ifdef _INTERNAL_DEBUG_
    int r1, r2;
    r1 = memcmp(out, temp, nbytes);
    r2 = memcmp(mac, tempmac, mac_size);
    if (r1 || r2)
        printf("nonce_len: %u adalen: %lu nbytes: %lu taglen: %u crypt: %d tag: %d\n",
               nonce_len, adalen, nbytes, mac_size, r1, r2);

    return r1 != 0 ? r1 : r2;
#else
    return memcmp(out, temp, nbytes) || memcmp(mac, tempmac, mac_size);
#endif
}

/* Test one test vector - use dec=1 to reverse pt and ct */
int ccmode_ccm_test_one_vector(const struct ccmode_ccm *ccm,
                                const struct ccmode_ccm_vector *v,
                                int dec, int chained)
{
    if (dec)
        return ccmode_ccm_test_one(ccm, v->keylen, v->key, v->noncelen, v->nonce, v->adalen, v->ada, v->ptlen, v->ct, v->pt, (unsigned)(v->ctlen - v->ptlen), v->ct + v->ptlen, chained);
    else
        return ccmode_ccm_test_one(ccm, v->keylen, v->key, v->noncelen, v->nonce, v->adalen, v->ada, v->ptlen, v->pt, v->ct, (unsigned)(v->ctlen - v->ptlen), v->ct + v->ptlen, chained);
}
