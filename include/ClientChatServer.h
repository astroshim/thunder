#ifndef __CLIENTDN_H
#define __CLIENTDN_H

#include "./Client.h"

class ClientChatServer : public Client
{
private:
  // Chat Server information

  int m_iSeq;
  int m_iPid;
  // int m_iMaxUser;
  // int m_iShmKey;

public:
  ClientChatServer();
  ClientChatServer(Socket *const _cSocket);
  virtual ~ClientChatServer();

  // DC->DSM command
  void WorkDSHello(const T_PACKET &_tPacket);
  void WorkUserClose(const T_PACKET &_tPacket);

  // DC->DSM command
  void WorkDSMHello(const T_PACKET &_tPacket);
  void WorkGetDSInfo(const T_PACKET &_tPacket);
  void WorkDSMPing(const T_PACKET &_tPacket);
  void WorkGoodBye(const T_PACKET &_tPacket);

  void MessageBroadcast(const T_PACKET &_tPacket);
  const int ExecuteCommand(Thread *_pThread);

  const int GetSeq();
  const int GetPid();
  void SetSeq(int _seq);
  void SetPid(int _pid);
};

#endif
