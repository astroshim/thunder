#include "../include/IOMP_Select.h"

IOMP_Select::IOMP_Select()
        :m_iMaxFd(1)
        ,m_iSec(0)
        ,m_iMSec(0)
        ,m_stTimeout(NULL)
{
  InitFdSet(&m_Rfds);
  InitFdSet(&m_Wfds);
  InitFdSet(&m_Efds);
  InitFdSet(&m_Allfds);
}

IOMP_Select::IOMP_Select(const int _iSec, const int _iUsec)
        :m_iMaxFd(1)
        ,m_iSec(_iSec)
        ,m_iMSec(_iUsec)
        ,m_stTimeout(NULL)
{
  m_stTimeout = (struct timeval *)malloc(sizeof(struct timeval));
  SetTimeout();

  InitFdSet(&m_Rfds);
  InitFdSet(&m_Wfds);
  InitFdSet(&m_Efds);
  InitFdSet(&m_Allfds);
}

IOMP_Select::~IOMP_Select()
{
  free(m_stTimeout);
}

void IOMP_Select::InitFdSet(fd_set* const _fds)
{
  FD_ZERO(_fds);
}

void IOMP_Select::AddFd(const int _iFd)
{
  FD_SET(_iFd, (fd_set *)&m_Rfds);
  m_iMaxFd = MAX(m_iMaxFd, _iFd+1);
}

void IOMP_Select::AddWriteFd(const int _iFd)
{
  FD_SET(_iFd, (fd_set *)&m_Wfds);
  m_iMaxFd = MAX(m_iMaxFd, _iFd+1);
}

void IOMP_Select::DelFd(const int _iFd)
{
  FD_CLR(_iFd, &m_Rfds);
}

void IOMP_Select::DelWriteFd(const int _iFd)
{
  FD_CLR(_iFd, &m_Wfds);
}

void IOMP_Select::SetTimeout()
{
  if(m_stTimeout != NULL)
  {
    m_stTimeout->tv_sec = m_iSec;
    m_stTimeout->tv_usec = m_iMSec;
  }
}

const int IOMP_Select::Polling()
{
  SetTimeout();

  memcpy(&m_Allfds, &m_Rfds, sizeof(fd_set));
  return select(m_iMaxFd, (fd_set *)&m_Allfds, (fd_set *)0, (fd_set *)0,
              (struct timeval *)m_stTimeout);
}

const int IOMP_Select::WritePolling()
{
  SetTimeout();
  memcpy(&m_Allfds, &m_Wfds, sizeof(fd_set));
  return select(m_iMaxFd, (fd_set *)0, (fd_set *)&m_Allfds, (fd_set *)0,
              (struct timeval *)m_stTimeout);
}

const int IOMP_Select::CheckEvent(const int _iFd)
{
  return FD_ISSET(_iFd, &m_Allfds);
}

