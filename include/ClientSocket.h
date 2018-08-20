#ifndef __CLIENTSOCKET_H
#define __CLIENTSOCKET_H

#include "./TcpSocket.h"

class ClientSocket : public TcpSocket
{
  public :
    ClientSocket();
    ClientSocket(const int _iFd);
    virtual ~ClientSocket();

    const int Connect(const char* const _pchAddr, const int iPort);
    const int NonBlockConnect(const char* const _pchAddr, const int iPort);
};

#endif

