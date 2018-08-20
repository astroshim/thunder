#include "../include/NPDebug.h"

/************************************************************************
** Get current date and time
** Arguments - _pchDate : string to store YYYYMMDD (possibly NULL)
**             _pchTime  : string to store HHMMSS   (possibly NULL)
**/
void GetLogTime(char *_pchDate, char *_pchTime)
{
  time_t tval;
  struct tm t;

  time(&tval);
  localtime_r(&tval, &t);

  if ( _pchDate )
    sprintf(_pchDate, "%04d%02d%02d",
    t.tm_year+1900, t.tm_mon+1, t.tm_mday);
  if ( _pchTime )
    sprintf(_pchTime, "%02d%02d%02d",
    t.tm_hour, t.tm_min, t.tm_sec);
}

/**
*
*/
void processAssert(const char *_pchFileName, int _iLine, const char  *_pchMsg)
{
//    char pchBuffer[MAX_DEBUG_MSG];
    char pchCurDate[10];
    char pchCurTime[10];

  GetLogTime(pchCurDate, pchCurTime);

/*
  snprintf(pchBuffer, sizeof(pchBuffer), "%s %s [%s,%d]: %s\n",
      cur_date, cur_time, _pchFileName, _iLine, _pchMsg);
    printf("'%s'\n", pchBuffer);
*/

  std::ofstream fStream(
    "__assertion_failed.log", std::ios::out | std::ios::app);

  fStream << "ASSERTION FAILED! - " << pchCurDate << " " << pchCurTime << std::endl
       << "File : " << _pchFileName   << std::endl
       << "Line : " << _iLine       << std::endl
       << "Expr : " << _pchMsg      << std::endl;

  fStream.close();
}

