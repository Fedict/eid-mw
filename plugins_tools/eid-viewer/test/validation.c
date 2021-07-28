#include <unix.h>
#include <pkcs11.h>
#include <testlib2.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>
#include "testlib.h"
#include "../dataverify.h"

int loadData(uint8_t** array, size_t* size, char* label, CK_SLOT_ID slot, CK_SESSION_HANDLE session){
	
	CK_OBJECT_HANDLE object ;
	CK_ULONG count = 0;
	CK_ULONG type;
	CK_ATTRIBUTE attr[2];
	char* name;
	
	attr[0].type = CKA_LABEL;
	attr[0].pValue = label;
	attr[0].ulValueLen = strlen(label);

	attr[1].type = CKA_CLASS;
	attr[1].pValue = &type;
	type = CKO_DATA;
	attr[1].ulValueLen = sizeof(CK_ULONG);

	check_rv(C_FindObjectsInit(session, attr, 2));

	CK_ATTRIBUTE datab[2] = {
		{CKA_LABEL, NULL_PTR, 0},
		{CKA_VALUE, NULL_PTR, 0},
	};

	check_rv(C_FindObjects(session, &object, 2, &count));
	if(!count){ 
		printf("no object found\n");
		return TEST_RV_FAIL;
	}

	check_rv(C_GetAttributeValue(session, object, datab, 2));
	
	name = malloc(datab[0].ulValueLen+1);
	*array = malloc(datab[1].ulValueLen+1);
	datab[0].pValue = name;
	datab[1].pValue = *array;
	
	check_rv(C_GetAttributeValue(session, object, datab, 2));
	name[datab[0].ulValueLen] = '\0';
	*size = datab[1].ulValueLen ;
	/*uncomment if test fails to see if data is correct:
	printf("label: %s\n",name)
	hex_dump(*array, size);
	*/
	check_rv(C_FindObjectsFinal(session));
	
	free(name);
	return TEST_RV_OK;
}

TEST_FUNC(readvalidation) {
	
	uint8_t *data; size_t datas;
	uint8_t *dsig; size_t dsigs;
	uint8_t *addr; size_t addrs;
	uint8_t *asig; size_t asigs;
	uint8_t *cert; size_t certs;
	uint8_t *phot; size_t phots;
	uint8_t *hash; size_t hashs;
	CK_SLOT_ID slot;
	CK_SESSION_HANDLE session;
	int ret = TEST_RV_OK;

        int offset;
        uint8_t odata[512];
	uint8_t odsig[512];
	uint8_t oaddr[512];
	uint8_t oasig[512];
	uint8_t ophot[8192];
	uint8_t ohash[0x30];
	
	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));	
	
	if (loadData( &data, &datas, "DATA_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL; 
	if (loadData( &dsig, &dsigs, "SIGN_DATA_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL; 
	if (loadData( &addr, &addrs, "ADDRESS_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL;
	if (loadData( &asig, &asigs, "SIGN_ADDRESS_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL;
	if (loadData( &hash, &hashs, "photo_hash", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL;
	if (loadData( &cert, &certs, "CERT_RN_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL;
	if (loadData( &phot, &phots, "PHOTO_FILE", slot, session) == TEST_RV_FAIL) ret = TEST_RV_FAIL;
	
	if (ret == TEST_RV_FAIL) return ret;
	
	//happy flow
	if (!eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
   		return TEST_RV_FAIL;
	}
	else {printf("happy flow ok\n");}
	
	//unhappy flow
	int now = time(NULL); 
	//int now = 1623918915;
	srand(now);
	//data change
	if ( rand () % 2){
		offset = rand() % datas;
		printf("data->seed: %d, offset: %d\n",now , offset);
		memcpy(odata, data, datas);
		odata[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, odata, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % dsigs;
		printf("dsig->seed: %d, offset: %d\n",now , offset);
		memcpy(odsig, dsig, dsigs);
		odsig[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, odsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % addrs;
		printf("addr->seed: %d, offset: %d\n",now , offset);
		memcpy(oaddr, addr, addrs);
		oaddr[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, oaddr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){
		offset = rand() % asigs;
		printf("asig->seed: %d, offset: %d\n",now , offset);
		memcpy(oasig, asig, asigs);
		oasig[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, oasig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % phots;
		printf("photo->seed: %d, offset: %d\n",now , offset);
		memcpy(ophot, phot, phots);
		ophot[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(ophot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % hashs;
		printf("hash->seed: %d, offset: %d\n",now , offset);
		memcpy(ohash, hash, hashs);
		ohash[offset]= rand() % 256;
		if (eid_vwr_check_data_validity(phot, phots, ohash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	return TEST_RV_OK;
}
