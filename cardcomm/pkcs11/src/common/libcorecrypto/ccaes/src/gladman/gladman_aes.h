/*
 ---------------------------------------------------------------------------
 Copyright (c) 2003, Dr Brian Gladman, Worcester, UK.   All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue 28/01/2004

 This file contains the definitions required to use AES in C. See aesopt.h
 for optimisation details.
*/

#ifndef _CORECRYPTO_GLADMAN_AES_H_
#define _CORECRYPTO_GLADMAN_AES_H_

#include <corecrypto/ccmode.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#define AES_128     /* define if AES with 128 bit keys is needed    */
#define AES_192     /* define if AES with 192 bit keys is needed    */
#define AES_256     /* define if AES with 256 bit keys is needed    */
#define AES_VAR     /* define if a variable key size is needed      */

/* The following must also be set in assembler files if being used  */

#define AES_ENCRYPT /* if support for encryption is needed          */
#define AES_DECRYPT /* if support for decryption is needed          */

typedef uint8_t  aes_08t;
typedef uint32_t aes_32t;

#define AES_BLOCK_SIZE  16  /* the AES block size in bytes          */
#define N_COLS           4  /* the number of columns in the state   */

/* The key schedule length is 11, 13 or 15 16-byte blocks for 128,  */
/* 192 or 256-bit keys respectively. That is 176, 208 or 240 bytes  */
/* or 44, 52 or 60 32-bit words. For simplicity this code allocates */
/* the maximum 60 word array for the key schedule for all key sizes */

#if defined( AES_VAR ) || defined( AES_256 )
#define KS_LENGTH       60
#elif defined( AES_192 )
#define KS_LENGTH       52
#else
#define KS_LENGTH       44
#endif

#define aes_ret     void
#define aes_rval    aes_ret

typedef struct
{   aes_32t ks[KS_LENGTH];
    aes_32t rn;
	uint8_t chainBuf[AES_BLOCK_SIZE];
	aes_32t cbcEnable;
} ccaes_gladman_encrypt_ctx;

typedef struct
{   aes_32t ks[KS_LENGTH];
    aes_32t rn;
    uint8_t chainBuf[AES_BLOCK_SIZE];
	aes_32t cbcEnable;
} ccaes_gladman_decrypt_ctx;

typedef struct
{
	ccaes_gladman_decrypt_ctx decrypt;
    ccaes_gladman_encrypt_ctx encrypt;
} ccaes_gladman_ctx;


/* This routine must be called before first use if non-static       */
/* tables are being used                                            */

void gen_tabs(void);

/* The key length (klen) is input in bytes when it is in the range  */
/* 16 <= klen <= 32 or in bits when in the range 128 <= klen <= 256 */

#if defined( AES_ENCRYPT )

#if defined(AES_128) || defined(AES_VAR)
aes_rval ccaes_gladman_encrypt_key128(const unsigned char *in_key, ccaes_gladman_encrypt_ctx cx[1]);
#endif

#if defined(AES_192) || defined(AES_VAR)
aes_rval ccaes_gladman_encrypt_key192(const unsigned char *in_key, ccaes_gladman_encrypt_ctx cx[1]);
#endif

#if defined(AES_256) || defined(AES_VAR)
aes_rval ccaes_gladman_encrypt_key256(const unsigned char *in_key, ccaes_gladman_encrypt_ctx cx[1]);
#endif

#if defined(AES_VAR)
aes_rval ccaes_gladman_encrypt_key(const unsigned char *in_key, size_t key_len, ccaes_gladman_encrypt_ctx cx[1]);
#endif

void ccaes_gladman_encrypt(const cccbc_ctx *ctx, cccbc_iv *iv, unsigned long num_blks,
                            const void *in, void *out);

#endif

#if defined( AES_DECRYPT )

#if defined(AES_128) || defined(AES_VAR)
aes_rval ccaes_gladman_decrypt_key128(const unsigned char *in_key, ccaes_gladman_decrypt_ctx cx[1]);
#endif

#if defined(AES_192) || defined(AES_VAR)
aes_rval ccaes_gladman_decrypt_key192(const unsigned char *in_key, ccaes_gladman_decrypt_ctx cx[1]);
#endif

#if defined(AES_256) || defined(AES_VAR)
aes_rval ccaes_gladman_decrypt_key256(const unsigned char *in_key, ccaes_gladman_decrypt_ctx cx[1]);
#endif

#if defined(AES_VAR)
aes_rval ccaes_gladman_decrypt_key(const unsigned char *in_key, size_t key_len, ccaes_gladman_decrypt_ctx cx[1]);
#endif

void ccaes_gladman_decrypt(const cccbc_ctx *ctx, cccbc_iv *iv, unsigned long num_blks,
                            const void *in, void *out);

#endif


#if defined(__cplusplus)
}
#endif

#endif	/* _CC_AES_H_ */
