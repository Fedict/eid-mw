/*
 * Copyright (c) 2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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

#define CCRSA_DUMP_KEY  0

#include <corecrypto/cc_debug.h>

void ccrsa_dump_key(ccrsa_full_ctx_t key) {
    cc_size nbits = ccn_bitlen(ccrsa_ctx_n(key), ccrsa_ctx_m(key));
    cc_printf("%lu bit rsa key\n", nbits);
    ccn_lprint(ccrsa_ctx_n(key),      "m  = 0x", ccrsa_ctx_m(key));
    ccn_lprint(ccrsa_ctx_n(key) + 1,  "rm = 0x", cczp_recip(ccrsa_ctx_zm(key)));
    ccn_lprint(ccrsa_ctx_n(key),      "e  = 0x", ccrsa_ctx_e(key));
    ccn_lprint(ccrsa_ctx_n(key),      "d  = 0x", ccrsa_ctx_d(key));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))),     "p  = 0x",
               cczp_prime(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))) + 1, "rp = 0x",
               cczp_recip(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zq(ccrsa_ctx_private(key))),     "q  = 0x",
               cczp_prime(ccrsa_ctx_private_zq(ccrsa_ctx_private(key))));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zq(ccrsa_ctx_private(key))) + 1, "rq = 0x",
               cczp_recip(ccrsa_ctx_private_zq(ccrsa_ctx_private(key))));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))),     "dp = 0x",
               ccrsa_ctx_private_dp(ccrsa_ctx_private(key)));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zq(ccrsa_ctx_private(key))),     "dq = 0x",
               ccrsa_ctx_private_dq(ccrsa_ctx_private(key)));
    ccn_lprint(cczp_n(ccrsa_ctx_private_zp(ccrsa_ctx_private(key))),     "qinv=0x",
               ccrsa_ctx_private_qinv(ccrsa_ctx_private(key)));
    cc_printf("\n");
    //cc_printf("(p*q)-m\n");
    //cc_printf("d.modulo(p-1)-dp\n");
    //cc_printf("d.modulo(q-1)-dq\n");
    //cc_printf("(q*qinv).modulo(p)\n");
    //cc_printf("(p-1)*(q-1)-phi\n");
    //cc_printf("(d*e).modulo((p-1)*(q-1))\n");
}



/* n e cczp_n(zp) and cczp_n(zq) zp->p and zq->p are given, dp, qinv, dq and m
   are output.
   d = e^(-1) mod (p-1)*(q-1) and is guaranteed to be fit in n units.
   dp is d mod (p-1) fits in cczp_n(zp) units
   qinv is q^(-1) mod p fits in cczp_n(zp) units
   dq is d mod (q-1) fits in cczp_n(zq) units
   m = p * q and will fit in n units. */

int ccrsa_crt_makekey(cczp_t zm,             /* p * q nm unit sized. */
                             const cc_unit *e,      /* public exponent cczp_n(zm) sized. */
                             cc_unit *d,            /* private exponent cczp_n(zm) sized. */
                             cczp_t zp,             /* zp * zq = public modulus */
                             cc_unit *dp,           /* d mod (p-1)   cczp_n(zp) sized */
                             cc_unit *qinv,         /* q^(-1) mod p  cczp_n(zp) sized */
                             cczp_t zq,             /* zp * zq = public modulus */
                             cc_unit *dq) {         /* d mod (q-1)   cczp_n(zp) sized */
    cc_size n = cczp_n(zm);

    /* modulus = p * q */
    /* p might be one whole unit longer than q, but the public modulus will
     never be more than pbits + qbits bits, and qbits is one less bit
     than pbits. */
#if CCRSA_DUMP_KEY
    ccn_lprint(cczp_n(zp), "\np = 0x", cczp_prime(zp));
    ccn_lprint(cczp_n(zq), "\nq = 0x", cczp_prime(zq));
#endif
    assert(ccn_cmpn(ccn_n(cczp_n(zp), cczp_prime(zp)), cczp_prime(zp),
                    ccn_n(cczp_n(zq), cczp_prime(zq)), cczp_prime(zq)) > 0);

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

    /* d = e^(-1) mod (p-1)(q-1) */
    cczp_decl_n(n, zphi);
    CCZP_N(zphi) = n;
    
    /* phi = (p-1)(q-1) = (pq - p - q + 1) = (m - p - q + 1) */
    ccn_subn(n, CCZP_PRIME(zphi), cczp_prime(zm), cczp_n(zp), cczp_prime(zp));
    ccn_subn(n, CCZP_PRIME(zphi), cczp_prime(zphi), cczp_n(zq), cczp_prime(zq));
    ccn_add1(n, CCZP_PRIME(zphi), CCZP_PRIME(zphi), 1);

    /* Compute the modular inverse of e over zphi. */
    cczp_init(zphi);
    cczp_mod_inv_slow(zphi, d, e);

#if CCRSA_DUMP_KEY
    ccn_lprint(n,      "\ne = 0x", e);
    ccn_lprint(n,      "\nd = 0x", d);
    ccn_lprint(cczp_n(zphi),      "\nphi = 0x", cczp_prime(zphi));
#endif

    /* Since p and q are both odd we just clear bit 0 to subtract 1. Making
     cczp_prime(zp) p - 1 and cczp_prime(zq) q - 1 and setting up the
     recipricols properly. */
    CCZP_PRIME(zp)[0] &= ~CC_UNIT_C(1);
    CCZP_PRIME(zq)[0] &= ~CC_UNIT_C(1);

    /* Since cczp_n(zp) can be n / 2 + 1, and cczp_mod() reads 2 * cczp_n(zp)
       units from it's third argument, this implies dtmp must be at least
       n + 2 units long for reading.
       TODO: Eliminate excess copies here, since cczp_mod makes yet another copy.
       Doing so would require ccn_muln and cczp_modn routines. */
    cc_unit dtmp[n + 2];
    ccn_set(n, dtmp, d);
    ccn_zero(2, dtmp + n);

    /* dp = d mod (p-1) */
    cczp_init(zp);
    cczp_modn(zp, dp, n, dtmp);
    /* dq = d mod (q-1) */
    cczp_init(zq);
    cczp_modn(zq, dq, n, dtmp);

    /* Set zp and zq back to p and q respectively by setting bit 0 back to
       one and recomputing the recipricols. */
    CCZP_PRIME(zp)[0] |= 1;
    CCZP_PRIME(zq)[0] |= 1;
    if (cczp_n(zp) > cczp_n(zq))
        CCZP_PRIME(zq)[cczp_n(zp) - 1] = 0;

    /* qInv = q^(-1) mod p. This requires q to be at least as long as p with
       proper zero padding. Our caller takes care if this. Obviously qInv can
       be as big as p too. */
    if (cczp_mod_inv(zp, qinv, cczp_prime(zq)))
        return CCRSA_KEYGEN_MODULUS_CRT_INV_ERROR;

    /* Initialize zq after the cczp_mod_inv above, since the recipricol of
       zq starts right after q in memory and we want the extra zero during
       the computation of qinv. */
    cczp_init(zp);
    cczp_init(zq);

    return 0;
}

static int ccrsa_generate_key_raw(unsigned long nbits, ccrsa_full_ctx_t fk,
                       size_t e_size, const void *e, struct ccrng_state *rng) {
    cc_size pbits = (nbits >> 1) + 1, qbits = nbits - pbits;
    cc_size n = ccn_nof(nbits);

    /* size of pub zp priv zp and zq - ensure p > q */
    ccrsa_ctx_n(fk) = n;
    ccrsa_pub_ctx_t pubk = ccrsa_ctx_public(fk);
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(fk);
    CCZP_N(ccrsa_ctx_private_zp(privk)) = ccn_nof(pbits);
    CCZP_N(ccrsa_ctx_private_zq(privk)) = ccn_nof(qbits);
    if (ccn_read_uint(n, ccrsa_ctx_e(pubk), e_size, e))
        return CCRSA_KEY_ERROR;

    if(ccn_bitlen(n,ccrsa_ctx_e(pubk))<=1) return CCRSA_KEY_ERROR; // e must be >1

    do {
        /* Generate random n bit primes p and q. cczp_prime(zq) might
           require a cc_unit of 0 padding since qbits < pbits. */
        CCZP_PRIME(ccrsa_ctx_private_zq(privk))[cczp_n(ccrsa_ctx_private_zp(privk)) - 1] = 0;
        if(cczp_random_prime(pbits, ccrsa_ctx_private_zp(privk), ccrsa_ctx_e(pubk), rng) != 0) return CCRSA_KEYGEN_PRIME_NOT_FOUND;
        if(cczp_random_prime(qbits, ccrsa_ctx_private_zq(privk), ccrsa_ctx_e(pubk), rng) != 0) return CCRSA_KEYGEN_PRIME_NOT_FOUND;
        /* Repeat if we fail to make a valid key from the candidates. */
    } while (ccrsa_crt_makekey(ccrsa_ctx_zm(pubk), ccrsa_ctx_e(pubk), ccrsa_ctx_d(fk),
                               ccrsa_ctx_private_zp(privk),
                               ccrsa_ctx_private_dp(privk), ccrsa_ctx_private_qinv(privk),
                               ccrsa_ctx_private_zq(privk), ccrsa_ctx_private_dq(privk)));

#if CCRSA_DUMP_KEY
    ccrsa_dump_key(fk);
#endif /* CCRSA_DUMP_KEY */

    return 0;
}

int ccrsa_generate_key(unsigned long nbits, ccrsa_full_ctx_t fk,
                       size_t e_size, const void *e, struct ccrng_state *rng)
{
	int iResult = 0;
	if ((iResult = (ccrsa_generate_key_raw(nbits, fk, e_size, e, rng))))
	{
		return iResult;
	}
	
	return ccrsa_pairwise_consistency_check(fk) ? 0 : CCRSA_KEYGEN_KEYGEN_CONSISTENCY_FAIL;
}
