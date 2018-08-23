#ifndef __NPLOG_H__
#define __NPLOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <dirent.h>

#include <pthread.h>

#include "./NonCopyable.h"

const unsigned int MAX_FILE_NAME = 256;
const unsigned int MAX_LOGLINE_SIZE = 1024;

//const unsigned int MAX_LOG_FILE_SIZE = 1000000000;  // 1 giga byte
const unsigned int MAX_LOG_FILE_SIZE = 50000000; // 50M

class CNPLog : public NonCopyable
{
private:
  //  static CNPLog *mCNPLog;

  /* log file fd */
  int m_iLogFd;

  /* 날짜가 변경되었는지 여부를 확인하기 위한 변수 */
  int m_iDay;

  /* Log File name */
  char m_pchFileName[MAX_FILE_NAME];

  const int OpenFd();
  void syslog_date_ch();

  const int FileSize(const char *const _pchFileName);
  const int FileCount(const char *const _pchDirName, const char *const _pchFileName);

  void BackupLogFile();
  void DeleteDir(const char *const _pchDirName);

  pthread_mutex_t m_Lock;

protected:
  CNPLog();
  //  CNPLog(char *_pchFileName);
  virtual ~CNPLog();

public:
  /*
       CNPLog();
       CNPLog(char *_pchFileName);
       ~CNPLog();
     */
  const int SetFileName(const char *const _pchFileName);

  /* 파일에다 로그 남기기 */
  void Log(const char *const message, ...);
  void LogWithOutLineFeed(const char *const message, ...);

  /* 파일에다 로그 남기기 */
  void LogDump(const char *const _pchData, const int _iSize);
  void LogDump(const char *const _pchSubject, const char *const _pchData, const int _iSize);

  static CNPLog &GetInstance();
  void BackupLogFile2(char *_pchFileName);
};

#endif //__NPLOG_H__
