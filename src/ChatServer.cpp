/**
 * Author:  astrosim
 */
#include "../include/Client.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif
#include "../include/Properties.h"
#include "../include/CircularQueue.h"

#include "../include/ChatServer.h"
#include "../include/ServerSocket.h"
#include "../include/Thread.h"
#include "../include/ThreadAcceptor.h"
#include "../include/ThreadReceiver.h"
#include "../include/ThreadBroadcaster.h"
#include "../include/BroadcastMessage.h"
#include "../include/ThreadSender.h"
#include "../include/ThreadTic.h"
#include "../include/ThreadManager.h"
#include "../include/ServerInfoDN.h"
#include "../include/NPUtil.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"
#include "../include/ChatUser.h"
#include "../include/MessageQ.h"
#include "../include/SharedMemory.h"
#include "../include/ReleaseSlot.h"

ChatServer::ChatServer()
  :m_pServerInfo(NULL)
  ,m_iConnCount(0)
  ,m_pDNServerSocket(NULL)
  ,m_pShmKcps(NULL)
  ,m_pShmDSStatus(NULL)
  ,m_pShmD(NULL)
  ,m_pIOMP(NULL)
  ,m_pReceiveQueue(NULL)
  ,m_pSendQueue(NULL)
  ,m_pBroadcastQueue(NULL)
  ,m_iSeq(-1)
  ,m_iMaxUser(0)
  ,m_iShmKey(0)
  ,m_iShmDSStatus(0)
   ,m_pSlot(NULL)
{
  this->SetStarted(false);
  pthread_mutex_init(&m_lockClient, NULL);
}

ChatServer::ChatServer(Properties& _cProperties)
  :m_iConnCount(0)
  ,m_pDNServerSocket(NULL)
  ,m_pShmKcps(NULL)
  ,m_pShmDSStatus(NULL)
  ,m_pShmD(NULL)
  ,m_pIOMP(NULL)
  ,m_pReceiveQueue(new CircularQueue())
  ,m_pSendQueue(new CircularQueue())
  ,m_pBroadcastQueue(new CircularQueue())
  ,m_iSeq(-1)
  ,m_iMaxUser(0)
  ,m_iShmKey(0)
  ,m_iShmDSStatus(0)
   ,m_pSlot(NULL)
{
  this->SetStarted(false);
  pthread_mutex_init(&m_lockClient, NULL);
  m_pServerInfo = new ServerInfoDN(_cProperties);
}

ChatServer::~ChatServer()
{
  this->SetStarted(false);
  delete m_pIOMP;
  delete m_pServerInfo;
  delete m_pReceiveQueue;
  delete m_pSendQueue;
  delete m_pBroadcastQueue;
  delete m_pDNServerSocket;
  delete m_pShmKcps;
  delete m_pShmDSStatus;
  delete m_pShmD;
  delete m_pSlot;
}

const int ChatServer::GetCurrentUserCount()
{
  return m_iConnCount;
}

const int ChatServer::GetMaxUser()
{
  return m_iMaxUser;
}

const int ChatServer::GetShmKey()
{
  return m_iShmKey;
}

const int ChatServer::GetSeq()
{
  return m_iSeq;
}

void ChatServer::SetSeq(const int _iSeq)
{
  m_iSeq = _iSeq;
}

void ChatServer::SetMaxUser(const int _iMaxUser)
{
  m_iMaxUser = _iMaxUser;
}

void ChatServer::SetShmKey(const int _iShmKey)
{
  m_iShmKey = _iShmKey;
}

const char* const ChatServer::GetIPAddr()
{
  return m_pServerInfo->GetIPAddr();
}

void ChatServer::PutReceiveQueue(const void* const _pVoid)
{
  m_pReceiveQueue->EnQueue(_pVoid);
}

const void* const ChatServer::GetReceiveQueue()
{
  return m_pReceiveQueue->DeQueue();
}

void ChatServer::PutSendQueue(const void* const _pVoid)
{
  m_pSendQueue->EnQueue(_pVoid);
}

const void* const ChatServer::GetSendQueue()
{
  return m_pSendQueue->DeQueue();
}

void ChatServer::PutBroadcastQueue(BroadcastMessage *message, Client *const _pClient)
{
  m_pBroadcastQueue->EnQueue(message);
}

const void* const ChatServer::GetBroadcastQueue()
{
  return m_pBroadcastQueue->DeQueue();
}

const char* const ChatServer::GetMRTGURL()
{
  return m_pServerInfo->GetMRTGURL();
}

const int ChatServer::GetServerPort()
{
  return m_pServerInfo->GetPort(SERVER_PORT);
}

const int ChatServer::GetDNServerPort()
{
  return m_pServerInfo->GetPort(SERVER_PORT);
}

const char* const ChatServer::GetVolName()
{
  return m_pServerInfo->GetVolName();
}

const char* const ChatServer::GetDirName()
{
  return m_pServerInfo->GetDirName();
}

const char* const ChatServer::GetLogFileName()
{
  return m_pServerInfo->GetLogFileName();
}

const unsigned int ChatServer::GetBandwidth(const char _chID)
{
  return m_pServerInfo->GetBandwidth(_chID);
}

// for string delimiter
vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;
    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}

const int ChatServer::ConnectToMgr()
{
  int port = m_pServerInfo->GetPort(SERVER_PORT_MGR);

  vector<string> v = split(m_pServerInfo->GetManagerIpAddresses(), ",");
  for (const string& server : v) {
    CNPLog::GetInstance().Log("Connect to (%s:%d)", server.c_str(), port);
    m_lstChatManagerSocket.push_back(NegotiationWithManager(server, port));
  }

  return 0;
}

ClientSocket* const ChatServer::NegotiationWithManager(string server, int port)
{
  CNPLog::GetInstance().Log("Trying Connect to Mgr.. (%s)(%d)", server.c_str(), port);

  ClientSocket *pCSocket = new ClientSocket();
  if(pCSocket->Connect(server.c_str(), port) < 0)
  {
    CNPLog::GetInstance().Log("Error Connect to Mgr (%s)(%d)", server.c_str(), port);
    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;
    return NULL;
  }

  // hello to mgr
  T_PACKET tHelloPacket ;
  Tcmd_HELLO_DS_DSM *sndbody = (Tcmd_HELLO_DS_DSM *)tHelloPacket.data;
  memset((char *)&tHelloPacket, 0x00, sizeof(T_PACKET));
  tHelloPacket.header.command = cmd_HELLO_DS_DSM;
  tHelloPacket.header.length = sizeof(Tcmd_HELLO_DS_DSM);

  sndbody->iPid = GetPid();

  if(pCSocket->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
  {
    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Send Fail (%s)(%d)", server.c_str(), port);

    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;
    return NULL;
  }

  memset((char *)&tHelloPacket, 0x00, sizeof(T_PACKET));
  if(pCSocket->Recv((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DSM_DS)) < 0)
  {
    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Recv Fail (%s)(%d)", server.c_str(), port);

    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;
    return NULL;
  }

  Tcmd_HELLO_DSM_DS *pRcvBody = (Tcmd_HELLO_DSM_DS *)tHelloPacket.data;
  CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Recv seq=(%d), shmKey=(%d), maxUser=(%d)",
      pRcvBody->iSeq, pRcvBody->iShmKey, pRcvBody->iMaxUser);

  SetSeq(pRcvBody->iSeq);

  SetShmKey(pRcvBody->iShmKey);
  m_iShmDSStatus = pRcvBody->iShmDSStatus;
  SetMaxUser(pRcvBody->iMaxUser);
  return pCSocket;
}

void ChatServer::HealthCheckUsers()
{
  if(*m_pShmD == D_D)
  {
    SetStarted(false);
  }

  pthread_mutex_lock(&m_lockClient);
  std::list<Client*>::iterator iter = m_lstClient.begin();
  while( iter != m_lstClient.end() )
  {
    Client *pClient = static_cast<Client *>(*iter);

    if(CNPUtil::GetMicroTime()-pClient->GetAccessTime() > TIME_ALIVE)
    {
      #ifdef _FREEBSD
      m_pIOMP->DelClient(pClient, EVFILT_READ);
      #else
      m_pIOMP->DelClient(pClient);
      #endif

      CNPLog::GetInstance().Log("ChatServer::HealthCheckUsers Kill Client [%p] fd=(%d)=(%f)\n",
          pClient,
          pClient->GetSocket()->GetFd(),
          CNPUtil::GetMicroTime()-pClient->GetAccessTime());

      iter = m_lstClient.erase( iter );
      m_pSlot->PutSlot(pClient->GetUserSeq());

      delete pClient;
      m_iConnCount--;
    }

    iter++;
  }
  pthread_mutex_unlock(&m_lockClient);
}

Client* const ChatServer::GetServerSocket()
{
  return m_pDNServerSocket;
}

void ChatServer::SetServerSocket(Client *_pClient)
{
  m_pDNServerSocket = _pClient;
}

void ChatServer::UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
#ifndef _FREEBSD
  m_pIOMP->ModifyFd(_pClient, _uiEvents);
#endif
}

#ifdef _FREEBSD
void ChatServer::AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags)
{
  m_pIOMP->AddClient(_pClient, _filter, _usFlags);
}
#else
void ChatServer::AddEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
  m_pIOMP->AddClient(_pClient, _uiEvents);
}
#endif

void ChatServer::AcceptClient(Socket* const _pClientSocket, ENUM_CLIENT_TYPE type)
{
  m_iConnCount++;
  pthread_mutex_lock(&m_lockClient);
  int iSlot = m_pSlot->GetFreeSlot();
  if(iSlot < 0)
  {
    CNPLog::GetInstance().Log("Accept Client SlotOverFlow ClientSocket=(%p)", _pClientSocket);
    delete _pClientSocket;
    pthread_mutex_unlock(&m_lockClient);
    return;
  }

  Client *pNewClient;
  pNewClient = new ChatUser(_pClientSocket);
  pNewClient->SetType(type);
  pNewClient->SetMainProcess(this);

  CNPLog::GetInstance().Log("1.NewClient Client=(%p), ClientSocket=(%p)", pNewClient, _pClientSocket);
  pNewClient->SetUserSeq(iSlot);
  m_lstClient.push_back((Client *)pNewClient);

  CNPLog::GetInstance().Log("NewClient(%p) ClientSocket=(%p), FD=(%d), slot=(%d) ",
      pNewClient,
      _pClientSocket,
      pNewClient->GetSocket()->GetFd(),
      pNewClient->GetUserSeq());
  pthread_mutex_unlock(&m_lockClient);

#ifndef _ONESHOT
  if(m_pIOMP->AddClient(pNewClient, EPOLLIN) < 0)
#else
#ifdef _FREEBSD
    if(m_pIOMP->AddClient(pNewClient, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR) < 0)
#else
      if(m_pIOMP->AddClient(pNewClient, EPOLLIN|EPOLLET|EPOLLONESHOT) < 0)
#endif
#endif
      {
        CloseClient(pNewClient);
        return;
      }
}

void ChatServer::CloseClient(Client* const _pClient)
{
  #ifdef _FREEBSD
  m_pIOMP->DelClient(_pClient, EVFILT_READ);
  #else
  m_pIOMP->DelClient(_pClient);
  #endif

  CNPLog::GetInstance().Log("ChatServer::CloseClient(%p) 1", _pClient);

  if(_pClient->GetType() == CLIENT_USER)
  {
    // static_cast<ChatUser *>(_pClient)->SendCloseToMgr();
  }

  pthread_mutex_lock(&m_lockClient);
  int iSlot = _pClient->GetUserSeq();
  m_lstClient.remove(_pClient);
  m_pSlot->PutSlot(iSlot);
  delete _pClient;
  pthread_mutex_unlock(&m_lockClient);
  m_iConnCount--;
}

void ChatServer::MessageBroadcastToManagers(BroadcastMessage *_message)
{
  T_PACKET tSendPacket;
  memset((char *)&tSendPacket, 0x00, sizeof(T_PACKET));

  memcpy(tSendPacket.data, _message->GetMessage(), _message->GetMessageSize());
  Tcmd_CHAT_DS_DSM *sndbody = (Tcmd_CHAT_DS_DSM *)tSendPacket.data;

  tSendPacket.header.command  = cmd_CHAT_DS_DSM;
  tSendPacket.header.length   = _message->GetMessageSize() + sizeof(unsigned int);

  sndbody->iPid = GetPid();
  memcpy(sndbody->message, _message->GetMessage(), _message->GetMessageSize());

  std::list<ClientSocket*>::iterator iter = m_lstChatManagerSocket.begin();
  while( iter != m_lstChatManagerSocket.end() )
  {
    ClientSocket *socket = static_cast<ClientSocket *>(*iter);

    #ifdef _FREEBSD
    CNPLog::GetInstance().Log("ChatServer::MessageBroadcastToManagers message socket=(%d), message=(%s)",  
            socket->GetFd(), _message->GetMessage());
    #endif

    CNPLog::GetInstance().Log("ChatServer:: 메세지를 manager로 relay ! size=(%d), message=(%s)", tSendPacket.header.length, sndbody->message);

    socket->Write((char *)&tSendPacket, PDUHEADERSIZE+tSendPacket.header.length);
    iter++;
  }
}

int ChatServer::RegisterManager()
{
  std::list<ClientSocket*>::iterator iter = m_lstChatManagerSocket.begin();
  while( iter != m_lstChatManagerSocket.end() )
  {
    ClientSocket *socket = static_cast<ClientSocket *>(*iter);
    AcceptClient(socket, CLIENT_CHAT_MANAGER);
    iter++;
  }
  return 0;
}

void ChatServer::MessageBroadcast(BroadcastMessage *_message)
{
  CNPLog::GetInstance().Log("ChatServer:: message type => %d", _message->GetMessageType());

  if (_message->GetMessageType() != RELAYED_MESSAGE) 
  {
    MessageBroadcastToManagers(_message);
  }

  // pthread_mutex_lock(&m_lockClient);
  std::list<Client*>::iterator iter = m_lstClient.begin();
  while( iter != m_lstClient.end() )
  {
    Client *pClient = static_cast<Client *>(*iter);

    if( pClient->GetType() == CLIENT_USER && pClient->GetSocket()->GetFd() != _message->GetSocketFd()) {
      CNPLog::GetInstance().Log("ChatServer::MessageBroadCast client socket:(%d), message socket=(%d), message=(%s)",  
                                      pClient->GetSocket()->GetFd(),
                                      _message->GetSocketFd(),
                                      _message->GetMessage());

      pClient->GetSocket()->Write(_message->GetMessage(), _message->GetMessageSize());
    }

    iter++;
  }
  // pthread_mutex_unlock(&m_lockClient);
}

void ChatServer::SetD()
{
  (*m_pShmD) = D_D;
}

void ChatServer::Run()
{
  this->SetStarted(true);
  SetPPid(getppid());

  /**
   *   IOMP Class Create
   */
#ifdef _FREEBSD
  m_pIOMP = new IOMP_KQUEUE(500000000);
#else
  m_pIOMP = new IOMP_EPoll(1000);
#endif

  // create log file. 
  char pchLogFileName[1024];
  memset(pchLogFileName, 0x00, sizeof(pchLogFileName));
  sprintf(pchLogFileName, "%s_%d", m_pServerInfo->GetLogFileName(), getpid());
  CNPLog::GetInstance().SetFileName(pchLogFileName);

  sleep(2);

  // using socket
  if(ConnectToMgr() < 0)
  {
    return;
  }
  CNPLog::GetInstance().Log("SERVER_PORT_DNMGR = (%d)", m_pServerInfo->GetPort(SERVER_PORT_MGR));
  CNPLog::GetInstance().Log("SERVER_PORT = (%d)", m_pServerInfo->GetPort(SERVER_PORT));

  // create release slot
  m_pSlot = new ReleaseSlot(GetMaxUser());

  // attach shm where ds status
  SharedMemory smDSStatus((key_t)m_iShmDSStatus, sizeof(struct TDSStatus));
  m_pShmDSStatus = (struct TDSStatus *)smDSStatus.GetDataPoint();
  m_pShmDSStatus = &(m_pShmDSStatus[GetSeq()]);

  SharedMemory smD((key_t)188891, sizeof(int));
  if(!smD.IsStarted())
  {
    return ;
  }
  m_pShmD = (int *)smD.GetDataPoint();
  if(m_pShmD == NULL)
  {
    printf("smKcps is NULL \n");
    return;
  }

  // for statistics 20090527
  SharedMemory smKcps((key_t)m_pServerInfo->GetShmKey(), sizeof(TStatistics)*MAX_COMPANY);
  if(!smKcps.IsStarted())
  {
    printf("smKcps SharedMemory \n");

    // 2. destroy
    SharedMemory smKcps((key_t)m_pServerInfo->GetShmKey());
    smKcps.Destroy();
    return ;
  }

  m_pShmKcps = (TStatistics *)smKcps.GetDataPoint();
  if(m_pShmKcps == NULL)
  {
    printf("smKcps is NULL \n");
    return;
  }
  memset(m_pShmKcps, 0, sizeof(TStatistics)*MAX_COMPANY);

  ThreadAcceptor *tAcceptor = new ThreadAcceptor(this);
  ThreadManager::GetInstance()->Spawn(tAcceptor);

  for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_SENDER); i++)
  {
    Thread *t = new ThreadSender(this);
    ThreadManager::GetInstance()->Spawn(t);
    CNPLog::GetInstance().Log("In ChatServer Sender Create (%p,%lu) ", t, t->GetThreadID());
  }

  for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_RECEIVER); i++)
  {
    Thread *t = new ThreadReceiver(this);
    ThreadManager::GetInstance()->Spawn(t);
    CNPLog::GetInstance().Log("In ChatServer Receiver Create (%p,%lu) ", t, t->GetThreadID());
  }

  for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_BROADCASTER); i++)
  {
    Thread *t = new ThreadBroadcaster(this);
    ThreadManager::GetInstance()->Spawn(t);
    CNPLog::GetInstance().Log("In ChatServer Broadcaster Create (%p,%lu) ", t, t->GetThreadID());
  }

  // ThreadTic *tTic = new ThreadTic(this);
  // ThreadManager::GetInstance()->Spawn(tTic);

  m_pShmDSStatus->status = ON;
  CNPLog::GetInstance().Log("In ChatServer Status pid=(%d), seq=(%d), status=(%d)",
      m_pShmDSStatus->pid
      ,m_pShmDSStatus->seq
      ,m_pShmDSStatus->status);

  RegisterManager();

  while(1)
  {
    int iEventCount;

    if((iEventCount = m_pIOMP->Polling()) <= 0)
    {
      if(!GetStarted())
      {
        break;
      }
      continue;
    }

    for(int i = 0; i < iEventCount; ++i)
    {

#ifdef _FREEBSD
      struct kevent *tEvents = m_pIOMP->GetEventStructure();
      Client *pClient = static_cast<Client *>(tEvents[i].udata);
#else
      struct  epoll_event *tEvents= m_pIOMP->GetEventStructure();
      Client *pClient = (Client *)(tEvents[i].data.ptr);
#endif

      // ServerSocket event Check
      if(pClient->GetType() == CLIENT_SERVER)
      {
        Socket *pClientSocket;
        if((pClientSocket = static_cast<ServerSocket *>(pClient->GetSocket())->Accept()) != NULL)
        {
          pClientSocket->SetNonBlock();

          CNPLog::GetInstance().Log("Accept Client IP=[%s], CurrentUserCount=(%d),MaxCount=(%d)" ,
              ((TcpSocket *)pClientSocket)->GetClientIpAddr(), GetCurrentUserCount(), GetMaxUser());

          if(GetCurrentUserCount() >= GetMaxUser())
          {
            CNPLog::GetInstance().Log("Max User OverFlow! Force Kill CurrentUser=(%d), MaxUser=(%d)",
                GetCurrentUserCount(),
                GetMaxUser());
            delete pClientSocket;
            continue;
          }

          if(static_cast<ServerSocket *>(pClient->GetSocket())->GetType() == SERVER_PORT)
          {
            AcceptClient(pClientSocket, CLIENT_USER);
          }
          else
          {
            CNPLog::GetInstance().Log("There is no platform!");
            delete pClientSocket;
          }
        }
        continue;
      }

#ifdef _FREEBSD
      if( tEvents[i].flags & EV_ERROR )
      {
        CNPLog::GetInstance().Log("In EPOLLERR or EPOLLHUP disconnect (%p) (%d) errno=(%d)(%s)",
            pClient, pClient->GetSocket()->GetFd(), errno, strerror(errno));
        errno = 0;
        CloseClient(pClient);
      }
      else
        if(tEvents[i].filter == EVFILT_READ)
        {
  #ifdef _DEBUG
          CNPLog::GetInstance().Log("EPOLLIN Client %p", pClient);
  #endif
          pClient->SetAccessTime();
          PutReceiveQueue(pClient);
        }

#else
      if(tEvents[i].events & (EPOLLERR | EPOLLHUP))
      {
        errno = 0;
        CloseClient(pClient);
      }
      else
        if(tEvents[i].events & EPOLLIN)
        {
  #ifdef _DEBUG
          CNPLog::GetInstance().Log("EPOLLIN Client %p, events=(%d)", pClient, tEvents[i].events);
  #endif

  #ifndef _ONESHOT
          m_pIOMP->DelClient(pClient);
  #endif
          pClient->SetAccessTime();
          PutReceiveQueue(pClient);
        }
  #ifndef _ONESHOT
        else
          if(tEvents[i].events & EPOLLOUT)
          {
            if( ((ChatUser*)pClient)->GetSendPacketCount() > 0 &&
                ((ChatUser*)pClient)->GetSendTime() < CNPUtil::GetMicroTime())
            {
              //CNPLog::GetInstance().Log("2. In Download Server =>>(%f), (%f)", ((ChatUser*)pClient)->GetSendTime(), CNPUtil::GetMicroTime());
              m_pIOMP->DelClient(pClient);
              pClient->SetAccessTime();
              PutSendQueue(pClient);
            }
          }
  #endif

#endif
      continue;
    }
  }

  ThreadManager::GetInstance()->ReleaseInstance();
}
