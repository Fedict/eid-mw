#include <backend.h>
#include <verify_cert.h>

#include <libxml/encoding.h>
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include <soap.h>

#include <string.h>

#include <pthread.h>
pthread_once_t once = PTHREAD_ONCE_INIT;

enum eid_vwr_result eid_vwr_verify_cert(void* certificate, size_t certlen) {
	return EID_VWR_FAILED;
}
