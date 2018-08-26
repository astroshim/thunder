#include "../include/NPUtil.h"
#include "../include/Client.h"
#include "../include/NPDebug.h"
#include "../include/NPLog.h" //Log
#include "../include/ThreadManager.h"

Client::Client()
    : m_cSocket(NULL), m_iUserSeq(-1), m_iState(STATE_CLOSED), m_eType(CLIENT_NOTHING), m_pMainProcess(NULL)
{
  SetAccessTime();
}

Client::Client(Socket *const _cSocket, const ENUM_CLIENT_TYPE _eType)
    : m_cSocket(_cSocket), m_iUserSeq(-1), m_iState(STATE_WAIT), m_eType(_eType), m_pMainProcess(NULL)
{
  SetAccessTime();
}

Client::~Client()
{
  delete m_cSocket;
  m_cSocket = NULL;
}

const unsigned int Client::GetIp()
{
  TcpSocket *pSocket;
  if ((pSocket = dynamic_cast<TcpSocket *>(m_cSocket)) == NULL)
  {
    return 0;
  }

  return pSocket->GetClientIp();
}

void Client::SetMainProcess(Process *const _pMainProcess)
{
  m_pMainProcess = _pMainProcess;
}

const int Client::IsValidPacket()
{
  /**
   * 2018: skip because chat server doesn't need packet header
  */
  CNPLog::GetInstance().Log("In Client::IsValidPacket ##### UsedSize=%d ", m_cCBuff.GetUsedSize());
  return m_cCBuff.GetUsedSize();

  /**
   * 
  if (m_cCBuff.GetUsedSize() < PDUHEADERSIZE)
  {
    return RECV_NOT_ENOUGH;
  }

  //int iLength = CNPUtil::Get2Byte((unsigned char *)m_cCBuff.GetHeaderPoint()+COMMAND_SIZE, 0);
  PACKET_HEADER *pPacketHeader = (PACKET_HEADER *)m_cCBuff.GetHeaderPoint();
  int iLength = pPacketHeader->length;

  if (iLength < 0 ||
      iLength > 65535 ||
      iLength > CIR_BUFSIZE)
  {
    CNPLog::GetInstance().Log("In Client::IsValidPacket Clear Buffer iLength=(%d) (%d)", iLength, COMMAND_SIZE);
    m_cCBuff.BufferClear();
    return RECV_ERROR;
  }

  return m_cCBuff.GetUsedSize();
  */
}

void Client::SetState(const ENUM_CLIENT_STATE _state)
{
  m_iState = _state;
}

const int Client::GetState()
{
  return m_iState;
}

const int Client::GetState(const ENUM_CLIENT_STATE _state)
{
  if (m_iState & _state)
  {
    return 1;
  }
  return 0;
}

const bool Client::IsClosed()
{
  if (m_iState & STATE_CLOSED)
    return true;

  return false;
}

const ENUM_CLIENT_TYPE Client::GetType()
{
  return m_eType;
}

void Client::SetType(const ENUM_CLIENT_TYPE _eType)
{
  m_eType = _eType;
}

const int Client::GetUserSeq()
{
  return m_iUserSeq;
}

void Client::SetUserSeq(int _iUserSeq)
{
  m_iUserSeq = _iUserSeq;
}

void Client::SetAccessTime()
{
  //m_tAccessTime = CNPUtil::GetUnixTime();
  m_tAccessTime = CNPUtil::GetMicroTime();
}

//const time_t Client::GetAccessTime()
const double Client::GetAccessTime()
{
  return m_tAccessTime;
}

Socket *const Client::GetSocket()
{
  return m_cSocket;
}

const int Client::GetPacket(char *const _pchBuffer, const int _iLength)
{
  int iRet = 0;
  if ((iRet = m_cCBuff.Get(_pchBuffer, _iLength)) < 0)
  {
    CNPLog::GetInstance().Log("In Client::GetPacket Error");
    return -1;
  }

  return 0;
}

/**
*/
const int Client::FillFromSocket()
{
  int iReadLen = 0;

  if ((iReadLen = m_cCBuff.Put(m_cSocket)) <= 0)
  {
    return USER_CLOSE;
  }

#ifdef _DEBUG
  CNPLog::GetInstance().Log("InClient::FillFromSocket(%p) capacity=[%d], filled=> %d ",
                            this, m_cCBuff.GetTotalSize(), m_cCBuff.GetUsedSize());
#endif

  return iReadLen;
}

const int Client::SendToClient(const T_PACKET *const _stSendPacket, const int _iLen)
{
  //return ((Socket *)(this->GetSocket()))->Write((char *)_stSendPacket, _iLen);
  return m_cSocket->Write((char *)_stSendPacket, _iLen);
}
