#ifndef __THREADQOS_H
#define __THREADQOS_H

#include "./Thread.h"

class DownloadServer;
class Client;
class ThreadQoS: public Thread
{
  private :

  protected:
    DownloadServer *m_pMainProcess;

  public:
    ThreadQoS();
    ThreadQoS(DownloadServer* const _pMainProcess);
    ~ThreadQoS();

    // for select()
    //virtual const int AddQoS(Client* const _pClient);
    // for epoll()
    virtual const int AddQoS(Client* const _pClient, const unsigned int _uiEvents);
    virtual const int RemoveQoS(Client* const _pClient);

    virtual void Run();
};

#endif

