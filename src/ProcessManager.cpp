#include "../include/ProcessManager.h"

ProcessManager*  ProcessManager::m_SelfInstance = 0;

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
  std::list<Process*>::iterator pos, posPrev;

  printf("ProcessManager destructor\n");

  pos = m_LstProcess.begin();
  while( pos != m_LstProcess.end() )
  {
    posPrev = pos++;
    m_LstProcess.erase( posPrev );

    /**
     *
     */
    printf("((Process *)(*posPrev))->GetPid() = '%d' \n", ((Process *)(*posPrev))->GetPid());
    kill(((Process *)(*posPrev))->GetPid(), 9);
    delete *posPrev;
  }
}

ProcessManager* ProcessManager::GetInstance()
{
  if( m_SelfInstance == 0)
    m_SelfInstance = new ProcessManager();

  return m_SelfInstance;
}

void ProcessManager::ReleaseInstance()
{
  if( m_SelfInstance != 0 )
  {
    delete m_SelfInstance;
    m_SelfInstance = 0;
  }
}

/**
 * child process watchdog.
 */
void ProcessManager::SpawnProcessAndWatchDog(int _iProcessCount)
{
  if(!SpawnProcess())
  {
    exit(0);
  }

  while(1)
  {
    if(!WatchProcess())
    {
      break;
    }
  }
}

/**
 *
 */
bool ProcessManager::SpawnProcess(pid_t _pid)
{
  std::list<Process*>::iterator pos, posPrev;

  pos = m_LstProcess.begin();
  while( pos != m_LstProcess.end() )
  {
    posPrev = pos++;

    if(((Process *)(*posPrev))->GetPid() == _pid)
    {
      ((Process *)(*posPrev))->SpawnProcess();
    }

    printf("WatchPid => '%d' \n", ((Process *)(*posPrev))->GetPid());
  }

  return true;
}

bool ProcessManager::WatchProcess()
{
  int status;
  pid_t wait_pid;

  wait_pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
  if(wait_pid <= 0)
  {
    sleep(5);
    return true;
  }

  if(wait_pid > 0) /* process killed */
  {
    if (WIFEXITED(status))
    {
      CNPLog::GetInstance().Log("1.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, WIFEXITED(status), WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
      CNPLog::GetInstance().Log("2.Catch the signal. pid=(%d) status=(%d,%d)", wait_pid, status, WIFSIGNALED(status));
    }

    /**
     *   restart process
     */
    if(!SpawnProcess(wait_pid))
    {
      return false;
    }
  }

  return true;
}

bool ProcessManager::Spawn(Process *_pProcess)
{
  /**
   * Process create
   */
  /*
     if(!SpawnProcess(_pProcess))
     return false;
     */

  if(_pProcess->SpawnProcess())
  {
    m_LstProcess.push_back(_pProcess);
  }
  else
  {
    return false;
  }

  /**
   * Child Process
   */
  //  WatchProcess();

  return true;
}

bool ProcessManager::DeleteProcess(pid_t _pid)
{
  std::list<Process*>::iterator pos, posPrev;

  pos = m_LstProcess.begin();
  while( pos != m_LstProcess.end() )
  {
    posPrev = pos++;

    if(((Process *)(*posPrev))->GetPid() == _pid)
    {
      printf("ProcessManager::DeleteProcess => '%d' \n", ((Process *)(*posPrev))->GetPid());
      /*
         m_LstProcess.erase(((Process *)(*posPrev)));
         */
      m_LstProcess.erase(posPrev);

      kill(((Process *)(*posPrev))->GetPid(), 9);
      delete *posPrev;
    }
  }

  return true;
}


