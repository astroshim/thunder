#include "../include/ServerInfoDNMgr.h"
#include "../include/NPUtil.h"
#include "../include/NPDebug.h" //Assert
#include "../include/NPLog.h"   //Log

ServerInfoDNMgr::ServerInfoDNMgr()
        : m_iMaxUser(0)
        , m_iDSMaxUser(0)
        , m_iDNPort(0)
        , m_iDNCnt(1)
        , m_iShmKey(0)
        , m_iShmDSStatus(0)
        //, m_iMessageQ(0)
{
  memset(m_pchIPAddr, 0x00, sizeof(char)*MAX_IP_LEN);
  memset(m_pchMRTGURL, 0x00, sizeof(char)*MAX_URL);
}

ServerInfoDNMgr::~ServerInfoDNMgr()
{
/*
  std::list<T_MEMBER_INFO*>::iterator pos;

  pos = m_lstMember.begin();
  while( pos != m_lstMember.end() )
  {
      delete *pos++;
  }
*/
}

ServerInfoDNMgr::ServerInfoDNMgr(Properties &_cProperties)
{
  memset(m_pchIPAddr, 0x00, sizeof(char)*MAX_IP_LEN);
  if(_cProperties.GetProperty("SERVER_IP") != "")
  {
    if(strlen(_cProperties.GetProperty("SERVER_IP").c_str()) > MAX_IP_LEN)
    {
          Assert(false, "In ServerInfoDNMgr SERVER_IP Length over! ");
    }

    strncpy(m_pchIPAddr, _cProperties.GetProperty("SERVER_IP").c_str(), MAX_IP_LEN);
  }

  if(_cProperties.GetProperty("SERVER_PORT_DNMGR") != "")
  {
    m_mapServerPort.insert(serverInfoValType(SERVER_PORT, atoi(_cProperties.GetProperty("SERVER_PORT_DNMGR").c_str())));
  }

  if(_cProperties.GetProperty("SERVER_PORT_DN") != "")
  {
    //m_mapServerPort.insert(serverInfoValType(SERVER_PORT, atoi(_cProperties.GetProperty("SERVER_PORT_DN").c_str())));
    m_iDNPort = atoi(_cProperties.GetProperty("SERVER_PORT_DN").c_str());
  }

  if(_cProperties.GetProperty("DM_MAX_USER") != "")
  {
    m_iMaxUser  = atoi(_cProperties.GetProperty("DM_MAX_USER").c_str());
  }

  if(_cProperties.GetProperty("DS_MAX_USER") != "")
  {
    m_iDSMaxUser = atoi(_cProperties.GetProperty("DS_MAX_USER").c_str());
  }


  if(_cProperties.GetProperty("DN_CNT") != "")
  {
    m_iDNCnt  = atoi(_cProperties.GetProperty("DN_CNT").c_str());
  }

  if(_cProperties.GetProperty("SHMKEY") != "")
  {
    m_iShmKey   = atoi(_cProperties.GetProperty("SHMKEY").c_str());
  }

  if(_cProperties.GetProperty("SHM_DS_STATUS") != "")
  {
    m_iShmDSStatus  = atoi(_cProperties.GetProperty("SHM_DS_STATUS").c_str());
  }


  // log file
  memset(m_pchLogFile, 0x00, sizeof(char)*LOG_FILE_LEN);
  if(_cProperties.GetProperty("SERVER_LOGFILE") != "")
  {
    if(strlen(_cProperties.GetProperty("SERVER_LOGFILE").c_str()) > LOG_FILE_LEN)
    {
          Assert(false, "In ServerInfoDNMgr SERVER_LOGFILE Length over! ");
    }

    strncpy(m_pchLogFile, _cProperties.GetProperty("SERVER_LOGFILE").c_str(), LOG_FILE_LEN);
  }

  if(_cProperties.GetProperty("THREAD_WORKER") != "")
  {
    m_mapThreadCount.insert(threadValType(THREAD_WORKER, atoi(_cProperties.GetProperty("THREAD_WORKER").c_str())));
  }

  if(_cProperties.GetProperty("MRTG_URL") != "")
  {
    strncpy(m_pchMRTGURL, _cProperties.GetProperty("MRTG_URL").c_str(), MAX_URL);
  }

/*
    if(_cProperties.GetProperty("MESSAGEQ_ID") != "")
    {
        m_iMessageQ = atoi(_cProperties.GetProperty("MESSAGEQ_ID").c_str());
    }
*/

/*
  if(_cProperties.GetProperty("UNIX_SOCK_PATH") != "")
  {
    strcpy(m_pchUnixSockPath, _cProperties.GetProperty("UNIX_SOCK_PATH").c_str());
  }
*/
}

const int ServerInfoDNMgr::GetMaxUser()
{
    return m_iMaxUser;
}

const int ServerInfoDNMgr::GetDSMaxUser()
{
    return m_iDSMaxUser;
}

const int ServerInfoDNMgr::GetDNPort()
{
    return m_iDNPort;
}

const char* const ServerInfoDNMgr::GetLogFileName()
{
    return m_pchLogFile;
}

const char* const ServerInfoDNMgr::GetIPAddr()
{
    return m_pchIPAddr;
}

const char* const ServerInfoDNMgr::GetMRTGURL()
{
    return m_pchMRTGURL;
}

/*
const char* const ServerInfoDNMgr::GetUnixSockPath()
{
    return m_pchUnixSockPath;
}
*/

/*
const int ServerInfoDNMgr::GetMessageQID()
{
    return m_iMessageQ;
}
*/
const int ServerInfoDNMgr::GetDNCnt()
{
    return m_iDNCnt;
}

const int ServerInfoDNMgr::GetShmKey()
{
    return m_iShmKey;
}

const int ServerInfoDNMgr::GetShmDSStatus()
{
    return m_iShmDSStatus;
}

const serverInfoMap& ServerInfoDNMgr::GetPortMap()
{
    return m_mapServerPort;
}

const int ServerInfoDNMgr::GetPort(const ENUM_SERVERSOCKET_TYPE _type)
{
    serverInfoMapItor itor;
  itor = m_mapServerPort.find(_type);

  if(itor != m_mapServerPort.end())
  {
    return (int)itor->second;
  }

  return -1;
}

const int ServerInfoDNMgr::GetThreadCount(const ENUM_THREAD_TYPE _type)
{
    threadMapItor itor;
  itor = m_mapThreadCount.find(_type);

  if(itor != m_mapThreadCount.end())
  {
    return (int)itor->second;
  }

  return -1;
}

