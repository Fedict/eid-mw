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


#include <corecrypto/ccrsa.h>
#include <corecrypto/ccrsa_priv.h>
#include <corecrypto/cc_debug.h>
#include <corecrypto/ccrng_rsafips_test.h>
#include <corecrypto/cc_macros.h>

#define CC_DEBUG_RSAKEYGEN_FIPS186 (CORECRYPTO_DEBUG && 0)

// Utility macros.
#define ccn_cleartop(N,r,partial_n) \
    if((N)>(partial_n)) ccn_zero((N)-(partial_n), (r)+(partial_n))
#define cczp_bitlen(zp) ccn_bitlen(cczp_n(zp), cczp_prime(zp))

// Configuration
#define SEED_X_MAX_RETRIES          100
#define RESEED_MAX_RETRIES          100
#define GENERATE_Q_MAX_RETRIES      100

// Use approximation for sqrt[2]:
// We precompute Sqrt(2)*2^255. Mathematica code snippet:
//  mySqrt2 = IntegerPart[N[Sqrt[2]*2^255, 40]];
//  Print[IntegerString[IntegerDigits[mySqrt2, 256], 16, 2]]

static const cc_unit sqrt_2_n = CCN256_N;
static const cc_unit sqrt_2[] = {CCN256_C(b5,04,f3,33,f9,de,64,84,59,7d,89,b3, \
                75,4a,be,9f,1d,6f,60,ba,89,3b,a8,4c,ed,17,ac,85,83,33,99,15)};

//==============================================================================
//                              Internal functions
//==============================================================================

// Determinate how many iterations of Miller-Rabin must be perform to achieve
// primality provablity in case primality testing relies solely on Miller-Rabin
// testing.
// Based on FIPS 186-4, combination of Table C2 and C3.
// C3 table for p and q being 512bits (2^-100 minium security)
// C2 table for p and q being 1024, 1536 (respectively 2^-112 and 2^128).
static cc_size
ccrsa_fips186_MR_only_iteration_number(cc_size bitlen) {
    cc_size iteration_nb=0;
    // for p1, p2, q1, q2
    if (bitlen<=170) {
        iteration_nb=38;
    }
    else if (bitlen<512) {
        iteration_nb=41;
    }
    // for p, q: 512
    else if (bitlen<1024) {
        iteration_nb=7;
    }
    // for p, q: 1024
    else if (bitlen<1536) {
        iteration_nb=5;
    }
    // for p, q: 1536 and above
    else {
        iteration_nb=4;
    }
    return iteration_nb;
}

// Determinate the bit length of p1, p2 for bit length of p.
// Per FIPS186-4, table Table B.1., p52
static cc_size
ccrsa_fips186_auxiliary_prime_length(cc_size plen) {
    cc_size auxiliary_prime_bitlen;
    // p,q bitlength <= 512: 1024 RSA key size (and below)
    if (plen<=512) {
        auxiliary_prime_bitlen=101;
    }
    // p,q bitlength [512,1024]: 2048 RSA key size
    else if (plen<=1024) {
        auxiliary_prime_bitlen=141;
    }
    // p,q bitlength>1024: 3072 RSA key size (and above)
    else {
        auxiliary_prime_bitlen=171;
    }
    return auxiliary_prime_bitlen;
}

// r = |a-b|
CC_INLINE void ccn_abs(cc_size n, cc_unit *r, const cc_unit *a, const cc_unit *b)
{
    const cc_unit *tmp_ab[2] __attribute__((aligned(16)))={a,b};
    cc_unit C=(1+ccn_cmp(n,a,b))>>1;
    // 0: a<=b
    // 1: a>b
    ccn_sub(n,r,tmp_ab[C^1],tmp_ab[C]); // |a-b|
}

// Check absolute value against delta
// -1,0 => |p-q| or | | <= 2^(plen-100)
// 1    => |u-v| > delta
static bool
cczp_check_delta_100bits(cc_size n,
                         const cc_unit *p, const cc_unit *q,
                         const cc_unit *Xp, const cc_unit *Xq)
{
    cc_unit tmp[n];
    cc_unit delta[n];
    cc_size pbits=ccn_bitlen(n,p);
    int r1,r2;

    // 2^(plen-100)
    ccn_zero(n,delta);
    ccn_set_bit(delta,pbits-100, 1);

    // Abs(p,q)
    ccn_abs(n,tmp,p,q);
    r1=ccn_cmp(n,tmp,delta);

    // Abs(Xp,Xq)
    ccn_abs(n,tmp,Xp,Xq);
    r2=ccn_cmp(n,tmp,delta);

    ccn_clear(n,tmp);
    return ((r1+r2)==2);
}

// Provided a value, find the next prime by increment.
static int
cczp_find_next_prime(cczp_t p) {
    cc_size n = cczp_n(p);
    cc_size MR_iterations=ccrsa_fips186_MR_only_iteration_number(ccn_bitlen(n,cczp_prime(p)));

    // Substract by two, check the value was >= 2.
    if(ccn_sub1(n, CCZP_PRIME(p), cczp_prime(p), 2) != 0) return CCRSA_KEYGEN_NEXT_PRIME_ERROR;

    // Make it odd
    CCZP_PRIME(p)[0] |= 1;

    // Increment until probably prime according to Miller-Rabin.
    do
    {
        // Increment to try again
        if(ccn_add1(n, CCZP_PRIME(p), cczp_prime(p), 2) != 0) return CCRSA_KEYGEN_NEXT_PRIME_ERROR;
        // Exit if we get a carry (integer can't be represented over n units.
    } while (cczp_rabin_miller(p, MR_iterations) == 0);
    return 0; // Prime found
}

// Generate a random number X such that
// (Sqrt(2)*(2^(pbits-1))<= X <= (2^(pbits)-1)
static int
ccn_seed_X(cc_size n, cc_unit *X, cc_size pbits, struct ccrng_state *rng) {
    int status=CCRSA_KEYGEN_SEED_X_ERROR;
    ccn_zero(n, X);
    cc_unit tmp[n];
    ccn_zero(n,X);

    for (size_t i=0;i<SEED_X_MAX_RETRIES;i++) {
        // Generate a random number X
        cc_require(ccn_random_bits(pbits, X, rng)==0,cleanup);

#if CC_DEBUG_RSAKEYGEN_FIPS186
        ccn_lprint(n, "tmp X  = 0x", X);
#endif

        // Set most significant bit
        ccn_set_bit(X,(pbits-1), 1); // must be pbits long

        // Compare to an approximation of sqrt2:
        // copy X to tmp, bit-shift tmp to compare against sqrt_2
        ccn_shift_right_multi(n, tmp, X, pbits-ccn_bitsof_n(sqrt_2_n));
        if ((ccn_cmp(sqrt_2_n,tmp,sqrt_2)!=-1))
        {
            status=0;
            break;
        }
    }

#if CC_DEBUG_RSAKEYGEN_FIPS186
    ccn_lprint(n, "X  = 0x", X);
#endif

cleanup:
    ccn_clear(n,tmp);
    return status;
}

// Generate the two auxiliary primes r1 and r2 from rng provided specified sizes.
static int
cczp_generate_auxiliary_primes(cc_size r1bits, cczp_t r1, cc_size r2bits, cczp_t r2, struct ccrng_state *rng) {
    int status=CCRSA_KEYGEN_SEED_r_ERROR;
    // Take seeds for r1 and r2
    ccn_zero(CCZP_N(r1),CCZP_PRIME(r1));
    ccn_zero(CCZP_N(r2),CCZP_PRIME(r2));
    if(ccn_random_bits(r1bits, CCZP_PRIME(r1), rng)) goto errOut;
    if(ccn_random_bits(r2bits, CCZP_PRIME(r2), rng)) goto errOut;

    // Set MSbit to guarantee bitsize
    ccn_set_bit(CCZP_PRIME(r1), r1bits-1, 1); // must be rxbits long
    ccn_set_bit(CCZP_PRIME(r2), r2bits-1, 1); // must be rxbits long

    // Transform seed into primes
#if CC_DEBUG_RSAKEYGEN_FIPS186
    ccn_lprint(CCZP_N(r1), "Xr1  = 0x", cczp_prime(r1));
#endif

    status=cczp_find_next_prime(r1);
    if (status!=0) goto errOut;

#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("r1 actual bit length %lu\n",cczp_bitlen(r1));
    ccn_lprint(CCZP_N(r1), "r1  = 0x", cczp_prime(r1));
    ccn_lprint(CCZP_N(r2), "Xr2  = 0x", cczp_prime(r2));
#endif

    status=cczp_find_next_prime(r2);
    if (status!=0) goto errOut;

#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("r2 actual bit length %lu\n",cczp_bitlen(r2));
    ccn_lprint(CCZP_N(r2), "r2  = 0x", cczp_prime(r2));
#endif
errOut:
    return status;
}

// R = ((r2^–1 mod 2r1) * r2) – (((2r1)^–1 mod r2) * 2r1).
// Output is {R, r1r2x2}
static void
cczp_compute_R(cc_size n, cc_unit *R, cczp_t r1r2x2, const cczp_t  r1,  const cczp_t  r2) {
    // Per spec, defined as the CRT so that R=1 (mod 2*r1) and R=-1 (mod r2)
    // This can be rewritten using Garner recombination (HAC p613)
    // R = 1 + 2*r1[r2 - ((r1)^-1 mod r2)]

    cc_size r1_bitsize=cczp_bitlen(r1);
    cc_size r2_bitsize=cczp_bitlen(r2);
    cc_size r_n = ((1+r1_bitsize) > r2_bitsize) ? ccn_nof((1+r1_bitsize)):ccn_nof(r2_bitsize);
    cc_assert(2*r_n<=n);

    // All intermediary variables normalized to fit on r_n cc_units
    cc_unit tmp1[r_n];
    cc_unit tmp2[r_n];

    // Calculate tmp1 = (r1^{-1} mod r2)
    cczp_modn(r2,tmp1,cczp_n(r1), cczp_prime(r1));
    cczp_mod_inv(r2, tmp1, tmp1);
    ccn_cleartop(r_n, tmp1, cczp_n(r2));

    // Go on with Garner's recombination
    ccn_setn(r_n, R, ccn_nof(r2_bitsize), cczp_prime(r2));  // normalize r2 (R as temp)
    ccn_sub(r_n,tmp1,R,tmp1);                               // r2 - ((r1)^-1 mod r2)
    ccn_setn(r_n, tmp2, ccn_nof(r1_bitsize), cczp_prime(r1)); // normalize r1
    ccn_add(r_n, tmp2, tmp2, tmp2);                         // 2*r1

    // r1*r2*2
    ccn_mul(r_n, CCZP_PRIME(r1r2x2), tmp2, R);
    ccn_cleartop(n, CCZP_PRIME(r1r2x2), 2*r_n);
    cczp_init(r1r2x2);

    // R = 1 + 2*r1*(r2 - ((r1)^-1 mod r2))
    ccn_mul(r_n, R, tmp2, tmp1);
    ccn_add1(2*r_n,R, R,1);       // can't overflow since ((r1)^-1 mod r2) > 0)
    ccn_cleartop(n, R, 2*r_n);

    // Clear temporary buffers
    cc_clear(sizeof(tmp1),tmp1);
    cc_clear(sizeof(tmp2),tmp2);
}

// Generate {p, X} from primes r1 and r2.
// Follows FIPS186-4, B.3.6
// "n" of both p and X must have been set
static int
ccrsa_generate_probable_prime_from_auxilary_primes(cc_size pbits, cczp_t p, cc_unit *X,
                                                   const cczp_t  r1,  const cczp_t  r2,
                                                   const cc_size e_n, const cc_unit *e,
                                                   struct ccrng_state *rng)
{
    cc_size i;

    int prime_status=CCRSA_KEYGEN_PRIME_NEED_NEW_SEED;
    cc_size n = cczp_n(p);
    cc_size MR_iterations=ccrsa_fips186_MR_only_iteration_number(pbits);
    cc_size e_bitsize=ccn_bitlen(e_n,e);
    cc_size r1r2x2max_bitsize;

    // Temp variable for the main loop
    cc_unit gcd[n];
    cc_unit R[n];
    cczp_decl_n(n, r1r2x2);
    CCZP_N(r1r2x2) = n;

    // Pre-requisite: Check log2(r1.r2) <= pbits - log2(pbits) - 6
    // Equivalent to Check log2(2.r1.r2) <= pbits - log2(pbits) - 5
    R[0]=pbits;
    r1r2x2max_bitsize=pbits-ccn_bitlen(1,R)-5;

    // This constraint met by ccrsa_fips186_auxiliary_prime_length
    // Therefore no need to check here.

    // 1) Check GCD(2r1,r2)!=1
    // r1 and r2 are prime and >2 so this check is not needed.

    // 2) R = ((r2^–1 mod 2r1) * r2) – (((2r1)^–1 mod r2) * 2r1).
    // and compute 2.r1.r2
    cczp_compute_R(n, R, r1r2x2, r1, r2);

    if (cczp_bitlen(r1r2x2)>r1r2x2max_bitsize) {
        prime_status=CCRSA_KEYGEN_R1R2_SIZE_ERROR;
    }

    // Outter loop for reseeding (rare case)
    for (size_t ctr=0; (ctr<RESEED_MAX_RETRIES) && (prime_status==CCRSA_KEYGEN_PRIME_NEED_NEW_SEED);ctr++)
    {
        cc_unit c; // carry

        // 3) Generate random X
        if (ccn_seed_X(n,X,pbits,rng)!=0) {
            prime_status=CCRSA_KEYGEN_PRIME_SEED_GENERATION_ERROR;
            break;
        }

        // 4) Y = X+((R–X) mod 2r1r2)
        {
            cc_unit tmp[n];
            cczp_modn(r1r2x2,tmp,n,X); // X mod 2r1r2
            cczp_sub(r1r2x2,CCZP_PRIME(p),R,tmp);  // (R-X) mod 2r1r2
            c=ccn_add(n,CCZP_PRIME(p),X,CCZP_PRIME(p));
            cc_clear(sizeof(tmp),tmp);
            // c is used for 1st iteration of for loop
        }

        // Inner loop for incremental search.
        // Candidate is now in p.
        // 5,8,9) Increment p until a good candidate is found
        // Iterate a maximum of 5*pbits
        prime_status=CCRSA_KEYGEN_PRIME_TOO_MANY_ITERATIONS;
        for (i=0;i<5*pbits;i++)
        {
            // 6) Check p >= 2^pbits
            if ((c>0) || (pbits<cczp_bitlen(p))) {
                // Candidate is too large, needs new seed
                prime_status=CCRSA_KEYGEN_PRIME_NEED_NEW_SEED;
                break;
            }

            // 7) Check if e is inversible by p-1, primality of candidate
            // Performance: depending on the size, change the order of the operation
            // to have the most likely to fail first.
            if (e_bitsize>16) {
                if (cczp_rabin_miller(p, MR_iterations)==1) {
                    CCZP_PRIME(p)[0] &= ~(cc_unit)1; // X - 1
                    ccn_gcdn(n, gcd, n, cczp_prime(p), e_n, e);
                    CCZP_PRIME(p)[0] |= (cc_unit)1; // restore X
                    if (ccn_is_one(n, gcd)) {
                        prime_status=0; // Prime found
                        break;
                    }
                }
            }
            else {
                CCZP_PRIME(p)[0] &= ~(cc_unit)1; // X - 1
                ccn_gcdn(n, gcd, n, cczp_prime(p), e_n, e);
                CCZP_PRIME(p)[0] |= (cc_unit)1; // restore X
                if (ccn_is_one(n, gcd)) {
                    if(cczp_rabin_miller(p, MR_iterations)==1) {
                        prime_status=0; // Prime found
                        break;
                    }
                }
            }

            // 10) p=p+2.r1.r2
            c=ccn_add(n,CCZP_PRIME(p),CCZP_PRIME(p),CCZP_PRIME(r1r2x2));
        }
    }

    // Prepare exit
    if (prime_status!=0) {
        ccn_clear(n,CCZP_PRIME(p));
        ccn_clear(n,X);
    }
    else {
        CCZP_N(p) = ccn_nof(cczp_bitlen(p));
        cczp_init(p);
    }
    // Clean working memory
    cc_clear(sizeof(gcd),gcd);
    cc_clear(sizeof(R),R);
    cc_clear(sizeof(r1r2x2),r1r2x2);
    return prime_status;
}

// Generate {p, X} from rng and the size of the arbitrary primes to use
static int
ccrsa_generate_probable_prime(cc_size pbits, cczp_t p, cc_unit *X,
                            cc_size r1_bitsize, cc_size r2_bitsize,
                            const cc_size e_n, const cc_unit *e,
                            struct ccrng_state *rng)
{
    int ret;
    cc_size n_alpha=ccn_nof(CC_MAX(r1_bitsize,r2_bitsize));
    cczp_decl_n(n_alpha, r1);
    cczp_decl_n(n_alpha, r2);
    CCZP_N(r1) = n_alpha;
    CCZP_N(r2) = n_alpha;
    cc_require((ret=cczp_generate_auxiliary_primes(r1_bitsize,r1,r2_bitsize,r2,rng))==0,cleanup);
    cc_require((ret=ccrsa_generate_probable_prime_from_auxilary_primes(pbits, p, X,
                                                                       r1, r2, e_n, e, rng))==0,cleanup);
cleanup:
    cc_clear(sizeof(r1),r1);
    cc_clear(sizeof(r2),r2);
    return ret;
}


// Fill out a ccrsa context given e, p, and q.  The "n" of the key context is expected
// to be set prior to this call.  p and q are cczps with no assumption as to their
// relative values.
// D is calculated per ANS 9.31 / FIPS 186 rules.
// TODO: share the calculation for dp, dq, and qinv with the stock keygen.
static int
ccrsa_crt_make_fips186_key(unsigned long nbits,
                                ccrsa_full_ctx_t fk,
                                const cc_size e_n, const cc_unit *e,
                                cczp_t p, cczp_t q)
{
    cc_size n = ccrsa_ctx_n(fk);
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(fk);

    if((cczp_bitlen(p)+cczp_bitlen(q)) > ccn_bitsof_n(n)) return CCRSA_INVALID_INPUT;

    //-------------------------------------------------------------------------------------
    // Setup e
    ccn_setn(n, ccrsa_ctx_e(pubk), e_n, e);

    //-------------------------------------------------------------------------------------
    // Setup p and q
    if(ccn_cmpn(ccn_n(cczp_n(p), cczp_prime(p)), cczp_prime(p),
                ccn_n(cczp_n(q), cczp_prime(q)), cczp_prime(q)) < 0) {
        CC_SWAP(p, q);
    }
    // With this structure you MUST initialize the n of p in the context before attempting
    // to access anything else within the private key.
    CCZP_N(ccrsa_ctx_private_zp(privk)) = cczp_n(p);
    CCZP_N(ccrsa_ctx_private_zq(privk)) = ccn_nof(cczp_bitlen(q));
    ccn_set(cczp_n(p), CCZP_PRIME(ccrsa_ctx_private_zp(privk)), cczp_prime(p));
    ccn_set(cczp_n(q), CCZP_PRIME(ccrsa_ctx_private_zq(privk)), cczp_prime(q));
    cczp_t zp = ccrsa_ctx_private_zp(privk);
    cczp_t zq = ccrsa_ctx_private_zq(privk);

    //-------------------------------------------------------------------------------------
    // Compute m
    cczp_t zm = ccrsa_ctx_zm(pubk);
    /* Since a straight up ccn_mul requires m to be up to 2 cc_units bigger
     than the keysize normally dictates, we use cczp_n(zq) for mul length,
     and separately check if cczp_n(zp) > cczp_n(zq)
     and zp->p[cczp_n(zp) -1] == 1 (it's only one bit at most), if so then
     we add zq to the msb of zp. */
    ccn_mul(cczp_n(zq), CCZP_PRIME(zm), cczp_prime(zp), cczp_prime(zq));
    if (cczp_n(zp) > cczp_n(zq)) {
        cc_unit carry = 0;
        // TODO: This bit is always set, so if above can be removed.
        if (cczp_prime(zp)[cczp_n(zp) - 1]) {
            cc_unit times = cczp_prime(zp)[cczp_n(zp) - 1];
            // Deal with m.s. bit of p
            while (times--) {
                carry += ccn_add(cczp_n(zq), CCZP_PRIME(zm) + cczp_n(zq), CCZP_PRIME(zm) + cczp_n(zq), cczp_prime(zq));
            }
        } else {
            carry = 0;
        }
        CCZP_PRIME(zm)[2 * cczp_n(zq)] = carry;
    }
    cczp_init(zm);

    if((cczp_bitlen(zm) +1 ) < nbits) return CCRSA_INVALID_INPUT;

    //-------------------------------------------------------------------------------------
    // Compute d = e^(-1) mod lcm((p-1),(q-1)) (9.31's "lambda function")
    // Since p and q are both odd we just clear bit 0 to subtract 1. Making
    // cczp_prime(zp) p - 1 and cczp_prime(zq) q - 1 and setting up the
    // reciprocals properly.
    CCZP_PRIME(zp)[0] &= ~1;
    CCZP_PRIME(zq)[0] &= ~1;
    if (cczp_n(zp) > cczp_n(zq)) CCZP_PRIME(zq)[cczp_n(zp) - 1] = 0;

    // lambda = lcm((p-1),(q-1))
    cczp_decl_n(n+2, zlambda); // zlambda needs to be n+2 for the lcm op.
    ccn_zero(n+2, CCZP_PRIME(zlambda));
    ccn_lcm(cczp_n(zp), CCZP_PRIME(zlambda), cczp_prime(zp), cczp_prime(zq));

    // Compute the modular inverse of e over zlambda. */
    CCZP_N(zlambda) = n;
    cczp_init(zlambda);
    cczp_mod_inv_slow(zlambda, ccrsa_ctx_d(fk), ccrsa_ctx_e(pubk));

    //-------------------------------------------------------------------------------------
    // Remainder of key is just like "stock" RSA keygen.  This should be consolidated with
    // that routine.
    //
    // Since cczp_n(zp) can be n / 2 + 1, and cczp_mod() reads 2 * cczp_n(zp)
    // units from it's third argument, this implies dtmp must be at least
    // n + 2 units long for reading.
    // TODO: Eliminate excess copies here, since cczp_mod makes yet another copy.
    // Doing so would require ccn_muln and cczp_modn routines.
    cc_unit dtmp[n + 2];
    ccn_set(n, dtmp, ccrsa_ctx_d(fk));
    ccn_zero(2, dtmp + n);

    //-------------------------------------------------------------------------------------
    // dp = d mod (p-1) */
    cczp_init(zp);
    cczp_modn(zp, ccrsa_ctx_private_dp(privk), n, dtmp);
    //-------------------------------------------------------------------------------------
    // dq = d mod (q-1) */
    cczp_init(zq);
    cczp_modn(zq, ccrsa_ctx_private_dq(privk), n, dtmp);

    //-------------------------------------------------------------------------------------
    //  Set zp and zq back to p and q respectively by setting bit 0 back to
    //  one and recomputing the reciprocals.
    CCZP_PRIME(zp)[0] |= 1;
    CCZP_PRIME(zq)[0] |= 1;
    if (cczp_n(zp) > cczp_n(zq)) CCZP_PRIME(zq)[cczp_n(zp) - 1] = 0;
    cczp_init(zp);

    //-------------------------------------------------------------------------------------
    // qInv = q^(-1) mod p. This requires q to be at least as long as p with
    // proper zero padding. Our caller takes care if this. Obviously qInv can
    // be as big as p too.
    if (cczp_mod_inv(zp, ccrsa_ctx_private_qinv(privk), cczp_prime(zq))) return CCRSA_KEYGEN_MODULUS_CRT_INV_ERROR;

    //-------------------------------------------------------------------------------------
    // Initialize zq after the cczp_mod_inv above, since the recipricol of
    // zq starts right after q in memory and we want the extra zero during
    // the computation of qinv.
    cczp_init(zq);

    return 0;
}

// This is pretty much the same interface as the "stock" RSA keygen except that
// two rng descriptors need to be provided.  You *can* call it with the same
// descriptor if you really want to.
// rng1 and rng2 are use for respective prime factors
// Note that "e" is expressed in pointer and length of bytes, not cc_units.

static int
ccrsa_generate_fips186_prime_factors(unsigned long nbits,
                               cczp_t p, cczp_t q,
                               cc_size e_n, const cc_unit *e,
                               struct ccrng_state *rng1, struct ccrng_state *rng2)
{
    if ((nbits < 1024)) return CCRSA_KEY_ERROR;
    int ret;
    cc_size pbits = (nbits+1) >> 1, qbits = nbits - pbits;
    cc_size alpha=ccrsa_fips186_auxiliary_prime_length(pbits);

    // Space to generate P and Q
    cc_size n_pq = ccn_nof(pbits);
    CCZP_N(p) = CCZP_N(q) = n_pq;

    // Auxiliary-Primes space to generate P & Q
    cc_unit xp[n_pq];
    cc_unit xq[n_pq];

    // e must be odd && e must be >1
    cc_require_action( ((e[0] & 1)==1) \
                      && (ccn_bitlen(e_n,e)>1),
                      cleanup,ret=CCRSA_KEY_ERROR);

    // Generate P
#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("Generating p of length %lu\n",pbits);
#endif
    cc_require((ret=ccrsa_generate_probable_prime(pbits, p, xp,
                                        alpha,  alpha, e_n, e, rng1))==0,cleanup);

#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("p actual bit length %lu\n",cczp_bitlen(p));
    ccn_lprint(CCZP_N(p),"p = 0x",CCZP_PRIME(p));
#endif

    // Now, do the same for q. But repeat until q,p and Xp, Xq are
    // sufficiently far apart, and d is sufficiently large
    ret=CCRSA_KEYGEN_PQ_DELTA_ERROR;
    for (size_t i=0;i<GENERATE_Q_MAX_RETRIES;i++) {
        // Generate Q - we're going to check for a large enough delta in various steps of this.
#if CC_DEBUG_RSAKEYGEN_FIPS186
        cc_printf("Generating q of length %lu\n",pbits);
#endif
        cc_require((ret=ccrsa_generate_probable_prime(qbits, q, xq,
                                        alpha,  alpha, e_n, e, rng2))==0,cleanup);
#if CC_DEBUG_RSAKEYGEN_FIPS186
        cc_printf("q actual bit length %lu\n",cczp_bitlen(q));
        ccn_lprint(CCZP_N(p),"q = 0x",CCZP_PRIME(q));
#endif

        // If (|p-q|<= 2^(plen-100)) or If (|Xp-Xq|<= 2^(plen-100)) retry
        // (Make sure the seed P and Q were far enough apart)
        if (cczp_check_delta_100bits(n_pq,cczp_prime(p),cczp_prime(q),xp,xq))
        {
            break; // We're done!
        }
        ret=CCRSA_KEYGEN_PQ_DELTA_ERROR; // Need to reseed.
    }

cleanup:
    // Clear stack stuff
    ccn_clear(n_pq, xp);
    ccn_clear(n_pq, xq);
    return ret;
}


//==============================================================================
//                              External functions
//==============================================================================

int
ccrsa_generate_fips186_key(unsigned long nbits, ccrsa_full_ctx_t fk,
                           size_t e_size, const void *eBytes,
                           struct ccrng_state *rng1, struct ccrng_state *rng2)
{
    int ret;
    cc_size pqbits = (nbits >> 1);
    cc_size n = ccn_nof(nbits);
    cc_size n_pq = ccn_nof(pqbits)+1;
    ccrsa_ctx_n(fk) = n;
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    cczp_decl_n(n_pq, p);
    cczp_decl_n(n_pq, q);

    // Use the RSA key area to hold e as a ccn_unit.  Get e_n so we don't
    // need to roll on the full ccn_unit if we don't have to.
    if (ccn_read_uint(n, ccrsa_ctx_e(pubk), e_size, eBytes)) return CCRSA_KEY_ERROR;
    cc_size e_n = ccn_nof_size(e_size);
    cc_unit *e = ccrsa_ctx_e(pubk);
    ccrsa_ctx_n(fk) = n;
    CCZP_N(p) = CCZP_N(q) = n_pq;

    // Prime factors
    cc_require((ret=ccrsa_generate_fips186_prime_factors(
                                         nbits,
                                         p, q,
                                         e_n, e,
                                         rng1,
                                         rng2))==0,cleanup);

    // Generate the key
    cc_require((ret=ccrsa_crt_make_fips186_key(nbits, fk, e_n, e, p, q))==0,cleanup);

    // Check that the key works
    ret=(ccrsa_pairwise_consistency_check(fk) ? 0 : CCRSA_KEYGEN_KEYGEN_CONSISTENCY_FAIL);

#if CC_DEBUG_RSAKEYGEN_FIPS186
    ccrsa_dump_key(fk);
#endif
cleanup:
    cc_clear(sizeof(p),p);
    cc_clear(sizeof(q),q);
    return ret;
}

// This interface is primarily here for FIPS testing.  It creates an RSA key with the various "random" numbers
// supplied as parameters; calling the same routines as the "generate" function in the same sequence.  It
// doesn't test the suitability of the numbers since the FIPS KAT tests provide "good" numbers from which to
// make keys.
//
// In addition this routine passes back the RSA key components if space is passed in to return the values.
// This is done here so that the proper "P" and "Q" values are returned, since we'll always put the largest
// value into P to work with the remainder of the math in this package.  Keys constructed in this manner
// behave the same as keys with no ordering for P & Q.
//

int
ccrsa_make_fips186_key(unsigned long nbits,
                   const cc_size e_n, const cc_unit *e,
                   const cc_size xp1Len, const cc_unit *xp1, const cc_size xp2Len, const cc_unit *xp2,
                   const cc_size xpLen, const cc_unit *xp,
                   const cc_size xq1Len, const cc_unit *xq1, const cc_size xq2Len, const cc_unit *xq2,
                   const cc_size xqLen, const cc_unit *xq,
                   ccrsa_full_ctx_t fk,
                   cc_size *np, cc_unit *r_p,
                   cc_size *nq, cc_unit *r_q,
                   cc_size *nm, cc_unit *r_m,
                   cc_size *nd, cc_unit *r_d)
{
    cc_size pqbits = (nbits >> 1);
    cc_size n = ccn_nof(nbits);
    cc_size n_pq = ccn_nof(pqbits)+1;
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    cczp_decl_n(n_pq, p);
    cczp_decl_n(n_pq, q);

    struct ccrng_rsafips_test_state rng;
    cc_size x1_bitsize;
    cc_size x2_bitsize;
    int ret;

    ccrsa_ctx_n(fk) = n;
    CCZP_N(p) = CCZP_N(q) = n_pq;
    cc_unit xpp[n_pq];
    cc_unit xqq[n_pq];

    // Auxiliary-Prime space to generate P & Q
    cc_require_action((e[0] & 1)==1,cleanup,ret=CCRSA_KEY_ERROR); // e must be odd.

    // Generate P
    ccrng_rsafips_test_init(&rng,xp1Len,xp1,xp2Len,xp2,xpLen,xp);
#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("Generating p\n");
#endif
    x1_bitsize = ccn_bitlen(xp1Len, xp1);
    x2_bitsize = ccn_bitlen(xp2Len, xp2);
    cc_require((ret=ccrsa_generate_probable_prime(pqbits, p, xpp,
                            x1_bitsize,  x2_bitsize, e_n, e, (struct ccrng_state *)&rng))==0,cleanup);


    // Generate Q
    ccrng_rsafips_test_init(&rng,xq1Len,xq1,xq2Len,xq2,xqLen,xq);
#if CC_DEBUG_RSAKEYGEN_FIPS186
    cc_printf("Generating q\n");
#endif
    x1_bitsize = ccn_bitlen(xq1Len, xq1);
    x2_bitsize = ccn_bitlen(xq2Len, xq2);
    cc_require((ret=ccrsa_generate_probable_prime(pqbits, q, xqq,
                            x1_bitsize,  x2_bitsize, e_n, e, (struct ccrng_state *)&rng))==0,cleanup);

    // Check delta between P and Q, XP, XQ
    ret=CCRSA_KEYGEN_PQ_DELTA_ERROR;
    cc_require(cczp_check_delta_100bits(n_pq,cczp_prime(p),cczp_prime(q),xpp,xqq),cleanup);

    // Return P&Q if requested now since we might assigned them in reverse in the CRT routine.
    if(np && r_p) { *np = cczp_n(p); ccn_set(*np, r_p, cczp_prime(p)); }
    if(nq && r_q) { *nq = cczp_n(q); ccn_set(*nq, r_q, cczp_prime(q)); }

    // Construct the key from p and q
    cc_require((ret=ccrsa_crt_make_fips186_key(nbits, fk, e_n, e, p, q))==0,cleanup);

    // Return m and d if requested.
    if(nm && r_m) { *nm = cczp_n(ccrsa_ctx_zm(pubk)); ccn_set(cczp_n(ccrsa_ctx_zm(pubk)), r_m, cczp_prime(ccrsa_ctx_zm(pubk))); }
    if(nd && r_d) { *nd = n; ccn_set(n, r_d, ccrsa_ctx_d(fk)); }

#if CC_DEBUG_RSAKEYGEN_FIPS186
    ccrsa_dump_key(fk);
#endif

cleanup:
    cc_clear(sizeof(p),p);
    cc_clear(sizeof(q),q);
    return ret;
}


//================================ EOF =======================================//
