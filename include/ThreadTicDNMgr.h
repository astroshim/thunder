#ifndef __THREADTIC_H
#define __THREADTIC_H

#include "./Thread.h"

class DownloadManager;
class ThreadTicDNMgr: public Thread
{
  private :
    DownloadManager   *m_pDownloadManager;

    int PostData(char *buf);
    // void SendStorageInfo(void);

  public:
    ThreadTicDNMgr(DownloadManager* const _pDownloadManager);
    ~ThreadTicDNMgr();

    void Run();
};

#endif


