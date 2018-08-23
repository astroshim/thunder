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
//#include "../include/ThreadQoS.h"

// #include "../include/ThreadQoSSelect.h"
// #ifndef _FREEBSD
// #include "../include/ThreadQoSEPoll.h"
// #endif
/*
#ifdef _FREEBSD
#include "../include/ThreadQoSSelect.h"
#else
#include "../include/ThreadQoSEPoll.h"
#endif
*/
#include "../include/ThreadManager.h"
#include "../include/ServerInfoDN.h"
#include "../include/NPUtil.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"
#include "../include/ChatUser.h"
//#include "../include/ClientServer.h"
#include "../include/MessageQ.h"

#include "../include/SharedMemory.h"
#include "../include/ReleaseSlot.h"

// #include <iostream>
// #include <sstream>
// #include <vector>

ChatServer::ChatServer()
  :m_pServerInfo(NULL)
  ,m_iConnCount(0)
  ,m_pDNServerSocket(NULL)
  ,m_pSendPipe(NULL)
  // ,m_pShm(NULL)
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
  //:m_pServerInfo(NULL)
  :m_iConnCount(0)
  ,m_pDNServerSocket(NULL)
  ,m_pSendPipe(NULL)
  // ,m_pShm(NULL)
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

  //CNPLog::GetInstance().Log("================ Create ChatServer m_pServerInfo=(%p)==", m_pServerInfo);
#ifdef _CLIENT_ARRAY
  for(int i = 0; i < MAX_CLIENT; i++)
  {
    m_arrClient[i] = new ChatUser(new ClientSocket(-1));
    m_arrClient[i]->SetState(STATE_CLOSED);
    m_arrClient[i]->SetMainProcess(this);

    CNPLog::GetInstance().Log("Client Array[%d] == [%p]", i, m_arrClient[i]);
  }
#endif
}

ChatServer::~ChatServer()
{
  //  CNPLog::GetInstance().Log("================ ~ChatServer() ");
  this->SetStarted(false);
  delete m_pIOMP;
  delete m_pServerInfo;
  delete m_pReceiveQueue;
  delete m_pSendQueue;
  delete m_pBroadcastQueue;
  delete m_pDNServerSocket;
  delete m_pSendPipe;
  // delete m_pShm;
  delete m_pShmKcps;
  delete m_pShmDSStatus;
  delete m_pShmD;
  delete m_pSlot;
  //  delete m_pMQ;

}

const int ChatServer::GetCurrentUserCount()
{
  /*
     int iCnt;

     pthread_mutex_lock(&m_lockClient);
     iCnt = m_lstClient.size();
     pthread_mutex_unlock(&m_lockClient);

     return iCnt;
     */

  return m_iConnCount;
}

const int ChatServer::GetMaxUser()
{
  return m_iMaxUser;
  //return m_pServerInfo->GetMaxUser();
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

// void ChatServer::BroadcastMessage(char *message, Client *const _pClient)
// {
//   // static_cast<ChatUser *>(_pClient)->SendCloseToMgr();
// }

// void ChatServer::PutBroadcastQueue(const void* const _pVoid)
void ChatServer::PutBroadcastQueue(char *message, Client *const _pClient)
{
  // Client *pNewClient;
  // pNewClient = new ChatUser(_pClientSocket);
  // pNewClient->SetMainProcess(this);

  // 새로운 class 생성하여 message 와 client 를 세팅해서 queue에 집어 넣자.
  BroadcastMessage *broadcastMessage = new BroadcastMessage(_pClient->GetSocket()->GetFd(), message);
  m_pBroadcastQueue->EnQueue(broadcastMessage);
}

const void* const ChatServer::GetBroadcastQueue()
{
  return m_pBroadcastQueue->DeQueue();
}

const char* const ChatServer::GetMRTGURL()
{
  return m_pServerInfo->GetMRTGURL();
}

// void ChatServer::SendStorageInfo()
// {
//   // Send to web about statistics
// }

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
  //CNPLog::GetInstance().Log("GetBandwidth ID---> (%c)", _chID);
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
    NegotiationWithManager(server, port);
  }

  return 0;
}

const int ChatServer::NegotiationWithManager(string server, int port)
{
  CNPLog::GetInstance().Log("Trying Connect to Mgr.. (%s)(%d)", server.c_str(), port);

  ClientSocket *pCSocket = new ClientSocket();
  if(pCSocket->Connect(server.c_str(), port) < 0)
  {
    CNPLog::GetInstance().Log("Error Connect to Mgr (%s)(%d)", server.c_str(), port);
    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;

    return -1;
  }

  // hello to mgr
  T_PACKET tHelloPacket ;
  Tcmd_HELLO_DS_DSM *sndbody = (Tcmd_HELLO_DS_DSM *)tHelloPacket.data;
  memset((char *)&tHelloPacket, 0x00, sizeof(T_PACKET));
  tHelloPacket.header.command = cmd_HELLO_DS_DSM;
  tHelloPacket.header.length = sizeof(Tcmd_HELLO_DS_DSM);

  sndbody->iPid = GetPid();

  if(pCSocket->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
    //if(static_cast<Socket *>(pCSocket)->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
    //if(((Socket *)(pCSocket))->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
  {
    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Send Fail (%s)(%d)", server.c_str(), port);

    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;
    return -1;
  }

  memset((char *)&tHelloPacket, 0x00, sizeof(T_PACKET));
  //if(((Socket *)(pCSocket))->Read((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DSM_DS)) < 0)
  //if(pCSocket->Read((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DSM_DS)) < 0)
  if(pCSocket->Recv((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DSM_DS)) < 0)
  {
    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Recv Fail (%s)(%d)", server.c_str(), port);

    pCSocket->Close();
    delete pCSocket;
    pCSocket = NULL;
    return -1;
  }

  m_pSendPipe = pCSocket;

  Tcmd_HELLO_DSM_DS *pRcvBody = (Tcmd_HELLO_DSM_DS *)tHelloPacket.data;
  CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Recv seq=(%d), shmKey=(%d), maxUser=(%d)",
      pRcvBody->iSeq, pRcvBody->iShmKey, pRcvBody->iMaxUser);

  //m_pServerInfo->SetMaxUser(pRcvBody->iMaxUser);
  SetSeq(pRcvBody->iSeq);
  SetShmKey(pRcvBody->iShmKey);
  m_iShmDSStatus = pRcvBody->iShmDSStatus;
  SetMaxUser(pRcvBody->iMaxUser);
  return 0;
}

void ChatServer::HealthCheckUsers()
{
  if(*m_pShmD == D_D)
  {
    SetStarted(false);
  }

#ifdef _CLIENT_ARRAY
  for(int i = 0; i < MAX_CLIENT; i++)
  {
    if(m_arrClient[i]->GetState() != STATE_CLOSED &&
        m_arrClient[i]->GetUserSeq() != -1)
    {
      Client *pClient = m_arrClient[i];
      if(CNPUtil::GetMicroTime() - pClient->GetAccessTime() > TIME_ALIVE)
      {
        CNPLog::GetInstance().Log("ChatServer::HealthCheckUsers Kill Client [%p] (%d) (%d)\n",
            pClient,
            m_arrClient[i]->GetState(),
            m_arrClient[i]->GetUserSeq());
        CloseClient(pClient->GetUserSeq());
      }
    }
  }
#else
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
          //((Socket *) (pClient->GetSocket()))->GetFd(),
          pClient->GetSocket()->GetFd(),
          CNPUtil::GetMicroTime()-pClient->GetAccessTime());


      iter = m_lstClient.erase( iter );

      m_pSlot->PutSlot(pClient->GetUserSeq());
      // memset(&(m_pShm[pClient->GetUserSeq()]), 0, sizeof(struct scoreboard_file));

      delete pClient;
      m_iConnCount--;
    }

    iter++;
  }
  pthread_mutex_unlock(&m_lockClient);
#endif
}

Client* const ChatServer::GetServerSocket()
{
  return m_pDNServerSocket;
}

void ChatServer::SetServerSocket(Client *_pClient)
{
  m_pDNServerSocket = _pClient;
}

ClientSocket* const ChatServer::GetSendPipeClient()
{
  return m_pSendPipe;
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

// #ifdef _ONESHOT
// const int ChatServer::AddQoS(Client* const _pClient, const unsigned int _uiEvents)
// {
//   return m_pTQoS->AddQoS(_pClient, _uiEvents);
// }
// #endif

#ifdef _CLIENT_ARRAY
void ChatServer::AcceptClient(const int _iClientFD)
{
  m_iConnCount++;

  pthread_mutex_lock(&m_lockClient);
  int iSlot = m_pSlot->GetFreeSlot();
  if(iSlot < 0)
  {
    pthread_mutex_unlock(&m_lockClient);
    return;
  }

  /*
  //struct sockaddr_in caddr;
  //int iLen = sizeof(struct sockaddr);
  //getsockname(_iClientFD, (struct sockaddr *)&caddr, (socklen_t *)&iLen);
  //CNPLog::GetInstance().Log("____________SetClientAddr = (%s)", inet_ntoa(caddr.sin_addr));
  */

  // client socket
  m_arrClient[iSlot]->SetSocketFd(_iClientFD);
  //  m_arrClient[iSlot]->SetMainProcess(this);
  m_arrClient[iSlot]->SetUserSeq(iSlot);

  m_arrClient[iSlot]->InitCircularBuffer();
  m_arrClient[iSlot]->SetState(STATE_WAIT);
  m_arrClient[iSlot]->GetSocket()->SetNonBlock();

  static_cast<TcpSocket *>(m_arrClient[iSlot]->GetSocket())->SetClientAddr();
  pthread_mutex_unlock(&m_lockClient);

#ifndef _ONESHOT
  if(m_pIOMP->AddClient(m_arrClient[iSlot], EPOLLIN) < 0)
#else
  #ifdef _FREEBSD
    if(m_pIOMP->AddClient(m_arrClient[iSlot], EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR) < 0)
  #else
      if(m_pIOMP->AddClient(m_arrClient[iSlot], EPOLLIN|EPOLLET|EPOLLONESHOT) < 0)
  #endif
#endif
      {
        CloseClient(iSlot);
        return;
      }
  CNPLog::GetInstance().Log("Accept Client slot=(%d)", iSlot);
  // CNPLog::GetInstance().Log("Accept Client userseq=(%d), (%p)", iSlot, &(m_pShm[iSlot]));
}

#else

void ChatServer::AcceptClient(Socket* const _pClientSocket)
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
        //    delete _pClientSocket;
        //    pthread_mutex_unlock(&m_lockClient);
        return;
      }
}
#endif

#ifdef _CLIENT_ARRAY
void ChatServer::CloseClient(const int _iSlot)
{
  pthread_mutex_lock(&m_lockClient);
  if( _iSlot == -1 )
  {
    CNPLog::GetInstance().Log("=====> ChatServer::CloseClient This Slot is already CLOSED!!! [%d]", _iSlot);
    return;
  }

#ifdef _FREEBSD
  m_pIOMP->DelClient(m_arrClient[_iSlot], EVFILT_READ);
#else
  m_pIOMP->DelClient(m_arrClient[_iSlot]);
#endif
  ChatUser *pClientUser = dynamic_cast<ChatUser *>(m_arrClient[_iSlot]);

  if(pClientUser != NULL)
  {
    pClientUser->SendCloseToMgr();
  }

  char pchTimeStr[10];
  memset(pchTimeStr, 0x00, sizeof(pchTimeStr));
  // CNPUtil::GetMicroTimeStr(pClientUser->GetFileSendStartTime(), pchTimeStr);

  // // added 09.11.13
  // CNPLog::GetInstance().Log("DisConnected %s %s %d %llu %.2f %s",
  //     (static_cast<TcpSocket *>(pClientUser->GetSocket()))->GetClientIpAddr()
  //     ,pClientUser->GetID()
  //     ,pClientUser->GetComCode()
  //     ,pClientUser->GetTotalSendSize()
  //     ,CNPUtil::GetMicroTime()- pClientUser->GetFileSendStartTime()
  //     ,pClientUser->GetFileName());

  m_arrClient[_iSlot]->SetState(STATE_CLOSED);
  m_arrClient[_iSlot]->GetSocket()->Close();

  m_arrClient[_iSlot]->FreePacket();
  // m_arrClient[_iSlot]->InitValiable();

  m_pSlot->PutSlot(_iSlot);
  // memset(&(m_pShm[_iSlot]), 0, sizeof(struct scoreboard_file));
  m_arrClient[_iSlot]->SetUserSeq(-1);
  pthread_mutex_unlock(&m_lockClient);
  m_iConnCount--;
  //CNPLog::GetInstance().Log("ChatServer::CloseClient2(%p) userseq=(%d)", pClientUser, _iSlot);
}

// void ChatServer::BroadcastMessage(char *message, Client *const _pClient)
// {
//   // static_cast<ChatUser *>(_pClient)->SendCloseToMgr();
// }


#else
void ChatServer::CloseClient(Client* const _pClient)
{
  #ifdef _FREEBSD
  m_pIOMP->DelClient(_pClient, EVFILT_READ);
  #else
  m_pIOMP->DelClient(_pClient);
  #endif

  char pchTimeStr[10];
  memset(pchTimeStr, 0x00, sizeof(pchTimeStr));

  // CNPUtil::GetMicroTimeStr(static_cast<ChatUser *>(_pClient)->GetFileSendStartTime(), pchTimeStr);

  // CNPLog::GetInstance().Log("ChatServer::CloseClient(%p) userseq=(%d), SDownTime=(%s), DownTime=(%f), DownSize=(%llu)",
  //     _pClient, _pClient->GetUserSeq(),
  //     pchTimeStr,
  //     CNPUtil::GetMicroTime()- static_cast<ChatUser *>(_pClient)->GetFileSendStartTime(),
  //     static_cast<ChatUser *>(_pClient)->GetTotalSendSize());

  if(_pClient->GetType() == CLIENT_USER)
  {
    static_cast<ChatUser *>(_pClient)->SendCloseToMgr();
  }

  pthread_mutex_lock(&m_lockClient);
  int iSlot = _pClient->GetUserSeq();
  m_lstClient.remove(_pClient);
  m_pSlot->PutSlot(iSlot);
  // memset(&(m_pShm[iSlot]), 0, sizeof(struct scoreboard_file));
  delete _pClient;
  pthread_mutex_unlock(&m_lockClient);
  m_iConnCount--;
}

void ChatServer::MessageBroadcast(BroadcastMessage *_message)
{
  // static_cast<ChatUser *>(_pClient)->SendCloseToMgr();
  pthread_mutex_lock(&m_lockClient);
  std::list<Client*>::iterator iter = m_lstClient.begin();
  while( iter != m_lstClient.end() )
  {
    Client *pClient = static_cast<Client *>(*iter);
    CNPLog::GetInstance().Log("ChatServer::MessageBroadCast client socket:(%d), message socket=(%d), message=(%s)",  
                                    pClient->GetSocket()->GetFd(),
                                    _message->GetSocketFD(),
                                    _message->GetMessage());

    iter++;
  }
  pthread_mutex_unlock(&m_lockClient);
}

#endif


// void ChatServer::WriteUserInfo(Client* const _pClient)
// {
//   int iSlot = _pClient->GetUserSeq();
//   ChatUser *pClient = static_cast<ChatUser *>(_pClient);
//   /*
//      ChatUser *pClient = dynamic_cast<ChatUser *>(_pClient);
//      if(pClient == NULL)
//      {
//      CNPLog::GetInstance().Log("WriteUserInfo slot=(%d) ERROR(%p)", iSlot, _pClient);
//      return;
//      }
//      */
//   //CNPLog::GetInstance().Log("WriteUserInfo slot=(%d) (%p)", iSlot, _pClient);

//   m_pShm[iSlot].cUse    = ON;
//   m_pShm[iSlot].comcode   = pClient->GetComCode();
//   m_pShm[iSlot].billno  = pClient->GetBillNo();

//   m_pShm[iSlot].kcps    = pClient->GetBandWidth();
//   if(m_pShm[iSlot].iFSize <= 0)
//   {
//     m_pShm[iSlot].iFSize  = pClient->GetFileSize();
//   }
//   m_pShm[iSlot].iDNSize   = pClient->GetTotalSendSize();

//   strcpy(m_pShm[iSlot].id, pClient->GetID());
//   strcpy(m_pShm[iSlot].filename, pClient->GetFileName());
//   m_pShm[iSlot].tAccessTime = CNPUtil::GetMicroTime();

// }


// void ChatServer::AddThroughput(const int _iIdx, const int _iSendSize)
// {
//   m_pShmKcps[0].kcps += _iSendSize;
//   m_pShmKcps[_iIdx].kcps += _iSendSize;
// }

// const int ChatServer::GetComCodeIdx(const int _iComCode)
// {
//   int i = 0;
//   for(i = 1; i < MAX_COMPANY; i++)
//   {
//     if(m_pShmKcps[i].comcode == _iComCode)
//     {
//       //CNPLog::GetInstance().Log("1.ChatServer::SetComCodeIdx=(%d) idx=(%d)",  _iComCode, i);
//       break;
//     }

//     if(m_pShmKcps[i].comcode == 0)
//     {
//       //CNPLog::GetInstance().Log("2.ChatServer::SetComCodeIdx=(%d) idx=(%d)",  _iComCode, i);
//       m_pShmKcps[i].comcode = _iComCode;
//       break;
//     }
//   }

//   return i;
// }

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
  //m_pIOMP = new IOMP_EPoll(20);
  m_pIOMP = new IOMP_EPoll(1000);
#endif

  // create log file. 
  char pchLogFileName[1024];
  memset(pchLogFileName, 0x00, sizeof(pchLogFileName));
  sprintf(pchLogFileName, "%s", m_pServerInfo->GetLogFileName());
  // sprintf(pchLogFileName, "%s_%d", m_pServerInfo->GetLogFileName(), getpid());
  //if(CNPLog::GetInstance().SetFileName(pchLogFileName));
  CNPLog::GetInstance().SetFileName(pchLogFileName);

  /*
     if(CNPLog::GetInstance().SetFileName(m_pServerInfo->GetLogFileName()));
     CNPLog::GetInstance().Log("ChatServer::Run forked GetNetworkByteOrder=(%d), pid=(%d)",
     CNPUtil::GetNetworkByteOrder(), GetPid());
     */
  sleep(3);
  // using socket
  if(ConnectToMgr() < 0)
  {
    return;
  }

  CNPLog::GetInstance().Log("SERVER_PORT_DNMGR = (%d)", m_pServerInfo->GetPort(SERVER_PORT_MGR));
  CNPLog::GetInstance().Log("SERVER_PORT = (%d)", m_pServerInfo->GetPort(SERVER_PORT));


  // create release slot
  m_pSlot = new ReleaseSlot(GetMaxUser());

  // // attach SharedMemory
  // SharedMemory sm((key_t)GetShmKey(), sizeof(struct scoreboard_file));
  // m_pShm = (struct scoreboard_file *)sm.GetDataPoint();
  // m_pShm = &(m_pShm[GetMaxUser() * GetSeq()]);


  // attach shm where ds status
  SharedMemory smDSStatus((key_t)m_iShmDSStatus, sizeof(struct TDSStatus));
  m_pShmDSStatus = (struct TDSStatus *)smDSStatus.GetDataPoint();
  m_pShmDSStatus = &(m_pShmDSStatus[GetSeq()]);

  // CNPLog::GetInstance().Log("ChatServer::Run pShm=(%p), StatusShmKey=(%d), skip=(%d), %d",
  //     m_pShm, m_iShmDSStatus, (sizeof(struct scoreboard_file) * (GetMaxUser() * GetSeq())), sizeof(struct scoreboard_file));

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
    printf("smKcps SharedMemory ���� ����! \n");

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

  ThreadTic *tTic = new ThreadTic(this);
  ThreadManager::GetInstance()->Spawn(tTic);

  m_pShmDSStatus->status = ON;
  CNPLog::GetInstance().Log("In ChatServer Status =======> pid=(%d), seq=(%d), status=(%d)",
      m_pShmDSStatus->pid
      ,m_pShmDSStatus->seq
      ,m_pShmDSStatus->status);

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

    //CNPLog::GetInstance().Log("epoll_event count=%d", iEventCount);
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
      //if(pServer == pClient)
      if(pClient->GetType() == CLIENT_SERVER)
      {
        //CNPLog::GetInstance().Log("ServerSocket Event =========");
        Socket *pClientSocket;
        if((pClientSocket = static_cast<ServerSocket *>(pClient->GetSocket())->Accept()) != NULL)
        {
          pClientSocket->SetNonBlock();

          /*
             CNPLog::GetInstance().Log("<Accept> ClientIp(%p)=(%s),CurrentUser=(%d),Max=(%d)" ,
             pClientSocket, ((TcpSocket *)pClientSocket)->GetClientIpAddr(), GetCurrentUserCount(), GetMaxUser());
             */
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

          //Client *pNewClient;
          //if(((ServerSocket *)pServer->GetSocket())->GetType() == SERVER_PORT)
          //if(((ServerSocket *)pClient->GetSocket())->GetType() == SERVER_PORT)
          if(static_cast<ServerSocket *>(pClient->GetSocket())->GetType() == SERVER_PORT)
          {
#ifndef _CLIENT_ARRAY
            AcceptClient(pClientSocket);
#endif
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
  #ifdef _CLIENT_ARRAY
        CloseClient(pClient->GetUserSeq());
  #else
        CloseClient(pClient);
  #endif
      }
      else
        //if(tEvents[i].events & EVFILT_READ)
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
        /*
           CNPLog::GetInstance().Log("In EPOLLERR or EPOLLHUP disconnect (%p) (%d) errno=(%d)(%s)",
           pClient, pClient->GetSocket()->GetFd(), errno, strerror(errno));
           */
        errno = 0;
  #ifdef _CLIENT_ARRAY
        CloseClient(pClient->GetUserSeq());
  #else
        CloseClient(pClient);
  #endif
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
