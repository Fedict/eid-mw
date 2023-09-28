

#ifndef UNIX_H
#define UNIX_H

#ifdef __GNUC__
#define EXPORTED __attribute__((visibility("default")))
#endif
#define CK_PTR *

#define CK_DEFINE_FUNCTION(returnType, name) \
   returnType EXPORTED name

#define CK_DECLARE_FUNCTION(returnType, name) \
   returnType EXPORTED name

#define CK_DECLARE_FUNCTION_POINTER(returnType, name) \
   returnType (* name)

#define CK_CALLBACK_FUNCTION(returnType, name) \
   returnType (* name)

#ifndef NULL_PTR
#define NULL_PTR 0
#endif

#endif
