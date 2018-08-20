#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

int m_iLogFd;
void GetTime(char *todaystr, char* timestr)
{
    time_t tval;
    struct tm t;

    time(&tval);
    localtime_r(&tval, &t);

    if ( todaystr )
        sprintf(todaystr, "%04d%02d%02d",
        t.tm_year+1900, t.tm_mon+1, t.tm_mday);
    if ( timestr )
        sprintf(timestr, "%02d%02d%02d",
        t.tm_hour, t.tm_min, t.tm_sec);
}

int OpenLogFile(char *_pchFileName)
{
  FILE * fpFile;

  if ((fpFile = fopen(_pchFileName, "w")) == NULL)
  {
    return -1;
  }
  m_iLogFd = fileno(fpFile);

  return 0;
}

int CloseLogFile()
{
  close(m_iLogFd);
  return 0;
}

void Log(const char* const message, ...)
{
  va_list ap;
  char buf[512*2];
  char str[512*3];
  char cur_date[20], cur_time[20];
  int len;

    if(m_iLogFd < 0)
  {
    // can't write the log
    return;
  }

  va_start(ap, message);

# ifdef  _HAVE_VSNPRINTF
  len = vsnprintf(buf, sizeof(buf), message, ap);   /* this is safe */
# else
  len = vsprintf(buf, message, ap);                 /* this is not safe */
# endif

  GetTime(cur_date, cur_time);

  len = snprintf(str, 512*3, "%s %s : %s\n",
  cur_date, cur_time, buf);

  write(m_iLogFd, str, len);
  va_end(ap);
}
