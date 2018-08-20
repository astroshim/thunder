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
   semctl 이란 함수를 이용해서 우리는 세마포어를 조정할수 있다. semctl 은 semid_ds 구조체를 변경함으로써 세마포어의 특성을 조정한다.
   첫번째 아규먼트인 semid 는 세마포어 지시자이다. semnum 은 세마포어 배열을 다룰 경우 사용되며, 보통은 0이다. cmd 는 세마포어 조작명령어 셋으로 다음과 같은 조작명령어들을 가지고 있다. 아래는 그중 중요하다고 생각되는 것들만을 설명하였다. 더 자세한 내용은 semctl 에 대한 man 페이지를 참고하기 바란다.
   IPC_STAT
   세마포어 상태값을 얻어오기 위해 사용되며, 상태값은 arg 에 저장된다.
   IPC_RMID
   세마포어 를 삭제하기 위해서 사용한다.
   IPC_SET
   semid_ds 의 ipc_perm 정보를 변경함으로써 세마포어에 대한 권한을 변경한다.
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

    /* 세마포어 집합 중 각각의 세마포어 semid 데이터 구조체 */
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
