#include "../include/NPLog.h"
#include "../include/NPUtil.h"
#ifdef _FREEBSD
#include <libgen.h>
#endif

CNPLog::CNPLog()
    : m_iLogFd(-1), m_iDay(0)
{
  pthread_mutex_init(&m_Lock, NULL);
}

CNPLog::~CNPLog()
{
  close(m_iLogFd);
}

const int CNPLog::SetFileName(const char *const _pchFileName)
{
  m_iDay = CNPUtil::GetCurDay();
  memset(m_pchFileName, 0x00, sizeof(char) * MAX_FILE_NAME);
  strncpy(m_pchFileName, _pchFileName, MAX_FILE_NAME);
  BackupLogFile();

  return OpenFd();
}

const int CNPLog::OpenFd()
{
  FILE *fpFile;

  //if ((fpFile = fopen(m_pchFileName, "a+")) == NULL)
  if ((fpFile = fopen(m_pchFileName, "w")) == NULL)
  {
    Assert(true, "CAN'T OPEN OR CREATE THE FILE \n");
    return -1;
  }

  m_iLogFd = fileno(fpFile);
  return 0;
}

void CNPLog::syslog_date_ch()
{

  /*
  */
  if (m_iDay != CNPUtil::GetCurDay() ||
      FileSize(m_pchFileName) > MAX_LOG_FILE_SIZE)
  {
    /*
      close(m_iLogFd);
    m_iLogFd = -1;
    */
    m_iDay = CNPUtil::GetCurDay();
  }
  else
  {
    return;
  }

  //fprintf(stderr, "1.mDay == (%d) (%s)\n", m_iDay, m_pchFileName);
  // close fd
  close(m_iLogFd);
  m_iLogFd = -1;

  BackupLogFile();
  OpenFd();
}

void CNPLog::BackupLogFile2(char *_pchFileName)
{
  char pchOrder[PATH_MAX], logdname[PATH_MAX];
  char *name_start, *name_end;
  int size;

  // check directory
  name_start = _pchFileName;
  name_end = basename(_pchFileName);
  //size = name_end - name_start;   // only linux
  size = strlen(name_start) - strlen(name_end);

  memcpy(logdname, name_start, size);
  logdname[size] = '\0';

  /*
  memset(pchOrder, 0x00, sizeof(pchOrder));
  sprintf( pchOrder, "\mv /root/DownloadServer/%s* /root/DownloadServer/%sold_log/", logdname, logdname);
*/
  memset(pchOrder, 0x00, sizeof(pchOrder));
  sprintf(pchOrder, "rm /root/thunder/%s* > /dev/null 2>&1", logdname);

  system(pchOrder);
  return;
}

void CNPLog::BackupLogFile()
{
  char oldlogdname[PATH_MAX], renfname[PATH_MAX];
  char *name_start, *name_end, *orifname;
  struct stat st;
  int size, rst;

  time_t now;
  struct tm *tm;
  struct tm buf;

  // check directory
  name_start = m_pchFileName;
  name_end = basename(m_pchFileName);
  //size = name_end - name_start;   // only linux
  size = strlen(name_start) - strlen(name_end);

  memcpy(oldlogdname, name_start, size);
  oldlogdname[size] = '\0';
  strcat(oldlogdname, "old_log");

  if ((rst = stat(oldlogdname, &st)) < 0)
  {
    // make old_log directory
    if (errno != ENOENT)
    {
      //      Assert(false, "No old_log Directory !");
      return;
    }

    if (mkdir(oldlogdname, S_IRWXU | S_IRWXG | S_IRWXO) < 0)
    {
      //      Assert(false, "Log Directory create Error!");
      return;
    }
  }

  /*
  // close fd
  close(m_iLogFd);
  m_iLogFd = -1;
*/
  // move file
  orifname = m_pchFileName;

  // change file
  now = time(NULL);
  tm = localtime_r(&now, &buf);
  sprintf(renfname, "%s/%s.%02d%02d%02d", oldlogdname, basename(orifname),
          tm->tm_year % 100,
          tm->tm_mon + 1,
          tm->tm_mday);

  int iLogFileCnt;
  if ((iLogFileCnt = FileCount(oldlogdname, basename(renfname))) > 0)
  {
    sprintf(renfname, "%s_%d", renfname, iLogFileCnt);
  }

  if (rename(orifname, renfname) < 0)
  {
    //    Assert(false, "Log rename Error!");
    // terminate
    return;
  }

  return;
}

const int CNPLog::FileSize(const char *const _pchFileName)
{
  struct stat file_stat;

  if (stat(_pchFileName, &file_stat) == 0)
    return (int)file_stat.st_size;

  return -1;
}

const int CNPLog::FileCount(const char *const _pchDirName, const char *const _pchFileName)
{
  int cnt = 0;
  struct dirent *item;
  DIR *dp;

  if (_pchFileName == NULL)
  {
    return cnt;
  }

  dp = opendir(_pchDirName);
  if (dp != NULL)
  {
    for (;;)
    {
      item = readdir(dp);
      if (item == NULL)
      {
        break;
      }
      else if (strncmp(item->d_name, _pchFileName, strlen(_pchFileName)) == 0)
      {
        cnt++;
      }
    }
    closedir(dp);
  }

  return cnt;
}

void CNPLog::DeleteDir(const char *const _pchDirName)
{
  char pchFileName[MAX_FILE_NAME];
  struct dirent *item;
  DIR *dp;

  dp = opendir(_pchDirName);
  if (dp != NULL)
  {
    for (;;)
    {
      item = readdir(dp);
      if (item == NULL)
      {
        break;
      }

      if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
      {
        continue;
      }
      memset(pchFileName, 0x00, MAX_FILE_NAME);
      sprintf(pchFileName, "%s/%s", _pchDirName, item->d_name);
      unlink(pchFileName);
    }
    closedir(dp);
  }
  rmdir(_pchDirName);
}

void CNPLog::Log(const char *const message, ...)
{
  va_list ap;
  char buf[MAX_LOGLINE_SIZE * 2];
  char str[MAX_LOGLINE_SIZE * 3];
  char cur_date[20], cur_time[20];
  int len;

  pthread_mutex_lock(&m_Lock);

  if (m_iLogFd < 0)
  {
    // can't write the log
    pthread_mutex_unlock(&m_Lock);
    return;
  }

  syslog_date_ch();
  va_start(ap, message);

#ifdef _HAVE_VSNPRINTF
  len = vsnprintf(buf, sizeof(buf), message, ap); /* this is safe */
#else
  len = vsprintf(buf, message, ap); /* this is not safe */
#endif

  CNPUtil::GetTime(cur_date, cur_time);

  /*
  while(buf[len-1] == '\n') { // '\n' ���� ����
    buf[len-1] = '\0';
    len--;
  }
*/
  len = snprintf(str, MAX_LOGLINE_SIZE * 3, "%s %s : %s\n",
                 cur_date, cur_time, buf);

  // check date and logfile backup
  //  syslog_date_ch();

  if (m_iLogFd <= 0 || (FileSize(m_pchFileName) < 0))
  {
    //fprintf(stderr, "2.mDay == (%d) (%s)\n", m_iDay, m_pchFileName);
    OpenFd();
  }

  write(m_iLogFd, str, len);
  pthread_mutex_unlock(&m_Lock);

  va_end(ap);
}

void CNPLog::LogWithOutLineFeed(const char *const message, ...)
{
  va_list ap;
  char buf[MAX_LOGLINE_SIZE * 2];
  char str[MAX_LOGLINE_SIZE * 3];
  int len;

  syslog_date_ch();
  va_start(ap, message);

#ifdef _HAVE_VSNPRINTF
  len = vsnprintf(buf, sizeof(buf), message, ap); /* this is safe */
#else
  len = vsprintf(buf, message, ap); /* this is not safe */
#endif

  /*
  while(buf[len-1] == '\n') { // '\n' ���� ����
    buf[len-1] = '\0';
    len--;
  }
*/
  len = snprintf(str, MAX_LOGLINE_SIZE * 3, "%s", buf);

  // check date and logfile backup
  //  syslog_date_ch();

  if (m_iLogFd <= 0 || (FileSize(m_pchFileName) < 0))
  {
    OpenFd();
  }

  write(m_iLogFd, str, len);
  va_end(ap);
}

/*
char *HexConvert(const char  *cpPtr, int nSize, char *title, char *data)
{
    int i, k;
    sprintf(data, "\n====================================[ Data Parsing ]========================================\n");
    sprintf(data, "%sTitle=[%s] length=[%d]\n", data, title, nSize);
    sprintf(data, "%s%s", data,"------------------------------------------------------------------------|-------------------\n");
    for (i=0; i < (nSize / 20)+1;i++)
    {
        sprintf(data,"%s%04d\t", data, i*20);
        for (k=0; k < 20; k++)
        {
            if (((i*20)+k) >= nSize)
            {
                break;
            }
            sprintf(data, "%s %02x",data, (unsigned char)cpPtr[(i*20)+k]);
        }

        if(k < 20)
            for (; k < 20; k++)
                sprintf(data, "%s   ", data);

        sprintf(data, "%s\t", data);

        for (k=0; k < 20; k++)
        {
            if (((i*20)+k) >= nSize)
            {
                break;
            }
            if((unsigned char)cpPtr[(i*20)+k] < 0x20)
                sprintf(data, "%s%c",data, '_');
            else
                sprintf(data, "%s%c",data, (unsigned char)cpPtr[(i*20)+k]);
        }
        sprintf(data,"%s|\n", data);
    }
    sprintf(data, "%s%s", data,"============================================================================================\n\n");

    return data;
}
*/

void CNPLog::LogDump(const char *const _pchSubject, const char *const _pchData, const int _iSize)
{
  int col;
  int idx;

  char b[33];
  char pchLine[81];

  Log("====================================[ LogDump ]========================================");
  Log(" Title=[%s] Length=[%d]", _pchSubject, _iSize);
  Log("-------------------------------------------------------------------------------------");

  // hex code
  memset(pchLine, 0x00, 81);
  idx = 0;
  col = 0;
  while (idx < _iSize)
  {
    if (col == 0)
    {
      memset(b, 0x00, 33);
      sprintf(b, "\n%05d      ", idx);
      strcat(pchLine, b);
    }
    memset(b, 0x00, 33);
    sprintf(b, "%02X ", (_pchData[idx] & 0x00FF));
    strcat(pchLine, b);
    /*
        if( col == 7 )
            strcat(pchLine, " ");

*/
    if (col == 15)
    {
      memset(b, 0x00, 33);
      memcpy(b, _pchData + (idx - 15), 16);
      snprintf(b, 16 + 2, " %s", _pchData + (idx - 15));
      strcat(pchLine, b);
      write(m_iLogFd, pchLine, strlen(pchLine));
      memset(pchLine, 0x00, 81);
      col = 0;
    }
    else
      col++;
    idx++;
    // ���� �� ���� �� ������ ������ ��� ��
    if (idx >= _iSize)
    {
      if (col == 0)
        break;
      for (; col <= 15; col++)
      {
        strcat(pchLine, "   ");
        if (col == 7)
          strcat(pchLine, " ");
      }
      col = _iSize;
      memset(b, 0x00, 33);
      memcpy(b, _pchData + col - (col % 16), (col % 16));
      snprintf(b, (col % 16) + 2, " %s", _pchData + col - (col % 16));
      strcat(pchLine, b);
      write(m_iLogFd, pchLine, strlen(pchLine));
      memset(pchLine, 0x00, 81);
    }
  }

  Log("\n-------------------------------------------------------------------------------------");
}

void CNPLog::LogDump(const char *const _pchData, const int _iSize)
{
  int col;
  int idx;

  char b[33];
  char pchLine[81];

  // hex code
  memset(pchLine, 0x00, 81);
  idx = 0;
  col = 0;
  while (idx < _iSize)
  {
    if (col == 0)
    {
      memset(b, 0x00, 33);
      sprintf(b, "\n%05d      ", idx);
      strcat(pchLine, b);
    }
    memset(b, 0x00, 33);
    sprintf(b, "%02X ", (_pchData[idx] & 0x00FF));
    strcat(pchLine, b);
    /*
        if( col == 7 )
            strcat(pchLine, " ");

*/
    if (col == 15)
    {
      memset(b, 0x00, 33);
      memcpy(b, _pchData + (idx - 15), 16);
      snprintf(b, 16 + 2, " %s", _pchData + (idx - 15));
      strcat(pchLine, b);
      write(m_iLogFd, pchLine, strlen(pchLine));
      memset(pchLine, 0x00, 81);
      col = 0;
    }
    else
      col++;
    idx++;
    // ���� �� ���� �� ������ ������ ��� ��
    if (idx >= _iSize)
    {
      if (col == 0)
        break;
      for (; col <= 15; col++)
      {
        strcat(pchLine, "   ");
        if (col == 7)
          strcat(pchLine, " ");
      }
      col = _iSize;
      memset(b, 0x00, 33);
      memcpy(b, _pchData + col - (col % 16), (col % 16));
      snprintf(b, (col % 16) + 2, " %s", _pchData + col - (col % 16));
      strcat(pchLine, b);
      write(m_iLogFd, pchLine, strlen(pchLine));
      //          Log("%s", pchLine);
      memset(pchLine, 0x00, 81);
    }
  }
}

CNPLog &CNPLog::GetInstance()
{
  static CNPLog mCNPLog;
  return mCNPLog;
}
