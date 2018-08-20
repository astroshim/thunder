#include "../include/Thread.h"

void* HandlerRunner(void *_pVoid);

Thread::Thread()
      :m_bIsStarted(false)
      ,m_ThreadID(0)
{
  //printf("Thread Spawn--->> (%d) \n", m_ThreadID);
}

Thread::~Thread()
{
}

const bool Thread::GetStarted()
{
  return m_bIsStarted;
}

void Thread::SetStarted(const bool _bStart)
{
  m_bIsStarted = _bStart;
}

const int Thread::GetThreadID()
{
  return *(int*)m_ThreadID;
}

const bool Thread::Spawn(void* const _pThread)
{
  if(pthread_create(&m_ThreadID, NULL, HandlerRunner, (void *)_pThread) != 0)
  {
    printf("Thread Spawn Error! \n");
    return false;
  }

  return true;
}

void* HandlerRunner(void *_pVoid)
{
  Thread *pThread = (Thread *)_pVoid;

  pThread->SetStarted(true);
  pThread->Run();

  return _pVoid;
}


