#ifndef __IOMP_KQUEUE_H
#define __IOMP_KQUEUE_H

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
#include <map> 

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

const unsigned int EPOLL_SIZE = 200;

class Client;
class IOMP_KQUEUE
{
  private:
    int   m_iHandle; 
    //int   m_iTimeout;
    unsigned long   m_iTimeout;

    struct kevent m_stEvent[EPOLL_SIZE];
    //struct  epoll_event m_stEvent[EPOLL_SIZE];

  public:
    IOMP_KQUEUE();
    IOMP_KQUEUE(const unsigned long _iSec);
    ~IOMP_KQUEUE();

    /**
     * Add socket to fd_set
     */
    //const int   AddClient(Client* const _pClient, const unsigned int _uiEvents);
    const int   AddClient(Client* const _pClient, const short _filter, const unsigned short _usFlags);

    /**
     * Modify socket to fd_set
     */
    const int   ModifyFd(Client* const _pClient,  const unsigned int _uiEvents);

    /**
     * Delete socket to fd_set
     */
    const int   DelFd(const int _iFd, const short _filter);
    const int   DelClient(Client * const _pClient, const short _filter);

    /**
     * Function polling
     */
    const int Polling();
    void    SetTimeout(const unsigned long _iSec);

    struct    kevent* const GetEventStructure();
};

#endif

