#ifndef __THREADRECEIVER_H
#define __THREADRECEIVER_H

#include "./Thread.h"

class DownloadServer;
class ThreadReceiver: public Thread
{
  private :
    DownloadServer *m_pDownloadServer;

  public:
    ThreadReceiver();
    ThreadReceiver(DownloadServer* const _pDownloadServer);
    ~ThreadReceiver();

    void Run();
};

#endif

