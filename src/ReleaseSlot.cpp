#include "../include/ReleaseSlot.h"

void ReleaseSlot::CreateClass(int _maxSlotCnt)
{
    m_Slot = NULL;
  m_IsStarted   = false;
    m_MaxSlotCnt  = _maxSlotCnt;
    m_MemByteCnt  = m_MaxSlotCnt/BIT_PER_BYTE;

    if(m_MaxSlotCnt%BIT_PER_BYTE != 0)
    {
        m_MemByteCnt++; 
    }

    if((m_Slot = (unsigned char *)new char[(sizeof(unsigned char)*m_MemByteCnt) +1]) == NULL)
  {
    perror("ReleaseSlot new : ");
    return;
  }
  memset(m_Slot, 0x00, (sizeof(unsigned char)*m_MemByteCnt)+1);
  m_IsStarted = true;
}

ReleaseSlot::ReleaseSlot(int _maxSlotCnt)
{   
  CreateClass(_maxSlotCnt);
}

ReleaseSlot::~ReleaseSlot()
{
    if(m_Slot)
  {
    delete m_Slot;
  }
}

bool ReleaseSlot::IsStarted()
{
  return m_IsStarted;
}

int ReleaseSlot::BitOn(int _byte_pos, int _free_slot)
{
    //m_Slot[_byte_pos] |= g_BitValue[_free_slot-1];
    m_Slot[_byte_pos] |= g_BitValue[_free_slot];

    return 0;
}

int ReleaseSlot::PutSlot(int iSlot)
{
  unsigned int byte_pos;    
  int bit_value;

  if(iSlot > m_MaxSlotCnt) return -1;

  byte_pos = (iSlot) >> MV_BYTE;          

  bit_value = iSlot & ~(byte_pos << MV_BYTE);  

    m_Slot[byte_pos] &= ~g_BitValue[bit_value];     // bit off

    return 0;
}

int ReleaseSlot::GetFreeSlot()
{
    int byte_pos = 0;
    int free_slot = 0;

    if((byte_pos = get_free_byte()) < 0) 
  {
    return -1;
  }
    free_slot = get_free_bit(byte_pos);

    BitOn(byte_pos,free_slot);
    return (byte_pos << MV_BYTE)+free_slot;
}

int ReleaseSlot::get_free_byte()
{
  int i;

    for(i = 0; i < m_MemByteCnt; i++)
    {
    int iSlot = (m_Slot[i] xor BYTE_KEY);

#ifdef _DEBUG
    printf(" xor ==> (%02X) (%d) \n", iSlot, iSlot);
#endif
    if(iSlot > 0)
    {
      break;
    }
  }

    if(i == m_MemByteCnt)
    {
        printf(" OverFlow~~~>> \n");
        return -1;
    }

  return i;
}

int ReleaseSlot::get_free_bit(int _byte_pos)
{
    int i;

    for(i = 0; i < BIT_PER_BYTE; i++)
    {
        if(!(m_Slot[_byte_pos] & (0x01 << i)))
        {
            break;
        }
    }

    return i;
}

int ReleaseSlot::PrintBit()
{
    for(int i = 0; i < m_MemByteCnt; i++)
    {
        printf("m_Slot[%d] ==> '%X','%d'\n", i, m_Slot[i], m_Slot[i]);
    }

    return 0;
}

/*
int main()
{
  int iSlot;
  ReleaseSlot cSlot(1024);

  for(int i = 0; i < 1024; i++)
  {
    iSlot = cSlot.GetFreeSlot();
    printf("alloc slot = (%d) \n", iSlot);
  }

sleep(10);

  for(int i = 0; i < 1024; i++)
  {
    cSlot.PutSlot(i);
  }
cSlot.PrintBit();
}
*/
