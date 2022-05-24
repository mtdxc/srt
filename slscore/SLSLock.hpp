
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Edward.Wu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _SLSLock_INCLUDE_
#define _SLSLock_INCLUDE_

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <stdarg.h>
#include <stdio.h>


/**
 * CSLSMutex
 */

class CSLSMutex
{
public:
    CSLSMutex()
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_mutex);
#else
        pthread_mutex_init(&m_mutex, NULL);
#endif
    }
    ~CSLSMutex()
    {
#ifdef _WIN32
        DeleteCriticalSection(&m_mutex);
#else
        pthread_mutex_destroy(&m_mutex);
#endif
    }
    void lock() {
#ifdef _WIN32
        EnterCriticalSection(&m_mutex);
#else
        pthread_mutex_lock(&m_mutex);
#endif
    }
    bool tryLock() {
#ifdef _WIN32
        return TryEnterCriticalSection(&m_mutex) != FALSE;
#else
        if (pthread_mutex_trylock(&m_mutex) != 0)
            return false;
        return true;
#endif
    }
    void unlock() {
#ifdef _WIN32
        LeaveCriticalSection(&m_mutex);
#else
        pthread_mutex_unlock(&m_mutex);
#endif
    }
private:
#ifdef _WIN32
    mutable CRITICAL_SECTION m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif
};

class CSLSRWLock
{
public:
    CSLSRWLock()
    {
        m_inited = false;
#ifdef _WIN32
        ::InitializeSRWLock(&m_rwlock);
        m_inited = true;
#else
        int ret = pthread_rwlock_init(&m_rwlock, NULL);
        if (0 == ret) {
            m_inited = true;
        }
        else {
            printf("pthread_rwlock_init faild, ret=%d.", ret);;
    }
#endif
}
    ~CSLSRWLock()
    {
        if (m_inited) {
#ifdef _WIN32
            ::ReleaseSRWLockExclusive(&m_rwlock);
#else
            int ret = pthread_rwlock_destroy(&m_rwlock);
            if (0 != ret)
            {
                printf("pthread_rwlock_destroy faild, ret=%d.", ret);;
        }
#endif
    }
    }

    int try_lock_write()
    {
#ifdef _WIN32
        return TryAcquireSRWLockExclusive(&m_rwlock);
#else
        return pthread_rwlock_trywrlock(&m_rwlock);
#endif
    }
    int try_lock_read()
    {
#ifdef _WIN32
        return TryAcquireSRWLockShared(&m_rwlock);
#else
        return pthread_rwlock_tryrdlock(&m_rwlock);
#endif
    }
    int lock_write()
    {
#ifdef _WIN32
        AcquireSRWLockExclusive(&m_rwlock);
        return 0;
#else
        return pthread_rwlock_wrlock(&m_rwlock);
#endif
    }
    int lock_read()
    {
#ifdef _WIN32
        AcquireSRWLockShared(&m_rwlock);
        return 0;
#else
        return pthread_rwlock_rdlock(&m_rwlock);
#endif
    }
    void unlock_read() {
#ifdef _WIN32
        ReleaseSRWLockShared(&m_rwlock);
#else
        pthread_rwlock_unlock(&m_rwlock);
#endif
    }
    void unlock_write() {
#ifdef _WIN32
        ReleaseSRWLockExclusive(&m_rwlock);
#else
        pthread_rwlock_unlock(&m_rwlock);
#endif
    }
private:
#ifdef _WIN32
    SRWLOCK          m_rwlock;
#else
    pthread_rwlock_t m_rwlock;
#endif
    bool             m_inited;
};

/**
 * CSLSLock
 */
class CSLSLock
{
public :
    CSLSLock(CSLSRWLock * clock, int write)
    {
        m_mutex = NULL;
        m_clock = NULL;
        m_locked =false;
        m_write = write;
	    if (NULL == clock)
	        return ;
	    int ret = 0;
	    if (write) {
	        ret = clock->lock_write();
        } else {
            ret = clock->lock_read();
        }
	    if (0 != ret) {
            printf("SLS Error: clock failure, ret=%d.\n", ret);
	    } else {
	        m_clock = clock;
	        m_locked =true;
	    }
    }

    CSLSLock(CSLSMutex * m)
    {
        m_clock = NULL;
        m_mutex = NULL;
        m_locked =false;
        if (m) {
            m_mutex = m;
            m->lock();
            m_locked = true;
        }

    }

    ~CSLSLock()
    {
        if (m_locked && m_mutex)
           m_mutex->unlock();
        if (m_locked && m_clock) {
          m_write ? m_clock->unlock_write() : m_clock->unlock_read();
        }
    }
private:

    CSLSMutex* m_mutex;
    CSLSRWLock* m_clock;
    bool m_write = false;
    bool m_locked;

};





#endif
