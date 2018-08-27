#include "../include/ThreadBroadcaster.h"
#include "../include/BroadcastMessage.h"
#include "../include/ChatServer.h"
#include "../include/Client.h"
#include "../include/ChatUser.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif

#include "../include/NPUtil.h"
#include "../include/NPLog.h"

#ifdef _FREEBSD
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#else
#include <sys/sendfile.h>
#endif

ThreadBroadcaster::ThreadBroadcaster()
    : m_pChatServer(NULL)
{
  CNPLog::GetInstance().Log("ThreadBroadcaster Construct");
}

ThreadBroadcaster::ThreadBroadcaster(ChatServer *const _pChatServer)
    : m_pChatServer(_pChatServer)
{
  CNPLog::GetInstance().Log("ThreadBroadcaster Construct");
}

ThreadBroadcaster::~ThreadBroadcaster()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadBroadcaster Destruct");
}

void ThreadBroadcaster::Run()
{
  while (1)
  {
    BroadcastMessage *message = (BroadcastMessage *)m_pChatServer->GetBroadcastQueue();
#ifdef _DEBUG
    CNPLog::GetInstance().Log("broadcast socket: %d, message : (%s)", message->GetSocketFd(), message->GetMessage());
#endif

    m_pChatServer->MessageBroadcast(message);
    delete message;

    // char *message = (char *)m_pChatServer->GetBroadcastQueue();
    // ChatUser *pClient = (ChatUser *)m_pChatServer->GetBroadcastQueue();
    // CNPLog::GetInstance().Log("broadcast message : (%s)", message);
  }

  delete this;
  pthread_exit(NULL);
}
