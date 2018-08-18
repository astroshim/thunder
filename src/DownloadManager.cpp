/**
 * Author:	astrosim
*/
#include "../include/DownloadManager.h"
#include "../include/DownloadServer.h"
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
#include "../include/ClientDN.h"

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

DownloadManager::DownloadManager()
					:m_pIOMP(NULL)
					,m_iConnCount(0)
					,m_iMacAddr(0)
					,m_iIPAddr(0)
					,m_pServerInfo(NULL)
					,m_pWorkQueue(NULL)
				  ,m_pDSInfo(NULL)
					,m_pShm(NULL)
					,m_pShmDSStatus(NULL)
{
    this->SetStarted(false);
	pthread_mutex_init(&m_lockClient, NULL);
	pthread_mutex_init(&m_lockClosed, NULL);
	m_pchStatistics = new char[MAX_STATISTICS];
}

DownloadManager::DownloadManager(Properties& _cProperties)
					:m_pIOMP(NULL)
					,m_iConnCount(0)
					,m_iMacAddr(0)
					,m_iIPAddr(0)
					,m_pWorkQueue(new CircularQueue())
					,m_pDSInfo(NULL)
					,m_pShm(NULL)
					,m_pShmDSStatus(NULL)
{
    this->SetStarted(false);
	pthread_mutex_init(&m_lockClient, NULL);
	//pthread_mutex_init(&m_lockShm, NULL);
	pthread_mutex_init(&m_lockClosed, NULL);
	m_pServerInfo = new ServerInfoDNMgr(_cProperties);
	m_pchStatistics = new char[MAX_STATISTICS];
//    CNPLog::GetInstance().Log("================ Create DownloadManager =====================");
}

DownloadManager::~DownloadManager()
{
    this->SetStarted(false);
//	DeleteAllMember();
	delete m_pIOMP;
    delete m_pServerInfo;
	delete m_pWorkQueue;
//	delete m_pRecvPipe;
//	delete m_pSlot;
	delete m_pDSInfo;
	delete m_pShm;
	delete m_pShmDSStatus;
//	delete m_pMQ;
	delete [] m_pchStatistics;
}

const int DownloadManager::GetCurrentUserCount()
{
/*
	return m_mapConnectList.size();


    int iCnt;
    pthread_mutex_lock(&m_lockClient);
    iCnt = m_lstClient.size();
    pthread_mutex_unlock(&m_lockClient);

    return iCnt;
*/
	return m_iConnCount;
}

const int DownloadManager::GetMaxUser()
{
    return m_pServerInfo->GetMaxUser() + m_pServerInfo->GetDNCnt();
}

const char* const DownloadManager::GetIPAddr()
{
    return m_pServerInfo->GetIPAddr();
}

const char* const DownloadManager::GetMRTGURL()
{
    return m_pServerInfo->GetMRTGURL();
}

const int DownloadManager::GetDNServerPort()
{
    return m_pServerInfo->GetDNPort();
}

const unsigned int DownloadManager::GetMacAddress()
{
	return m_iMacAddr;
}

const unsigned int DownloadManager::GetIPAddress()
{
	return m_iIPAddr;
}

const char* const DownloadManager::GetLogFileName()
{
    return m_pServerInfo->GetLogFileName();
}

/*
���Ϻл� url : http://211.37.6.118/hotfile/ftpstat.php
id /pw = wizsolution/������

                filename                    user    multi
9bba0aac873499a598a558a4c0843ea8_808775680  2       6       3613
809c3586a94da1273171005217c8761d_723888128  1       1       51
69e34b10a87230acc97a80666718e2ef_734056448  1       3       1428
                                 total      4       10      5092

ftpcount= 4     : User��
ftpfiles= 3     : ���� ������
multicount= 10  : session��
ftpkcps = 5092

ftpwho =9bba0aac873499a598a558a4c0843ea8_808775680,2,3613,465545,6|809c3586a94da1273171005217c8761d_723888128,1,51,465545,1
.....

filename, count, kcps,size, multi | filename, count, kcps,size, multi | filename, count, kcps,size, multi


======================================================================================
��������
ftpcount(����count)
ftpfiles
ftpdownload(���ϰ��� = ftpfiles�� ������ ��)
ftpkcps
cMac

ftpwho = filename, id, count, kcps, size | ....
======================================================================================
*/
const int DownloadManager::GetStatistics(
				struct scoreboard_file* const pSt,
				struct scoreboard_file** const _pNext,
				uint32_t _iPos,
				uint32_t* const _piFCount, 	// file count
				uint32_t* const _piUserCnt,	// id count
				uint32_t* const _piKcps,	// kcps
				uint32_t* const _piSameCnt)	// ���� ������ �ǳ� �ٱ� ���ؼ�..
{
	// ������ ������ ���� �� �ִ�..
    struct scoreboard_file *pNext = pSt + 1;
	if(_iPos+1 > MAX_CLIENT)
	{
		return 0;
	}

    //if( strncmp(pSt->filename, pNext->filename, 2) == 0)
    if( strcmp(pSt->filename, pNext->filename) == 0)
    {
        if(strcmp(pSt->id, pNext->id) != 0)
        {
            *_piUserCnt += 1;
        }

        //*_piFCount += 1;	// _piFCount �� ���� �̸��� �ٸ���.. (���������ϱ�..)
        *_piSameCnt += 1;	// ���� ������ �ǳ� �ٱ� ���ؼ�
        *_piKcps += pSt->kcps;
		_iPos += 1;
        GetStatistics(pNext, &pNext, _iPos, _piFCount, _piUserCnt, _piKcps, _piSameCnt);
    }
    else
    {
        *_piSameCnt += 1;	// ���� ������ �ǳ� �ٱ� ���ؼ�
        *_piFCount += 1;
        *_piKcps += pSt->kcps;
    }

    *_pNext = pNext;
    return 0;
}

const int DownloadManager::PostData(const char* _pData, const char* _pchURL, const char *_pchAuth)
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

const int DownloadManager::PostData(const char* _pData, const char* _pchURL)
{
/*
	int     sock;
	int     port;
	int     ret;
	char    send_data[1024*10];
	char    recv_data[1024*10];

	struct hostent * host_info;
	struct in_addr inAddr;
	struct timeval timeout;

	int iDNCnt = 0;

	timeout.tv_sec = (long)10;
	timeout.tv_usec = 0;

	char pchHost[128];
	char pchPage[128];

	memset(pchHost, 0x00, sizeof(pchHost));
	memset(pchPage, 0x00, sizeof(pchPage));

GetMRTGURL();

	host_info = gethostbyname("gm.gridcdn.com");
	port = 80;

	if( host_info != NULL )
	    inAddr = *(struct in_addr *)host_info->h_addr;
	else    //  get ip fail
	    return -1;

	memset(send_data, 0x00, sizeof(send_data));

http://wizsolution:dnlwmeoqkr@211.37.6.118/hotfile/postftpwho_multi.php
�̷��Ŵ� ��� �����ϳ�~
*/
//	sprintf(send_data,   "POST /remote/getAgentInfo.php HTTP/1.1\r\nAccept: */*\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s\r\n",
//	                        strlen(_pData), _pData);

/*
	Log("Trying to connect to '%s' port=%d data=(%s)", inet_ntoa(inAddr), port, send_data);
	if ( (sock = connect_tcp_socket( inet_ntoa(inAddr), port)) == -1 ) {
	    Log("In CallWeb, Connection NOT established");
	    return -1;
	}

	ret = timed_send(sock, send_data, strlen(send_data), &timeout);
	if( ret <= 0 )
	{
	    Log("In CallWeb, write ret = '%d', reason='%s'", ret, strerror(errno));
	    close(sock);
	    return -1;
	}

	ret = timed_recv(sock, recv_data, sizeof(recv_data), &timeout);
	if( ret <= 0 ) {
	    Log("In CallWeb, read ret = '%d', reason='%s'", ret, strerror(errno));
	    close(sock);
	    return -1;
	}

printf("post => (%s)", recv_data);
	close(sock);
*/

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

int cmpfileinfo(const void *p1, const void *p2)
{
	// arrange with filename
    if(strcmp((*(struct scoreboard_file *)p1).filename, (*(struct scoreboard_file *)p2).filename) > 0 &&
        (*(struct scoreboard_file *)p1).cUse == 1)
    {
        return -1;
    }

    if(strcmp((*(struct scoreboard_file *)p1).filename, (*(struct scoreboard_file *)p2).filename) == 0 &&
        (*(struct scoreboard_file *)p1).cUse == 1)
    {
        return 0;
    }

    return 1;
}

void DownloadManager::SendStorageInfoOld()
{
	struct scoreboard_file tTmpDsInfo[MAX_CLIENT];
	memset(tTmpDsInfo, 0, MAX_CLIENT * sizeof(struct scoreboard_file));

	//pthread_mutex_lock(&m_lockShm);
	//memcpy((char *)&tTmpDsInfo, (char *)&m_tDsInfo, MAX_CLIENT * sizeof(struct scoreboard_file));
	memcpy((char *)&tTmpDsInfo, (char *)&(m_pShm[0]), MAX_CLIENT * sizeof(struct scoreboard_file));
	//pthread_mutex_unlock(&m_lockShm);

	// sorting.
	qsort(tTmpDsInfo, MAX_CLIENT, sizeof(struct scoreboard_file), cmpfileinfo);

	memset(m_pchStatistics, 0x00, MAX_STATISTICS);
    char *pStr = &(m_pchStatistics[0]);

    pStr += 56 + 4 + 4 + 4+ 8;	// => "ftpwhover=0.4&ftpcount=%d&ftpfiles=%d&ftpdownload=%d&ftpkcps=%llu&" length
    //pStr += 41 + 4 + 4 + 4+ 8;	// => "ftpcount=%d&ftpfiles=%d&multicount=%d&ftpkcps=%llu&" length

	uint32_t iTotalFCount 	= 0;
	uint64_t iTotalKcps 	= 0;
	uint32_t iTotalUserCnt 	= 0;

    strcpy(pStr, "ftpwho=");
	int iPos = 0;
	while(1)
	{

		struct scoreboard_file *pNext = NULL;
		if(tTmpDsInfo[iPos].cUse != ON ||
				iPos+1 >= MAX_CLIENT)
	    {
	        break;
	    }
//CNPLog::GetInstance().Log("pos=(%d) file=(%s)", iPos, tTmpDsInfo[iPos].filename);

		uint32_t iFCount = 0, iUserCnt = 1, iKcps = 0, iSameCnt = 0;
		GetStatistics(
					&(tTmpDsInfo[iPos]),
					&pNext,
					iPos,
					&iFCount,
					&iUserCnt,
					&iKcps,
					&iSameCnt);

		sprintf(pStr, "%s%s,%d,%d,%llu|",
							pStr,
							tTmpDsInfo[iPos].filename,
							iUserCnt,				// user ���� ���±�..
							iKcps,
							tTmpDsInfo[iPos].iFSize);

//CNPLog::GetInstance().Log("pStr================(%s)",pStr);
		iTotalFCount 	+= iFCount;
		iTotalKcps 		+= iKcps;
		iTotalUserCnt 	+= iUserCnt;

		iPos += iSameCnt;
		if(iPos >= MAX_CLIENT)
		{
			break;
		}
	}

	if(iTotalUserCnt > 0)
	{
		iTotalUserCnt--;
	}
#ifdef _HOMESTORAGE
    sprintf(m_pchStatistics, "ftpwhover=0.4&flag=1&ftpcount=%d&ftpfiles=%d&ftpdownload=%d&ftpkcps=%llu&%s",
							iTotalUserCnt, iTotalFCount, iTotalFCount, iTotalKcps, pStr);
#else
    sprintf(m_pchStatistics, "ftpwhover=0.4&ftpcount=%d&ftpfiles=%d&ftpdownload=%d&ftpkcps=%llu&%s",
							iTotalUserCnt, iTotalFCount, iTotalFCount, iTotalKcps, pStr);
							//iPos, iTotalFCount, iTotalFCount, iTotalKcps, pStr);
	char pchTmp[128];
    struct in_addr laddr;
    laddr.s_addr = m_iIPAddr;
	memset(pchTmp, 0x00, sizeof(pchTmp));
	sprintf(pchTmp, "&ip=%s", inet_ntoa(laddr));
	strcat(m_pchStatistics, pchTmp);
#endif


/*
CNPLog::GetInstance().Log("SendToWeb=(%s) URL=(%s)", m_pchStatistics, GetMRTGURL());
CNPLog::GetInstance().Log("SendToWeb=(%d) URL=(%s) iTotalUserCnt=(%d)", strlen(m_pchStatistics), GetMRTGURL(), iTotalUserCnt);
CNPLog::GetInstance().Log("(%s)", m_pchStatistics);
*/
	if(PostData(m_pchStatistics, GetMRTGURL()) >= 0)
	{
		CNPLog::GetInstance().Log("PostData Success!");
	}
}

void DownloadManager::SendStorageInfo()
{
	SendStorageInfoOld();
	return;

/*
CNPLog::GetInstance().Log("SendStorageInfo Called!");
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if(m_pShm[i].cUse == ON)
		{
			CNPLog::GetInstance().Log("m_pShm[%d].cUse=(%d) (%p)", i, m_pShm[i].cUse, &(m_pShm[i]));
			CNPLog::GetInstance().Log("m_pShm[%d].comcode=(%d)", i, m_pShm[i].comcode);
			CNPLog::GetInstance().Log("m_pShm[%d].billno=(%d)", i, m_pShm[i].billno);
			CNPLog::GetInstance().Log("m_pShm[%d].kcps=(%d)", i, m_pShm[i].kcps);
			CNPLog::GetInstance().Log("m_pShm[%d].filename=(%s)", i, m_pShm[i].filename);
			CNPLog::GetInstance().Log("m_pShm[%d].id=(%s)", i, m_pShm[i].id);
			CNPLog::GetInstance().Log("m_pShm[%d].dnsize=(%llu)", i, m_pShm[i].iDNSize);
		}
	}
return;
*/

	struct scoreboard_file tTmpDsInfo[MAX_CLIENT];
	memset(tTmpDsInfo, 0, MAX_CLIENT * sizeof(struct scoreboard_file));

	//pthread_mutex_lock(&m_lockShm);
	//memcpy((char *)&tTmpDsInfo, (char *)&m_tDsInfo, MAX_CLIENT * sizeof(struct scoreboard_file));
	memcpy((char *)&tTmpDsInfo, (char *)&(m_pShm[0]), MAX_CLIENT * sizeof(struct scoreboard_file));
	//pthread_mutex_unlock(&m_lockShm);

	// sorting.
	qsort(tTmpDsInfo, MAX_CLIENT, sizeof(struct scoreboard_file), cmpfileinfo);

/*
	char databuf[20000];
	memset(databuf, 0x00, sizeof(databuf));
    char *pStr = &(databuf[0]);
*/
//	char    m_pchStatistics[MAX_STATISTICS];
	memset(m_pchStatistics, 0x00, MAX_STATISTICS);
    char *pStr = &(m_pchStatistics[0]);

    pStr += 41 + 4 + 4 + 4+ 8;	// => "ftpcount=%d&ftpfiles=%d&multicount=%d&ftpkcps=%llu&" length

	uint32_t iTotalFCount 	= 0;
	uint64_t iTotalKcps 	= 0;
	uint32_t iTotalUserCnt 	= 0;

    strcpy(pStr, "ftpwho=");
	int iPos = 0;
	while(1)
	{

		struct scoreboard_file *pNext = NULL;
		if(tTmpDsInfo[iPos].cUse != ON ||
				iPos+1 >= MAX_CLIENT)
	    {
	        break;
	    }
//CNPLog::GetInstance().Log("pos=(%d) file=(%s)", iPos, tTmpDsInfo[iPos].filename);

		uint32_t iFCount = 0, iUserCnt = 1, iKcps = 0, iSameCnt = 0;
		GetStatistics(
					&(tTmpDsInfo[iPos]),
					&pNext,
					iPos,
					&iFCount,
					&iUserCnt,
					&iKcps,
					&iSameCnt);

		sprintf(pStr, "%s%s,%d,%d,%llu,%d|",
							pStr,
							tTmpDsInfo[iPos].filename,
							//iFCount,				// user ���� ���±�..
							iUserCnt,				// user ���� ���±�..
							iKcps,
							tTmpDsInfo[iPos].iFSize,
							iUserCnt);

//CNPLog::GetInstance().Log("pStr================(%s)",pStr);

		iTotalFCount 	+= iFCount;
		iTotalKcps 		+= iKcps;
		iTotalUserCnt 	+= iUserCnt;

		iPos += iSameCnt;
		if(iPos >= MAX_CLIENT)
		{
			break;
		}
	}

    sprintf(m_pchStatistics, "ftpcount=%d&ftpfiles=%d&multicount=%d&ftpkcps=%llu&%s",
							iTotalUserCnt, iTotalFCount, iPos, iTotalKcps, pStr);

CNPLog::GetInstance().Log("SendToWeb=(%d) URL=(%s)", strlen(m_pchStatistics), GetMRTGURL());
}

void DownloadManager::PutWorkQueue(const void* const _pVoid)
{
	m_pWorkQueue->EnQueue(_pVoid);
}

const void* const DownloadManager::GetWorkQueue()
{
	return m_pWorkQueue->DeQueue();
}

void DownloadManager::PutClosedList(Tcmd_USER_CLOSE_DS_DSM* const _pClosedInfo)
{
	pthread_mutex_lock(&m_lockClosed);
//CNPLog::GetInstance().Log("DownloadManager::PutClosedList =>(%p)", _pClosedInfo);
	m_lstClosed.push_back(_pClosedInfo);
    pthread_mutex_unlock(&m_lockClosed);
}

const uint64_t DownloadManager::GetClientDownloadSize(uint32_t _iComCode, uint32_t _iBillno)
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

void DownloadManager::HealthCheckClosedList()
{

 	pthread_mutex_lock(&m_lockClosed);
	//CNPLog::GetInstance().Log("DownloadManager::HealthCheckClosedList list_size=(%d), max=(%d)", m_lstClosed.size(), m_lstClosed.max_size());

    std::list<Tcmd_USER_CLOSE_DS_DSM*>::iterator iter = m_lstClosed.begin();
    while( iter != m_lstClosed.end() )
    {
        Tcmd_USER_CLOSE_DS_DSM *pClosedInfo = static_cast<Tcmd_USER_CLOSE_DS_DSM *>(*iter);

        double dNow = CNPUtil::GetMicroTime();
        //if((CNPUtil::GetMicroTime() - pClosedInfo->dClosedTime) > TIME_ALIVE)
        if((dNow - pClosedInfo->dClosedTime) > TIME_ALIVE)
        {
            CNPLog::GetInstance().Log("DownloadManager::HealthCheckClosedList(%p) Kill \
                            comcode=(%d),billno=(%d),downSize=(%llu), (%.2f)",
                        pClosedInfo, pClosedInfo->nComCode, pClosedInfo->nBillNo, pClosedInfo->nDownSize, dNow);

            iter = m_lstClosed.erase( iter );
            delete pClosedInfo;
        }

		iter++;
    }

/*
    std::list<Tcmd_USER_CLOSE_DS_DSM*>::iterator pos, itrClientPrev;
    pos = m_lstClosed.begin();
    while( pos != m_lstClosed.end() )
	{
        itrClientPrev = pos++;
		if(itrClientPrev == NULL)
		{
			break;
		}
        Tcmd_USER_CLOSE_DS_DSM *pClosedInfo = static_cast<Tcmd_USER_CLOSE_DS_DSM *>(*itrClientPrev);

		double dNow = CNPUtil::GetMicroTime();

		//if((CNPUtil::GetMicroTime() - pClosedInfo->dClosedTime) > TIME_ALIVE)
		if((dNow - pClosedInfo->dClosedTime) > TIME_ALIVE)
        {
			CNPLog::GetInstance().Log("DownloadManager::HealthCheckClosedList(%p) Kill \
							comcode=(%d),billno=(%d),downSize=(%llu), (%.2f)",
						pClosedInfo, pClosedInfo->nComCode, pClosedInfo->nBillNo, pClosedInfo->nDownSize, dNow);


            m_lstClosed.erase( itrClientPrev );
            //delete *itrClientPrev;
            delete pClosedInfo;
        }
    }
*/
	pthread_mutex_unlock(&m_lockClosed);
}


void DownloadManager::HealthCheckUsers()
{
	pthread_mutex_lock(&m_lockClient);

	std::list<Client*>::iterator iter = m_lstClient.begin();
    while( iter != m_lstClient.end() )
    {
		Client *pClient = static_cast<Client *>(*iter);
        if(pClient->GetType() == CLIENT_USER)
		{
			if(CNPUtil::GetMicroTime()-pClient->GetAccessTime() > TIME_ALIVE ||
							pClient->IsClosed())
			{
				CNPLog::GetInstance().Log("DownloadManager::HealthCheckUsers Kill Client [%p] fd=(%d)=(%d)",
							pClient, pClient->GetSocket()->GetFd(), CNPUtil::GetUnixTime()-pClient->GetAccessTime());
				iter = m_lstClient.erase( iter );

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
/*
	std::list<Client*>::iterator pos, itrClientPrev;
	pos = m_lstClient.begin();
	while( pos != m_lstClient.end() )
	{
		itrClientPrev = pos++;
		//Client *pClient = (Client *)*itrClientPrev;
		Client *pClient = static_cast<Client *>(*itrClientPrev);

		if(pClient->GetType() != CLIENT_USER)
		{
			continue;
		}

	    if(CNPUtil::GetMicroTime()-pClient->GetAccessTime() > TIME_ALIVE ||
						pClient->IsClosed())
		{
			CNPLog::GetInstance().Log("DownloadManager::HealthCheckUsers Kill Client [%p] fd=(%d)=(%d)",
						pClient, pClient->GetSocket()->GetFd(), CNPUtil::GetUnixTime()-pClient->GetAccessTime());

	        m_pIOMP->DelClient(pClient);
	        m_lstClient.erase( itrClientPrev );
	        //delete (Client *)*itrClientPrev;
			delete pClient;
			m_iConnCount--;
		}
	}
*/
	pthread_mutex_unlock(&m_lockClient);

	HealthCheckClosedList();
}

/*
const serverInfoMap& DownloadManager::GetServerPortMap()
{
    return m_pServerInfo->GetPortMap();
}
*/

void DownloadManager::UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
#ifndef _FREEBSD
    m_pIOMP->ModifyFd(_pClient, _uiEvents);
#endif
}

#ifdef _FREEBSD
void DownloadManager::AddEPoll(Client* const _pClient, const short _filter, const unsigned short _usFlags)
{
	m_pIOMP->AddClient(_pClient, _filter, _usFlags);
}
#else
void DownloadManager::AddEPoll(Client* const _pClient, const unsigned int _uiEvents)
{
	m_pIOMP->AddClient(_pClient, _uiEvents);
}
#endif

void DownloadManager::CloseClient(Client* const _pClient)
//void DownloadManager::CloseClient(Client* _pClient)
{
CNPLog::GetInstance().Log("DownloadManager::CloseClient ==> (%p)(%d)",
					_pClient, _pClient->GetSocket()->GetFd());
#ifdef _FREEBSD
	m_pIOMP->DelClient(_pClient, EVFILT_READ);
#else
	m_pIOMP->DelClient(_pClient);
#endif

	pthread_mutex_lock(&m_lockClient);
	m_lstClient.remove(_pClient);
	pthread_mutex_unlock(&m_lockClient);

	// if you SetState() before erase the Client in the client map,
	// it could occur the crash program.
	// because of SetState() is on a collision with HealthCheck
	_pClient->SetState(STATE_CLOSED);

	if(_pClient->GetType() == CLIENT_DN)
	{
		memset(&(m_pDSInfo[_pClient->GetUserSeq()]), 0, sizeof(Tcmd_HELLO_DSM_DS));
	}

	delete _pClient;

	m_iConnCount--;

//	CloseQueue::GetInstance().EnQueue(_pClient);
}

void DownloadManager::SettingDS(const int _iPos, int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const
_piShmDSStatus, int _iPid)
{
	m_pDSInfo[_iPos].iSeq 		= _iPos;
	m_pDSInfo[_iPos].iPid 		= _iPid;
	m_pDSInfo[_iPos].iMaxUser 	= m_pServerInfo->GetDSMaxUser();
	m_pDSInfo[_iPos].iShmKey 	= m_pServerInfo->GetShmKey();
	m_pDSInfo[_iPos].dHelloTime = CNPUtil::GetMicroTime();

	*_piSeq 		= _iPos;
	*_piMaxUser 	= m_pServerInfo->GetDSMaxUser();
	*_piShmKey 		= m_pServerInfo->GetShmKey();
	*_piShmDSStatus	= m_pServerInfo->GetShmDSStatus();

	// DS ���� Hello �� ����, pid, pos �� �����ϰ�, DS status�� OFF �� �����Ѵ�.
	// DS ���� thread ���� ������ ������, status �� ON �Ѵ�.
	m_pShmDSStatus[_iPos].pid = _iPid;
	m_pShmDSStatus[_iPos].seq = _iPos;
	m_pShmDSStatus[_iPos].status = OFF;

CNPLog::GetInstance().Log("Setting DS pid=(%d), seq=(%d), status=(%d)",
							m_pShmDSStatus[_iPos].pid
							,m_pShmDSStatus[_iPos].seq
							,m_pShmDSStatus[_iPos].status );
}

const int DownloadManager::SetDS(int* const _piSeq, int* const _piMaxUser, int* const _piShmKey, int* const _piShmDSStatus, int _iPid)
{
	int i;

	for(i = 0; i < m_pServerInfo->GetDNCnt(); i++)
	{
		if( m_pDSInfo[i].iPid <= 0)
		{
			SettingDS(i, _piSeq,_piMaxUser,_piShmKey, _piShmDSStatus, _iPid);
			break;
		}
		else
		{
			if(!IsAliveProcess(m_pDSInfo[i].iPid))
			{
CNPLog::GetInstance().Log("���� ���μ��� �̹Ƿ� �ٽ� �Ҵ�ȴ�.slot(%d) (%d)=>(%d)",
				i, m_pDSInfo[i].iPid, _iPid);

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
CNPLog::GetInstance().Log("DS ��Ȳ.slot(%d) (%d),(%d)",
				i, m_pDSInfo[i].iPid, _iPid);
	}
*/

	return 0;
}

void DownloadManager::DoFork(Process *_pProcess)
{
    /**
    *   IOMP Class Create
    */
#ifdef _FREEBSD
    m_pIOMP = new IOMP_KQUEUE(500000000);	// 0.5 sec
#else
	m_pIOMP = new IOMP_EPoll(100);
#endif

	/**
	*	Create Server Socket for DownloadManager
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
	*	Create Server Socket for DownloadServer
	*/
    Client *pDNServer = NULL;
	ServerSocket *pServerSocket = new ServerSocket(SERVER_PORT);
	if(pServerSocket->Bind(m_pServerInfo->GetDNPort()) < 0)
	{
	    return;
	}
	if(pServerSocket->Listen(100) < 0)
	{
	    return;
	}
	if(pServerSocket->SetNonBlock() < 0)
	{
	    return;
	}

	pServerSocket->SetLinger();
	pDNServer = new ClientServer(pServerSocket);

	// Create SharedMemory
	// 1. create
	SharedMemory sm((key_t)m_pServerInfo->GetShmKey(), MAX_CLIENT * sizeof(struct scoreboard_file));
    if(!sm.IsStarted())
	{
		printf("SharedMemory ���� ����1 \n");

		// 2. destroy
		SharedMemory sm1((key_t)m_pServerInfo->GetShmKey());
		sm1.Destroy();
		return ;
	}
	m_pShm = (struct scoreboard_file *)sm.GetDataPoint();
	if(m_pShm == NULL)
	{
		printf("Shm is NULL \n");
		return;
	}
//printf("Shared Memory start=(%p) \n", m_pShm);
	memset(m_pShm, 0, MAX_CLIENT * sizeof(struct scoreboard_file));

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
//printf("Shared Memory start=(%p) \n", m_pShmDSStatus);
	memset(m_pShmDSStatus, 0, 100 * sizeof(struct TDSStatus));
	// **

	((DownloadServer *)_pProcess)->SetServerSocket(pDNServer);

    CNPLog::GetInstance().BackupLogFile2((char *)m_pServerInfo->GetLogFileName());
	//SpawnProcessWithPipe(_pProcess, m_pServerInfo->GetDNCnt());
	SpawnProcess(_pProcess, m_pServerInfo->GetDNCnt());

	// Run Manager
	Run();
}

void DownloadManager::Run()
{
    this->SetStarted(true);

	CNPUtil::GetIPConfig(&m_iMacAddr, &m_iIPAddr);

    //if(CNPLog::GetInstance().SetFileName((char *)(cDNMgrProperties.GetProperty("SERVER_LOGFILE").c_str())))
//printf("[%s] \n", m_pServerInfo->GetLogFileName());


/*
for(int i =0; i < 10; i++)
{
*/
    //CNPLog::GetInstance().BackupLogFile2((char *)m_pServerInfo->GetLogFileName());
    if(CNPLog::GetInstance().SetFileName((char *)(m_pServerInfo->GetLogFileName())) )
    {
        Assert(false, "LogFile create error! ");
		return;
    }
/*
}
*/

	// alloc release slot
//	m_pSlot = new ReleaseSlot(MAX_CLIENT);

	// Download Server info table
	m_pDSInfo = new Tcmd_HELLO_DSM_DS[m_pServerInfo->GetDNCnt()];
	memset(m_pDSInfo, 0, m_pServerInfo->GetDNCnt() * sizeof(Tcmd_HELLO_DSM_DS));

    struct in_addr laddr;
    laddr.s_addr = m_iIPAddr;

    struct ether_addr haddr;
    memcpy((void *)&haddr, (void *)&m_iMacAddr, 6);

	CNPLog::GetInstance().Log("DownloadManager::Run : pSm=(%p), size=(%d), stSize=(%d), ip=(%s)(%s)",
							m_pShm,  MAX_CLIENT * sizeof(struct scoreboard_file), sizeof(struct scoreboard_file), inet_ntoa(laddr),
							(char *)ether_ntoa ((struct ether_addr *)&haddr));

	// Worker create.
	for(int i = 0; i < m_pServerInfo->GetThreadCount(THREAD_WORKER); i++)
	{
		Thread *t = new ThreadWorker(this);
		ThreadManager::GetInstance()->Spawn(t);
		CNPLog::GetInstance().Log("In DownloadManager WORKER Create (%p,%lu) ", t, t->GetThreadID());
	}

	ThreadTic *tTic = new ThreadTic(this);
	ThreadManager::GetInstance()->Spawn(tTic);
	CNPLog::GetInstance().Log("In DownloadManager ThreadTic Create (%p) ", tTic);

	while(1)
	{
		int iEventCount;

		WatchChildProcess();

		if((iEventCount = m_pIOMP->Polling()) <= 0)
		{
			//CNPLog::GetInstance().Log("epoll_wait error errno=%d, strerror=(%s)", errno, strerror(errno));
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

                    CNPLog::GetInstance().Log("Accept ====> ClientIp=(%s) conncnt=(%d),maxuser=(%d)",
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
                    //if(((ServerSocket *)pClient->GetSocket())->GetType() == SERVER_PORT)
                    if(static_cast<ServerSocket *>(pClient->GetSocket())->GetType() == SERVER_PORT)
                    {
/*
						CNPLog::GetInstance().Log("1.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
						pClientSocket->SetSndBufSize(1024000);
						CNPLog::GetInstance().Log("2.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
*/
                        pNewClient = new ClientDN(pClientSocket);
#ifdef _FREEBSD
						if(m_pIOMP->AddClient(pNewClient, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR) < 0)
#else
						//if(m_pIOMP->AddClient(pNewClient, EPOLLIN|EPOLLET) < 0)
						if(m_pIOMP->AddClient(pNewClient, EPOLLIN|EPOLLET|EPOLLONESHOT) < 0)
#endif
						{
							delete pNewClient;
							continue;
						}

						pNewClient->SetState(STATE_WAIT);
						//pNewClient->SetDownloadManager(this);
						pNewClient->SetMainProcess(this);
						pthread_mutex_lock(&m_lockClient);
/*
					    m_mapConnectList.insert( connValType(((Socket *)(pNewClient->GetSocket()))->GetFd(),
										(Client *)pNewClient));
*/
						m_lstClient.push_back((Client *)pNewClient);
						pthread_mutex_unlock(&m_lockClient);

                        CNPLog::GetInstance().Log("NewClient(%p) fd ==>(%d) ",
													pNewClient,
													pNewClient->GetSocket()->GetFd());

//						m_lstClient.push_back(pNewClient);
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
							//((Socket *)(pClient->GetSocket()))->GetFd(), errno?EBADF:1000?EINVAL:2000?EFAULT:3000?EINTR:4000,
							pClient->GetSocket()->GetFd(), errno, strerror(errno));

				CloseClient(pClient);
	        }
			else
	        if(tEvents[i].events & EPOLLIN)
	        {
				//m_pIOMP->DelClient(pClient);
#ifdef _DEBUG
	            CNPLog::GetInstance().Log("EPOLLIN Client %p, events=(%d)", pClient, tEvents[i].events);
#endif
	            pClient->SetAccessTime();
				PutWorkQueue(pClient);
	            //EventQueue::GetInstance().EnQueue(pClient);
	        }
			else
	        if(tEvents[i].events & EPOLLOUT)
	        {
	            CNPLog::GetInstance().Log("EPOLLOUT Client %p, events=(%d)", pClient, tEvents[i].events);
/*
				if(((ClientUser*)pClient)->GetSendPacketCount() > 0)
				{
					m_pIOMP->DelClient(pClient);
					PutSendQueue(pClient);
	            	//SendQueue::GetInstance().EnQueue(pClient);
				}
*/
			}
#endif

	        continue;
		}
	}

    ThreadManager::GetInstance()->ReleaseInstance();
}

