/* crypto/cast/cast_lcl.h */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <corecrypto/cc_priv.h>

#define OPENSSL_EXTERN	extern

#define E_CAST(n,key,L,R,OP1,OP2,OP3) \
{   \
	CAST_LONG a,b,c,d; \
	t=(key[n*2] OP1 R)&0xffffffff; \
	t=CC_ROL(t,(int)(key[n*2+1])); \
	a=CAST_S_table0[(t>> 8)&0xff]; \
	b=CAST_S_table1[(t    )&0xff]; \
	c=CAST_S_table2[(t>>24)&0xff]; \
	d=CAST_S_table3[(t>>16)&0xff]; \
	L^=(((((a OP2 b)&0xffffffff) OP3 c)&0xffffffff) OP1 d)&0xffffffff; \
}

#define OPENSSL_GLOBAL

/*
 * These rename this tables to avoid symbols collision between libSystem
 * and libcrypto.
 */
#define CAST_S_table0	CC_CAST_S_table0
#define CAST_S_table1	CC_CAST_S_table1
#define CAST_S_table2	CC_CAST_S_table2
#define CAST_S_table3	CC_CAST_S_table3
#define CAST_S_table4	CC_CAST_S_table4
#define CAST_S_table5	CC_CAST_S_table5
#define CAST_S_table6	CC_CAST_S_table6
#define CAST_S_table7	CC_CAST_S_table7

OPENSSL_EXTERN const CAST_LONG CAST_S_table0[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table1[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table2[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table3[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table4[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table5[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table6[256];
OPENSSL_EXTERN const CAST_LONG CAST_S_table7[256];
