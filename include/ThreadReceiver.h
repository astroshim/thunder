#ifndef __THREADRECEIVER_H
#define __THREADRECEIVER_H

#include "./Thread.h"

class ChatServer;
class ThreadReceiver: public Thread
{
  private :
    ChatServer *m_pChatServer;

  public:
    ThreadReceiver();
    ThreadReceiver(ChatServer* const _pChatServer);
    ~ThreadReceiver();

    void Run();
};

#endif

