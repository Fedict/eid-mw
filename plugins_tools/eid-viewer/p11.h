#ifndef EID_VWR_P11_H
#define EID_VWR_P11_H






#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include <pkcs11t.h>
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11t.h>
#endif

#include <eid-util/utftranslate.h>

#define EIDV_RV_OK 0
#define EIDV_RV_FAIL -1
#define EIDV_RV_TERMINATE -2
#define EIDV_RV_MEM_FAIL -3

#ifdef WIN32
//event waited for by the mainloop, before checking the reader states
HANDLE readerCheckEvent;
//event set by the mainloop, to notify it completed checking the reader states
HANDLE readerContinueWaitEvent;
DWORD WINAPI eid_wait_for_pkcs11event(void* val);
int eid_vwr_p11_check_reader_list(void* slot_ID);
int eid_vwr_p11_update_slot_list_ui(CK_SLOT_ID_PTR slotlist, CK_ULONG slotCount);
int eid_vwr_p11_reset_slot_list(CK_SLOT_ID_PTR *ppcurrentSlotList, CK_ULONG *pcurrentReaderCount, CK_SLOT_ID *pcurrentCardSlotID);
int eid_vwr_p11_find_eid_card(CK_SLOT_ID_PTR slotID);
int eid_vwr_p11_token_supported(CK_SLOT_ID_PTR slotID);
int eid_vwr_p11_wait_for_slot_event(BOOLEAN blocking, CK_SLOT_ID_PTR pSlotID);
#else
int eid_vwr_p11_find_first_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR loc, CK_ULONG_PTR slots_found);
#endif

struct _slotdesc;

int eid_vwr_p11_init(void);
int eid_vwr_p11_read_id(void *);
int eid_vwr_p11_read_certs(void *);
int eid_vwr_p11_open_session(void *slot);
int eid_vwr_p11_close_session(void);
int eid_vwr_p11_finalize_find(void);
int eid_vwr_p11_do_pinop(void *);
int eid_vwr_p11_do_challenge(void* data);
int eid_vwr_p11_leave_pinop(void);
int eid_vwr_p11_select_slot(CK_BBOOL automatic, CK_SLOT_ID manualslot);
int eid_vwr_p11_name_slots(struct _slotdesc *slots, CK_ULONG_PTR len);
int eid_vwr_p11_check_version(void *data);

#ifdef __cplusplus
extern "C"
{
#endif
	void eid_vwr_p11_to_ui(const EID_CHAR *, const void *, int);
#ifdef __cplusplus
}
#endif

#endif
