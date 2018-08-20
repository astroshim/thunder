#ifndef __UNIXSOCKET_H
#define __UNIXSOCKET_H

//#include "./Socket.h"
class Socket;

class UnixSocket : public Socket
{
  private:
    struct sockaddr_un  m_SockAddr;

  public :
    UnixSocket();
    UnixSocket(const char* const _pchPath);
    ~UnixSocket();

    const int DelSocket();
    const int Bind();
    const int BindClient(char *_pchPath);

    const int     SendMsg(char* const _pchPassingData, const int _iDataLen);
    const int     RecvMsg(char* const _pchPassingData, const int _iDataLen);

    /**
     *   Socket Passing
     */
    const int     SendFD(const int _iSendFd, char* const _pchPassingData, const int _iDataLen);
    const Socket* const RecvFD(char* const _pchPassedData, const int _iDataLen);
};

#endif

