#include "../include/ConnectList.h"
#include "../include/NPDebug.h"
#include "../include/NPLog.h"

ConnectList::ConnectList()
{
  iCurrentConntionCnt = 0;
  m_iIdx = 0;

  /*
  ** Inin mutex and condition value.
  */
  pthread_mutex_init(&buf_lock, NULL);
}

ConnectList::~ConnectList()
{
  /*
  ** Destroy mutex and condition value.
  */
  pthread_mutex_destroy(&buf_lock);
}

int ConnectList::InsertData(void *data)
{
  pthread_mutex_lock(&this->buf_lock);

  if(iCurrentConntionCnt >= FD_QSIZE ||
    m_iIdx > FD_QSIZE)
  {
    CNPLog::GetInstance().Log("ConnectList Overflow !");
    pthread_mutex_unlock(&this->buf_lock);
    return -1;
  }

  this->data[m_iIdx++] = (void *)data;

CNPLog::GetInstance().Log("ConnectList::InsertData ==> (%p) (%d)", data, m_iIdx);
  iCurrentConntionCnt++;
  pthread_mutex_unlock(&this->buf_lock);
  return 0;
}

void * ConnectList::DeleteData()
{
  void *data;

  pthread_mutex_lock(&this->buf_lock);

  if (m_iIdx <= 0)
  {
    m_iIdx = 0;
    pthread_mutex_unlock(&this->buf_lock);
    return NULL;
  }

  data = this->data[--m_iIdx];

CNPLog::GetInstance().Log("ConnectList::DeleteData ==> (%p) (%d)", data, m_iIdx);
  iCurrentConntionCnt--;
  pthread_mutex_unlock(&this->buf_lock);
  return (data);
}

