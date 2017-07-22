// Copyright (C) 2017 David Reid. See included LICENSE file.

#ifdef OC_WIN32
#define OC_THREADING_WIN32
#else
#define OC_THREADING_POSIX
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Win32
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_THREADING_WIN32
bool ocThreadCreate__Win32(ocThreadEntryProc entryProc, void* pData, ocThread* pThread)
{
    *pThread = CreateThread(NULL, 0, entryProc, pData, 0, NULL);
    if (*pThread == NULL) {
        return false;
    }

    return true;
}

void ocThreadWait__Win32(ocThread* pThread)
{
    WaitForSingleObject(*pThread, INFINITE);
}


bool ocMutexInit__Win32(ocMutex* pMutex)
{
    *pMutex = CreateEventA(NULL, FALSE, TRUE, NULL);
    if (*pMutex == NULL) {
        return false;
    }

    return true;
}

void ocMutexUninit__Win32(ocMutex* pMutex)
{
    CloseHandle(*pMutex);
}

void ocMutexLock__Win32(ocMutex* pMutex)
{
    WaitForSingleObject(*pMutex, INFINITE);
}

void ocMutexUnlock__Win32(ocMutex* pMutex)
{
    SetEvent(*pMutex);
}


bool ocSemaphoreInit__Win32(int initialValue, ocSemaphore* pSemaphore)
{
    *pSemaphore = CreateSemaphoreA(NULL, initialValue, LONG_MAX, NULL);
    if (*pSemaphore == NULL) {
        return false;
    }

    return true;
}

void ocSemaphoreUninit__Win32(ocSemaphore* pSemaphore)
{
    CloseHandle(*pSemaphore);
}

bool ocSemaphoreWait__Win32(ocSemaphore* pSemaphore)
{
    return WaitForSingleObject(*pSemaphore, INFINITE) == WAIT_OBJECT_0;
}

bool ocSemaphoreRelease__Win32(ocSemaphore* pSemaphore)
{
    return ReleaseSemaphore(*pSemaphore, 1, NULL) != 0;
}
#endif  // Win32


///////////////////////////////////////////////////////////////////////////////
//
// Posix
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_THREADING_POSIX
bool ocThreadCreate__Posix(ocThreadEntryProc entryProc, void* pData, ocThread* pThread)
{
    return pthread_create(pThread, NULL, entryProc, pData) == 0;
}

void ocThreadWait__Posix(ocThread* pThread)
{
    pthread_join(*pThread, NULL);
}


bool ocMutexInit__Posix(ocMutex* pMutex)
{
    return pthread_mutex_init(pMutex, NULL) == 0;
}

void ocMutexUninit__Posix(ocMutex* pMutex)
{
    pthread_mutex_destroy(pMutex);
}

void ocMutexLock__Posix(ocMutex* pMutex)
{
    pthread_mutex_lock(pMutex);
}

void ocMutexUnlock__Posix(ocMutex* pMutex)
{
    pthread_mutex_unlock(pMutex);
}


bool ocSemaphoreInit__Posix(int initialValue, ocSemaphore* pSemaphore)
{
    return sem_init(pSemaphore, 0, (unsigned int)initialValue) != -1;
}

void ocSemaphoreUninit__Posix(ocSemaphore* pSemaphore)
{
    sem_close(pSemaphore);
}

bool ocSemaphoreWait__Posix(ocSemaphore* pSemaphore)
{
    return sem_wait(pSemaphore) != -1;
}

bool ocSemaphoreRelease__Posix(ocSemaphore* pSemaphore)
{
    return sem_post(pSemaphore) != -1;
}
#endif  // Posix




//// Thread ////

bool ocThreadCreate(ocThreadEntryProc entryProc, void* pData, ocThread* pThread)
{
    if (pThread == NULL) return false;
    ocZeroObject(pThread);

    if (entryProc == NULL) return false;

#ifdef OC_THREADING_WIN32
    return ocThreadCreate__Win32(entryProc, pData, pThread);
#endif
#ifdef OC_THREADING_POSIX
    return ocThreadCreate__Posix(entryProc, pData, pThread);
#endif
}

void ocThreadWait(ocThread* pThread)
{
    if (pThread == NULL) return;

#ifdef OC_THREADING_WIN32
    ocThreadWait__Win32(pThread);
#endif
#ifdef OC_THREADING_POSIX
    ocThreadWait__Posix(pThread);
#endif
}


//// Mutex ////

bool ocMutexInit(ocMutex* pMutex)
{
    if (pMutex == NULL) return false;

#ifdef OC_THREADING_WIN32
    return ocMutexInit__Win32(pMutex);
#endif
#ifdef OC_THREADING_POSIX
    return ocMutexInit__Posix(pMutex);
#endif
}

void ocMutexUninit(ocMutex* pMutex)
{
    if (pMutex == NULL) return;

#ifdef OC_THREADING_WIN32
    ocMutexUninit__Win32(pMutex);
#endif
#ifdef OC_THREADING_POSIX
    ocMutexUninit__Posix(pMutex);
#endif
}

void ocMutexLock(ocMutex* pMutex)
{
    if (pMutex == NULL) return;

#ifdef OC_THREADING_WIN32
    ocMutexLock__Win32(pMutex);
#endif
#ifdef OC_THREADING_POSIX
    ocMutexLock__Posix(pMutex);
#endif
}

void ocMutexUnlock(ocMutex* pMutex)
{
    if (pMutex == NULL) return;

#ifdef OC_THREADING_WIN32
    ocMutexUnlock__Win32(pMutex);
#endif
#ifdef OC_THREADING_POSIX
    ocMutexUnlock__Posix(pMutex);
#endif
}


//// Semaphore ///

bool ocSemaphoreInit(int initialValue, ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) return false;

#ifdef OC_THREADING_WIN32
    return ocSemaphoreInit__Win32(initialValue, pSemaphore);
#endif
#ifdef OC_THREADING_POSIX
    return ocSemaphoreInit__Posix(initialValue, pSemaphore);
#endif
}

void ocSemaphoreUninit(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) return;

#ifdef OC_THREADING_WIN32
    ocSemaphoreUninit__Win32(pSemaphore);
#endif
#ifdef OC_THREADING_POSIX
    ocSemaphoreUninit__Posix(pSemaphore);
#endif
}

bool ocSemaphoreWait(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) return false;

#ifdef OC_THREADING_WIN32
    return ocSemaphoreWait__Win32(pSemaphore);
#endif
#ifdef OC_THREADING_POSIX
    return ocSemaphoreWait__Posix(pSemaphore);
#endif
}

bool ocSemaphoreRelease(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) return false;

#ifdef OC_THREADING_WIN32
    return ocSemaphoreRelease__Win32(pSemaphore);
#endif
#ifdef OC_THREADING_POSIX
    return ocSemaphoreRelease__Posix(pSemaphore);
#endif
}
