
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
 *  BELPICSchema.h
 *  TokendMuscle
 */

#ifndef _BELPICSCHEMA_H_
#define _BELPICSCHEMA_H_

#include "Schema.h"
#include "BELPICKeyHandle.h"
#include "BELPICAttributeCoder.h"

namespace Tokend
{
	class Relation;
	class MetaRecord;
	class AttributeCoder;
}

class BELPICSchema:public Tokend::Schema
{
NOCOPY(BELPICSchema) public:
	BELPICSchema();
	virtual ~ BELPICSchema();

	virtual void create();

protected:
	Tokend::Relation * createKeyRelation(CSSM_DB_RECORDTYPE keyType);

private:
	// Coders we need.
	BELPICDataAttributeCoder mBELPICDataAttributeCoder;

	Tokend::ConstAttributeCoder mKeyAlgorithmCoder;
	BELPICKeySizeAttributeCoder mKeySizeCoder;

	BELPICKeyHandleFactory mBELPICKeyHandleFactory;
};

#endif /* !_BELPICSCHEMA_H_ */
