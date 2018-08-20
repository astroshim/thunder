#ifndef __CLIENTUSERMGR_H
#define __CLIENTUSERMGR_H

#include "./Client.h"

class ClientUserMgr: public Client
{
  private:
    int m_iPort;
    char m_pchIpAddr[MAX_IP_LEN];

  public: 
    ClientUserMgr();
    ClientUserMgr(Socket* const _cSocket);
    virtual ~ClientUserMgr();

    //const int ExecuteCommand();
    const int ExecuteCommand(Thread *_pThread);
};

#endif

