#ifndef EID_VWR_P11_H
#define EID_VWR_P11_H

#include <unix.h>
#include <pkcs11t.h>

#define EIDV_RV_OK 0
#define EIDV_RV_FAIL -1

int p11_init();
int p11_perform_find(CK_SESSION_HANDLE session, CK_BBOOL do_objid);
int p11_find_first_slot(CK_SLOT_ID_PTR loc);
int p11_read_id(void*);
int p11_read_certs(void*);
int p11_open_session(void* slot);
int p11_close_session(void*);
int p11_finalize_find(void*);

#endif
