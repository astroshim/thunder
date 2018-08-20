#ifndef  __EVENTQUEUE_H
#define  __EVENTQUEUE_H

#include <stdio.h>
#include <pthread.h>
#include "./NonCopyable.h"

#define QSIZE       10240

/*
 * circular queue
 **/
class EventQueue : private NonCopyable
{
  public:
    static EventQueue & GetInstance();
    void EnQueue(const void* const data);
    const void* const DeQueue();

  protected:
    EventQueue();
    virtual ~EventQueue();

  private:
    int start_idx;            /* start of valid data */
    int num_full;           /* # of full locations */
    pthread_mutex_t buf_lock;     /* lock the structure */
    pthread_cond_t notfull;       /* full -> not full condition */
    pthread_cond_t notempty;      /* empty -> notempty condition */
    void *data[QSIZE];          /* need pointer  */
};
#endif

