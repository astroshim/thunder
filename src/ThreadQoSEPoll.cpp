#include "../include/ThreadQoSEPoll.h"
#include "../include/DownloadServer.h"
#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/Client.h"
#include "../include/ClientUserDN.h"
#include "../include/NPLog.h"
#include "../include/NPUtil.h"

//#include "../include/IOMP_Select.h"
#include "../include/IOMP_EPoll.h"
//#include "../include/QoSQueue.h"

ThreadQoSEPoll::ThreadQoSEPoll(DownloadServer* const _pMainProcess)
              :ThreadQoS(_pMainProcess)
{
  this->SetStarted(true);
  CNPLog::GetInstance().Log("ThreadQoSEPoll Construct");
}

ThreadQoSEPoll::ThreadQoSEPoll()
{
  this->SetStarted(false);
}

ThreadQoSEPoll::~ThreadQoSEPoll()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadQoSEPoll Destruct");
}

/*
const int ThreadQoSEPoll::UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents)
{       
    return m_pIOMP->ModifyFd(_pClient, _uiEvents);
}   
*/

const int ThreadQoSEPoll::AddQoS(Client* const _pClient, const unsigned int _uiEvents)
{   
//CNPLog::GetInstance().Log("epoll_Add (%p)", _pClient);
    return m_pIOMP->AddClient(_pClient, _uiEvents);
}

const int ThreadQoSEPoll::RemoveQoS(Client* const _pClient)
{
  m_pIOMP->DelClient(_pClient);
  m_pMainProcess->PutSendQueue(_pClient);
    return 0;
}

void ThreadQoSEPoll::Run()
{
   m_pIOMP = new IOMP_EPoll(1000);

  while(1)
  {
        int iEventCount;

        if((iEventCount = m_pIOMP->Polling()) <= 0)
        {
            //CNPLog::GetInstance().Log("epoll_wait error errno=%d, strerror=(%s)", errno, strerror(errno));
            continue;
        }

//CNPLog::GetInstance().Log("ThreadQoSEPoll epoll_event count=%d", iEventCount);
        for(int i = 0; i <iEventCount; ++i)
        {
            struct  epoll_event *tEvents= m_pIOMP->GetEventStructure();
            Client *pClient = static_cast<Client *>(tEvents[i].data.ptr);
      if(pClient == NULL)
      {
CNPLog::GetInstance().Log("ThreadQoSEPoll epoll_Error");
        m_pIOMP->DelClient(pClient);
        continue;
      }

            if(tEvents[i].events & (EPOLLERR | EPOLLHUP))
            {
                CNPLog::GetInstance().Log("QoS In EPOLLERR or EPOLLHUP disconnect (%p) (%d) errno=(%d)(%s)",
                        pClient, pClient->GetSocket()->GetFd(), errno, strerror(errno));
                errno = 0;
        m_pIOMP->DelClient(pClient);

              m_pMainProcess->CloseClient(pClient);
            }
      else
            if(tEvents[i].events & EPOLLOUT)
            {
              if(static_cast<ClientUserDN*>(pClient)->GetSendTime() < CNPUtil::GetMicroTime())
              //if(pClient->GetSendTime() < CNPUtil::GetMicroTime())
        {
//CNPLog::GetInstance().Log("ThreadQos POLLOUT Send to Sender=>>(%p)", pClient);
          /*
          m_pIOMP->DelClient(pClient);
          m_pMainProcess->PutSendQueue(pClient);
          */
          RemoveQoS(pClient);
        }
/*
                if( ((ClientUserDN*)pClient)->GetSendPacketCount() > 0 &&
                    ((ClientUserDN*)pClient)->GetSendTime() < CNPUtil::GetMicroTime())
                {
//CNPLog::GetInstance().Log("In ThreadQoSEPoll =>>(%f), (%f)", ((ClientUserDN*)pClient)->GetSendTime(), CNPUtil::GetMicroTime());
                    m_pIOMP->DelClient(pClient);
                    m_pMainProcess->PutSendQueue(pClient);
                }
*/
            }

            continue;
        }

    CNPUtil::NanoSleep(1000000L);   // 0.1
  }

  pthread_exit(NULL);
}

