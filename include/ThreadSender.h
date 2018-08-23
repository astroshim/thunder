#ifndef __THREADSENDER_H
#define __THREADSENDER_H

#include "./Thread.h"

class ChatServer;

class ThreadSender: public Thread
{
  private :
    ChatServer *m_pChatServer;

  public:
    ThreadSender();
    ThreadSender(ChatServer* const _pChatServer);
    ~ThreadSender();

    void Run();

};

#endif

