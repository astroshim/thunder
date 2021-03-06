#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/ClientSocket.h"

#include "../include/NPLog.h"
#include "../include/NPDebug.h"

ServerSocket::ServerSocket(const ENUM_SERVERSOCKET_TYPE _type)
        :m_Type(_type)
{
}

ServerSocket::~ServerSocket() 
{
}

const ENUM_SERVERSOCKET_TYPE ServerSocket::GetType() 
{
  return m_Type;
}

const int ServerSocket::Bind(const int _iPort)
{
  struct sockaddr_in *stAddr;

  stAddr = GetServerAddr();
  stAddr->sin_family = AF_INET;
  stAddr->sin_port = htons(_iPort);
  stAddr->sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(GetFd(), (struct sockaddr *)stAddr, sizeof(struct sockaddr_in)) < 0)
  {
    //perror("Fail Bind");
    return -1;
  }
  return 0;
}

const int ServerSocket::Listen(const int _iQLen)
{
  if(listen(GetFd(), _iQLen) < 0)
  {
    //perror("Fail Listen");
    return -1;
  }

  return 0;
}

Socket* const ServerSocket::Accept()
{
  int iClientSocket;
  struct sockaddr_in stClientAddr;
  size_t iAddrLength;

  //Socket *pClientSocket;
  ClientSocket *pClientSocket;
/*
  if(!IsTcp())
  {
    return NULL;
  }
*/
  iAddrLength = sizeof(struct sockaddr_in);

  iClientSocket = accept(GetFd(), (struct sockaddr *)&stClientAddr, (socklen_t *)&iAddrLength);
  if(iClientSocket < 0)
  {
        CNPLog::GetInstance().Log("1.accept() fail error=[%d],(%s) ", errno, strerror(errno));
    return NULL;
  }

    /**
    *   ClientSocket Class create
    */
    pClientSocket = new ClientSocket(iClientSocket);
  if(m_iFd < 0 || pClientSocket == NULL)
  {
        CNPLog::GetInstance().Log("oh my god (%d), error=[%d],(%s) ", m_iFd, errno, strerror(errno));
    delete pClientSocket;
    return NULL;
  }
    pClientSocket->SetClientAddr(&stClientAddr);

  return pClientSocket;
}

const int ServerSocket::AcceptFD()
{
  int iClientSocket;
  struct sockaddr_in *pstClientAddr;
  //struct sockaddr_in stClientAddr;
  size_t iAddrLength;

  pstClientAddr = GetClientAddr();
  iAddrLength = sizeof(struct sockaddr_in);

  iClientSocket = accept(GetFd(), (struct sockaddr *)pstClientAddr, (socklen_t *)&iAddrLength);
  //iClientSocket = accept(GetFd(), (struct sockaddr *)&m_ClientAddr, (size_t *)&iAddrLength);
  if(iClientSocket < 0)
  {
        //CNPLog::GetInstance().Log("2.accept() fail error=[%d],(%s) ", errno, strerror(errno));
    return -1;
  }

  char pchIP[16];
  memset(pchIP, 0x00, sizeof(pchIP));
  strncpy(pchIP, inet_ntoa(pstClientAddr->sin_addr), 16);
  if(strcmp( "116.124.252.236", pchIP) == 0 ||
    strcmp("116.124.252.237", pchIP) == 0 ||
    strcmp("116.124.252.238", pchIP) == 0 ||
    strcmp("116.124.252.239", pchIP) == 0 ||
    strcmp("116.124.252.240", pchIP) == 0 ||
    strcmp("116.124.252.241", pchIP) == 0 ||
    strcmp("116.124.252.40", pchIP) == 0 ||
    strcmp("116.124.252.41", pchIP) == 0 ||
    strcmp("116.124.253.251", pchIP) == 0 ||
    strcmp("116.124.253.252", pchIP) == 0 ||
    strcmp("118.216.107.53", pchIP) == 0 ||
    strcmp("118.216.107.54", pchIP) == 0 ||
    strcmp("211.37.6.121", pchIP) == 0 ||
    strcmp("211.37.6.122", pchIP) == 0 )
  {
      CNPLog::GetInstance().Log("Accept FDS IP=[%s], FD=(%d)", pchIP, iClientSocket);
  }
  else
  {
      CNPLog::GetInstance().Log("Accept Client IP=[%s], FD=(%d)", inet_ntoa(pstClientAddr->sin_addr), iClientSocket);
  }

    SetClientAddr(pstClientAddr);
//    pClientSocket->SetClientAddr(&stClientAddr);
//    pClientSocket->SetClientAddr(pstClientAddr);
  return iClientSocket;
}

