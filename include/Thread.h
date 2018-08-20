#ifndef __THREAD_H
#define __THREAD_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

/**
 * NonCopyable Class
 */
class Thread  : private NonCopyable
{
  public:
    Thread();
    virtual ~Thread();
    virtual void Run()=0;

    const bool  Spawn(void* const pThread);
    const int   GetThreadID();

    const bool  GetStarted();
    void    SetStarted(const bool _bStart);

  protected:

  private:
    bool    m_bIsStarted;
    pthread_t   m_ThreadID;
};

#endif

