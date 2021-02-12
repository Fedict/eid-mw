#ifndef BEID_FUZZ_H
#define BEID_FUZZ_H
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
#ifdef __cplusplus
extern "C" {
#endif
void __attribute__((visibility("default"))) beid_set_fuzz_data(const uint8_t *data, size_t size, const char* filename);
void __attribute__((visibility("default"))) beid_set_fuzz_only(int fuzzed);
#ifdef __cplusplus
}
#endif
#endif
#endif
