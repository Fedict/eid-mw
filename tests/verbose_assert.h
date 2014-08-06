#ifndef VERBOSE_ASSERT_H
#define VERBOSE_ASSERT_H

#include <assert.h>

extern int va_counter;

#define verbose_assert(a) { printf("assertion %d: ", va_counter++); assert((a)); printf("ok\n"); }

#endif
