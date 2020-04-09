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


#ifndef _CORECRYPTO_CCZP_H_
#define _CORECRYPTO_CCZP_H_

#include <corecrypto/ccn.h>
#include <corecrypto/ccrng.h>

/* Don't use this struct directly, except in static tables such as
   eliptic curve parameter definitions.  Instead declare objects as cc_unit
   arrays of the length returned by either cczp_nof_n() or cczp_short_nof_n().
 */
struct cczp {
    cc_size n;
    cc_unit options;        // ex: Montgomery arithmetic, bit field
    void(*mod_prime)(const struct cczp *zp, cc_unit *r, const cc_unit *s, cc_ws_t ws);
} __attribute__((aligned(CCN_UNIT_SIZE)));

struct cczp_short {
    cc_size n;
    cc_unit options;        // ex: Montgomery arithmetic, bit field
    void(*mod_prime)(const struct cczp *zp, cc_unit *r, const cc_unit *s, cc_ws_t ws);
} __attribute__((aligned(CCN_UNIT_SIZE)));

struct cczp_prime {
    struct cczp hdr;
    cc_unit ccn[];
};

typedef union {
    cc_unit *u;
    struct cczp *zp;
    struct cczp_prime *prime;
} cczp_t __attribute__((transparent_union));

typedef union {
    cc_unit *u;
    struct cczp *zp;
    struct cczp_prime *prime;
    struct cczp_short *szp;
    cczp_t _lzp;
} cczp_short_t __attribute__((transparent_union));

typedef union {
    const cc_unit *u;
    const struct cczp *zp;
    const struct cczp_prime *prime;
    cczp_t _nczp;
} cczp_const_t __attribute__((transparent_union));

typedef union {
    const cc_unit *u;
    const struct cczp *zp;
    struct cczp_prime *prime;
    const struct cczp_short *szp;
    cczp_const_t _clzp;
    cczp_short_t _szp;
    cczp_t _lzp;
} cczp_const_short_t __attribute__((transparent_union));

typedef void(ccmod_prime_f)(cczp_const_t, cc_unit *, const cc_unit *, cc_ws_t);

/* Return the size of an cczp where each ccn is _size_ bytes. */
#define cczp_size(_size_) (sizeof(struct cczp) + ccn_sizeof_n(1) + 2 * (_size_))

/* Return the size of an cczp where each ccn is _size_ bytes. */
#define cczp_short_size(_size_) (sizeof(struct cczp) + (_size_))

/* Return number of units that a struct cczp needs to be in units for a prime
   size of N units.  This is large enough for all operations.  */
#define cczp_nof_n(_n_) (ccn_nof_size(sizeof(struct cczp)) + 1 + 2 * (_n_))

/* Return number of units that a struct cczp needs to be in units for a prime
   size of _n_ units.  The _short variant does not have room for CCZP_RECIP,
   so it can not be used with cczp_mod, cczp_mul, cczp_sqr. It can be used
   with cczp_add, cczp_sub, cczp_div2, cczp_mod_inv. */
#define cczp_short_nof_n(_n_) (ccn_nof_size(sizeof(struct cczp)) + (_n_))

#define cczp_decl_n(_n_, _name_)  cc_ctx_decl(struct cczp, ccn_sizeof_n(cczp_nof_n(_n_)), _name_)
#define cczp_short_decl_n(_n_, _name_) cc_ctx_decl(struct cczp_short, ccn_sizeof_n(cczp_short_nof_n(_n_)), _name_)

#define CCZP_N(ZP) (((cczp_short_t)(ZP)).zp->n)
#define CCZP_MOD_PRIME(ZP) (((cczp_short_t)(ZP)).zp->mod_prime)
#define CCZP_PRIME(ZP) (((cczp_short_t)(ZP)).prime->ccn)
#define CCZP_RECIP(ZP) (((cczp_t)(ZP)).prime->ccn + cczp_n(ZP))

CC_CONST CC_NONNULL_TU((1))
CC_INLINE cc_size cczp_n(cczp_const_short_t zp) {
    return zp.zp->n;
}

CC_CONST CC_NONNULL_TU((1))
CC_INLINE cc_unit cczp_options(cczp_const_short_t zp) {
    return zp.zp->options;
}

CC_CONST CC_NONNULL_TU((1))
CC_INLINE ccmod_prime_f *cczp_mod_prime(cczp_const_short_t zp) {
    return zp.zp->mod_prime;
}

CC_CONST CC_NONNULL_TU((1))
CC_INLINE const cc_unit *cczp_prime(cczp_const_short_t zp) {
    return zp.u + ccn_nof_size(sizeof(struct cczp));
}

/* Return a pointer to the Reciprocal or Montgomery constant of zp, which is
   allocated cczp_n(zp) + 1 units long. */
CC_CONST CC_NONNULL_TU((1))
CC_INLINE const cc_unit *cczp_recip(cczp_const_t zp) {
    return zp.u + cczp_n(zp) + ccn_nof_size(sizeof(struct cczp));
}

/* Ensure both cczp_mod_prime(zp) and cczp_recip(zp) are valid. cczp_n and
   cczp_prime must have been previously initialized. */
CC_NONNULL_TU((1))
void cczp_init(cczp_t zp);

/* Compute r = s2n mod cczp_prime(zp). Will write cczp_n(zp)
 units to r and reads 2 * cczp_n(zp) units units from s2n. If r and s2n are not
 identical they must not overlap.  Before calling this function either
 cczp_init(zp) must have been called or both CCZP_MOD_PRIME((cc_unit *)zp)
 and CCZP_RECIP((cc_unit *)zp) must be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
void cczp_mod(cczp_const_t zp, cc_unit *r, const cc_unit *s2n, cc_ws_t ws);

/* Compute r = sn mod cczp_prime(zp), Will write cczp_n(zp)
 units to r and reads sn units units from s. If r and s are not
 identical they must not overlap.  Before calling this function either
 cczp_init(zp) must have been called or both CCZP_MOD_PRIME((cc_unit *)zp)
 and CCZP_RECIP((cc_unit *)zp) must be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 4))
void cczp_modn(cczp_const_t zp, cc_unit *r, cc_size ns, const cc_unit *s);

/* Compute r = x * y mod cczp_prime(zp). Will write cczp_n(zp) units to r
   and reads cczp_n(zp) units units from both x and y. If r and x are not
   identical they must not overlap, The same holds for r and y.  Before
   calling this function either cczp_init(zp) must have been called or both
   CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must be
   initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_mul(cczp_const_t zp, cc_unit *t, const cc_unit *x, const cc_unit *y);

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 5))
void cczp_mul_ws(cczp_const_t zp, cc_unit *t, const cc_unit *x, const cc_unit *y, cc_ws_t ws);

/* Compute r = x * x mod cczp_prime(zp). Will write cczp_n(zp) units to r
   and reads cczp_n(zp) units from x. If r and x are not identical they must
   not overlap. Before calling this function either cczp_init(zp) must have
   been called or both CCZP_MOD_PRIME((cc_unit *)zp) and
   CCZP_RECIP((cc_unit *)zp) must be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
void cczp_sqr(cczp_const_t zp, cc_unit *r, const cc_unit *x);

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_sqr_ws(cczp_const_t zp, cc_unit *r, const cc_unit *x, cc_ws_t ws);

/* Compute r = x^(1/2) mod cczp_prime(zp). Will write cczp_n(zp) units to r
 and reads cczp_n(zp) units from x. If r and x are not identical they must
 not overlap. Before calling this function either cczp_init(zp) must have
 been called or both CCZP_MOD_PRIME((cc_unit *)zp) and
 CCZP_RECIP((cc_unit *)zp) must be initialized some other way. 
 Only support prime = 3 mod 4 */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
int cczp_sqrt(cczp_const_t zp, cc_unit *r, const cc_unit *x);

/* Compute r = m ^ e mod cczp_prime(zp). Will write cczp_n(zp) units to r and
   reads cczp_n(zp) units units from m and e. If r and m are not identical
   they must not overlap. r and e must not overlap nor be identical.
   Before calling this function either cczp_init(zp) must have been called
   or both CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must
   be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_power(cczp_const_t zp, cc_unit *r, const cc_unit *m,
                const cc_unit *e);

/* Compute r = m ^ e mod cczp_prime(zp). Will write cczp_n(zp) units to r and
 reads cczp_n(zp) units units from m.  Reads ebitlen bits from e.
 m must be <= to cczp_prime(zp).  If r and m are not identical they must not
 overlap. r and e must not overlap nor be identical.
 Before calling this function either cczp_init(zp) must have been called
 or both CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must
 be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 5))
void cczp_powern(cczp_const_t zp, cc_unit *r, const cc_unit *s,
                 size_t ebitlen, const cc_unit *e);

/* Compute r = x + y mod cczp_prime(zp). Will write cczp_n(zp) units to r and
   reads cczp_n(zp) units units from x and y. If r and x are not identical
   they must not overlap. Only cczp_n(zp) and cczp_prime(zp) need to be valid.
   Can be used with cczp_short_nof_n sized cc_unit array zp. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_add(cczp_const_short_t zp, cc_unit *r, const cc_unit *x,
              const cc_unit *y);

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 5))
void cczp_add_ws(cczp_const_short_t zp, cc_unit *r, const cc_unit *x,
                 const cc_unit *y, cc_ws_t ws);

/* Compute r = x - y mod cczp_prime(zp). Will write cczp_n(zp) units to r and
   reads cczp_n(zp) units units from x and y. If r and x are not identical
   they must not overlap. Only cczp_n(zp) and cczp_prime(zp) need to be valid.
   Can be used with cczp_short_nof_n sized cc_unit array zp. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_sub(cczp_const_short_t zp, cc_unit *r, const cc_unit *x, const cc_unit *y);

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 5))
void cczp_sub_ws(cczp_const_short_t zp, cc_unit *r, const cc_unit *x,
                 const cc_unit *y, cc_ws_t ws);

/* Compute r = x / 2 mod cczp_prime(zp). Will write cczp_n(zp) units to r and
   reads cczp_n(zp) units units from x. If r and x are not identical
   they must not overlap. Only cczp_n(zp) and cczp_prime(zp) need to be valid.
   Can be used with cczp_short_nof_n sized cc_unit array zp. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
void cczp_div2(cczp_const_short_t zp, cc_unit *r, const cc_unit *x);

/* Compute q = a_2n / cczp_prime(zd) (mod cczp_prime(zd)) . Will write cczp_n(zd)
   units to q and r. Will read 2 * cczp_n(zd) units units from a. If r and a
   are not identical they must not overlap. Before calling this function
   either cczp_init(zp) must have been called or both
   CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must be
   initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4))
void cczp_div(cczp_const_t zd, cc_unit *q, cc_unit *r, const cc_unit *a_2n);

/* Solve the equation r * x mod cczp_prime(zp) = 1 for r. Will write
   cczp_n(zp) units to r and reads cczp_n(zp) units units from x.
   Only cczp_n(zp) and cczp_prime(zp) need to be valid. Can be used with
   cczp_short_nof_n sized cc_unit array zp. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
int cczp_mod_inv(cczp_const_short_t zp, cc_unit *r, const cc_unit *x);

/* Solve the equation r * x mod cczp_prime(zp) = 1 for r. Will write
 cczp_n(zp) units to r and reads cczp_n(zp) units units from x.
 Compute inverse with an exponentiation r=a^(p-1) mod p 
 Support Montgomery and non-Montgomery form.
 r and s must not overlap.
 It leaks the value of the prime. To be used for public (not secret) primes only
 (ex. Elliptic Curves)
 */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
int cczp_mod_inv_field(cczp_const_t zp, cc_unit *r, const cc_unit *x);

/* Solve the equation r * x mod cczp_prime(zp) = 1 for r. Will write
   cczp_n(zp) units to r and reads cczp_n(zp) units units from x.
   Before calling this function either cczp_init(zp) must have been called
   or both CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must
   be initialized some other way. */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3))
void cczp_mod_inv_slow(cczp_const_t zp, cc_unit *r, const cc_unit *x);

/* Solve the equation r * x mod cczp_prime(zp) = 1 for r. Will write
 cczp_n(zp) units to r and reads cczp_n(zp) units units from x.
 Before calling this function either cczp_init(zp) must have been called
 or both CCZP_MOD_PRIME((cc_unit *)zp) and CCZP_RECIP((cc_unit *)zp) must
 be initialized some other way. 
 nx must be <= 2*cczp_n(zp)
 */
CC_NONNULL_TU((1)) CC_NONNULL((2, 4))
void cczp_mod_inv_slown(cczp_const_t zp, cc_unit *r, const cc_size nx, const cc_unit *x);

/* Perform rabin miller primality testing on zp, return 1 if p is prime
   return 0 otherwise. */
CC_NONNULL_TU((1))
int cczp_rabin_miller(cczp_t zp, unsigned long mr_depth);

/* Generate a random nbits sized prime in zp.  zp will be guaranteed
   relatively prime to e. */
CC_NONNULL_TU((2)) CC_NONNULL((3, 4))
int cczp_random_prime(cc_size nbits, cczp_t zp, const cc_unit *e,
                       struct ccrng_state *rng);

#endif /* _CORECRYPTO_CCZP_H_ */
