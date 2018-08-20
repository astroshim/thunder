#ifndef __UTIL_H
#define __UTIL_H

#include "../include/Include.h"
#include <openssl/rc4.h>

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include <sys/ioctl.h>
#include <net/if.h>

#ifdef _FREEBSD
#include <netinet/if_ether.h> // for mac

// #include <net/ethernet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
//#elif _LINUX
#else
#include <netinet/ether.h> // for linux
#endif




#include <pthread.h>

typedef unsigned char UCHAR;
typedef int DWORD;
typedef unsigned int UINT;
typedef char* LPSTR;
typedef const char* LPCSTR;

/*
#ifdef USE_WCHAR
typedef wchar_t TCHAR;
#else
typedef unsigned char TCHAR;
#endif
 */
typedef wchar_t TCHAR;

typedef TCHAR* LPCTSTR;

#ifndef CRYPTO_MSG_SIZE
// default
#define CRYPTO_MSG_SIZE   8192
#endif


/* using system time */
typedef struct time_buf
{
  long    tv_sec;
  long    tv_usec;
}stTimeBuf;

//void sig_handler(int signo);

class CNPUtil
{
  private:
    static int  m_iEqualNetworkByteOrder;
    /*
       static unsigned int m_iSessionKey;  // create session key
       static pthread_mutex_t m_lockKey; // for session key
     */

    ///// sha256 variables
#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

    typedef struct
    {
      uint32 total[2];
      uint32 state[8];
      uint8 buffer[64];
    }
    sha256_context;

    ///// sha256 functions
    static void sha256_starts( sha256_context *ctx );
    static void sha256_update( sha256_context *ctx, uint8 *input, uint32 length );
    static void sha256_finish( sha256_context *ctx, uint8 digest[32] );
    static void sha256_process( sha256_context *ctx, uint8 data[64] );

    ////////////////////////////////////////////////////////////////////////////
    // RSA
    ////////////////////////////////////////////////////////////////////////////
    int prime(int);
    long int find_key(long int);
    long int GCD_RSA(long int, long int);
    long int extend(long int, long int);
    char *rsa_encrypt(const char *, long int *, int len);
    char *rsa_decoder(char *, long int, int len);

  public:
    CNPUtil();

    /**
     * Message Queue �񵿱⸦ ���ؼ�..
     */
    static void SetSignal(int _sec);

    static void NSleep(const unsigned long  _usec);
    static void NanoSleep(const unsigned long  _nsec);

    /**
     * Time �� ��´�.
     */
    static void GetTime(struct time_buf *_time);
    static void GetTime(char *todaystr, char* timestr);
    static int  GetTime();
    static time_t GetUnixTime();
    static void GetUnixTimeStr(unsigned long _ulUnixTime, char *_pchTime);
    static void GetMicroTime(char *_pchTimeString);
    static const double GetMicroTime(void);
    static void GetMicroTimeStr(double _dTime, char *_pchTimeString);

    static int GetMin();

    //
    static void GetIPConfig( unsigned long long *_piMac,  unsigned long *_piIp);


    static unsigned long GetLastChangedTime(const char* const _pchDirName);
    static void DeleteFile(const char* const _pchDirName, const unsigned long _uiTime);

    /**
     * Time �� ��´�.
     */
    static int GetCurDay();

    static void replaceAll(char *s, const char *olds, const char *news);
    /**
     * Packet Length�� ������� �Լ�
     */
    static int GetByteOrder(void);
    static int Get4Byte(unsigned char *data,int pos);
    static int Get2Byte(unsigned char *data,int pos);

    static void Change344To444FormatPhoneNumber(char *src);

    static int URLDecode(const char *src, char *dst);

    static void LTrim(char *str);
    static void RTrim(char *str);

    static int GetResultCode(char *Data);
    static int GetResultHtml(char *pchSrc, char *pchDst);
    static int GetURL(char *Data, char *_pchURL);
    static int GetQueryPage(char *Data, char *_pchURL);

    static int GetNetworkByteOrder();

    static int GoSha256(char *_pchSource, int _iLen, unsigned char *_pchResult);

    // crypt
    ////////////////////////////////////////////////////////////////////////////////////////
    static DWORD EncryptCharToUCHAR( LPCSTR szOrigin, DWORD dwHowMuch, UCHAR* lpszKey, DWORD dwKeySize, UCHAR* pbCipherText);
    static DWORD DecryptUCHARToChar( UCHAR* pbCipherText, DWORD dwHowMuch,  UCHAR* lpszKey, DWORD dwKeySize, LPSTR szOrigin);

    static DWORD CheckLength(LPCTSTR lpString, UINT nTotal);
    static DWORD CheckLength(UCHAR* lpString, UINT nTotal);
    static DWORD Decrypt(UCHAR *pbKey, DWORD dwKeySize, UCHAR *pbCipherText, UCHAR *pbPlaintext, DWORD dwHowMuch);
    static DWORD Encrypt(UCHAR *pbKey, DWORD dwKeySize, UCHAR *pbPlaintext, UCHAR *pbCipherText, DWORD dwHowMuch);
    ////////////////////////////////////////////////////////////////////////////////////////

    // get session key
    //static unsigned int GetSessionKey();
};
#endif

