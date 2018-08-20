#include "../include/ClientDN.h"
#include "../include/NPUtil.h"
//#include "../include/DownloadServer.h"
#include "../include/DownloadManager.h"

//#include "../include/Socket.h"

ClientDN::ClientDN()
          :m_iPort(0)
{
}

ClientDN::ClientDN(Socket* const _cSocket)
          :Client(_cSocket, CLIENT_USER),
          //:Client(_cSocket, CLIENT_DN),
          m_iPort(0)
{
}

ClientDN::~ClientDN()
{
}

/**
*   DS ���� DSM ���� ������ �� �ҷ�����.
    DSM Event!
void ClientDN::WorkInformFile(const T_PACKET &_tPacket)
{
    Tcmd_INFORM_FILE *pClientBody = (Tcmd_INFORM_FILE *)_tPacket.data ;
//    ((DownloadManager *)m_pMainProcess)->SetFileInfo(pClientBody);
}

void ClientDN::WorkDownSize(const T_PACKET &_tPacket)
{
    Tcmd_DOWNLOAD_SIZE *pClientBody = (Tcmd_DOWNLOAD_SIZE *)_tPacket.data ;
//   ((DownloadManager *)m_pMainProcess)->SetFileSize(pClientBody);
}
*/

void ClientDN::WorkDSHello(const T_PACKET &_tPacket)
{
  int iSeq, iMaxUser, iShmKey, iShmDSStatus;
    Tcmd_HELLO_DS_DSM *pClientBody = (Tcmd_HELLO_DS_DSM *)_tPacket.data ;

  SetType(CLIENT_DN);
    DownloadManager *pManager = NULL;
    if((pManager = dynamic_cast<DownloadManager*>(m_pMainProcess)))
    {
      if(pManager->SetDS(&iSeq, &iMaxUser, &iShmKey, &iShmDSStatus, pClientBody->iPid) < 0)
    {
      CNPLog::GetInstance().Log("WorkDSHello DS OverFlow");
      return;
    }
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkDSHello(%p) pManager is NULL!! ", this);
    }
/*
    //if(((DownloadManager *)m_pMainProcess)->SetDS(&iSeq, &iMaxUser, &iShmKey, pClientBody->iPid) < 0)
    if(static_cast<DownloadManager *>(m_pMainProcess)->SetDS(&iSeq, &iMaxUser, &iShmKey, pClientBody->iPid) < 0)
  {
      CNPLog::GetInstance().Log("WorkDSHello DS OverFlow");
    exit(1);
    return;
  }
*/

  SetUserSeq(iSeq);

    T_PACKET tPacket;
    Tcmd_HELLO_DSM_DS *sndbody     = (Tcmd_HELLO_DSM_DS *)tPacket.data;
    memset((char *)&tPacket, 0x00, sizeof(T_PACKET));
    tPacket.header.command  = cmd_HELLO_DSM_DS;
    tPacket.header.length   = sizeof(Tcmd_HELLO_DSM_DS);

  sndbody->iSeq     = iSeq;
  sndbody->iMaxUser   = iMaxUser;
  sndbody->iShmKey  = iShmKey;
  sndbody->iShmDSStatus   = iShmDSStatus;

    CNPLog::GetInstance().Log("WorkDSHello seq=(%d), pid=(%d),maxUser=(%d), shmKey=(%d), shmDSStatusKey=(%d)",
              iSeq, pClientBody->iPid, iMaxUser, iShmKey, iShmDSStatus);
    //((Socket *)(GetSocket()))->Write((char *)&tPacket, PDUHEADERSIZE+tPacket.header.length);
    GetSocket()->Write((char *)&tPacket, PDUHEADERSIZE+tPacket.header.length);
}

void ClientDN::WorkUserClose(const T_PACKET &_tPacket)
{
    Tcmd_USER_CLOSE_DS_DSM *pClientBody = (Tcmd_USER_CLOSE_DS_DSM *)_tPacket.data ;

    Tcmd_USER_CLOSE_DS_DSM *pClosed = new Tcmd_USER_CLOSE_DS_DSM;
  memcpy(pClosed, pClientBody, sizeof(Tcmd_USER_CLOSE_DS_DSM));

    CNPLog::GetInstance().Log("Work UserClose pClosed=(%p), nBillNo=(%d), nDownSize=(%llu)",
              pClosed, pClosed->nBillNo, pClosed->nDownSize);
/*
    CNPLog::GetInstance().Log("Work UserClose pClosed=(%p), nComCode=(%d), nBillNo=(%d), nDownSize=(%llu)",
              pClosed, pClosed->nComCode, pClosed->nBillNo, pClosed->nDownSize);
*/

  DownloadManager *pManager = NULL;
  if((pManager = dynamic_cast<DownloadManager*>(m_pMainProcess)))
  {
      pManager->PutClosedList(pClosed);
  }
  else
  {
      CNPLog::GetInstance().Log("Work UserClose(%p) pManager is NULL!! ", this);
  }
}

/**
*   BBS <---> DSM
*/
void ClientDN::WorkDownloadFinishBBS(const T_PACKET &_tPacket)
{
    Tcmd_BBS_DS_DOWNFINISH_REQ *pClientBody = (Tcmd_BBS_DS_DOWNFINISH_REQ *)_tPacket.data ;

    T_PACKET *pSendPacket = new T_PACKET;
    Tcmd_BBS_DS_DOWNFINISH_RES *sndbody     = (Tcmd_BBS_DS_DOWNFINISH_RES *)pSendPacket->data;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_BBS_DS_DOWNFINISH_RES;
    pSendPacket->header.length  = sizeof(Tcmd_BBS_DS_DOWNFINISH_RES);

    // billno + comcode �� Client�� ã�ƾ� �Ѵ�.
    sndbody->nRet       = 1;    // 1: success, 2 : not exist client

    sndbody->nComCode   = pClientBody->nComCode;
    sndbody->nBillNo    = pClientBody->nBillNo;

    // init m_tDsInfo
    DownloadManager *pManager = NULL;
    if((pManager = dynamic_cast<DownloadManager*>(m_pMainProcess)))
    {
      sndbody->nDownSize  =
                pManager->GetClientDownloadSize(pClientBody->nComCode, pClientBody->nBillNo);
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkDownloadFinishBBS(%p) pManager is NULL!! ", this);
    }

/*
    sndbody->nDownSize  =
                ((DownloadManager *)m_pMainProcess)->GetClientDownloadSize(pClientBody->nComCode, pClientBody->nBillNo);
*/
    if(sndbody->nDownSize <= 0)
    {
        sndbody->nRet       = 2;    // 1: success, 2 : not exist client
    }

    CNPLog::GetInstance().Log("ClientDN::WorkDownloadFinishBBS ComCode=(%d), BillNo=(%d), DownSize=(%llu)",
                    pClientBody->nComCode, pClientBody->nBillNo, sndbody->nDownSize);

    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    delete pSendPacket;
}

/**
  * DC <---> DSM
 */
void ClientDN::WorkDSMHello(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_HELLO_DSM_DC;
    pSendPacket->header.length  = 0;

    //CNPLog::GetInstance().Log("ClientDN::DSM Hello(%d)", ((Socket *)(GetSocket()))->GetFd());

    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    delete pSendPacket;
}

void ClientDN::WorkGetDSInfo(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    Tcmd_GET_DS_INFO_DSM_DC *sndbody     = (Tcmd_GET_DS_INFO_DSM_DC *)pSendPacket->data;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_GET_DS_INFO_DSM_DC;
    pSendPacket->header.length  = sizeof(Tcmd_GET_DS_INFO_DSM_DC);

    sndbody->iCmd   = cmd_GET_DS_INFO_DSM_DC;

    sndbody->iPort  = m_pMainProcess->GetDNServerPort();
    //sndbody->iIP    = htonl(INADDR_ANY);
    //sndbody->iIP    = ((DownloadManager *)m_pMainProcess)->GetIPAddress();
    DownloadManager *pManager = NULL;
    if((pManager = dynamic_cast<DownloadManager*>(m_pMainProcess)))
    {
      sndbody->iIP    = pManager->GetIPAddress();
    }
    else
    {
        CNPLog::GetInstance().Log("Work WorkGetDSInfo(%p) pManager is NULL!! ", this);
    }

CNPLog::GetInstance().Log("ClientDN::DSM GetDSInfo ip=(%lu)", sndbody->iIP);
    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    delete pSendPacket;
}

void ClientDN::WorkGoodBye(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command     = cmd_GOODBYE_DSM_DC;
    pSendPacket->header.length  = 0;

    //CNPLog::GetInstance().Log("ClientDN::WorkGoodBye(%d)", ((Socket *)(GetSocket()))->GetFd());
    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    delete pSendPacket;
}

void ClientDN::WorkDSMPing(const T_PACKET &_tPacket)
{
    T_PACKET *pSendPacket = new T_PACKET;
    memset((char *)pSendPacket, 0x00, sizeof(T_PACKET));
    pSendPacket->header.command = cmd_HEARTBEAT_DSM_DC;
    pSendPacket->header.length  = 0;

    CNPLog::GetInstance().Log("ClientDN::DSM Ping(%p)", this);
    //((Socket *)(GetSocket()))->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE+pSendPacket->header.length);
    delete pSendPacket;
}

const int ClientDN::ExecuteCommand(Thread *_pThread)
{
  T_PACKET tPacket;
    PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();

  memset((char *)&tPacket, 0x00, sizeof(tPacket));
  if(Client::GetPacket((char *)&tPacket, pPacketHeader->length + PDUHEADERSIZE) < 0)
  {
    CNPLog::GetInstance().Log("In ClientDN::ExecuteCommand() GetPacketError!");
    return -1;
  }

#ifdef _DEBUG
  CNPLog::GetInstance().Log("ClientDN::ExecuteCommand(%p) command=(%d)", this, tPacket.header.command);
#endif

    switch(tPacket.header.command)
    {
    // DS -> DSM
    case cmd_HELLO_DS_DSM:
        WorkDSHello(tPacket);
    break;
    case cmd_USER_CLOSE_DS_DSM:
        WorkUserClose(tPacket);
    break;
/*
    case cmd_INFORM_FILE:
        WorkInformFile(tPacket);
        break;
    case cmd_DOWNLOAD_SIZE:
        WorkDownSize(tPacket);
        break;
*/

    // BBS -> DSM
  // request filesize of client getted.
    case cmd_BBS_DS_DOWNFINISH_REQ:
        WorkDownloadFinishBBS(tPacket);
        break;

    // DC -> DSM
    case cmd_HELLO_DC_DSM:   // blank packet
        WorkDSMHello(tPacket);
        break;

    case cmd_GET_DS_INFO_DC_DSM:
        WorkGetDSInfo(tPacket);
        break;

    case cmd_GOODBYE_DC_DSM:    // blank packet
        WorkGoodBye(tPacket);
        break;

    case cmd_HEARTBEAT_DC_DSM:
        WorkDSMPing(tPacket);
        break;
    // ===>> DSM command

    default :
        //WorkDSMPing(tPacket);
        CNPLog::GetInstance().Log("ClientDN::ExecuteCommand UNKNOWN PDU TYPE(%p) (%d)", this,
        tPacket.header.command);
    }

  return 0;
}

