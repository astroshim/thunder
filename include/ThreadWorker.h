#ifndef __THREADWORKER_H
#define __THREADWORKER_H

#include "./Thread.h"

class DownloadManager;
class ThreadWorker: public Thread
{
  private :
    DownloadManager *m_pDNMgr;

  public:
    ThreadWorker();
    ThreadWorker(DownloadManager* const _pDNMgr);
    ~ThreadWorker();

    void Run();

};

#endif

