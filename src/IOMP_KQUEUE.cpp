#include "../include/IOMP_KQUEUE.h"
#include "../include/EventQueue.h"
#include "../include/Socket.h"

#include "../include/NPUtil.h"
#include "../include/NPLog.h"
#include "../include/Client.h"

IOMP_KQUEUE::IOMP_KQUEUE()
          :m_iHandle(-1),
          m_iTimeout(0)
{
  if((m_iHandle = kqueue()) < 0)
  {
    CNPLog::GetInstance().Log("kqueue error"); 
  }

/*
  if((m_iHandle = epoll_create(EPOLL_SIZE)) < 0) 
    CNPLog::GetInstance().Log("epoll_create error"); 
*/

}

IOMP_KQUEUE::IOMP_KQUEUE(const unsigned long _iSec)
          :m_iHandle(-1),
          m_iTimeout(_iSec)
{
  if((m_iHandle = kqueue()) < 0) 
  {
    CNPLog::GetInstance().Log("kqueue error"); 
  }

/*
  if((m_iHandle = epoll_create(EPOLL_SIZE)) < 0) 
  {
    CNPLog::GetInstance().Log("epoll_create error"); 
  }
*/
}

IOMP_KQUEUE::~IOMP_KQUEUE()
{

}

//const int IOMP_KQUEUE::AddClient(Client* const _pClient, const unsigned int _uiEvents)
const int IOMP_KQUEUE::AddClient(Client* const _pClient, const short _filter, const unsigned short _usFlags)
{
  struct kevent ee;
  //EV_SET(&ee, ((Socket *)(_pClient->GetSocket()))->GetFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);

/*
CNPLog::GetInstance().Log("IOMP_KQUEUE::AddClient client=(%p) m_iHandle=(%d) fd=(%d)\n", 
      _pClient, m_iHandle, _pClient->GetSocket()->GetFd()); 
*/

  EV_SET(&ee, ((Socket *)(_pClient->GetSocket()))->GetFd(), _filter, _usFlags, 0, 0, _pClient);
  if(kevent(m_iHandle, &ee, 1, 0, 0, NULL) < -1)
  {
    CNPLog::GetInstance().Log("IOMP_KQUEUE::AddClient Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }

/*
  struct kevent ee;

  ee.ident = static_cast<Socket *>(_pClient->GetSocket())->GetFd(); // set socket
  ee.filter = EVFILT_READ;
  ee.flags = EV_ADD;
  ee.fflags = 0;
  ee.data = 0
  ee.udata = pClient;
*/

/*
  struct epoll_event ee;
  
  ee.events = _uiEvents;
  ee.data.fd  = ((Socket *)(_pClient->GetSocket()))->GetFd(); // set socket
  ee.data.ptr = _pClient;       // set clientclass
  
  if(epoll_ctl(m_iHandle, EPOLL_CTL_ADD, ((Socket *)(_pClient->GetSocket()))->GetFd(), &ee) < 0) 
  {
    CNPLog::GetInstance().Log("IOMP_KQUEUE::AddClient Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }
*/

  return 0;
}

const int IOMP_KQUEUE::ModifyFd(Client* const _pClient, const unsigned int _uiEvents)
{
/*
  struct epoll_event ee;
  
  ee.events = _uiEvents;
  ee.data.fd  = ((Socket *)(_pClient->GetSocket()))->GetFd(); 
  ee.data.ptr = _pClient;             
  
  // modify epoll 
  if(epoll_ctl(m_iHandle, EPOLL_CTL_MOD, ((Socket *)(_pClient->GetSocket()))->GetFd(), &ee) < 0) 
  {
    CNPLog::GetInstance().Log("IOMP_KQUEUE::ModifyFd Error!! \n"); 
    return -1;
  }
*/

  return 0;
}

//const int IOMP_KQUEUE::DelFd(const int _iFd)
const int IOMP_KQUEUE::DelFd(const int _iFd, const short _filter)
{
  struct kevent ee;
  //EV_SET(&ee, _iFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&ee, _iFd, _filter, EV_DELETE, 0, 0, NULL);
  if(kevent(m_iHandle, &ee, 1, 0, 0, NULL) < -1)
  {
    CNPLog::GetInstance().Log("IOMP_KQUEUE::DelFd Error!! (%d) (%s)\n", _iFd, strerror(errno)); 
    return -1;
  }
/*
  if(epoll_ctl(m_iHandle, EPOLL_CTL_DEL, _iFd, NULL) < 0)
  {
    //CNPLog::GetInstance().Log("IOMP_KQUEUE::DelFd Error!! \n"); 
    return -1;
  }
*/
  return 0;
}

const int IOMP_KQUEUE::DelClient(Client * const _pClient, const short _filter)
{
  struct kevent ee;
  //EV_SET(&ee, ((Socket *)(_pClient->GetSocket()))->GetFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&ee, _pClient->GetSocket()->GetFd(), _filter, EV_DELETE, 0, 0, NULL);
  if(kevent(m_iHandle, &ee, 1, 0, 0, NULL) < -1)
  {
    CNPLog::GetInstance().Log("IOMP_KQUEUE::DelFd Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }

/*
  if(epoll_ctl(m_iHandle, EPOLL_CTL_DEL, ((Socket *)(_pClient->GetSocket()))->GetFd(), m_stEvent) < 0)
  {
//    CNPLog::GetInstance().Log("IOMP_KQUEUE::DelClient Error!! (%p) (%s)\n", _pClient, strerror(errno)); 
    return -1;
  }
*/
  return 0;
}

void IOMP_KQUEUE::SetTimeout(const unsigned long _iSec)
{
  m_iTimeout = _iSec;
}

struct  kevent* const IOMP_KQUEUE::GetEventStructure()
{
  return m_stEvent;
}

const int IOMP_KQUEUE::Polling()
{
    //return epoll_wait(m_iHandle, m_stEvent, EPOLL_SIZE, m_iTimeout);
  if ( m_iTimeout == -1 )
  {
    return kevent( m_iHandle, 0, 0, m_stEvent, EPOLL_SIZE, NULL );
  }
  else
  {
    struct timespec delay;           /* used for wasting time */
    delay.tv_sec = 0;
    delay.tv_nsec = m_iTimeout;

    //struct timespec poll_tv = Time::getMSFromNullTimespec( timeout_ms );
    return kevent( m_iHandle, 0, 0, m_stEvent, EPOLL_SIZE, &delay );
  }


}
