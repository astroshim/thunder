#ifndef __MUTEX_H
#define __MUTEX_H

/*
 * A fast mutex for synchronizing non-recursive methods.
 * Use a Lock to lock and unlock a Mutex.
 * Use a RecursiveMutex if you need to be able to lock a mutex
 * more than once before unlocking it.
 *
 * This class is not reference-counted; each instance
 * is intended to be encapsulated as a member variable
 * by the object that uses it.
 */

#include <pthread.h>

class Lock;

class Mutex 
{
  public:
    Mutex();
    virtual ~Mutex();

    //protected:
    virtual void Lock();
    virtual void UnLock();
    virtual void Wait();

    //private:
    Mutex(const Mutex& src );
    Mutex& operator=(const Mutex& right);
    void Notify();
    void NotifyAll();

  private:
    pthread_mutex_t     mutex;
    pthread_mutexattr_t mutexAttr;
    pthread_cond_t      cond;
    pthread_condattr_t  condAttr;
};

#endif


