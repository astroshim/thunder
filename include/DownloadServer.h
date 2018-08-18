#ifndef __DOWNLOADSERVER_H
#define __DOWNLOADSERVER_H

#include "./Process.h"

//#include <map>

//#define DOWNLOADSERVER_PROPERTIES "/home/sunbee/DownloadServer/DS.properties"
#define DOWNLOADSERVER_PROPERTIES "./DS.properties"

class Socket;
class Client;
#ifdef _FREEBSD
class IOMP_KQUEUE;
#else
class IOMP_EPoll;
#endif
class ClientSocket;
class Properties;
class ServerInfoDN;
class CircularQueue;

class ReleaseSlot;
#ifdef _ONESHOT
class ThreadQoS;
#endif

#ifdef _CLIENT_ARRAY
// 090313 added
class ClientUserDN;
#endif

/*
typedef map<int, Client*> mapConnection;
typedef mapConnection::value_type connValType;
typedef mapConnection::iterator connItor;
*/

class DownloadServer : public Process
{
private:
	static const int TIME_ALIVE = 300; 	// 5��
	static const int MAX_COMPANY = 100;
#ifdef _CLIENT_ARRAY
	static const unsigned int MAX_CLIENT = 200;
#endif

	/**
	*	DownloadServer Server Infomation
	*/
	ServerInfoDN		*m_pServerInfo;

	int m_iConnCount;

	Client 				*m_pDNServerSocket;				//
	//Client			*m_pSendPipe;
	ClientSocket		*m_pSendPipe;
	scoreboard_file     *m_pShm;
	TStatistics			*m_pShmKcps;
	TDSStatus			*m_pShmDSStatus;	// for DS Status
	//unsigned long long  *m_pShmKcps;
	int  *m_pShmD;

#ifdef _FREEBSD
	IOMP_KQUEUE *m_pIOMP;
#else
	IOMP_EPoll *m_pIOMP;
#endif

	CircularQueue *m_pReceiveQueue;
	CircularQueue *m_pSendQueue;

//	Socket 		*pMgrSocket;
//	MessageQ 	*m_pMQ;

//    mapConnection   m_mapConnectList;
//    ClientUserDN   *m_lstClient;

#ifdef _CLIENT_ARRAY
	ClientUserDN* 	m_arrClient[MAX_CLIENT];
#else
    list <Client*>   	m_lstClient;
#endif
	pthread_mutex_t 	m_lockClient;

	int m_iSeq;
	int m_iMaxUser;
	int m_iShmKey;			// DSM ���� �������� key
	int m_iShmDSStatus;		// DSM ���� �������� key
	ReleaseSlot     *m_pSlot;
	//int *m_pSlot;
#ifdef _ONESHOT
	ThreadQoS *m_pTQoS;
#endif

	//const bool	SetFDSet(IOMP_EPoll* const _pcIomp);
public:
	DownloadServer();
	DownloadServer(Properties& _cProperties);
	virtual ~DownloadServer();

	void 	Run();
	Client* const 	GetServerSocket();
	const char* const GetMRTGURL();
	const char* const GetLogFileName();

	const int ConnectToMgr();

	const int	GetMaxUser();
	const int	GetShmKey();
	const int	GetSeq();

	void 	SetMaxUser(const int _iMaxUser);
	void 	SetShmKey(const int _iShmKey);
	void 	SetSeq(const int _iSeq);

	const char*	const GetIPAddr();
	const int GetCurrentUserCount();
	void HealthCheckUsers();
	void        SendStorageInfo();

#ifdef _FREEBSD
   	void AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags);
#else
	void AddEPoll(Client* const _pClient, const unsigned int _uiEvents);
#endif
	void UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents);
	void AcceptClient(Socket* const _pClientSocket);
	void CloseClient(Client* const _pClient);
#ifdef _CLIENT_ARRAY
	void AcceptClient(const int _iClientFD);
	void CloseClient(const int _iSlot);
#endif

	void WriteUserInfo(Client* const _pClient);
	// 20090527
	void AddThroughput(const int _iIdx, const int _iSendSize);

	Client* const GetClient(const unsigned int _uiSessionKey);

	void PutReceiveQueue(const void* const _pVoid);
	const void* const GetReceiveQueue();

	void PutSendQueue(const void* const _pVoid);
	const void* const GetSendQueue();

	const int GetDNServerPort();
	const int GetServerPort();
	void 	SetServerSocket(Client *_pClient);

	const char* const GetVolName();
	const char* const GetDirName();

	ClientSocket* const 	GetSendPipeClient();
	const uint64_t GetDownloadSize(const uint32_t _nBillNo);

	const unsigned int GetBandwidth(const char _chID);
#ifdef _ONESHOT
	const int AddQoS(Client* const _pClient, const unsigned int _uiEvents);
#endif
	void SetD();
	void SetComCodeIdx(ClientUserDN* const _pClient);
	const int GetComCodeIdx(const int _iComCode);

};

#endif

