/**
 * Author:  astrosim
 */
#include "../include/ChatManager.h"
#include "../include/ChatServer.h"
#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/Thread.h"
#include "../include/ThreadWorker.h"
#include "../include/ThreadTic.h"
#include "../include/ThreadManager.h"
#include "../include/ServerInfoDNMgr.h"
#include "../include/NPUtil.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"
#include "../include/ClientServer.h"
#include "../include/ClientChatServer.h"

#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif

#include "../include/CircularQueue.h"

#include "../include/SharedMemory.h"

#ifndef _FREEBSD
#include <curl/curl.h>
#endif

ChatManager::ChatManager()
  :m_pIOMP(NULL)
  ,m_iConnCount(0)
  ,m_iMacAddr(0)
  ,m_iIPAddr(0)
  ,m_pServerInfo(NULL)
  ,m_pWorkQueue(NULL)
  ,m_ChatServerInfo(NULL)
  // ,m_pShm(NULL)
   ,m_pShmDSStatus(NULL)
{
  this->SetStarted(false);
  pthread_mutex_init(&m_lockClient, NULL);
  pthread_mutex_init(&m_lockClosed, NULL);
  m_pchStatistics = new char[MAX_STATISTICS];
}

ChatManager::ChatManager(Properties& _cProperties)
  :m_pIOMP(NULL)
  ,m_iConnCount(0)
  ,m_iMacAddr(0)
  ,m_iIPAddr(0)
  ,m_pWorkQueue(new CircularQueue())
  ,m_ChatServerInfo(NULL)
  // ,m_pShm(NULL)
   ,m_pShmDSStatus(NULL)
{
  this->SetStarted(false);
  pthread_mutex_init(&m_lockClient, NULL);
  //pthread_mutex_init(&m_lockShm, NULL);
  pthread_mutex_init(&m_lockClosed, NULL);
  m_pServerInfo = new ServerInfoDNMgr(_cProperties);
  m_pchStatistics = new char[MAX_STATISTICS];
  //    CNPLog::GetInstance().Log("================ Create ChatManager =====================");
}

ChatManager::~ChatManager()
{
  this->SetStarted(false);
  //  DeleteAllMember();
  delete m_pIOMP;
  delete m_pServerInfo;
  delete m_pWorkQueue;
  //  delete m_pRecvPipe;
  //  delete m_pSlot;
  delete m_ChatServerInfo;
  // delete m_pShm;
  delete m_pShmDSStatus;
  //  delete m_pMQ;
  delete [] m_pchStatistics;
}

const int ChatManager::GetCurrentUserCount()
{
  /*
     return m_mapConnectList.size();


     int iCnt;
     pthread_mutex_lock(&m_lockClient);
     iCnt = m_lstChatServer.size();
     pthread_mutex_unlock(&m_lockClient);

     return iCnt;
     */
  return m_iConnCount;
}

const int ChatManager::GetMaxUser()
{
  return m_pServerInfo->GetMaxUser() + m_pServerInfo->GetDNCnt();
}

const char* const ChatManager::GetIPAddr()
{
  return m_pServerInfo->GetIPAddr();
}

const char* const ChatManager::GetMRTGURL()
{
  return m_pServerInfo->GetMRTGURL();
}

const int ChatManager::GetDNServerPort()
{
  return m_pServerInfo->GetDNPort();
}

const unsigned int ChatManager::GetMacAddress()
{
  return m_iMacAddr;
}

const unsigned int ChatManager::GetIPAddress()
{
  return m_iIPAddr;
}

const char* const ChatManager::GetLogFileName()
{
  return m_pServerInfo->GetLogFileName();
}

const int ChatManager::PostData(const char* _pData, const char* _pchURL, const char *_pchAuth)
{
#ifndef _FREEBSD

  /*
     CURL *curl;
     CURLcode res;

     curl = curl_easy_init();
     if(curl)
     {
     curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
     curl_easy_setopt(curl, CURLOPT_URL, _pchURL);
     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _pData);
     curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(_pData));

     curl_easy_setopt (curl, CURLOPT_TIMEOUT, 150L);
     curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT, 15L);
     res = curl_easy_perform(curl);
     if (res != 0)
     {
     CNPLog::GetInstance().Log("cURL PostData perform error(%d)", res);
     }

     curl_easy_cleanup(curl);
     } else {
     CNPLog::GetInstance().Log("cURL PostData init error");
     return -1;
     }
     */

  CURL *curl;
  CURLcode res;

  struct curl_slist *headers=NULL;
  //headers = curl_slist_append(headers, "Authorization: Basic d2ViOnFuc3Rrcw==");
  headers = curl_slist_append(headers, _pchAuth);
  curl = curl_easy_init();
  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
    curl_easy_setopt(curl, CURLOPT_URL, _pchURL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _pData);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(_pData));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers );

    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //curl_easy_setopt(curl, CURLOPT_USERPWD, "Authorization: Basic d2ViOnFuc3Rrcw==" );
    ////curl_easy_setopt(curl, CURLOPT_USERPWD, "d2ViOnFuc3Rrcw==" );

    curl_easy_setopt (curl, CURLOPT_TIMEOUT, 150L);
    curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT, 15L);
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      CNPLog::GetInstance().Log("cURL perform error(%d)", res);
    }

    curl_easy_cleanup(curl);
  } else {
    CNPLog::GetInstance().Log("cURL init error");
    return -1;
  }

  if(headers) curl_slist_free_all(headers);

#endif
  return 0;
}

const int ChatManager::PostData(const char* _pData, const char* _pchURL)
{
#ifndef _FREEBSD
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
    curl_easy_setopt(curl, CURLOPT_URL, _pchURL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _pData);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(_pData));

    curl_easy_setopt (curl, CURLOPT_TIMEOUT, 150L);
    curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT, 15L);
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      CNPLog::GetInstance().Log("cURL perform error(%d)", res);
    }

    curl_easy_cleanup(curl);
  } else {
    CNPLog::GetInstance().Log("cURL init error");
    return -1;
  }
#endif

  return 0;
}

void ChatManager::PutWorkQueue(const void* const _pVoid)
{
  m_pWorkQueue->EnQueue(_pVoid);
}

const void* const ChatManager::GetWorkQueue()
{
  return m_pWorkQueue->DeQueue();
}

void ChatManager::PutClosedList(Tcmd_USER_CLOSE_DS_DSM* const _pClosedInfo)
{
  pthread_mutex_lock(&m_lockClosed);
  CNPLog::GetInstance().Log("ChatManager::PutClosedList =>(%p)", _pClosedInfo);
  m_lstClosed.push_back(_pClosedInfo);
  pthread_mutex_unlock(&m_lockClosed);
}

const uint64_t ChatManager::GetClientDownloadSize(uint32_t _iComCode, uint32_t _iBillno)
{
  uint64_t iRet = 0;

  pthread_mutex_lock(&m_lockClosed);

  std::list<Tcmd_USER_CLOSE_DS_DSM*>::iterator iter = m_lstClosed.begin();
  while( iter != m_lstClosed.end() )
  {
    Tcmd_USER_CLOSE_DS_DSM *pClosedInfo = static_cast<Tcmd_USER_CLOSE_DS_DSM *>(*iter);

    if(pClosedInfo->nComCode == _iComCode &&
        pClosedInfo->nBillNo == _iBillno)
    {
      iter = m_lstClosed.erase( iter );
      iRet += pClosedInfo->nDownSize;
      delete pClosedInfo;
    }

    iter++;
  }

  pthread_mutex_unlock(&m_lockClosed);
  return iRet;
}

void ChatManager::HealthCheckClosedList()
{

  pthread_mutex_lock(&m_lockClosed);
  //CNPLog::GetInstance().Log("ChatManager::HealthCheckClosedList list_size=(%d), max=(%d)", m_lstClosed.size(), m_lstClosed.max_size());

  std::list<Tcmd_USER_CLOSE_DS_DSM*>::iterator iter = m_lstClosed.begin();
  while( iter != m_lstClosed.end() )
  {
    Tcmd_USER_CLOSE_DS_DSM *pClosedInfo = static_cast<Tcmd_USER_CLOSE_DS_DSM *>(*iter);

    double dNow = CNPUtil::GetMicroTime();
    //if((CNPUtil::GetMicroTime() - pClosedInfo->dClosedTime) > TIME_ALIVE)
    if((dNow - pClosedInfo->dClosedTime) > TIME_ALIVE)
    {
      CNPLog::GetInstance().Log("ChatManager::HealthCheckClosedList(%p) Kill \
          comcode=(%d),billno=(%d),downSize=(%llu), (%.2f)",
          pClosedInfo, pClosedInfo->nComCode, pClosedInfo->nBillNo, pClosedInfo->nDownSize, dNow);

      iter = m_lstClosed.erase( iter );
      delete pClosedInfo;
    }

    iter++;
  }

  pthread_mutex_unlock(&m_lockClosed);
}


void ChatManager::HealthCheckUsers()
{
  pthread_mutex_lock(&m_lockClient);

  std::list<Client*>::iterator iter = m_lstChatServer.begin();
  while( iter != m_lstChatServer.end() )
  {
    Client *pClient = static_cast<Client *>(*iter);
    if(pClient->GetType() == CLIENT_USER)
    {
      if(CNPUtil::GetMicroTime()-pClient->GetAccessTime() > TIME_ALIVE ||
          pClient->IsClosed())
      {
        CNPLog::GetInstance().Log("ChatManager::HealthCheckUsers Kill Client [%p] fd=(%d)=(%d)",
            pClient, pClient->GetSocket()->GetFd(), CNPUtil::GetUnixTime()-pClient->GetAccessTime());
        iter = m_lstChatServer.erase( iter );

#ifdef _FREEBSD
        m_pIOMP->DelClient(pClient, EVFILT_READ);
#else
        m_pIOMP->DelClient(pClient);
#endif
        delete pClient;
        m_iConnCount--;
      }
    }

    iter++;
  }
  pthread_mutex_unlock(&m_lockClient);
  HealthCheckClosedList();
}

void ChatManager::UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
#ifndef _FREEBSD
  m_pIOMP->ModifyFd(_pClient, _uiEvents);
#endif
}

#ifdef _FREEBSD
void ChatManager::AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags)
{
  m_pIOMP->AddClient(_pClient, _filter, _usFlags);
}
#else
void ChatManager::AddEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
  m_pIOMP->AddClient(_pClient, _uiEvents);
}
#endif

void ChatManager::CloseClient(Client* const _pClient)
  //void ChatManager::CloseClient(Client* _pClient)
{
  CNPLog::GetInstance().Log("ChatManager::CloseClient ==> (%p)(%d)", _pClient, _pClient->GetSocket()->GetFd());
#ifdef _FREEBSD
  m_pIOMP->DelClient(_pClient, EVFILT_READ);
#else
  m_pIOMP->DelClient(_pClient);
#endif

  pthread_mutex_lock(&m_lockClient);
  m_lstChatServer.remove(_pClient);
  pthread_mutex_unlock(&m_lockClient);

  // if you SetState() before erase the Client in the client map,
  // it could occur the crash program.
  // because of SetState() is on a collision with HealthCheck
  _pClient->SetState(STATE_CLOSED);

  if(_pClient->GetType() == CLIENT_CHAT_SERVER)
  {
    memset(&(m_ChatServerInfo[_pClient->GetUserSeq()]), 0, sizeof(Tcmd_HELLO_DSM_DS));
  }

  delete _pClient;

  m_iConnCount--;

  //  CloseQueue::GetInstance().EnQueue(_pClient);
}

void ChatManager::SettingDS(const int _iPos, int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const
    _piShmDSStatus, int _iPid)
{
  m_ChatServerInfo[_iPos].iSeq     = _iPos;
  m_ChatServerInfo[_iPos].iPid     = _iPid;
  m_ChatServerInfo[_iPos].iMaxUser   = m_pServerInfo->GetDSMaxUser();
  m_ChatServerInfo[_iPos].iShmKey  = m_pServerInfo->GetShmKey();
  m_ChatServerInfo[_iPos].dHelloTime = CNPUtil::GetMicroTime();

  *_piSeq     = _iPos;
  *_piMaxUser   = m_pServerInfo->GetDSMaxUser();
  *_piShmKey    = m_pServerInfo->GetShmKey();
  *_piShmDSStatus = m_pServerInfo->GetShmDSStatus();

  // DS 에서 Hello 가 오면, pid, pos 를 세팅하고, DS status를 OFF 로 세팅한다.
  // DS 에서 thread 까지 생성이 끝날때, status 를 ON 한다.
  m_pShmDSStatus[_iPos].pid = _iPid;
  m_pShmDSStatus[_iPos].seq = _iPos;
  m_pShmDSStatus[_iPos].status = OFF;

  CNPLog::GetInstance().Log("Setting DS pid=(%d), seq=(%d), status=(%d)",
      m_pShmDSStatus[_iPos].pid
      ,m_pShmDSStatus[_iPos].seq
      ,m_pShmDSStatus[_iPos].status );
}

const int ChatManager::SetDS(int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const _piShmDSStatus, int _iPid)
{
  int i = 0;
  for(i = 0; i < m_pServerInfo->GetDNCnt(); i++)
  {
    if( m_ChatServerInfo[i].iPid <= 0)
    {
      SettingDS(i, _piSeq,_piMaxUser,_piShmKey, _piShmDSStatus, _iPid);
      break;
    }
    else
    {
      if(!IsAliveProcess(m_ChatServerInfo[i].iPid))
      {
        CNPLog::GetInstance().Log("SetDS slot(%d) (%d)=>(%d)", i, m_ChatServerInfo[i].iPid, _iPid);

#if 0
        CNPLog::GetInstance().Log("죽은 프로세스 이므로 다시 할당된다.slot(%d) (%d)=>(%d)",
                    i, m_ChatServerInfo[i].iPid, _iPid);
#endif

        SettingDS(i, _piSeq,_piMaxUser,_piShmKey, _piShmDSStatus, _iPid);
        break;
      }
    }
  }

  if(i == m_pServerInfo->GetDNCnt())
  {
    return -1;
  }

/*
  for(i = 0; i < m_pServerInfo->GetDNCnt(); i++)
  {
    CNPLog::GetInstance().Log("DS 현황.slot(%d) (%d),(%d)", i, m_ChatServerInfo[i].iPid, _iPid);
  }
*/

  return 0;
}

const int ChatManager::MessageBroadcast(const T_PACKET &_tPacket)
{
  Tcmd_CHAT_DS_DSM *pChatPacket = (Tcmd_CHAT_DS_DSM *)_tPacket.data;
  CNPLog::GetInstance().Log("ClientChatServer::MessageBroadcast Broadcast 요청 받음 from pid=(%d), length=(%d), message=(%s)", 
                              pChatPacket->iPid, _tPacket.header.length, pChatPacket->message);

  // for(int i = 0; i < m_pServerInfo->GetDNCnt(); i++)
  // {
  //   if( m_ChatServerInfo[i].iPid == pChatPacket->iPid) 
  //   {
  //     continue;
  //   }
  //   CNPLog::GetInstance().Log("ChatServer pid=(%d), requested pid=(%d)", m_ChatServerInfo[i].iPid, pChatPacket->iPid); 
  //   // ClientSocket *socket = static_cast<ClientSocket *>(*iter);
  //   // socket->Write((char *)&tSendPacket, PDUHEADERSIZE+tSendPacket.header.length);
  // }

  std::list<Client *>::iterator iter = m_lstChatServer.begin();
  while( iter != m_lstChatServer.end() )
  {
    ClientChatServer *chatServer = static_cast<ClientChatServer *>(*iter);
    if( chatServer->GetPid() != pChatPacket->iPid) 
    {
      CNPLog::GetInstance().Log("message broadcasting from manager to (%d)", chatServer->GetSocket()->GetFd());
      chatServer->GetSocket()->Write((char *)&_tPacket, PDUHEADERSIZE+_tPacket.header.length);
    }

    iter++;
  }
  return 0;
}

void ChatManager::DoFork(Process *_pProcess)
{
  /**
   *   IOMP Class Create
   */
#ifdef _FREEBSD
  m_pIOMP = new IOMP_KQUEUE(500000000); // 0.5 sec
#else
  m_pIOMP = new IOMP_EPoll(100);
#endif

  /**
   * Create Server Socket for ChatManager
   */
  Client *pServer = NULL;
  serverInfoMap &tmpMap = (serverInfoMap &)m_pServerInfo->GetPortMap();
  for(serverInfoMapItor itor = tmpMap.begin();
      itor != tmpMap.end(); ++itor)
  {
    ServerSocket *pServerSocket = new ServerSocket(itor->first);
    pServerSocket->SetReUse();
    if(pServerSocket->Bind(itor->second) < 0)
    {
      return;
    }
    if(pServerSocket->Listen(10) < 0)
    {
      return;
    }
    if(pServerSocket->SetNonBlock() < 0)
    {
      return;
    }
    pServerSocket->SetLinger();

    pServer = new ClientServer(pServerSocket);
    ///
    //   ServerSocket add to IOMP.
    //
#ifdef _FREEBSD
    m_pIOMP->AddClient(pServer, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ERROR);
#else
    m_pIOMP->AddClient(pServer, EPOLLIN);

#endif
  }

  /**
   * Create Server Socket for ChatServer
   */
  Client *pDNServer = NULL;
  ServerSocket *pServerSocket = new ServerSocket(SERVER_PORT);
  if(pServerSocket->Bind(m_pServerInfo->GetDNPort()) < 0)
  {
    return;
  }
  if(pServerSocket->Listen(1000) < 0)
  {
    return;
  }
  if(pServerSocket->SetNonBlock() < 0)
  {
    return;
  }

  pServerSocket->SetLinger();
  pDNServer = new ClientServer(pServerSocket);

  /**
   * create shm to check DS status
   */
  SharedMemory smDSStatus((key_t)m_pServerInfo->GetShmDSStatus(), 100 * sizeof(struct TDSStatus));
  if(!smDSStatus.IsStarted())
  {
    printf("DS Status SharedMemory ���� ���� \n");

    // 2. destroy
    SharedMemory sm((key_t)m_pServerInfo->GetShmDSStatus());
    sm.Destroy();
    return ;
  }
  m_pShmDSStatus = (struct TDSStatus *)smDSStatus.GetDataPoint();
  if(m_pShmDSStatus == NULL)
  {
    printf("Shm is NULL \n");
    return;
  }
  memset(m_pShmDSStatus, 0, 100 * sizeof(struct TDSStatus));
  // **

  ((ChatServer *)_pProcess)->SetServerSocket(pDNServer);

  CNPLog::GetInstance().BackupLogFile2((char *)m_pServerInfo->GetLogFileName());
  //SpawnProcessWithPipe(_pProcess, m_pServerInfo->GetDNCnt());
  SpawnProcess(_pProcess, m_pServerInfo->GetDNCnt());

  // Run Manager
  Run();
}

void ChatManager::Run()
{
  this->SetStarted(true);

  CNPUtil::GetIPConfig(&m_iMacAddr, &m_iIPAddr);

  if(CNPLog::GetInstance().SetFileName((char *)(m_pServerInfo->GetLogFileName())) )
  {
    Assert(false, "LogFile create error! ");
    return;
  }
  /*
     }
     */

  // alloc release slot
  //  m_pSlot = new ReleaseSlot(MAX_CLIENT);

  // Download Server info table
  m_ChatServerInfo = new Tcmd_HELLO_DSM_DS[m_pServerInfo->GetDNCnt()];
  memset(m_ChatServerInfo, 0, m_pServerInfo->GetDNCnt() * sizeof(Tcmd_HELLO_DSM_DS));

  struct in_addr laddr;
  laddr.s_addr = m_iIPAddr;

  struct ether_addr haddr;
  memcpy((void *)&haddr, (void *)&m_iMacAddr, 6);

  // Worker create.
  for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_WORKER); i++)
  {
    Thread *t = new ThreadWorker(this);
    ThreadManager::GetInstance()->Spawn(t);
    CNPLog::GetInstance().Log("In ChatManager WORKER Create (%p,%lu) ", t, t->GetThreadID());
  }

  // ThreadTic *tTic = new ThreadTic(this);
  // ThreadManager::GetInstance()->Spawn(tTic);
  // CNPLog::GetInstance().Log("In ChatManager ThreadTic Create (%p) ", tTic);

  while(1)
  {
    int iEventCount;

    WatchChildProcess();

    if((iEventCount = m_pIOMP->Polling()) <= 0)
    {
  
#ifdef _DEBUG
      CNPLog::GetInstance().Log("epoll_wait error errno=%d, strerror=(%s)", errno, strerror(errno));
#endif
      if(!GetStarted())
      {
        break;
      }
      continue;
    }

    for(int i = 0; i <iEventCount; ++i)
    {

#ifdef _FREEBSD
      struct kevent *tEvents = m_pIOMP->GetEventStructure();
      Client *pClient = static_cast<Client *>(tEvents[i].udata);
#else
      struct  epoll_event *tEvents= m_pIOMP->GetEventStructure();
      Client *pClient = (Client *)(tEvents[i].data.ptr);
#endif

      // ServerSocket event Check
      //if(pServer == pClient)
      if(pClient->GetType() == CLIENT_SERVER)
      {
        //m_pIOMP->ModifyFd(pServer);
#ifdef _DEBUG
        CNPLog::GetInstance().Log("ServerSocket Event !");
#endif
        Socket *pClientSocket;
        //if((pClientSocket = ((ServerSocket *)pServer->GetSocket())->Accept()) != NULL)
        //if((pClientSocket = ((ServerSocket *)pClient->GetSocket())->Accept()) != NULL)
        if((pClientSocket = static_cast<ServerSocket *>(pClient->GetSocket())->Accept()) != NULL)
        {
          pClientSocket->SetNonBlock();

          struct in_addr laddr;
          laddr.s_addr = static_cast<TcpSocket *>(pClientSocket)->GetClientIp();

          CNPLog::GetInstance().Log("ChatManager Accept ClientIp=(%s), conncnt=(%d), maxuser=(%d)",
              inet_ntoa(laddr), m_iConnCount, GetMaxUser());
          if(m_iConnCount >= GetMaxUser())
          {
            CNPLog::GetInstance().Log("Max User OverFlow! Force Kill CurrentUser=(%d), MaxUser=(%d)",
                m_iConnCount,
                GetMaxUser());
            delete pClientSocket;
            continue;
          }

          Client *pNewClient;
          if(static_cast<ServerSocket *>(pClient->GetSocket())->GetType() == SERVER_PORT)
          {
            pNewClient = new ClientChatServer(pClientSocket);
#ifdef _FREEBSD
            if(m_pIOMP->AddClient(pNewClient, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR) < 0)
#else
              if(m_pIOMP->AddClient(pNewClient, EPOLLIN|EPOLLET|EPOLLONESHOT) < 0)
#endif
              {
                delete pNewClient;
                continue;
              }

            pNewClient->SetState(STATE_WAIT);
            pNewClient->SetMainProcess(this);
            pthread_mutex_lock(&m_lockClient);
            m_lstChatServer.push_back((Client *)pNewClient);
            pthread_mutex_unlock(&m_lockClient);

            CNPLog::GetInstance().Log("NewClient(%p) fd ==>(%d) ",
                pNewClient,
                pNewClient->GetSocket()->GetFd());

            m_iConnCount++;
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
        CNPLog::GetInstance().Log("In KQUEUE disconnect (%p) (%d) errno=(%d)(%s)", pClient,
            pClient->GetSocket()->GetFd(), errno, strerror(errno));

        CloseClient(pClient);
        /*
           if( EventList[i].data == EBADF || tEvents[i].data == ENOENT )
           else
           */
      }

      if( tEvents[i].filter == EVFILT_READ )
      {
        CNPLog::GetInstance().Log("In KQUEUE READ (%p) (%d) ", pClient, pClient->GetSocket()->GetFd());
        pClient->SetAccessTime();
        PutWorkQueue(pClient);
      }

#else
      if(tEvents[i].events & (EPOLLERR | EPOLLHUP))
      {
        CNPLog::GetInstance().Log("In EPOLLERR or EPOLLHUP disconnect (%p) (%d) errno=(%d)(%s)", pClient,
            pClient->GetSocket()->GetFd(), errno, strerror(errno));

        CloseClient(pClient);
      }
      else if(tEvents[i].events & EPOLLIN)
      {
        //m_pIOMP->DelClient(pClient);
#ifdef _DEBUG
        CNPLog::GetInstance().Log("EPOLLIN Client %p, events=(%d)", pClient, tEvents[i].events);
#endif
        pClient->SetAccessTime();
        PutWorkQueue(pClient);
      }
      else if(tEvents[i].events & EPOLLOUT)
      {
        CNPLog::GetInstance().Log("EPOLLOUT Client %p, events=(%d)", pClient, tEvents[i].events);
      }
#endif

      continue;
    }
  }

  ThreadManager::GetInstance()->ReleaseInstance();
}

