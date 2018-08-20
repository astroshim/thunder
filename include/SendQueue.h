#ifndef  __SENDQUEUE_H
#define  __SENDQUEUE_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

#define SENDQSIZE       1024

/*
 * circular queue
 **/
class SendQueue : private NonCopyable
{
  public:
    static SendQueue & GetInstance();

    void EnQueue(const void* const data);
    const void* const DeQueue();

  protected:
    SendQueue();
    virtual ~SendQueue();

  private:
    int start_idx;            /* start of valid data */
    int num_full;           /* # of full locations */
    pthread_mutex_t buf_lock;     /* lock the structure */
    pthread_cond_t notfull;       /* full -> not full condition */
    pthread_cond_t notempty;      /* empty -> notempty condition */
    void *data[SENDQSIZE];          /* need pointer  */
};
#endif

