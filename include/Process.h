#ifndef  __PROCESS_H
#define  __PROCESS_H

#include "./Include.h"

const int MAX_PROCESS_CNT = 128;

class Process
{
  public:
    Process();
    virtual ~Process();
    virtual void    Run()=0;

    virtual void      HealthCheckUsers()=0;
    virtual const char* const GetMRTGURL()=0;
    virtual const int   GetDNServerPort()=0;
    // virtual void    SendStorageInfo()=0;
    virtual const char* const GetLogFileName()=0;


    const pid_t    GetPPid();
    void        SetPPid(const pid_t _pid);
    const pid_t    GetPid();
    void        SetPid(const pid_t _pid);

    const bool    GetStarted();
    void        SetStarted(const bool _bool);

    const int     GetSignalNo();
    void        SetSignalNo(const int _bool);

    const bool  SpawnProcess();
    const bool  SpawnProcess(Process *_pProcess, int _iProcessCnt);
    const bool  SpawnProcessWithPipe(Process *_pProcess, int _iProcessCnt);

    const bool  SpawnChildProcess();
    const bool  SpawnChildProcessWithPipe();

    /*
       const bool  WatchProcess();
       const bool  WatchProcess(Process *_pProcess);
     */
    const bool  WatchChildProcess();
    const bool  WatchChildProcessWithPipe();

    const int     GetRecvPipe();
    const int     GetSendPipe();

    void SetRecvPipe(const int _iRecvPipe);
    void SetSendPipe(const int _iSendPipe);

    const bool IsAliveProcess(const int _iPid);
    /*
       void SetSeq(const int _iSeq);
       const int GetSeq();
     */
  private:
    //pid_t m_lstChildPid[MAX_PROCESS_CNT];

    int   m_iPipe[2];

    pid_t   m_PPid;
    pid_t   m_Pid;
    bool  m_IsStarted;
    int     m_iSignalNo;
    //  int m_iSeq; // server seq

    Process *m_pChild;

    /*
       const int AddChildPid(const pid_t _pid);
       const int DelChildPid(const pid_t _pid);
     */

};
#endif

