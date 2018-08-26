#include "../include/Process.h"
#include "../include/NPLog.h"
#include "../include/NPDebug.h"

Process::Process()
    : m_PPid(0), m_Pid(0), m_IsStarted(false), m_iSignalNo(0)
      //      ,m_iSeq(-1)
      ,
      m_pChild(NULL)
{
  m_iPipe[0] = -1;
  m_iPipe[1] = -1;

  /*
     for(int i = 0; i < MAX_PROCESS_CNT; i++)
     {
     m_lstChildPid[i] = -1;
     }
     */
}

Process::~Process()
{
  //  CNPLog::GetInstance().Log("Process Class Destruct");
}

const bool Process::GetStarted()
{
  return m_IsStarted;
}

void Process::SetStarted(const bool _bool)
{
  m_IsStarted = _bool;
}

void Process::SetSignalNo(const int _iSignalNo)
{
  m_iSignalNo = _iSignalNo;
}

const int Process::GetSignalNo()
{
  return m_iSignalNo;
}

const pid_t Process::GetPPid()
{
  return m_PPid;
  //return getppid();
}

void Process::SetPPid(const pid_t _pid)
{
  m_PPid = _pid;
}

const pid_t Process::GetPid()
{
  return m_Pid;
}

void Process::SetPid(const pid_t _pid)
{
  m_Pid = _pid;
}

const int Process::GetRecvPipe()
{
  return m_iPipe[0];
}

const int Process::GetSendPipe()
{
  return m_iPipe[1];
}

void Process::SetRecvPipe(const int _iRecvPipe)
{
  m_iPipe[0] = _iRecvPipe;
}

void Process::SetSendPipe(const int _iSendPipe)
{
  m_iPipe[1] = _iSendPipe;
}

/*
   void Process::SetSeq(const int _iSeq)
   {
   m_iSeq = _iSeq;
   }

   const int Process::GetSeq()
   {
   return m_iSeq;
   }
   */

const bool Process::IsAliveProcess(const int _iPid)
{
  if (kill(_iPid, 0) < 0)
  {
    if (errno == ESRCH)
    {
      CNPLog::GetInstance().Log("Process is Dead=(%d)(%s), pid=(%d)",
                                errno, strerror(errno), _iPid);

      return false;
    }
  }

  return true;
}

const bool Process::WatchChildProcessWithPipe()
{
  int status;
  pid_t wait_pid;

  wait_pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
  if (wait_pid <= 0)
  {
    //sleep(5);
    return true;
  }

  if (wait_pid > 0) /* process killed */
  {
    if (WIFEXITED(status))
    {
      CNPLog::GetInstance().Log("1.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, WIFEXITED(status), WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
      if (WIFEXITED(status) == SIGXFSZ) // file size excess.
      {
      }

      CNPLog::GetInstance().Log("2.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, status, WIFSIGNALED(status));
    }

    /**
     *   restart process
     */
    if (!SpawnChildProcessWithPipe())
    {
      return false;
    }
  }

  return true;
}

const bool Process::WatchChildProcess()
{
  int status;
  pid_t wait_pid;

  wait_pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
  if (wait_pid <= 0)
  {
    return true;
  }

  if (wait_pid > 0) /* process killed */
  {
    if (WIFEXITED(status))
    {
      CNPLog::GetInstance().Log("1.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, WIFEXITED(status), WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
      if (WIFEXITED(status) == SIGXFSZ) // file size excess.
      {
      }
      CNPLog::GetInstance().Log("2.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, status, WIFSIGNALED(status));
    }

    /**
     *   restart process
     */
    if (!SpawnChildProcess())
    {
      return false;
    }
    /*
#ifndef _FREEBSD
if(!SpawnChildProcess())
{
fprintf(stderr, "SpawnChildProcess Failure!!! \n");
sleep(10);
return false;
}
fprintf(stderr, "SpawnChildProcess Success!!! \n");
sleep(10);

#else
CNPLog::GetInstance().Log("Manager is killed by itself");
exit(1);
#endif
*/
  }

  return true;
}

/**
*/
const bool Process::SpawnProcess()
{
  int pid = 0;

  pid = fork();
  switch (pid)
  {
  case 0:
#ifndef _FREEBSD
    setpgrp();
#endif
    //this->Run();
    SetPid(getpid());
    Run();
    exit(0);

  case -1:
    Assert(false, "Process Spawn failure! ");
    return false;

  default:
    this->SetStarted(true);
    this->SetPid(pid);

    break;
  }

  return true;
}

const bool Process::SpawnChildProcessWithPipe()
{
  int pid = 0;

  pid = fork();
  switch (pid)
  {
  case 0:
#ifndef _FREEBSD
    setpgrp();
#endif
    m_pChild->SetSendPipe(GetSendPipe());
    m_pChild->SetPid(getpid());
    m_pChild->Run();
    //this->Run();
    //Run();
    exit(0);

  case -1:
    Assert(false, "Process Spawn failure! ");
    return false;

  default:
    /*
         this->SetStarted(true);
         this->SetPid(pid);
         */
    break;
  }

  return true;
}

const bool Process::SpawnChildProcess()
{
  int pid = 0;

  pid = fork();
  switch (pid)
  {
  case 0:
#ifndef _FREEBSD
    setpgrp();
#endif
    m_pChild->SetPid(getpid());
    m_pChild->Run();
    //this->Run();
    //Run();
    exit(0);

  case -1:
    Assert(false, "Process Spawn failure! ");
    return false;

  default:
    /*
         fprintf(stderr, "Process id => (%d), (%d)", pid, getpid());
         this->SetStarted(true);
         this->SetPid(pid);
         */
    break;
  }

  return true;
}

const bool Process::SpawnProcess(Process *_pProcess, int _iProcessCnt)
{
  m_pChild = _pProcess;

  for (int i = 0; i < _iProcessCnt; i++)
  {
    int pid = 0;
    pid = fork();
    switch (pid)
    {
    case 0:
#ifndef _FREEBSD
      setpgrp();
#endif
      //_pProcess->SetSeq(i);
      _pProcess->SetPid(getpid());
      _pProcess->Run();
      //this->Run();
      //Run();
      exit(0);

    case -1:
      Assert(false, "Process Spawn failure! ");
      return false;

    default:
      this->SetStarted(true);
      this->SetPid(pid);
      /*
#ifdef _FREEBSD
fprintf(stderr, "Process child => (%d), (%d)\n", pid, getpid());
AddChildPid(pid);
#endif
*/

      // execute parent process
      //      Run();

      break;
    }
  }

  return true;
}

const bool Process::SpawnProcessWithPipe(Process *_pProcess, int _iProcessCnt)
{
  m_pChild = _pProcess;

  int iPipe[2];
  if (pipe(iPipe) < 0)
  //if(pipe(m_iPipe) < 0)
  {
    return -1;
  }

  for (int i = 0; i < _iProcessCnt; i++)
  {
    int pid = 0;
    pid = fork();
    switch (pid)
    {
    case 0:
      /*
           close(m_iPipe[0]);  // close read pipe
           m_iPipe[0] = -1;
           */
      close(iPipe[0]); // close read pipe
      iPipe[0] = -1;

      /*
           printf("child ================>> recvpipe =(%d) \n", iPipe[0]);
           printf("child ================>> sendpipe =(%d) \n", iPipe[1]);
           */

      _pProcess->SetSendPipe(iPipe[1]);

      _pProcess->SetPid(getpid());
#ifndef _FREEBSD
      setpgrp();
#endif
      _pProcess->Run();
      //this->Run();
      //Run();
      exit(0);

    case -1:
      Assert(false, "Process Spawn failure! ");
      return false;

    default:
      /*
           close(m_iPipe[1]);  // close write pipe
           m_iPipe[1] = -1;

           close(iPipe[1]);  // close write pipe
           iPipe[1] = -1;

           printf("parent ================>> recvpipe =(%d) \n", iPipe[0]);
           printf("parent ================>> sendpipe =(%d) \n", iPipe[1]);
           */
      SetRecvPipe(iPipe[0]);
      SetSendPipe(iPipe[1]);

      this->SetStarted(true);
      this->SetPid(pid);

      // execute parent process
      //      Run();

      break;
    }
  }

  return true;
}
