#ifndef __SERVERSOCKET_H
#define __SERVERSOCKET_H

#include "./Include.h"
//#include "./TcpSocket.h"

class TcpSocket;
class ServerSocket : public TcpSocket
{
  private:
    ENUM_SERVERSOCKET_TYPE m_Type;

  public:
    ServerSocket(const ENUM_SERVERSOCKET_TYPE _type);
    virtual ~ServerSocket();

    const int Bind(const int _iPort);
    const int Listen(const int _iQLen);
    Socket* const Accept();
    const int AcceptFD();

    const ENUM_SERVERSOCKET_TYPE GetType();
};
#endif

