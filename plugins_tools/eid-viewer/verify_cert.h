#ifndef EID_VWR_VERIFY_H
#define EID_VWR_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <oslayer.h>

enum eid_vwr_result eid_vwr_verify_cert(const void* certificate, size_t certlen, const void* ca, size_t calen, const void*(*perform_ocsp_request)(char*, void*, long, long*, void**), void(*free_ocsp_request)(const void*));

#ifdef __cplusplus
}
#endif

#endif
