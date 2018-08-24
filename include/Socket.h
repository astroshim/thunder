#ifndef __SOCKET_H
#define __SOCKET_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
//#include <stropts.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <signal.h>

#include <iostream>
using namespace std;

const int USER_CLOSE        = -100;

class Socket
{
  protected:
    int   m_iFd;

  public:
    Socket();
    Socket(const int  _iFd);
    virtual ~Socket();

    const int   GetFd();
    void SetFd(const int _iFd);
    const int   Close();

    /**
     * Data Send and Recv
     */
    const int   Read(void* const _vPtr, const size_t _n);
    const int   ReadLine(void* const _vPtr, const size_t _n);
    const int   Write(const void* const _vPtr, const size_t _n);
    const int   Send(const char* const _pchBuf,  const int _iSize);
    const int   Recv(char* const _pchBuf, const int _iSize);

    /*  use udp & tcp */
    const int   SendTo(const void* const _vPtr, const size_t _iLen, const struct sockaddr *_to, const socklen_t _toLen);
    const int   RecvFrom(void* const _vPtr, const size_t _iLen, struct sockaddr* const _from, socklen_t* const _fromLen);

    const int   Available();

    const int   GetSockName(char* const _pchIpAddr);
    const int   GetPeerName(char* const _pchPeerName);

    const int   GetReturnCode();

    /**
     * Socket Option
     */
    const int   GetRcvBufSize();
    const int   GetSndBufSize();

    const int   SetSndBufSize(const int _iBufSize);
    const int   SetRcvBufSize(const int _iBufSize);

    const int   SetTcpCORK(int _iOptVal);
    const int   SetNonBlock();
    const int   SetBlock();
    const int   SetKeepAlive();
    const int   SetLinger();
    const int   SetReUse();
    const int   SetFreeBind();
    const int   SetNoDelay();

    /**
     * Network Address manage
     */
    const int   GetIpAddr(const struct hostent* const _pHost, char* const _pchDesIp);
    const int   GetHostName(char* const _pchHostName);
    const int   GetHostByName(char* const _pchHostName, char* const _pchStr);
    const int   GetHostByAddr();
    const int   GetIpNodeByName(char* const _pchHostName, const int _iAf);
    const int   GetIpNodeByAddr(char* const _pchAddr, const int _iAf);
};

#endif


