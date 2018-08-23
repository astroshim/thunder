#include "../include/ThreadCloser.h"
#include "../include/DownloadServer.h"
#include "../include/Client.h"
#include "../include/CloseQueue.h"
#include "../include/EventQueue.h"

#include "../include/FDQueue.h"
#include "../include/NPLog.h"

#include <unistd.h>

ThreadCloser::ThreadCloser()
          :m_pDownloadServer(NULL)
{
  CNPLog::GetInstance().Log("ThreadCloser Construct");
}

ThreadCloser::ThreadCloser(DownloadServer* const _pDownloadServer)
          :m_pDownloadServer(_pDownloadServer)
{
  CNPLog::GetInstance().Log("ThreadCloser Construct");
}

ThreadCloser::~ThreadCloser()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadCloser Destruct");
}

void ThreadCloser::Run()
{
  while(1)
  {
    int iPacketLen;
        Client *pClient = (Client *)CloseQueue::GetInstance().DeQueue();
    CNPLog::GetInstance().Log("In ThreadCloser [%p]thread Client god! (%p) fd=(%d)",  
              this, pClient, ((Socket *)(pClient->GetSocket()))->GetFd());
  }

  delete this;
  pthread_exit(NULL);
}

