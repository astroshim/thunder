#include "../include/ServerInfoDN.h"
#include "../include/NPUtil.h"
#include "../include/NPDebug.h" //Assert
#include "../include/NPLog.h"   //Log

ServerInfoDN::ServerInfoDN()
  : m_iMaxUser(0)
    , m_iShmKey(0)
    //, m_iMgrServerPort(0)
    //, m_iMessageQ(0)
{
  memset(m_pchIPAddr, 0x00, sizeof(char)*MAX_IP_LEN);
  memset(m_pchMRTGURL, 0x00, sizeof(char)*MAX_URL);
  memset(m_pchVolName, 0x00, sizeof(char)*MAX_VOLUME);
  memset(m_pchDirName, 0x00, sizeof(char)*MAX_DIRNAME);
  memset(m_pchManagerIpAddresses, 0x00, sizeof(char)*MAX_URL);
  //  memset(m_pchUnixSockPath, 0x00, sizeof(char)*UNIX_PATH_MAX);
}

ServerInfoDN::~ServerInfoDN()
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

ServerInfoDN::ServerInfoDN(Properties &_cProperties)
{
  for(int i = 0; i < MAX_BAND_CNT; i++)
  {
    iBandwidth[i] = 0;
  }

  memset(m_pchIPAddr, 0x00, sizeof(char)*MAX_IP_LEN);
  if(_cProperties.GetProperty("SERVER_IP") != "")
  {
    if(strlen(_cProperties.GetProperty("SERVER_IP").c_str()) > MAX_IP_LEN)
    {
      Assert(false, "In ServerInfoDN SERVER_IP Length over! ");
    }

    strncpy(m_pchIPAddr, _cProperties.GetProperty("SERVER_IP").c_str(), MAX_IP_LEN);
  }

  if(_cProperties.GetProperty("SERVER_PORT_DNMGR") != "")
  {
    m_mapServerPort.insert(serverInfoValType(SERVER_PORT_MGR, atoi(_cProperties.GetProperty("SERVER_PORT_DNMGR").c_str())));
  }

  if(_cProperties.GetProperty("SERVER_PORT") != "")
  {
    m_mapServerPort.insert(serverInfoValType(SERVER_PORT, atoi(_cProperties.GetProperty("SERVER_PORT").c_str())));
  }

  if(_cProperties.GetProperty("DNMGR_SERVER_IPs") != "")
  {
    strncpy(m_pchManagerIpAddresses, _cProperties.GetProperty("DNMGR_SERVER_IPs").c_str(), MAX_URL);
  }

  /*
     if(_cProperties.GetProperty("MAX_USER") != "")
     {
     m_iMaxUser  = atoi(_cProperties.GetProperty("MAX_USER").c_str());
     }
     */

  // log file
  memset(m_pchLogFile, 0x00, sizeof(char)*LOG_FILE_LEN);
  if(_cProperties.GetProperty("SERVER_LOGFILE") != "")
  {
    if(strlen(_cProperties.GetProperty("SERVER_LOGFILE").c_str()) > LOG_FILE_LEN)
    {
      Assert(false, "In ServerInfoDN SERVER_LOGFILE Length over! ");
    }

    strncpy(m_pchLogFile, _cProperties.GetProperty("SERVER_LOGFILE").c_str(), LOG_FILE_LEN);
  }

  if(_cProperties.GetProperty("THREAD_SENDER") != "")
  {
    m_mapThreadCount.insert(threadValType(THREAD_SENDER, atoi(_cProperties.GetProperty("THREAD_SENDER").c_str())));
  }

  if(_cProperties.GetProperty("THREAD_RECEIVER") != "")
  {
    m_mapThreadCount.insert(threadValType(THREAD_RECEIVER, atoi(_cProperties.GetProperty("THREAD_RECEIVER").c_str())));
  }

  if(_cProperties.GetProperty("THREAD_BROADCASTER") != "")
  {
    m_mapThreadCount.insert(threadValType(THREAD_BROADCASTER, atoi(_cProperties.GetProperty("THREAD_BROADCASTER").c_str())));
  }

  if(_cProperties.GetProperty("MRTG_URL") != "")
  {
    strncpy(m_pchMRTGURL, _cProperties.GetProperty("MRTG_URL").c_str(), MAX_URL);
  }

  if(_cProperties.GetProperty("VOLNAME") != "")
  {
    strncpy(m_pchVolName, _cProperties.GetProperty("VOLNAME").c_str(), MAX_VOLUME);
  }

  if(_cProperties.GetProperty("DIRECTORY_NAME") != "")
  {
    strncpy(m_pchDirName, _cProperties.GetProperty("DIRECTORY_NAME").c_str(), MAX_DIRNAME);
  }

  if(_cProperties.GetProperty("SHMKEY") != "")
  {
    m_iShmKey   = atoi(_cProperties.GetProperty("SHMKEY").c_str());
  }

  /*
     if(_cProperties.GetProperty("MESSAGEQ_ID") != "")
     {
     m_iMessageQ = atoi(_cProperties.GetProperty("MESSAGEQ_ID").c_str());
     }

     if(_cProperties.GetProperty("UNIX_SOCK_PATH") != "")
     {
     strcpy(m_pchUnixSockPath, _cProperties.GetProperty("UNIX_SOCK_PATH").c_str());
     }
     */

  if(_cProperties.GetProperty("BAND_MAX") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND_MAX").c_str();
    iBandwidth[0] = DefineBandwidth(pchTmp);

    /*
       if(pchTmp[strlen(pchTmp)-1] == 'M' ||
       pchTmp[strlen(pchTmp)-1] == 'm')
       {
       pchTmp[strlen(pchTmp)-1] = 0x00;
       iBandMax = atoi(pchTmp) * 1024 * 1024;
       }
       else
       if( pchTmp[strlen(pchTmp)-1] == 'K' ||
       pchTmp[strlen(pchTmp)-1] == 'k')
       {
       pchTmp[strlen(pchTmp)-1] = 0x00;
       iBandMax = atoi(pchTmp) * 1024;

       }
       else  // bytes
       {
       iBandMax = atoi(pchTmp) * 1024;
       }
       */
  }

  if(_cProperties.GetProperty("BAND1") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND1").c_str();
    iBandwidth[1] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND2") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND2").c_str();
    iBandwidth[2] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND3") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND3").c_str();
    iBandwidth[3] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND4") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND4").c_str();
    iBandwidth[4] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND5") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND5").c_str();
    iBandwidth[5] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND6") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND6").c_str();
    iBandwidth[6] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND7") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND7").c_str();
    iBandwidth[7] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND8") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND8").c_str();
    iBandwidth[8] = DefineBandwidth(pchTmp);
  }
  if(_cProperties.GetProperty("BAND9") != "")
  {
    char *pchTmp = (char *)_cProperties.GetProperty("BAND9").c_str();
    iBandwidth[9] = DefineBandwidth(pchTmp);
  }
}

const unsigned int ServerInfoDN::GetBandwidth(const unsigned char _chID)
{
  int iPos = 0;

  if(_chID >= '0' &&
      _chID <= '9' )
  {
    iPos = _chID - '0';
  }

  return iBandwidth[iPos];
}

const unsigned int ServerInfoDN::DefineBandwidth(char* const _pchBand)
{
  unsigned int iBandwidth = 0;

  if(_pchBand[strlen(_pchBand)-1] == 'M' ||
      _pchBand[strlen(_pchBand)-1] == 'm')
  {
    char pchBand[10];
    memset(pchBand, 0x00, sizeof(pchBand));
    strncpy(pchBand, _pchBand, strlen(_pchBand)-1);
    iBandwidth = atoi(pchBand) * 1024 * 1024;
  }
  else
    if( _pchBand[strlen(_pchBand)-1] == 'K' ||
        _pchBand[strlen(_pchBand)-1] == 'k')
    {
      char pchBand[10];
      memset(pchBand, 0x00, sizeof(pchBand));
      strncpy(pchBand, _pchBand, strlen(_pchBand)-1);
      iBandwidth = atoi(pchBand) * 1024;

    }
    else    // bytes
    {
      iBandwidth = atoi(_pchBand) * 1024;
    }

  return iBandwidth;
}


const int ServerInfoDN::GetShmKey()
{
  return m_iShmKey;
}


const int ServerInfoDN::GetMaxUser()
{
  return m_iMaxUser;
}

void ServerInfoDN::SetMaxUser(const int _iMaxUser)
{
  m_iMaxUser = _iMaxUser;
}

const char* const ServerInfoDN::GetLogFileName()
{
  return m_pchLogFile;
}

const char* const ServerInfoDN::GetIPAddr()
{
  return m_pchIPAddr;
}

const char* const ServerInfoDN::GetMRTGURL()
{
  return m_pchMRTGURL;
}

const char* const ServerInfoDN::GetVolName()
{
  return m_pchVolName;
}

const char* const ServerInfoDN::GetDirName()
{
  return m_pchDirName;
}

const char* const ServerInfoDN::GetManagerIpAddresses()
{
  return m_pchManagerIpAddresses;
}

/*
   const char* const ServerInfoDN::GetUnixSockPath()
   {
   return m_pchUnixSockPath;
   }
   */

/*
   const int ServerInfoDN::GetMessageQID()
   {
   return m_iMessageQ;
   }
   */

const serverInfoMap& ServerInfoDN::GetPortMap()
{
  return m_mapServerPort;
}

const int ServerInfoDN::GetPort(const ENUM_SERVERSOCKET_TYPE _type)
{
  serverInfoMapItor itor;
  itor = m_mapServerPort.find(_type);

  if(itor != m_mapServerPort.end())
  {
    return (int)itor->second;
  }

  return -1;
}

const int ServerInfoDN::GetThreadCount(const ENUM_THREAD_TYPE _type)
{
  threadMapItor itor;
  itor = m_mapThreadCount.find(_type);

  if(itor != m_mapThreadCount.end())
  {
    return (int)itor->second;
  }

  return -1;
}

