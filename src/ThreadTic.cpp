#include "../include/ThreadTic.h"
#include "../include/Process.h"
#include "../include/Client.h"
#include "../include/NPLog.h"
#include "../include/NPUtil.h"
#ifdef _FREEBSD
#include <libgen.h>
#endif

ThreadTic::ThreadTic(Process* const _pMainProcess)
{
  CNPLog::GetInstance().Log("ThreadTic Construct");
  m_pMainProcess = _pMainProcess;
}

ThreadTic::~ThreadTic()
{
  this->SetStarted(false);
  CNPLog::GetInstance().Log("ThreadTic Destruct");
}

void ThreadTic::Run()
{
  int iHealthCnt = 1;
  this->SetStarted(true);

  char pchOldLogDir[PATH_MAX];
  char pchLogDir[PATH_MAX];
  char * name_start, * name_end;
  int size;

  // check directory
  name_start = (char *)m_pMainProcess->GetLogFileName();
  name_end = basename((char *)m_pMainProcess->GetLogFileName());
  //size = name_end - name_start; // only linux
  size = strlen(name_start) - strlen(name_end);

  memcpy(pchOldLogDir, name_start, size);
  pchOldLogDir[size] = '\0';

  memcpy(pchLogDir, name_start, size);
  pchLogDir[size] = '\0';

  strcat(pchOldLogDir, "old_log");  // jjang file
  // zfile �� old_log�� ������� �ʴ´�.

  while(1)
  {

// #ifndef _FREEBSD
//     if(iHealthCnt%12 == 0)  // 
//     {
//       m_pMainProcess->HealthCheckUsers();
//       if(!(m_pMainProcess->GetMRTGURL() == NULL || strncmp(m_pMainProcess->GetMRTGURL(), "no", 2) == 0))
//       {
//         m_pMainProcess->SendStorageInfo();
//       }
//     }
// #endif

    if(iHealthCnt >= 720)
    {
      CNPUtil::DeleteFile(pchOldLogDir, 86400*2); // old_log dir ������ ��Ʋ ������ �����.
      CNPUtil::DeleteFile(pchLogDir, 86400*2);  // log dir ������ ��Ʋ ������ �����.
      iHealthCnt = 1;
    }

    /**
     *   �θ����μ����� ��� �ִ��� ����, ������ �׾������!
     */
    /*
       int status;
    //int wait_pid = waitpid(m_pMainProcess->GetPPid(), &status, WIFEXITED|WIFSIGNALED|WIFSTOPPED|WSTOPSIG|WUNTRACED|WNOHANG);
    int wait_pid = waitpid(m_pMainProcess->GetPPid(), &status, WUNTRACED|WNOHANG);
    CNPLog::GetInstance().Log("1.Parent process pid=(%d), wait_pid=(%d)", m_pMainProcess->GetPPid(), wait_pid);
    if(wait_pid > 0) // parent killed
    {
    CNPLog::GetInstance().Log("Parent process killed=(%d)(%s), pid=(%d, %d), status=(%d)",
    errno, strerror(errno), m_pMainProcess->GetPid(), getpid(), GetStarted());
    m_pMainProcess->SetStarted(false);
    }
    */

    /*
       CNPLog::GetInstance().Log("1.Parent process pid=(%d), status=(%d, %d)", m_pMainProcess->GetPPid(),
       kill(m_pMainProcess->GetPPid(), 0),
       kill(m_pMainProcess->GetPPid(), SIGUSR2));
       */
    if (kill(m_pMainProcess->GetPPid(), 0) < 0)
    {
      /*
         CNPLog::GetInstance().Log("2.Parent process pid=(%d, %d), status=(%d)", m_pMainProcess->GetPPid(), m_pMainProcess->GetPid(), getpid(), GetStarted());
         */
      if (errno == ESRCH)
      {
        CNPLog::GetInstance().Log("Parent process killed=(%d)(%s), pid=(%d, %d), status=(%d)",
            errno, strerror(errno), m_pMainProcess->GetPid(), getpid(), GetStarted());
        m_pMainProcess->SetStarted(false);
      }
    }

    sleep(5);
    iHealthCnt++;
  }

  pthread_exit(NULL);
}

