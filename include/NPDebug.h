#ifndef __NPDEBUG_H
#define __NPDEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <stdarg.h>

#include <iostream>
#include <fstream>
using namespace std;

const unsigned int MAX_DEBUG_MSG = 1024;


// 파일에다 기록하는 Assert 함수
void processAssert(const char *filename, int lineNumber, const char  *message);
#define Assert(x, msg) { if (!bool(x)) { processAssert( __FILE__, __LINE__,  msg); } }

//void Assert(const char  *message, ...);

/// 포인터 검사 매크로
#define AssertPtr(ptr) \
  if(!ptr || sizeof(*ptr) <= 0) \
Assert(false, "Invalid Ptr"); \

// Get current date and time
void GetLogTime(char *todaystr, char* timestr);

#endif

