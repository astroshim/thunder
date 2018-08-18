#ifndef __DOWNLOADMANAGER_H
#define __DOWNLOADMANAGER_H

#include "./Process.h"
/*
#include "./Client.h"
#include "./IOMP.h"
#include "./IOMP_EPoll.h"
#include "./ServerSocket.h"
#include "./Properties.h"
#include "./ServerInfoDNMgr.h"

#include "./CircularQueue.h"

//#include "./ClientMember.h"
*/
#include "./Packet.h"

//#define DOWNLOADMANAGER_PROPERTIES "/home/sunbee/DownloadServer/DSMgr.properties"
#define DOWNLOADMANAGER_PROPERTIES "./DSMgr.properties"

class Client;
#ifdef _FREEBSD
class IOMP_KQUEUE;
#else
class IOMP_EPoll;
#endif
class ClientSocket;
class Properties;
class ServerInfoMgr;
class CircularQueue;
class ServerInfoDNMgr;
//class ReleaseSlot;

/*
#include <map>
typedef map<int, Client*> mapConnection;
typedef mapConnection::value_type connValType;
typedef mapConnection::iterator connItor;
*/

const unsigned int MAX_DN = 10;	
//const unsigned int MAX_CLIENT = 300;	
const unsigned int MAX_CLIENT = 2000;	
const unsigned int MAX_MAC_LEN = 20;

/*
// DSMgr 에서 관리하는 구조..
struct scoreboard_file {
	char cUse;
	int isupload;
	unsigned int comcode;
	unsigned int billno;

	unsigned int count;
	unsigned int kcps;
	uint64_t 	iFSize;
	uint64_t	iDNSize;
	char id[16];
	char filename[128];
	time_t      tAccessTime;      // Access time
};
*/

class DownloadManager : public Process
{
private:
	static const int TIME_ALIVE = 300; 	// 
	//static const int TIME_ALIVE = 600; 	// 
	static const int MAX_STATISTICS = 20000; 	// 

//	Client *m_pMS;				// member server
//	list <Client*> m_lstSMS;	// submember server

#ifdef _FREEBSD
	IOMP_KQUEUE *m_pIOMP;
#else
	IOMP_EPoll *m_pIOMP;
#endif

	int 	m_iConnCount;
	unsigned long long m_iMacAddr;
	unsigned long m_iIPAddr;
/*
	char 	m_pchMacAddr[MAX_MAC_LEN];
	char 	m_pchIPAddr[MAX_IP_LEN];
*/
//	char 	m_pchStatistics[MAX_STATISTICS];
	char 	*m_pchStatistics;

	/**
	*	DownloadManager Server Infomation
	*/
	ServerInfoDNMgr	*m_pServerInfo;
	CircularQueue 	*m_pWorkQueue;
	CircularQueue 	*m_pBBSQueue;

//	Client 			*m_pRecvPipe;
//	ReleaseSlot 	*m_pSlot;

	Tcmd_HELLO_DSM_DS 	*m_pDSInfo;			// DS info
	scoreboard_file		*m_pShm;
	TDSStatus			*m_pShmDSStatus;	// for ds status
	//pthread_mutex_t 	m_lockShm;       


	// DownloadServer
	list <Client*>   m_lstClient;
	pthread_mutex_t m_lockClient;       
    //mapConnection   m_mapConnectList;

	list <Tcmd_USER_CLOSE_DS_DSM*>   m_lstClosed;
	pthread_mutex_t m_lockClosed;       


/*
#ifdef _FREEBSD
	const bool	SetFDSet(IOMP_KQUEUE* const _pcIomp);
#else
	const bool	SetFDSet(IOMP_EPoll* const _pcIomp);
#endif
*/
	void 	DeleteAllMember();

	const int PingRoutine(Client* const _pClient, const int _iCommand);

	const int GetStatistics(
                struct scoreboard_file* const pSt,
                struct scoreboard_file** const _pNext,
                uint32_t _iPos,   
                uint32_t* const _piFCount,   // file count
                uint32_t* const _piUserCnt,  // id count
                uint32_t* const _piKcps,	// kcps
                uint32_t* const _piSameCnt);     
public:
	DownloadManager();
	DownloadManager(Properties& _cProperties);
	virtual ~DownloadManager();


	void 	Run();
    const int GetDNServerPort();
	const char* const GetMRTGURL();
	const char* const GetBBSURL();
	const char* const GetBBSAuth();
	const char* const GetLogFileName();

	void 	DoFork(Process *_pProcess);

	const int	GetMaxUser();
	const char*	const GetIPAddr();
//	const serverInfoMap& GetServerPortMap();
	const int GetCurrentUserCount();
	void HealthCheckUsers();
	void SendStorageInfo();
	void SendStorageInfoOld();

#ifdef _FREEBSD
	void AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags);
#else
	void AddEPoll(Client* const _pClient, const unsigned int _uiEvents);
#endif
	void UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents);
	void CloseClient(Client* const _pClient);

	void PutWorkQueue(const void* const _pVoid); 
	const void* const GetWorkQueue();

    void PutBBSQueue(const void* const _pVoid);
    const void* const GetBBSQueue();


	const unsigned int GetMacAddress();
	const unsigned int GetIPAddress();

	const int PostData(const char *_pData, const char *_pchURL);
	const int PostData(const char *_pData, const char *_pchURL, const char *_pchAuth);

	const int SetDS(int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const _piShmDSStatus, const int _iPid);
	void SettingDS(const int _iPos, int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const _piShmDSStatus, int _iPid);

	void PutClosedList(Tcmd_USER_CLOSE_DS_DSM* const _pClosedInfo);
	const uint64_t GetClientDownloadSize(const uint32_t _nComCode, const uint32_t _nBillNo);
	void HealthCheckClosedList();
};

#endif

