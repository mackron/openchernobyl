// Copyright (C) 2016 David Reid. See included LICENSE file.

// Multithreading is implemented differently depending on the platform. Of note is the entry point which is slightly different. This means
// every thread entry point needs a simple #ifdef check to use the correct signature. Use DRED_THREAD_PROC_SIGNATURE to help with this.

#ifdef OC_WIN32
typedef DWORD (WINAPI * ocThreadEntryProc)(void* pData);
typedef HANDLE ocThread;
typedef HANDLE ocMutex;
typedef HANDLE ocSemaphore;

#define DRED_THREAD_PROC_SIGNATURE(name, data) DWORD WINAPI name(void* data)
#else
typedef void* (* ocThreadEntryProc)(void* pData);
typedef pthread_t ocThread;
typedef pthread_mutex_t ocMutex;
typedef sem_t ocSemaphore;

#define DRED_THREAD_PROC_SIGNATURE(name, data) void* name(void* data)
#endif


//// Thread ////

// Starts a new thread.
//
// There is no delete function. Threads should be terminated naturally.
bool ocThreadCreate(ocThread* pThread, ocThreadEntryProc entryProc, void* pData);

// Waits for a thread to return.
void ocThreadWait(ocThread* pThread);


//// Mutex ////

// Creates a mutex.
bool ocMutexCreate(ocMutex* pMutex);

// Deletes a mutex.
void ocMutexDelete(ocMutex* pMutex);

// Locks a mutex.
void ocMutexLock(ocMutex* pMutex);

// Unlocks a mutex.
void ocMutexUnlock(ocMutex* pMutex);


//// Semaphore ///

// Creates a semaphore.
bool ocSemaphoreCreate(ocSemaphore* pSemaphore, int initialValue);

// Delets a semaphore.
void ocSemaphoreDelete(ocSemaphore* pSemaphore);

// Waits on the given semaphore object and decrements it's counter by one upon returning.
bool ocSemaphoreWait(ocSemaphore* pSemaphore);

// Releases the given semaphore and increments it's counter by one upon returning.
bool ocSemaphoreRelease(ocSemaphore* pSemaphore);