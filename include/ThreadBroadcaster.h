#ifndef __THREADBROADCASTER_H
#define __THREADBROADCASTER_H

#include "./Thread.h"

class ChatServer;
// class BroadcastMessage;

class ThreadBroadcaster: public Thread
{
  private :
    ChatServer *m_pChatServer;

  public:
    ThreadBroadcaster();
    ThreadBroadcaster(ChatServer* const _pChatServer);
    ~ThreadBroadcaster();

    void Run();
};

#endif

