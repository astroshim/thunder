#include "../include/ClientChatServer.h"
#include "../include/NPUtil.h"
#include "../include/ChatManager.h"

ClientChatServer::ClientChatServer()
{
}

ClientChatServer::ClientChatServer(Socket *const _cSocket)
    : Client(_cSocket, CLIENT_USER)
{
}

ClientChatServer::~ClientChatServer()
{
}

void ClientChatServer::SetSeq(int _seq) { 
    m_iSeq = _seq;
}

void ClientChatServer::SetPid(int _pid) { 
    m_iPid = _pid;
}

const int ClientChatServer::GetSeq() {
    return m_iSeq;
}

const int ClientChatServer::GetPid() {
    return m_iPid;
}

void ClientChatServer::WorkDSHello(const T_PACKET &_tPacket)
{
    int iSeq = 0, iMaxUser = 0, iShmKey = 0, iShmDSStatus = 0;
    Tcmd_HELLO_DS_DSM *pClientBody = (Tcmd_HELLO_DS_DSM *)_tPacket.data;

    SetType(CLIENT_CHAT_SERVER);
    ChatManager *pManager = NULL;
    if ((pManager = dynamic_cast<ChatManager *>(m_pMainProcess)))
    {
        if (pManager->SetDS(&iSeq, &iMaxUser, &iShmKey, &iShmDSStatus, pClientBody->iPid) < 0)
        {
            CNPLog::GetInstance().Log("WorkDSHello DS OverFlow");
            return;
        }
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkDSHello(%p) pManager is NULL!! ", this);
    }

    SetUserSeq(iSeq);

    T_PACKET tPacket;
    Tcmd_HELLO_DSM_DS *sndbody = (Tcmd_HELLO_DSM_DS *)tPacket.data;
    memset((char *)&tPacket, 0x00, sizeof(T_PACKET));
    tPacket.header.command = cmd_HELLO_DSM_DS;
    tPacket.header.length = sizeof(Tcmd_HELLO_DSM_DS);

    sndbody->iSeq = iSeq;
    sndbody->iMaxUser = iMaxUser;
    sndbody->iShmKey = iShmKey;
    sndbody->iShmDSStatus = iShmDSStatus;

    CNPLog::GetInstance().Log("WorkDSHello seq=(%d), pid=(%d),maxUser=(%d), shmKey=(%d), shmDSStatusKey=(%d)",
                              iSeq, pClientBody->iPid, iMaxUser, iShmKey, iShmDSStatus);
    m_iPid = pClientBody->iPid;

    GetSocket()->Write((char *)&tPacket, PDUHEADERSIZE + tPacket.header.length);
}

// void ClientChatServer::WorkUserClose(const T_PACKET &_tPacket) //client close
// {
//     Tcmd_USER_CLOSE_DS_DSM *pClientBody = (Tcmd_USER_CLOSE_DS_DSM *)_tPacket.data;

//     Tcmd_USER_CLOSE_DS_DSM *pClosed = new Tcmd_USER_CLOSE_DS_DSM;
//     memcpy(pClosed, pClientBody, sizeof(Tcmd_USER_CLOSE_DS_DSM));

//     CNPLog::GetInstance().Log("Work UserClose pClosed=(%p), nBillNo=(%d), nDownSize=(%llu)",
//                               pClosed, pClosed->nBillNo, pClosed->nDownSize);
//     /*
//     CNPLog::GetInstance().Log("Work UserClose pClosed=(%p), nComCode=(%d), nBillNo=(%d), nDownSize=(%llu)",
//               pClosed, pClosed->nComCode, pClosed->nBillNo, pClosed->nDownSize);
// */
//     ChatManager *pManager = NULL;
//     if ((pManager = dynamic_cast<ChatManager *>(m_pMainProcess)))
//     {
//         pManager->PutClosedList(pClosed);
//     }
//     else
//     {
//         CNPLog::GetInstance().Log("Work UserClose(%p) pManager is NULL!! ", this);
//     }
// }

/**
  * DC <---> DSM
 */
// void ClientChatServer::WorkDSMHello(const T_PACKET &_tPacket)
// {
//     T_PACKET *pSendPacket = new T_PACKET;
//     memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
//     pSendPacket->header.command = cmd_HELLO_DSM_DC;
//     pSendPacket->header.length = 0;

//     //CNPLog::GetInstance().Log("ClientChatServer::DSM Hello(%d)", ((Socket *)(GetSocket()))->GetFd());

//     //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
//     GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE + pSendPacket->header.length);
//     delete pSendPacket;
// }

void ClientChatServer::WorkGetDSInfo(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    Tcmd_GET_DS_INFO_DSM_DC *sndbody = (Tcmd_GET_DS_INFO_DSM_DC *)pSendPacket->data;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_GET_DS_INFO_DSM_DC;
    pSendPacket->header.length = sizeof(Tcmd_GET_DS_INFO_DSM_DC);

    sndbody->iCmd = cmd_GET_DS_INFO_DSM_DC;

    sndbody->iPort = m_pMainProcess->GetDNServerPort();
    //sndbody->iIP    = htonl(INADDR_ANY);
    //sndbody->iIP    = ((ChatManager *)m_pMainProcess)->GetIPAddress();
    ChatManager *pManager = NULL;
    if ((pManager = dynamic_cast<ChatManager *>(m_pMainProcess)))
    {
        sndbody->iIP = pManager->GetIPAddress();
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkGetDSInfo(%p) pManager is NULL!! ", this);
    }

    CNPLog::GetInstance().Log("ClientChatServer::DSM GetDSInfo ip=(%lu)", sndbody->iIP);
    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE + pSendPacket->header.length);
    delete pSendPacket;
}

// void ClientChatServer::WorkGoodBye(const T_PACKET &_tPacket)
// {
//     T_PACKET *pSendPacket = new T_PACKET;
//     memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
//     pSendPacket->header.command = cmd_GOODBYE_DSM_DC;
//     pSendPacket->header.length = 0;

//     //CNPLog::GetInstance().Log("ClientChatServer::WorkGoodBye(%d)", ((Socket *)(GetSocket()))->GetFd());
//     //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
//     GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE + pSendPacket->header.length);
//     delete pSendPacket;
// }

void ClientChatServer::WorkDSMPing(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_HEARTBEAT_DSM_DC;
    pSendPacket->header.length = 0;

    CNPLog::GetInstance().Log("ClientChatServer::DSM Ping(%p)", this);
    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE + pSendPacket->header.length);
    delete pSendPacket;
}

void ClientChatServer::MessageBroadcast(const T_PACKET &_tPacket)
{
    Tcmd_CHAT_DS_DSM *pChatPacket = (Tcmd_CHAT_DS_DSM *)_tPacket.data;
    ChatManager *pManager = NULL;
    if ((pManager = dynamic_cast<ChatManager *>(m_pMainProcess)))
    {
        pManager->MessageBroadcast(_tPacket);
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkGetDSInfo(%p) pManager is NULL!! ", this);
    }
}

const int ClientChatServer::ExecuteCommand(Thread *_pThread)
{
    CNPLog::GetInstance().Log("manager 에서 메세지 받음");

    T_PACKET tPacket;
    PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();

    memset((char *)&tPacket, 0x00, sizeof(tPacket));
    if (Client::GetPacket((char *)&tPacket, pPacketHeader->length + PDUHEADERSIZE) < 0)
    {
        CNPLog::GetInstance().Log("In ClientChatServer::ExecuteCommand() GetPacketError!");
        return -1;
    }

#ifdef _DEBUG
    CNPLog::GetInstance().Log("ClientChatServer::ExecuteCommand(%p) command=(%d)", this, tPacket.header.command);
#endif

    switch (tPacket.header.command)
    {
    // DS -> DSM
    case cmd_CHAT_DS_DSM:
        MessageBroadcast(tPacket);
        break;

    // DS -> DSM
    case cmd_HELLO_DS_DSM:
        WorkDSHello(tPacket);
        break;
    // case cmd_USER_CLOSE_DS_DSM:
    //     WorkUserClose(tPacket);
    //     break;

    // // DC -> DSM
    // case cmd_HELLO_DC_DSM: // blank packet
    //     WorkDSMHello(tPacket);
    //     break;

    case cmd_GET_DS_INFO_DC_DSM:
        WorkGetDSInfo(tPacket);
        break;

    // case cmd_GOODBYE_DC_DSM: // blank packet
    //     WorkGoodBye(tPacket);
    //     break;

    case cmd_HEARTBEAT_DC_DSM:
        WorkDSMPing(tPacket);
        break;
        // ===>> DSM command

    default:
        //WorkDSMPing(tPacket);
        CNPLog::GetInstance().Log("ClientChatServer::ExecuteCommand UNKNOWN PDU TYPE(%p) (%d)", this,
                                  tPacket.header.command);
    }

    return 0;
}
