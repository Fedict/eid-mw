
/*
 *  Copyright (c) 2004 Apple Computer, Inc. All Rights Reserved.
 * 
 *  @APPLE_LICENSE_HEADER_START@
 *  
 *  This file contains Original Code and/or Modifications of Original Code
 *  as defined in and that are subject to the Apple Public Source License
 *  Version 2.0 (the 'License'). You may not use this file except in
 *  compliance with the License. Please obtain a copy of the License at
 *  http://www.opensource.apple.com/apsl/ and read it before using this
 *  file.
 *  
 *  The Original Code and all software distributed under the License are
 *  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 *  EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 *  INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *  Please see the License for the specific language governing rights and
 *  limitations under the License.
 *  
 *  @APPLE_LICENSE_HEADER_END@
 */

/*
 *  BELPICRecord.cpp
 *  TokendMuscle
 */

#include "BELPICRecord.h"

#include "BELPICError.h"
#include "BELPICToken.h"
#include "Attribute.h"
#include "MetaAttribute.h"
#include "MetaRecord.h"
#include <security_cdsa_client/aclclient.h>
#include <Security/SecKey.h>


//
// BELPICRecord
//
BELPICRecord::~BELPICRecord()
{
}

//
// BELPICCertificateRecord
//
BELPICBinaryFileRecord::~BELPICBinaryFileRecord()
{
}

#define BELPIC_MAXSIZE_CERT           8000

BELPICCertificateRecord::~BELPICCertificateRecord()
{
}

Tokend::Attribute *
	BELPICCertificateRecord::getDataAttribute(Tokend::TokenContext *
						  tokenContext)
{
	CssmData data;

	BELPICToken & belpicToken =
		static_cast < BELPICToken & >(*tokenContext);
	if (belpicToken.cachedObject(0, mDescription, data))
	{
		Tokend::Attribute * attribute =
			new Tokend::Attribute(data.Data, data.Length);
		free(data.Data);
		return attribute;
	}

	PCSC::Transaction _(belpicToken);
	belpicToken.select(mDF, mEF);

	uint8 certificate[BELPIC_MAXSIZE_CERT];
	size_t certificateLength = sizeof(certificate);

	belpicToken.readBinary(certificate, certificateLength);
	data.Data = certificate;
	data.Length = certificateLength;

	/* Zetes: the cert files on cards older then V1.0 have padding bytes
	 * at the end (to allow updates that might be longer then the current
	 * cert). This works fine for certificate selection (keychain) but
	 * gives problems during an SSL handshake. So we first remove those
	 * padding bytes by adapting the data.Length. */
	if (certificateLength > 500 && certificate[0] == 0x30
	    && certificate[1] == 0x82)
	{
		size_t realCertLength =
			256 * certificate[2] + certificate[3] + 4;
		if (realCertLength > 500
		    && realCertLength < certificateLength)
			data.Length = realCertLength;
	}

	belpicToken.cacheObject(0, mDescription, data);

	return new Tokend::Attribute(data.Data, data.Length);
}

//
// BELPICProtectedRecord
//
BELPICProtectedRecord::~BELPICProtectedRecord()
{
}

Tokend::Attribute *
	BELPICProtectedRecord::getDataAttribute(Tokend::TokenContext *
						tokenContext)
{
	// no caching
	CssmData data;

	BELPICToken & belpicToken =
		static_cast < BELPICToken & >(*tokenContext);

	PCSC::Transaction _(belpicToken);
	belpicToken.select(mDF, mEF);

	uint8 certificate[BELPIC_MAXSIZE_CERT];
	size_t certificateLength = sizeof(certificate);

	belpicToken.readBinary(certificate, certificateLength);
	data.Data = certificate;
	data.Length = certificateLength;

	return new Tokend::Attribute(data.Data, data.Length);
}

void BELPICProtectedRecord::getAcl(const char *tag, uint32 & count,
				   AclEntryInfo * &acls)
{
	if (!mAclEntries)
	{
		mAclEntries.allocator(Allocator::standard());
		// Reading this object's data requires PIN1
		mAclEntries.
			add(CssmClient::AclFactory::
			    PinSubject(mAclEntries.allocator(), 1),
			    AclAuthorizationSet
			    (CSSM_ACL_AUTHORIZATION_DB_READ, 0));
	}
	count = mAclEntries.size();
	acls = mAclEntries.entries();
}

//
// BELPICKeyRecord
//
BELPICKeyRecord::BELPICKeyRecord(const uint8_t * keyId,
				 const char *description, uint32_t keySize,
				 const Tokend::MetaRecord & metaRecord,
				 bool signOnly,
				 bool PPDU):BELPICRecord(description),
mKeyId(keyId), mKeySize(keySize), mSignOnly(signOnly), mPPDU(PPDU)
{
	attributeAtIndex(metaRecord.metaAttribute(kSecKeyDecrypt).
			 attributeIndex(), new Tokend::Attribute(!signOnly));
	attributeAtIndex(metaRecord.metaAttribute(kSecKeyUnwrap).
			 attributeIndex(), new Tokend::Attribute(!signOnly));
	attributeAtIndex(metaRecord.metaAttribute(kSecKeySign).
			 attributeIndex(), new Tokend::Attribute(signOnly));
	attributeAtIndex(metaRecord.metaAttribute(kSecKeyKeySizeInBits).
			 attributeIndex(), new Tokend::Attribute(keySize));
	attributeAtIndex(metaRecord.metaAttribute(kSecKeyEffectiveKeySize).
			 attributeIndex(), new Tokend::Attribute(keySize));
}

BELPICKeyRecord::~BELPICKeyRecord()
{
}

void BELPICKeyRecord::computeCrypt(BELPICToken & belpicToken, bool sign,
				   const AccessCredentials * cred,
				   const unsigned char *data,
				   size_t dataLength, unsigned char *output,
				   size_t & outputLength)
{
	PCSC::Transaction _(belpicToken);
	belpicToken.selectKeyForSign(mKeyId);

	if (mPPDU == true)
	{
		belpicToken._verifyPIN(1, NULL, 0);
	} else if (cred)
	{
		uint32 size = cred->size();

		for (uint32 ix = 0; ix < size; ++ix)
		{
			const TypedList & sample = (*cred)[ix];

			if (sample.type() ==
			    CSSM_SAMPLE_TYPE_PROMPTED_PASSWORD
			    && sample.length() == 2)
				if (sample.type() ==
				    CSSM_SAMPLE_TYPE_PASSWORD)
				{
					CssmData & pin = sample[1].data();
					if (pin.Length >= BELPIC_MIN_PIN_LEN
					    && pin.Length <=
					    BELPIC_MAX_PIN_LEN)
					{
						belpicToken._verifyPIN(1,
								       pin.
								       Data,
								       pin.
								       Length);
						break;
					} else if (pin.Length == 0)
					{
						// %%% <rdar://4334623>
						// PIN previously verified by securityd;
						// continue to look at remaining samples
					} else
					{
						CssmError::
							throwMe
							(CSSM_ERRCODE_SAMPLE_VALUE_NOT_SUPPORTED);
					}
				}
		}

	}

	if (dataLength > sizeInBits() / 8)
		CssmError::throwMe(CSSMERR_CSP_BLOCK_SIZE_MISMATCH);

	if (sign != mSignOnly)
		CssmError::throwMe(CSSMERR_CSP_KEY_USAGE_INCORRECT);

	size_t apduSize = dataLength + 5;
	unsigned char apdu[apduSize];
	size_t resultLength = sizeInBits() / 8 + 2;
	unsigned char result[resultLength];

	apdu[0] = 0x00;
	apdu[1] = 0x2A;
	apdu[2] = 0x9E;
	apdu[3] = 0x9A;
	apdu[4] = dataLength;
	memcpy(apdu + 5, data, dataLength);
	BELPICError::check(belpicToken.exchangeAPDU(apdu, apduSize, result,
						    resultLength));
	if (resultLength != sizeInBits() / 8 + 2)
	{
		secdebug("belpic",
			 " %s: computeCrypt: expected size: %ld, got: %ld",
			 mDescription, sizeInBits() / 8 + 2, resultLength);
		PCSC::Error::throwMe(SCARD_E_PROTO_MISMATCH);
	}

	if (outputLength < resultLength - 2)
		CssmError::throwMe(CSSMERR_CSP_BLOCK_SIZE_MISMATCH);

	outputLength = resultLength - 2;
	memcpy(output, result, outputLength);
}

void BELPICKeyRecord::getAcl(const char *tag, uint32 & count,
			     AclEntryInfo * &acls)
{
	// @@@ Key 1 has any acl for sign, key 2 has pin1 acl, and key3 has pin1
	// acl with auto-lock which we express as a prompted password subject.
	if (!mAclEntries)
	{
		mAclEntries.allocator(Allocator::standard());
		// Anyone can read the DB record for this key (which is a reference
		// CSSM_KEY)
		mAclEntries.
			add(CssmClient::AclFactory::
			    AnySubject(mAclEntries.allocator()),
			    AclAuthorizationSet
			    (CSSM_ACL_AUTHORIZATION_DB_READ, 0));
		// Setup the remainder of the acl based on the key type.

		//PIN1 is hardcoded for now
		// Apparently, PINS other than '1' are not yet supported by TokenD.
		char tmptag[20];

		snprintf(tmptag, sizeof(tmptag), "PIN%d", 1);
		if (*mKeyId == 0x82)
		{
			if (mPPDU == true)
			{
				mAclEntries.
					add(CssmClient::AclFactory::
					    AnySubject(mAclEntries.
						       allocator()),
					    AclAuthorizationSet
					    (CSSM_ACL_AUTHORIZATION_SIGN, 0),
					    tmptag);
			} else
			{
				mAclEntries.
					add(CssmClient::AclFactory::
					    PinSubject(mAclEntries.
						       allocator(), 1),
					    AclAuthorizationSet
					    (CSSM_ACL_AUTHORIZATION_SIGN, 0),
					    tmptag);
			}
		} else if (*mKeyId == 0x83)
		{
			CssmData prompt;

			mAclEntries.
				add(CssmClient::AclFactory::
				    PromptPWSubject(mAclEntries.allocator(),
						    prompt),
				    AclAuthorizationSet
				    (CSSM_ACL_AUTHORIZATION_SIGN, 0), tmptag);
		}
	}
	count = mAclEntries.size();
	acls = mAclEntries.entries();
}
