#ifndef  __PROCESSMANAGER_H
#define  __PROCESSMANAGER_H

#include "./Process.h"

class ProcessManager
{
  public:
    static  ProcessManager*   GetInstance();
    static  void              ReleaseInstance();

    bool Spawn(Process *_pProcess);

    bool DeleteProcess(pid_t _pid);

    bool WatchProcess();

  private:
    /**
     * Process Create
     */
    bool SpawnProcess(pid_t _pid);


    ProcessManager();
    virtual ~ProcessManager();

    static    ProcessManager*     m_SelfInstance;
    std::list   < Process* >    m_LstProcess;
};

#endif

