// Copyright (C) 2016 David Reid. See included LICENSE file.

// Multithreading is implemented differently depending on the platform. Of note is the entry point which is slightly different. The thread
// entry point needs to be declared like this:
//     ocThreadResult OC_THREADCALL MyThreadEntryProc(void* pData);

#ifdef OC_WIN32
#define OC_THREADCALL WINAPI
typedef DWORD ocThreadResult;
typedef HANDLE ocThread;
typedef HANDLE ocMutex;
typedef HANDLE ocSemaphore;
#else
#define OC_THREADCALL
typedef void* ocThreadResult;
typedef pthread_t ocThread;
typedef pthread_mutex_t ocMutex;
typedef sem_t ocSemaphore;
#endif
typedef ocThreadResult (OC_THREADCALL * ocThreadEntryProc)(void* pData);


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