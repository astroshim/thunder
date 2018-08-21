#include "../include/NPUtil.h"

int CNPUtil::m_iEqualNetworkByteOrder = GetByteOrder();
/*
   unsigned int CNPUtil::m_iSessionKey = 1;
   pthread_mutex_t CNPUtil::m_lockKey = PTHREAD_MUTEX_INITIALIZER;
   */

void sig_timer(int signo)
{
  //  setitimer(ITIMER_REAL, &timer, NULL);
  return;
}

CNPUtil::CNPUtil()
{
}

int CNPUtil::GetNetworkByteOrder()
{
  return m_iEqualNetworkByteOrder;
}

void CNPUtil::SetSignal(int _sec)
{
  struct sigaction alrm_sig;
  struct itimerval timer;      /* setitime */

  alrm_sig.sa_handler = sig_timer;
  alrm_sig.sa_flags = 0;

  if( (sigemptyset(&alrm_sig.sa_mask) == -1) ||
      (sigaction(SIGALRM, &alrm_sig, NULL) == -1) )
  {
    perror("Failed to set Alarm Signal handler");
    return ;
  }

  timer.it_value.tv_sec = _sec;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_usec = 0;
  timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer, NULL);

  return;
}

void CNPUtil::GetIPConfig( unsigned long long *_piMac,  unsigned long *_piIp)
{
  int sock, ret;
  struct ifreq ifr;

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("socket() error\n");
    return;
  }

  strcpy(ifr.ifr_name, "eth0");

  // Get IP Adress
  ret = ioctl (sock, SIOCGIFADDR, &ifr);
  if (ret < 0)
  {
    close(sock);
    return;
  }
  *_piIp = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

#ifndef _FREEBSD
  // Get Netmask
  //ret = ioctl(sock, SIOCGIFNETMASK, &ifr);
  ret = ioctl (sock, SIOCGIFHWADDR, &ifr);
  if (ret < 0)
  {
    close(sock);
    return ;
  }

  struct sockaddr *sa = &(ifr.ifr_hwaddr);;
  memcpy((void *)_piMac, (void *)&(sa->sa_data), 6);

  struct ether_addr haddr;
  memcpy((void *)&haddr, (void *)_piMac, 6);
#else
  _piMac = NULL;
#endif

  close(sock);
}

void CNPUtil::NSleep(const unsigned long  _usec)
{
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = _usec;

  select( 0, NULL, NULL, NULL, &tv );
}

void CNPUtil::replaceAll(char *s, const char *olds, const char *news)
{
  char result[256], *sr, *pOrigin;
  size_t i, count = 0;
  size_t oldlen = strlen(olds); if (oldlen < 1) return;
  size_t newlen = strlen(news);

  if (newlen != oldlen)
  {
    for (i = 0; s[i] != '\0';)
    {
      if (memcmp(&s[i], olds, oldlen) == 0) count++, i += oldlen;
      else i++;
    }
  } else i = strlen(s);


  memset(result, 0x00, sizeof(result));
  //result = (char *) malloc(i + 1 + count * (newlen - oldlen));
  //if (result == NULL) return NULL;

  pOrigin = s;
  sr = &(result[0]);
  while (*pOrigin)
  {
    if (memcmp(pOrigin, olds, oldlen) == 0)
    {
      memcpy(sr, news, newlen);
      sr += newlen;
      pOrigin  += oldlen;
    }
    else *sr++ = *pOrigin++;
  }
  *sr = '\0';
  strcpy(s, result);
}

void CNPUtil::NanoSleep(const unsigned long _nsec)
{
  struct timespec delay;           /* used for wasting time */

  delay.tv_sec = 0;
  delay.tv_nsec = _nsec;
  nanosleep(&delay, NULL);
}

void CNPUtil::LTrim(char *str)
{
  char* tmp;
  for(tmp=str; tmp[0] && isspace(tmp[0]); tmp++);
  strcpy(str,tmp);
}

void CNPUtil::RTrim(char *str)
{
  char* tmp;
  for(tmp=str-1; str[0]; str++)
  {
    if(!isspace(str[0])) tmp=str;
  }
  tmp[1]=0;
}

void CNPUtil::GetTime(struct time_buf  *_time)
{
  struct timeval tmp_time;

  _time->tv_sec =  time(0);
  gettimeofday(&tmp_time, (struct timezone *)0);
  _time->tv_usec = tmp_time.tv_sec;
}

/************************************************************************
 ** get_time - Get current date and time
 ** Arguments - todaystr : string to store YYYYMMDD (possibly NULL)
 **             timestr  : string to store HHMMSS   (possibly NULL)
 **/
void CNPUtil::GetTime(char *todaystr, char* timestr)
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

int CNPUtil::GetTime()
{
  time_t tval;
  struct tm t;

  time(&tval);
  localtime_r(&tval, &t);

  return t.tm_hour;
}

time_t CNPUtil::GetUnixTime()
{
  time_t tval;
  struct tm t;

  time(&tval);

  localtime_r(&tval, &t);
  return tval;
}

void CNPUtil::GetUnixTimeStr(unsigned long _ulUnixTime, char *_pchTime)
{
  time_t tval;
  struct tm t;

  tval = _ulUnixTime;
  localtime_r(&tval, &t);

  if ( _pchTime )
  {
    sprintf(_pchTime, "%02d%02d%02d", t.tm_hour, t.tm_min, t.tm_sec);
  }
}

const double CNPUtil::GetMicroTime(void)
{
  struct timeval tv;
  struct timezone tz;

  if (gettimeofday(&tv, &tz) < 0) {
    return 0.0;
  }
  return (double) tv.tv_sec + ((double) tv.tv_usec) / 1000000.0;
}

void CNPUtil::GetMicroTimeStr(double _dTime, char *_pchTime)
{
  struct timeval tv;
  struct tm t;
  //char _pchTimeString[40];

  if(_dTime < 0.0)
  {
    return;
  }

  memcpy(&tv, &_dTime, sizeof(struct timeval));
  // struct tm *t;
  //t = localtime (&tv.tv_sec);
#ifdef _FREEBSD
  localtime_r((time_t *)(&(tv.tv_sec)), &t);
#else
  localtime_r(&(tv.tv_sec), &t);
#endif

  //strftime (_pchTimeString, 40, "%Y-%m-%d %H:%M:%S", t);
  if ( _pchTime )
  {
    sprintf(_pchTime, "%02d%02d%02d", t.tm_hour, t.tm_min, t.tm_sec);
  }
}

void CNPUtil::GetMicroTime(char *_pchTimeString)
{
  struct timeval tv;
  struct tm* ptm;
  //  char _pchTimeString[50];
  //  long milliseconds;

  /* Obtain the time of day, and convert it to a tm struct. */
  gettimeofday (&tv, NULL);
#ifdef _FREEBSD
  ptm = localtime ((time_t *)(&tv.tv_sec));
#else
  ptm = localtime (&tv.tv_sec);
#endif

  /* Format the date and time, down to a single second. */
  strftime (_pchTimeString, 50, "%Y-%m-%d %H:%M:%S", ptm);

  /* Compute milliseconds from microseconds. */
  //  milliseconds = tv.tv_usec / 1000;

  /* Print the formatted time, in seconds, followed by a decimal point
     and the milliseconds. */
  //  printf ("%s.%03ld\n", _pchTimeString, milliseconds);

  sprintf(_pchTimeString, "%s%ld", _pchTimeString, tv.tv_usec);
}

int CNPUtil::GetMin()
{
  time_t tval;
  struct tm t;

  time(&tval);
  localtime_r(&tval, &t);

  return t.tm_min;
}

/************************************************************************
 ** get_day - Get current day
 ** Arguments - VOID
 **/

/************************************************************************
 ** get_day - Get current day
 ** Arguments - VOID
 **/
int CNPUtil::GetCurDay(void)
{
  time_t tval;
  struct tm t;

  time(&tval);
  localtime_r(&tval, &t);

  return t.tm_mday;
}

int CNPUtil::GetResultHtml(char *pchSrc, char *pchDst)
{
  char *p;

  if( (p=strstr(pchSrc, "\r\n\r\n")) == NULL )
    return -1;

  strncpy(pchDst, p+4, strlen(p+4)+1);
  pchDst[strlen(pchDst)+1] = '\0';

  return strlen(pchDst);
}

/**
 *   get the page "a.html" from "GET /a.html HTTP/1.1......."
 */
int CNPUtil::GetQueryPage(char *Data, char *_pchURL)
{
  char *p, *q, *r;

  if( (p=strstr(Data, "GET ")) == NULL )
    return -1;

  if( (q=strchr(p+4, '/')) == NULL )
    return -1;

  if( (r=strstr(q+1, " HTTP")) == NULL )
    return -1;

  strncpy(_pchURL, q+1, r-q-1);
  return 0;
}

/**
 * get the url "xxx.xxx.gameloft.com" from "http://xxx.xxx.gameloft.com/".
 */
int CNPUtil::GetURL(char *Data, char *_pchURL)
{
  char *p, *q, *r;

  if( (p=strstr(Data, "http:/")) == NULL )
    return -1;

  if( (q=strchr(p+6, '/')) == NULL )
    return -1;

  if( (r=strchr(q+1, '/')) == NULL )
    return -1;

  strncpy(_pchURL, q+1, r-q-1);
  return 0;
}

int CNPUtil::GetResultCode(char *Data)
{
  char *p, *q, *r;
  char ResultCode[20];

  if( (p=strstr(Data, "Result Code")) == NULL )
    return -1;

  if( (q=strchr(p+11, '\"')) == NULL )
    return -1;

  if( (r=strchr(q+1, '\"')) == NULL )
    return -1;

  strncpy(ResultCode, q+1, r-q-1);
  ResultCode[r-q-1] = '\0';

  printf("ResultCode = (%s) \n", ResultCode);
  //    return atoi(ResultCode);
  return 0;
}

//////////////////////////////////////////////////////////////////////////
int CNPUtil::Get4Byte(unsigned char *data,int pos)
{
  union {
    int integral;
    unsigned char chars[4];
  } bytes;

  if (!m_iEqualNetworkByteOrder)  // little endian
  {
    bytes.chars[0] = data[pos+3];
    bytes.chars[1] = data[pos+2];
    bytes.chars[2] = data[pos+1];
    bytes.chars[3] = data[pos];
  }
  else
  {
    bytes.chars[0] = data[pos];
    bytes.chars[1] = data[pos+1];
    bytes.chars[2] = data[pos+2];
    bytes.chars[3] = data[pos+3];
  }

  return  bytes.integral;
}

//////////////////////////////////////////////////////////////////////////
int CNPUtil::Get2Byte(unsigned char *data,int pos)
{
  //  CNPLog::GetInstance().Log("====================>> data=(%p) (%d)", data, pos);

  union {
    unsigned short integral;
    unsigned char chars[2];
  } bytes;

  if (!m_iEqualNetworkByteOrder)  // little endian
  {
    bytes.chars[0] = data[pos+1];
    bytes.chars[1] = data[pos];
  }
  else
  {
    bytes.chars[0] = data[pos];
    bytes.chars[1] = data[pos+1];
  }
  //  CNPLog::GetInstance().Log("====================>> (%d) (%d) (%d)", bytes.chars[0], bytes.chars[1], bytes.integral);

  return  bytes.integral;
}

//////////////////////////////////////////////////////////////////////////
/*
   int CNPUtil::Get1Byte(unsigned char *data,int pos)
   {
   union {
   short integral;
   char chars[1];
   } bytes;

   if (!m_iEqualNetworkByteOrder)  // little endian
   {
   bytes.chars[0] = data[pos+1];
   bytes.chars[1] = data[pos];
   }
   else
   {
   bytes.chars[0] = data[pos];
   bytes.chars[1] = data[pos+1];
   }

   return  bytes.integral;
   }
   */

unsigned long CNPUtil::GetLastChangedTime(const char* const _pchDirName)
{
  struct  stat st;

  if( (stat(_pchDirName, &st)) < 0 )
  {
    if( errno == ENOENT )
    {
    }
    else
    {
    }

    return 0;
  }

  return st.st_mtime;
}

void CNPUtil::DeleteFile(const char* const _pchDirName, const unsigned long _uiTime)
{
  char pchFileName[MAX_FILE_NAME];
  struct dirent *item;
  DIR *dp;

  dp = opendir(_pchDirName);
  if (dp != NULL)
  {
    for(;;)
    {
      item = readdir(dp);
      if (item == NULL)
      {
        break;
      }

      if(strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
      {
        continue;
      }

      memset(pchFileName, 0x00, MAX_FILE_NAME);
      sprintf(pchFileName, "%s/%s", _pchDirName, item->d_name);

      if(GetLastChangedTime(pchFileName) + _uiTime < GetUnixTime())
      {
        unlink(pchFileName);
      }

    }
    closedir(dp);
  }
  //rmdir(_pchDirName);
}

int  CNPUtil::GetByteOrder(void)
{
  union {
    short i2;
    char  i1[2];
  } val;
  val.i2 = 0x0102;

  if(val.i1[0]==0x02 )
  {
    return 0; // little endian system
  }

  return 1; // big endian system
}


void CNPUtil::Change344To444FormatPhoneNumber(char *src)
{
  char    Type444PhoneNumber[12];

  if( strncmp(src, "10", 2) == 0 )
  {
    Type444PhoneNumber[0] = '0';
    Type444PhoneNumber[1] = '1';
    Type444PhoneNumber[2] = '0';
    Type444PhoneNumber[3] = '0';

    if( strlen(src) == 9 )
    {
      Type444PhoneNumber[4] = '0';
      Type444PhoneNumber[5] = src[2];
      Type444PhoneNumber[6] = src[3];
      Type444PhoneNumber[7] = src[4];

      Type444PhoneNumber[8] = src[5];
      Type444PhoneNumber[9] = src[6];
      Type444PhoneNumber[10] = src[7];
      Type444PhoneNumber[11] = src[8];
    }
    else
    {
      Type444PhoneNumber[4] = src[2];
      Type444PhoneNumber[5] = src[3];
      Type444PhoneNumber[6] = src[4];
      Type444PhoneNumber[7] = src[5];

      Type444PhoneNumber[8] = src[6];
      Type444PhoneNumber[9] = src[7];
      Type444PhoneNumber[10] = src[8];
      Type444PhoneNumber[11] = src[9];
    }
  }
  else
  {
    Type444PhoneNumber[0] = src[0];
    Type444PhoneNumber[1] = src[1];
    Type444PhoneNumber[2] = src[2];
    Type444PhoneNumber[3] = '0';

    if( strlen(src) == 10 )
    {
      Type444PhoneNumber[4] = '0';
      Type444PhoneNumber[5] = src[3];
      Type444PhoneNumber[6] = src[4];
      Type444PhoneNumber[7] = src[5];

      Type444PhoneNumber[8] = src[6];
      Type444PhoneNumber[9] = src[7];
      Type444PhoneNumber[10] = src[8];
      Type444PhoneNumber[11] = src[9];

    }
    else
    {
      Type444PhoneNumber[4] = src[3];
      Type444PhoneNumber[5] = src[4];
      Type444PhoneNumber[6] = src[5];
      Type444PhoneNumber[7] = src[6];

      Type444PhoneNumber[8] = src[7];
      Type444PhoneNumber[9] = src[8];
      Type444PhoneNumber[10] = src[9];
      Type444PhoneNumber[11] = src[10];
    }

    strncpy(src, Type444PhoneNumber, 12);
  }
}

int CNPUtil::URLDecode(const char *src, char *dst)
{
  int state = 0, code, i;

  if ( !src )
    return -1;

  for(i = 0; i < strlen(src); i++) {
    switch (state) {
      case 0:
        if (src[i] == '%') {
          state = 1;
        } else {
          *dst++ = src[i];
        }
        break;
      case 1:
        if (isdigit(src[i]) == 0) {

          errno = EILSEQ;
          return -1;
        }
        code = src[i] - 48;
      case 2:
        if (state == 2) {
          /* charicter */
          if (isdigit(src[i]) == 0) {
            if (isalnum(src[i]) == 0) {
              errno = EILSEQ;
              return -1;
            }
            *dst++ = (code * 16) + src[i] - 55;
          }
          else {
            /* digit */
            *dst++ = (code * 16) + src[i] - 48;
          }

          state = 0;
        } else {
          state = 2;
        }
        break;
    }
  }
  *dst = '\0';

  return 0;
}

/*
 *  FIPS-180-2 compliant SHA-256 implementation
 *
 *  Copyright (C) 2001-2003  Christophe Devine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define GET_UINT32(n,b,i)                       \
{                                               \
  (n) = ( (uint32) (b)[(i)    ] << 24 )       \
  | ( (uint32) (b)[(i) + 1] << 16 )       \
  | ( (uint32) (b)[(i) + 2] <<  8 )       \
  | ( (uint32) (b)[(i) + 3]       );      \
}

#define PUT_UINT32(n,b,i)                       \
{                                               \
  (b)[(i)    ] = (uint8) ( (n) >> 24 );       \
  (b)[(i) + 1] = (uint8) ( (n) >> 16 );       \
  (b)[(i) + 2] = (uint8) ( (n) >>  8 );       \
  (b)[(i) + 3] = (uint8) ( (n)       );       \
}

void CNPUtil::sha256_starts( sha256_context *ctx )
{
  ctx->total[0] = 0;
  ctx->total[1] = 0;

  ctx->state[0] = 0x6A09E667;
  ctx->state[1] = 0xBB67AE85;
  ctx->state[2] = 0x3C6EF372;
  ctx->state[3] = 0xA54FF53A;
  ctx->state[4] = 0x510E527F;
  ctx->state[5] = 0x9B05688C;
  ctx->state[6] = 0x1F83D9AB;
  ctx->state[7] = 0x5BE0CD19;
}

void CNPUtil::sha256_process( sha256_context *ctx, uint8 data[64] )
{
  uint32 temp1, temp2, W[64];
  uint32 A, B, C, D, E, F, G, H;

  GET_UINT32( W[0],  data,  0 );
  GET_UINT32( W[1],  data,  4 );
  GET_UINT32( W[2],  data,  8 );
  GET_UINT32( W[3],  data, 12 );
  GET_UINT32( W[4],  data, 16 );
  GET_UINT32( W[5],  data, 20 );
  GET_UINT32( W[6],  data, 24 );
  GET_UINT32( W[7],  data, 28 );
  GET_UINT32( W[8],  data, 32 );
  GET_UINT32( W[9],  data, 36 );
  GET_UINT32( W[10], data, 40 );
  GET_UINT32( W[11], data, 44 );
  GET_UINT32( W[12], data, 48 );
  GET_UINT32( W[13], data, 52 );
  GET_UINT32( W[14], data, 56 );
  GET_UINT32( W[15], data, 60 );

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
  (                                               \
                                                  W[t] = S1(W[t -  2]) + W[t -  7] +          \
                                                  S0(W[t - 15]) + W[t - 16]            \
                                                  )

#define P(a,b,c,d,e,f,g,h,x,K)                  \
  {                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
  }

  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];
  E = ctx->state[4];
  F = ctx->state[5];
  G = ctx->state[6];
  H = ctx->state[7];

  P( A, B, C, D, E, F, G, H, W[ 0], 0x428A2F98 );
  P( H, A, B, C, D, E, F, G, W[ 1], 0x71374491 );
  P( G, H, A, B, C, D, E, F, W[ 2], 0xB5C0FBCF );
  P( F, G, H, A, B, C, D, E, W[ 3], 0xE9B5DBA5 );
  P( E, F, G, H, A, B, C, D, W[ 4], 0x3956C25B );
  P( D, E, F, G, H, A, B, C, W[ 5], 0x59F111F1 );
  P( C, D, E, F, G, H, A, B, W[ 6], 0x923F82A4 );
  P( B, C, D, E, F, G, H, A, W[ 7], 0xAB1C5ED5 );
  P( A, B, C, D, E, F, G, H, W[ 8], 0xD807AA98 );
  P( H, A, B, C, D, E, F, G, W[ 9], 0x12835B01 );
  P( G, H, A, B, C, D, E, F, W[10], 0x243185BE );
  P( F, G, H, A, B, C, D, E, W[11], 0x550C7DC3 );
  P( E, F, G, H, A, B, C, D, W[12], 0x72BE5D74 );
  P( D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE );
  P( C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7 );
  P( B, C, D, E, F, G, H, A, W[15], 0xC19BF174 );
  P( A, B, C, D, E, F, G, H, R(16), 0xE49B69C1 );
  P( H, A, B, C, D, E, F, G, R(17), 0xEFBE4786 );
  P( G, H, A, B, C, D, E, F, R(18), 0x0FC19DC6 );
  P( F, G, H, A, B, C, D, E, R(19), 0x240CA1CC );
  P( E, F, G, H, A, B, C, D, R(20), 0x2DE92C6F );
  P( D, E, F, G, H, A, B, C, R(21), 0x4A7484AA );
  P( C, D, E, F, G, H, A, B, R(22), 0x5CB0A9DC );
  P( B, C, D, E, F, G, H, A, R(23), 0x76F988DA );
  P( A, B, C, D, E, F, G, H, R(24), 0x983E5152 );
  P( H, A, B, C, D, E, F, G, R(25), 0xA831C66D );
  P( G, H, A, B, C, D, E, F, R(26), 0xB00327C8 );
  P( F, G, H, A, B, C, D, E, R(27), 0xBF597FC7 );
  P( E, F, G, H, A, B, C, D, R(28), 0xC6E00BF3 );
  P( D, E, F, G, H, A, B, C, R(29), 0xD5A79147 );
  P( C, D, E, F, G, H, A, B, R(30), 0x06CA6351 );
  P( B, C, D, E, F, G, H, A, R(31), 0x14292967 );
  P( A, B, C, D, E, F, G, H, R(32), 0x27B70A85 );
  P( H, A, B, C, D, E, F, G, R(33), 0x2E1B2138 );
  P( G, H, A, B, C, D, E, F, R(34), 0x4D2C6DFC );
  P( F, G, H, A, B, C, D, E, R(35), 0x53380D13 );
  P( E, F, G, H, A, B, C, D, R(36), 0x650A7354 );
  P( D, E, F, G, H, A, B, C, R(37), 0x766A0ABB );
  P( C, D, E, F, G, H, A, B, R(38), 0x81C2C92E );
  P( B, C, D, E, F, G, H, A, R(39), 0x92722C85 );
  P( A, B, C, D, E, F, G, H, R(40), 0xA2BFE8A1 );
  P( H, A, B, C, D, E, F, G, R(41), 0xA81A664B );
  P( G, H, A, B, C, D, E, F, R(42), 0xC24B8B70 );
  P( F, G, H, A, B, C, D, E, R(43), 0xC76C51A3 );
  P( E, F, G, H, A, B, C, D, R(44), 0xD192E819 );
  P( D, E, F, G, H, A, B, C, R(45), 0xD6990624 );
  P( C, D, E, F, G, H, A, B, R(46), 0xF40E3585 );
  P( B, C, D, E, F, G, H, A, R(47), 0x106AA070 );
  P( A, B, C, D, E, F, G, H, R(48), 0x19A4C116 );
  P( H, A, B, C, D, E, F, G, R(49), 0x1E376C08 );
  P( G, H, A, B, C, D, E, F, R(50), 0x2748774C );
  P( F, G, H, A, B, C, D, E, R(51), 0x34B0BCB5 );
  P( E, F, G, H, A, B, C, D, R(52), 0x391C0CB3 );
  P( D, E, F, G, H, A, B, C, R(53), 0x4ED8AA4A );
  P( C, D, E, F, G, H, A, B, R(54), 0x5B9CCA4F );
  P( B, C, D, E, F, G, H, A, R(55), 0x682E6FF3 );
  P( A, B, C, D, E, F, G, H, R(56), 0x748F82EE );
  P( H, A, B, C, D, E, F, G, R(57), 0x78A5636F );
  P( G, H, A, B, C, D, E, F, R(58), 0x84C87814 );
  P( F, G, H, A, B, C, D, E, R(59), 0x8CC70208 );
  P( E, F, G, H, A, B, C, D, R(60), 0x90BEFFFA );
  P( D, E, F, G, H, A, B, C, R(61), 0xA4506CEB );
  P( C, D, E, F, G, H, A, B, R(62), 0xBEF9A3F7 );
  P( B, C, D, E, F, G, H, A, R(63), 0xC67178F2 );

  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
  ctx->state[5] += F;
  ctx->state[6] += G;
  ctx->state[7] += H;
}

void CNPUtil::sha256_update( sha256_context *ctx, uint8 *input, uint32 length )
{
  uint32 left, fill;

  if( ! length ) return;

  left = ctx->total[0] & 0x3F;
  fill = 64 - left;

  ctx->total[0] += length;
  ctx->total[0] &= 0xFFFFFFFF;

  if( ctx->total[0] < length )
    ctx->total[1]++;

  if( left && length >= fill )
  {
    memcpy( (void *) (ctx->buffer + left),
        (void *) input, fill );
    sha256_process( ctx, ctx->buffer );
    length -= fill;
    input  += fill;
    left = 0;
  }

  while( length >= 64 )
  {
    sha256_process( ctx, input );
    length -= 64;
    input  += 64;
  }

  if( length )
  {
    memcpy( (void *) (ctx->buffer + left),
        (void *) input, length );
  }
}

static uint8 sha256_padding[64] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void CNPUtil::sha256_finish( sha256_context *ctx, uint8 digest[32] )
{
  uint32 last, padn;
  uint32 high, low;
  uint8 msglen[8];

  high = ( ctx->total[0] >> 29 )
    | ( ctx->total[1] <<  3 );
  low  = ( ctx->total[0] <<  3 );

  PUT_UINT32( high, msglen, 0 );
  PUT_UINT32( low,  msglen, 4 );

  last = ctx->total[0] & 0x3F;
  padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

  sha256_update( ctx, sha256_padding, padn );
  sha256_update( ctx, msglen, 8 );

  PUT_UINT32( ctx->state[0], digest,  0 );
  PUT_UINT32( ctx->state[1], digest,  4 );
  PUT_UINT32( ctx->state[2], digest,  8 );
  PUT_UINT32( ctx->state[3], digest, 12 );
  PUT_UINT32( ctx->state[4], digest, 16 );
  PUT_UINT32( ctx->state[5], digest, 20 );
  PUT_UINT32( ctx->state[6], digest, 24 );
  PUT_UINT32( ctx->state[7], digest, 28 );
}


int CNPUtil::GoSha256(char *_pchSource, int _iLen, unsigned char *_pchResult)
{
  sha256_context ctx;

  sha256_starts( &ctx );
  sha256_update( &ctx, (uint8 *) _pchSource, _iLen );
  sha256_finish( &ctx, _pchResult );
  return 0;
}


#ifdef _CRYPTO
DWORD CNPUtil::EncryptCharToUCHAR( LPCSTR szOrigin, DWORD dwHowMuch, UCHAR* lpszKey, DWORD dwKeySize, UCHAR* pbCipherText)
{
  if ( dwHowMuch < 0 || dwHowMuch > CRYPTO_MSG_SIZE )
    dwHowMuch = CheckLength((UCHAR*)szOrigin, CRYPTO_MSG_SIZE);

  DWORD dwErr = Encrypt(lpszKey, dwKeySize, (UCHAR*)szOrigin, pbCipherText, dwHowMuch);
  DWORD dwRet = 0;

  if ( 0 == dwErr )
    dwRet = dwHowMuch;

  return dwRet;
}

DWORD CNPUtil::DecryptUCHARToChar( UCHAR* pbCipherText, DWORD dwHowMuch,  UCHAR* lpszKey, DWORD dwKeySize, LPSTR szOrigin)
{
  if ( dwHowMuch < 0 || dwHowMuch > CRYPTO_MSG_SIZE )
    dwHowMuch = CheckLength(pbCipherText, CRYPTO_MSG_SIZE);

  CNPLog::GetInstance().Log("DecryptUCHARToChar ==> dwHowMuch(%d)", dwHowMuch);

  DWORD dwErr = Decrypt(lpszKey, dwKeySize, pbCipherText, (UCHAR*)szOrigin, dwHowMuch);
  DWORD dwRet = 0;

  if ( 0 == dwErr )
  {
    dwRet = dwHowMuch;
  }

  return dwRet;
}

//////////////////////////////////////////////////////////////////////////
//
DWORD CNPUtil::Encrypt(UCHAR *pbKey, DWORD dwKeySize, UCHAR *pbPlaintext, UCHAR *pbCipherText, DWORD dwHowMuch)
{
  DWORD   dwRet = false;

  RC4_KEY rc4_key;
  RC4_set_key(&rc4_key, dwKeySize, (unsigned char*)pbKey);

  RC4(&rc4_key, dwHowMuch, (unsigned char*)pbPlaintext, (unsigned char*)pbCipherText);

  return dwRet;
}

//////////////////////////////////////////////////////////////////////////
//
DWORD CNPUtil::Decrypt(UCHAR *pbKey, DWORD dwKeySize, UCHAR *pbCipherText, UCHAR *pbPlaintext, DWORD dwHowMuch)
{
  DWORD   dwRet = 0;
  RC4_KEY rc4_key;

  RC4_set_key(&rc4_key, dwKeySize, pbKey);

  RC4(&rc4_key, dwHowMuch,
      (unsigned char*)pbCipherText, (unsigned char*)pbPlaintext);
  return dwRet;
}

//----------------------------------------------------------------------------------------------------
// [10/11/2005] Written by Hyun Min Cho
// FuncName :: CheckLength
//----------------------------------------------------------------------------------------------------
/*
   DWORD CNPUtil::CheckLength( LPCTSTR lpString,  UINT nTotal)
   {
   for (int i = nTotal; i>0; i--)
   {
   if ( lpString[i-1] != NULL )
   return i;
   }

   return NULL;
   }
   */

DWORD CNPUtil::CheckLength( UCHAR* lpString,  UINT nTotal)
{
  for (int i = nTotal; i>0; i--)
  {
    if ( lpString[i-1] != 0x00 )
      return i;
  }

  return 0;
}
#endif

/*
   unsigned int CNPUtil::GetSessionKey()
   {
//  int iRetSessionKey;
pthread_mutex_lock(&m_lockKey);
unsigned int iRetSessionKey = m_iSessionKey++;
//if(m_iSessionKey > 400000000)
if(m_iSessionKey > 10)
{
m_iSessionKey = 1;
}
pthread_mutex_unlock(&m_lockKey);
return iRetSessionKey;
}
*/



