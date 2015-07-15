#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <pthread.h>
#endif
#include <string.h>

#include "backend.h"
#include "state.h"

#ifdef WIN32
HANDLE mutex = NULL;
HANDLE cond = NULL;
HANDLE lpHandles[2];

#define LOCK_MUTEX(mutex) WaitForSingleObject( mutex, INFINITE )
#define UNLOCK_MUTEX(mutex) ReleaseMutex(mutex)

#define SET_SIGNAL(cond) SetEvent(cond);
#define WAIT_SIGNAL(cond, mutex) \
	lpHandles[0] = cond; \
	lpHandles[1] = mutex; \
	WaitForMultipleObjects(2, lpHandles, TRUE, INFINITE);

#else
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_MUTEX(mutex) pthread_mutex_lock(&mutex)
#define UNLOCK_MUTEX(mutex) pthread_mutex_unlock(&mutex)

#define SET_SIGNAL(cond) pthread_cond_signal(&cond)
#define WAIT_SIGNAL(cond, mutex) pthread_cond_wait(&cond, &mutex)
#endif


struct list {
	struct list *next;
	int exiting;
	enum eid_vwr_state_event e;
	void* data;
	void (*free)(void*);
	void (*done)(void*);
} *cmdlist = NULL;


#ifdef WIN32
DWORD WINAPI thread_main(void* val) {
	INT error;

	if( WaitForSingleObject( mutex, INFINITE ) == WAIT_FAILED)
	{
		error = GetLastError();
		be_log(EID_VWR_LOG_COARSE, "WAIT_FAILED with error: %.8x", error);
	}

#else
static void* thread_main(void* val) {
	int rv;
	if((rv = pthread_mutex_lock(&mutex)) != 0) {
		be_log(EID_VWR_LOG_COARSE, "Could not lock mutex: %s", strerror(rv));
	}
#endif

	for(;;) {
		while(cmdlist != NULL) {
			struct list *tmp = cmdlist;

			if(cmdlist->exiting != 0) {
				goto exit;
			}
			cmdlist = cmdlist->next;
			// Avoid deadlocks. Event-handling code could be doing
			// anything, including dealing with (other?) mutexes.


			UNLOCK_MUTEX(mutex);
			sm_handle_event_onthread(tmp->e, tmp->data);
			if(tmp->done != NULL) {
				tmp->done(tmp->data);
			}
			LOCK_MUTEX(mutex);
			if(tmp->free != NULL) {
				tmp->free(tmp->data);
			}
			free(tmp);
		}

		WAIT_SIGNAL(cond, mutex);

	}
exit:
	UNLOCK_MUTEX(mutex);
#ifdef WIN32
	return 0;
#else
	return NULL;
#endif
}

void add_item(struct list* item) {
	struct list **ptr;
	LOCK_MUTEX(mutex);

	ptr = &cmdlist;
	while(*ptr != NULL) {
		ptr = &((*ptr)->next);
	}
	*ptr = item;

	SET_SIGNAL(cond);
	UNLOCK_MUTEX(mutex);
}


void sm_stop_thread() {
	struct list *item = (struct list *)calloc(sizeof(struct list), 1);

	item->exiting = 1;

	add_item(item);
}

void sm_start_thread() {
#ifdef WIN32
	HANDLE thread = NULL;
	mutex = CreateMutex (NULL, FALSE, NULL);
	cond = CreateEvent (NULL, FALSE, FALSE, NULL);
	thread = CreateThread(NULL, 0, thread_main, NULL, 0, NULL);
#else
	pthread_t thread;
	pthread_create(&thread, NULL, thread_main, NULL);
#endif
}

void sm_handle_event(enum eid_vwr_state_event e, void* data, void(*freefunc)(void*), void(*donefunc)(void*)) {
	struct list *item = (struct list *)calloc(sizeof(struct list), 1);

	item->e = e;
	item->data = data;
	item->free = freefunc;

	add_item(item);
}
