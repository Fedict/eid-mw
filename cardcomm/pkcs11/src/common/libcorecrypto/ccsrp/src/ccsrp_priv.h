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


#include <corecrypto/cc.h>
#include <corecrypto/cc_debug.h>
#include <corecrypto/ccpbkdf2.h>
#include <corecrypto/ccsrp.h>
#include <corecrypto/cczp_priv.h>
#include <corecrypto/cc_macros.h>

/* Context Dump
 
 The current context is mapped space containing:
 
 digest di pointer
 gp pointer
 authenticated boolean flag (bit field size 1)
 noUsernameInX boolean flag (bit field size 1)
 padding to 32 bytes
 cc_unit[n] public key
 cc_unit[n] private key
 uint8_t[di->output_size] session key
 uint8_t[di->output_size] M
 uint8_t[di->output_size] HAMK
*/

CC_INLINE const uint8_t *dump_hex(const uint8_t *p, size_t len) {
    for(size_t i=0; i<len; i++) {
        cc_printf("%02x", *p);
        p++;
    }
    cc_printf("\n");
    return p;
}

CC_INLINE void
dump_hex_label(char *label, const uint8_t *p, size_t len) {
    cc_printf(">>>>> %s: ", label);
    dump_hex(p, len);
}

 
CC_INLINE void
dump_ctx(char *label, ccsrp_ctx_t srp) {
    const uint8_t *p = (const uint8_t *) srp._full;
    size_t bytes_n = ccsrp_ctx_sizeof_n(srp);
    size_t bytes_d = ccsrp_ctx_keysize(srp);
    
    cc_printf("______________________%s start ________________________\n\n", label);
    cc_printf("\nHeader Pointers, etc\n");
    p = dump_hex(p, CCSRP_HDR_PAD);
    cc_printf("public  key: ");
    p = dump_hex(p, bytes_n);
    cc_printf("private key: ");
    p = dump_hex(p, bytes_n);
    cc_printf("session key: ");
    p = dump_hex(p, bytes_d);
    cc_printf("          M: ");
    p = dump_hex(p, bytes_d);
    cc_printf("       HAMK: ");
    dump_hex(p, bytes_d);
    cc_printf("______________________%s end   ________________________\n\n", label);
}

CC_INLINE void
srp_lprint(cc_size n, const char *label, const cc_unit *s) {
    cc_printf(">>>>> %s: ", label);
    dump_hex((const uint8_t *)s, ccn_sizeof_n(n));
    cc_printf("\n");
}



CC_INLINE void
ccsrp_export_ccn(ccsrp_ctx_t srp, const cc_unit *a, void *bytes) {
    ccn_write_uint_padded(ccsrp_ctx_n(srp), a, ccsrp_ctx_sizeof_n(srp), bytes);
}

CC_INLINE void
ccsrp_import_ccn(ccsrp_ctx_t srp, cc_unit *a, const void *bytes) {
    ccn_read_uint(ccsrp_ctx_n(srp), a, ccsrp_ctx_sizeof_n(srp), bytes);
}

CC_INLINE void
ccsrp_import_ccn_with_len(ccsrp_ctx_t srp, cc_unit *a, size_t len, const void *bytes) {
    ccn_read_uint(ccsrp_ctx_n(srp), a, len, bytes);
}

CC_INLINE void
ccsrp_digest_ccn(ccsrp_ctx_t srp, const cc_unit *s, void *dest) {
    uint8_t buf[ccsrp_ctx_sizeof_n(srp)];
    ccsrp_export_ccn(srp, s, buf);
    ccdigest(ccsrp_ctx_di(srp), ccsrp_ctx_sizeof_n(srp), buf, dest);
}

CC_INLINE void
ccsrp_digest_ccn_ccn(ccsrp_ctx_t srp, cc_unit *r, const cc_unit *a, const cc_unit *b) {
    const struct ccdigest_info *di = ccsrp_ctx_di(srp);
    cc_size n = ccsrp_ctx_n(srp);
    uint8_t hash[di->output_size];
    uint8_t ab_bytes[ccn_sizeof_n(n)*2];
    ccsrp_export_ccn(srp, a, ab_bytes);
    ccsrp_export_ccn(srp, b, ab_bytes+ccn_sizeof_n(n));
    ccdigest(di, ccn_sizeof_n(n)*2, ab_bytes, hash);
    cc_clear(ccn_sizeof_n(n)*2,ab_bytes);
    ccn_read_uint(ccsrp_ctx_n(srp), r, di->output_size, hash);
    cc_clear(di->output_size,hash);
}

CC_INLINE void
ccsrp_digest_update_ccn(ccsrp_ctx_t srp, ccdigest_ctx_t ctx, const cc_unit *s) {
    uint8_t buf[ccsrp_ctx_sizeof_n(srp)];
    ccsrp_export_ccn(srp, s, buf);
    ccdigest_update(ccsrp_ctx_di(srp), ctx, ccsrp_ctx_sizeof_n(srp), buf);
}

// x = SHA(s | SHA(U | ":" | p))

CC_INLINE void
ccsrp_generate_x(ccsrp_ctx_t srp, cc_unit *x,
                 const char *username,
                 size_t salt_len, const void *salt, 
                 size_t password_len, const void *password) {
    const struct ccdigest_info *di = ccsrp_ctx_di(srp);
    uint8_t hash[di->output_size];
    ccdigest_di_decl(di, ctx);
    ccdigest_init(di, (ccdigest_ctx_t)ctx);
    if (!srp.hdr->flags.noUsernameInX)
        ccdigest_update(di, (ccdigest_ctx_t)ctx, strlen(username), username);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, 1, ":");
    ccdigest_update(di, (ccdigest_ctx_t)ctx, password_len, password);
    ccdigest_final(di, (ccdigest_ctx_t)ctx, hash);
    ccdigest_init(di, (ccdigest_ctx_t)ctx);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, salt_len, salt);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, di->output_size, hash);
    ccdigest_final(di, (ccdigest_ctx_t)ctx, hash);
    ccsrp_import_ccn_with_len(srp, x, di->output_size, hash);
    cc_clear(di->output_size,hash);
}

CC_INLINE void
ccsrp_generate_k(ccsrp_ctx_t srp, cc_unit *k) {
    ccsrp_digest_ccn_ccn(srp, k, ccdh_gp_prime(ccsrp_ctx_gp(srp)), ccsrp_ctx_gp_g(srp));
}

CC_INLINE void
ccsrp_generate_v(ccsrp_ctx_t srp, cc_unit *x) {
    cczp_power(ccsrp_ctx_zp(srp), ccsrp_ctx_v(srp), ccsrp_ctx_gp_g(srp), x);
}

CC_INLINE void
ccsrp_generate_u(ccsrp_ctx_t srp, cc_unit *u, cc_unit *A, cc_unit *B) {
    ccsrp_digest_ccn_ccn(srp, u, A, B);
}

CC_INLINE void
ccsrp_generate_server_S(ccsrp_ctx_t srp, cc_unit *S, cc_unit *u, cc_unit *A) {
    /* S = (A *(v^u)) ^ b */
    cc_unit tmp1[ccsrp_ctx_n(srp)], tmp2[ccsrp_ctx_n(srp)];
    ccn_zero_multi(ccsrp_ctx_n(srp), tmp1, tmp2, NULL);
    cczp_power_fast(ccsrp_ctx_zp(srp), tmp1, ccsrp_ctx_v(srp), u); // u is public, ok to use non secure exponentiation
    cczp_mul(ccsrp_ctx_zp(srp), tmp2, A, tmp1);
    cczp_power(ccsrp_ctx_zp(srp), S, tmp2, ccsrp_ctx_private(srp));
    ccn_zero_multi(ccsrp_ctx_n(srp), tmp1, tmp2, NULL);
}

CC_INLINE void
ccsrp_generate_client_S(ccsrp_ctx_t srp, cc_unit *S, cc_unit *k, cc_unit *x, cc_unit *u, cc_unit *B) {
    /* Client Side S = (B - k*(g^x)) ^ (a + ux) */
    cc_size n=ccsrp_ctx_n(srp);
    cc_unit tmp1[2*n], tmp2[ccsrp_ctx_n(srp)];
    ccn_zero_multi(n, tmp1, tmp2, NULL);
    cc_unit c;
    // In ccsrp_init
    // tmp1 = a + ux
    ccn_mul(n, tmp1, u, x);
    c=ccn_add(n, tmp1, ccsrp_ctx_private(srp), tmp1);
    if (2*8*ccsrp_ctx_di(srp)->output_size >= ccn_bitlen(n,ccsrp_ctx_prime(srp))) {
        // if u*x is bigger than p in size, need to handle carry
        // and reduction mod p-1
        ccn_add1(n,&tmp1[n],&tmp1[n],c);
        ccn_sub1(n,tmp2,ccsrp_ctx_prime(srp),1); // p-1
        ccn_divmod(n,NULL,tmp1,tmp1,tmp2);
    }
    else {
        cc_assert(c==0); // Carry is not possible here
    }
    
    // tmp2 = k * (g^x)
    cczp_power(ccsrp_ctx_zp(srp), tmp2, ccsrp_ctx_gp_g(srp), x);
    cczp_mul(ccsrp_ctx_zp(srp), tmp2, k, tmp2);
    
    // tmp2 = (B - k*(g^x))
    cczp_sub(ccsrp_ctx_zp(srp), tmp2, B, tmp2);
    
    // S = tmp2 ^ tmp1
    cczp_power(ccsrp_ctx_zp(srp), S, tmp2, tmp1);
    ccn_zero_multi(n, tmp1, tmp2, NULL);
}

CC_INLINE void
ccsrp_generate_M(ccsrp_ctx_t srp, const char *username, size_t salt_len, const void *salt,
            const cc_unit *A, const cc_unit *B) {
    const struct ccdigest_info *di = ccsrp_ctx_di(srp);
    size_t hashlen = di->output_size;
    uint8_t hash_n[hashlen];
    uint8_t hash_g[hashlen];
    uint8_t H_I[hashlen];
    uint8_t H_xor[hashlen];
    ccdigest_di_decl(di, ctx);    
    
    ccsrp_digest_ccn(srp, ccsrp_ctx_prime(srp), hash_n);
    ccsrp_digest_ccn(srp, ccsrp_ctx_gp_g(srp), hash_g);
    
    ccdigest(di, strlen(username), username, H_I);
    
    for (size_t i=0; i < hashlen; i++ )
        H_xor[i] = hash_n[i] ^ hash_g[i];
    
    ccdigest_init(di, (ccdigest_ctx_t)ctx);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, hashlen, H_xor);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, hashlen, H_I);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, salt_len, salt);
    ccsrp_digest_update_ccn(srp, (ccdigest_ctx_t)ctx, A);
    ccsrp_digest_update_ccn(srp, (ccdigest_ctx_t)ctx, B);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, hashlen, ccsrp_ctx_K(srp));
    ccdigest_final(di, (ccdigest_ctx_t)ctx, ccsrp_ctx_M(srp));
}

CC_INLINE void
ccsrp_generate_H_AMK(ccsrp_ctx_t srp, const cc_unit *A) {
    const struct ccdigest_info *di = ccsrp_ctx_di(srp);
    ccdigest_di_decl(di, ctx);
    ccdigest_init(di, (ccdigest_ctx_t)ctx);
    ccsrp_digest_update_ccn(srp, (ccdigest_ctx_t)ctx, A);
    ccdigest_update(di, (ccdigest_ctx_t)ctx, di->output_size, ccsrp_ctx_M(srp));
    ccdigest_update(di, (ccdigest_ctx_t)ctx, di->output_size, ccsrp_ctx_K(srp));
    ccdigest_final(di, (ccdigest_ctx_t)ctx, ccsrp_ctx_HAMK(srp));
}

CC_INLINE void
ccsrp_generate_client_pubkey(ccsrp_ctx_t srp) {
    cczp_power(ccsrp_ctx_zp(srp), ccsrp_ctx_public(srp), ccsrp_ctx_gp_g(srp), ccsrp_ctx_private(srp));
}

CC_INLINE void
ccsrp_generate_server_pubkey(ccsrp_ctx_t srp, cc_unit *k) {
    /* B = kv + g^b */
    cc_unit tmp1[ccsrp_ctx_n(srp)], tmp2[ccsrp_ctx_n(srp)];
    ccn_zero_multi(ccsrp_ctx_n(srp), tmp1, tmp2, NULL);
    ccn_zero(ccsrp_ctx_n(srp), tmp1);
    cczp_mul(ccsrp_ctx_zp(srp), tmp1, k, ccsrp_ctx_v(srp));
    cczp_power(ccsrp_ctx_zp(srp), tmp2, ccsrp_ctx_gp_g(srp), ccsrp_ctx_private(srp));
    cczp_add(ccsrp_ctx_zp(srp), ccsrp_ctx_public(srp), tmp1, tmp2);
    ccn_zero_multi(ccsrp_ctx_n(srp), tmp1, tmp2, NULL);
}


