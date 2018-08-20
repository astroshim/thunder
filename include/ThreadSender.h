#ifndef __THREADSENDER_H
#define __THREADSENDER_H

#include "./Thread.h"

class DownloadServer;

class ThreadSender: public Thread
{
  private :
    DownloadServer *m_pDownloadServer;

  public:
    ThreadSender();
    ThreadSender(DownloadServer* const _pDownloadServer);
    ~ThreadSender();

    void Run();

};

#endif

