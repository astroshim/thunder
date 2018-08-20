#ifndef _BITSEARCHING_H_
#define _BITSEARCHING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//a << b == a * (2의 b승)

/*
   -- argument로 받자
   unsigned const int MAX_SLOT_CNT =   1024 ;                       // 관리 할 slot 개수
   unsigned const int BYTE_CNT     =   MAX_SLOT_CNT/BIT_PER_BYTE ;  // 필요한 byte 수
 */

unsigned const int BIT_PER_BYTE =   8;                        // 1byte 의 bit 수
unsigned const int MV_BYTE      =   3;

unsigned const char BYTE_KEY = 0xFF;
unsigned const int g_BitValue[] = {0x01,  // 1
  0x02,   // 2
  0x04,   // 4
  0x08,   // 8
  0x10,   // 16
  0x20,   // 32
  0x40,   // 64
  0x80, // 128
  0x100,  // 256
  0x200,  // 512
  0x400,  // 1024
  0x800,  // 2048
  0x1000, // 4096
  0x2000, // 8192
  0x4000,
  0x8000,
  0x10000,
  0x20000,
  0x40000,
  0x80000 // 524288
};
class ReleaseSlot
{
  public:
    ReleaseSlot(int _maxSlotCnt);

    ~ReleaseSlot();

    int BitOn(int _byte_pos, int _free_slot);
    int BitOff(int _bit_pos);
    int PutSlot(int iSlot);

    int GetFreeSlot();
    int PrintBit();

    bool  IsStarted();

  private:
    int m_MaxSlotCnt;       /* 관리할 slot 개수 */
    int m_MemByteCnt;       /* 필요한 메모리 byte 수 */
    unsigned char *m_Slot;

    int   get_free_bit(int _byte_pos);
    int   get_free_byte();
    void  CreateClass(int _maxSlotCnt);

    bool m_IsStarted;
};

#endif

