#include "../include/EventQueue.h"

EventQueue::EventQueue()
        :start_idx(0)
        ,num_full(0)
{
  /*
  ** Allocation circular buffer.
  */
  pthread_mutex_init(&buf_lock, NULL);
  pthread_cond_init(&notfull, NULL);
  pthread_cond_init(&notempty, NULL);
}

EventQueue::~EventQueue()
{
  /*
  ** Destroy mutex and condition value.
  */
  pthread_mutex_destroy(&buf_lock);
  pthread_cond_destroy(&notfull);
  pthread_cond_destroy(&notempty);
}

void EventQueue::EnQueue(const void* const data)
{
  pthread_mutex_lock(&this->buf_lock);

  while (this->num_full == QSIZE) 
    pthread_cond_wait(&this->notfull, &this->buf_lock);

  this->data[(this->start_idx + this->num_full) % QSIZE] = (void *)data;

  this->num_full += 1;

  pthread_cond_signal(&this->notempty);
  pthread_mutex_unlock(&this->buf_lock);
}

const void* const EventQueue::DeQueue()
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

EventQueue & EventQueue::GetInstance()
{
  static EventQueue mEventQueue;
  return mEventQueue;
}

