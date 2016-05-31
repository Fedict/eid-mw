#ifndef EID_VWR_MACROS_H
#define EID_VWR_MACROS_H

#ifdef WIN32
#define DllExport __declspec( dllexport )
#else
#define DllExport __attribute__((visibility("default")))
#endif

#endif
