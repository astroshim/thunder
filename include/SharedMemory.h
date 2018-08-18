#ifndef SHMEM_H
#define SHMEM_H

#include "Include.h"

class SharedMemory
{
private:
    key_t     		m_Key;
    int     		m_ShmId;
    const size_t    m_Size;
    void    		*m_Data;
	bool			m_IsStarted;

    void CreateSM(key_t _shm_key);
public:
    SharedMemory(key_t _shm_key, size_t _size);
    SharedMemory(key_t _shm_key, size_t _size, const void *_pAddr);
    SharedMemory(size_t size);
    SharedMemory(key_t _key);
    ~SharedMemory();

	key_t	GetKey();
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
struct ipc_perm shm_perm; // �۹̼�
int shm_segsz; // ���׸�Ʈ�� ũ�� (bytes)
time_t shm_atime; // ������ ������ �ð�
time_t shm_dtime; // ������ ���ŵ� �ð�
time_t shm_ctime; // ������ ����� �ð�
unsigned short shm_cpid; // ���� ���μ��� ���̵�
unsigned short shm_lpid; // ������ �۵��� ���μ��� ���̵�
short shm_nattch; // ���� ������ ���μ��� ��
unsigned short shm_npages;// ���׸�Ʈ�� ũ��(������)
unsigned long *shm_pages; 
struct shm_desc *attaches;// ������ ���� �����
};

struct ipc_perm {
key_t key; // Ű�� 
ushort uid; // �������� ��ȿ����� ���̵�
ushort gid; // �������� ��ȿ�׷� ���̵� 
ushort cuid; // �������� ��ȿ����� ���̵�
ushort cgid; // �������� ��ȿ�׷� ���̵�
ushort mode; // ���� ���� 
ushort seq; // sequence number
};
*/

#endif



