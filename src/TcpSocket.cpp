//#include "../include/Socket.h"
#include "../include/TcpSocket.h"
#include "../include/NPLog.h"

TcpSocket::TcpSocket()
{
    bzero((void *)&m_ServerAddr, sizeof(m_ServerAddr));
    bzero((void *)&m_ClientAddr, sizeof(m_ClientAddr));

//  super.m_iFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  m_iFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TcpSocket::TcpSocket(const int _iFd):Socket(_iFd)
{
}

TcpSocket::~TcpSocket() 
{
  //CNPLog::GetInstance().Log("TcpSockt class free");
}

struct sockaddr_in* const TcpSocket::GetServerAddr()
{
    return &m_ServerAddr;
}

struct sockaddr_in* const TcpSocket::GetClientAddr()
{
    return &m_ClientAddr;
}

// to get the connected client ip.
void  TcpSocket::SetClientAddr(struct sockaddr_in* const _pstSockAddr)
{
  memcpy((void *)&m_ClientAddr, _pstSockAddr, sizeof(struct sockaddr_in));
  //CNPLog::GetInstance().Log("IP = (%s), (%s) \n", inet_ntoa(m_ClientAddr.sin_addr), inet_ntoa(_pstSockAddr->sin_addr));
}

void  TcpSocket::SetClientAddr()
{
  int iLen = sizeof(struct sockaddr);
  getpeername(m_iFd, (struct sockaddr *)&m_ClientAddr, (socklen_t *)&iLen);

//CNPLog::GetInstance().Log("SetClientAddr = (%s)", inet_ntoa(m_ClientAddr.sin_addr));
}

const unsigned int TcpSocket::GetClientIp()
{
  //return static_cast<unsigned long>(m_ClientAddr.sin_addr.s_addr);
  return m_ClientAddr.sin_addr.s_addr;
}

const char* const TcpSocket::GetClientIpAddr()
{
  //CNPLog::GetInstance().Log("Get IP = (%s) \n", inet_ntoa(m_ClientAddr.sin_addr));
    return inet_ntoa(m_ClientAddr.sin_addr);
}

const unsigned int TcpSocket::GetServerIp()
{
  return m_ServerAddr.sin_addr.s_addr;
}

