#ifndef EID_VWR_MACROS_H
#define EID_VWR_MACROS_H

#ifdef __GNUC__
#include <unistd.h>
#endif

#if defined(WIN32) || defined(_WIN32)

#ifndef DllExport
#define DllExport __declspec( dllexport )
#endif

#ifndef SLEEP
#define SLEEP(x) Sleep(x*1000)
#endif

#ifndef NORETURN
#define NORETURN
#endif

#else

#ifndef DllExport
#if defined(__GNUC__) || defined(__clang__)
#define DllExport __attribute__((visibility("default")))
#else
#define DllExport
#endif
#endif

#ifndef SLEEP
#define SLEEP(x) sleep(x)
#endif

#ifndef NORETURN
#if defined(__GNUC__) || defined(__clang__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif
#endif
#endif

#endif /* EID_VWR_MACROS_H */
