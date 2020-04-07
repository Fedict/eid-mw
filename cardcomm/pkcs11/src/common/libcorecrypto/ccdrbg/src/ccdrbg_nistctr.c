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



/*
 * NIST SP 800-90 CTR_DRBG (Random Number Generator)
 */

/*
 In English, this is a Deterministic Random Bit Generator,
 a.k.a. Pseudo-Random Number Generator.

 Strictly speaking, a DRBG is the output stage of a PRNG that
 needs to be seeded from an entropy source. For a full discussion
 of DRBGs, look at NIST SP 800-90. You can search for it. They
 define DRBGs based on hash functions, HMACs, ciphers in counter
 mode, and elliptic curves. This is the cipher one, using AES.
 It's been written and tested with AES-128. It should be generic
 enough to use with AES-256, but is presently untested.

 A DRBG has four routines:
 instantiate()
 generate()
 reseed()
 destroy()

 Further discussion of each routine is below. This implements the
 DRBG with a derivation function, and is intended to be used with
 prediction resistance, but that has to be done in an upper level
 with the entropy source.

 The typical usage is that instantiate() creates the DRBG and
 feeds it its initial entropy, along with a nonce, and optional
 personalization information. The generate() function generates
 random bits. The reseed() function reseeds it with more entropy.
 The destroy() function clears and deallocates the structures.

 Note that while a DRBG is a "bit" generator, this package
 generates bytes. If you need less than a byte, extract it.

 A DRBG must be reseeded every so often. You can get the number
 of calls to it remaining before a mandatory reseed from
 CCADRBGGetReseedCountdown().

 Note that this DRBG is not thread-safe. Its callers must not
 only manage entropy for it, but they must use it appropriately.

 Fortunately, CommonRNG.[ch] has a nice implementation of all that,
 and is probably what you should be using.

 */


#include <corecrypto/ccdrbg.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/cc_priv.h>
#include "ccdrbg_nistctr.h"
#include <corecrypto/cc_debug.h> //cc_debug.h defines printf() used in cc_macroes.h both in DEBUG and CC_KERNEL modes
#include <corecrypto/cc_macros.h>

#define NONFIPSINC128 0

//
// Utility functions
//


//
// increment_bigend_128
// Increment the counter-mode counter as a big-endian number.
//
// If this is not done big-endian, then then NIST test vectors will fail.
// However, there is no security change to do it little-endian, it just
// gives different results. It is therefore possible to get a speed up
// when not in FIPS-mode by doing it little-endian. The NOFIPS version of
// this routine does it little-endian.
//
// The NOFIPS128 constant compiles in an option to do this non-FIPS speedup.
// Note that we have it turned off because it's not a big speedup when
// compared to everything else we have to do. It just gets lost in all the
// AES operations.
//

static void
increment_bigend_128(struct ccdrbg_nistctr_state *drbg, uint32_t *V)
{

	signed long i;
	uint32_t x;

#if 0
    // this is to test generate_block(), when generated blocks are (by error) equal.
    // abort() will be called
    static int disable_counter = 0;
    if(disable_counter++>20)
        return;
#endif

    for (i = CCADRBG_OUTLEN_INTS(drbg) - 1; i >= 0; --i)
	{
		x = CC_H2BE32(V[i]) + 1;
		V[i] = CC_H2BE32(x);
		if (x)	// There was only a carry if we are zero
			return;
	}
}

#if NONFIPSINC128
static void
increment_bigend_128_NOFIPS(struct ccdrbg_nistctr_state *drbg, uint32_t *V)
{
	int32_t		i;
	uint32_t	x;

	for (i = CCADRBG_OUTLEN_INTS - 1; i >= 0; --i)
	{
		x = V[i] + 1;
		V[i] = x;
		if (x)	/* There was only a carry if we are zero */
			return;
	}
}
#endif

/*
 * NIST SP 800-90 March 2007
 * 10.4.3 BCC Function
 */
static void
bcc_update(struct ccdrbg_nistctr_state *drbg, const uint32_t *data, unsigned long n, uint32_t *chaining_value)
{
	unsigned long	i, j;
	uint32_t		input_block[CCADRBG_OUTLEN_INTS(drbg)];

	/* [4] for i = 1 to n */
	for (i = 0; i < n; ++i)
	{
		/* [4.1] input_block = chaining_value XOR block_i */
		for (j = 0; j < CCADRBG_OUTLEN_INTS(drbg); ++j)
			input_block[j] = chaining_value[j] ^ *data++;

		/* [4.2] chaining_value = Block_Encrypt(Key, input_block) */
        drbg->ecb->ecb(drbg->df_key, 1, input_block, chaining_value);
    }

	/* [5] output_block = chaining_value */
	/* chaining_value already is output_block, so no copy is required */
}

static void
bcc(struct ccdrbg_nistctr_state *drbg, const uint32_t *data, unsigned long n, uint32_t *output_block)
{
	uint32_t	*chaining_value = output_block;

	/* [1] chaining_value = 0^outlen */
	cc_clear(CCADRBG_OUTLEN(drbg),&chaining_value[0]);

	bcc_update(drbg, data, n, output_block);
}

/*
 * NIST SP 800-90 March 2007
 * 10.4.2 Derivation Function Using a Block Cipher Algorithm
 */

static __inline int
check_int_alignment(const void *p)
{
	/*
	 * It would be great if "intptr_t" could be found in
	 * some standard place.
	 */
	intptr_t ip = (const char *)p - (const char *)0;

	if (ip & (intptr_t)(sizeof(int) - 1))
		return 0;

	return 1;
}



static void
df_bcc_update(struct ccdrbg_nistctr_state *drbg, const char *input_string, unsigned long input_string_length, uint32_t *temp)
{
	unsigned long i, len;
	_CCADRBG_BCC	*ctx = &drbg->bcc;
	unsigned long	idx = ctx->index;
	uint8_t         *S = (uint8_t *)ctx->S;

	if (idx)
	{
		cc_assert(idx < CCADRBG_OUTLEN(drbg));
		len = CCADRBG_OUTLEN(drbg) - idx;
		if (input_string_length < len)
			len = input_string_length;

		memcpy(&S[idx], input_string, len);

        cc_assert(len <= 0xFFFFffff);
		idx += len;
		input_string += len;
		input_string_length -= len;

		if (idx < CCADRBG_OUTLEN(drbg))
		{
			ctx->index = idx;
			return;
		}

		/* We have a full block in S, so let's process it */
		/* [9.2] BCC */
		bcc_update(drbg, ctx->S, 1, temp);
		idx = 0;
	}

	/* ctx->S is empty, so let's handle as many input blocks as we can */
	len = input_string_length / CCADRBG_OUTLEN(drbg);
	if (len > 0)
	{

#if 1
		if (check_int_alignment(input_string))
		{
			/* [9.2] BCC */
			bcc_update(drbg, (const uint32_t *)input_string, len, temp);

			input_string += len * CCADRBG_OUTLEN(drbg);
			input_string_length -= len * CCADRBG_OUTLEN(drbg);
		}
		else
#endif
		{
			for (i = 0; i < len; ++i)
			{
				memcpy(&S[0], input_string, CCADRBG_OUTLEN(drbg));

				/* [9.2] BCC */
				bcc_update(drbg, ctx->S, 1, temp);

				input_string += CCADRBG_OUTLEN(drbg);
				input_string_length -= CCADRBG_OUTLEN(drbg);
			}
		}
	}

	cc_assert(input_string_length < CCADRBG_OUTLEN(drbg));

	if (input_string_length)
	{
		memcpy(&S[0], input_string, input_string_length);
		idx = input_string_length;
	}

	ctx->index = idx;
}

static void
bcc_init(struct ccdrbg_nistctr_state * drbg, uint32_t L, uint32_t N, uint32_t *temp)
{
	uint32_t S[2];

    drbg->bcc.index = 0;

	/* [4] S = L || N || input_string || 0x80 */
	S[0] = CC_H2BE32(L);
	S[1] = CC_H2BE32(N);

    df_bcc_update(drbg, (const char *)S, sizeof(S), temp);
}

static void
df_bcc_final(struct ccdrbg_nistctr_state * drbg, uint32_t *temp)
{
	unsigned long idx;
	_CCADRBG_BCC		*ctx = &drbg->bcc;
	static const char	endmark[] = { (char)0x80 };
	uint8_t	*S = (uint8_t *)ctx->S;

	df_bcc_update(drbg, endmark, sizeof(endmark), temp);

	idx = ctx->index;
	if (idx)
	{
		cc_clear(CCADRBG_OUTLEN(drbg) - idx,&S[idx]);

		/* [9.2] BCC */
		bcc_update(drbg, (uint32_t *)&S[0], 1, temp);
	}
}

static int
df(struct ccdrbg_nistctr_state *drbg, const char *input_string[], uint32_t L[],
    uint32_t input_string_count, uint8_t *output_string, unsigned long N)
{
	unsigned long   j, k, blocks;
    uint64_t        sum_L;
	uint32_t		*temp;
	uint32_t		*X;
	uint32_t		buffer[CCADRBG_TEMPLEN_INTS(drbg)];
    /* declare a key */
    ccecb_ctx_decl(drbg->ecb->size, key);

	/*
	 * NIST SP 800-90 March 2007 10.4.2 states that 512 bits is
	 * the maximum length for the approved block cipher algorithms.
     *
     * Also states that L(sum_L) and N are 32 bits integers.
	 */
    cc_assert(drbg->ecb->block_size<=512/8);
	uint32_t output_buffer[512 / 8 / sizeof(uint32_t)];

	if (N > sizeof(output_buffer) || N < 1)
	{
		ccecb_ctx_clear(drbg->ecb->size, key);
		return -1;
	}
		

	sum_L = 0;
	for (j = 0; j < input_string_count; ++j)
		sum_L += L[j];
    //sum_L is the sum of the all input data-lengths. Since maximum parameters lengths are set properly
    //in the header file, sum_L cannot be more than 32 bits. But a change to those parameters by
    //someone who is not aware of this summation here, would be a disaster.
    //Therefore, we make sum_L 64 bits and we perform the test here.
    if(sum_L > 0xFFFFffff)
        return -1;

	/* [6] temp = Null string */
	temp = buffer;

	/* [9] while len(temp) < keylen + outlen, do */
	for (j = 0; j < CCADRBG_TEMPLEN_BLOCKS(drbg); ++j)
	{
		/* [9.2] temp = temp || BCC(K, (IV || S)) */

		/* Since we have precomputed BCC(K, IV), we start with that... */
		memcpy(&temp[0], &drbg->encryptedIV[j*CCADRBG_OUTLEN(drbg)+0], CCADRBG_OUTLEN(drbg));

        /* typecast: ok, checks above */
		bcc_init(drbg, (uint32_t)sum_L, (uint32_t)N, temp);

		/* Compute the rest of BCC(K, (IV || S)) */
		for (k = 0; k < input_string_count; ++k)
			df_bcc_update(drbg, input_string[k], L[k], temp);

		df_bcc_final(drbg, temp);

		temp += CCADRBG_OUTLEN_INTS(drbg);
	}

	/* [6] temp = Null string */
	temp = buffer;

	/* [10] K = Leftmost keylen bits of temp */
    drbg->ecb->init(drbg->ecb, key, CCADRBG_KEYLEN(drbg), &temp[0]);

	/* [11] X = next outlen bits of temp */
	X = &temp[CCADRBG_KEYLEN_INTS(drbg)];

	/* [12] temp = Null string */
	temp = output_buffer;

	/* [13] While len(temp) < number_of_bits_to_return, do */
	blocks = (N / CCADRBG_OUTLEN(drbg));
	if (N & (CCADRBG_OUTLEN(drbg) - 1))
		++blocks;
	for (j = 0; j < blocks; ++j)
	{
		/* [13.1] X = Block_Encrypt(K, X) */
        drbg->ecb->ecb(key, 1, X, temp);
		X = temp;
		temp += CCADRBG_OUTLEN_INTS(drbg);
	}

	/* [14] requested_bits = Leftmost number_of_bits_to_return of temp */
	memcpy(output_string, output_buffer, N);
	ccecb_ctx_clear(drbg->ecb->size, key);
	cc_clear(sizeof(buffer), buffer);
	cc_clear(sizeof(output_buffer), output_buffer);

	return 0;
}


static void
df_initialize(struct ccdrbg_nistctr_state * drbg)
{
	uint32_t		i;
	uint8_t			K[CCADRBG_KEYLEN(drbg)];
	uint32_t		IV[CCADRBG_OUTLEN_INTS(drbg)];

	/* [8] K = Leftmost keylen bits of 0x00010203 ... 1D1E1F */
	for (i = 0; i < sizeof(K); ++i)
		K[i] = (uint8_t)i;

    drbg->ecb->init(drbg->ecb, drbg->df_key, sizeof(K), K);

	/*
	 * Precompute the partial BCC result from encrypting the IVs:
	 *     encryptedIV[i] = BCC(K, IV(i))
	 */

	/* [7] i = 0 */
	/* [9.1] IV = i || 0^(outlen - len(i)) */
	cc_clear(sizeof(IV),&IV[0]);

		/* [9.3] i = i + 1 */
	for (i = 0; i < CCADRBG_TEMPLEN_BLOCKS(drbg); ++i)
	{
		/* [9.1] IV = i || 0^(outlen - len(i)) */
		IV[0] = CC_H2BE32(i);

		/* [9.2] temp = temp || BCC(K, (IV || S))  (the IV part, at least) */
		bcc(drbg, &IV[0], 1, (uint32_t *)&drbg->encryptedIV[i*CCADRBG_OUTLEN(drbg)+0]);
	}
}

/*
 * NIST SP 800-90 March 2007
 * 10.2.1.2 The Update Function
 */
static int
drbg_update(struct ccdrbg_nistctr_state * drbg, const uint32_t *provided_data)
{
	uint32_t		i;
    uint32_t		temp[CCADRBG_TEMPLEN_INTS(drbg)];
	uint32_t		*output_block;
	
    /* Clear temp buffer */
    cc_clear(sizeof(temp),temp);

	/* 2. while (len(temp) < seedlen) do */
	for (output_block = temp; output_block < &temp[CCADRBG_SEEDLEN_INTS(drbg)];
		output_block += CCADRBG_OUTLEN_INTS(drbg))
	{
		/* 2.1 V = (V + 1) mod 2^outlen */
#if NONFIPSINC128
		(drbg->inc128)(drbg, drbg->V);
#else
		increment_bigend_128(drbg, drbg->V);
#endif
		/* 2.2 output_block = Block_Encrypt(K, V) */

        drbg->ecb->ecb(drbg->key, 1, drbg->V, output_block);
    }

	// check that the two halves are not the same.
    unsigned char* tempPtr = (unsigned char*)temp;
	if (!cc_cmp_safe(CCADRBG_KEYLEN(drbg), tempPtr, &tempPtr[CCADRBG_KEYLEN(drbg)]))
    {
		cc_clear(sizeof(temp), temp);
		return CCDRBG_STATUS_ERROR;
	}

	/* 3 temp is already of size seedlen (CCADRBG_SEEDLEN_INTS) */

	/* 4 (part 1) temp = temp XOR provided_data */
	for (i = 0; i < CCADRBG_KEYLEN_INTS(drbg); ++i)
		temp[i] ^= *provided_data++;

	/* 5 Key = leftmost keylen bits of temp */
    drbg->ecb->init(drbg->ecb, drbg->key, CCADRBG_KEYLEN(drbg), &temp[0]);
	/* 4 (part 2) combined with 6 V = rightmost outlen bits of temp */
	for (i = 0; i < CCADRBG_OUTLEN_INTS(drbg); ++i)
		drbg->V[i] = temp[CCADRBG_KEYLEN_INTS(drbg) + i] ^ *provided_data++;
		
	cc_clear(sizeof(temp), temp);
	return CCDRBG_STATUS_OK;
}


//make sure drbg is initialized, before calling this function
static int validate_inputs(struct ccdrbg_nistctr_state *drbg,
                           unsigned long entropyLength,
                           unsigned long additionalInputLength,
                           unsigned long psLength)
{
    int rc=CCDRBG_STATUS_PARAM_ERROR;
    
    cc_require(drbg->keylen<=CCADRBG_MAX_KEYLEN, end); //keylen too long
    
    //NIST SP800 compliance checks
     if(drbg->use_df){
        cc_require (psLength <= CCDRBG_MAX_PSINPUT_SIZE, end); //personalization string too long
        cc_require (entropyLength <= CCDRBG_MAX_ENTROPY_SIZE, end); //supplied too much entropy
        cc_require (additionalInputLength <= CCDRBG_MAX_ADDITIONALINPUT_SIZE, end); //additional input too long
        cc_require (entropyLength >= drbg->ecb->block_size, end); //supplied too litle entropy
    }else{
        unsigned long seedlen = CCADRBG_SEEDLEN(drbg);  //outlen + keylen
        
        cc_require (psLength <= seedlen, end); //personalization string too long
        cc_require (entropyLength == seedlen, end); //supplied too much or too little entropy
        cc_require (additionalInputLength <= seedlen, end); //additional input too long
    }
    
    rc=CCDRBG_STATUS_OK;
end:
    return rc;
}

/*
 * NIST SP 800-90 March 2007
 * 10.2.1.4.2 The Process Steps for Reseeding When a Derivation
 *            Function is Used
 */
static int
reseed(struct ccdrbg_state *rng,
    unsigned long entropyLength, const void *entropy,
    unsigned long additionalLength, const void *additional)
{
	int         err;
    uint32_t    count;
	const char	*input_string[2];
	uint32_t	length[2];
    struct ccdrbg_nistctr_state *drbg=(struct ccdrbg_nistctr_state *)rng;
	uint32_t	seed_material[CCADRBG_SEEDLEN_INTS(drbg)];

    
    err =validate_inputs(drbg, entropyLength, additionalLength, 0); if(err!=CCDRBG_STATUS_OK) return err;
    
    if(drbg->use_df) {
        /* [1] seed_material = entropy || additional */
        input_string[0] = entropy;
        /* typecast: guaranteed to fit by the above checks */
        length[0] = (uint32_t)entropyLength;
        count = 1;

        if (additional && additionalLength)
        {
            input_string[count] = additional;
            /* typecast: guaranteed to fit by above checks */
            length[count] = (uint32_t)additionalLength;
            ++count;
        }

        /* [2] seed_material = Block_Cipher_df(seed_material, seedlen) */
        err = df(drbg, input_string, length, count,
                (uint8_t *)seed_material, sizeof(seed_material));
        if (err)
            return err;
    } else {
        cc_clear(sizeof(seed_material),seed_material);
        cc_assert(additionalLength==0 || additionalLength==sizeof(seed_material)); //additionalLength is validated above
        CC_MEMCPY(seed_material, additional, additionalLength);
        cc_xor(CCADRBG_SEEDLEN(drbg), seed_material, seed_material, entropy);
    }

	/* [3] (Key, V) = Update(seed_material, Key, V) */
	if (drbg_update(drbg, seed_material))
	{
		return CCDRBG_STATUS_PARAM_ERROR;
	}

	/* [4] reseed_counter = 1 */
	drbg->reseed_counter = 1;

	return CCDRBG_STATUS_OK;
}

static void
done(struct ccdrbg_state *rng)
{

    struct ccdrbg_nistctr_state *drbg=(struct ccdrbg_nistctr_state *)rng;
    unsigned long bs=drbg->ecb->block_size;
    cc_clear((((drbg->keylen + bs * 2-1)/bs)*bs),drbg->encryptedIV);
    cc_clear(bs, drbg->V);
    cc_clear(drbg->keylen + bs, drbg->nullInput);
    cc_clear(bs, drbg->bcc.S);
    cc_clear(drbg->ecb->size, drbg->key);
    cc_clear(drbg->ecb->size, drbg->df_key);

    cc_clear(sizeof(*drbg), drbg);

    // Possibly superfluous, but NIST wants it.
    drbg->reseed_counter = UINT32_MAX;
}

/*
 * NIST SP 800-90 March 2007
 * 10.2.1.5.2 The Process Steps for Generating Pseudorandom Bits When a
 *            Derivation Function is Used for the DRBG Implementation
 */
static int
gen_block(struct ccdrbg_nistctr_state * drbg, void *output_block)
{
    /* [4.1] V = (V + 1) mod 2^outlen */
    increment_bigend_128(drbg, &drbg->V[0]);

    /* [4.2] output_block = Block_Encrypt(Key, V) */
    drbg->ecb->ecb(drbg->key, 1, drbg->V, output_block);

    return CCDRBG_STATUS_OK;
}


static int
generate_block(struct ccdrbg_nistctr_state * drbg, uint32_t	*blk)
{
    int rc;
    uint8_t	temp2[CCADRBG_BLOCKSIZE(drbg)];

    // the folowing lines are performed as rquested in Radar 19129408
    if (drbg->strictFIPS)
    {
        rc = gen_block(drbg, temp2); cc_require(rc==CCDRBG_STATUS_OK, errOut);
        rc = gen_block(drbg, blk);   cc_require(rc==CCDRBG_STATUS_OK, errOut);
        rc = cc_cmp_safe(CCADRBG_BLOCKSIZE(drbg), blk, temp2);
        rc = (rc==0) ? CCDRBG_STATUS_ABORT: CCDRBG_STATUS_OK;
errOut:
        cc_clear(CCADRBG_BLOCKSIZE(drbg), temp2);

    }else{
        rc=gen_block(drbg, blk);
    }

    if(rc==CCDRBG_STATUS_ABORT){
        //The world as we know it has come to an end
        //the DRBG data structure is zeroized. subsequent calls to
        //DRBG ends up in NULL dereferencing and/or unpredictable state.
        //catastrophic error in SP 800-90A
        done((struct ccdrbg_state *)drbg);
        cc_abort(NULL);
    }
    
    return rc;

}


static int validate_gen_params(struct ccdrbg_nistctr_state *drbg,  unsigned long dataOutLength, unsigned long additionalLength)
{
    int rc=CCDRBG_STATUS_PARAM_ERROR;
    
    
    cc_require (dataOutLength >= 1, end); //Requested zero byte in one request
    cc_require (dataOutLength <= CCDRBG_MAX_REQUEST_SIZE, end); //Requested too many bytes in one request
    
    unsigned long max = drbg->use_df? CCDRBG_MAX_ADDITIONALINPUT_SIZE:CCADRBG_SEEDLEN(drbg);
    cc_require (additionalLength<=max, end); //Additional input too long
        
    
    // 1. If (reseed_counter > 2^^48), then Return (“Reseed required”, Null, V, Key, reseed_counter).
    cc_assert(sizeof(drbg->reseed_counter>=8)); //make sure it fits 2^48
    rc = CCDRBG_STATUS_NEED_RESEED;
    cc_require (drbg->reseed_counter <= CCDRBG_RESEED_INTERVAL || !drbg->strictFIPS, end); //Reseed required
   
    rc=CCDRBG_STATUS_OK;
end:
    return rc;
}


static int
generate(struct ccdrbg_state *rng,
         unsigned long dataOutLength, void *dataOut,
         unsigned long additionalLength, const void *additional)
{
    int rc = CCDRBG_STATUS_OK;
    unsigned long i;
    unsigned long len;
    uint8_t		*p;
    uint32_t	*temp;
    const char	*input_string[1];
    uint32_t	length[1];
    struct ccdrbg_nistctr_state *drbg = (struct ccdrbg_nistctr_state *)rng;
    uint32_t	buffer[CCADRBG_OUTLEN(drbg)];
    uint32_t	additional_buffer[CCADRBG_SEEDLEN_INTS(drbg)];
    unsigned long blocks = dataOutLength / CCADRBG_OUTLEN(drbg);

    /* [1] If reseed_counter > reseed_interval ... */
    rc = validate_gen_params(drbg, dataOutLength, (additional !=NULL)?additionalLength:0); cc_require(rc==CCDRBG_STATUS_OK, errOut);

    /* [2] If (addional_input != Null), then */
    if (additional && additionalLength)
    {
        if(drbg->use_df) {
            input_string[0] = additional;
            /* typecast: guaranteed to fit by the checks above */
            length[0] = (uint32_t)additionalLength;
            /* [2.1] additional = Block_Cipher_df(additional, seedlen) */
            rc = df(drbg, input_string, length, 1,
                    (uint8_t *)additional_buffer, sizeof(additional_buffer));
            cc_require(rc==CCDRBG_STATUS_OK, errOut);
        } else {
            cc_clear(sizeof(additional_buffer), additional_buffer);
            cc_assert(additionalLength==0 || additionalLength==sizeof(additional_buffer)); //additionalLength is validated above
            CC_MEMCPY(additional_buffer, additional, additionalLength);
        }

        /* [2.2] (Key, V) = Update(additional, Key, V) */
        rc=drbg_update(drbg, additional_buffer); cc_require(rc==CCDRBG_STATUS_OK, errOut);
    }

    if (blocks && check_int_alignment(dataOut))
    {
        /* [3] temp = Null */
        temp = (uint32_t *)dataOut;
        for (i = 0; i < blocks; ++i)
        {
            // Here is the conundrum that is FIPS.
            // In order to test the DRBG for CAVS one must NOT set strictFIPS
            // and thus not have a compliant DRBG.  That is the only way to
            // ensure that the CAVS test will pass.  On the other hand the
            // 'normal' usage of the DRBG MUST set the strictFIPS flag.  So
            // that which is used by our customers is NOT what was tested.
            rc = generate_block(drbg, temp); cc_require(rc==CCDRBG_STATUS_OK, errOut);
            temp += CCADRBG_OUTLEN_INTS(drbg);
            dataOutLength -= CCADRBG_OUTLEN(drbg);
        }

        dataOut = (uint8_t *)temp;
    }

    /* [3] temp = Null */
    temp = buffer;

    len = CCADRBG_OUTLEN(drbg);

    /* [4] While (len(temp) < requested_number_of_bits) do: */
    p = dataOut;
    while (dataOutLength > 0)
    {
        // See note above.
        rc = generate_block(drbg, temp); cc_require(rc==CCDRBG_STATUS_OK, errOut);
        if (dataOutLength < CCADRBG_OUTLEN(drbg))
            len = dataOutLength;

        memcpy(p, temp, len);

        p += len;
        dataOutLength -= len;
    }

    /* [6] (Key, V) = Update(additional, Key, V) */
    rc = drbg_update(drbg, additional && additionalLength ? &additional_buffer[0] :
                     &drbg->nullInput[0]);
    cc_require(rc==CCDRBG_STATUS_OK, errOut);
    
    /* [7] reseed_counter = reseed_counter + 1 */
    ++drbg->reseed_counter;
    
errOut:
    cc_clear(sizeof(additional_buffer),additional_buffer);
    return rc;
}


/*
static uint64_t
CCADRBGGetReseedCountdown(struct ccdrbg_nistctr_state *drbg)
{
    return (CCDRBG_RESEED_INTERVAL - drbg->reseed_counter);
}
*/

/*
 * NIST SP 800-90 March 2007
 * 10.2.1.3.2 The Process Steps for Instantiation When a Derivation
 *            Function is Used
 */

//length of input personalization string ps might be zero
//nonce is not validated, caller needs to make sure nonce is right as per NIST 800-90A section 8.6.7

static int nistctr_init(const struct ccdrbg_nistctr_custom *custom, struct ccdrbg_nistctr_state *drbg, char *keys,
                        const void* entropy, unsigned long entropyLength,
                        const void* nonce, unsigned long nonceLength,
                        const void* ps, unsigned long psLength
                        )
{
	int         err;
    uint32_t    count;
    char *buf;
    
    drbg->ecb = custom->ecb;
    drbg->keylen = custom->keylen;
    buf=keys;
    
    unsigned long bs=drbg->ecb->block_size;
    drbg->encryptedIV = (uint8_t *)buf; buf+=((drbg->keylen+bs*2-1)/bs)*bs;
    drbg->V = (uint32_t *)buf; buf+=bs; //CCADRBG_OUTLEN(drbg);
    drbg->nullInput = (uint32_t *)buf; buf+=drbg->keylen+bs; //CCADRBG_SEEDLEN(drbg);
    drbg->bcc.S = (uint32_t *)buf; buf+=bs; //CCADRBG_OUTLEN(drbg);
    drbg->key = (ccecb_ctx *)buf; buf+=drbg->ecb->size;
    drbg->df_key = (ccecb_ctx *)buf;

	// First initialize the struct
	drbg->strictFIPS = custom->strictFIPS;
    drbg->use_df = custom->use_df;

#if NONFIPSINC128
	if (strictFIPS)
		drbg->inc128 = increment_bigend_128;
	else
		drbg->inc128 = increment_bigend_128_NOFIPS;
#endif

	for (count = 0; count < CCADRBG_SEEDLEN_INTS(drbg); count++)
		drbg->nullInput[count] = 0;

	// Reseed counter is set in [6] below.
	// V is set in [4] and [5]

	// Initialize the derivation function
	//
    
    //nonce is not checked, caller needs to make sure nonce is right as per NIST 800-90A section 8.6.7
    int rc=validate_inputs(drbg, entropyLength, 0, psLength);
    if(rc!=CCDRBG_STATUS_OK){
        done((struct ccdrbg_state *)drbg);
        return rc;
    }
    
    uint8_t		K[CCADRBG_KEYLEN(drbg)];
    uint32_t	seed_material[CCADRBG_SEEDLEN_INTS(drbg)];

    if(drbg->use_df) {
        uint32_t    length[3];
        const char	*input_string[3];

         df_initialize(drbg);

        /* [1] seed_material = entropy || nonce || ps */

        input_string[0] = entropy;
        /* typecast: guaranteed to fit by above checks */
        length[0] = (uint32_t)entropyLength;

        input_string[1] = nonce;
        /* typecast: guaranteed to fit by above checks */
        length[1] = (uint32_t)nonceLength;

        count = 2;
        if (ps && psLength)
        {
            input_string[count] = ps;
            /* typecast: guaranteed to fit by above checks */
            length[count] = (uint32_t) psLength;
            ++count;
        }
            /* [2] seed_material = Block_Cipher_df(seed_material, seedlen) */
        err = df(drbg, input_string, length, count,
                 (uint8_t *)seed_material, sizeof(seed_material));
        if (err)
		{
			cc_clear(sizeof(seed_material),seed_material);
			done((struct ccdrbg_state *)drbg);
			return err;
		}
            
    } else {
        cc_clear(sizeof(seed_material),seed_material);
        cc_assert(psLength==0 || psLength==sizeof(seed_material)); //pslength is validated above
        CC_MEMCPY(seed_material, ps, psLength);
        cc_xor(CCADRBG_SEEDLEN(drbg), seed_material, seed_material, entropy);
    }

	/* [3] Key = 0^keylen */
	cc_clear(sizeof(K), K);
    drbg->ecb->init(drbg->ecb, drbg->key, sizeof(K), K);

	/* [4] V = 0^outlen */
    cc_clear(CCADRBG_OUTLEN(drbg),drbg->V);

	/* [5] (Key, V) = Update(seed_material, Key, V) */
	if (drbg_update(drbg, seed_material))
	{
		cc_clear(sizeof(seed_material),seed_material);
		done((struct ccdrbg_state *)drbg);
		return CCDRBG_STATUS_PARAM_ERROR;
	}
	cc_clear(sizeof(seed_material),seed_material);

	/* [6] reseed_counter = 1 */
	drbg->reseed_counter = 1;

	return CCDRBG_STATUS_OK;
}


static int init(const struct ccdrbg_info *info, struct ccdrbg_state *drbg,
                unsigned long entropyLength, const void* entropy,
                unsigned long nonceLength, const void* nonce,
                unsigned long psLength, const void* ps)
{

    struct ccdrbg_nistctr_state *prng = (struct ccdrbg_nistctr_state *)drbg;
    const struct ccdrbg_nistctr_custom *custom = info->custom;

    /* Hack! This better be aligned! */
    char *keys = (char *)(prng + 1);

    return nistctr_init(custom, prng, keys,
                        entropy, entropyLength,
                        nonce, nonceLength,
                        ps, psLength);

}


/* This initialize an info object with the right options */
void ccdrbg_factory_nistctr(struct ccdrbg_info *info, const struct ccdrbg_nistctr_custom *custom)
{
    info->size = sizeof(struct ccdrbg_nistctr_state) + CCDRBG_NISTCTR_SIZE(custom->ecb, custom->keylen);
    info->init = init;
    info->generate = generate;
    info->reseed = reseed;
    info->done = done;
    info->custom = custom;
};
