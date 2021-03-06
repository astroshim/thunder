#include "../include/NPLog.h"
#include "../include/NPUtil.h"
#include "../include/CircularBuff.h"
#include "../include/Packet.h"
#include "../include/Socket.h"

CircularBuff::CircularBuff()
  :m_iHead(0)
  ,m_iTail(0)
  ,m_iBufferSize(0)
  ,m_iUseBufferSize(0)
   ,m_pchBuffer(NULL)
{
  NewBuffer(CIR_BUFSIZE);
}

CircularBuff::~CircularBuff()
{
  m_iHead = 0;
  m_iTail = 0;

  if( m_pchBuffer != NULL )
  {
    delete [] m_pchBuffer;
    m_pchBuffer = NULL;
  }
}

const int CircularBuff::NewBuffer(const int iSize)
{
  if( m_pchBuffer != NULL )
  {
    return -1;
  }

  m_pchBuffer          = new char[iSize+1];
  m_iBufferSize       = iSize;
  m_iUseBufferSize    = 0;

  return iSize;
}

void CircularBuff::InitBuffer()
{
  m_iHead = 0;
  m_iTail = 0;
  m_iUseBufferSize = 0;
  memset(m_pchBuffer, 0x00, sizeof(char) * m_iBufferSize);
}

const int CircularBuff::GetTotalSize()
{
  return m_iBufferSize;
}

const int CircularBuff::GetUsedSize()
{
  return m_iUseBufferSize;
}

const int CircularBuff::GetFreeSize()
{
  return ( m_iBufferSize - m_iUseBufferSize);
}

const unsigned char* const CircularBuff::GetHeaderPoint()
{
  return (unsigned char *)&m_pchBuffer[m_iHead];
}

void CircularBuff::PrintBufferDump()
{
  CNPLog::GetInstance().LogDump("CircularBuff", (char *)&m_pchBuffer[m_iHead], m_iUseBufferSize);
}

/**
*/
void CircularBuff::Resize()
{
  char *pchNewBuffer  = new char[(m_iBufferSize + CIR_BUFSIZE)+1];

  if (m_iHead < m_iTail)
  {
    memcpy(pchNewBuffer, &m_pchBuffer[m_iHead], (m_iTail-m_iHead));
  }
  else
  {
    int iRightLen = m_iBufferSize - m_iHead;
    memcpy(pchNewBuffer, &m_pchBuffer[m_iHead], iRightLen);
    if(m_iTail > 0)
    {
      memcpy(&pchNewBuffer[iRightLen], m_pchBuffer, m_iTail);
    }
  }

  delete m_pchBuffer;
  m_pchBuffer = pchNewBuffer;

  m_iBufferSize       = m_iBufferSize + CIR_BUFSIZE;
  m_iHead = 0;
  m_iTail = m_iUseBufferSize    = m_iUseBufferSize;
}

const int CircularBuff::Put(const char* const _pchBuffer, const int _iLength)
{
  if ( _iLength > GetFreeSize() )
  {
    return -1;
  }

  int iFree = 0;

  if (m_iHead <= m_iTail)
  {
    if (m_iHead == 0)
    {
      iFree = m_iBufferSize - m_iTail;
      memcpy(&m_pchBuffer[m_iTail], _pchBuffer, _iLength);
    }
    else
    {
      iFree = m_iBufferSize - m_iTail;

      if (_iLength <= iFree)
      {
        memcpy(&m_pchBuffer[m_iTail], _pchBuffer, _iLength);
      }
      else
      {
        memcpy(&m_pchBuffer[m_iTail], _pchBuffer, iFree);
        memcpy(m_pchBuffer, &_pchBuffer[iFree], _iLength - iFree);
      }
    }
  }
  else
  {
    memcpy(&m_pchBuffer[m_iTail], _pchBuffer, _iLength);
  }

  m_iTail += _iLength;

  if (m_iBufferSize <= m_iTail)
  {
    m_iTail -= m_iBufferSize;
  }

  m_iUseBufferSize += _iLength;

  return _iLength;
}

const int CircularBuff::Get(char* const _pchBuffer, const int _iLength)
{
#ifdef _DEBUG
  CNPLog::GetInstance().Log("CircularBuff::Get iLen=(%d) usedsize=(%d) Head(%d),Tail(%d), buff=(%p)",
      _iLength, GetUsedSize(), m_iHead, m_iTail, m_pchBuffer);
#endif
  if ( _iLength > GetUsedSize() )
  {
    CNPLog::GetInstance().Log("CircularBuff::Get len=(%d) usedsize=(%d)", _iLength, GetUsedSize());
    return RECV_NOT_ENOUGH;
  }

  if (m_iHead < m_iTail)
  {
    memcpy(_pchBuffer, &m_pchBuffer[m_iHead], _iLength);
  }
  else
  {
    int iRightLen = m_iBufferSize - m_iHead; // 600


    if (_iLength <= iRightLen)
    {
      memcpy(_pchBuffer, &m_pchBuffer[m_iHead], _iLength);
    }
    else
    {
      memcpy(_pchBuffer, &m_pchBuffer[m_iHead], iRightLen);
      memcpy(((char *)_pchBuffer + iRightLen), m_pchBuffer, _iLength - iRightLen);
    }
  }

  m_iHead += _iLength;
  if (m_iBufferSize <= m_iHead) m_iHead -= m_iBufferSize;

  m_iUseBufferSize -= _iLength;
  return _iLength;
}

const int CircularBuff::Put(Socket* const _pSocket)
{
  int iReadLen=0, iFree;
  int iLength = _pSocket->Available();

  if(iLength <= 0)
  {
    return USER_CLOSE;
  }

  if ( iLength > GetFreeSize() )
  {
    CNPLog::GetInstance().Log("ClientBuffer OverFlow! (%d),iLength=(%d),GetFreeSize()=(%d)", _pSocket->GetFd(), iLength, GetFreeSize());
    // buffer overflow!
    return USER_CLOSE;

    /**
     * resize?
     Resize();
     */
  }
#ifdef _DEBUG
  CNPLog::GetInstance().Log("In CircularBuff::Put(%p) iLength=(%d), m_iUseBufferSize=(%d), m_iHead=(%d), m_iTail=(%d)",
      m_pchBuffer, iLength, m_iUseBufferSize, m_iHead, m_iTail);
#endif

  if (m_iHead <= m_iTail)
  {
    if (m_iHead == 0)
    {
      iFree = m_iBufferSize - m_iTail;
      if((iReadLen = _pSocket->ReadLine((char *)&m_pchBuffer[m_iTail],
              iLength )) <= 0)
      {
        return USER_CLOSE;
      }
    }
    else
    {
      iFree = m_iBufferSize - m_iTail;

      if (iLength <= iFree)
      {
        if((iReadLen = _pSocket->ReadLine((char *)&m_pchBuffer[m_iTail],
                iLength )) <= 0)
        {
          return USER_CLOSE;
        }
        /*
#ifdef _DEBUG
CNPLog::GetInstance().LogDump("CircularBuff::Put", (char *)&m_pchBuffer[m_iTail], iLength);
#endif
*/
      }
      else
      {
        if((iReadLen = _pSocket->ReadLine((char *)&m_pchBuffer[m_iTail],
                iFree )) <= 0)
        {
          return USER_CLOSE;
        }
        int iTmp = 0;
        if((iTmp = _pSocket->ReadLine((char *)m_pchBuffer,
                iLength-iFree )) <= 0)
        {
          return USER_CLOSE;
        }
        iReadLen += iTmp;
      }
    }
  }
  else
  {
    if((iReadLen = _pSocket->ReadLine((char *)&m_pchBuffer[m_iTail],
            iLength )) <= 0)
    {
      return USER_CLOSE;
    }
  }

  m_iTail += iReadLen;

  if (m_iBufferSize <= m_iTail)
  {
    m_iTail -= m_iBufferSize;
  }
  m_iUseBufferSize += iReadLen;

#ifdef _DEBUG
  CNPLog::GetInstance().Log("In CircularBuff::Put(%s) Leave iReadLen=(%d), m_iUseBufferSize=(%d), m_iHead=(%d), m_iTail=(%d)",
      m_pchBuffer, iReadLen, m_iUseBufferSize, m_iHead, m_iTail);
#endif
  return iReadLen;
}

