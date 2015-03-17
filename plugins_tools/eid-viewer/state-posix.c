#include <pthread.h>
#include <string.h>

#include "backend.h"
#include "state.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct list {
	struct list *next;
	int exiting;
	enum eid_vwr_state_event e;
	void* data;
	void (*free)(void*);
	void (*done)(void*);
} *cmdlist = NULL;

static void* thread_main(void* val) {
	int rv;
	if((rv = pthread_mutex_lock(&mutex)) != 0) {
		be_log(EID_VWR_LOG_COARSE, "Could not lock mutex: %s", strerror(rv));
	}
	for(;;) {
		while(cmdlist != NULL) {
			struct list *tmp = cmdlist;

			if(cmdlist->exiting != 0) {
				goto exit;
			}
			// Avoid deadlocks. Event-handling code could be doing
			// anything, including dealing with (other?) mutexes.
			pthread_mutex_unlock(&mutex);
			sm_handle_event_onthread(tmp->e, tmp->data);
			if(tmp->done != NULL) {
				tmp->done(tmp->data);
			}
			pthread_mutex_lock(&mutex);
			if(tmp->free != NULL) {
				tmp->free(cmdlist->data);
			}
			cmdlist = cmdlist->next;
			free(tmp);
		}
		pthread_cond_wait(&cond, &mutex);
	}
exit:
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void add_item(struct list* item) {
	struct list **ptr;
	pthread_mutex_lock(&mutex);

	ptr = &cmdlist;
	while(*ptr != NULL) {
		ptr = &((*ptr)->next);
	}
	*ptr = item;

	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}


void sm_stop_thread() {
	struct list *item = calloc(sizeof(struct list), 1);

	item->exiting = 1;

	add_item(item);
}

void sm_start_thread() {
	pthread_t thread;
	pthread_create(&thread, NULL, thread_main, NULL);
}

void sm_handle_event(enum eid_vwr_state_event e, void* data, void(*freefunc)(void*), void(*donefunc)(void*)) {
	struct list *item = calloc(sizeof(struct list), 1);

	item->e = e;
	item->data = data;
	item->free = freefunc;

	add_item(item);
}
