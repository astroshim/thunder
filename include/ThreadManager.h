#ifndef  __THREADMANAGER_H
#define  __THREADMANAGER_H

#include <list>
#include <pthread.h>

//#include "./Thread.h"

class Thread;

class ThreadManager : public NonCopyable
{
  public:
    static  ThreadManager*  GetInstance();
    static  void      ReleaseInstance();

    bool Spawn(Thread *pThread);

    /*
     * 등록된 thread중 작업 완료된 thread를 삭제해주는 루틴이 들어가야 한다.
     **/

  private:
    ThreadManager();
    virtual ~ThreadManager();

    static  ThreadManager*    m_SelfInstance;
    std::list < Thread* > m_lstThreadHandle;

};
#endif
