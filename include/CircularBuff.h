#ifndef __CIRCULARBUF_H
#define __CIRCULARBUF_H

#include "./Include.h"
/*
 * #include "./Packet.h" #include "./Socket.h"
 */

// const unsigned int CIR_BUFSIZE = 1024000;  /* 1M bytes */
const unsigned int CIR_BUFSIZE = 1024;  /* 1k bytes */
const int NOT_ENOUGH_BUFFER = -10;

class   Socket;

class   CircularBuff
{
  public:
    CircularBuff();
    virtual ~ CircularBuff();

    /* put data in the CircularBuffer */
    const int     Put(const char *const _pchBuffer, const int _iLength);
    /* get data in the CircularBuffer */
    const int     Get(char *const _pchBuffer, const int _iLength);

    /* put data in the CircularBuffer use socket */
    const int     Put(Socket * const _pSocket);

    /* get data in the CircularBuffer */
    //int Get(T_PACKET * _pstPacket);
    //int Get(char *_pchPacket);

    const int     GetTotalSize();
    const int     GetUsedSize();
    const int     GetFreeSize();

    /* put ClosePacket in the CircularBuffer */
    void      PutClosePacket();

    //int IsValid();

    virtual inline void BufferClear() {
      memset(m_pchBufferHeader, 0x00, m_iBufferSize);
      m_iHead = m_iTail = 0;
      m_iUseBufferSize = 0;
    }

    const unsigned char *const GetHeaderPoint();

    void      PrintBufferDump();
    void      InitBuffer();

  private:
    /* Main Buffer Head */
    int       m_iHead;

    /* Main Buffer Tail */
    int       m_iTail;

    int       m_iBufferSize;

    /* use Main Buffer */
    int       m_iUseBufferSize;

    /* Buffer */
    char           *m_pchBufferHeader;

    void      Resize ();
    /* alloc buffer */
    const int     NewBuffer(const int iSize);
};

#endif
