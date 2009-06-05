#include "MyApp.h"

#ifdef _WIN32
#ifdef _DEBUG
        #pragma comment( lib, "beidlibd" )
#else
        #pragma comment( lib, "beidlib" )
#endif
#endif

// otherwise ugly Java VM crash on linux when firefox closed
#ifdef WIN32
static CMyApp *pMyApp = new CMyApp();
#endif

#if defined(__GNUC__)
    typedef long long __int64; /*For gcc on Windows */
#endif
#include <jni.h>
#include <stdlib.h>
#include <string.h>

#define BEID_TITLE "Exception from eidlib JNI"

typedef struct
{
    long data;
} BEID_Long;

/* Support for throwing Java exceptions */
typedef enum 
{
  JavaOutOfMemoryError = 1, 
  JavaIOException, 
  JavaRuntimeException, 
  JavaIndexOutOfBoundsException,
  JavaArithmeticException,
  JavaIllegalArgumentException,
  JavaNullPointerException,
  JavaDirectorPureVirtual,
  JavaUnknownError
} JavaExceptionCodes;

typedef struct 
{
  JavaExceptionCodes code;
  const char *java_exception;
} JavaExceptions_t;


static void JavaThrowException(JNIEnv *jenv, JavaExceptionCodes code, const char *msg) 
{
  jclass excep;
  static const JavaExceptions_t java_exceptions[] = 
  {
    { JavaOutOfMemoryError, "java/lang/OutOfMemoryError" },
    { JavaIOException, "java/io/IOException" },
    { JavaRuntimeException, "java/lang/RuntimeException" },
    { JavaIndexOutOfBoundsException, "java/lang/IndexOutOfBoundsException" },
    { JavaArithmeticException, "java/lang/ArithmeticException" },
    { JavaIllegalArgumentException, "java/lang/IllegalArgumentException" },
    { JavaNullPointerException, "java/lang/NullPointerException" },
    { JavaDirectorPureVirtual, "java/lang/RuntimeException" },
    { JavaUnknownError,  "java/lang/UnknownError" },
    { (JavaExceptionCodes)0,  "java/lang/UnknownError" } };
  const JavaExceptions_t *except_ptr = java_exceptions;

  while (except_ptr->code != code && except_ptr->code)
    except_ptr++;

  jenv->ExceptionClear();
  excep = jenv->FindClass(except_ptr->java_exception);
  if (excep)
    jenv->ThrowNew(excep, msg);

    /* free the local ref */
    jenv->DeleteLocalRef(excep);
}

#define BEID_assert(nullreturn, expr, msg) if (!(expr)) {JavaThrowException(jenv, JavaIllegalArgumentException, msg); return nullreturn; } else
#define BEID_assert_noret(expr, msg) if (!(expr)) {JavaThrowException(jenv, JavaIllegalArgumentException, msg); return; } else


#include <eidlib.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Status_1general(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Status *arg1 = (BEID_Status *) 0 ;
    long result = 0;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Status **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->general);
    }
    
    jresult = (jint)result; 
    return jresult;
}

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Status_1system(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Status *arg1 = (BEID_Status *) 0 ;
    long result = 0;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Status **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->system);
    }
    
    jresult = (jint)result; 
    return jresult;
}

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Status_1pcsc(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Status *arg1 = (BEID_Status *) 0 ;
    long result = 0;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Status **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->pcsc);
    }
    
    jresult = (jint)result; 
    return jresult;
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Status_1cardSW(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Status *arg1 = (BEID_Status *) 0 ;
    jbyteArray  jSWArray;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Status **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        jSWArray = jenv->NewByteArray((jsize)2);
        if (jSWArray == NULL) 
        {
            return NULL;
        }
    }
    
  /* copy the temp buffer into the java array */
  jenv->SetByteArrayRegion(jSWArray, 0, 2, (jbyte *) arg1->cardSW);
  if (jenv->ExceptionOccurred() != NULL)
    return NULL;

  return jSWArray;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Status(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Status *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Status *)(BEID_Status *) calloc(1, sizeof(BEID_Status));
    
    *(BEID_Status **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Status(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Status *arg1 = (BEID_Status *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Status **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1certif(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Certif *arg1 = (BEID_Certif *) 0 ;
    jbyteArray  jCertifArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif **)&jarg1; 
    
    BEID_assert(0, arg1, BEID_TITLE)
    {
        jCertifArray = jenv->NewByteArray(((arg1)->certifLength));
        if (jCertifArray == NULL) 
        {
            return NULL;
        }
    }
    
  /* copy the temp buffer into the java array */
  jenv->SetByteArrayRegion(jCertifArray, 0, arg1->certifLength, (jbyte *) arg1->certif);
  if (jenv->ExceptionOccurred() != NULL)
    return NULL;

  return jCertifArray;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1certifLabel(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Certif *arg1 = (BEID_Certif *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->certifLabel);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1certifStatus(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Certif *arg1 = (BEID_Certif *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->certifStatus);
    }
    
    jresult = (jint)result; 
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Certif(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Certif *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Certif *)(BEID_Certif *) calloc(1, sizeof(BEID_Certif));
    
    *(BEID_Certif **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Certif(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Certif *arg1 = (BEID_Certif *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif **)&jarg1; 
    free((char *) arg1);
    
}


JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1Check_1usedPolicy(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Certif_Check *arg1 = (BEID_Certif_Check *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif_Check **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->usedPolicy);
    }
    
    jresult = (jint)result; 
    return jresult;
}

JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1Check_1certificate(JNIEnv *jenv, jclass jcls, jlong jarg1, jint jarg2) {
    jlong jresult = 0 ;
    BEID_Certif_Check *arg1 = (BEID_Certif_Check *) 0;
    int arg2 = 0;
    BEID_Certif *result = 0;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif_Check **)&jarg1;
    arg2 = (int)jarg2;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        if(arg2 < arg1->certificatesLength)
        {
            result = (BEID_Certif *)(BEID_Certif *) &((arg1)->certificates[arg2]);
        }
    }
    *(BEID_Certif **)&jresult = result; 
    return jresult;
}

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1Check_1certificatesLength(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Certif_Check *arg1 = (BEID_Certif_Check *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif_Check **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->certificatesLength);
    }
    
    jresult = (jint)result; 
    return jresult;
}

JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Certif_1Check_1signatureCheck(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_Certif_Check *arg1 = (BEID_Certif_Check *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif_Check **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->signatureCheck);
    }
    
    jresult = (jint)result; 
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Certif_1Check(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Certif_Check *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Certif_Check *)(BEID_Certif_Check *) calloc(1, sizeof(BEID_Certif_Check));
    
    *(BEID_Certif_Check **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Certif_1Check(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Certif_Check *arg1 = (BEID_Certif_Check *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Certif_Check **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Pin_1pinType(JNIEnv *jenv, jclass jcls, jlong jarg1, jint jarg2) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    long arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    arg2 = (long)jarg2; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if (arg1) (arg1)->pinType = arg2;
    }
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Pin_1id(JNIEnv *jenv, jclass jcls, jlong jarg1, jshort jarg2) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    BYTE arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    arg2 = (BYTE)jarg2; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if (arg1) (arg1)->id = arg2;
    }
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Pin_1usageCode(JNIEnv *jenv, jclass jcls, jlong jarg1, jint jarg2) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    long arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    arg2 = (long)jarg2; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if (arg1) (arg1)->usageCode = arg2;
    }
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Pin_1shortUsage(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    char *arg2;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        arg2 = 0;
        if (jarg2) 
        {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return;
        }
    }
    {
        if (arg1->shortUsage) free((char*)arg1->shortUsage);
        if (arg2) 
        {
            arg1->shortUsage = (char *) malloc(strlen(arg2)+1);
            strcpy((char*)arg1->shortUsage,arg2);
        } 
        else 
        {
            arg1->shortUsage = 0;
        }
    }
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Pin_1longUsage(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    char *arg2;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        arg2 = 0;
        if (jarg2) 
        {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return ;
        }
    }
    {
        if (arg1->longUsage) free((char*)arg1->longUsage);
        if (arg2) 
        {
            arg1->longUsage = (char *) malloc(strlen(arg2)+1);
            strcpy((char*)arg1->longUsage,arg2);
        } 
        else 
        {
            arg1->longUsage = 0;
        }
    }
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Pin(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Pin *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Pin *)(BEID_Pin *) calloc(1, sizeof(BEID_Pin));
    
    *(BEID_Pin **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Pin(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Pin *arg1 = (BEID_Pin *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1version(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    short result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (short) ((arg1)->version);
    }
    
    jresult = (jshort)result; 
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1cardNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->cardNumber);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1chipNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->chipNumber);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1validityDateBegin(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->validityDateBegin);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1validityDateEnd(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->validityDateEnd);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1municipality(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->municipality);
        if(result) 
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1nationalNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->nationalNumber);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1name(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->name);
        if(result) 
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1firstName1(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->firstName1);
        if(result) 
        {    
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1firstName2(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->firstName2);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1firstName3(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->firstName3);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1nationality(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->nationality);
        jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1birthLocation(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->birthLocation);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1birthDate(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->birthDate);    
        jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1sex(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->sex);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1nobleCondition(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->nobleCondition);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1documentType(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (long) ((arg1)->documentType);    
        jresult = (jint)result; 
    }
    return jresult;
}


JNIEXPORT jboolean JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1whiteCane(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jboolean jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    BOOL result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BOOL) ((arg1)->whiteCane);    
        jresult = (jboolean)result; 
    }
    return jresult;
}


JNIEXPORT jboolean JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1yellowCane(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jboolean jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    BOOL result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BOOL) ((arg1)->yellowCane);
        jresult = (jboolean)result; 
    }
    return jresult;
}


JNIEXPORT jboolean JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1extendedMinority(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jboolean jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    BOOL result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BOOL) ((arg1)->extendedMinority);    
        jresult = (jboolean)result; 
    }
    return jresult;
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1ID_1Data_1hashPhoto(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    BYTE *result;
    jbyteArray  jHashArray;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *)(BYTE *) ((arg1)->hashPhoto);
        jHashArray = jenv->NewByteArray((jsize)BEID_MAX_HASH_PICTURE_LEN);
        if (jHashArray == NULL) 
        {
            return NULL;
        }
    }
    
  /* copy the temp buffer into the java array */
  jenv->SetByteArrayRegion(jHashArray, (jsize)0, (jsize)BEID_MAX_HASH_PICTURE_LEN, (jbyte *)result);
  if (jenv->ExceptionOccurred() != NULL)
    return NULL;

  return jHashArray;    
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1ID_1Data(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_ID_Data *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_ID_Data *)(BEID_ID_Data *) calloc(1, sizeof(BEID_ID_Data));
    
    *(BEID_ID_Data **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1ID_1Data(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1version(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    short result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (short) ((arg1)->version);    
        jresult = (jshort)result; 
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1street(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->street);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1streetNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->streetNumber);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1boxNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->boxNumber);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1zip(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->zip);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}

JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1municipality(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->municipality);
        if(result)
        {
//#ifdef _UNICODE
//            jresult = jenv->NewString(result, _tcslen(result)); 
//#else
            jresult = jenv->NewStringUTF(result); 
//#endif
        }
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Address_1country(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jstring jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (char *)(char *) ((arg1)->country);
        if(result) jresult = jenv->NewStringUTF(result); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Address(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Address *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Address *)(BEID_Address *) calloc(1, sizeof(BEID_Address));
    
    *(BEID_Address **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Address(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    free((char *) arg1);    
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1SerialNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE *result;
    jbyteArray  jSerialArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *)(BYTE *) ((arg1)->SerialNumber);
    
        jSerialArray = jenv->NewByteArray((jsize)16);
        if (jSerialArray == NULL) 
        {
            return NULL;
        }
    }
    
  /* copy the temp buffer into the java array */
  jenv->SetByteArrayRegion(jSerialArray, (jsize)0, (jsize)16, (jbyte *)result);
  if (jenv->ExceptionOccurred() != NULL)
    return NULL;

  return jSerialArray;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1ComponentCode(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->ComponentCode);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1OSNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->OSNumber);    
        jresult = (jshort)result; 
    }
    return jresult;
}

JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1OSVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->OSVersion);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1SoftmaskNumber(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->SoftmaskNumber);    
        jresult = (jshort)result; 
    }
    return jresult;
}

JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1SoftmaskVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->SoftmaskVersion);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1AppletVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->AppletVersion);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jint JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1GlobalOSVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    unsigned short result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (unsigned short) ((arg1)->GlobalOSVersion);    
        jresult = (jint)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1AppletInterfaceVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->AppletInterfaceVersion);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1PKCS1Support(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->PKCS1Support);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1KeyExchangeVersion(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->KeyExchangeVersion);
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1ApplicationLifeCycle(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->ApplicationLifeCycle);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1GraphPerso(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->GraphPerso);
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1ElecPerso(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->ElecPerso);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1ElecPersoInterface(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->ElecPersoInterface);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jshort JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1VersionInfo_1Reserved(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BYTE result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE) ((arg1)->Reserved);    
        jresult = (jshort)result; 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1VersionInfo(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_VersionInfo *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_VersionInfo *)(BEID_VersionInfo *) calloc(1, sizeof(BEID_VersionInfo));
    
    *(BEID_VersionInfo **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1VersionInfo(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Bytes_1data(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Bytes *arg1 = (BEID_Bytes *) 0 ;
    BYTE *result;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Bytes **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->data);
        jArray = jenv->NewByteArray((jsize)arg1->length);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }
    
    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->length, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}

JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Bytes(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Bytes *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Bytes *)(BEID_Bytes *) calloc(1, sizeof(BEID_Bytes));
    result->data = (BYTE *) (BYTE *) calloc(4096, sizeof(unsigned char));
    result->length = 4096;

    *(BEID_Bytes **)&jresult = result; 
    return jresult;
}



JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Bytes(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Bytes *arg1 = (BEID_Bytes *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Bytes **)&jarg1;
    free((char *) arg1->data);
    arg1->length = 0;
    free((char *) arg1);
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Long_1data(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jlong jresult = 0 ;
    BEID_Long *arg1 = (BEID_Long *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Long **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        jresult = (long) ((arg1)->data);
    }
    
    return jresult;
}

JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Long(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Long *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Long *)(BEID_Long *) calloc(1, sizeof(BEID_Long));
    
    *(BEID_Long **)&jresult = result; 
    return jresult;
}



JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Long(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Long *arg1 = (BEID_Long *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Long **)&jarg1; 
    free((char *) arg1);
    
}

JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1idData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->idData, arg2, lLength);
            arg1->idLength = lLength;
        }
        else
        {
            arg1->idLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1idData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->idData);
        jArray = jenv->NewByteArray((jsize)arg1->idLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->idLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1idSigData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->idSigData, arg2, lLength);
            arg1->idSigLength = lLength;
        }
        else
        {
            arg1->idSigLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1idSigData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->idSigData);
        jArray = jenv->NewByteArray((jsize)arg1->idSigLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->idSigLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1addrData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->addrData, arg2, lLength);
            arg1->addrLength = lLength;
        }
        else
        {
            arg1->addrLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1addrData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->addrData);
        jArray = jenv->NewByteArray((jsize)arg1->addrLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->addrLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1addrSigData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->addrSigData, arg2, lLength);
            arg1->addrSigLength = lLength;
         }
        else
        {
            arg1->addrSigLength = 0;
        }
   }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1addrSigData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->addrSigData);
        jArray = jenv->NewByteArray((jsize)arg1->addrSigLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->addrSigLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1pictureData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->pictureData, arg2, lLength);
            arg1->pictureLength = lLength;
        }
        else
        {
            arg1->pictureLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1pictureData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->pictureData);
        jArray = jenv->NewByteArray((jsize)arg1->pictureLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->pictureLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1cardData(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->cardData, arg2, lLength);
            arg1->cardDataLength = lLength;
        }
        else
        {
            arg1->cardDataLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1cardData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->cardData);
        jArray = jenv->NewByteArray((jsize)arg1->cardDataLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->cardDataLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1tokenInfo(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->tokenInfo, arg2, lLength);
            arg1->tokenInfoLength = lLength;
        }
        else
        {
            arg1->tokenInfoLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1tokenInfo(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->tokenInfo);
        jArray = jenv->NewByteArray((jsize)arg1->tokenInfoLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->tokenInfoLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1certRN(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->certRN, arg2, lLength);
            arg1->certRNLength = lLength;
        }
        else
        {
            arg1->certRNLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1certRN(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->certRN);
        jArray = jenv->NewByteArray((jsize)arg1->certRNLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->certRNLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1challenge(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->challenge, arg2, lLength);
            arg1->challengeLength = lLength;
        }
        else
        {
            arg1->challengeLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1challenge(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->challenge);
        jArray = jenv->NewByteArray((jsize)arg1->challengeLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->challengeLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_set_1BEID_1Raw_1response(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *arg2 = (BYTE *) 0;
    long lLength = 0;

    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert_noret(arg1, BEID_TITLE)
    {
        if(jarg2 != NULL)
        {
            arg2 = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
            lLength = jenv->GetArrayLength(jarg2);
            memcpy(arg1->response, arg2, lLength);
            arg1->responseLength = lLength;
        }
        else
        {
            arg1->responseLength = 0;
        }
    }
}


JNIEXPORT jbyteArray JNICALL Java_be_belgium_eid_eidlibJNI_get_1BEID_1Raw_1response(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BYTE *result = (BYTE *) 0;
    jbyteArray  jArray;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = (BYTE *) ((arg1)->response);
        jArray = jenv->NewByteArray((jsize)arg1->responseLength);
        if (jArray == NULL) 
        {
            return NULL;
        }
    }

    /* copy the temp buffer into the java array */
    jenv->SetByteArrayRegion(jArray, (jsize)0, (jsize)arg1->responseLength, (jbyte *)result);
    if (jenv->ExceptionOccurred() != NULL)
        return NULL;

    return jArray;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_new_1BEID_1Raw(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Raw *result;
    
    (void)jenv;
    (void)jcls;
    result = (BEID_Raw *)(BEID_Raw *) calloc(1, sizeof(BEID_Raw));
    
    *(BEID_Raw **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_be_belgium_eid_eidlibJNI_delete_1BEID_1Raw(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    free((char *) arg1);
    
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1Init(JNIEnv *jenv, jclass jcls, jstring jarg1, jint jarg2, jint jarg3, jlong jarg4) {
    jlong jresult = 0 ;
    char *arg1 ;
    long arg2 ;
    long arg3 ;
    BEID_Long *arg4 = (BEID_Long *) 0;
    BEID_Status * resultptr = NULL;

    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    {
        arg1 = 0;
        if (jarg1) 
        {
            arg1 = (char *)jenv->GetStringUTFChars(jarg1, 0);
            if (!arg1) return 0;
        }
    }
    arg2 = (long)jarg2; 
    arg3 = (long)jarg3; 
    arg4 = *(BEID_Long **)&jarg4;
    BEID_assert(0, arg4, BEID_TITLE)
    {
        result = BEID_Init(arg1,arg2,arg3,&(arg4->data));
        resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg1) jenv->ReleaseStringUTFChars(jarg1, arg1); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1Exit(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    result = BEID_Exit();
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetID(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
    jlong jresult = 0 ;
    BEID_ID_Data *arg1 = (BEID_ID_Data *) 0 ;
    BEID_Certif_Check *arg2 = (BEID_Certif_Check *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_ID_Data **)&jarg1; 
    arg2 = *(BEID_Certif_Check **)&jarg2;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        BEID_assert(0, arg2, BEID_TITLE)
        {
            result = BEID_GetID(arg1,arg2);
        }
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetAddress(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
    jlong jresult = 0 ;
    BEID_Address *arg1 = (BEID_Address *) 0 ;
    BEID_Certif_Check *arg2 = (BEID_Certif_Check *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Address **)&jarg1; 
    arg2 = *(BEID_Certif_Check **)&jarg2; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        BEID_assert(0, arg2, BEID_TITLE)
        {
            result = BEID_GetAddress(arg1,arg2);
        }
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetPicture(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
    jlong jresult = 0 ;
    BEID_Bytes *arg1 = (BEID_Bytes *) 0 ;
    BEID_Certif_Check *arg2 = (BEID_Certif_Check *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Bytes **)&jarg1; 
    arg2 = *(BEID_Certif_Check **)&jarg2; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        BEID_assert(0, arg2, BEID_TITLE)
        {
            result = BEID_GetPicture(arg1,arg2);
        }
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetRawData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jlong jresult = 0 ;
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = BEID_GetRawData(arg1);
    }

    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1SetRawData(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jlong jresult = 0 ;
    BEID_Raw *arg1 = (BEID_Raw *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Raw **)&jarg1; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = BEID_SetRawData(arg1);
    }

    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetVersionInfo(JNIEnv *jenv, jclass jcls, jlong jarg1, jint jarg2, jlong jarg3) {
    jlong jresult = 0 ;
    BEID_VersionInfo *arg1 = (BEID_VersionInfo *) 0 ;
    BOOL arg2;
    BEID_Bytes *arg3 = (BEID_Bytes *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_VersionInfo **)&jarg1; 
    arg2 = (BOOL)jarg2; 
    arg3 = *(BEID_Bytes **)&jarg3;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        result = BEID_GetVersionInfo(arg1,arg2,arg3);
    }

    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1BeginTransaction(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    result = BEID_BeginTransaction();
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1EndTransaction(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    result = BEID_EndTransaction();
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1SelectApplication(JNIEnv *jenv, jclass jcls, jbyteArray jarg1) {
    jlong jresult = 0 ;
    BEID_Status result;
    BEID_Bytes arg1 = {0};
    
    (void)jenv;
    (void)jcls;
    arg1.data = (BYTE *)jenv->GetByteArrayElements(jarg1, 0);
    arg1.length = jenv->GetArrayLength(jarg1);

    result = BEID_SelectApplication(&arg1);
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg1.data) jenv->ReleaseByteArrayElements(jarg1, (jbyte *)arg1.data, 0); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1VerifyPIN(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2, jlong jarg3) {
    jlong jresult = 0 ;
    BEID_Pin *arg1 = (BEID_Pin *) 0;
    char *arg2 ;
    BEID_Long *arg3 = (BEID_Long *) 0;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        arg2 = 0;
        if (jarg2) 
        {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return 0;
        }
    }
    arg3 = *(BEID_Long **)&jarg3;
    BEID_assert(0, arg3, BEID_TITLE)
    {
        result = BEID_VerifyPIN(arg1,arg2,&(arg3->data));
    }
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1ChangePIN(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2, jstring jarg3, jlong jarg4) {
    jlong jresult = 0 ;
    BEID_Pin *arg1 = (BEID_Pin *) 0;
    char *arg2 ;
    char *arg3 ;
    BEID_Long *arg4 = (BEID_Long *) 0;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1;
    BEID_assert(0, arg1, BEID_TITLE)
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return 0;
        }
    }
    {
        arg3 = 0;
        if (jarg3) {
            arg3 = (char *)jenv->GetStringUTFChars(jarg3, 0);
            if (!arg3) return 0;
        }
    }
    arg4 = *(BEID_Long **)&jarg4;
    BEID_assert(0, arg4, BEID_TITLE)
    {
        result = BEID_ChangePIN(arg1,arg2,arg3,&(arg4->data));
    }
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
    {
        if (arg3) jenv->ReleaseStringUTFChars(jarg3, arg3); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1GetPINStatus(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2, jint jarg3, jlong jarg4) {
    jlong jresult = 0 ;
    BEID_Pin *arg1 = (BEID_Pin *) 0;
    BEID_Long *arg2 = (BEID_Long *) 0;
    BOOL arg3 ;
    BEID_Bytes *arg4 = (BEID_Bytes *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(BEID_Pin **)&jarg1;
    arg2 = *(BEID_Long **)&jarg2; 
    arg3 = (BOOL)jarg3; 
    arg4 = *(BEID_Bytes **)&jarg4; 
    BEID_assert(0, arg1, BEID_TITLE)
    {
        BEID_assert(0, arg2, BEID_TITLE)
        {
            result = BEID_GetPINStatus(arg1,&(arg2->data),arg3,arg4);
        }
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1ReadFile(JNIEnv *jenv, jclass jcls, jbyteArray jarg1, jlong jarg2, jlong jarg3) {
    jlong jresult = 0 ;
    BEID_Bytes arg1 = {0};
    BEID_Bytes *arg2 = (BEID_Bytes *) 0 ;
    BEID_Pin *arg3 = (BEID_Pin *) 0;
    BEID_Status result;
    
    
    (void)jenv;
    (void)jcls;
    arg1.data = (BYTE *)jenv->GetByteArrayElements(jarg1, 0);
    arg1.length = jenv->GetArrayLength(jarg1);
    arg2 = *(BEID_Bytes **)&jarg2; 
    arg3 = *(BEID_Pin **)&jarg3;
    BEID_assert(0, arg2, BEID_TITLE)
    {
        BEID_assert(0, arg3, BEID_TITLE)
        {
            result = BEID_ReadFile(&arg1,arg2,arg3);
        }
    }

    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg1.data) jenv->ReleaseByteArrayElements(jarg1, (jbyte *)arg1.data, 0); 
    }

    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1WriteFile(JNIEnv *jenv, jclass jcls, jbyteArray jarg1, jbyteArray jarg2, jlong jarg3) {
    jlong jresult = 0 ;
    BEID_Bytes arg1 = {0};
    BEID_Bytes arg2 = {0};
    BEID_Pin *arg3 = (BEID_Pin *) 0;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1.data = (BYTE *)jenv->GetByteArrayElements(jarg1, 0);
    arg1.length = jenv->GetArrayLength(jarg1);
    arg2.data = (BYTE *)jenv->GetByteArrayElements(jarg2, 0);
    arg2.length = jenv->GetArrayLength(jarg2);
    arg3 = *(BEID_Pin **)&jarg3;
    BEID_assert(0, arg3, BEID_TITLE)
    {
        result = BEID_WriteFile(&arg1,&arg2,arg3);
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg1.data) jenv->ReleaseByteArrayElements(jarg1, (jbyte *)arg1.data, 0); 
        if (arg2.data) jenv->ReleaseByteArrayElements(jarg2, (jbyte *)arg2.data, 0); 
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1FlushCache(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    result = BEID_FlushCache();
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jlong JNICALL Java_be_belgium_eid_eidlibJNI_BEID_1SendAPDU(JNIEnv *jenv, jclass jcls, jbyteArray jarg1, jlong jarg2, jlong jarg3) {
    jlong jresult = 0 ;
    BEID_Bytes arg1 = {0};
    BEID_Pin *arg2 = (BEID_Pin *) 0;
    BEID_Bytes *arg3 = (BEID_Bytes *) 0 ;
    BEID_Status result;
    
    (void)jenv;
    (void)jcls;
    arg1.data = (BYTE *)jenv->GetByteArrayElements(jarg1, 0);
    arg1.length = jenv->GetArrayLength(jarg1);
    arg2 = *(BEID_Pin **)&jarg2;
    arg3 = *(BEID_Bytes **)&jarg3; 
    BEID_assert(0, arg3, BEID_TITLE)
    {
        result = BEID_SendAPDU(&arg1,arg2,arg3);
    }
    
    {
        BEID_Status * resultptr = (BEID_Status *) malloc(sizeof(BEID_Status));
        memmove(resultptr, &result, sizeof(BEID_Status));
        *(BEID_Status **)&jresult = resultptr;
    }
    {
        if (arg1.data) jenv->ReleaseByteArrayElements(jarg1, (jbyte *)arg1.data, 0); 
    }
    return jresult;
}


#ifdef __cplusplus
}
#endif

