#ifndef __IOMP_SELECT_H
#define __IOMP_SELECT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/socket.h>
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

#include "./IOMP.h"

class IOMP_Select : public IOMP
{
  private:
    int   m_iMaxFd;
    int   m_iSec;
    int   m_iMSec;
    struct timeval *m_stTimeout;  

    fd_set  m_Rfds;
    fd_set  m_Wfds;
    fd_set  m_Efds;
    fd_set  m_Allfds;

    /**
     * Init fd_set
     */
    void  InitFdSet(fd_set* const _fds);

  public:
    IOMP_Select();
    IOMP_Select(const int _iSec, const int _iUsec);
    ~IOMP_Select();

    /**
     * Add socket to fd_set
     */
    void  AddFd(const int _iFd);
    void  AddWriteFd(const int _iFd);

    /**
     * Delete socket to fd_set
     */
    void  DelFd(const int _iFd);
    void  DelWriteFd(const int _iFd);

    /**
     * Function polling
     */
    const int   Polling();
    const int   WritePolling();
    void  SetTimeout();

    /**
     * Function geting polling result
     */
    const int   CheckEvent(const int _iFd);
};

#endif


