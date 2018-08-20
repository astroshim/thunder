#ifndef  __QOSQUEUE_H
#define  __QOSQUEUE_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

#define QSIZE       1024

/*
 * circular queue
 **/
class QoSQueue : private NonCopyable
{
  public:
    static QoSQueue & GetInstance();
    void EnQueue(const void* const data);
    const void* const DeQueue();

  protected:
    QoSQueue();
    virtual ~QoSQueue();

  private:
    int start_idx;            /* start of valid data */
    int num_full;           /* # of full locations */
    pthread_mutex_t buf_lock;     /* lock the structure */
    pthread_cond_t notfull;       /* full -> not full condition */
    pthread_cond_t notempty;      /* empty -> notempty condition */
    void *data[QSIZE];          /* need pointer  */
};
#endif

