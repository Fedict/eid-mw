
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "hash.h"
#include "mwexception.h"
#include "eiderrors.h"

using namespace eIDMW;

CHash::CHash()
{
	m_bInitialized = false;
}

unsigned long CHash::GetHashLength(tHashAlgo algo)
{
	switch (algo)
	{
		case ALGO_MD5:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCMD5_OUTPUT_SIZE;
#else
            return CC_MD5_DIGEST_LENGTH;
#endif
#else
			return 16;
#endif

		case ALGO_SHA1:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCSHA1_OUTPUT_SIZE;
#else
            return CC_SHA1_DIGEST_LENGTH;
#endif
#else
			return 20;
#endif

        case ALGO_MD5_SHA1:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return (CCMD5_OUTPUT_SIZE + CCSHA1_OUTPUT_SIZE);
#else
            return (CC_MD5_DIGEST_LENGTH + CC_SHA1_DIGEST_LENGTH);
#endif
#else
			return 36;
#endif

        case ALGO_SHA256:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCSHA256_OUTPUT_SIZE;
#else
            return CC_SHA256_DIGEST_LENGTH;
#endif
#else
			return 32;
#endif

        case ALGO_SHA384:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCSHA384_OUTPUT_SIZE;
#else
            return CC_SHA384_DIGEST_LENGTH;
#endif
#else
			return 48;
#endif

        case ALGO_SHA512:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCSHA512_OUTPUT_SIZE;
#else
            return CC_SHA512_DIGEST_LENGTH;
#endif
#else
			return 64;
#endif

        case ALGO_RIPEMD160:
#if defined(__APPLE__) && defined(__MACH__) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
			return CCRMD160_STATE_SIZE;
#else
            return CC_SHA1_DIGEST_LENGTH; // Same length as SHA1
#endif
#else
			return 20;
#endif
	}

    throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

    return 0;
}

CByteArray CHash::Hash(tHashAlgo algo, const CByteArray & data)
{
	return Hash(algo, data, 0, data.Size());
}

CByteArray CHash::Hash(tHashAlgo algo, const CByteArray & data,
		       unsigned long ulOffset, unsigned long ulLen)
{
	Init(algo);
	Update(data, ulOffset, ulLen);
	return GetHash();
}

void CHash::Init(tHashAlgo algo)
{
	switch (algo)
	{
		case ALGO_MD5:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccmd5_di(), m_md1);
#else
            CC_MD5_Init(&m_md1.md5);
#endif
#else
			md5_init(&m_md1);
#endif
			break;

		case ALGO_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccsha1_di(), m_md1);
#else
            CC_SHA1_Init(&m_md1.sha1);
#endif
#else
			sha1_init(&m_md1);
#endif
			break;

		case ALGO_MD5_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccmd5_di(), m_md1);
            ccdigest_init(ccsha1_di(), m_md2);
#else
            CC_MD5_Init(&m_md1.md5);
            CC_SHA1_Init(&m_md2.sha1);
#endif
#else
			md5_init(&m_md1);
			sha1_init(&m_md2);
#endif
			break;

		case ALGO_SHA256:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccsha256_di(), m_md1);
#else
            CC_SHA256_Init(&m_md1.sha256);
#endif
#else
			sha256_init(&m_md1);
#endif
			break;

		case ALGO_SHA384:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccsha384_di(), m_md1);
#else
            CC_SHA384_Init(&m_md1.sha512);
#endif
#else
			sha384_init(&m_md1);
#endif
			break;

		case ALGO_SHA512:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(ccsha512_di(), m_md1);
#else
            CC_SHA512_Init(&m_md1.sha512);
#endif
#else
			sha512_init(&m_md1);
#endif
            break;

		case ALGO_RIPEMD160:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_init(&ccrmd160_di, m_md1);
#else
            CCDigestInit(kCCDigestRMD160, m_md1.rmd160);
#endif
#else
			rmd160_init(&m_md1);
#endif
			break;

		default:
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	m_Algo = algo;
	m_bInitialized = true;
}

void CHash::Update(const CByteArray & data)
{
	Update(data, 0, data.Size());
}

void CHash::Update(const CByteArray & data, unsigned long ulOffset,
		   unsigned long ulLen)
{
	if (m_bInitialized == false)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	if (ulLen != 0)
	{
		const unsigned char *pucData = data.GetBytes() + ulOffset;

		switch (m_Algo)
		{
			case ALGO_MD5:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccmd5_di(), m_md1, ulLen, pucData);
#else
                CC_MD5_Update(&m_md1.md5, pucData, (CC_LONG)ulLen);
#endif
#else
				md5_process(&m_md1, pucData, ulLen);
#endif
				break;

			case ALGO_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccsha1_di(), m_md1, ulLen, pucData);
#else
                CC_SHA1_Update(&m_md1.sha1, pucData, (CC_LONG)ulLen);
#endif
#else
				sha1_process(&m_md1, pucData, ulLen);
#endif
				break;

			case ALGO_MD5_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccmd5_di(), m_md1, ulLen, pucData);
                ccdigest_update(ccsha1_di(), m_md2, ulLen, pucData);
#else
                CC_MD5_Update(&m_md1.md5, pucData, (CC_LONG)ulLen);
                CC_SHA1_Update(&m_md2.sha1, pucData, (CC_LONG)ulLen);
#endif
#else
				md5_process(&m_md1, pucData, ulLen);
				sha1_process(&m_md2, pucData, ulLen);
#endif
				break;

			case ALGO_SHA256:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccsha256_di(), m_md1, ulLen, pucData);
#else
                CC_SHA256_Update(&m_md1.sha256, pucData, (CC_LONG)ulLen);
#endif
#else
				sha256_process(&m_md1, pucData, ulLen);
#endif
				break;

			case ALGO_SHA384:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccsha384_di(), m_md1, ulLen, pucData);
#else
                CC_SHA384_Update(&m_md1.sha512, pucData, (CC_LONG)ulLen);
#endif
#else
				sha384_process(&m_md1, pucData, ulLen);
#endif
				break;

			case ALGO_SHA512:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(ccsha512_di(), m_md1, ulLen, pucData);
#else
                CC_SHA512_Update(&m_md1.sha512, pucData, (CC_LONG)ulLen);
#endif
#else
				sha512_process(&m_md1, pucData, ulLen);
#endif
				break;

			case ALGO_RIPEMD160:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
                ccdigest_update(&ccrmd160_ltc_di, m_md1, ulLen, pucData);
#else
                CCDigestUpdate(m_md1.rmd160, pucData, ulLen);
#endif
#else
				rmd160_process(&m_md1, pucData, ulLen);
#endif
				break;
		}
	}

    throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
}

CByteArray CHash::GetHash(void)
{
	if (m_bInitialized == false)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	m_bInitialized = false;

	// hash result
    // make sure this is enough if other hashes are added!!!
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
    unsigned char tucHash[CCSHA512_OUTPUT_SIZE] = { 0 };
#else
    unsigned char tucHash[CC_SHA512_DIGEST_LENGTH] = { 0 };
#endif
#else
    unsigned char tucHash[64] = { 0 };
#endif

	switch (m_Algo)
	{
		case ALGO_MD5:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccmd5_di(), m_md1, tucHash);
#else
            CC_MD5_Final(tucHash, &m_md1.md5);
#endif
#else
			md5_done(&m_md1, tucHash);
#endif
			break;

		case ALGO_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccsha1_di(), m_md1, tucHash);
#else
            CC_SHA1_Final(tucHash, &m_md1.sha1);
#endif
#else
			sha1_done(&m_md1, tucHash);
#endif
			break;

		case ALGO_MD5_SHA1:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccmd5_di(), m_md1, tucHash);
            ccdigest_final(ccsha1_di(), m_md2, (tucHash + CCMD5_OUTPUT_SIZE));
#else
            CC_MD5_Final(tucHash, &m_md1.md5);
            CC_SHA1_Final((tucHash + CC_MD5_DIGEST_LENGTH), &m_md2.sha1);
#endif
#else
			md5_done(&m_md1, tucHash);
			sha1_done(&m_md2, (tucHash + 16));
#endif
			break;

		case ALGO_SHA256:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccsha256_di(), m_md1, tucHash);
#else
            CC_SHA256_Final(tucHash, &m_md1.sha256);
#endif
#else
			sha256_done(&m_md1, tucHash);
#endif
			break;

		case ALGO_SHA384:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccsha384_di(), m_md1, tucHash);
#else
            CC_SHA384_Final(tucHash, &m_md1.sha512);
#endif
#else
			sha384_done(&m_md1, tucHash);
#endif
			break;

		case ALGO_SHA512:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(ccsha512_di(), m_md1, tucHash);
#else
            CC_SHA512_Final(tucHash, &m_md1.sha512);
#endif
#else
			sha512_done(&m_md1, tucHash);
#endif
			break;

		case ALGO_RIPEMD160:
#if (defined(__APPLE__) && defined(__MACH__)) && !defined(_USE_LIBTOMCRYPT)
#ifdef _USE_CORECRYPTO
            ccdigest_final(&ccrmd160_ltc_di, m_md1, tucHash);
#else
            CCDigestFinal(m_md1.rmd160, tucHash);
#endif
#else
			rmd160_done(&m_md1, tucHash);
#endif
			break;

		default:
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	return CByteArray(tucHash, GetHashLength(m_Algo));
}
