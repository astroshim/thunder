#ifndef __THREADACCEPTOR_H
#define __THREADACCEPTOR_H

#include "./Thread.h"

class DownloadServer;
class ThreadAcceptor: public Thread
{
  private :
    DownloadServer *m_pMainProcess;

  public:
    ThreadAcceptor();
    ThreadAcceptor(DownloadServer* const _pMainProcess);
    ~ThreadAcceptor();

    void Run();
};

#endif

