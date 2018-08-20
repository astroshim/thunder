#include "../include/Thread.h"
#include "../include/ThreadManager.h"

//#include "../include/EventQueue.h"
#include "../include/NPLog.h"

ThreadManager*  ThreadManager::m_SelfInstance = 0;

ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
  std::list<Thread*>::iterator pos, posPrev;

//printf("ThreadManager destructor\n");

  pos = m_lstThreadHandle.begin();
  while( pos != m_lstThreadHandle.end() )
  {
    posPrev = pos++;
//printf("in thread manager destructor '%p', '%d'\n", *posPrev, ((Thread *)(*posPrev))->GetStarted());
    m_lstThreadHandle.erase( posPrev );
    delete *posPrev;
  }
}

ThreadManager* ThreadManager::GetInstance()
{
  if( m_SelfInstance == 0)
    m_SelfInstance = new ThreadManager();

  return m_SelfInstance;
}

void ThreadManager::ReleaseInstance()
{
  if( m_SelfInstance != 0 )
  {
    delete m_SelfInstance;
    m_SelfInstance = 0;
  }
}

bool ThreadManager::Spawn(Thread *pThread)
{
  pThread->Spawn((void *)pThread);

  m_lstThreadHandle.push_back(pThread);
  return true;
}

