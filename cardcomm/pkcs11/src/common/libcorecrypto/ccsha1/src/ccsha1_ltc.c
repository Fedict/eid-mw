/*
 * Copyright (c) 2010,2011,2012,2015 Apple Inc. All rights reserved.
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
 * Parts of this code adapted from LibTomCrypt
 *
 * LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */


#define USE_SUPER_COOL_NEW_CCOID_T
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccdigest_priv.h>
#include <corecrypto/cc_priv.h>

#define F0(x,y,z)  (z ^ (x & (y ^ z)))
#define F1(x,y,z)  (x ^ y ^ z)
#define F2(x,y,z)  ((x & y) | (z & (x | y)))
#define F3(x,y,z)  (x ^ y ^ z)


static void sha1_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint32_t a,b,c,d,e,W[80],i;
    uint32_t *s = ccdigest_u32(state);
    const unsigned char *buf = in;

    while (nblocks--) {


        /* copy the state into 512-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            CC_LOAD32_BE(W[i], buf + (4*i));
        }

        /* read state */
        a = s[0];
        b = s[1];
        c = s[2];
        d = s[3];
        e = s[4];

        /* expand it */
        for (i = 16; i < 80; i++) {
            W[i] = CC_ROL(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);
        }

        /* compress */
        /* round one */
#define FF0(a,b,c,d,e,i) e = (CC_ROLc(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999); b = CC_ROLc(b, 30);
#define FF1(a,b,c,d,e,i) e = (CC_ROLc(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1); b = CC_ROLc(b, 30);
#define FF2(a,b,c,d,e,i) e = (CC_ROLc(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdc); b = CC_ROLc(b, 30);
#define FF3(a,b,c,d,e,i) e = (CC_ROLc(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6); b = CC_ROLc(b, 30);

#ifdef CC_SMALL_CODE
        uint32_t t;

        for (i = 0; i < 20; ) {
            FF0(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 40; ) {
            FF1(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 60; ) {
            FF2(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 80; ) {
            FF3(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
        }

#else

        for (i = 0; i < 20; ) {
            FF0(a,b,c,d,e,i++);
            FF0(e,a,b,c,d,i++);
            FF0(d,e,a,b,c,i++);
            FF0(c,d,e,a,b,i++);
            FF0(b,c,d,e,a,i++);
        }

        /* round two */
        for (; i < 40; )  {
            FF1(a,b,c,d,e,i++);
            FF1(e,a,b,c,d,i++);
            FF1(d,e,a,b,c,i++);
            FF1(c,d,e,a,b,i++);
            FF1(b,c,d,e,a,i++);
        }

        /* round three */
        for (; i < 60; )  {
            FF2(a,b,c,d,e,i++);
            FF2(e,a,b,c,d,i++);
            FF2(d,e,a,b,c,i++);
            FF2(c,d,e,a,b,i++);
            FF2(b,c,d,e,a,i++);
        }

        /* round four */
        for (; i < 80; )  {
            FF3(a,b,c,d,e,i++);
            FF3(e,a,b,c,d,i++);
            FF3(d,e,a,b,c,i++);
            FF3(c,d,e,a,b,i++);
            FF3(b,c,d,e,a,i++);
        }
#endif

#undef FF0
#undef FF1
#undef FF2
#undef FF3
        /* store state */
        s[0] += a;
        s[1] += b;
        s[2] += c;
        s[3] += d;
        s[4] += e;

        buf+=CCSHA1_BLOCK_SIZE;
    }
}

const struct ccdigest_info ccsha1_ltc_di = {
    .output_size = CCSHA1_OUTPUT_SIZE,
    .state_size = CCSHA1_STATE_SIZE,
    .block_size = CCSHA1_BLOCK_SIZE,
    .oid_size = ccoid_sha1_len,
    .oid = CC_DIGEST_OID_SHA1,
    .initial_state = ccsha1_initial_state,
    .compress = sha1_compress,
    .final = ccdigest_final_64be,
};
