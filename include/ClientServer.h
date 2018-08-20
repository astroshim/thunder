#ifndef __CLIENTSERVER_H
#define __CLIENTSERVER_H

#include "./Client.h"

class ClientServer: public Client
{
  private:
    int m_iPort;
    char m_pchIpAddr[MAX_IP_LEN];

  public: 
    ClientServer();
    ClientServer(Socket* const _cSocket);
    virtual ~ClientServer();

    const int ExecuteCommand(Thread *_pThread);
};

#endif

