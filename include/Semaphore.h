#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

//unsigned const key_t sem_key = 9191;
unsigned const int sem_key = 9191;

union semun 
{
	int             val;
    struct semid_ds *buf;
    unsigned short int        *array;
};

/*
union semun {
   int val;                    // value for SETVAL 
   struct semid_ds *buf;       // buffer for IPC_STAT, IPC_SET 
   unsigned short int *array;  // array for GETALL, SETALL 
   struct seminfo *__buf;      // buffer for IPC_INFO 
};
*/

/*
semctl �̶� �Լ��� �̿��ؼ� �츮�� ������� �����Ҽ� �ִ�. semctl �� semid_ds ����ü�� ���������ν� ���������� Ư���� �����Ѵ�.

ù��° �ƱԸ�Ʈ�� semid �� �������� �������̴�. semnum �� �������� �迭�� �ٷ� ��� ���Ǹ�, ������ 0�̴�. cmd �� �������� ���۸�ɾ� ������ ������ ���� ���۸�ɾ���� ������ �ִ�. �Ʒ��� ���� �߿��ϴٰ� �����Ǵ� �͵鸸�� �����Ͽ���. �� �ڼ��� ������ semctl �� ���� man �������� �����ϱ� �ٶ���. 
IPC_STAT 
�������� ���°��� ������ ���� ���Ǹ�, ���°��� arg �� ����ȴ�. 
IPC_RMID 
�������� �� �����ϱ� ���ؼ� ����Ѵ�. 
IPC_SET 
semid_ds �� ipc_perm ������ ���������ν� ������� ���� ������ �����Ѵ�. 
*/

class Semaphore 
{
private:
	int m_SemId;

public:
	Semaphore(const key_t _sema_key);
	~Semaphore();

	const bool SemaCreate(const key_t mykey);
	const bool SemaDelete();
	const bool set_semvalue();
	const bool del_semvalue();
	const bool Lock();
	const bool Unlock();

	const bool Locked();

/* �������� ���� �� ������ �������� semid ������ ����ü */ 
/*
struct semid_ds { 
  struct ipc_perm sem_perm;            // permissions .. see ipc.h 
  time_t          sem_otime;           // last semop time 
  time_t          sem_ctime;           // last change time 
  struct sem      *sem_base;           // ptr to first semaphore in array 
  struct sem_queue *sem_pending;       // pending operations to be processed 
  struct sem_queue **sem_pending_last; // last pending operation 
  struct sem_undo *undo;         // undo requests on this array 
  ushort          sem_nsems;           // no. of semaphores in array 
}; 
*/
	const int CountEvents();
	const time_t GetLastOpTime();
	const time_t GetLastChangeTime();
};

#endif // __SEMAPHORE_H


