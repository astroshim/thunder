#include "../include/Include.h"
#include "../include/Socket.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"
#include "../include/Packet.h"

Socket::Socket()
  :m_iFd(-1)
{
}

Socket::Socket(const int _iFd)
  :m_iFd(_iFd)
{
}

Socket::~Socket()
{
  //CNPLog::GetInstance().Log("Sockt class free");
  close(m_iFd);
  m_iFd = -1;
}

const int Socket::GetFd()
{
  return m_iFd;
}

void Socket::SetFd(const int _iFd)
{
  m_iFd = _iFd;
}

const int Socket::Close()
{
  close(m_iFd);
  m_iFd = -1;
  return 0;
}

/*
   struct sockaddr_in* Socket::GetServerAddr()
   {
   return &m_ServerAddr;
   }

   struct sockaddr_in* Socket::GetClientAddr()
   {
   return &m_ClientAddr;
   }
   */

const int Socket::Read(void* const _vPtr, const size_t _messageLengthInSocket)
{
  size_t    remainMessageLength;
  ssize_t   nRead;
  char    *pchPtr = NULL;

  pchPtr = (char *)_vPtr;
  remainMessageLength = _messageLengthInSocket;

  while(remainMessageLength > 0)
  {
    if( (nRead = read(m_iFd, pchPtr, remainMessageLength)) < 0 )
    {
      if(errno == EINTR)
      {
        nRead = 0;
      }
      else
      {
        return -1;
      }
    }
    else if(nRead == 0)
    {
      break;
    }

    remainMessageLength   -= nRead;
    pchPtr  += nRead;

  }

  // return (_messageLengthInSocket - remainMessageLength);
  return nRead;
}

// const int Socket::ReadLine(void* const _vPtr, const size_t _messageLengthInSocket)
// {
//   size_t    remainMessageLength;
//   ssize_t   nRead;
//   // char    *pchPtr = NULL;

//   // T_PACKET *pMessage = new T_PACKET;
//   // memset((char *)pMessage, 0x00, sizeof(T_PACKET));
//   // pMessage->header.command = cmd_BBS_DS_DOWNFINISH_RES;
//   // pMessage->header.length = sizeof(Tcmd_BBS_DS_DOWNFINISH_RES);

//   char message[1024];
//   memset(message, 0x00, sizeof(message));

//   // pchPtr = (char *)_vPtr;
//   remainMessageLength = _messageLengthInSocket;

//   while(remainMessageLength > 0)
//   {
//     // if( (nRead = read(m_iFd, pchPtr, remainMessageLength)) < 0 )
//     if( (nRead = read(m_iFd, message, remainMessageLength)) < 0 )
//     {
//       CNPLog::GetInstance().Log("######## read failed is %d \n", nRead);
//       if(errno == EINTR)
//       {
//         nRead = 0;
//       }
//       else
//       {
//         return -1;
//       }
//     }
//     else if(nRead == 0)
//     {
//       CNPLog::GetInstance().Log("######## nRead is 0 \n");
//       break;
//     }

//     //else  /* 'numRead' must be 1 if we get here */
//     //{
//     //  if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
//     //    totRead++;
//     //    *buf++ = ch;
//     //  }

//     //  if (ch == '\n')
//     //    break;
//     //}

//     remainMessageLength   -= nRead;
//     // pchPtr  += nRead;

//     // if (*(pchPtr-1) == '\n') {
//     if (message[nRead] == '\n') {
// #ifdef _DEBUG
//       CNPLog::GetInstance().Log("ReadLine Break because linefeed \n");
// #endif

//       memcpy(_vPtr, message, nRead);
//       break;
//     }
//   }

// #ifdef _DEBUG
//   CNPLog::GetInstance().Log("ReadLine messageLengthInSocket: %ld, remain message size in socket: %ld, nRead: %ld \n", _messageLengthInSocket, remainMessageLength, nRead);
// #endif

//   return nRead;
//   // return (_messageLengthInSocket - remainMessageLength);
// }

const int Socket::Write(const void* const _vPtr,  const size_t _n)
{
  size_t    nLeft;
  ssize_t   nWritten;
  const char  *pchPtr;

  pchPtr  = (char *)_vPtr;
  nLeft   = _n;

  while(nLeft > 0)
  {
    if( (nWritten = write(m_iFd, pchPtr, nLeft)) < 0)
    {
      CNPLog::GetInstance().Log("Error Write errno=(%d)(%s)", errno, strerror(errno));

      if(errno == EINTR)
      {
        nWritten = 0;
      }
      else
      {
        return -1;
      }
    }

    nLeft   -= nWritten;
    pchPtr  += nWritten;
  }

  return (_n - nLeft);;
}

const int Socket::Send(const char* const _pchBuf, const int _iSize)
{
  fd_set  set;
  int   iSet, iCnt;
  int   iSendSize;
  struct  timeval timeout;

  if(m_iFd < 0)
  {
    return -1;
  }

  iSet = m_iFd + 1;
  FD_ZERO(&set);
  FD_SET(m_iFd, &set);

  timeout.tv_sec = (long)3;
  timeout.tv_usec = 0;

  if( (iCnt = select(iSet, 0, &set, 0, &timeout)) == 0)
  {
    return -1;
  }

  if(iCnt < 0)
  {
    return -1;
  }

  if(FD_ISSET(m_iFd, &set))
  {
    iSendSize = send(m_iFd, _pchBuf, _iSize, 0);
    CNPLog::GetInstance().Log("Timed_Send : [%d] \n", iSendSize);
    return iSendSize;
  }

  return -1;
}

const int Socket::Recv(char* const _pchBuf, const int _iSize)
{
  fd_set set;
  int   iSet, iCnt;
  int   iRecvSize;
  struct timeval timeout;

  if(m_iFd < 0)
  {
    return -1;
  }

  iSet = m_iFd + 1;
  FD_ZERO(&set);
  FD_SET(m_iFd, &set);

  timeout.tv_sec = (long)15;
  timeout.tv_usec = 0;

  if( (iCnt = select(iSet, &set, 0, 0, &timeout)) == 0)
  {
    return -1;
  }

  if( iCnt < 0)
  {
    return -1;
  }

  if(FD_ISSET(m_iFd, &set))
  {
    iRecvSize = recv(m_iFd, _pchBuf, _iSize, 0);
    CNPLog::GetInstance().Log("In Socket::Recv(%d) ==> [%d]", _iSize, iRecvSize);
    return iRecvSize;
  }

  return -1;
}

const int Socket::SendTo(const void* const _vPtr, const size_t _iLen, const struct sockaddr *_to, const socklen_t _toLen)
{
  int iBytesSent;

  fd_set wfds;
  struct timeval tv;
  int iRetVal = 1;

  while(1)
  {
    do
      iBytesSent = sendto(m_iFd, (char *)_vPtr, _iLen, 0, _to, _toLen);
    while(iBytesSent == -1 && errno == EINTR);

    if(iBytesSent == -1 && errno == EAGAIN)
    {

      FD_ZERO(&wfds);
      FD_SET(m_iFd, &wfds);
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      do
        iRetVal = select(m_iFd+1, NULL, &wfds, NULL, &tv);
      while(iRetVal == -1 && errno == EINTR);

      if(iRetVal != 1)
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  return iBytesSent;
}

const int Socket::RecvFrom(void* const _vPtr, const size_t _iLen, struct sockaddr* const _from, socklen_t* const _fromLen)
{
  int iBytesRecv = 0;

  fd_set rfds;
  struct timeval tv;
  int iRetVal = 1;

  while(1)
  {
    do
      iBytesRecv = recvfrom(m_iFd, (char *)_vPtr, _iLen, 0, _from, _fromLen);
    while(iBytesRecv == -1 && errno == EINTR);

    if(iBytesRecv == -1 && errno == EAGAIN)
    {

      FD_ZERO(&rfds);
      FD_SET(m_iFd, &rfds);
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      do
        iRetVal = select(m_iFd + 1, &rfds, NULL, NULL, &tv);
      while(iRetVal == -1 && errno == EINTR);

      if(iRetVal != 1)
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  return iBytesRecv;
}

const int Socket::Available()
{
  int iRead = 0;

  ioctl(m_iFd, FIONREAD, &iRead);
  return iRead;
}

const int Socket::GetSockName(char* const _pchIpAddr)
{
  struct sockaddr_in sockSt;
  socklen_t iStLen;

  iStLen = sizeof(sockSt);

  if(getsockname(m_iFd, (struct sockaddr *)&sockSt, (socklen_t *)&iStLen) < 0)
  {
    CNPLog::GetInstance().Log("Fail GetSockName errno=(%d)", errno);
    return -1;
  }

  //  memset(_pchIpAddr, 0x00, sizeof(_pchIpAddr));
  memset(_pchIpAddr, 0x00, strlen(inet_ntoa(sockSt.sin_addr))+1);
  strncpy(_pchIpAddr, inet_ntoa(sockSt.sin_addr), MAX_IP_LEN);

  return 0;
}

const int Socket::GetPeerName(char* const _pchPeerName)
{
  unsigned char *pchAddr;
  struct sockaddr_in  sockSt;
  socklen_t iRcvLeng;

  iRcvLeng = sizeof(sockSt);

  if(getpeername(m_iFd, (struct sockaddr *) &sockSt, (socklen_t *)&iRcvLeng) < 0)
  {
    CNPLog::GetInstance().Log("Fail GetPeerName errno=(%d)", errno);
    return -1;
  }

  pchAddr = (unsigned char *)&sockSt.sin_addr;
  sprintf(_pchPeerName, "%d.%d.%d.%d%c",
      pchAddr[0], pchAddr[1], pchAddr[2], pchAddr[3], 0x00);

  return 0;
}

const int Socket::SetNonBlock()
{
  /*
O_NONBLOCK  : POSIX nonblocking I/O
FNONBLOCK   : POSIX nonblocking I/O
*/
  if(fcntl(m_iFd, F_SETFL, O_NDELAY) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetNonBlock errno=(%d)", errno);
    return -1;
  }

  return 0;
}

const int Socket::SetBlock()
{
  if(fcntl(m_iFd, F_SETFL, fcntl(m_iFd, F_GETFL, 0)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetBlock errno=(%d)", errno);
    return -1;
  }

  return 0;
}

// Get the return code from the connect
const int Socket::GetReturnCode()
{
  int   iError;
  socklen_t   iSockLen = 0;

  iSockLen = sizeof(iError);
  if(getsockopt(m_iFd, SOL_SOCKET, SO_ERROR, &iError, (socklen_t *)&iSockLen) < 0)
  {
    CNPLog::GetInstance().Log("Fail GetReturnCode errno=(%d)", errno);
    return -1;
  }

  return iError;
}

const int Socket::GetRcvBufSize()
{
  int   iRcvBufSize;
  socklen_t   iSockOptSize = 0;

  iSockOptSize = sizeof(iRcvBufSize);
  if(getsockopt(m_iFd, SOL_SOCKET, SO_RCVBUF, &iRcvBufSize, (socklen_t *)&iSockOptSize) < 0)
  {
    CNPLog::GetInstance().Log("Fail GetRcvBufSize errno=(%d)", errno);
    return 0;
  }

  return iRcvBufSize;
}

const int Socket::GetSndBufSize()
{
  int   iSndBufSize;
  socklen_t   iSockOptSize = 0;

  iSockOptSize = sizeof(iSndBufSize);
  if(getsockopt(m_iFd, SOL_SOCKET, SO_SNDBUF, &iSndBufSize, (socklen_t *)&iSockOptSize) < 0)
  {
    CNPLog::GetInstance().Log("Fail GetSndBufSize errno=(%d)", errno);
    return 0;
  }

  return iSndBufSize;
}

/**
 * It should be changed before opening the socket.
 */
const int Socket::SetSndBufSize(const int _iBufSize)
{
  if(setsockopt(m_iFd, SOL_SOCKET, SO_SNDBUF, &_iBufSize, sizeof(_iBufSize)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetSndBufSize errno=(%d)", errno);
    return -1;
  }

  return 0;
}

/**
 * It should be changed before opening the socket.
 */
const int Socket::SetRcvBufSize(const int _iBufSize)
{
  if(setsockopt(m_iFd, SOL_SOCKET, SO_RCVBUF, &_iBufSize, sizeof(_iBufSize)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetRcvBufSize errno=(%d)", errno);
    return -1;
  }

  return 0;
}

const int Socket::SetKeepAlive()
{
  int iSoKeepAlive = 1;

  if(setsockopt(m_iFd, SOL_SOCKET, SO_KEEPALIVE, &iSoKeepAlive, sizeof(iSoKeepAlive)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetKeepAlive errno=(%d)", errno);
    return -1;
  }

  return 0;
}

/**
 * do not want time wait.
 */
const int Socket::SetLinger()
{
  struct linger stLinger;

  /**
  */
  stLinger.l_onoff = 1;
  stLinger.l_linger = 0;

  if(setsockopt(m_iFd, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetLinger errno=(%d)", errno);
    return -1;
  }

  return 0;
}

const int Socket::SetReUse()
{
  int nFlag = 1;

  if(setsockopt(m_iFd, SOL_SOCKET, SO_REUSEADDR, (char *)&nFlag, sizeof(nFlag)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetLinger errno=(%d)", errno);
    return -1;
  }

  return 0;
}


/**
 *
 */
const int Socket::SetFreeBind()
{
  int iReUseAddr = 1;

  if( setsockopt(m_iFd, SOL_SOCKET, SO_REUSEADDR, (char *)&iReUseAddr, sizeof(iReUseAddr)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetFreeBind errno=(%d)", errno);
    return -1;
  }

  return 0;
}

const int Socket::SetTcpCORK(int _iOptVal)
{
  //int optval = 1; // 1 is cork, 0 is un cork

#ifdef _FREEBSD
  if(setsockopt(m_iFd, IPPROTO_TCP, TCP_NOPUSH, (char *)&_iOptVal, sizeof(_iOptVal)) < 0)
#else
    if(setsockopt(m_iFd, IPPROTO_TCP, TCP_CORK, (char *)&_iOptVal, sizeof(_iOptVal)) < 0)
#endif
    {
      CNPLog::GetInstance().Log("Fail TCP_CORK errno=(%d)", errno);
      return -1;
    }

  return 0;
}

const int Socket::SetNoDelay()
{
  int iNoDelay = 1;

  if(setsockopt(m_iFd, IPPROTO_TCP, TCP_NODELAY, (char *)&iNoDelay, sizeof(iNoDelay)) < 0)
  {
    CNPLog::GetInstance().Log("Fail SetNoDelay errno=(%d)", errno);
    return -1;
  }

  return 0;
}

const int Socket::GetIpAddr(const struct hostent* const _pHost, char* const _pchDesIp)
{
  struct in_addr sockSt;

  memcpy(&sockSt, _pHost->h_addr_list[0], sizeof(struct in_addr));
  strncpy(_pchDesIp, inet_ntoa(sockSt), MAX_IP_LEN);

  return 0;
}

const int Socket::GetHostName(char* const _pchHostName)
{
  if(gethostname(_pchHostName, 32) < 0)
  {
    return -1;
  }


  return 0;
}

const int Socket::GetHostByName(char* const _pchHostName, char* const _pchStr)
{
  struct hostent * pHost;

  if(!_pchHostName)
  {
    return -1;
  }

  _pchStr[0] = 0;
  pHost = gethostbyname(_pchHostName);

  if (pHost== NULL)
    return -1; // not found

  GetIpAddr(pHost, _pchStr);

  return 0;
}

const int Socket::GetHostByAddr()
{
  /*
     struct hostent *hptmp, *hp;

     if ((hptmp = gethostbyaddr(addr, sizeof(struct in_addr), af)) == NULL)
     return NULL;

     if ((hp =  (struct hostent *) malloc(sizeof(struct hostent))) != NULL)
     memcpy(hp, hptmp, sizeof(struct hostent));


     return hp;
     */
  return 0;
}

// my_getipnodebyname
const int Socket::GetIpNodeByName(char* const _pchHostName,  const int _iAf)
{
  /*
     int err;

     struct hostent *hp = getipnodebyname(_pchHostName, _iAf, AI_DEFAULT, &err);
     */

  return 0;
}

const int Socket::GetIpNodeByAddr(char* const _pchAddr,  const int _iAf)
{
  /*
     int err;

     unsigned int len = (_iAf == AF_INET) ? 4 : 16;
     struct hostent *hp = getipnodebyaddr(_pchAddr, len, _iAf, &err);
     */
  return 0;
}

