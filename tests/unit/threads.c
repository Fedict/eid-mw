#include <unix.h>
#include <pkcs11.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "testlib.h"

static int create_count;
static int destroy_count;
static int lock_count;
static int unlock_count;
static CK_RV threaded_test_result;
static pthread_mutex_t condmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

CK_RV create_mutex(CK_VOID_PTR_PTR mutex) {
	pthread_mutex_t* mut = calloc(sizeof(pthread_mutex_t), 1);
	if(pthread_mutex_init(mut, NULL) < 0) {
		free(mut);
		switch(errno) {
			case ENOMEM:
				return CKR_HOST_MEMORY;
			default:
				return CKR_GENERAL_ERROR;
		}
	}

	*mutex = mut;
	create_count++;
	return CKR_OK;
}

CK_RV destroy_mutex(CK_VOID_PTR mutex) {
	pthread_mutex_t* mut = mutex;

	if(pthread_mutex_destroy(mut) < 0) {
		switch(errno) {
			case EBUSY:
			case EINVAL:
				return CKR_MUTEX_BAD;
			default:
				return CKR_GENERAL_ERROR;
		}
	}

	free(mutex);
	destroy_count++;
	return CKR_OK;
}

CK_RV lock_mutex(CK_VOID_PTR mutex) {
	pthread_mutex_t* mut = mutex;

	if(pthread_mutex_lock(mut) < 0) {
		switch(errno) {
			case EINVAL:
			case EAGAIN:
			case EDEADLK:
			case EPERM:
				return CKR_MUTEX_BAD;
			default:
				return CKR_GENERAL_ERROR;
		}
	}

	lock_count++;
	return CKR_OK;
}

CK_RV unlock_mutex(CK_VOID_PTR mutex) {
	pthread_mutex_t* mut = mutex;

	unlock_count++;
	if(pthread_mutex_unlock(mut) < 0) {
		switch(errno) {
			case EPERM:
				return CKR_MUTEX_NOT_LOCKED;
			case EINVAL:
			case EAGAIN:
			case EDEADLK:
				return CKR_MUTEX_BAD;
			default:
				return CKR_GENERAL_ERROR;
		}
	}

	return CKR_OK;
}

int threaded_test() {
	CK_SESSION_HANDLE h;
	CK_SLOT_ID slot;
	CK_RV ret;

	printf("In thread:\n");

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return (int) ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, NULL_PTR, &h));

	pthread_mutex_lock(&condmutex);

	printf("signaling main thread...\n");
	pthread_cond_signal(&cond);
	printf("waiting for main thread now...\n");
	pthread_cond_wait(&cond, &condmutex);
	printf("main thread done, continuing\n");
	check_rv(C_CloseSession(h));

	return CKR_OK;
}

void* thread_func(void* v EIDT_UNUSED) {
	threaded_test_result = threaded_test();

	pthread_mutex_unlock(&condmutex);

	return NULL;
}

TEST_FUNC(threads) {
	CK_C_INITIALIZE_ARGS args_os = {
		.flags = CKF_OS_LOCKING_OK,
	};
	CK_C_INITIALIZE_ARGS args_man = {
		.CreateMutex = create_mutex,
		.DestroyMutex = destroy_mutex,
		.LockMutex = lock_mutex,
		.UnlockMutex = unlock_mutex,
	};
	pthread_t thread;
	CK_SESSION_HANDLE handle;
	CK_RV ret;
	CK_SLOT_ID slot;
	CK_SESSION_INFO slinfo;
	CK_TOKEN_INFO tkinfo;

	check_rv(C_Initialize(&args_os));
	check_rv(C_Finalize(NULL_PTR));

	check_rv(C_Initialize(&args_man));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return (int)ret;
	}

	pthread_mutex_lock(&condmutex);
#undef CHECK_RV_DEALLOC
#define CHECK_RV_DEALLOC pthread_mutex_unlock(&condmutex)

	printf("Spawning a thread:\n");
	pthread_create(&thread, NULL, thread_func, NULL);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &handle));

	printf("Thread running, waiting for it to do stuff now:\n");
	pthread_cond_wait(&cond, &condmutex);
	printf("Thread signaled us, doing something...\n");
	check_rv(C_GetTokenInfo(slot, &tkinfo))
	verbose_assert(tkinfo.ulSessionCount == 2);
	printf("RW session count: %lu\n", tkinfo.ulRwSessionCount);
	printf("Signaling thread again\n");
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&condmutex);
#undef CHECK_RV_DEALLOC
#define CHECK_RV_DEALLOC
	pthread_join(thread, NULL);
	printf("Thread finished\n");

	check_rv(C_GetSessionInfo(handle, &slinfo));
	verbose_assert((slinfo.flags & CKF_RW_SESSION) == 0);

	if(threaded_test_result != TEST_RV_OK) {
		return (int)threaded_test_result;
	}

	check_rv(C_Finalize(NULL_PTR));

	printf("created: %d, destroyed: %d, locked: %d, unlocked: %d\n", create_count, destroy_count, lock_count, unlock_count);
	verbose_assert(create_count != 0 && destroy_count != 0 && lock_count != 0 && unlock_count != 0);
	verbose_assert(create_count == destroy_count);
	verbose_assert(lock_count == unlock_count);
	return TEST_RV_OK;
}
