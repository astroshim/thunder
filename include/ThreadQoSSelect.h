#ifndef __THREADQOSSELECT_H
#define __THREADQOSSELECT_H

#include "./ThreadQoS.h"
#include <list>

class DownloadServer;
class Client;
class IOMP_Select;

class ThreadQoSSelect: public ThreadQoS
{
  private :
    //DownloadServer *m_pMainProcess;
    IOMP_Select *m_pIOMP;

    std::list <Client*> m_lstClient;
    pthread_mutex_t     m_lockClient;

  public:
    ThreadQoSSelect();
    ThreadQoSSelect(DownloadServer* const _pMainProcess);
    ~ThreadQoSSelect();

    //const int AddQoS(Client* const _pClient);
    const int AddQoS(Client* const _pClient, const unsigned int _uiEvents);
    const int RemoveQoS(Client* const _pClient);
    void Run();
};

#endif

