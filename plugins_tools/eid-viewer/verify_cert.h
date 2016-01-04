#ifndef EID_VWR_VERIFY_H
#define EID_VWR_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <oslayer.h>

enum eid_vwr_result eid_vwr_verify_cert(void* certificate, size_t certlen, void* ca, size_t calen);

#ifdef __cplusplus
}
#endif

#endif
