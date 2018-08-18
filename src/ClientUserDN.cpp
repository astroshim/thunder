#include "../include/ClientUserDN.h"
#include "../include/NPUtil.h"
#include "../include/DownloadServer.h"
#include "../include/ThreadReceiver.h"

ClientUserDN::ClientUserDN()
{
    memset((char *)&m_tFileInfo, 0x00, sizeof(TFileInfo));

	m_tFileInfo.iFileFd = -1;
	memset(m_tFileInfo.sFileName, 0x00, FILE_SZ);
	m_tFileInfo.nOffset = 0;
	m_tFileInfo.nDownSize = 0;
	memset(m_tFileInfo.sID, 0x00, ID_SZ);
	m_tFileInfo.nBillNo = 0;
	m_tFileInfo.nComCode = 0;
	m_tFileInfo.nSvcCode = 0;

	m_iBandWidth = 0;
	m_nTotalSendSize = 0;
	m_dStartTime = 0.0;
	m_dSendTime = 0.0;
	m_dFileSendStartTime = 0.0;
	m_iComCodeIdx = 1;
	memset(m_pchDownPath, 0x00, PATH_SZ);
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
	m_eSendMode = SEND_ONLY_FILE;
	pthread_mutex_init(&m_mtLst, NULL);
}

ClientUserDN::ClientUserDN(Socket* const _cSocket)
					:Client(_cSocket, CLIENT_USER)
					//,m_pDownloadServer(NULL)
					 //m_uiSessionKey(0)
{
    memset((char *)&m_tFileInfo, 0x00, sizeof(TFileInfo));

	m_tFileInfo.iFileFd = -1;
	memset(m_tFileInfo.sFileName, 0x00, FILE_SZ);
	m_tFileInfo.nOffset = 0;
	m_tFileInfo.nDownSize = 0;
	memset(m_tFileInfo.sID, 0x00, ID_SZ);
	m_tFileInfo.nBillNo = 0;
	m_tFileInfo.nComCode = 0;
	m_tFileInfo.nSvcCode = 0;

	m_iBandWidth = 0;
	m_nTotalSendSize = 0;
	m_dStartTime = 0.0;
	m_dSendTime = 0.0;

	m_dFileSendStartTime = 0.0;
	m_iComCodeIdx = 1;
	memset(m_pchDownPath, 0x00, PATH_SZ);
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
	m_eSendMode = SEND_ONLY_FILE;
	pthread_mutex_init(&m_mtLst, NULL);
}

ClientUserDN::~ClientUserDN()
{
    if(m_tFileInfo.iFileFd > 0)
    {
        close(m_tFileInfo.iFileFd);
    }
	FreePacket();
}

#ifdef _CLIENT_ARRAY
void ClientUserDN::InitValiable()
{
    if(m_tFileInfo.iFileFd > 0)
    {
        close(m_tFileInfo.iFileFd);
    }
    memset((char *)&m_tFileInfo, 0, sizeof(TFileInfo));
	m_tFileInfo.iFileFd = -1;

    m_iBandWidth = 0;
    m_nTotalSendSize = 0;
    m_dStartTime = 0.0;
    m_dSendTime = 0.0;
	m_dFileSendStartTime = 0.0;
	m_iComCodeIdx = 1;

	memset(m_pchDownPath, 0x00, PATH_SZ);
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
	m_eSendMode = SEND_ONLY_FILE;
}
#endif

void ClientUserDN::FreePacket()
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

const int ClientUserDN::GetSendPacketCount()
{
	return m_lstSendPacket.size();
}

#ifdef _FREEBSD
const off_t ClientUserDN::GetFileOffset()
#else
const off_t ClientUserDN::GetFileOffset() // mac
// const off64_t ClientUserDN::GetFileOffset()
#endif
{
	return m_tFileInfo.nOffset;
}

#ifdef _FREEBSD
const size_t ClientUserDN::GetSendSize()
{
	return m_tFileInfo.nDownSize;
}

void ClientUserDN::SetSendSize(const size_t _iSize)
{
	m_tFileInfo.nDownSize = _iSize;
//CNPLog::GetInstance().Log("ClientUserDN::SetSendSize = (%d) ", m_tFileInfo.nDownSize);
}

void ClientUserDN::SetTotalSendSize(const size_t _iSize)
{
	m_nTotalSendSize = _iSize;
//CNPLog::GetInstance().Log("ClientUserDN::SetTotalSendSize = (%d) ", m_nTotalSendSize);
}

void ClientUserDN::AddTotalSendSize(const size_t _iSize)
{
	m_nTotalSendSize += _iSize;
//CNPLog::GetInstance().Log("ClientUserDN::AddTotalSendSize = (%d) ", m_nTotalSendSize);
}

const size_t ClientUserDN::GetTotalSendSize()
{
	return m_nTotalSendSize;
}

const size_t ClientUserDN::GetFileSize()
{
	return m_tFileStat.st_size;
}

#else
const uint64_t ClientUserDN::GetSendSize()
{
	return m_tFileInfo.nDownSize;
}

void ClientUserDN::SetSendSize(const uint64_t _iSize)
{
	m_tFileInfo.nDownSize = _iSize;
}

void ClientUserDN::SetTotalSendSize(const uint64_t _iSize)
{
	m_nTotalSendSize = _iSize;
}

void ClientUserDN::AddTotalSendSize(const uint64_t _iSize)
{
	m_nTotalSendSize += _iSize;
}

const uint64_t ClientUserDN::GetTotalSendSize()
{
	return m_nTotalSendSize;
}

const uint64_t ClientUserDN::GetFileSize()
{
	return m_tFileStat.st_size;
}

#endif

const double ClientUserDN::GetStartTime()
{
	return m_dStartTime;
}

void ClientUserDN::SetStartTime(const double _dStartTime)
{
	m_dStartTime = _dStartTime;
}

const double ClientUserDN::GetSendTime()
{
	return m_dSendTime;
}

void ClientUserDN::SetSendTime(const double _dSendTime)
{
	m_dSendTime = _dSendTime;
}

const double ClientUserDN::GetFileSendStartTime()
{
	return m_dFileSendStartTime;
}

const int ClientUserDN::GetComCodeIdx()
{
	return m_iComCodeIdx;
}

void ClientUserDN::SetComCodeIdx(const int _iComCodeIdx)
{
	m_iComCodeIdx = _iComCodeIdx;
}

void ClientUserDN::AddFileOffset(const off_t _nOffset)
{
	m_tFileInfo.nOffset += _nOffset;
}

const int ClientUserDN::GetFileFd()
{
	return m_tFileInfo.iFileFd;
}

const char* ClientUserDN::GetFileName()
{
	return m_tFileInfo.sFileName;
}

const char* ClientUserDN::GetID()
{
	return m_tFileInfo.sID;
}

const uint32_t ClientUserDN::GetBillNo()
{
	return m_tFileInfo.nBillNo;
}

const uint32_t ClientUserDN::GetComCode()
{
	return m_tFileInfo.nComCode;
}

const uint32_t ClientUserDN::GetBandWidth()
{
	return m_iBandWidth;
}

const char* ClientUserDN::GetFilePath()
{
	return m_pchDownPath;
}

list<T_PACKET*>* const ClientUserDN::GetPacketList()
{
	return &m_lstSendPacket;
}

void ClientUserDN::AddPacketFront(T_PACKET *_pPacket)
{
	pthread_mutex_lock(&m_mtLst);
	m_lstSendPacket.push_front(_pPacket);
	pthread_mutex_unlock(&m_mtLst);
}

const char* const ClientUserDN::GetVolumName()
{
    DownloadServer *pServer = NULL;
    if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
    {
    }
    else
    {
        CNPLog::GetInstance().Log("Work GetVolumName(%p) pServer is NULL!! ", this);
		return NULL;
    }

	return pServer->GetVolName();
}

const char* const ClientUserDN::GetDirName()
{
    DownloadServer *pServer = NULL;
    if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
    {
    }
    else
    {
        CNPLog::GetInstance().Log("Work GetDirName(%p) pServer is NULL!! ", this);
		return NULL;
    }

	return pServer->GetDirName();
}

void ClientUserDN::RemovePacket(T_PACKET *_pPacket)
{
	pthread_mutex_lock(&m_mtLst);
	m_lstSendPacket.remove(_pPacket);
	pthread_mutex_unlock(&m_mtLst);
}

const int ClientUserDN::SetFileStat(const char* _pchFileName)
{
#ifdef _FREEBSD
    if(stat(_pchFileName, &m_tFileStat) < 0)
#else
    if(stat64(_pchFileName, &m_tFileStat) < 0)
#endif
    {
		return -1;
    }

	return 0;
}

const ENUM_SEND_MODE ClientUserDN::GetSendMode()
{
	return m_eSendMode;
}

void ClientUserDN::SetSendMode(const ENUM_SEND_MODE _eMode)
{
	m_eSendMode = _eMode;
}

#ifdef _FREEBSD
void ClientUserDN::SpeedControl(off_t _nOffSet)
#else
// void ClientUserDN::SpeedControl(off64_t _nOffSet)
void ClientUserDN::SpeedControl(off_t _nOffSet) // mac
#endif
{
    double ended;

	// 20090224 :
	if(m_iBandWidth >= BADN_MAX)
	{
		m_dSendTime = m_dStartTime;
		return;
	}

	if (m_iBandWidth > 0UL)
	{
	    //long double delay;
	    double delay;

#ifdef _DEBUG
CNPLog::GetInstance().Log("Sleep In SendFil(%p) ComCode=(%d), BillNo=(%d), BW=(%d), _nOffSet=(%llu), GetFileOffset()=(%llu)",
								this, m_tFileInfo.nComCode, m_tFileInfo.nBillNo, m_iBandWidth, _nOffSet, m_tFileInfo.nOffset);
#endif

	    ended = CNPUtil::GetMicroTime();
	    //delay = ( (this->GetFileOffset() - _nOffSet ) / (long double) m_iBandWidth)

		// �������� �� - ������ / bandwidth - (�ð�);
	    delay = ( (this->GetSendSize() - _nOffSet ) / (long double) m_iBandWidth)
				        - (long double) (ended - m_dStartTime);

#ifdef _DEBUG
CNPLog::GetInstance().Log("Sleep In SendFile(%p) ComCode=(%d), BillNo=(%d), delay=(%llf)",
								this, m_tFileInfo.nComCode, m_tFileInfo.nBillNo, delay);
#endif
	    if (delay > (double) MAX_THROTTLING_DELAY)
	    {
	        m_dStartTime = ended;
	        delay = (double) MAX_THROTTLING_DELAY;
	    }
	    if (delay > 0.0L)
	    {
			m_dSendTime = m_dStartTime + delay;

#ifdef _DEBUG
CNPLog::GetInstance().Log("Send's time(%p) => m_dStartTime=(%f) + delay=(%f) = Send�ؾ��� �ð�(%f))",
						this, m_dStartTime, delay, m_dSendTime);
#endif

			// sleep �Ѵ�.
			//CNPUtil::NSleep((unsigned long)(delay *  1000000.0L));
	    }
	}
}

void ClientUserDN::SetFilePath(char *id)
{
	memset(m_pchDownPath, 0x00, PATH_SZ);

	char pchHead[12], pchTail[32];
	memset(pchHead, 0x00, sizeof(pchHead));
	memset(pchTail, 0x00, sizeof(pchTail));

	char *p1;

    if(GetDirName()[0] == '/')
    {
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s", GetVolumName(), GetDirName());
        return ;
    }

    p1 = strchr((char *)GetDirName(), '/');
    if(p1 == NULL)
    {
   		snprintf(m_pchDownPath, PATH_SZ, "/%s", GetVolumName());
        return ;
    }

    strncpy(pchHead, GetDirName(), strlen(GetDirName()) - strlen(p1));
	strcpy(pchTail, p1+1);

//CNPLog::GetInstance().Log("pchTail ======> [%s], head=(%s) ", pchTail, pchHead);
    char pchDirHead[10];
    char pchDir[10];
    int nLen = 0;
	memset(pchDirHead, 0x00, sizeof(pchDirHead));
	memset(pchDir, 0x00, sizeof(pchDir));

	nLen = strlen(pchHead);
    strncpy(pchDirHead, pchHead, nLen -1);

    if (id[0] == 'a')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48));
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'b')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+1);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'c')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+2);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'd')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+3);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'e')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+4);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'f')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+5);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'g')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+6);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else if (id[0] == 'h')
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48)+7);
        snprintf(m_pchDownPath, PATH_SZ, "/%s%s/%s", GetVolumName(), pchDir, pchTail);
    }
    else
    {
    	sprintf(pchDir, "%s%d", pchDirHead, (pchHead[nLen-1]-48));
   		snprintf(m_pchDownPath, PATH_SZ, "/%s%s", GetVolumName(), pchDir);
    }
}

void ClientUserDN::SetFilePathOld(char *id)
{
	memset(m_pchDownPath, 0x00, PATH_SZ);

	if (id[0] == 'a')
	{
	    //snprintf(m_pchDownPath, PATH_SZ, "/%s01/files", ((DownloadServer *)m_pMainProcess)->GetVolName());
	    snprintf(m_pchDownPath, PATH_SZ, "/%s01/files", GetVolumName());
	}
	else if (id[0] == 'b')
	{
	    snprintf(m_pchDownPath, PATH_SZ, "/%s02/files", GetVolumName());
	}
	else if (id[0] == 'c')
	{
	    snprintf(m_pchDownPath, PATH_SZ, "/%s03/files", GetVolumName());
	}
	else if (id[0] == 'd')
	{
	    snprintf(m_pchDownPath, PATH_SZ, "/%s04/files", GetVolumName());
	}
	else if (id[0] == '/')
	{
	    snprintf(m_pchDownPath, PATH_SZ, "%.16s", id);
	}
	else
	{
	    //sprintf(m_pchDownPath, "/%s01/files", "disk");
	    //snprintf(m_pchDownPath, PATH_SZ, "/%s01/files", ((DownloadServer *)m_pMainProcess)->GetVolName());
	    //snprintf(m_pchDownPath, PATH_SZ, "/%s01/files", ((DownloadServer *)m_pMainProcess)->GetVolName());
	    snprintf(m_pchDownPath, PATH_SZ, "/%s01/files", GetVolumName());
	}
}

void ClientUserDN::SetDownSpeed(const char *id)
{
    DownloadServer *pServer = NULL;
    if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
    {
		m_iBandWidth = pServer->GetBandwidth(id[1]);
    }
    else
    {
        CNPLog::GetInstance().Log("SetDownSpeed(%p) pServer is NULL!! ", this);
		m_iBandWidth = 1024*1024*5;
    }

/*
CNPLog::GetInstance().Log("SetDownSpeed id=(%s), Bandwidth=(%d) ", id, m_iBandWidth);
	m_iBandWidth = BADN_MAX;

    if (id[0] == 'd' && id[1] == '1')
    {
        return;
    }

    if (id[1] == '1')
    {
        m_iBandWidth = BAND_1;
    }
    else if (id[1] == '2')
    {
        m_iBandWidth = BAND_2;
    }
    else if (id[1] == '3')
    {
        m_iBandWidth = BAND_3;
    }
    else if (id[1] == '4')
    {
        m_iBandWidth = BAND_4;
    }
    else if (id[1] == '5')
    {
        m_iBandWidth = BAND_5;
    }
    else if (id[1] == '6')
    {
        m_iBandWidth = BAND_6;
    }
    else if (id[1] == '7')
    {
        m_iBandWidth = BAND_7;
    }
    else if (id[1] == '8')
    {
        m_iBandWidth = BAND_8;
    }
    else if (id[1] == '9')
    {
        m_iBandWidth = BAND_9;
    }
*/
}

void ClientUserDN::SendPacket(const T_PACKET* _pPacket)
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
		CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
	}
	delete _pPacket;
#endif
}

void ClientUserDN::SendCloseToMgr()
{
	// send to Mgr close info
	T_PACKET tSendPacket;
	Tcmd_USER_CLOSE_DS_DSM *sndbody = (Tcmd_USER_CLOSE_DS_DSM *)tSendPacket.data;
	tSendPacket.header.command  = cmd_USER_CLOSE_DS_DSM;
	tSendPacket.header.length   = sizeof(Tcmd_USER_CLOSE_DS_DSM);

	if(m_tFileInfo.nComCode <= 0 &&  m_tFileInfo.nBillNo <= 0)
	{
		return;
	}

	sndbody->nComCode   = GetComCode();
	sndbody->nBillNo    = GetBillNo();
	sndbody->nDownSize 	= GetTotalSendSize();
	sndbody->dClosedTime = CNPUtil::GetMicroTime();

    DownloadServer *pServer = NULL;
    if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
    {
		pServer->GetSendPipeClient()->Write((char *)&tSendPacket,
													PDUHEADERSIZE + tSendPacket.header.length);
    }
    else
    {
        CNPLog::GetInstance().Log("Work SendCloseToMgr(%p) pServer is NULL!! ", this);
    }

/*
	CNPLog::GetInstance().Log("SendCloseToMgr (%p), billno=(%d)(%llu)",
					this, GetBillNo(), GetTotalSendSize());
*/
}

void ClientUserDN::WorkGetFileSize(const T_PACKET &_tPacket)
{
    Tcmd_GET_FSIZE_DC_DS *pClientBody = (Tcmd_GET_FSIZE_DC_DS *)_tPacket.data ;

    Tcmd_GET_FSIZE_DC_DS *sndbody     = (Tcmd_GET_FSIZE_DC_DS *)m_tSendPacket.data;
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
    m_tSendPacket.header.command = cmd_GET_FSIZE_DS_DC;
    m_tSendPacket.header.length  = sizeof(Tcmd_GET_FSIZE_DC_DS);

	sndbody->iCmd 	= cmd_GET_FSIZE_DS_DC;

/*
	char pchTmpFileName[128];
	memset(pchTmpFileName, 0x00, sizeof(pchTmpFileName));
	strcpy(pchTmpFileName, pClientBody->pchFileName);
	CNPUtil::replaceAll(pchTmpFileName);
	snprintf(m_tFileInfo.sFileName, FILE_SZ, "%s", pchTmpFileName);
*/

	CNPUtil::replaceAll(pClientBody->pchFileName, "\\", "/");
	snprintf(m_tFileInfo.sFileName, FILE_SZ, "%s", pClientBody->pchFileName);

	SetFilePath(pClientBody->pchID);

	char pchFullFileName[FULL_SZ+1];
	memset(pchFullFileName, 0x00, sizeof(pchFullFileName));
	snprintf(pchFullFileName, FULL_SZ, "%s/%s", m_pchDownPath, m_tFileInfo.sFileName);

	CNPUtil::LTrim(pchFullFileName);
	CNPUtil::RTrim(pchFullFileName);

	if(SetFileStat(pchFullFileName) < 0)
	{
		CNPLog::GetInstance().Log("Not Exist File ClientUserDN(%p):: FileName=(%s), pchID=(%s)",
			this, pchFullFileName, pClientBody->pchID);

		sndbody->iCmd 	= cmd_DFILE_NOT_EXIST;

		if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
		{
			CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
		}
		return;
	}

	sndbody->nFSize	= m_tFileStat.st_size;

	strncpy(m_tFileInfo.sID, 		pClientBody->pchID, ID_SZ);
	strncpy(sndbody->pchID, 		pClientBody->pchID, ID_SZ);
	strncpy(sndbody->pchFileName, 	pClientBody->pchFileName, FILE_SZ);

#ifndef _CLIENT_ARRAY
	if(m_iBandWidth == 0)
#endif
	{
		SetDownSpeed(pClientBody->pchID);
	}

/*
	CNPLog::GetInstance().Log("in GET_FSIZE_DC_DS ClientUserDN(%p):: \
comcode=(%d), billno=(%d), sndbody->pchFileName=(%s), id=(%s), band=(%d)",
			this, m_tFileInfo.nComCode, m_tFileInfo.nBillNo, sndbody->pchFileName, pClientBody->pchID, m_iBandWidth);
*/

	char pchIP[16];
	memset(pchIP, 0x00, sizeof(pchIP));
	strncpy(pchIP, static_cast<TcpSocket *>(GetSocket())->GetClientIpAddr(), 16);
	if(strcmp( "116.124.252.236", pchIP) == 0 ||
	    strcmp("116.124.252.237", pchIP) == 0 ||
	    strcmp("116.124.252.238", pchIP) == 0 ||
	    strcmp("116.124.252.239", pchIP) == 0 ||
	    strcmp("116.124.252.240", pchIP) == 0 ||
	    strcmp("116.124.252.241", pchIP) == 0 ||
	    strcmp("116.124.252.40", pchIP) == 0 ||
	    strcmp("116.124.252.41", pchIP) == 0 ||
	    strcmp("116.124.253.251", pchIP) == 0 ||
	    strcmp("116.124.253.252", pchIP) == 0 ||
	    strcmp("118.216.107.53", pchIP) == 0 ||
	    strcmp("118.216.107.54", pchIP) == 0 ||
	    strcmp("211.37.6.121", pchIP) == 0 ||
	    strcmp("211.37.6.122", pchIP) == 0 )
	{
		CNPLog::GetInstance().Log("Request File FDS IP=[%s], ID=[%s], ComCode=[%d], FileName=[%s]",
						pchIP
						,pClientBody->pchID
						,m_tFileInfo.nComCode
						, sndbody->pchFileName);
	}
	else
	{
		CNPLog::GetInstance().Log("Request File Client IP=[%s], ID=[%s], ComCode=[%d], FileName=[%s]",
						(static_cast<TcpSocket *>(GetSocket()))->GetClientIpAddr()
						,pClientBody->pchID
						,m_tFileInfo.nComCode
						, sndbody->pchFileName);
	}

	if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	{
		CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
	}

	m_dFileSendStartTime = CNPUtil::GetMicroTime();
}

#ifndef _ONESHOT
void ClientUserDN::WorkGetFile(const T_PACKET &_tPacket)
{
    Tcmd_GET_FILE_DC_DS *pClientBody = (Tcmd_GET_FILE_DC_DS *)_tPacket.data ;
    T_PACKET *pSendPacket = new T_PACKET;
    //Tcmd_GET_FILE_DS_DC *sndbody     = (Tcmd_GET_FILE_DS_DC *)pSendPacket->data;

#ifdef _DEBUG
	CNPLog::GetInstance().Log("ClientUserDN::WorkGetFile(%p) cmd=(%d), ComCode=(%d), fileName=(%s), offset=(%lld), downsize=(%lld), id=(%s), billno=(%d)",
									this,
									pClientBody->iCmd,
									m_tFileInfo.nComCode,
									pClientBody->pchFileName,
									pClientBody->nOffset,
									pClientBody->nDownSize,
									pClientBody->pchID,
									pClientBody->nBillNo);
#endif

	m_tFileInfo.nOffset 	= pClientBody->nOffset;
	m_tFileInfo.nDownSize 	= pClientBody->nDownSize;
	m_tFileInfo.nBillNo 	= pClientBody->nBillNo;

    pSendPacket->header.command = cmd_GET_FILE_DS_DC;
    pSendPacket->header.length = pClientBody->nDownSize;

	if (
		(m_tFileStat.st_size <= 0) ||
				((m_tFileInfo.nOffset > m_tFileStat.st_size) ||
				((m_tFileInfo.nOffset + m_tFileInfo.nDownSize) > m_tFileStat.st_size) )
		)
	{
    	pSendPacket->header.command = cmd_DFILE_ERROR;
		CNPLog::GetInstance().Log("file stat error OR already downloaded(%p) offset=(%llu),size=(%llu),reqsize=(%llu)",
									this, m_tFileInfo.nOffset,m_tFileStat.st_size,m_tFileInfo.nDownSize);
		pthread_mutex_lock(&m_mtLst);
		m_lstSendPacket.push_back(pSendPacket);
		pthread_mutex_unlock(&m_mtLst);
		return;
	}

	// file�� ����.
	if(m_tFileInfo.iFileFd < 0)
	{
		char pchFullFileName[FULL_SZ+1];
		memset(pchFullFileName, 0x00, sizeof(pchFullFileName));
		snprintf(pchFullFileName, FULL_SZ, "%s/%s", m_pchDownPath, m_tFileInfo.sFileName);

#ifdef _FREEBSD
		if((m_tFileInfo.iFileFd = open((const char *)pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#else
        if((m_tFileInfo.iFileFd = open64(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#endif
		{
    		pSendPacket->header.command = cmd_DFILE_ERROR;
			CNPLog::GetInstance().Log("file open error(%p), (%s)", this, strerror(errno));
			pthread_mutex_lock(&m_mtLst);
			m_lstSendPacket.push_back(pSendPacket);
			pthread_mutex_unlock(&m_mtLst);
			return;
		}
	}

	if(m_eSendMode == SEND_WIDTH_HEADER)
	{
		CNPLog::GetInstance().Log("Sendfile with header (%p)", this);
		if(GetSocket()->Write((char *)pSendPacket, PDUHEADERSIZE) < 0)
		{
			CNPLog::GetInstance().Log("In Sendfile with header Write Error (%p)", this);
		}
	}

	pthread_mutex_lock(&m_mtLst);
	m_lstSendPacket.push_back(pSendPacket);
	pthread_mutex_unlock(&m_mtLst);
}
#else
int ClientUserDN::WorkGetFile(const T_PACKET &_tPacket)
{
    Tcmd_GET_FILE_DC_DS *pClientBody = (Tcmd_GET_FILE_DC_DS *)_tPacket.data ;

	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
    m_tSendPacket.header.command = cmd_GET_FILE_DS_DC;
    m_tSendPacket.header.length  = pClientBody->nDownSize;

#ifdef _DEBUG
	CNPLog::GetInstance().Log("ClientUserDN::WorkGetFile(%p) cmd=(%d), ComCode=(%d), fileName=(%s), offset=(%lld), downsize=(%lld), id=(%s), billno=(%d)",
									this,
									pClientBody->iCmd,
									m_tFileInfo.nComCode,
									pClientBody->pchFileName,
									pClientBody->nOffset,
									pClientBody->nDownSize,
									pClientBody->pchID,
									pClientBody->nBillNo);
#endif

	m_tFileInfo.nOffset 	= pClientBody->nOffset;
	m_tFileInfo.nDownSize 	= pClientBody->nDownSize;
	m_tFileInfo.nBillNo 	= pClientBody->nBillNo;

	if (
		(m_tFileStat.st_size <= 0) ||
				((m_tFileInfo.nOffset > m_tFileStat.st_size) ||
				((m_tFileInfo.nOffset + m_tFileInfo.nDownSize) > m_tFileStat.st_size) )
		)
	{
		CNPLog::GetInstance().Log("file stat error OR already downloaded(%p) offset=(%llu),size=(%llu),reqsize=(%llu)",
									this, m_tFileInfo.nOffset,m_tFileStat.st_size,m_tFileInfo.nDownSize);

    	m_tSendPacket.header.command = cmd_DFILE_ERROR;
		if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
		{
			CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
		}
		return 0;
	}

	// file�� ����.
	if(m_tFileInfo.iFileFd < 0)
	{
		char pchFullFileName[FULL_SZ+1];
		memset(pchFullFileName, 0x00, sizeof(pchFullFileName));
		snprintf(pchFullFileName, FULL_SZ, "%s/%s", m_pchDownPath, m_tFileInfo.sFileName);

#ifdef _FREEBSD
		if((m_tFileInfo.iFileFd = open(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#else
        if((m_tFileInfo.iFileFd = open64(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#endif
		{
			CNPLog::GetInstance().Log("file open error(%p), (%s)", this, strerror(errno));

    		m_tSendPacket.header.command = cmd_DFILE_ERROR;
			if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
			{
				CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
			}
			return 0;
		}
	}

	if(m_eSendMode == SEND_WIDTH_HEADER)
	{
		CNPLog::GetInstance().Log("Sendfile with header (%p)", this);
		if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE) < 0)
		{
			CNPLog::GetInstance().Log("In Sendfile with header Write Error (%p)", this);
		}
	}

	return 1;
}
#endif

/*
#ifdef _CDN
#endif
*/
#define DFLAG_PATCH         1   // ��ġ ���� �ٿ�ε�
#define DFLAG_INSTALL       2   // �ν��� ���� �ٿ�ε�
#define DFLAG_FILES         3   // ���� ���� �ٿ�ε�

void ClientUserDN::get_down_path_ex(char *id, char *down_path, int dflag)
{
#ifdef _CDN_HS
    if (id[0] == 'a')
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk01/ccn/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk01/ccn/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk01/ccn/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
    else if (id[0] == 'b')
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk02/ccn/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk02/ccn/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk02/ccn/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
    else if (id[0] == '/')
    {
        sprintf(down_path, "%.16s", id);
    }
    else
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk02/ccn/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk02/ccn/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk02/ccn/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
#else
    if (id[0] == 'a')
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk01/files/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk01/files/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk01/files/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
    else if (id[0] == 'b')
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk02/files/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk02/files/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk02/files/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
    else if (id[0] == '/')
    {
        sprintf(down_path, "%.16s", id);
    }
    else
    {
        if (dflag == DFLAG_PATCH)
            sprintf(down_path, "/disk02/files/%u/%u/PATCH", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else if (dflag == DFLAG_INSTALL)
            sprintf(down_path, "/disk02/files/%u/%u/INSTALL", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
        else
            sprintf(down_path, "/disk02/files/%u/%u/FILES", m_tFileInfo.nComCode, m_tFileInfo.nSvcCode);
    }
#endif
}

int ClientUserDN::WorkGetCDNFile(const T_PACKET &_tPacket)
{
    Tcmd_GET_FILE_EX_DC_DS *pClientBody = (Tcmd_GET_FILE_EX_DC_DS *)_tPacket.data ;

	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
    m_tSendPacket.header.command = cmd_GET_FILE_EX_DS_DC;
    m_tSendPacket.header.length  = pClientBody->_Data.d.nDownSize;

#ifdef _DEBUG
	CNPLog::GetInstance().Log("ClientUserDN::WorkGetFile(%p) cmd=(%d), ComCode=(%d), fileName=(%s), offset=(%lld), downsize=(%lld), id=(%s), billno=(%d), dir(%s), flag=(%d)",
									this,
									pClientBody->_Data.d.iCmd,
									m_tFileInfo.nComCode,
									pClientBody->_Data.d.pchFileName,
									pClientBody->_Data.d.nOffset,
									pClientBody->_Data.d.nDownSize,
									pClientBody->_Data.d.pchID,
									pClientBody->_Data.d.nBillNo,
									pClientBody->_Data.d.sDir,
									pClientBody->_Data.d.nDFlag);
#endif

	m_tFileInfo.nOffset 	= pClientBody->_Data.d.nOffset;
	m_tFileInfo.nDownSize 	= pClientBody->_Data.d.nDownSize;
	m_tFileInfo.nBillNo 	= pClientBody->_Data.d.nBillNo;

/*
	if (
		(m_tFileStat.st_size <= 0) ||
				((m_tFileInfo.nOffset > m_tFileStat.st_size) ||
				((m_tFileInfo.nOffset + m_tFileInfo.nDownSize) > m_tFileStat.st_size) )
		)
	{
		CNPLog::GetInstance().Log("file stat error OR already downloaded(%p) offset=(%llu),size=(%llu),reqsize=(%llu)",
									this, m_tFileInfo.nOffset,m_tFileStat.st_size,m_tFileInfo.nDownSize);

    	m_tSendPacket.header.command = cmd_DFILE_ERROR;
		if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
		{
			CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
		}
		return 0;
	}
*/
	snprintf(m_tFileInfo.sFileName, FILE_SZ, "%s", pClientBody->_Data.d.pchFileName);

    // file�� ����.
    if(m_tFileInfo.iFileFd < 0)
    {
        char down_path[612] = {0,};
        get_down_path_ex(pClientBody->_Data.d.pchID, down_path, pClientBody->_Data.d.nDFlag);

        char pchFullFileName[612] = {0,};
        sprintf(pchFullFileName, "%s/%s/%s", down_path, pClientBody->_Data.d.sDir, m_tFileInfo.sFileName);

#ifdef _FREEBSD
        if((m_tFileInfo.iFileFd = open(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#else
        if((m_tFileInfo.iFileFd = open64(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#endif
        {
            CNPLog::GetInstance().Log("file open error(%p), (%s)", this, strerror(errno));

            m_tSendPacket.header.command = cmd_DFILE_ERROR;
            if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
            {
                CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
            }
            return 0;
        }

/*
        char pchFullFileName[FULL_SZ+1];
        memset(pchFullFileName, 0x00, sizeof(pchFullFileName));
        snprintf(pchFullFileName, FULL_SZ, "%s/%s", m_pchDownPath, m_tFileInfo.sFileName);
*/

        // cdn..
        if(SetFileStat(pchFullFileName) < 0)
        {
CNPLog::GetInstance().Log("Not Exist File ClientUserDN CDN(%p):: path=(%s),reqfile=(%s), pchID=(%s)",
                this, m_pchDownPath, pClientBody->_Data.d.pchFileName, pClientBody->_Data.d.pchID);

            m_tSendPacket.header.command = cmd_DFILE_ERROR;
            if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
            {
                CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
            }
            return 0;
        }
        strncpy(m_tFileInfo.sID,        pClientBody->_Data.d.pchID, ID_SZ);

        // check
        if (
            (m_tFileStat.st_size <= 0) ||
                    ((m_tFileInfo.nOffset > m_tFileStat.st_size) ||
                    ((m_tFileInfo.nOffset + m_tFileInfo.nDownSize) > m_tFileStat.st_size) )
            )
        {
            CNPLog::GetInstance().Log("CDN file stat error OR already downloaded CDN(%p) offset=(%llu),size=(%llu),reqsize=(%llu)",
                                        this, m_tFileInfo.nOffset,m_tFileStat.st_size,m_tFileInfo.nDownSize);

            m_tSendPacket.header.command = cmd_DFILE_ERROR;
            if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
            {
                CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
            }
            return 0;
        }

#ifdef _FREEBSD
        if((m_tFileInfo.iFileFd = open(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#else
        if((m_tFileInfo.iFileFd = open64(pchFullFileName, O_RDONLY | O_NONBLOCK)) < 0)
#endif
        {
            CNPLog::GetInstance().Log("file open error(%p), (%s)", this, strerror(errno));

            m_tSendPacket.header.command = cmd_DFILE_ERROR;
            if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
            {
                CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
            }
            return 0;
        }
    }

/*
	if(m_eSendMode == SEND_WIDTH_HEADER)
	{
		CNPLog::GetInstance().Log("Sendfile with header (%p)", this);
		if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE) < 0)
		{
			CNPLog::GetInstance().Log("In Sendfile with header Write Error (%p)", this);
		}
	}
*/

	return 1;
}

void ClientUserDN::WorkHelloCDN(const T_PACKET &_tPacket)
{
    Tcmd_HELLO_CODES_DC_DS *pClientBody = (Tcmd_HELLO_CODES_DC_DS *)_tPacket.data ;

	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
    m_tSendPacket.header.command = cmd_HELLO_DS_DC;
    m_tSendPacket.header.length  = 0;


	DownloadServer *pServer = NULL;
	if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
	{
		m_iComCodeIdx = pServer->GetComCodeIdx(pClientBody->nComCode);
	}

	m_tFileInfo.nComCode = pClientBody->nComCode;
	m_tFileInfo.nSvcCode = pClientBody->nSvcCode;

	//CNPLog::GetInstance().Log("ClientUserDN(%p) Hello ComCode=(%d)", this, m_tFileInfo.nComCode);
    //if(((Socket *)(GetSocket()))->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	//if(static_cast<Socket*>(GetSocket())->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
    {
        CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
    }

	//CNPLog::GetInstance().Log("ClientUserDN(%p) Hello comcode=(%d)", this, m_tFileInfo.nComCode);
}

void ClientUserDN::WorkHello(const T_PACKET &_tPacket)
{
    Tcmd_HELLO_DC_DS *pClientBody = (Tcmd_HELLO_DC_DS *)_tPacket.data ;

	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
    m_tSendPacket.header.command = cmd_HELLO_DS_DC;
    m_tSendPacket.header.length  = 0;

	m_tFileInfo.nComCode = pClientBody->iComCode;

	if(pClientBody->iComCode == D_D)
	{
		DownloadServer *pServer = NULL;
		if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
		{
			pServer->SetD();
			//m_pMainProcess->SetSignalNo(D_D);
		}
	}

	DownloadServer *pServer = NULL;
	if((pServer = dynamic_cast<DownloadServer*>(m_pMainProcess)))
	{
		m_iComCodeIdx = pServer->GetComCodeIdx(pClientBody->iComCode);
	}

	//CNPLog::GetInstance().Log("ClientUserDN(%p) Hello ComCode=(%d)", this, m_tFileInfo.nComCode);
    //if(((Socket *)(GetSocket()))->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	//if(static_cast<Socket*>(GetSocket())->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
    {
        CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
    }

	//CNPLog::GetInstance().Log("ClientUserDN(%p) Hello comcode=(%d)", this, m_tFileInfo.nComCode);
}

void ClientUserDN::WorkPing(const T_PACKET &_tPacket)
{
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
	m_tSendPacket.header.command 	= cmd_HEARTBEAT_DS_DC;
	m_tSendPacket.header.length 	= 0;

	CNPLog::GetInstance().Log("ClientUserDN::WorkPing(%p)", this);
    //if(((Socket *)(GetSocket()))->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	//if(static_cast<Socket*>(GetSocket())->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
    {
        CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
    }
}

void ClientUserDN::WorkGoodBye(const T_PACKET &_tPacket)
{
	memset((char *)&m_tSendPacket, 0x00, sizeof(T_PACKET));
	m_tSendPacket.header.command 	= cmd_GOODBYE_DS_DC;
	m_tSendPacket.header.length 	= 0;

	CNPLog::GetInstance().Log("ClientUserDN::WorkGoodBye(%p)", this);
    //if(((Socket *)(GetSocket()))->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	//if(static_cast<Socket*>(GetSocket())->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
	if(GetSocket()->Write((char *)&m_tSendPacket, PDUHEADERSIZE+m_tSendPacket.header.length) < 0)
    {
        CNPLog::GetInstance().Log("In ClientUserDN Write Error (%p)", this);
    }
}

const int ClientUserDN::ExecuteCommand(Thread *_pThread)
{
	T_PACKET tPacket;

	/*
	int iCommand   = CNPUtil::Get2Byte((unsigned char *)m_cCBuff.GetHeaderPoint(), 0);
	int iPacketLen = CNPUtil::Get2Byte((unsigned char *)m_cCBuff.GetHeaderPoint()+COMMAND_SIZE, 0);
	*/
    PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();

	memset((char *)&tPacket, 0x00, sizeof(tPacket));
	if(Client::GetPacket((char *)&tPacket, pPacketHeader->length + PDUHEADERSIZE) < 0)
	{
		CNPLog::GetInstance().Log("In ClientUserDN::ExecuteCommand() GetPacketError! ");
		return -1;
	}

#ifdef _DEBUG
	CNPLog::GetInstance().Log("In ClientUserDN::ExecuteCommand (%p) command=(%d)", this,
			tPacket.header.command);
#endif

    switch(tPacket.header.command)
    {
	// DC -> DS
    case cmd_HELLO_DC_DS:
		WorkHello(tPacket);
        break;

	// 1. C => S (ComCode)
    case cmd_HELLO_COMCODE_DC_DS:    	// client�� cmd_HELLO_DC_DS �� ������ �ʰ�, cmd_HELLO_COMCODE_DC_DS �� �����±�.
		WorkHello(tPacket);
        break;

    case cmd_HELLO_CODES_DC_DS:
		WorkHelloCDN(tPacket);
        break;

	case cmd_GET_FILE_EX_DC_DS:
		return WorkGetCDNFile(tPacket);
        break;
/*
#ifdef _CDN
#endif
*/
	// 2. C => S (pchID, pchFileName, nFSize)
    case cmd_GET_FSIZE_DC_DS:
		SetSendMode(SEND_ONLY_FILE);
		WorkGetFileSize(tPacket);
        break;

    case cmd_GET_FSIZE_DC_DS2:
		SetSendMode(SEND_WIDTH_HEADER);
		WorkGetFileSize(tPacket);
        break;

	// 3. C => S (pchID, pchFileName, nOffset, nDownSize, nBillNo)
    case cmd_GET_FILE_DC_DS:
#ifndef _ONESHOT
		WorkGetFile(tPacket);
#else
		return WorkGetFile(tPacket);
#endif
        break;

    case cmd_HEARTBEAT_DC_DS:
		WorkPing(tPacket);
        break;

    case cmd_GOODBYE_DC_DS:
		WorkGoodBye(tPacket);
        break;

	// ===>> DS command

    default :
        CNPLog::GetInstance().Log("UNKNOWN PDU TYPE(%p), (%d)", this, tPacket.header.command);
    }

	return 0;
}

