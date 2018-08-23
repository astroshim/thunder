#ifndef __THREADWORKER_H
#define __THREADWORKER_H

#include "./Thread.h"

class ChatManager;
class ThreadWorker: public Thread
{
  private :
    ChatManager *m_chatManager;

  public:
    ThreadWorker();
    ThreadWorker(ChatManager* const chatManager);
    ~ThreadWorker();

    void Run();

};

#endif

