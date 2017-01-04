#ifndef __TOMCRYPT_HASH_H__
#define __TOMCRYPT_HASH_H__

#define USE_SHA1
#define USE_SHA256
#define USE_SHA384
#define USE_SHA512
#define USE_MD5
#define USE_RIPEMD160

#define CRYPT_OK               0
#define CRYPT_INVALID_ARG      1
#define CRYPT_NOP              2
#define CRYPT_FAIL_TESTVECTOR  3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tomcrypt_cfg.h"
#include "tomcrypt_macros.h"
#include "tomcrypt_argchk.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- HASH FUNCTIONS ---- */
#ifdef USE_SHA512
	struct sha512_state
	{
		ulong64 length, state[8];
		unsigned long curlen;
		unsigned char buf[128];
	};
#endif

#ifdef USE_SHA256
	struct sha256_state
	{
		ulong64 length;
		ulong32 state[8], curlen;
		unsigned char buf[64];
	};
#endif

#ifdef USE_SHA1
	struct sha1_state
	{
		ulong64 length;
		ulong32 state[5], curlen;
		unsigned char buf[64];
	};
#endif

#ifdef USE_MD5
	struct md5_state
	{
		ulong64 length;
		ulong32 state[4], curlen;
		unsigned char buf[64];
	};
#endif

#ifdef USE_MD4
	struct md4_state
	{
		ulong64 length;
		ulong32 state[4], curlen;
		unsigned char buf[64];
	};
#endif

#ifdef USE_TIGER
	struct tiger_state
	{
		ulong64 state[3], length;
		unsigned long curlen;
		unsigned char buf[64];
	};
#endif

#ifdef USE_MD2
	struct md2_state
	{
		unsigned char chksum[16], X[48], buf[16];
		unsigned long curlen;
	};
#endif

#ifdef USE_RIPEMD128
	struct rmd128_state
	{
		ulong64 length;
		unsigned char buf[64];
		ulong32 curlen, state[4];
	};
#endif

#ifdef USE_RIPEMD160
	struct rmd160_state
	{
		ulong64 length;
		unsigned char buf[64];
		ulong32 curlen, state[5];
	};
#endif

#ifdef USE_WHIRLPOOL
	struct whirlpool_state
	{
		ulong64 length, state[8];
		unsigned char buf[64];
		ulong32 curlen;
	};
#endif

#ifdef CHC_HASH
	struct chc_state
	{
		ulong64 length;
		unsigned char state[MAXBLOCKSIZE], buf[MAXBLOCKSIZE];
		ulong32 curlen;
	};
#endif

	typedef union Hash_state
	{
#ifdef CHC_HASH
		struct chc_state chc;
#endif
#ifdef USE_WHIRLPOOL
		struct whirlpool_state whirlpool;
#endif
#ifdef USE_SHA512
		struct sha512_state sha512;
#endif
#ifdef USE_SHA256
		struct sha256_state sha256;
#endif
#ifdef USE_SHA1
		struct sha1_state sha1;
#endif
#ifdef USE_MD5
		struct md5_state md5;
#endif
#ifdef USE_MD4
		struct md4_state md4;
#endif
#ifdef USE_MD2
		struct md2_state md2;
#endif
#ifdef USE_TIGER
		struct tiger_state tiger;
#endif
#ifdef USE_RIPEMD128
		struct rmd128_state rmd128;
#endif
#ifdef USE_RIPEMD160
		struct rmd160_state rmd160;
#endif
	} hash_state;

	extern struct ltc_hash_descriptor
	{

    /** name of hash */
		char *name;

    /** internal ID */
		unsigned char ID;

    /** Size of digest in octets */
		unsigned long hashsize;

    /** Input block size in octets */
		unsigned long blocksize;

    /** ASN.1 DER identifier */
		unsigned char DER[64];

    /** Length of DER encoding */
		unsigned long DERlen;

    /** Init a hash state
      @param hash   The hash to initialize
      @return CRYPT_OK if successful
    */
		int (*init) (hash_state * hash);

    /** Process a block of data 
      @param hash   The hash state
      @param in     The data to hash
      @param inlen  The length of the data (octets)
      @return CRYPT_OK if successful
    */
		int (*process) (hash_state * hash, const unsigned char *in,
				unsigned long inlen);

    /** Produce the digest and store it
      @param hash   The hash state
      @param out    [out] The destination of the digest
      @return CRYPT_OK if successful
    */
		int (*done) (hash_state * hash, unsigned char *out);

    /** Self-test
      @return CRYPT_OK if successful, CRYPT_NOP if self-tests have been disabled
    */
		int (*test) (void);
	} hash_descriptor[];

#ifdef CHC_HASH
	int chc_register(int cipher);
	int chc_init(hash_state * md);
	int chc_process(hash_state * md, const unsigned char *in,
			unsigned long inlen);
	int chc_done(hash_state * md, unsigned char *hash);
	int chc_test(void);
	extern const struct ltc_hash_descriptor chc_desc;
#endif

#ifdef USE_WHIRLPOOL
	int whirlpool_init(hash_state * md);
	int whirlpool_process(hash_state * md, const unsigned char *in,
			      unsigned long inlen);
	int whirlpool_done(hash_state * md, unsigned char *hash);
	int whirlpool_test(void);
	extern const struct ltc_hash_descriptor whirlpool_desc;
#endif

#ifdef USE_SHA512
	int sha512_init(hash_state * md);
	int sha512_process(hash_state * md, const unsigned char *in,
			   unsigned long inlen);
	int sha512_done(hash_state * md, unsigned char *hash);
	int sha512_test(void);
	extern const struct ltc_hash_descriptor sha512_desc;
#endif

#ifdef USE_SHA384
#ifndef USE_SHA512
#error SHA512 is required for SHA384
#endif
	int sha384_init(hash_state * md);
#define sha384_process sha512_process
	int sha384_done(hash_state * md, unsigned char *hash);
	int sha384_test(void);
	extern const struct ltc_hash_descriptor sha384_desc;
#endif

#ifdef USE_SHA256
	int sha256_init(hash_state * md);
	int sha256_process(hash_state * md, const unsigned char *in,
			   unsigned long inlen);
	int sha256_done(hash_state * md, unsigned char *hash);
	int sha256_test(void);
	extern const struct ltc_hash_descriptor sha256_desc;

#ifdef SHA224
#ifndef SHA256
#error SHA256 is required for SHA224
#endif
	int sha224_init(hash_state * md);
#define sha224_process sha256_process
	int sha224_done(hash_state * md, unsigned char *hash);
	int sha224_test(void);
	extern const struct ltc_hash_descriptor sha224_desc;
#endif
#endif

#ifdef USE_SHA1
	int sha1_init(hash_state * md);
	int sha1_process(hash_state * md, const unsigned char *in,
			 unsigned long inlen);
	int sha1_done(hash_state * md, unsigned char *hash);
	int sha1_test(void);
	extern const struct ltc_hash_descriptor sha1_desc;
#endif

#ifdef USE_MD5
	int md5_init(hash_state * md);
	int md5_process(hash_state * md, const unsigned char *in,
			unsigned long inlen);
	int md5_done(hash_state * md, unsigned char *hash);
	int md5_test(void);
	extern const struct ltc_hash_descriptor md5_desc;
#endif

#ifdef USE_MD4
	int md4_init(hash_state * md);
	int md4_process(hash_state * md, const unsigned char *in,
			unsigned long inlen);
	int md4_done(hash_state * md, unsigned char *hash);
	int md4_test(void);
	extern const struct ltc_hash_descriptor md4_desc;
#endif

#ifdef USE_MD2
	int md2_init(hash_state * md);
	int md2_process(hash_state * md, const unsigned char *in,
			unsigned long inlen);
	int md2_done(hash_state * md, unsigned char *hash);
	int md2_test(void);
	extern const struct ltc_hash_descriptor md2_desc;
#endif

#ifdef USE_TIGER
	int tiger_init(hash_state * md);
	int tiger_process(hash_state * md, const unsigned char *in,
			  unsigned long inlen);
	int tiger_done(hash_state * md, unsigned char *hash);
	int tiger_test(void);
	extern const struct ltc_hash_descriptor tiger_desc;
#endif

#ifdef USE_RIPEMD128
	int rmd128_init(hash_state * md);
	int rmd128_process(hash_state * md, const unsigned char *in,
			   unsigned long inlen);
	int rmd128_done(hash_state * md, unsigned char *hash);
	int rmd128_test(void);
	extern const struct ltc_hash_descriptor rmd128_desc;
#endif

#ifdef USE_RIPEMD160
	int rmd160_init(hash_state * md);
	int rmd160_process(hash_state * md, const unsigned char *in,
			   unsigned long inlen);
	int rmd160_done(hash_state * md, unsigned char *hash);
	int rmd160_test(void);
	extern const struct ltc_hash_descriptor rmd160_desc;
#endif

	int find_hash(const char *name);
	int find_hash_id(unsigned char ID);
	int find_hash_any(const char *name, int digestlen);
	int register_hash(const struct ltc_hash_descriptor *hash);
	int unregister_hash(const struct ltc_hash_descriptor *hash);
	int hash_is_valid(int idx);

	int hash_memory(int hash,
			const unsigned char *in, unsigned long inlen,
			unsigned char *out, unsigned long *outlen);
	int hash_memory_multi(int hash, unsigned char *out,
			      unsigned long *outlen, const unsigned char *in,
			      unsigned long inlen, ...);
	int hash_filehandle(int hash, FILE * in, unsigned char *out,
			    unsigned long *outlen);
	int hash_file(int hash, const char *fname, unsigned char *out,
		      unsigned long *outlen);

/* a simple macro for making hash "process" functions */
#define HASH_PROCESS(func_name, compress_name, state_var, block_size)                       \
int func_name (hash_state * md, const unsigned char *in, unsigned long inlen)               \
{                                                                                           \
    unsigned long n;                                                                        \
    int           err;                                                                      \
    LTC_ARGCHK(md != NULL);                                                                    \
    LTC_ARGCHK(in != NULL);                                                                   \
    if (md-> state_var .curlen > sizeof(md-> state_var .buf)) {                             \
       return CRYPT_INVALID_ARG;                                                            \
    }                                                                                       \
    while (inlen > 0) {                                                                       \
        if (md-> state_var .curlen == 0 && inlen >= block_size) {                             \
           if ((err = compress_name (md, (unsigned char *)in)) != CRYPT_OK) { \
              return err;         \
           }                                        \
           md-> state_var .length += block_size * 8;                                        \
           in             += block_size;                                                   \
           inlen          -= block_size;                                                   \
        } else {                                                                            \
           n = MIN(inlen, (block_size - md-> state_var .curlen));                             \
           memcpy(md-> state_var .buf + md-> state_var.curlen, in, (size_t)n);             \
           md-> state_var .curlen += n;                                                     \
           in             += n;                                                            \
           inlen          -= n;                                                            \
           if (md-> state_var .curlen == block_size) {                                      \
              if ((err = compress_name (md, md-> state_var .buf)) != CRYPT_OK) {\
                 return err;                                      \
              } \
              md-> state_var .length += 8*block_size;                                       \
              md-> state_var .curlen = 0;                                                   \
           }                                                                                \
       }                                                                                    \
    }                                                                                       \
    return CRYPT_OK;                                                                        \
}

#ifdef __cplusplus
}
#endif

#endif			     /* __TOMCRYPT_HASH_H__ */
