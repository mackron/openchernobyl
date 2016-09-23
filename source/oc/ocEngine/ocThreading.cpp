// Copyright (C) 2016 David Reid. See included LICENSE file.

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
bool ocThreadCreate__Win32(ocThread* pThread, ocThreadEntryProc entryProc, void* pData)
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



bool ocMutexCreate__Win32(ocMutex* pMutex)
{
    *pMutex = CreateEventA(NULL, FALSE, TRUE, NULL);
    if (*pMutex == NULL) {
        return false;
    }

    return true;
}

void ocMutexDelete__Win32(ocMutex* pMutex)
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



bool ocSemaphoreCreate__Win32(ocSemaphore* pSemaphore, int initialValue)
{
    *pSemaphore = CreateSemaphoreA(NULL, initialValue, LONG_MAX, NULL);
    if (*pSemaphore == NULL) {
        return false;
    }

    return true;
}

void ocSemaphoreDelete__Win32(ocSemaphore* pSemaphore)
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
bool ocThreadCreate__Posix(ocThread* pThread, ocThreadEntryProc entryProc, void* pData)
{
    return pthread_create(pThread, NULL, entryProc, pData) == 0;
}

void ocThreadWait__Posix(ocThread* pThread)
{
    pthread_join(*pThread, NULL);
}



bool ocMutexCreate__Posix(ocMutex* pMutex)
{
    return pthread_mutex_init(pMutex, NULL) == 0;
}

void ocMutexDelete__Posix(ocMutex* pMutex)
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



bool ocSemaphoreCreate__Posix(ocSemaphore* pSemaphore, int initialValue)
{
    return sem_init(pSemaphore, 0, (unsigned int)initialValue) != -1;
}

void ocSemaphoreDelete__Posix(ocSemaphore* pSemaphore)
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

bool ocThreadCreate(ocThread* pThread, ocThreadEntryProc entryProc, void* pData)
{
    if (pThread == NULL || entryProc == NULL) {
        return false;
    }

#ifdef OC_THREADING_WIN32
    return ocThreadCreate__Win32(pThread, entryProc, pData);
#endif

#ifdef OC_THREADING_POSIX
    return ocThreadCreate__Posix(pThread, entryProc, pData);
#endif
}

void ocThreadWait(ocThread* pThread)
{
    if (pThread == NULL) {
        return;
    }

#ifdef OC_THREADING_WIN32
    ocThreadWait__Win32(pThread);
#endif

#ifdef OC_THREADING_POSIX
    ocThreadWait__Posix(pThread);
#endif
}


//// Mutex ////

bool ocMutexCreate(ocMutex* pMutex)
{
    if (pMutex == NULL) {
        return false;
    }

#ifdef OC_THREADING_WIN32
    return ocMutexCreate__Win32(pMutex);
#endif

#ifdef OC_THREADING_POSIX
    return ocMutexCreate__Posix(pMutex);
#endif
}

void ocMutexDelete(ocMutex* pMutex)
{
    if (pMutex == NULL) {
        return;
    }

#ifdef OC_THREADING_WIN32
    ocMutexDelete__Win32(pMutex);
#endif

#ifdef OC_THREADING_POSIX
    ocMutexDelete__Posix(pMutex);
#endif
}

void ocMutexLock(ocMutex* pMutex)
{
    if (pMutex == NULL) {
        return;
    }

#ifdef OC_THREADING_WIN32
    ocMutexLock__Win32(pMutex);
#endif

#ifdef OC_THREADING_POSIX
    ocMutexLock__Posix(pMutex);
#endif
}

void ocMutexUnlock(ocMutex* pMutex)
{
    if (pMutex == NULL) {
        return;
    }

#ifdef OC_THREADING_WIN32
    ocMutexUnlock__Win32(pMutex);
#endif

#ifdef OC_THREADING_POSIX
    ocMutexUnlock__Posix(pMutex);
#endif
}


//// Semaphore ///

bool ocSemaphoreCreate(ocSemaphore* pSemaphore, int initialValue)
{
    if (pSemaphore == NULL) {
        return false;
    }

#ifdef OC_THREADING_WIN32
    return ocSemaphoreCreate__Win32(pSemaphore, initialValue);
#endif

#ifdef OC_THREADING_POSIX
    return ocSemaphoreCreate__Posix(pSemaphore, initialValue);
#endif
}

void ocSemaphoreDelete(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) {
        return;
    }

#ifdef OC_THREADING_WIN32
    ocSemaphoreDelete__Win32(pSemaphore);
#endif

#ifdef OC_THREADING_POSIX
    ocSemaphoreDelete__Posix(pSemaphore);
#endif
}

bool ocSemaphoreWait(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) {
        return false;
    }

#ifdef OC_THREADING_WIN32
    return ocSemaphoreWait__Win32(pSemaphore);
#endif

#ifdef OC_THREADING_POSIX
    return ocSemaphoreWait__Posix(pSemaphore);
#endif
}

bool ocSemaphoreRelease(ocSemaphore* pSemaphore)
{
    if (pSemaphore == NULL) {
        return false;
    }

#ifdef OC_THREADING_WIN32
    return ocSemaphoreRelease__Win32(pSemaphore);
#endif

#ifdef OC_THREADING_POSIX
    return ocSemaphoreRelease__Posix(pSemaphore);
#endif
}
