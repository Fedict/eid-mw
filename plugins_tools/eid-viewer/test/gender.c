#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "common.h"
#include <eid-viewer/oslayer.h>
#include <pthread.h>

static void(*nsd)(const EID_CHAR*, const EID_CHAR*) = NULL;
static int have_found_cardnumber = 0;
static enum eid_vwr_states curstate;


static void newstringdata(const EID_CHAR* label, const EID_CHAR* data) {
	nsd(label, data);
	switch(eid_vwr_convert_get_lang()){
	case EID_VWR_LANG_NL:
		if(!strcmp(label, "gender")){
			verbose_assert(strcmp(data, "M") == 0);
		}
		if(!strcmp(label, "date_of_birth")){
			verbose_assert(strcmp(data, "30 JUN  1967") == 0);
		}
		if(!strcmp(label, "document_type")){
			verbose_assert(strcmp(data, "IDENTITEITSKAART") == 0);
		}
		break;
	case EID_VWR_LANG_EN:
		if(!strcmp(label, "gender")){
			verbose_assert(strcmp(data, "M") == 0);
		}
		if(!strcmp(label, "date_of_birth")){
			verbose_assert(strcmp(data, "30 JUN 1967") == 0);
		}
		if(!strcmp(label, "document_type")){
			verbose_assert(strcmp(data, "IDENTITY CARD") == 0);
		}
		break;
	case EID_VWR_LANG_FR:
		if(!strcmp(label, "gender")){
			verbose_assert(strcmp(data, "M") == 0);
		}
		if(!strcmp(label, "date_of_birth")){
			verbose_assert(strcmp(data, "30 JUIN 1967") == 0);
		}
		if(!strcmp(label, "document_type")){
			verbose_assert(strcmp(data, "CARTE D'IDENTITE") == 0);
		}
		break;
	case EID_VWR_LANG_DE:
		if(!strcmp(label, "gender")){
			verbose_assert(strcmp(data, "M") == 0);
		}
		if(!strcmp(label, "date_of_birth")){
			verbose_assert(strcmp(data, "30.JUN.1967") == 0);
		}
		if(!strcmp(label, "document_type")){
			verbose_assert(strcmp(data, "PERSONALAUSWEIS") == 0);
		}
		break;
	case EID_VWR_LANG_NONE:break;
	default:
		exit(TEST_RV_SKIP);
	}		
}

static void newstate(enum eid_vwr_states s) {
	curstate = s;	
}


TEST_FUNC(gender) {
	if(!have_robot()) {
		printf("no robot");
		return TEST_RV_SKIP;
	}
	struct eid_vwr_ui_callbacks* cb;
	robot_insert_reader();
	cb = createcbs();
	cb->newstate = newstate;
	nsd = cb->newstringdata;
	cb->newstringdata = newstringdata;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	
	robot_insert_reader();
	printf("press enter\n");
	#ifdef WIN32
	eid_vwr_be_deserialize( L"../67.06.30-296.59.eid");
	#else 
	eid_vwr_be_deserialize( "./67.06.30-296.60.eid");
	#endif
	
	printf("FR:\n");
	eid_vwr_convert_set_lang(EID_VWR_LANG_FR);
	SLEEP(1);
	
	printf("NL:\n");
	eid_vwr_convert_set_lang(EID_VWR_LANG_NL);

	printf("EN:\n");
	eid_vwr_convert_set_lang(EID_VWR_LANG_EN);

	printf("DE:\n");
	eid_vwr_convert_set_lang(EID_VWR_LANG_DE);
	
	eid_vwr_close_file ();

	return TEST_RV_OK;
}
