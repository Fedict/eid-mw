#ifndef BEID_FUZZ_H
#define BEID_FUZZ_H
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
#include "common/export.h"
#ifdef __cplusplus
extern "C" {
#endif
EIDMW_CAL_API void beid_set_fuzz_data(const uint8_t *data, size_t size, const char* filename);
#ifdef __cplusplus
}
#endif
#endif
