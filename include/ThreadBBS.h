#ifndef __THREADBBS_H
#define __THREADBBS_H

#include "./Thread.h"

class DownloadManager;
class ThreadBBS: public Thread
{
  private :
    DownloadManager *m_pDNMgr;

  public:
    ThreadBBS();
    ThreadBBS(DownloadManager* const _pDNMgr);
    ~ThreadBBS();

    void Run();

};

#endif

