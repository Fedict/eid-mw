#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include "p11.h"
#else
#include <pthread.h>
#endif
#include <string.h>

#include "backend.h"
#include "state.h"

#ifdef WIN32
HANDLE mutex = NULL;
HANDLE cond = NULL;

#define LOCK_MUTEX(mutex) WaitForSingleObject( mutex, INFINITE )
#define UNLOCK_MUTEX(mutex) ReleaseMutex(mutex)

#define SET_SIGNAL(cond) SetEvent(cond);
//no need to be atomic, the signalled state remains untill auto released by some thread waiting for it
#define WAIT_SIGNAL(cond, mutex) \
	UNLOCK_MUTEX(mutex); \
	WaitForSingleObject( cond, INFINITE ); \
	LOCK_MUTEX(mutex);

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


/* Main loop for state machine thread */
#ifdef WIN32
DWORD WINAPI thread_main(void* val) {
	INT error;

	if( WaitForSingleObject( mutex, INFINITE ) == WAIT_FAILED)
	{
		error = GetLastError();
		be_log(EID_VWR_LOG_COARSE, TEXT("WAIT_FAILED with error: %.8x"), error);
	}

#else
static void* thread_main(void* val EIDV_UNUSED) {
	int rv;
	if((rv = pthread_mutex_lock(&mutex)) != 0) {
		be_log(EID_VWR_LOG_COARSE, TEXT("Could not lock mutex: %s"), strerror(rv));
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

#ifdef WIN32
			//the EVENT_DEVICE_CHANGED is not a state changing event
			//it can be several combinations of READER_FOUND, CARD_INSERTED, CARD_REMOVED, READER_ATTACHED, READER_REMOVED events
			//in order to find out which of the STATE changing events should be triggered, some pkcs11 calls need to be made, 
			//and we want all (especially getslotlist in v2.20) pkcs11 calls (besides waiting for device changes) to be done on this thread
			if (tmp->e == EVENT_DEVICE_CHANGED)
			{
				eid_vwr_p11_check_reader_list(tmp->data);
			}
			else
			{
				sm_handle_event_onthread(tmp->e, tmp->data);
			}
#else
			sm_handle_event_onthread(tmp->e, tmp->data);
#endif
			if (tmp->done != NULL) {
				tmp->done(tmp->data);
			}

			LOCK_MUTEX(mutex);
			if (tmp->free != NULL) {
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

/* Add an event to the list of events to be processed */
static void add_item(struct list* item) {
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

/* Stop the thread */
void sm_stop_thread() {
	struct list *item = (struct list *)calloc(sizeof(struct list), 1);

	item->exiting = 1;

	add_item(item);
}

/* Start the thread */
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

/* Handle an event. Caller: backend or UI */
void sm_handle_event(enum eid_vwr_state_event e, void* data, void(*freefunc)(void*), void(*donefunc)(void*)) {
	struct list *item = (struct list *)calloc(sizeof(struct list), 1);

	item->e = e;
	item->data = data;
	item->free = freefunc;
	item->done = donefunc;

	add_item(item);
}
