#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include "./Socket.h"

//class Socket;
class TcpSocket : public Socket
{
  private:
    struct sockaddr_in  m_ServerAddr;
    struct sockaddr_in  m_ClientAddr;
    //protected:

  public :
    TcpSocket();
    TcpSocket(const int _iFd);  // for ClientSocket
    virtual ~TcpSocket();

    struct sockaddr_in* const GetServerAddr();
    struct sockaddr_in* const GetClientAddr();

    virtual void  SetClientAddr();
    virtual void  SetClientAddr(struct sockaddr_in* const _pstSockAddr);

    const char* const GetClientIpAddr();

    const unsigned int GetClientIp();
    const unsigned int GetServerIp();
    /*
       const char* const GetClientIp();
       const char* const GetServerIp();
     */
};

#endif
