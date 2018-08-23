#ifndef __THREADACCEPTOR_H
#define __THREADACCEPTOR_H

#include "./Thread.h"

class ChatServer;
class ThreadAcceptor: public Thread
{
  private :
    ChatServer *m_pMainProcess;

  public:
    ThreadAcceptor();
    ThreadAcceptor(ChatServer* const _pMainProcess);
    ~ThreadAcceptor();

    void Run();
};

#endif

