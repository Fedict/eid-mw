#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "common.h"
#include <eid-viewer/oslayer.h>
#include <pthread.h>

static void(*nsd)(const EID_CHAR*, const EID_CHAR*) = NULL;
static void(*nst)(enum eid_vwr_states new_state) = NULL;
static int have_found_cardnumber = 0;
static int result = TEST_RV_FAIL;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

static void newstringdata(const EID_CHAR* label, const EID_CHAR* data);
static void newstate(enum eid_vwr_states new_state);

TEST_FUNC(deserialize) {
	struct eid_vwr_ui_callbacks* cb;

	if(have_robot()) {
		robot_remove_card();
		sleep(1);
	}
	cb = createcbs();
	verbose_assert(cb != NULL);
	nsd = cb->newstringdata;
	cb->newstringdata = newstringdata;
	nst = cb->newstate;
	cb->newstate = newstate;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cnd, &mut);
	pthread_mutex_unlock(&mut);
	return result;
}
static void newstringdata(const EID_CHAR* label, const EID_CHAR* data) {
	nsd(label, data);
	if(!strcmp(label, "card_number")) {
		verbose_assert(strcmp(data, "592000155079") == 0);
		have_found_cardnumber = 1;
	}
}

static void newstate(enum eid_vwr_states new_state) {
	static enum eid_vwr_states prev_state = STATE_LIBOPEN;
	if(nst != NULL) nst(new_state);
	pthread_mutex_lock(&mut);
	switch(new_state) {
		case STATE_NO_TOKEN:
			if(prev_state == STATE_FILE) {
				pthread_cond_signal(&cnd);
				break;
			}
			if(!have_robot()) {
				CK_SLOT_ID sl;
				CK_RV rv = find_slot(CK_TRUE, &sl);
				if(rv != TEST_RV_SKIP) {
					result = TEST_RV_SKIP;
					pthread_cond_signal(&cnd);
					break;
				}
			}
			printf("opening " SRCDIR "/67.06.30-296.59.eid\n");
			eid_vwr_be_deserialize(SRCDIR "/67.06.30-296.59.eid");
			break;
		case STATE_FILE_WAIT:
			verbose_assert(cursrc == EID_VWR_SRC_FILE);
			verbose_assert(have_found_cardnumber == 1);
			result = TEST_RV_OK;
			pthread_cond_signal(&cnd);
			break;
		case STATE_TOKEN:
			result = TEST_RV_FAIL;
			pthread_cond_signal(&cnd);
			break;
		default:
			break;
	}
	prev_state = new_state;
	pthread_mutex_unlock(&mut);
}

