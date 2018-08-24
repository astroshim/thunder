#ifndef __PACKET_H
#define __PACKET_H

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

const unsigned short cmd_CHAT_DS_DSM                = 2100;
const unsigned short cmd_CHAT_DSM_DS                = 2101;

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

const unsigned short cmd_CHAT_DC_DS                = 2000;
const unsigned short cmd_CHAT_DS_DC                = 2001;

#pragma     pack(push, 1)
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
/*
// C->S로 GET_FSIZE 패킷이 올때 DNMgr로 관리 대상 파일정보를 보낸다.
typedef struct
{
  uint32_t nComCode;
  uint32_t nBandWidth;
  uint64_t nFileSize;
  char   pchID[16];
  char     pchFileName[98];
} Tcmd_INFORM_FILE;
// 주기적으로 DS가 DNMgr로 받은파일 size를 보낸다.
typedef struct
{
  uint32_t nComCode;
  uint32_t nBillNo;
  uint64_t nDownSize;
} Tcmd_DOWNLOAD_SIZE;
*/


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
  double   dHelloTime;
}Tcmd_HELLO_DSM_DS;

// close DS -> DSM
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
#pragma pack(pop)

#endif

