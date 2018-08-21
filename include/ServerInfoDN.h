#ifndef __SERVERINFODN_H
#define __SERVERINFODN_H

#include "./Include.h"
#include "./Properties.h"
#include "./Packet.h"

//const unsigned int IP_LEN = 20;
//const unsigned int LOG_FILE_LEN = 256;
const unsigned int MAX_URL    = 256;
const unsigned int MAX_VOLUME   = 32;
const unsigned int MAX_DIRNAME  = 32;

typedef map<ENUM_SERVERSOCKET_TYPE, int> serverInfoMap;
typedef serverInfoMap::value_type   serverInfoValType;
typedef serverInfoMap::iterator     serverInfoMapItor;

typedef map<ENUM_THREAD_TYPE, int> threadMap;
typedef threadMap::value_type   threadValType;
typedef threadMap::iterator     threadMapItor;

class ServerInfoDN
{
  private:
    static const int MAX_BAND_CNT = 10;

    serverInfoMap   m_mapServerPort;  // server port list
    threadMap     m_mapThreadCount; // thread list

    int   m_iMaxUser;         // max connection user
    int   m_iShmKey;
    //  int   m_iMgrServerPort;
    //  int   m_iMessageQ;
    char  m_pchIPAddr[MAX_IP_LEN];  // ip addr
    char  m_pchLogFile[LOG_FILE_LEN]; // log filename length
    char  m_pchUnixSockPath[UNIX_PATH_MAX];

    char    m_pchMRTGURL[MAX_URL];
    char    m_pchVolName[MAX_VOLUME];
    char    m_pchDirName[MAX_DIRNAME];
    char    m_pchManagerIpAddresses[MAX_URL];

    // added 090423
    unsigned int iBandwidth[MAX_BAND_CNT];

    const unsigned int DefineBandwidth(char* const _pchBand);
  public:
    ServerInfoDN();
    ServerInfoDN(Properties &_cProperties);
    ~ServerInfoDN();

    const int GetShmKey();
    const int   GetMaxUser();
    void  SetMaxUser(const int _iMaxUser);
    const char* const GetLogFileName();
    const char* const GetIPAddr();
    //  const int GetMessageQID();
    const char* const GetUnixSockPath();

    const char* const GetMRTGURL();
    const char* const GetVolName();
    const char* const GetDirName();
    const char* const GetManagerIpAddresses();

    const serverInfoMap&    GetPortMap();
    const int GetPort(const ENUM_SERVERSOCKET_TYPE _type);  // ��û�� port type�� ���� �ش� port�� return �Ѵ�.
    const int GetThreadCount(const ENUM_THREAD_TYPE _type);

    const unsigned int  GetBandwidth(const unsigned char _chID);
};

#endif
