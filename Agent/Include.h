#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>

#include <errno.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/resource.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <mntent.h>
#include <linux/types.h>
#include <sys/vfs.h>

#define JJANGFILE 1   
#define CLUBFOS     2  
#define CDN     4       
#define JJANGCLUB   8  
#define VSHARE      16 
#define KDISK       32 
#define MFILE       64 
#define PDPOP       128
#define MOMODISK    256
#define ZFILE       101
#define ONDISK      102
#define FDISK       103
#define KDISK_FREE  33 
#define FDISK_FREE  104
#define GOGODISK  105 
#define DDAOGI    106 
#define VHARD     107 
#define DAWOONZ   109 
#define CLUBNEX   110 
#define ZIO     116 

#define HOME_STORAGE  99 

//#define MAX_CPUCNT  32 
/*
#define MOUNT_STR_LEN 255
#define DEVICE_STR_LEN 80
#define MAX_CPU_NUM 32

#define DF_LINE_LEN MOUNT_STR_LEN + DEVICE_STR_LEN
*/

#ifdef _VHARD
#define TMP_FILE "./.sysinfo_vhard"
#else
#define TMP_FILE "./.sysinfo"
#endif

// /proc/loadavg
typedef struct _loadavg
{
  float avg1;
  float avg5;
  float avg15;
}TLoadAvg;

// df
typedef struct _df
{
//  unsigned long long iDiskSize;   // Mbytes
//  unsigned long long iAvail;      // Mbytes
  unsigned long long iDiskSize;   // Kbytes
  unsigned long long iAvail;      // Kbytes
} TDF;

// cpuinfo
typedef struct _cpu_usage
{
  time_t ctime;
  int iCpuCnt;
  unsigned long long nUser;
  unsigned long long nNice;
  unsigned long long nSystem;
  unsigned long long nIdle;
  unsigned long long nIO;
  unsigned long long nIRQ;
/*
  unsigned long long nUser[MAX_CPUCNT];
  unsigned long long nSystem[MAX_CPUCNT];
  unsigned long long nIdle[MAX_CPUCNT];
*/
} TCpuUsage;

// memory
typedef struct _mem_data
{
  ulong mtotal;
  ulong mfree;
  ulong mbuffer;
  ulong mcached;
} TMemory;

// netstat
typedef struct _netstat
{
  char  protocol[16];
  int   recv_q; 
  int   send_q; 
  char  local_addr[64];
  char  remote_addr[64];
  int   status[16]; 
} TNetstat;

int GetDf2(TDF *_df);
int GetDf(TDF *_df, const char* _pchDir);
int GetCpuIdle(TCpuUsage *_cpu_usage);
int GetMem(TMemory *_mem_data);
int GetLoadAvg(TLoadAvg *_loadavg);

int savesysinfo(TCpuUsage *_pCpuUsage);
int loadsysinfo(TCpuUsage *s);


const unsigned long long GetKcps();
const int GetKcps2(int _nKey, char *_pchCps);
int GetDNServerVersion(const char *_pDNServer, char *_pVersion);
int GetDownloadConnCnt(const int _nPort);
int GetDSMgrConnCnt(const int _nPort);
int GetFtpCount();

int OpenLogFile(char *_pchFileName);
int CloseLogFile();
void Log(const char* const message, ...);

//int GetDownloadUserCnt();
int CheckDNServer(const int _nPort);
int GetFtpTraffic(char *_pchDate, unsigned long long *_total_cps);
//int GetFtpTraffic(int *_hour, unsigned long long *_total_cps);
