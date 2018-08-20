#include "../include/ThreadReceiver.h"
#include "../include/DownloadServer.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif
#include "../include/Client.h"
#include "../include/NPLog.h"

ThreadReceiver::ThreadReceiver()
  :m_pDownloadServer(NULL)
{
  CNPLog::GetInstance().Log("ThreadReceiver Construct");
}

ThreadReceiver::ThreadReceiver(DownloadServer* const _pDownloadServer)
  :m_pDownloadServer(_pDownloadServer)
{
  m_pDownloadServer = _pDownloadServer;
  CNPLog::GetInstance().Log("ThreadReceiver Construct");
}

ThreadReceiver::~ThreadReceiver()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadReceiver Destruct");
}

void ThreadReceiver::Run()
{
  this->SetStarted(true);

  while(1)
  {
    Client *pClient = (Client *)m_pDownloadServer->GetReceiveQueue();
#ifdef _DEBUG
    CNPLog::GetInstance().Log("In ThreadReceiver [%p]thread Client Geted! (%p) fd=(%d)",
        this, pClient, pClient->GetSocket()->GetFd());
#endif

    int iRet;
    if((iRet = pClient->FillFromSocket()) <= 0)
    {
      if(iRet == USER_CLOSE)
      {
        //CNPLog::GetInstance().Log("Close In ThreadReceiver.(%p)", pClient);
#ifdef _DEBUG
#endif

#ifdef _CLIENT_ARRAY
        m_pDownloadServer->CloseClient(pClient->GetUserSeq());
#else
        m_pDownloadServer->CloseClient(pClient);
#endif
        continue;
      }
    }
    else
    {
      int iPacketLen;
#ifndef _ONESHOT
      while((iPacketLen = pClient->IsValidPacket()) > 0)
        //if(pClient->IsValidPacket() > 0)
      {
        if(pClient->ExecuteCommand(this) < 0)
        {
          break;
        }
      }
#else
      if(pClient->IsValidPacket() > 0)
      {
        if(pClient->ExecuteCommand(this) > 0)
        {
          //CNPLog::GetInstance().Log("In ThreadReceiver Go To the Sender (%p) fd=(%d)",  pClient, pClient->GetSocket()->GetFd());
          m_pDownloadServer->PutSendQueue(pClient);
          continue;
        }
      }
#endif

#ifdef _DEBUG
      CNPLog::GetInstance().Log("In ThreadReceiver [%p]thread Client Geted! (%p) fd=(%d), iPacketLen=(%d)",
          this, pClient, pClient->GetSocket()->GetFd(), iPacketLen);
#endif
    }

#ifndef _ONESHOT
    m_pDownloadServer->AddEPoll(pClient, EPOLLIN|EPOLLOUT);
#else
    //CNPLog::GetInstance().Log("In ThreadReceiver Go To the Main (%p) fd=(%d)",  pClient, pClient->GetSocket()->GetFd());
#ifdef _FREEBSD
    m_pDownloadServer->AddEPoll(pClient, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR);
#else
    m_pDownloadServer->UpdateEPoll(pClient, EPOLLIN|EPOLLET|EPOLLONESHOT);
#endif
#endif
  }

  delete this;
  pthread_exit(NULL);
}

