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

using namespace std;
#include <iostream>
#include <sstream>
#include <vector>

class ChatServer : public Process
{
  private:
    static const int TIME_ALIVE = 300; // 5��
    static const int MAX_COMPANY = 100;

    /**
     * ChatServer Server Infomation
     */
    ServerInfoDN *m_pServerInfo;

    int m_iConnCount;

    Client *m_pDNServerSocket; //

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

    list<Client *> m_lstClient;
    pthread_mutex_t m_lockClient;

    list<ClientSocket *> m_lstChatManagerSocket;

    int m_iSeq;
    int m_iMaxUser;
    int m_iShmKey;      // key from dsm
    int m_iShmDSStatus; // key from dsm
    ReleaseSlot *m_pSlot;

  public:
    ChatServer();
    ChatServer(Properties &_cProperties);
    virtual ~ChatServer();

    void Run();
    Client *const GetServerSocket();
    const char *const GetMRTGURL();
    const char *const GetLogFileName();

    const int ConnectToMgr();
    ClientSocket *const NegotiationWithManager(string server, int port);

    const int GetMaxUser();
    const int GetShmKey();
    const int GetSeq();

    void SetMaxUser(const int _iMaxUser);
    void SetShmKey(const int _iShmKey);
    void SetSeq(const int _iSeq);

    const char *const GetIPAddr();
    const int GetCurrentUserCount();
    void HealthCheckUsers();

#ifdef _FREEBSD
    void AddEPoll(Client *const _pClient, const short _filter, const unsigned short _usFlags);
#else
    void AddEPoll(Client *const _pClient, const unsigned int _uiEvents);
#endif
    void UpdateEPoll(Client *const _pClient, const unsigned int _uiEvents);
    void AcceptClient(Socket *const _pClientSocket, ENUM_CLIENT_TYPE type);
    void CloseClient(Client *const _pClient);
    int RegisterManager();

    Client *const GetClient(const unsigned int _uiSessionKey);

    void PutReceiveQueue(const void *const _pVoid);
    const void *const GetReceiveQueue();

    void PutSendQueue(const void *const _pVoid);
    const void *const GetSendQueue();

    void PutBroadcastQueue(BroadcastMessage *message, Client *const _pClient);
    const void *const GetBroadcastQueue();

    const int GetDNServerPort();
    const int GetServerPort();
    void SetServerSocket(Client *_pClient);

    const char *const GetVolName();
    const char *const GetDirName();

    const uint64_t GetDownloadSize(const uint32_t _nBillNo);

    const unsigned int GetBandwidth(const char _chID);
    void SetD();

    void MessageBroadcast(BroadcastMessage *_message);
    void MessageBroadcastToManagers(BroadcastMessage *_message);
};

#endif
