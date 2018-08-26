#ifndef __SERVERINFODNMGR_H
#define __SERVERINFODNMGR_H

#include "./Include.h"
#include "./Properties.h"
#include "./Packet.h"

const unsigned int MAX_URL = 256;

typedef map<ENUM_SERVERSOCKET_TYPE, int> serverInfoMap;
typedef serverInfoMap::value_type   serverInfoValType;
typedef serverInfoMap::iterator     serverInfoMapItor;

typedef map<ENUM_THREAD_TYPE, int> threadMap;
typedef threadMap::value_type   threadValType;
typedef threadMap::iterator     threadMapItor;

class ServerInfoDNMgr
{
  private:
    serverInfoMap   m_mapServerPort;  // server port list
    threadMap     m_mapThreadCount; // thread list

    int   m_iMaxUser;         // max connection user
    int   m_iDSMaxUser;         // max connection user
    int   m_iDNPort;          //
    int   m_iDNCnt;         //
    int   m_iShmKey;          //
    int   m_iShmDSStatus;         //
    char  m_pchIPAddr[MAX_IP_LEN];    // ip addr
    char  m_pchLogFile[LOG_FILE_LEN]; // log filename length

    char  m_pchMRTGURL[MAX_URL];

  public:
    ServerInfoDNMgr();
    ServerInfoDNMgr(Properties &_cProperties);
    ~ServerInfoDNMgr();

    const int   GetMaxUser();
    const int   GetDSMaxUser();
    const char* const GetLogFileName();
    const char* const GetIPAddr();

    const char* const GetMRTGURL();
    const int GetDNCnt();
    const int GetShmKey();
    const int GetShmDSStatus();
    //  const int GetMessageQID();
    //  const char* const GetUnixSockPath();

    const serverInfoMap&    GetPortMap();
    const int GetPort(const ENUM_SERVERSOCKET_TYPE _type);  
    const int GetThreadCount(const ENUM_THREAD_TYPE _type);

    const int GetDNPort();
};

#endif
