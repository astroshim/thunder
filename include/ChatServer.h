#ifndef __CHATSERVER_H
#define __CHATSERVER_H

#include "./Process.h"

#define CHATSERVER_PROPERTIES "./ChatServer.properties"

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
class BroadcastMessage;

class ReleaseSlot;
// #ifdef _ONESHOT
// class ThreadQoS;
// #endif

#ifdef _CLIENT_ARRAY
// 090313 added
class ChatUser;
#endif

using namespace std;
#include <iostream>
#include <sstream>
#include <vector>

class ChatServer : public Process
{
  private:
    static const int TIME_ALIVE = 300; // 5��
    static const int MAX_COMPANY = 100;
#ifdef _CLIENT_ARRAY
    static const unsigned int MAX_CLIENT = 200;
#endif

    /**
     * ChatServer Server Infomation
     */
    ServerInfoDN *m_pServerInfo;

    int m_iConnCount;

    Client *m_pDNServerSocket; //
    ClientSocket *m_pSendPipe;
    // scoreboard_file *m_pShm;
    TStatistics *m_pShmKcps;
    TDSStatus *m_pShmDSStatus; // for DS Status
    int *m_pShmD;

#ifdef _FREEBSD
    IOMP_KQUEUE *m_pIOMP;
#else
    IOMP_EPoll *m_pIOMP;
#endif

    CircularQueue *m_pReceiveQueue;
    CircularQueue *m_pSendQueue;
    CircularQueue *m_pBroadcastQueue;

#ifdef _CLIENT_ARRAY
    ChatUser *m_arrClient[MAX_CLIENT];
#else
    list<Client *> m_lstClient;
#endif
    pthread_mutex_t m_lockClient;

    int m_iSeq;
    int m_iMaxUser;
    int m_iShmKey;      // key from dsm
    int m_iShmDSStatus; // key from dsm
    ReleaseSlot *m_pSlot;
// #ifdef _ONESHOT
//     ThreadQoS *m_pTQoS;
// #endif

  public:
    ChatServer();
    ChatServer(Properties &_cProperties);
    virtual ~ChatServer();

    void Run();
    Client *const GetServerSocket();
    const char *const GetMRTGURL();
    const char *const GetLogFileName();

    const int ConnectToMgr();
    const int NegotiationWithManager(string server, int port);

    const int GetMaxUser();
    const int GetShmKey();
    const int GetSeq();

    void SetMaxUser(const int _iMaxUser);
    void SetShmKey(const int _iShmKey);
    void SetSeq(const int _iSeq);

    const char *const GetIPAddr();
    const int GetCurrentUserCount();
    void HealthCheckUsers();
    // void SendStorageInfo();

#ifdef _FREEBSD
    void AddEPoll(Client *const _pClient, const short _filter, const unsigned short _usFlags);
#else
    void AddEPoll(Client *const _pClient, const unsigned int _uiEvents);
#endif
    void UpdateEPoll(Client *const _pClient, const unsigned int _uiEvents);
    void AcceptClient(Socket *const _pClientSocket);
    void CloseClient(Client *const _pClient);

#ifdef _CLIENT_ARRAY
    void AcceptClient(const int _iClientFD);
    void CloseClient(const int _iSlot);
#endif

    // void WriteUserInfo(Client *const _pClient);
    // void AddThroughput(const int _iIdx, const int _iSendSize);

    Client *const GetClient(const unsigned int _uiSessionKey);

    void PutReceiveQueue(const void *const _pVoid);
    const void *const GetReceiveQueue();

    void PutSendQueue(const void *const _pVoid);
    const void *const GetSendQueue();

    void PutBroadcastQueue(char *message, Client *const _pClient);
    // void PutBroadcastQueue(const void *const _pVoid);
    const void *const GetBroadcastQueue();

    const int GetDNServerPort();
    const int GetServerPort();
    void SetServerSocket(Client *_pClient);

    const char *const GetVolName();
    const char *const GetDirName();

    ClientSocket *const GetSendPipeClient();
    const uint64_t GetDownloadSize(const uint32_t _nBillNo);

    const unsigned int GetBandwidth(const char _chID);
// #ifdef _ONESHOT
//     const int AddQoS(Client *const _pClient, const unsigned int _uiEvents);
// #endif
    void SetD();
    // void SetComCodeIdx(ChatUser *const _pClient);
    // const int GetComCodeIdx(const int _iComCode);

    // void BroadcastMessage(char *message, Client *const _pClient);
    void MessageBroadcast(BroadcastMessage *_message);
};

#endif
