#ifndef __THREADTIC_H
#define __THREADTIC_H

#include "./Thread.h"

class Process;
class ThreadTic: public Thread
{
  private :
    Process   *m_pMainProcess;

    /*
       int PostData();
       void SendStorageInfo(void);
     */

  public:
    ThreadTic(Process* const _pMainProcess);
    ~ThreadTic();

    void Run();
};

#endif


