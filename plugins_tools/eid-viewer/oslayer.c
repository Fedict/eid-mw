#include <eid-viewer/oslayer.h>
#include <state.h>
#include <p11.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <cache.h>
#include "backend.h"

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	sm_init();
	sm_handle_event(EVENT_SET_CALLBACKS, cb_, NULL, NULL);
	return 0;
}

#ifdef WIN32
DWORD WINAPI eid_wait_for_pkcs11event(void* val) {
	int ret;
	CK_FLAGS flags = 0;
	CK_SLOT_ID slotID;

	while (1)
	{
		ret = C_WaitForSlotEvent(flags,   /*nonblocking flag: CKF_DONT_BLOCK*/
			&slotID,  /* location that receives the slot ID */
			NULL_PTR); /* reserved.  Should be NULL_PTR */

		if (ret != CKR_OK)
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("C_WaitForSlotEvent with retVal: %.8x"), ret);
			if (ret == CKR_CRYPTOKI_NOT_INITIALIZED)
			{
				return;
			}
			SLEEP(1);
		}

		if (!SetEvent(readerCheckEvent))
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("eid_wait_for_pkcs11event with error: %.8x"), GetLastError());
			return;
		}
	}
	return ret;		
}


int eid_vwr_init()
{
	HANDLE hThread;
	int result = CKR_OK;

	readerCheckEvent = CreateEvent(
		NULL,   // default security attributes
		FALSE,  // auto-reset event object
		FALSE,  // initial state is nonsignaled
		NULL);  // unnamed object

	if (readerCheckEvent == NULL)
	{
		be_log(EID_VWR_LOG_ERROR, TEXT("CreateEvent error: %.8x"), GetLastError());
		result = CKR_FUNCTION_FAILED;
	}

	/* Create the pkcs11 card / card reader event thread */
	hThread = CreateThread(NULL, 0, eid_wait_for_pkcs11event, NULL, 0, NULL);
	if (readerCheckEvent == NULL)
	{
		be_log(EID_VWR_LOG_ERROR, TEXT("CreateEvent error: %.8x"), GetLastError());
		result = CKR_FUNCTION_FAILED;
	}
	return result;
}
#endif

void eid_vwr_be_mainloop() {
#ifdef WIN32
	int result;
	int eventSetup = eid_vwr_init();
#endif
	for(;;) {
#ifdef WIN32
		result = eid_vwr_poll();
#else
		eid_vwr_poll();
#endif

#ifdef WIN32
		//use polling if eid_vwr_init failed
		if ( (result == 0) && (eventSetup == CKR_OK) )
		{
			eid_vwr_p11_wait_event();
		}
		else
		{
			SLEEP(1);
		}
#else
		SLEEP(1);
#endif
	}
}

int eid_vwr_poll() {
	CK_SLOT_ID_PTR no_token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	CK_SLOT_ID_PTR token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	static CK_ULONG count_old = 0;
	static CK_SLOT_ID token_old = 0xCAFEBABE;
	CK_ULONG count = 0;
	CK_ULONG i;
	int retval = 0;

	if(eid_vwr_p11_find_first_slot(CK_FALSE, no_token, &count) == EIDV_RV_OK) {
		sm_handle_event(EVENT_READER_FOUND, no_token, free, NULL);
	} else {
		free(no_token);
		//pkcs11 not ready yet?
		retval = 1;
	}
	if(count_old != count) {
		slotdesc* slots = (slotdesc*)malloc(count * sizeof(slotdesc));
		memset(slots, 0, count * sizeof(slotdesc));
		eid_vwr_p11_name_slots(slots, &count);
		if(be_readers_changed(count, slots) == EIDV_RV_OK) {
			count_old = count;
		}
		else
		{
			//no callback present
			retval = 2;
		}
		for (i = 0; i < count; i++)
		{
			if (slots[i].description != NULL) {
				free(slots[i].description);
			}
		}
		free (slots);
	}
	if(eid_vwr_p11_find_first_slot(CK_TRUE, token, &count) == EIDV_RV_OK) {
		if(token_old != *token) {
			CK_SLOT_ID_PTR tmp = malloc(sizeof(CK_SLOT_ID));
			*tmp = *token;
			sm_handle_event(EVENT_TOKEN_REMOVED, tmp, free, NULL);
			token_old = *token;
		}
		sm_handle_event(EVENT_TOKEN_INSERTED, token, free, NULL);
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, token, free, NULL);
	}
	return retval;
}

void eid_vwr_be_serialize(const EID_CHAR* target_file) {
	EID_CHAR* copy = EID_STRDUP(target_file);
	sm_handle_event(EVENT_SERIALIZE, copy, free, NULL);
}

void eid_vwr_be_deserialize(const EID_CHAR* source_file) {
	EID_CHAR* copy = EID_STRDUP(source_file);
	sm_handle_event(EVENT_OPEN_FILE, copy, free, NULL);
}

const char* eid_vwr_be_get_xmlform() {
	const struct eid_vwr_cache_item* item;
	/* When we have card data, the state machine ensures that the
	 * cache will contain the XML form of the data under the "xml"
	 * label. Thus, if that cache entry is empty, this means we
	 * don't have any data yet. */
	if(!cache_have_label(TEXT("xml"))) {
		return NULL;
	}
	item = cache_get_data(TEXT("xml"));
	return (const char*)item->data;
}

void eid_vwr_be_select_slot(int automatic, unsigned long manualslot) {
	eid_vwr_p11_select_slot(automatic ? CK_TRUE : CK_FALSE, (CK_SLOT_ID)manualslot);
}

void eid_vwr_be_set_invalid() {
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
}

void eid_vwr_close_file() {
	sm_handle_event(EVENT_CLOSE_FILE, NULL, NULL, NULL);
}
