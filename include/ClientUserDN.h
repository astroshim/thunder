#ifndef __CLIENTUSERDN_H
#define __CLIENTUSERDN_H

#include "./Client.h"

//class DownloadServer;
class ClientUserDN: public Client
{
  private:
    //  DownloadServer  *m_pDownloadServer;

    // define band width
    static const unsigned int BADN_MAX = 1024 * 1024 * 100;

    static const unsigned int MAX_THROTTLING_DELAY = 42;
    static const int PATH_SZ  = 30;
    static const int FILE_SZ  = 98;
    static const int ID_SZ    = 16;
    static const int FULL_SZ  = PATH_SZ + FILE_SZ;

    /*
       typedef enum
       {
       SEND_ONLY_FILE = 0,
       SEND_WIDTH_HEADER
       }ENUM_SEND_MODE;
     */

#ifdef _FREEBSD
    struct stat m_tFileStat;
#else
    struct stat64 m_tFileStat;
#endif

    struct TFileInfo{
      int       iFileFd;
      char            sFileName[FILE_SZ];
#ifdef _FREEBSD
      off_t         nOffset;
      size_t          nDownSize;        // download ���� ������..
#else
      uint64_t        nOffset;
      uint64_t        nDownSize;        // download ���� ������..
      // sendfile �� size�̴�.
#endif
      char      sID[ID_SZ];       // ���� �ӵ� ����.
      uint32_t    nBillNo;
      uint32_t    nComCode;
      uint32_t    nSvcCode;
#ifdef _CDN
#endif
    }m_tFileInfo;

    uint32_t  m_iBandWidth;
#ifdef _FREEBSD
    size_t  m_nTotalSendSize;
#else
    uint64_t  m_nTotalSendSize;
#endif
    char    m_pchDownPath[PATH_SZ];
    double    m_dStartTime;
    double    m_dSendTime;

    double    m_dFileSendStartTime;
    /*
       double    m_dStartFileGetTime;
       double    m_dEndFileGetTime;
     */

    ENUM_SEND_MODE    m_eSendMode;
    int     m_iComCodeIdx;  // comcode ���� ���۷��� �����ϱ� ����.(�����޸� idx)

    list <T_PACKET*>  m_lstSendPacket;

    T_PACKET m_tSendPacket;

    pthread_mutex_t   m_mtLst;

    void SetDownSpeed(const char *id);
    //void SpeedControl(unsigned long throttling_bandwidth_dl);

    void SendPacket(const T_PACKET* _pPacket);

    const int SetFileStat(const char* _pchFileName);
    const char* const GetVolumName();
    const char* const GetDirName();

    //  DownloadServer  *m_pDownloadServer;
  public:
    ClientUserDN();
    ClientUserDN(Socket* const _cSocket);
    virtual ~ClientUserDN();

#ifdef _CLIENT_ARRAY
    void InitValiable();
#endif
    void FreePacket();

    list<T_PACKET*>* const GetPacketList();
    void AddPacketFront(T_PACKET *_pPacket);
    void RemovePacket(T_PACKET *_pPacket);

    const int   GetSendPacketCount();

    const int GetFileFd();
    const char* GetFileName();
#ifdef _FREEBSD
    const off_t GetFileOffset();
#else
    const off_t GetFileOffset(); // mac
    // const off64_t GetFileOffset(); // linux
#endif
    //  void SetFileOffset(const off_t _nOffset);
    void AddFileOffset(const off_t _nOffset);

    const char* GetID();

    const uint32_t GetBillNo();
    const uint32_t GetComCode();
    const uint32_t GetBandWidth();


#ifdef _FREEBSD
    void      SetSendSize(const size_t _iSize);
    const size_t  GetSendSize();

    void      SetTotalSendSize(const size_t _iSize);
    void      AddTotalSendSize(const size_t _nOffset);

    const size_t  GetTotalSendSize();
    const size_t  GetFileSize();
#else
    void      SetSendSize(const uint64_t _iSize);
    const uint64_t  GetSendSize();

    void      SetTotalSendSize(const uint64_t _iSize);
    void      AddTotalSendSize(const uint64_t _nOffset);

    const uint64_t  GetTotalSendSize();
    const uint64_t  GetFileSize();
#endif

    const char*   GetFilePath();
    void  SetFilePath(char *id);
    void  SetFilePathOld(char *id);

    /*
       const uint32_t  GetRealSendSize();
     */

    void SetStartTime(const double _dStartTime);
    const double  GetStartTime();
    void SetSendTime(const double _dSendTime);
    const double  GetSendTime();

    //void SetFileSendStartTime(const double _dSendTime);
    const double  GetFileSendStartTime();

#ifdef _FREEBSD
    void SpeedControl(off_t _nOffSet);
#else
    // void SpeedControl(off64_t _nOffSet); // linux
    void SpeedControl(off_t _nOffSet); // mac
#endif

    //const int   ExecuteCommand();
    const int   ExecuteCommand(Thread *_pThread);

    //  void SetDownloadServer(DownloadServer *_pDownloadServer);

    // DS<->DSM command
    void SendCloseToMgr();
    /*
       void SendFileInfoToMgr();
       void SendFileSizeToMgr();
     */
    void      SetSendMode(const ENUM_SEND_MODE _eMode);
    const ENUM_SEND_MODE GetSendMode();

    // DS command
    void WorkHello(const T_PACKET &_tPacket);
    void WorkHelloCDN(const T_PACKET &_tPacket);
    void WorkGetFileSize(const T_PACKET &_tPacket);
#ifndef _ONESHOT
    void WorkGetFile(const T_PACKET &_tPacket);
#else
    int WorkGetFile(const T_PACKET &_tPacket);
#endif

#ifdef _CDN
#endif
    int WorkGetCDNFile(const T_PACKET &_tPacket);
    void get_down_path_ex(char *id, char *down_path, int dflag);

    void WorkPing(const T_PACKET &_tPacket);
    void WorkGoodBye(const T_PACKET &_tPacket);

    const int GetComCodeIdx();
    void SetComCodeIdx(const int _iComCodeIdx);

    void WorkCheckViolation(const T_PACKET &_tPacket, Thread *_pThread);
    void WorkCheckViolation2(const T_PACKET &_tPacket, Thread *_pThread);
};

#endif

