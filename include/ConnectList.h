#ifndef  __CONNECTLIST_H
#define  __CONNECTLISTFDQUEUE_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

#define FD_QSIZE        10240

/*
 * fd queue
 **/
class ConnectList 
{
  public:
    int   InsertData(void *data);
    int   DeleteData(void *data);

  protected:
    ConnectList();
    virtual ~ConnectList();

  private:
    int m_iIdx;           // index position 
    int iCurrentConntionCnt;    // queued count
    pthread_mutex_t buf_lock;   /* lock the structure */

    void *data[FD_QSIZE];     /* need pointer  */
};
#endif

