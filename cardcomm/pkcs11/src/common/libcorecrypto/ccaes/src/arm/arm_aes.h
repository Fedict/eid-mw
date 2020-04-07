#ifndef __NO_ASM__
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
 Issue 31/01/2006

 This file contains the definitions required to use AES in C. See aesopt.h
 for optimisation details.
*/

#ifndef _CORECRYPTO_ARM_AES_H_
#define _CORECRYPTO_ARM_AES_H_


#if defined(__cplusplus)
extern "C"
{
#endif

#define AES_128     /* define if AES with 128 bit keys is needed    */
#define AES_192     /* define if AES with 192 bit keys is needed    */
#define AES_256     /* define if AES with 256 bit keys is needed    */
#define AES_VAR     /* define if a variable key size is needed      */
#define AES_MODES   /* define if support is needed for modes        */

/* The following must also be set in assembler files if being used  */

#define AES_ENCRYPT /* if support for encryption is needed          */
#define AES_DECRYPT /* if support for decryption is needed          */
#define AES_ERR_CHK /* for parameter checks & error return codes    */
#define AES_REV_DKS /* define to reverse decryption key schedule    */

#define AES_BLOCK_SIZE  16  /* the AES block size in bytes          */
#define N_COLS           4  /* the number of columns in the state   */

typedef	unsigned int    uint_32t;
typedef unsigned char   uint_8t;
typedef unsigned short  uint_16t;
typedef unsigned char   aes_08t;
typedef	unsigned int    aes_32t;

#define void_ret  void
#define int_ret   int

/* The key schedule length is 11, 13 or 15 16-byte blocks for 128,  */
/* 192 or 256-bit keys respectively. That is 176, 208 or 240 bytes  */
/* or 44, 52 or 60 32-bit words.                                    */

#if defined( AES_VAR ) || defined( AES_256 )
#define KS_LENGTH       60
#elif defined( AES_192 )
#define KS_LENGTH       52
#else
#define KS_LENGTH       44
#endif

#if defined( AES_ERR_CHK )
#define aes_ret     int
#define aes_good    0
#define aes_error  -1
#else
#define aes_ret     void
#endif

#define aes_rval    aes_ret

typedef struct
{   aes_32t ks[KS_LENGTH];
    aes_32t rn;
} ccaes_arm_encrypt_ctx;

typedef struct
{   aes_32t ks[KS_LENGTH];
    aes_32t rn;
} ccaes_arm_decrypt_ctx;


typedef struct
{   
	ccaes_arm_decrypt_ctx decrypt;
    ccaes_arm_encrypt_ctx encrypt;
} ccaes_arm_ctx;


/* implemented in case of wrong call for fixed tables */

void gen_tabs(void);


/* Key lengths in the range 16 <= key_len <= 32 are given in bytes, */
/* those in the range 128 <= key_len <= 256 are given in bits       */

#if defined( AES_ENCRYPT )

#if defined(AES_128) || defined(AES_VAR)
aes_rval ccaes_arm_encrypt_key128(const unsigned char *key, ccaes_arm_encrypt_ctx cx[1]);
#endif

#if defined(AES_192) || defined(AES_VAR)
aes_rval ccaes_arm_encrypt_key192(const unsigned char *key, ccaes_arm_encrypt_ctx cx[1]);
#endif

#if defined(AES_256) || defined(AES_VAR)
aes_rval ccaes_arm_encrypt_key256(const unsigned char *key, ccaes_arm_encrypt_ctx cx[1]);
#endif

#if defined(AES_VAR) && !defined(__arm64__)
aes_rval ccaes_arm_encrypt_key(const unsigned char *key, int key_len, ccaes_arm_encrypt_ctx cx[1]);
#endif

aes_rval ccaes_arm_encrypt(const unsigned char *in, unsigned char *out, const ccaes_arm_encrypt_ctx cx[1]);

#if !defined(__arm64__)
aes_rval ccaes_arm_encrypt_cbc(const unsigned char *in_blk, const unsigned char *in_iv, unsigned int num_blk,
					 unsigned char *out_blk, const ccaes_arm_encrypt_ctx cx[1]);
#endif

#endif

#if defined( AES_DECRYPT )

#if defined(AES_128) || defined(AES_VAR)
aes_rval ccaes_arm_decrypt_key128(const unsigned char *key, ccaes_arm_decrypt_ctx cx[1]);
#endif

#if defined(AES_192) || defined(AES_VAR)
aes_rval ccaes_arm_decrypt_key192(const unsigned char *key, ccaes_arm_decrypt_ctx cx[1]);
#endif

#if defined(AES_256) || defined(AES_VAR)
aes_rval ccaes_arm_decrypt_key256(const unsigned char *key, ccaes_arm_decrypt_ctx cx[1]);
#endif

#if defined(AES_VAR) && !defined(__arm64__)
aes_rval ccaes_arm_decrypt_key(const unsigned char *key, int key_len, ccaes_arm_decrypt_ctx cx[1]);
#endif

aes_rval ccaes_arm_decrypt(const unsigned char *in, unsigned char *out, const ccaes_arm_decrypt_ctx cx[1]);

#if !defined(__arm64__)
aes_rval ccaes_arm_decrypt_cbc(const unsigned char *in_blk, const unsigned char *in_iv, unsigned int num_blk,
					 unsigned char *out_blk, const ccaes_arm_decrypt_ctx cx[1]);
#endif

#endif


#if defined(__cplusplus)
}
#endif

#endif /* _CORECRYPTO_ARM_AES_H_ */
#endif /* __NO_ASM__ */
