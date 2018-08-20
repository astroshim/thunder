#ifndef SHMEM_H
#define SHMEM_H

#include "Include.h"

class SharedMemory
{
  private:
    key_t         m_Key;
    int         m_ShmId;
    const size_t    m_Size;
    void        *m_Data;
    bool      m_IsStarted;

    void CreateSM(key_t _shm_key);
  public:
    SharedMemory(key_t _shm_key, size_t _size);
    SharedMemory(key_t _shm_key, size_t _size, const void *_pAddr);
    SharedMemory(size_t size);
    SharedMemory(key_t _key);
    ~SharedMemory();

    key_t GetKey();
    bool IsStarted ();

    bool Create (key_t _shm_key);
    bool Create ();
    bool Attach (const void *_pAddr);
    bool Destroy ();

    const int   GetID();
    void *GetDataPoint();

    const time_t GetLastAccessTime();
    const time_t GetLastDetechTime();
    const time_t GetLastChangeTime();

    const unsigned short GetAccessPCount();
    const int   GetSize();

    void SetPermissions (mode_t mode);
};


/*
   struct shmid_ds {
   struct ipc_perm shm_perm; // 퍼미션
   int shm_segsz; // 세그먼트의 크기 (bytes)
   time_t shm_atime; // 마지막 접근한 시간
   time_t shm_dtime; // 마지막 제거된 시간
   time_t shm_ctime; // 마지막 변경된 시간
   unsigned short shm_cpid; // 만든 프로세스 아이디
   unsigned short shm_lpid; // 마지막 작동한 프로세스 아이디
   short shm_nattch; // 현재 접근한 프로세스 수
   unsigned short shm_npages;// 세그먼트의 크기(페이지)
   unsigned long *shm_pages;
   struct shm_desc *attaches;// 접근을 위한 기술자
   };
   struct ipc_perm {
   key_t key; // 키값
   ushort uid; // 소유자의 유효사용자 아이디
   ushort gid; // 소유자의 유효그룹 아이디
   ushort cuid; // 만든이의 유효사용자 아이디
   ushort cgid; // 만든이의 유효그룹 아이디
   ushort mode; // 접근 권한
   ushort seq; // sequence number
   };
 */

#endif
