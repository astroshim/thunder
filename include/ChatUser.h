#ifndef __CLIENTUSERDN_H
#define __CLIENTUSERDN_H

#include "./Client.h"

class BroadcastMessage;
class ChatUser: public Client
{
  private:

    ENUM_SEND_MODE    m_eSendMode;

    list <T_PACKET*>  m_lstSendPacket;

    T_PACKET m_tSendPacket;

    pthread_mutex_t   m_mtLst;

    void SendPacket(const T_PACKET* _pPacket);

  public:
    ChatUser();
    ChatUser(Socket* const _cSocket);
    virtual ~ChatUser();

    void FreePacket();

    list<T_PACKET*>* const GetPacketList();
    void AddPacketFront(T_PACKET *_pPacket);
    void RemovePacket(T_PACKET *_pPacket);

    const int   GetSendPacketCount();

    const char* GetID();

    const int   ExecuteCommand(Thread *_pThread);

    // DS<->DSM command
    // void SendCloseToMgr();
    void SetSendMode(const ENUM_SEND_MODE _eMode);
    const ENUM_SEND_MODE GetSendMode();

    void MessageBroadcast(BroadcastMessage *message);

    // DS command
    void WorkHello(const T_PACKET &_tPacket);
    void WorkPing(const T_PACKET &_tPacket);
    void WorkGoodBye(const T_PACKET &_tPacket);

    void WorkCheckViolation(const T_PACKET &_tPacket, Thread *_pThread);
    void WorkCheckViolation2(const T_PACKET &_tPacket, Thread *_pThread);
};

#endif

