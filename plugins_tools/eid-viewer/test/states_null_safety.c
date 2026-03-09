#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#include <io.h>
#else
#include "unix.h"
#include "pkcs11.h"
#include <unistd.h>
#endif
#include <testlib.h>
#include <eid-viewer/oslayer.h>
#include <eid-viewer/macros.h>
#include <stdlib.h>
#include "common.h"

/* Test for NULL pointer dereference regression */
TEST_FUNC(states_null_safety) {
	/* This test ensures that the state machine handles events gracefully
	 * before initialization and doesn't crash with segmentation fault */
	
	/* Call public API functions before state machine initialization
	 * This should not crash and should handle uninitialized state gracefully */
	eid_vwr_be_deserialize(TEXT("nonexistent_file.xml"));
	eid_vwr_be_serialize(TEXT("test_output.xml"));
	eid_vwr_be_set_invalid();
	
	/* If we reach this point without crashing, the NULL pointer protection is working */
	return TEST_RV_OK;
}
