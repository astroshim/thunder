#include "../include/ThreadWorker.h"
#include "../include/ChatManager.h"
#include "../include/Client.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif
#include "../include/NPLog.h"

ThreadWorker::ThreadWorker()
    : m_chatManager(NULL)
{
  CNPLog::GetInstance().Log("ThreadWorker Construct");
}

ThreadWorker::ThreadWorker(ChatManager *const chatManager)
    : m_chatManager(chatManager)
{
  m_chatManager = chatManager;
  CNPLog::GetInstance().Log("ThreadWorker Construct");
}

ThreadWorker::~ThreadWorker()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadWorker Destruct");
}

void ThreadWorker::Run()
{
  this->SetStarted(true);

  while (1)
  {
    // get the Client to the Eventqueue.
    //Client *pClient = (Client *)EventQueue::GetInstance().DeQueue();
    Client *pClient = (Client *)m_chatManager->GetWorkQueue();
#ifdef _DEBUG
    CNPLog::GetInstance().Log("In ThreadWorker [%p]thread Client got! (%p) fd=(%d)",
                              this, pClient, ((Socket *)(pClient->GetSocket()))->GetFd());
#endif

    int iRet;
    // Data Recv
    if ((iRet = pClient->FillFromSocket()) <= 0)
    {
      if (iRet == USER_CLOSE)
      {
#ifdef _DEBUG
        CNPLog::GetInstance().Log("Close In ThreadWorker.(%p)", pClient);
#endif
        m_chatManager->CloseClient(pClient);
        continue;
      }
    }
    else
    {
      int iPacketLen;
      while ((iPacketLen = pClient->IsValidPacket()) > 0)
      {
        if (pClient->ExecuteCommand(this) < 0)
        {
          break;
        }
      }
#ifdef _DEBUG
      CNPLog::GetInstance().Log("In ThreadWorker.(%p), packetLen=(%d)", pClient, iPacketLen);
#endif
    }

#ifdef _FREEBSD
    //m_chatManager->UpdateEPoll(pClient, EV_ADD | EV_ENABLE |EV_ONESHOT);
    m_chatManager->AddEPoll(pClient, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT | EV_ERROR);
#else
    m_chatManager->UpdateEPoll(pClient, EPOLLIN | EPOLLET | EPOLLONESHOT);
    //m_chatManager->AddEPoll(pClient, EPOLLIN|EPOLLET);
#endif
  }

  delete this;
  pthread_exit(NULL);
}
