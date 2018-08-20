#include "../include/QoSQueue.h"

QoSQueue::QoSQueue()
        :start_idx(0)
        ,num_full(0)
{
  /*
  ** Allocation circular buffer.
  */
/*
  circ_buf_t * data_q = (circ_buf_t *) malloc(sizeof (circ_buf_t));
  if (data_q == NULL)
    exit(0);
*/
  pthread_mutex_init(&buf_lock, NULL);
  pthread_cond_init(&notfull, NULL);
  pthread_cond_init(&notempty, NULL);
}

QoSQueue::~QoSQueue()
{
  /*
  ** Destroy mutex and condition value.
  */
  pthread_mutex_destroy(&buf_lock);
  pthread_cond_destroy(&notfull);
  pthread_cond_destroy(&notempty);
}

void QoSQueue::EnQueue(const void* const data)
{
  pthread_mutex_lock(&this->buf_lock);

  while (this->num_full == QSIZE) 
    pthread_cond_wait(&this->notfull, &this->buf_lock);

  this->data[(this->start_idx + this->num_full) % QSIZE] = (void *)data;

  this->num_full += 1;

  pthread_cond_signal(&this->notempty);
  pthread_mutex_unlock(&this->buf_lock);
}

const void* const QoSQueue::DeQueue()
{
  void *data;

  pthread_mutex_lock(&this->buf_lock);

  while (this->num_full == 0)
    pthread_cond_wait(&this->notempty, &this->buf_lock);

  data = this->data[this->start_idx];

  this->start_idx = (this->start_idx + 1) % QSIZE;

  this->num_full -= 1;

  pthread_cond_signal(&this->notfull);

  pthread_mutex_unlock(&this->buf_lock);

  return (data);
}

QoSQueue & QoSQueue::GetInstance()
{
  static QoSQueue mQoSQueue;
  return mQoSQueue;
}

