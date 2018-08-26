#ifndef __PACKET_H
#define __PACKET_H

#pragma once

#include "./Include.h"

const int MAXPDUSIZE      = 4096; /* 4KB */
const int PDUHEADERSIZE     = 4;
const int PDUBODYSIZE     = (MAXPDUSIZE-PDUHEADERSIZE);

// command length
const unsigned int COMMAND_SIZE = 2;
const unsigned int CLIENT_DATA_SIZE = (MAXPDUSIZE - PDUHEADERSIZE);

/*----------------------------------------------------------------------------
 *  Download Server Manager(DSM) <------> Download Server(DS)
 *  just push
 *  --------------------------------------------------------------------------*/
/*
   const unsigned short cmd_INFORM_FILE                    = 1800;
   const unsigned short cmd_DOWNLOAD_SIZE                  = 1801;
 */

const unsigned short cmd_HELLO_DS_DSM                   = 1802;
const unsigned short cmd_HELLO_DSM_DS                   = 1803;

const unsigned short cmd_USER_CLOSE_DS_DSM              = 1804;
//const unsigned short cmd_USER_CLOSE_DSM_DC              = 1805;

/*----------------------------------------------------------------------------
 *  BBS <------> Download Server Manager(DSM)
 *  --------------------------------------------------------------------------*/
const unsigned short cmd_BBS_DS_DOWNFINISH_REQ      = 1612;
const unsigned short cmd_BBS_DS_DOWNFINISH_RES      = 1613;

/*----------------------------------------------------------------------------
 *  Download Client(DC) <------> Download Server Manager(DSM) CMD 1600~1699
 *  --------------------------------------------------------------------------*/
const unsigned short cmd_HELLO_DC_DSM                   = 1600;
const unsigned short cmd_HELLO_DSM_DC                   = 1601;
const unsigned short cmd_GOODBYE_DC_DSM                 = 1602;
const unsigned short cmd_GOODBYE_DSM_DC                 = 1603;
const unsigned short cmd_GET_DS_INFO_DC_DSM             = 1604;
const unsigned short cmd_GET_DS_INFO_DSM_DC             = 1605;
const unsigned short cmd_DS_NOT_EXIST                   = 1606;
const unsigned short cmd_GET_GRIDCDN_BILL_DC_DSM    = 1607;
const unsigned short cmd_GET_GRIDCDN_BILL_DSM_DC    = 1608;
const unsigned short cmd_BILLINFO_NOT_EXIST             = 1609;
const unsigned short cmd_HEARTBEAT_DC_DSM               = 1610;
const unsigned short cmd_HEARTBEAT_DSM_DC               = 1611;
const unsigned short cmd_IBAS_DS_DOWNFINISH_REQ     = 1612;
const unsigned short cmd_IBAS_DS_DOWNFINISH_RES     = 1613;

const unsigned short cmd_CHAT_DS_DSM                = 1620;
const unsigned short cmd_CHAT_DSM_DS                = 1621;

/*----------------------------------------------------------------------------
 *  Download Client(DC) <------> Download Server(DS) CMD 1700~1799
 *  --------------------------------------------------------------------------*/
const unsigned short cmd_HELLO_DC_DS                    = 1700;
const unsigned short cmd_HELLO_DS_DC                    = 1701;
const unsigned short cmd_GOODBYE_DC_DS                  = 1702;
const unsigned short cmd_GOODBYE_DS_DC                  = 1703;
const unsigned short cmd_GET_FILE_DC_DS                 = 1704;
const unsigned short cmd_GET_FILE_DS_DC                 = 1705;
const unsigned short cmd_DFILE_NOT_EXIST                = 1706;
const unsigned short cmd_DFILE_ERROR                    = 1707;
const unsigned short cmd_GET_FILE_GRIDCDN_DC_DS     = 1708;
const unsigned short cmd_GET_FILE_GRIDCDN_DS_DC     = 1709;
const unsigned short cmd_HEARTBEAT_DC_DS                = 1710;
const unsigned short cmd_HEARTBEAT_DS_DC                = 1711;
const unsigned short cmd_HELLO_COMCODE_DC_DS            = 1712;
const unsigned short cmd_GET_FSIZE_DC_DS                = 1713;
const unsigned short cmd_GET_FSIZE_DS_DC                = 1714;



//////////////////////////////////////////////////////////////////////////////////////
// Enswer
//////////////////////////////////////////////////////////////////////////////////////
const unsigned short cmd_CHECK_VIOLATION_DC_DS          = 1720;     //엔서즈 관련 추가
const unsigned short cmd_CHECK_VIOLATION_DS_DC          = 1721;  //엔서즈 관련 추가

// 파일이름 포함됨.
const unsigned short cmd_CHECK_VIOLATION_DC_DS2         = 1722;  //엔서즈 관련 추가
const unsigned short cmd_CHECK_VIOLATION_DS_DC2         = 1723;  //엔서즈 관련 추가


const unsigned int  MAX_ENSWER_COUNT = 256;

// added
// 헤더 4byte를 먼저 보내고 sendfile() 호출한다.
const unsigned short cmd_GET_FSIZE_DC_DS2                = 1721;
//const unsigned short cmd_GET_FILE_DC_DS2                = 1720;
//const unsigned short cmd_GET_FILE_DS_DC2                = 1721; // recevie 는 cmd_GET_FILE_DS_DC 그대로 사용한다.

const unsigned int FILE_SZ  = 98;
const unsigned int ID_SZ = 16;
const unsigned int DIR_SZ = 256;

#ifdef _CDN
#endif
//CDN
const short cmd_HELLO_CODES_DC_DS       = 1750; // cmd_HELLO_CODES_DC_DS의 응답은 cmd_HELLO_DS_DC 로 한다.

const short cmd_GET_FILE_EX_DC_DS       = 1751;
const short cmd_GET_FILE_EX_DS_DC       = 1752;

// cmd_HELLO_CODES_DC_DS
typedef struct
{
  uint32_t nComCode;
  uint32_t nSvcCode;
}Tcmd_HELLO_CODES_DC_DS;

// cmd_GET_FILE_EX_DC_DS
typedef struct
{
  union
  {
    struct
    {
      unsigned short  iCmd;
      char            pchFileName[FILE_SZ];
      uint64_t        nOffset;
      uint64_t        nDownSize;
      char            pchID[ID_SZ];
      uint32_t        nBillNo;
      char            sDir[DIR_SZ];
      uint32_t        nDFlag; // 1: patch or full, 2: each file
    } d;
    unsigned char m_aBuf[136+DIR_SZ+4];
  } _Data;
} Tcmd_GET_FILE_EX_DC_DS;

#pragma pack(push, 2)
typedef struct
{
  unsigned short    command;
  unsigned short    length;
}PACKET_HEADER;

typedef struct
{
  PACKET_HEADER   header;
  char            data[PDUBODYSIZE];
}T_PACKET;

/*************************************************************
 * DS ---> DSM
 *************************************************************/
// hello DS -> DSM
typedef struct
{
  uint32_t iPid;  // chat server pid
  char message[1000];
}Tcmd_CHAT_DS_DSM;

// hello DS -> DSM
typedef struct
{
  uint32_t iPid;
}Tcmd_HELLO_DS_DSM;

// hello DSM -> DS
typedef struct
{
  uint32_t iSeq;
  uint32_t iPid;
  uint32_t iMaxUser;
  uint32_t iShmKey;
  uint32_t iShmDSStatus;
  double   dHelloTime;
}Tcmd_HELLO_DSM_DS;

// hello DS -> DSM
typedef struct
{
  uint32_t nComCode;
  uint32_t nBillNo;
  uint64_t nDownSize;
  double   dClosedTime;
}Tcmd_USER_CLOSE_DS_DSM;

/*************************************************************
 * BBS <---> DSM
 *************************************************************/
typedef struct
{
  uint32_t nComCode;
  uint32_t nBillNo;
} Tcmd_BBS_DS_DOWNFINISH_REQ;

typedef struct
{
  uint32_t nRet;
  uint32_t nComCode;
  uint32_t nBillNo;
  uint64_t nDownSize;
} Tcmd_BBS_DS_DOWNFINISH_RES;


/*************************************************************
 * DC <---> DSM
 *************************************************************/
// DSM -> DC (get ds info)
typedef struct
{
  unsigned short iCmd;
  unsigned short iPort;
  unsigned int   iIP;
} Tcmd_GET_DS_INFO_DSM_DC;


/*************************************************************
 * DC <---> DS
 *************************************************************/
// DC -> DS
typedef struct
{
  unsigned int iComCode;
} Tcmd_HELLO_DC_DS;

// DC <-> DS  (Get File size)
// 그대로 채워서 return 한다.
typedef struct
{
  unsigned short  iCmd;     // ack 값이다.
  // cmd_DFILE_NOT_EXIST     = 1706;
  // const short cmd_DFILE_ERROR      = 1707;
  //char            sFileName[DEF_FILENAME_SIZE];
  char            pchFileName[98];
  uint64_t    nFSize;
  char      pchID[16];
}Tcmd_GET_FSIZE_DC_DS;

// DC -> DS (Download file info)
typedef struct
{
  unsigned short  iCmd; // 에러나면 에러코드, 아니면 main command
  char      pchFileName[98];
  uint64_t    nOffset;
  uint64_t    nDownSize;
  char      pchID[16];
  uint32_t    nBillNo;
} Tcmd_GET_FILE_DC_DS;

typedef struct
{

} Tcmd_GET_FILE_DS_DC;

typedef struct
{
} Tcmd_HEARTBEAT_DC_DS;

typedef struct
{
} Tcmd_HEARTBEAT_DS_DC;

typedef struct
{
} Tcmd_GOODBYE_DC_DS;

/*
   typedef struct
   {
   unsigned long nComCode;
   char pchID[16];
   char pchFileName[98];
   }Tcmd_CHECK_VIOLATION_DC_DS;
//압축파일일 경우 각각의 내부파일들을 따로 검사한다고 생각하고
//여러개의 응답을 한꺼번에 받을 수 있도록 만든 프로토콜.
//현재 nCount는 엔서즈 체크가 성공했을 경우 1로 설정되고 실패할 경우 0으로 설정됨
//nRetCode와 nID의 값은 [0]에만 값이 들어오도록 되어있다.
typedef struct
{
int nRet;   //앤서즈 체크가 실패할 경우 에러코드(API_define.h 및 EnswerAPI문서 참조, 실패시 -값, 성공시 0)
int nCount;   //nRet이 0 일경우 반드시 1
int nRetCode[MAX_ENSWER_COUNT]; //체크 결과 코드(API_define.h 및 EnswerAPI문서 참조)
int nID[MAX_ENSWER_COUNT];  //엔서즈 ID(엔서즈 내부적으로 컨텐츠를 구분하는 고유값. EnswerAPI문서 참조)
}Tcmd_CHECK_VIOLATION_DS_DC;
 */

typedef struct
{
  unsigned long nComCode;
  char pchID[16];
  char pchHashName[98];
  char pchFileName[256];
}Tcmd_CHECK_VIOLATION_DC_DS2;

//압축파일일 경우 각각의 내부파일들을 따로 검사한다고 생각하고
//여러개의 응답을 한꺼번에 받을 수 있도록 만든 프로토콜.
//현재 nCount는 엔서즈 체크가 성공했을 경우 1로 설정되고 실패할 경우 0으로 설정됨
//nRetCode와 nID의 값은 [0]에만 값이 들어오도록 되어있다.
typedef struct
{
  int nRet;       //앤서즈 체크가 실패할 경우 에러코드(API_define.h 및 EnswerAPI문서 참조, 실패시 -값, 성공시 0)
  int nCount;     //nRet이 0 일경우 반드시 1
  int nRetCode[MAX_ENSWER_COUNT]; //체크 결과 코드(API_define.h 및 EnswerAPI문서 참조)
  int nID[MAX_ENSWER_COUNT];  //엔서즈 ID(엔서즈 내부적으로 컨텐츠를 구분하는 고유값. EnswerAPI문서 참조)
}Tcmd_CHECK_VIOLATION_DS_DC2;

#pragma pack(pop)

#endif

