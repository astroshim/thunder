#include "../include/IOMP_EPoll.h"
#include "../include/EventQueue.h"
#include "../include/Socket.h"

#include "../include/NPUtil.h"
#include "../include/NPLog.h"
#include "../include/Client.h"

IOMP_EPoll::IOMP_EPoll()
          :m_iEpollHandle(-1),
          m_iTimeout(0)
{
  if((m_iEpollHandle = epoll_create(EPOLL_SIZE)) < 0) 
    CNPLog::GetInstance().Log("epoll_create error"); 

}

IOMP_EPoll::IOMP_EPoll(const int _iSec)
          :m_iEpollHandle(-1),
          m_iTimeout(_iSec)
{
  if((m_iEpollHandle = epoll_create(EPOLL_SIZE)) < 0) 
  {
    CNPLog::GetInstance().Log("epoll_create error"); 
  }
}

IOMP_EPoll::~IOMP_EPoll()
{
}

const int IOMP_EPoll::AddClient(Client* const _pClient, const unsigned int _uiEvents)
{
  struct epoll_event ee;
  memset(&ee, 0, sizeof(struct epoll_event));
  
  ee.events = _uiEvents;
  ee.data.fd  = ((Socket *)(_pClient->GetSocket()))->GetFd(); // set socket
  ee.data.ptr = _pClient;       // set clientclass
  
  if(epoll_ctl(m_iEpollHandle, EPOLL_CTL_ADD, ((Socket *)(_pClient->GetSocket()))->GetFd(), &ee) < 0) 
  {
    CNPLog::GetInstance().Log("IOMP_EPoll::AddClient Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }

  return 0;
}

const int IOMP_EPoll::ModifyFd(Client* const _pClient, const unsigned int _uiEvents)
{
  struct epoll_event ee;
  memset(&ee, 0, sizeof(struct epoll_event));
  
  ee.events = _uiEvents;
  ee.data.fd  = ((Socket *)(_pClient->GetSocket()))->GetFd(); 
  ee.data.ptr = _pClient;             
  
  /* modify epoll */
  if(epoll_ctl(m_iEpollHandle, EPOLL_CTL_MOD, ((Socket *)(_pClient->GetSocket()))->GetFd(), &ee) < 0) 
  {
    CNPLog::GetInstance().Log("IOMP_EPoll::ModifyFd Error!! \n"); 
    return -1;
  }

  return 0;
}

const int IOMP_EPoll::DelFd(const int _iFd)
{
  if(epoll_ctl(m_iEpollHandle, EPOLL_CTL_DEL, _iFd, NULL) < 0)
  {
    //CNPLog::GetInstance().Log("IOMP_EPoll::DelFd Error!! \n"); 
    return -1;
  }
  return 0;
}

const int IOMP_EPoll::DelClient(Client * const _pClient)
{
  //if(epoll_ctl(m_iEpollHandle, EPOLL_CTL_DEL, ((Socket *)(_pClient->GetSocket()))->GetFd(), NULL) < 0)
  // 090217 update
  if(epoll_ctl(m_iEpollHandle, EPOLL_CTL_DEL, ((Socket *)(_pClient->GetSocket()))->GetFd(), m_stEvent) < 0)
  {
//    CNPLog::GetInstance().Log("IOMP_EPoll::DelClient Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }

  return 0;
}

void IOMP_EPoll::SetTimeout(const int _iSec)
{
  m_iTimeout = _iSec;
}

struct  epoll_event* const IOMP_EPoll::GetEventStructure()
{
  return m_stEvent;
}

const int IOMP_EPoll::Polling()
{
    return epoll_wait(m_iEpollHandle, m_stEvent, EPOLL_SIZE, m_iTimeout);
}
