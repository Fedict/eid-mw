
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifndef __CERT_H__
#define __CERT_H__

#include "asn1.h"

#ifdef WIN32
#define snprintf _snprintf
#else
#define strnicmp strncasecmp
#endif

/* constants */
#define LEN_DATE  14
#define MAX_OID_SIZE  32
#define MAX_OID_STR_SIZE 1024
#define MAX_DATE_STR_SIZE 1024

/* errors */
#define E_X509_BUF_TOO_SMALL         -1
#define E_X509_TIME_WRONG_FORMAT     -2
#define E_X509_DECODE                -3
#define E_X509_ALLOC                 -4
#define E_X509_UNKNOWN_KEYTYPE       -5
#define E_X509_DN_BAD_ENC            -6
#define E_X509_INCOMPLETE            -7

#ifdef __cplusplus
extern "C"
{
#endif
	/* x509v3:
		SubjectPublicKeyInfo																		/1 1 7/

		SubjectPublicKeyInfo  ::=  SEQUENCE  {
        algorithm            AlgorithmIdentifier,													/1 1 7 1/
		subjectPublicKey     BIT STRING  }															/1 1 7 2/
	
		AlgorithmIdentifier  ::=  SEQUENCE  {														/1 1 7 1/
		algorithm               OBJECT IDENTIFIER,													/1 1 7 1 1/
		parameters              ANY DEFINED BY algorithm OPTIONAL 									/1 1 7 1 2/(should be NULL for RSA, defines curve type for EC (see EcpkParameters))

	 for RSA:
		in subjectPublicKey     BIT STRING:															/1 1 7 2/
		RSAPublicKey ::= SEQUENCE {																	/1 1 7 2 1/(sequence in the bitstring, so first jump into the bitstring, then one deeper into the sequence)
		modulus            INTEGER,    -- n															/1 1 7 2 1 1/
		publicExponent     INTEGER  }  -- e															/1 1 7 2 1 2/

	 for EC:

		in subjectPublicKey     BIT STRING:															/1 1 7 2/
		The elliptic curve public key (an ECPoint which is an OCTET STRING)							
		is mapped to a subjectPublicKey (a BIT STRING)												/1 1 7 2 1/(jump into the bitstring)

		EcpkParameters ::= CHOICE {
		ecParameters  ECParameters(SEQUENCE),				
		namedCurve    OBJECT IDENTIFIER,															/1 1 7 1 2/
		implicitlyCA  NULL }

*/


#define X509_VERSION          "\1\1\1\1"
#define X509_SERIAL           "\1\1\2"
#define X509_SIGN_ALGO	      "\1\1\3"
#define X509_ISSUER           "\1\1\4"
#define X509_VALID_FROM       "\1\1\5\1"
#define X509_VALID_UNTIL      "\1\1\5\2"
#define X509_SUBJECT          "\1\1\6"
#define X509_KEYTYPE          "\1\1\7\1\1"
#define X509_EC_CURVE	      "\1\1\7\1\2"
#define X509_PUBLIC_KEY       "\1\1\7\2"
#define X509_PKINFO           "\1\1\7\2\1"
#define X509_RSA_MOD          "\1\1\7\2\1\1"
#define X509_RSA_EXP          "\1\1\7\2\1\2"

#define X509_SIGNATURE_OID    "\1\2\1"
#define X509_SIGNATURE        "\1\3"

	typedef struct
	{
		unsigned int lcert;
		char *subject;
		unsigned int l_subject;
		char *issuer;
		unsigned int l_issuer;
		char *mod;
		unsigned int l_mod;
		char *exp;
		unsigned int l_exp;
		char *pkinfo;
		unsigned int l_pkinfo;
		char *serial;
		unsigned int l_serial;
		char *validfrom;
		unsigned int l_validfrom;
		char *validto;
		unsigned int l_validto;
		char *curve;
		unsigned int l_curve;
	} T_CERT_INFO;

	typedef struct
	{
		unsigned int lkey;
		char *mod;
		unsigned int l_mod;
		char *exp;
		unsigned int l_exp;
		char *pkinfo;
		unsigned int l_pkinfo;
		char *curve;
		unsigned int l_curve;
	} T_KEY_INFO;

	int cert_get_info(const unsigned char *pcert, unsigned int lcert, T_CERT_INFO * info);
	void cert_free_info(T_CERT_INFO * info);

	int key_get_info(const unsigned char *pkey, unsigned int lkey, T_KEY_INFO * info);
	void key_free_info(T_KEY_INFO * info);

#ifdef __cplusplus
}
#endif


#endif
