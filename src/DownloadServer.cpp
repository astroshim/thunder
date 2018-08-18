/**
 * Author:	astrosim
*/
#include "../include/Client.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif
#include "../include/Properties.h"
#include "../include/CircularQueue.h"

#include "../include/DownloadServer.h"
#include "../include/ServerSocket.h"
#include "../include/Thread.h"
#include "../include/ThreadAcceptor.h"
#include "../include/ThreadReceiver.h"
#include "../include/ThreadSender.h"
#include "../include/ThreadTic.h"
//#include "../include/ThreadQoS.h"

#include "../include/ThreadQoSSelect.h"
#ifndef _FREEBSD
// #include "../include/ThreadQoSEPoll.h"
#endif
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
#include "../include/ClientUserDN.h"
//#include "../include/ClientServer.h"
#include "../include/MessageQ.h"

#include "../include/SharedMemory.h"
#include "../include/ReleaseSlot.h"

DownloadServer::DownloadServer()
					:m_pServerInfo(NULL)
					,m_iConnCount(0)
					,m_pDNServerSocket(NULL)
					,m_pSendPipe(NULL)
					,m_pShm(NULL)
					,m_pShmKcps(NULL)
					,m_pShmDSStatus(NULL)
					,m_pShmD(NULL)
					,m_pIOMP(NULL)
					,m_pReceiveQueue(NULL)
					,m_pSendQueue(NULL)
					,m_iSeq(-1)
					,m_iMaxUser(0)
					,m_iShmKey(0)
					,m_iShmDSStatus(0)
					,m_pSlot(NULL)
{
    this->SetStarted(false);
	pthread_mutex_init(&m_lockClient, NULL);
}

DownloadServer::DownloadServer(Properties& _cProperties)
					//:m_pServerInfo(NULL)
					:m_iConnCount(0)
					,m_pDNServerSocket(NULL)
					,m_pSendPipe(NULL)
					,m_pShm(NULL)
					,m_pShmKcps(NULL)
					,m_pShmDSStatus(NULL)
					,m_pShmD(NULL)
					,m_pIOMP(NULL)
					,m_pReceiveQueue(new CircularQueue())
					,m_pSendQueue(new CircularQueue())
					,m_iSeq(-1)
					,m_iMaxUser(0)
					,m_iShmKey(0)
					,m_iShmDSStatus(0)
					,m_pSlot(NULL)
{
    this->SetStarted(false);
	pthread_mutex_init(&m_lockClient, NULL);
	m_pServerInfo = new ServerInfoDN(_cProperties);
    //CNPLog::GetInstance().Log("================ Create DownloadServer m_pServerInfo=(%p)==", m_pServerInfo);
#ifdef _CLIENT_ARRAY
	for(int i = 0; i < MAX_CLIENT; i++)
	{
		m_arrClient[i] = new ClientUserDN(new ClientSocket(-1));
		m_arrClient[i]->SetState(STATE_CLOSED);
		m_arrClient[i]->SetMainProcess(this);
	}
#endif
}

DownloadServer::~DownloadServer()
{
//  CNPLog::GetInstance().Log("================ ~DownloadServer() ");
    this->SetStarted(false);
	delete m_pIOMP;
    delete m_pServerInfo;
	delete m_pReceiveQueue;
	delete m_pSendQueue;
	delete m_pDNServerSocket;
	delete m_pSendPipe;
	delete m_pShm;
	delete m_pShmKcps;
	delete m_pShmDSStatus;
	delete m_pShmD;
	delete m_pSlot;
//	delete m_pMQ;

}

const int DownloadServer::GetCurrentUserCount()
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

const int DownloadServer::GetMaxUser()
{
	return m_iMaxUser;
    //return m_pServerInfo->GetMaxUser();
}

const int DownloadServer::GetShmKey()
{
	return m_iShmKey;
}

const int DownloadServer::GetSeq()
{
	return m_iSeq;
}

void DownloadServer::SetSeq(const int _iSeq)
{
	m_iSeq = _iSeq;
}

void DownloadServer::SetMaxUser(const int _iMaxUser)
{
	m_iMaxUser = _iMaxUser;
}

void DownloadServer::SetShmKey(const int _iShmKey)
{
	m_iShmKey = _iShmKey;
}

const char* const DownloadServer::GetIPAddr()
{
    return m_pServerInfo->GetIPAddr();
}

void DownloadServer::PutReceiveQueue(const void* const _pVoid)
{
	m_pReceiveQueue->EnQueue(_pVoid);
}

const void* const DownloadServer::GetReceiveQueue()
{
	return m_pReceiveQueue->DeQueue();
}

void DownloadServer::PutSendQueue(const void* const _pVoid)
{
	m_pSendQueue->EnQueue(_pVoid);
}

const void* const DownloadServer::GetSendQueue()
{
	return m_pSendQueue->DeQueue();
}

const char* const DownloadServer::GetMRTGURL()
{
	return m_pServerInfo->GetMRTGURL();
}

void DownloadServer::SendStorageInfo()
{
	// Send to web about statistics
}

const int DownloadServer::GetServerPort()
{
	return m_pServerInfo->GetPort(SERVER_PORT);
}

const int DownloadServer::GetDNServerPort()
{
	return m_pServerInfo->GetPort(SERVER_PORT);
}

const char* const DownloadServer::GetVolName()
{
	return m_pServerInfo->GetVolName();
}

const char* const DownloadServer::GetDirName()
{
	return m_pServerInfo->GetDirName();
}

const char* const DownloadServer::GetLogFileName()
{
    return m_pServerInfo->GetLogFileName();
}

const unsigned int DownloadServer::GetBandwidth(const char _chID)
{
	//CNPLog::GetInstance().Log("GetBandwidth ID---> (%c)", _chID);
    return m_pServerInfo->GetBandwidth(_chID);
}


const int DownloadServer::ConnectToMgr()
{
	int iMgrPort = m_pServerInfo->GetPort(SERVER_PORT_MGR);
	CNPLog::GetInstance().Log("Trying Connect to Mgr (%s)(%d)",
								m_pServerInfo->GetIPAddr(), iMgrPort);

	//m_pSendPipe = new ClientSocket();
	//if(m_pSendPipe->NonBlockConnect(m_pServerInfo->GetIPAddr(), iMgrPort) < 0)
	ClientSocket *pCSocket = new ClientSocket();
	if(pCSocket->Connect(m_pServerInfo->GetIPAddr(), iMgrPort) < 0)
	{
	    CNPLog::GetInstance().Log("Error Connect to Mgr (%s)(%d)",
										m_pServerInfo->GetIPAddr(), iMgrPort);
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

//if (dynamic_cast<CDerived*>(p)) { : : }
/*
	if (dynamic_cast<Socket*>(pCSocket))
	{
	}
*/

	if(pCSocket->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
	//if(static_cast<Socket *>(pCSocket)->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
	//if(((Socket *)(pCSocket))->Write((char *)&tHelloPacket, PDUHEADERSIZE+sizeof(Tcmd_HELLO_DS_DSM)) < 0)
	{
	    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Send Fail (%s)(%d)",
										m_pServerInfo->GetIPAddr(), iMgrPort);

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
	    CNPLog::GetInstance().Log("In ConnectToMgr:: Hello Recv Fail (%s)(%d)",
										m_pServerInfo->GetIPAddr(), iMgrPort);

	    pCSocket->Close();
		delete pCSocket;
		pCSocket = NULL;
		return -1;
	}

	m_pSendPipe = pCSocket;
//	m_pSendPipe = new ClientUserDN(pMemberSocket, CLIENT_MEMBER);

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

void DownloadServer::HealthCheckUsers()
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
				CNPLog::GetInstance().Log("DownloadServer::HealthCheckUsers Kill Client [%p] (%d) (%d)\n",
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
            m_pIOMP->DelClient(pClient);

            CNPLog::GetInstance().Log("DownloadServer::HealthCheckUsers Kill Client [%p] fd=(%d)=(%f)\n",
                            pClient,
                            //((Socket *) (pClient->GetSocket()))->GetFd(),
                            pClient->GetSocket()->GetFd(),
                            CNPUtil::GetMicroTime()-pClient->GetAccessTime());


            iter = m_lstClient.erase( iter );

            m_pSlot->PutSlot(pClient->GetUserSeq());
            memset(&(m_pShm[pClient->GetUserSeq()]), 0, sizeof(struct scoreboard_file));

            delete pClient;
            m_iConnCount--;
        }

		iter++;
    }
	pthread_mutex_unlock(&m_lockClient);
#endif

/*
	std::list<Client*>::iterator pos, itrClientPrev;

	pos = m_lstClient.begin();
	while( pos != m_lstClient.end() )
	{
		itrClientPrev = pos++;
		Client *pClient = static_cast<Client *>(*itrClientPrev);

		if(CNPUtil::GetMicroTime()-pClient->GetAccessTime() > TIME_ALIVE)
		{
	        m_pIOMP->DelClient(pClient);

	        CNPLog::GetInstance().Log("DownloadServer::HealthCheckUsers Kill Client [%p] fd=(%d)=(%f)\n",
							pClient,
							//((Socket *) (pClient->GetSocket()))->GetFd(),
							pClient->GetSocket()->GetFd(),
							CNPUtil::GetMicroTime()-pClient->GetAccessTime());


	        m_lstClient.erase( itrClientPrev );

			m_pSlot->PutSlot(pClient->GetUserSeq());
			memset(&(m_pShm[pClient->GetUserSeq()]), 0, sizeof(struct scoreboard_file));

			delete pClient;
			m_iConnCount--;
	        //delete (Client *)*itrClientPrev;
		}
	}
*/
}

Client* const DownloadServer::GetServerSocket()
{
	return m_pDNServerSocket;
}

void DownloadServer::SetServerSocket(Client *_pClient)
{
	m_pDNServerSocket = _pClient;
}

ClientSocket* const DownloadServer::GetSendPipeClient()
{
    return m_pSendPipe;
}

void DownloadServer::UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
#ifdef _FREEBSD
#else
    m_pIOMP->ModifyFd(_pClient, _uiEvents);
#endif
}

#ifdef _FREEBSD
void DownloadServer::AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags)
{
    m_pIOMP->AddClient(_pClient, _filter, _usFlags);
}
#else
void DownloadServer::AddEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
    m_pIOMP->AddClient(_pClient, _uiEvents);
}
#endif

#ifdef _ONESHOT
const int DownloadServer::AddQoS(Client* const _pClient, const unsigned int _uiEvents)
{
	return m_pTQoS->AddQoS(_pClient, _uiEvents);
}
#endif

#ifdef _CLIENT_ARRAY
void DownloadServer::AcceptClient(const int _iClientFD)
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

	// client socket�� ����
	m_arrClient[iSlot]->SetSocketFd(_iClientFD);
//	m_arrClient[iSlot]->SetMainProcess(this);
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
   	//CNPLog::GetInstance().Log("Accept Client userseq=(%d), (%p)", iSlot, &(m_pShm[iSlot]));
}

#else

void DownloadServer::AcceptClient(Socket* const _pClientSocket)
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
    pNewClient = new ClientUserDN(_pClientSocket);
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
//		delete _pClientSocket;
//		pthread_mutex_unlock(&m_lockClient);
        return;
    }
}
#endif

#ifdef _CLIENT_ARRAY
void DownloadServer::CloseClient(const int _iSlot)
{
	pthread_mutex_lock(&m_lockClient);
    if( _iSlot == -1 )
    {
        CNPLog::GetInstance().Log("=====> DownloadServer::CloseClient This Slot is already CLOSED!!! [%d]", _iSlot);
        return;
    }

#ifdef _FREEBSD
	m_pIOMP->DelClient(m_arrClient[_iSlot], EVFILT_READ);
#else
	m_pIOMP->DelClient(m_arrClient[_iSlot]);
#endif
	ClientUserDN *pClientUser = dynamic_cast<ClientUserDN *>(m_arrClient[_iSlot]);

	if(pClientUser != NULL)
	{
		pClientUser->SendCloseToMgr();
	}

	char pchTimeStr[10];
	memset(pchTimeStr, 0x00, sizeof(pchTimeStr));
	CNPUtil::GetMicroTimeStr(pClientUser->GetFileSendStartTime(), pchTimeStr);

	// added 09.11.13
	CNPLog::GetInstance().Log("DisConnected %s %s %d %llu %.2f %s",
										(static_cast<TcpSocket *>(pClientUser->GetSocket()))->GetClientIpAddr()
										,pClientUser->GetID()
										,pClientUser->GetComCode()
										,pClientUser->GetTotalSendSize()
										,CNPUtil::GetMicroTime()- pClientUser->GetFileSendStartTime()
										,pClientUser->GetFileName());

	m_arrClient[_iSlot]->SetState(STATE_CLOSED);
	m_arrClient[_iSlot]->GetSocket()->Close();

	m_arrClient[_iSlot]->FreePacket();
	m_arrClient[_iSlot]->InitValiable();

	m_pSlot->PutSlot(_iSlot);
	memset(&(m_pShm[_iSlot]), 0, sizeof(struct scoreboard_file));
    m_arrClient[_iSlot]->SetUserSeq(-1);
	pthread_mutex_unlock(&m_lockClient);
	m_iConnCount--;
	//CNPLog::GetInstance().Log("DownloadServer::CloseClient2(%p) userseq=(%d)", pClientUser, _iSlot);
}
#else

void DownloadServer::CloseClient(Client* const _pClient)
{
	m_pIOMP->DelClient(_pClient);

	char pchTimeStr[10];
	memset(pchTimeStr, 0x00, sizeof(pchTimeStr));
	CNPUtil::GetMicroTimeStr(static_cast<ClientUserDN *>(_pClient)->GetFileSendStartTime(), pchTimeStr);

	CNPLog::GetInstance().Log("DownloadServer::CloseClient(%p) userseq=(%d), SDownTime=(%s), DownTime=(%f), DownSize=(%llu)",
					_pClient, _pClient->GetUserSeq(),
					pchTimeStr,
					CNPUtil::GetMicroTime()- static_cast<ClientUserDN *>(_pClient)->GetFileSendStartTime(),
					static_cast<ClientUserDN *>(_pClient)->GetTotalSendSize());

	if(_pClient->GetType() == CLIENT_USER)
	{
		static_cast<ClientUserDN *>(_pClient)->SendCloseToMgr();
	}
/*
	ClientUserDN *pClientUser = dynamic_cast<ClientUserDN *>(_pClient);
	if(pClientUser != NULL)
	{
		pClientUser->SendCloseToMgr();
		//((ClientUserDN *)_pClient)->SendCloseToMgr();
	}
*/

	pthread_mutex_lock(&m_lockClient);
	int iSlot = _pClient->GetUserSeq();
	m_lstClient.remove(_pClient);
	m_pSlot->PutSlot(iSlot);
	memset(&(m_pShm[iSlot]), 0, sizeof(struct scoreboard_file));
	delete _pClient;
	pthread_mutex_unlock(&m_lockClient);
	m_iConnCount--;
}
#endif

void DownloadServer::WriteUserInfo(Client* const _pClient)
{
	int iSlot = _pClient->GetUserSeq();
	ClientUserDN *pClient = static_cast<ClientUserDN *>(_pClient);
/*
	ClientUserDN *pClient = dynamic_cast<ClientUserDN *>(_pClient);
	if(pClient == NULL)
	{
		CNPLog::GetInstance().Log("WriteUserInfo slot=(%d) ERROR(%p)", iSlot, _pClient);
		return;
	}
*/
//CNPLog::GetInstance().Log("WriteUserInfo slot=(%d) (%p)", iSlot, _pClient);

	m_pShm[iSlot].cUse 		= ON;
	m_pShm[iSlot].comcode 	= pClient->GetComCode();
	m_pShm[iSlot].billno 	= pClient->GetBillNo();

	m_pShm[iSlot].kcps	 	= pClient->GetBandWidth();
	if(m_pShm[iSlot].iFSize <= 0)
	{
		m_pShm[iSlot].iFSize	= pClient->GetFileSize();
	}
	m_pShm[iSlot].iDNSize 	= pClient->GetTotalSendSize();

	strcpy(m_pShm[iSlot].id, pClient->GetID());
	strcpy(m_pShm[iSlot].filename, pClient->GetFileName());
	m_pShm[iSlot].tAccessTime	= CNPUtil::GetMicroTime();

}

/*
void DownloadServer::AddThroughput(const int _iSendSize)
{
	(*m_pShmKcps) += _iSendSize;
}
*/

void DownloadServer::AddThroughput(const int _iIdx, const int _iSendSize)
{
	// idx 0�� �� ���۷��̴�.
	m_pShmKcps[0].kcps += _iSendSize;
	m_pShmKcps[_iIdx].kcps += _iSendSize;
}


const int DownloadServer::GetComCodeIdx(const int _iComCode)
{
	int i = 0;
	for(i = 1; i < MAX_COMPANY; i++)
	{
		if(m_pShmKcps[i].comcode == _iComCode)
		{
//CNPLog::GetInstance().Log("1.DownloadServer::SetComCodeIdx=(%d) idx=(%d)",  _iComCode, i);
			break;
		}

		if(m_pShmKcps[i].comcode == 0)
		{
//CNPLog::GetInstance().Log("2.DownloadServer::SetComCodeIdx=(%d) idx=(%d)",  _iComCode, i);
			m_pShmKcps[i].comcode = _iComCode;
			break;
		}
	}

	return i;
}
/*
void DownloadServer::SetComCodeIdx(ClientUserDN* const _pClient)
{
	if(!_pClient)
	{
		return;
	}

	for(int i = 1; i < MAX_COMPANY; i++)
	{
		if(m_pShmKcps[i].comcode == _pClient->GetComCode())
		{
CNPLog::GetInstance().Log("1.DownloadServer::SetComCodeIdx=(%d) idx=(%d)",  m_pShmKcps[i].comcode, i);
			_pClient->SetComCodeIdx(i);
			break;
		}

		if(m_pShmKcps[i].comcode == 0)
		{
CNPLog::GetInstance().Log("2.DownloadServer::SetComCodeIdx=(%d) idx=(%d)",  m_pShmKcps[i].comcode, i);
			m_pShmKcps[i].comcode = _pClient->GetComCode();
			_pClient->SetComCodeIdx(i);
			break;
		}
	}
}
*/

void DownloadServer::SetD()
{
	(*m_pShmD) = D_D;
}

void DownloadServer::Run()
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

	// create log file.	: pid ���� �α׸� �����.
	char pchLogFileName[1024];
	memset(pchLogFileName, 0x00, sizeof(pchLogFileName));
	sprintf(pchLogFileName, "%s_%d", m_pServerInfo->GetLogFileName(), getpid());
    //if(CNPLog::GetInstance().SetFileName(pchLogFileName));
    CNPLog::GetInstance().SetFileName(pchLogFileName);

/*
	// �ð������� �α� ������ ���ܴ޶�...	==> Z����
    if(CNPLog::GetInstance().SetFileName(m_pServerInfo->GetLogFileName()));
	CNPLog::GetInstance().Log("DownloadServer::Run forked GetNetworkByteOrder=(%d), pid=(%d)",
								CNPUtil::GetNetworkByteOrder(), GetPid());
*/
	sleep(3);
	// using socket
    if(ConnectToMgr() < 0)
    {
        return;
    }

	// create release slot
	m_pSlot = new ReleaseSlot(GetMaxUser());

	// attach SharedMemory
    SharedMemory sm((key_t)GetShmKey(), sizeof(struct scoreboard_file));
    m_pShm = (struct scoreboard_file *)sm.GetDataPoint();
	m_pShm = &(m_pShm[GetMaxUser() * GetSeq()]);


	// attach shm where ds status
    SharedMemory smDSStatus((key_t)m_iShmDSStatus, sizeof(struct TDSStatus));
    m_pShmDSStatus = (struct TDSStatus *)smDSStatus.GetDataPoint();
	m_pShmDSStatus = &(m_pShmDSStatus[GetSeq()]);

	CNPLog::GetInstance().Log("DownloadServer::Run pShm=(%p), StatusShmKey=(%d), skip=(%d), %d",
									m_pShm, m_iShmDSStatus, (sizeof(struct scoreboard_file) * (GetMaxUser() * GetSeq())), sizeof(struct scoreboard_file));

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
	//*m_pShmD = 0;


	/*
	// for statistics 20090331
    // Create SharedMemory
    SharedMemory smKcps((key_t)m_pServerInfo->GetShmKey(), sizeof(unsigned long long));
    if(!smKcps.IsStarted())
    {
        printf("smKcps SharedMemory ���� ����! \n");
        return ;
    }

    m_pShmKcps = (unsigned long long *)smKcps.GetDataPoint();
    if(m_pShmKcps == NULL)
    {
        printf("smKcps is NULL \n");
        return;
    }
    //memset(m_pShmKcps, 0, sizeof(unsigned long long));
	*m_pShmKcps = 0;
	*/

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
		CNPLog::GetInstance().Log("In DownloadServer Sender Create (%p,%lu) ", t, t->GetThreadID());
	}

	for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_RECEIVER); i++)
	{
		Thread *t = new ThreadReceiver(this);
		ThreadManager::GetInstance()->Spawn(t);
		CNPLog::GetInstance().Log("In DownloadServer Receiver Create (%p,%lu) ", t, t->GetThreadID());
	}

	ThreadTic *tTic = new ThreadTic(this);
	ThreadManager::GetInstance()->Spawn(tTic);

#ifdef _ONESHOT
	#ifdef _FREEBSD
	m_pTQoS = new ThreadQoSSelect(this);
	#else	// linux
	m_pTQoS = new ThreadQoSEPoll(this);
	#endif
	ThreadManager::GetInstance()->Spawn(m_pTQoS);
#endif

	m_pShmDSStatus->status = ON;
	CNPLog::GetInstance().Log("In DownloadServer Status =======> pid=(%d), seq=(%d), status=(%d)",
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
				if( ((ClientUserDN*)pClient)->GetSendPacketCount() > 0 &&
					((ClientUserDN*)pClient)->GetSendTime() < CNPUtil::GetMicroTime())
				{
//CNPLog::GetInstance().Log("2. In Download Server =>>(%f), (%f)", ((ClientUserDN*)pClient)->GetSendTime(), CNPUtil::GetMicroTime());
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
