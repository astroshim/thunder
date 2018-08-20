#ifndef __THREADCLOSER_H
#define __THREADCLOSER_H

#include "./Thread.h"

class DownloadServer;

class ThreadCloser: public Thread
{
  private :
    DownloadServer *m_pDownloadServer;

  public:
    ThreadCloser();
    ThreadCloser(DownloadServer* const _pDownloadServer);
    ~ThreadCloser();

    void Run();
};

#endif

