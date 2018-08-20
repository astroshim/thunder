#include "../include/Mutex.h"

Mutex::Mutex() 
{
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutex_init(&mutex, &mutexAttr);
    pthread_condattr_init(&condAttr);
    pthread_cond_init(&cond, &condAttr);
}

Mutex::~Mutex() 
{
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&mutexAttr);
    pthread_cond_destroy(&cond);
    pthread_condattr_destroy(&condAttr);
}

void Mutex::Lock() 
{
    pthread_mutex_lock(&mutex);
}

void Mutex::UnLock() 
{
    pthread_mutex_unlock(&mutex);
}

void Mutex::Wait() 
{
    pthread_cond_wait(&cond, &mutex);
}

void Mutex::Notify() 
{
    pthread_cond_signal(&cond);
}

void Mutex::NotifyAll() 
{
    pthread_cond_broadcast(&cond);
}


