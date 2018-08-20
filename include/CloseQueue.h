#ifndef  __CLOSEQUEUE_H
#define  __CLOSEQUEUE_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

#define CLOSEQSIZE        200

/*
 * circular queue
 **/
class CloseQueue : private NonCopyable
{
  public:
    static CloseQueue & GetInstance();

    void EnQueue(const void* const data);
    const void* const DeQueue();

  protected:
    CloseQueue();
    virtual ~CloseQueue();

  private:
    int start_idx;            /* start of valid data */
    int num_full;           /* # of full locations */
    pthread_mutex_t buf_lock;     /* lock the structure */
    pthread_cond_t notfull;       /* full -> not full condition */
    pthread_cond_t notempty;      /* empty -> notempty condition */
    void *data[CLOSEQSIZE];         /* need pointer  */
};
#endif

