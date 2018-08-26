#include "../include/ChatUser.h"
#include "../include/NPUtil.h"
#include "../include/ChatServer.h"
#include "../include/ThreadReceiver.h"
#include "../include/BroadcastMessage.h"

ChatUser::ChatUser()
{
  memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
  m_eSendMode = SEND_ONLY_FILE;
  pthread_mutex_init(&m_mtLst, NULL);
}

ChatUser::ChatUser(Socket* const _cSocket)
  :Client(_cSocket, CLIENT_USER)
{
  memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
  m_eSendMode = SEND_ONLY_FILE;
  pthread_mutex_init(&m_mtLst, NULL);
}

ChatUser::~ChatUser()
{
  FreePacket();
}

void ChatUser::FreePacket()
{
  std::list<T_PACKET*>::iterator pos;
  pthread_mutex_lock(&m_mtLst);
  pos = m_lstSendPacket.begin();
  while( pos != m_lstSendPacket.end() )
  {
    delete *pos++;
  }
  pthread_mutex_unlock(&m_mtLst);
}

const int ChatUser::GetSendPacketCount()
{
  return m_lstSendPacket.size();
}

list<T_PACKET*>* const ChatUser::GetPacketList()
{
  return &m_lstSendPacket;
}

void ChatUser::AddPacketFront(T_PACKET *_pPacket)
{
  pthread_mutex_lock(&m_mtLst);
  m_lstSendPacket.push_front(_pPacket);
  pthread_mutex_unlock(&m_mtLst);
}

void ChatUser::RemovePacket(T_PACKET *_pPacket)
{
  pthread_mutex_lock(&m_mtLst);
  m_lstSendPacket.remove(_pPacket);
  pthread_mutex_unlock(&m_mtLst);
}

void ChatUser::SendPacket(const T_PACKET* _pPacket)
{
#ifdef _USE_SENDTHREAD
  pthread_mutex_lock(&m_mtLst);
  m_lstSendPacket.push_back(_pPacket);
  pthread_mutex_unlock(&m_mtLst);
#else
  // response directly
  //if(((Socket *)(GetSocket()))->Write((char *)_pPacket, PDUHEADERSIZE+_pPacket->header.length) < 0)
  //if(static_cast<Socket*>(GetSocket())->Write((char *)_pPacket, PDUHEADERSIZE+_pPacket->header.length) < 0)
  if(GetSocket()->Write((char *)_pPacket, PDUHEADERSIZE+_pPacket->header.length) < 0)
  {
    CNPLog::GetInstance().Log("In ChatUser Write Error (%p)", this);
  }
  delete _pPacket;
#endif
}

// void ChatUser::SendCloseToMgr()
// {
//   // send to Mgr close info
//   T_PACKET tSendPacket;
//   Tcmd_USER_CLOSE_DS_DSM *sndbody = (Tcmd_USER_CLOSE_DS_DSM *)tSendPacket.data;
//   tSendPacket.header.command  = cmd_USER_CLOSE_DS_DSM;
//   tSendPacket.header.length   = sizeof(Tcmd_USER_CLOSE_DS_DSM);

//   ChatServer *chatServer = NULL;
//   if((chatServer = dynamic_cast<ChatServer*>(m_pMainProcess)))
//   {
//     chatServer->GetSendPipeClient()->Write((char *)&tSendPacket,
//         PDUHEADERSIZE + tSendPacket.header.length);
//   }
//   else
//   {
//     CNPLog::GetInstance().Log("Work SendCloseToMgr(%p) chatServer is NULL!! ", this);
//   }
// }

void ChatUser::WorkHello(const T_PACKET &_tPacket)
{
  Tcmd_HELLO_DC_DS *pClientBody = (Tcmd_HELLO_DC_DS *)_tPacket.data ;

  memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
  m_tSendPacket.header.command = cmd_HELLO_DS_DC;
  m_tSendPacket.header.length  = 0;

  // m_tFileInfo.nComCode = pClientBody->iComCode;

  if(pClientBody->iComCode == D_D)
  {
    ChatServer *pServer = NULL;
    if((pServer = dynamic_cast<ChatServer*>(m_pMainProcess)))
    {
      pServer->SetD();
      //m_pMainProcess->SetSignalNo(D_D);
    }
  }

  ChatServer *pServer = NULL;
  if((pServer = dynamic_cast<ChatServer*>(m_pMainProcess)))
  {
    // m_iComCodeIdx = pServer->GetComCodeIdx(pClientBody->iComCode);
  }

  if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
  {
    CNPLog::GetInstance().Log("In ChatUser Write Error (%p)", this);
  }
}

void ChatUser::WorkPing(const T_PACKET &_tPacket)
{

  memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
  m_tSendPacket.header.command  = cmd_HEARTBEAT_DS_DC;
  m_tSendPacket.header.length   = 0;

  CNPLog::GetInstance().Log("ChatUser::WorkPing(%p)", this);
  if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
  {
    CNPLog::GetInstance().Log("In ChatUser Write Error (%p)", this);
  }
}

void ChatUser::WorkGoodBye(const T_PACKET &_tPacket)
{
  memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
  m_tSendPacket.header.command  = cmd_GOODBYE_DS_DC;
  m_tSendPacket.header.length   = 0;

  CNPLog::GetInstance().Log("ChatUser::WorkGoodBye(%p)", this);
  //if(((Socket *)(GetSocket()))->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
  //if(static_cast<Socket*>(GetSocket())->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
  if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
  {
    CNPLog::GetInstance().Log("In ChatUser Write Error (%p)", this);
  }
}

void ChatUser::MessageBroadcast(BroadcastMessage *message)
{
  ChatServer *chatServer = NULL;
  if((chatServer = dynamic_cast<ChatServer*>(m_pMainProcess)))
  {
    chatServer->PutBroadcastQueue(message, this);
    // chatServer->BroadcastMessage(message, this);
    // chatServer->GetSendPipeClient()->Write((char *)&tSendPacket, PDUHEADERSIZE + tSendPacket.header.length);
  }
  else
  {
    CNPLog::GetInstance().Log("BroadcastMessage(%p) chatServer is NULL!! ", this);
  }
}

const int ChatUser::ExecuteCommand(Thread *_pThread)
{
  /**
   * todo: 여기서 \n 을 검사해서 보내면 된다.
   *       부하가 문제 된다면, 일단 skip 하자
   */

  if (GetType() == CLIENT_CHAT_MANAGER)
  {
    CNPLog::GetInstance().Log("Broadcast 요청 (manager를 통해 전달해온 메세지) == client 에 broadcasting 만 하면 되는 메세지.");

    T_PACKET tPacket;
    PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();

    memset((char *)&tPacket, 0x00, sizeof(tPacket));
    if (Client::GetPacket((char *)&tPacket, pPacketHeader->length + PDUHEADERSIZE) < 0)
    {
        CNPLog::GetInstance().Log("In ClientChatServer::ExecuteCommand() GetPacketError!");
        return -1;
    }

    if (tPacket.header.command == cmd_CHAT_DS_DSM)
    {
      BroadcastMessage *broadcastMessage = new BroadcastMessage();
      Tcmd_CHAT_DS_DSM *pChatPacket = (Tcmd_CHAT_DS_DSM *)tPacket.data;

      CNPLog::GetInstance().Log("MESSAGE => (%d)(%s)", (pPacketHeader->length-4), pChatPacket->message);

      broadcastMessage->SetMessage(pChatPacket->message);
      broadcastMessage->SetMessageSize(pPacketHeader->length-4);
      broadcastMessage->SetSocketFd(GetSocket()->GetFd());
      broadcastMessage->SetMessageType(RELAYED_MESSAGE);

      CNPLog::GetInstance().Log("In ChatUser:: message (%d)(%s), buffedSize: %d", 
                                                            broadcastMessage->GetSocketFd(), 
                                                            broadcastMessage->GetMessage(), 
                                                            broadcastMessage->GetMessageSize());
      MessageBroadcast(broadcastMessage);
    }


    /*
    switch(tPacket.header.command)
    {
      // DC -> DS
      case cmd_CHAT_DS_DSM :
        BroadcastMessage *broadcastMessage = new BroadcastMessage();
        Tcmd_CHAT_DS_DSM *pChatPacket = (Tcmd_CHAT_DS_DSM *)tPacket.data;

        CNPLog::GetInstance().Log("MESSAGE => (%d)(%s)", (pPacketHeader->length-4), pChatPacket->message);

        broadcastMessage->SetMessage(pChatPacket->message);
        broadcastMessage->SetMessageSize(pPacketHeader->length-4);
        broadcastMessage->SetSocketFd(GetSocket()->GetFd());
        broadcastMessage->SetMessageType(RELAYED_MESSAGE);

        CNPLog::GetInstance().Log("In ChatUser:: message (%d)(%s), buffedSize: %d", 
                                                              broadcastMessage->GetSocketFd(), 
                                                              broadcastMessage->GetMessage(), 
                                                              broadcastMessage->GetMessageSize());
        MessageBroadcast(broadcastMessage);

        break;

      default :
        CNPLog::GetInstance().Log("UNKNOWN PDU TYPE(%p), (%d)", this, tPacket.header.command);
    }
    */



/*
    BroadcastMessage *broadcastMessage = new BroadcastMessage();
    broadcastMessage->SetMessageSize(m_cCBuff.GetUsedSize());
    if(Client::GetPacket((char *)broadcastMessage->GetMessage(), broadcastMessage->GetMessageSize()) < 0)
    {
      CNPLog::GetInstance().Log("In ChatUser::ExecuteCommand() GetPacketError! ");
      return -1;
    }
    broadcastMessage->SetSocketFd(GetSocket()->GetFd());
    broadcastMessage->SetMessageType(RELAYED_MESSAGE);

    CNPLog::GetInstance().Log("In ChatUser:: message (%d)(%s), buffedSize: %d", 
                                                          broadcastMessage->GetSocketFd(), 
                                                          broadcastMessage->GetMessage(), 
                                                          broadcastMessage->GetMessageSize());
    MessageBroadcast(broadcastMessage);
    */

    return 0;
  }


#ifdef _DEBUG
  ChatServer *pServer = dynamic_cast<ChatServer*>(m_pMainProcess);
  CNPLog::GetInstance().Log("ChatUser::ExecuteCommand pServer=(%p)", pServer);
#endif

  BroadcastMessage *broadcastMessage = new BroadcastMessage();
  // m_pBroadcastQueue->EnQueue(broadcastMessage);

  broadcastMessage->SetMessageSize(m_cCBuff.GetUsedSize());
  if(Client::GetPacket((char *)broadcastMessage->GetMessage(), broadcastMessage->GetMessageSize()) < 0)
  {
    CNPLog::GetInstance().Log("In ChatUser::ExecuteCommand() GetPacketError! ");
    return -1;
  }

  broadcastMessage->SetSocketFd(GetSocket()->GetFd());
  // broadcastMessage.SetMessage(message);

  // if (GetType() == CLIENT_CHAT_MANAGER)
  // {
  //   CNPLog::GetInstance().Log("Broadcast 요청 (manager를 통해 전달해온 메세지) == client 에 broadcasting 만 하면 되는 메세지.");
  //   broadcastMessage->SetMessageType(RELAYED_MESSAGE);
  // }

  CNPLog::GetInstance().Log("In ChatUser:: message (%d)(%s), buffedSize: %d", broadcastMessage->GetSocketFd(), broadcastMessage->GetMessage(), broadcastMessage->GetMessageSize());
  MessageBroadcast(broadcastMessage);


  // int buffedSize = m_cCBuff.GetUsedSize();
  // char message[buffedSize+1];
  // message[buffedSize] = 0x00;
  // // const unsigned char* message = m_cCBuff.GetHeaderPoint();

  // if(Client::GetPacket((char *)&message, buffedSize) < 0)
  // {
  //   CNPLog::GetInstance().Log("In ChatUser::ExecuteCommand() GetPacketError! ");
  //   return -1;
  // }
  // CNPLog::GetInstance().Log("In ChatUser:: message (%s), buffedSize: %d", message, buffedSize);

  // BroadcastMessage(message);
  // return 0;




//   /*
//      int iCommand   = CNPUtil::Get2Byte((unsigned char *)m_cCBuff.GetHeaderPoint(), 0);
//      int iPacketLen = CNPUtil::Get2Byte((unsigned char *)m_cCBuff.GetHeaderPoint()+COMMAND_SIZE, 0);
//      */
//   PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();

//   memset((char *)&tPacket, 0x00, sizeof(tPacket));
//   if(Client::GetPacket((char *)&tPacket, pPacketHeader->length + PDUHEADERSIZE) < 0)
//   {
//     CNPLog::GetInstance().Log("In ChatUser::ExecuteCommand() GetPacketError! ");
//     return -1;
//   }

// #ifdef _DEBUG
//   CNPLog::GetInstance().Log("In ChatUser::ExecuteCommand (%p) command=(%d)", this,
//       tPacket.header.command);
// #endif

//   switch(tPacket.header.command)
//   {
//     // DC -> DS
//     case cmd_HELLO_DC_DS:
//       WorkHello(tPacket);
//       break;

//       // 1. C => S (ComCode)
//     case cmd_HELLO_COMCODE_DC_DS:     // client�� cmd_HELLO_DC_DS �� ������ �ʰ�, cmd_HELLO_COMCODE_DC_DS �� �����±�.
//       WorkHello(tPacket);
//       break;

//     case cmd_HELLO_CODES_DC_DS:
//       WorkHelloCDN(tPacket);
//       break;

//     case cmd_GET_FILE_EX_DC_DS:
//       return WorkGetCDNFile(tPacket);
//       break;

//       // 2. C => S (pchID, pchFileName, nFSize)
//     case cmd_GET_FSIZE_DC_DS:
//       SetSendMode(SEND_ONLY_FILE);
//       WorkGetFileSize(tPacket);
//       break;

//     case cmd_GET_FSIZE_DC_DS2:
//       SetSendMode(SEND_WIDTH_HEADER);
//       WorkGetFileSize(tPacket);
//       break;

//       // 3. C => S (pchID, pchFileName, nOffset, nDownSize, nBillNo)
//     case cmd_GET_FILE_DC_DS:
// #ifndef _ONESHOT
//       WorkGetFile(tPacket);
// #else
//       return WorkGetFile(tPacket);
// #endif
//       break;

//     case cmd_HEARTBEAT_DC_DS:
//       WorkPing(tPacket);
//       break;

//     case cmd_GOODBYE_DC_DS:
//       WorkGoodBye(tPacket);
//       break;

//       // ===>> DS command

//     default :
//       CNPLog::GetInstance().Log("UNKNOWN PDU TYPE(%p), (%d)", this, tPacket.header.command);
//   }

  // return 0;
}

