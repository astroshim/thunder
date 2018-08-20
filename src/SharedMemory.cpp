#include "../include/SharedMemory.h"

void SharedMemory::CreateSM(key_t _shm_key)
{
    m_Key = _shm_key;
    m_IsStarted = false;
    m_Data = NULL;
    m_ShmId = 0;

    //Destroy();

    if(Create(_shm_key))
    {
        //if(Attach(NULL))
        if(Attach(0))
        {
            m_IsStarted = true;
        }
    }
    else
    {
        m_IsStarted = false;
    }

//cout << "In SharedMemory key=> " << _shm_key << ", id=> " << m_ShmId << ", size=> " << m_Size << endl;
}

SharedMemory::SharedMemory(key_t _shm_key, size_t _size):m_Size(_size)
{
  CreateSM(_shm_key);
}

SharedMemory::SharedMemory(key_t _shm_key, size_t _size, const void *_pAddr):m_Size(_size)
{
    m_Key = _shm_key;
    m_IsStarted = false;
    m_Data = NULL;
    m_ShmId = 0;

    if(Create(_shm_key))
    {
        if(Attach(_pAddr))
        {
            m_IsStarted = true;
        }
    }
    else
    {
        m_IsStarted = false;
    }
}

SharedMemory::SharedMemory(key_t _shm_key):m_Size(0)
{   
  CreateSM(_shm_key);
}

SharedMemory::SharedMemory(size_t _size):m_Size(_size)
{
  m_Key = 0;
    m_Data = NULL;
    m_ShmId = 0;

    if(Create())
    {
        //if(Attach(NULL))
        if(Attach(0))
        {
      m_IsStarted = true;
    }
    }
    else
  {
    m_IsStarted = false;
  }

//cout << "In SharedMemory 2" << endl;
}

SharedMemory::~SharedMemory()
{
    //Destroy();
    m_Data = NULL;
  m_IsStarted = false;
    m_ShmId = 0;
}

key_t SharedMemory::GetKey()
{
  return m_Key;
}

bool SharedMemory::IsStarted()
{
  return m_IsStarted;
}

bool SharedMemory::Create(key_t _shm_key)
{
  //if((m_ShmId = shmget((key_t)_shm_key, m_Size, IPC_PRIVATE)) < 0)
    //if((m_ShmId = shmget (_shm_key, m_Size, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    if((m_ShmId = shmget ((key_t)_shm_key, m_Size, IPC_CREAT | 0666)) < 0)
  {
    sleep(1);
      if((m_ShmId = shmget (_shm_key, m_Size, IPC_CREAT | 0666)) < 0)
      {
          perror("SharedMemory::Create 1 ");
          return false;
      }
  }

/*
    if((m_ShmId = shmget (_shm_key, m_Size, IPC_CREAT | 0666)) < 0)
    {
        perror("SharedMemory::Create 1 : ");
        return false;
    }
*/

    return true;
}

bool SharedMemory::Create()
{
//    if((m_ShmId = shmget (IPC_PRIVATE, m_Size, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    if((m_ShmId = shmget (IPC_PRIVATE, m_Size, IPC_CREAT | 0666)) < 0)
    {
        perror("SharedMemory::Create 2 : ");
        return false;
    }

    return true;
}

bool SharedMemory::Destroy()
{
    if (m_ShmId >= 0)
    {
        if(shmctl(m_ShmId, IPC_RMID, 0) < 0)
        {
            perror("in destroy ");
            return false;
        }

        if(shmdt(m_Data) < 0)
        {
            perror("in destroy ");
            return false;
        }

        return true;
    }

    return false;
}

bool SharedMemory::Attach (const void *_pAddr)
{
//printf("m_shmid ==> '%d'\n", m_ShmId);
    if(m_ShmId >= 0)
    {
        //if((m_Data = (void *)shmat (m_ShmId, _pAddr, SHM_RND)) < 0)
        if((m_Data = (void *)shmat (m_ShmId, _pAddr, 0)) <  (void *)0)
        {
            perror("in attach ");
            return false;
        }
    }

//cout << "m_Data ==> " << m_Data << endl;
    return true;
}

const int SharedMemory::GetID()
{
    return m_ShmId;
}

void *SharedMemory::GetDataPoint()
{
    return m_Data;
}

const time_t SharedMemory::GetLastAccessTime()
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);

    return shm.shm_atime;
}

const time_t SharedMemory::GetLastDetechTime()
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);

    return shm.shm_dtime;
}

const time_t SharedMemory::GetLastChangeTime()
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);

    return shm.shm_ctime;
}

const unsigned short SharedMemory::GetAccessPCount()
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);

    return shm.shm_nattch;
}

const int SharedMemory::GetSize()
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);

    return shm.shm_segsz;
}

void SharedMemory::SetPermissions (mode_t mode)
{
    struct shmid_ds shm;
    shmctl (m_ShmId, IPC_STAT, &shm);
    shm.shm_perm.mode = mode;
    shmctl (m_ShmId, IPC_SET, &shm);

    return;
}

