#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>

#include <errno.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/resource.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <semaphore.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <sys/wait.h>
/*
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <netinet/ether.h>
#include <net/if.h>
*/

#include <iostream>
using namespace std;

#include <list>

//#include "./NPDebug.h"
//#include "./NPLog.h"

const char ON   = 1;
const char OFF  = 0;

//const int RECV_NOT_ENOUGH   = 0;  // CircularBuff

//const int RECV_NOT_ENOUGH   = 0;  // CircularBuff에 데이타가 Get할만큼 충분하지 못하다.
const int RECV_NOT_ENOUGH   = -2; // CircularBuff에 데이타가 Get할만큼 충분하지 못하다.
const int RECV_ERROR    = -1; // CircularBuff에 들어가있는 데이타가 이상하다.!!

const unsigned int UNIX_PATH_MAX  = 256;
const unsigned int LOG_FILE_LEN   = 256;
const int MAX_IP_LEN        = 20;
const int MAX_PORT_CNT      = 10;   // 한 서버가 관리할 최대 ServerPort 개수

const int MAX_USER_ID       = 24;
const int MAX_PASSWD        = 24;

const int MAX_PHONE_LEN   = 16;

const int MAX_CLIENT_COUNT  = 200;  // 한 프로세스에서 관리하는 최대 client count

/**
 * *   Server socket type define
 * */
typedef enum
{
    SERVER_PORT = 0,    /* To receive client */
    SERVER_PORT_MGR,
    SERVER_PORT_PC      /* nothing */
}ENUM_SERVERSOCKET_TYPE;

/**
*   Client class Type
*/
typedef enum
{
    CLIENT_NOTHING = 0,   /*  */
    CLIENT_USER,          /* User */
    CLIENT_SERVER,          /* for Server Socket */
    CLIENT_DN           /* Download Server */
//    CLIENT_MEMBER,        /* Member Server */
//    CLIENT_SMEMBER        /* SubMember Server */
}ENUM_CLIENT_TYPE;

/**
 *  Client Status
 */
typedef enum
{
  STATE_WAIT = 1,
  STATE_SEND = 2,
  STATE_CLOSED = 4

  /*
    STATE_ADD,        // accept 된 상태
    STATE_MODIFY,     // epoll oneshot
    STATE_USER_CLOSE    // 연결종료된 상태
  */
}ENUM_CLIENT_STATE;

/**
*   Thread type define
*/
typedef enum
{
    THREAD_WORKER = 0,    /* WORKER  */
    THREAD_SENDER,        /* SENDER */
    THREAD_RECEIVER         /* receiver */
}ENUM_THREAD_TYPE;

// DS <-> SHM <-> DSM
struct scoreboard_file {
    char cUse;
    int isupload;
    unsigned int comcode;
    unsigned int billno;

    unsigned int count;
    unsigned int kcps;
    uint64_t    iFSize;
    uint64_t    iDNSize;
    char id[16];
    char filename[128];
    double      tAccessTime;      // Access time
    //time_t      tAccessTime;      // Access time
};

/******************************************************************************
* ACK List
******************************************************************************/
const int ACK_OK          = 1;
const int ACK_ERROR         = -1;


void   ProcessParent();

#endif

