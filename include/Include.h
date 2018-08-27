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

#include "./NPDebug.h"
#include "./NPLog.h"

const char ON   = 1;
const char OFF  = 0;

//const int RECV_NOT_ENOUGH   = 0;  // CircularBuff
const int RECV_NOT_ENOUGH   = -2; // CircularBuff
const int RECV_ERROR    = -1; // CircularBuff

const unsigned int UNIX_PATH_MAX  = 256;
const unsigned int LOG_FILE_LEN   = 256;
const int MAX_IP_LEN        = 20;
const int MAX_PORT_CNT      = 10;

const int MAX_USER_ID       = 24;
const int MAX_PASSWD        = 24;

const int MAX_PHONE_LEN   = 16;

const int MAX_CLIENT_COUNT  = 200;

const int D_D   = 9999999;

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
  CLIENT_NOTHING = 0,     /*  */
  CLIENT_USER,            /* User */
  CLIENT_SERVER,          /* for Server Socket */
  CLIENT_CHAT_MANAGER,    /* Chat Manager user */      
  CLIENT_CHAT_SERVER      /* Chat Server user */
}ENUM_CLIENT_TYPE;

/**
 *  Client Status
 */
typedef enum
{
  STATE_WAIT = 1,
  STATE_SEND = 2,
  STATE_CLOSED = 4

}ENUM_CLIENT_STATE;

/**
 *   Thread type define
 */
typedef enum
{
  THREAD_WORKER = 0,    /* WORKER  */
  THREAD_SENDER,        /* SENDER */
  THREAD_RECEIVER,         /* receiver */
  THREAD_BROADCASTER         /* broadcast thread */
}ENUM_THREAD_TYPE;

// file send mode
typedef enum
{
  SEND_ONLY_FILE = 0,
  SEND_WIDTH_HEADER
}ENUM_SEND_MODE;


// shm to check DS Status
// use DSMgr
struct TDSStatus {
  unsigned int pid;
  unsigned int seq;
  char status;
};

struct TStatistics {
  unsigned int comcode;
  unsigned long long kcps;
};

/******************************************************************************
 * ACK List
 ******************************************************************************/
const int ACK_OK          = 1;
const int ACK_ERROR         = -1;


void   ProcessParent();

#endif

