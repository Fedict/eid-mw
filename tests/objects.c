#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

#define ADD_CKO(cko) ckos[cko] = #cko

int objects() {
	CK_RV rv;
	CK_SESSION_HANDLE session;
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	CK_SLOT_ID slot;
	CK_ATTRIBUTE attr;
	CK_OBJECT_CLASS klass;
	char* ckos[8];
	int ret;

	ADD_CKO(CKO_DATA);
	ADD_CKO(CKO_CERTIFICATE);
	ADD_CKO(CKO_PUBLIC_KEY);
	ADD_CKO(CKO_PRIVATE_KEY);
	ADD_CKO(CKO_SECRET_KEY);
	ADD_CKO(CKO_HW_FEATURE);
	ADD_CKO(CKO_DOMAIN_PARAMETERS);
	ADD_CKO(CKO_MECHANISM);

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
	check_rv;

	rv = C_FindObjectsInit(session, NULL_PTR, 0);
	check_rv;

	do {
		rv = C_FindObjects(session, &object, 1, &count);
		check_rv;

		if(!count) continue;

		attr.type = CKA_CLASS;
		attr.pValue = &klass;
		attr.ulValueLen=sizeof(klass);
		rv = C_GetAttributeValue(session, object, &attr, 1);
		check_rv;

		printf("Found object %lx of class %s (%#lx)\n", object, ckos[klass], klass);
		verbose_assert(klass == CKO_CERTIFICATE || klass == CKO_PUBLIC_KEY || klass == CKO_PRIVATE_KEY);
	} while(count);

	rv = C_FindObjectsInit(session, NULL_PTR, 0);
	assert(ckrv_decode(rv, 2,
		CKR_OK, TEST_RV_FAIL,
		CKR_OPERATION_ACTIVE, TEST_RV_OK) == TEST_RV_OK);

	rv = C_FindObjectsFinal(session);
	check_rv;

	rv = C_CloseSession(session);
	check_rv;

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
	check_rv;

	rv = C_FindObjectsInit(session, NULL_PTR, 0);
	check_rv;

	rv = C_CloseSession(session);
	check_rv;

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
	check_rv;

	rv = C_FindObjectsInit(session, NULL_PTR, 0);
	check_rv;

	rv = C_CloseSession(session);
	check_rv;

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
	check_rv;

	if(have_robot()) {
		rv = C_FindObjectsInit(session, NULL_PTR, 0);
		check_rv;

		robot_remove_card();
		rv = C_FindObjects(session, &object, 1, &count);
		assert(ckrv_decode(rv, 2,
			CKR_TOKEN_NOT_PRESENT, TEST_RV_OK,
			CKR_DEVICE_REMOVED, TEST_RV_OK) == TEST_RV_OK);

		rv = C_GetAttributeValue(session, object, &attr, 1);
		assert(ckrv_decode(rv, 3,
			CKR_OK, TEST_RV_FAIL,
			CKR_TOKEN_NOT_PRESENT, TEST_RV_OK,
			CKR_DEVICE_REMOVED, TEST_RV_OK) == TEST_RV_OK);

		robot_insert_card();

		rv = C_FindObjectsInit(session, NULL_PTR, 0);
		assert(ckrv_decode(rv, 2,
			CKR_OK, TEST_RV_FAIL,
			CKR_DEVICE_REMOVED, TEST_RV_OK,	// really?!?
			CKR_SESSION_HANDLE_INVALID, TEST_RV_OK) == TEST_RV_OK);

		rv = C_CloseSession(session);
		check_rv;

		rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
		check_rv;

		rv = C_FindObjectsInit(session, NULL_PTR, 0);
		check_rv;
	} else {
		printf("Robot not present, skipping card removal/insertion part of test...\n");
	}

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
