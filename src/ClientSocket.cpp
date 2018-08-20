#include "../include/ClientSocket.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"

ClientSocket::ClientSocket()
{}

ClientSocket::ClientSocket(const int _iFd):TcpSocket(_iFd)
{
}

ClientSocket::~ClientSocket() 
{
}

const int ClientSocket::Connect(const char* const _pchAddr, const int _iPort)
{
  struct sockaddr_in *stServerAddr;

  stServerAddr = GetServerAddr();
  stServerAddr->sin_family = AF_INET;

  if(!_pchAddr)
  {
    stServerAddr->sin_addr.s_addr = inet_addr(INADDR_ANY);
  }
  else
  {
    stServerAddr->sin_addr.s_addr = inet_addr(_pchAddr);
  } 
  
  stServerAddr->sin_port = htons((u_short)_iPort);
  if( (connect(m_iFd, (struct sockaddr*)stServerAddr, sizeof(struct sockaddr_in))) < 0)
  {
    CNPLog::GetInstance().Log("ClientSocket::Connect Fail Connect errno=(%d)(%s)", errno, strerror(errno));
    return -1;
  }
  
  return 0;
}

const int ClientSocket::NonBlockConnect(const char* const _pchAddr, const int _iPort)
{
  struct sockaddr_in *stServerAddr;
  int iRet;

  stServerAddr = GetServerAddr();
  stServerAddr->sin_family = AF_INET;

  if(!_pchAddr)
  {
    stServerAddr->sin_addr.s_addr = inet_addr(INADDR_ANY);
  }
  else
  {
    stServerAddr->sin_addr.s_addr = inet_addr(_pchAddr);
  } 

  if(SetNonBlock() < 0)
  {
    return -1;
  }
  stServerAddr->sin_port = htons((u_short)_iPort);
  if( (iRet = connect(m_iFd, (struct sockaddr*)stServerAddr, sizeof(struct sockaddr_in))) < 0)
  {

    if(errno == EINPROGRESS)
    {
      CNPLog::GetInstance().Log("ClientSocket::Connect EINPROGRESS");
      do
      {
        struct timeval tval;
        fd_set   rset, wset;

        FD_ZERO(&rset);
        FD_SET(m_iFd, &rset);
        wset = rset;
        tval.tv_sec = 10;
        tval.tv_usec = 0;

        iRet = select(m_iFd+1, &rset, &wset, NULL, &tval );
        if(iRet < 0 && errno != EINTR)
        {
          CNPLog::GetInstance().Log("nonblock connect error errno=(%d)(%s)", errno, strerror(errno));
          return -1;
        }
        else
        if(iRet > 0)
        {
          if (FD_ISSET(m_iFd, &rset) || FD_ISSET(m_iFd, &wset)) 
          {

            if(GetReturnCode() < 0)
            {
              CNPLog::GetInstance().Log("nonblock socket SO_ERROR errno=(%d)(%s)", errno,
                  strerror(errno));
              return -1;   
            }
          }
          else
          {
            CNPLog::GetInstance().Log("NonBlock Connect select error errno=(%d)(%s)", errno,
              strerror(errno));
            return -1;
          }

          break;
        }
        else
        {
          CNPLog::GetInstance().Log("nonblock connect timeout errno=(%d)", errno);
          return -1;
        }
      }while(1);
    }
    else
    {
      CNPLog::GetInstance().Log("ClientSocket::Connect Fail Connect errno ====>>>> ret=(%d),(%d)",iRet, errno);
    }

  }
  SetBlock();
  
  return 0;
}



