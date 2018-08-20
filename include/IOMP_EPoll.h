#ifndef __IOMP_EPoll_H
#define __IOMP_EPoll_H

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
#include <stropts.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/epoll.h>
#include <map>

const unsigned int EPOLL_SIZE = 200;

class Client;
class IOMP_EPoll
{
  private:
    int   m_iEpollHandle; // epoll

    int   m_iTimeout;

    struct  epoll_event m_stEvent[EPOLL_SIZE];

  public:
    IOMP_EPoll();
    IOMP_EPoll(const int _iSec);
    ~IOMP_EPoll();

    /**
     * Add socket to fd_set
     */
    const int   AddClient(Client* const _pClient, const unsigned int _uiEvents);

    /**
     * Modify socket to fd_set
     */
    const int   ModifyFd(Client* const _pClient,  const unsigned int _uiEvents);

    /**
     * Delete socket to fd_set
     */
    const int   DelFd(const int _iFd);
    const int   DelClient(Client * const _pClient);

    /**
     * Function polling
     */
    const int Polling();
    void    SetTimeout(const int _iSec);

    struct    epoll_event* const GetEventStructure();
};

#endif


