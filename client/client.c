#include "./Include.h"
#include "./Packet.h"

#include <openssl/rc4.h>
//#include <arpa/inet.h>

#define CRYPTO_MSG_SIZE     8192

//#define DOWN_SIZE     100*1024
#define DOWN_SIZE     10*1024*1024

char g_pchKey[33];

//extern int m_iLogFd;
extern int OpenLogFile(char *_pchFileName);
extern int CloseLogFile();
extern void Log(const char* const message, ...); 


const int Read(int fd, void* const _vPtr, const size_t _n)
{   
    size_t      nLeft;
    ssize_t     nRead;
    char        *pchPtr;
				    
    pchPtr = (char *)_vPtr;
    nLeft = _n;
						    
    while(nLeft > 0)
    {
		if( (nRead = read(fd, pchPtr, nLeft)) < 0 )
        {
            if(errno == EINTR)
            {
		        nRead = 0;
	        }
	        else 
	        {
		        return -1;
	        }
	    }
	    else if(nRead == 0)
	    {
	   		break;
        }

        nLeft   -= nRead;
        pchPtr  += nRead;
				    
    }
							    
    return (_n - nLeft);
}       



const int GetRcvBufSize(int _iFd)
{   
    int     iRcvBufSize;
    socklen_t   iSockOptSize = 0;
			    
    iSockOptSize = sizeof(iRcvBufSize);
    if(getsockopt(_iFd, SOL_SOCKET, SO_RCVBUF, &iRcvBufSize, (socklen_t *)&iSockOptSize) < 0)
    {   
        Log("Fail GetRcvBufSize errno=(%d)\n", errno);
        return 0;
    }

	Log("RcvBufSize =(%d)\n", iRcvBufSize);
					    
    return iRcvBufSize;
}

const int GetSndBufSize(int _iFd)
{   
    int     iRcvBufSize;
    socklen_t   iSockOptSize = 0;
			    
    iSockOptSize = sizeof(iRcvBufSize);
    if(getsockopt(_iFd, SOL_SOCKET, SO_SNDBUF, &iRcvBufSize, (socklen_t *)&iSockOptSize) < 0)
    {   
        Log("Fail GetRcvBufSize errno=(%d)\n", errno);
        return 0;
    }

	Log("SndBufSize =(%d)\n", iRcvBufSize);
    return iRcvBufSize;
}

int CheckLength( unsigned char* lpString,  unsigned int nTotal)
{
    for (int i = nTotal; i>0; i--)
    {
        if ( lpString[i-1] != 0x00 )
            return i;
    }

    return 0;
}

int Encrypt(unsigned char *pbKey, int dwKeySize, unsigned char *pbPlaintext, unsigned char *pbCipherText, int dwHowMuch)
{   
    int       dwRet = false;
    
    RC4_KEY rc4_key;
    RC4_set_key(&rc4_key, dwKeySize, pbKey);
    
    RC4(&rc4_key, dwHowMuch, (unsigned char*)pbPlaintext, (unsigned char*)pbCipherText);
    return dwRet;
}

int Decrypt(unsigned char *pbKey, int dwKeySize, unsigned char *pbCipherText, unsigned char *pbPlaintext, int dwHowMuch)
{   
	int dwRet;
    RC4_KEY rc4_key;

    RC4_set_key(&rc4_key, dwKeySize, pbKey);

    RC4(&rc4_key, dwHowMuch,
        (unsigned char*)pbCipherText, (unsigned char*)pbPlaintext);
    return dwRet;
}

int EncryptCharToUCHAR( const char* szOrigin, int dwHowMuch, unsigned char* lpszKey, int dwKeySize, unsigned char* pbCipherText)
{
    if ( dwHowMuch < 0 || dwHowMuch > CRYPTO_MSG_SIZE )
        dwHowMuch = CheckLength((unsigned char*)szOrigin, CRYPTO_MSG_SIZE);

/*
    ASSERT ( szOrigin != NULL && AfxIsValidAddress(szOrigin, dwHowMuch) );
    ASSERT ( pbCipherText != NULL && AfxIsValidAddress(pbCipherText, dwHowMuch) );
*/
	Log("EncryptCharTounsigned char dwHowMuch=(%d) \n", dwHowMuch);


    int dwErr = Encrypt(lpszKey, dwKeySize, (unsigned char*)szOrigin, pbCipherText, dwHowMuch);
    int dwRet = 0;

    if ( 0 == dwErr )
        dwRet = dwHowMuch;

    return dwRet;
}

int DecryptUCHARToChar( unsigned char* pbCipherText, int dwHowMuch,  unsigned char* lpszKey, int dwKeySize, char* szOrigin)
{
    if ( dwHowMuch < 0 || dwHowMuch > CRYPTO_MSG_SIZE )
        dwHowMuch = CheckLength(pbCipherText, CRYPTO_MSG_SIZE);

/*
    ASSERT ( szOrigin != NULL && AfxIsValidAddress(szOrigin, dwHowMuch) );
    ASSERT ( pbCipherText != NULL && AfxIsValidAddress(pbCipherText, dwHowMuch) );
*/
    int dwErr = Decrypt(lpszKey, dwKeySize, pbCipherText, (unsigned char*)szOrigin, dwHowMuch);
    int dwRet = 0;

    if ( 0 == dwErr )
    {
        //memcpy(szOrigin, m_szDecryptText, dwHowMuch);
        dwRet = dwHowMuch;
    }

    return dwRet;
}

int SendHelloToDSM(int _fd)
{   
    T_PACKET sndpacket;
    sndpacket.header.command    = cmd_HELLO_DC_DSM;
    sndpacket.header.length     = 0;
    
Log("cmd_HELLO_DC_DSM=(%d) \n", sndpacket.header.length);
    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}
    
void RecvHelloFromDSM(int _fd)
{
    T_PACKET rcvpacket;
    // ack를 받아보자.
    read(_fd, (char *)&rcvpacket, PDUHEADERSIZE);
Log("RecvHelloFromDSM OK \n");
}

int SendGetDSInfo(int _fd)
{
    T_PACKET sndpacket;
    sndpacket.header.command    = cmd_GET_DS_INFO_DC_DSM;
    sndpacket.header.length     = 0;
    
Log("cmd_GET_DS_INFO_DC_DSM=(%d) \n", sndpacket.header.length);
    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}

int RecvGetDSInfo(int _fd)
{
    T_PACKET rcvpacket;
    Tcmd_GET_DS_INFO_DSM_DC *rcvbody = (Tcmd_GET_DS_INFO_DSM_DC *)&rcvpacket.data;;

    if(read(_fd, (char *)&rcvpacket, PDUHEADERSIZE+sizeof(Tcmd_GET_DS_INFO_DSM_DC)) <= 0)
    {
        Log("연결종료 \n");
        return 0;
    }

Log("cmd_GET_DS_INFO_DSM_DC ack=(%d), ip=(%d), port=(%d) \n",
                    rcvbody->iCmd, rcvbody->iIP, rcvbody->iPort);

    return rcvbody->iPort;
}

int SendGoodBye(int _fd)
{
    T_PACKET sndpacket;
    sndpacket.header.command    = cmd_GOODBYE_DC_DSM;
    sndpacket.header.length     = 0;
    
Log("cmd_GOODBYE_DC_DSM=(%d) \n", sndpacket.header.length);
    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}

int RecvGoodBye(int _fd)
{
    T_PACKET rcvpacket;
    // ack를 받아보자.
    read(_fd, (char *)&rcvpacket, PDUHEADERSIZE);
Log("DSM Hello ok \n");
}

int SendHello(int _fd, int _comcode)
{
    T_PACKET sndpacket;
    Tcmd_HELLO_DC_DS *sndbody = (Tcmd_HELLO_DC_DS *)&sndpacket.data;

//    sndpacket.header.command	= htons(CL_MCHS_HELLO_REQ);
    //sndpacket.header.command	= cmd_HELLO_DC_DS;
    sndpacket.header.command	= cmd_HELLO_COMCODE_DC_DS;
    sndpacket.header.length 	= sizeof(Tcmd_HELLO_DC_DS);
//    sndpacket.header.length 	= PDUHEADERSIZE + sizeof(TCL_MCHS_HELLO_REQ);

    // hello packet을 보낸다.
    sndbody->iComCode = _comcode;
    //sndbody->iComCode = 9999999;
Log("cmd_HELLO_DC_DS comcode=(%d) \n", sndbody->iComCode);

	int iLen = sndpacket.header.length;
//	sndpacket.header.length = htons(iLen);
	return write(_fd, (char *)&sndpacket, iLen+PDUHEADERSIZE);
}

void RecvHello(int _fd)
{
    T_PACKET rcvpacket;

	// ack를 받아보자.
	read(_fd, (char *)&rcvpacket, PDUHEADERSIZE);

	// 두번에 나눠 받아보자 epoll이 제대로 동작하는지...
	//read(_fd, (char *)&rcvpacket, PDUHEADERSIZE);
	//read(_fd, (char *)&rcvpacket + PDUHEADERSIZE, sizeof(TCL_MCHS_HELLO_RES));
	Log("In DS RecvHello ok ");

	//memcpy((char *)g_pchKey, (char *)rcvbody->pchEncryptKey, 33);
}

int SendGetFSize(int _fd, int _iClientNo, char *_pchFileName)
{
    T_PACKET sndpacket;
    Tcmd_GET_FSIZE_DC_DS *sndbody = (Tcmd_GET_FSIZE_DC_DS *)&sndpacket.data;
    
    sndpacket.header.command    = cmd_GET_FSIZE_DC_DS;
    sndpacket.header.length     = sizeof(Tcmd_GET_FSIZE_DC_DS);

    sndbody->iCmd = 1;
    //sndbody->nFSize = 734691923;
    sndbody->nFSize = 0;

    //strcpy(sndbody->pchFileName, "aaa.avi.0");
    strcpy(sndbody->pchFileName, _pchFileName);
	//sprintf(sndbody->pchFileName, "aaa.avi.%d", _iClientNo%2);

	//sprintf(sndbody->pchID, "a%dsunbee", _iClientNo%10);
	sprintf(sndbody->pchID, "a%dsunbee", _iClientNo%10);
	//strcpy(sndbody->pchID, "a0sunbee");

Log("Sendto GET_FSIZE_DC_DS =(%s), id=(%s) ", sndbody->pchFileName, sndbody->pchID);
    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}   

uint64_t RecvGetFSize(int _fd)
{
    T_PACKET rcvpacket;
    Tcmd_GET_FSIZE_DC_DS *rcvbody = (Tcmd_GET_FSIZE_DC_DS *)&rcvpacket.data;;   

    if(read(_fd, (char *)&rcvpacket, PDUHEADERSIZE+sizeof(Tcmd_GET_FSIZE_DC_DS)) <= 0)
    {
		Log("In  RecvGetFSize 연결종료.= (%d),(%s) \n", errno, strerror(errno));
        return 0;
    }

Log("GET_FSIZE_DS_DC file =(%s), id=(%s), fSize=(%llu) ", 
					rcvbody->pchFileName, rcvbody->pchID, rcvbody->nFSize);

	return rcvbody->nFSize;
}

int SendReqGetFile(int _fd, uint64_t _offset, int _iBillno, int _nDownSize, char *_pchFileName)
{
    T_PACKET sndpacket;
    Tcmd_GET_FILE_DC_DS *sndbody = (Tcmd_GET_FILE_DC_DS *)&sndpacket.data;

    sndpacket.header.command    = cmd_GET_FILE_DC_DS;
    sndpacket.header.length     = sizeof(Tcmd_GET_FILE_DC_DS);

    sndbody->iCmd = 1;
    //strcpy(sndbody->pchFileName, "aaa.avi.0");
    strcpy(sndbody->pchFileName, _pchFileName);
    sndbody->nOffset = _offset;
    sndbody->nDownSize = _nDownSize;
    sndbody->nBillNo = _iBillno;
    strcpy(sndbody->pchID, "a9sunbee");

Log("GET_FILE_DC_DS file=(%s), id=(%s) offset=(%llu), downsize=(%d)\n", 
			sndbody->pchFileName, sndbody->pchID, _offset, sndbody->nDownSize);
    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}   
    
int RecvReqGetFile(int _fd, int _iDownSize)
{
/*
    T_PACKET rcvpacket;
    Tcmd_GET_FILE_DS_DC *rcvbody = (Tcmd_GET_FILE_DS_DC *)&rcvpacket.data;;

    if(read(_fd, (char *)&rcvpacket, PDUHEADERSIZE+DOWN_SIZE) <= 0)
    {
        printf("연결종료 \n");
        return;
    }
*/
Log("In RecvReqGetFile downsize=(%d) \n", _iDownSize);


	int iRead;
	char rcvpacket[DOWN_SIZE+1];
	//memset(rcvpacket, 0x00, _iDownSize);
	memset(rcvpacket, 0x00, DOWN_SIZE+1);
    //if((iRead = read(_fd, (char *)rcvpacket, DOWN_SIZE)) <= 0)
    if((iRead = Read(_fd, (char *)rcvpacket, _iDownSize)) <= 0)
    {   
		Log("In RecvReqGetFile 연결종료.= (%d),(%s) \n", errno, strerror(errno));
        return 0;
    }

Log("GET_FILE_DS_DC recv length=(%d) \n", iRead);
	return iRead;
}


int GetBufSize(int fd)
{
	int     iRcvBufSize;
	socklen_t   iSockOptSize = 0;

	iSockOptSize = sizeof(iRcvBufSize);
	if(getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &iRcvBufSize, (socklen_t *)&iSockOptSize) < 0)
	{
		return 0;
	}
	Log("iRcvBufSize ==> (%d) \n", iRcvBufSize);
    return iRcvBufSize;
}

int Connect(char *_pchIP, int _iPort)
{
	int sockfd;
	struct sockaddr_in address;
	int len;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(_pchIP);
	address.sin_port = htons((u_short)_iPort);
	len = sizeof(address);

	if(connect(sockfd, (struct sockaddr *)&address, len) < 0) {
		Log("Connect Error= (%d),(%s)", errno, strerror(errno));
		exit(1);
	}
	else
		puts("socket create!!");

	return sockfd;
}


int main(int argc, char *argv[])
{
	int sockfd;
	int i;
	int iDSPort = 0;


/*
timespec 이란 구조체 는 tv_sec 와 tv_nsec 란 맴버 변수를 가지고 있네요.
clock_gettime()으로 시간을 가져오면,
그때 당시의 sec(초단위) 와 nsec(나노초) 가 timespec에 저장이 됩니다.
*/
	float time_dif;
	timespec start,end;
/*
// cipher test

    unsigned char pchCipherText[4109];
    char pchOriginalText[4109];
    int iEncSize = 0;

    memset(pchCipherText, 0x00, sizeof(pchCipherText));
    iEncSize = EncryptCharToUCHAR((char *)"abcd", strlen("abcd"), (unsigned char *)g_pchKey, 32, pchCipherText);

    memset(pchOriginalText, 0x00, sizeof(pchOriginalText));
    DecryptUCHARToChar(pchCipherText, iEncSize,  (unsigned char *)g_pchKey, 32, pchOriginalText);

printf("pchOriginalText == (%d)(%s) \n", iEncSize, pchOriginalText);
return 0;
*/

	int iClientNo = 0;
	char pchLogFile[128];

	char pchIP[24];
	char pchFileName[128];

	memset(pchLogFile, 0x00, sizeof(pchLogFile));
	memset(pchIP, 0x00, sizeof(pchIP));
	memset(pchFileName, 0x00, sizeof(pchFileName));

	strcpy(pchLogFile, "log/client_");
/*
	if(argc >= 2)
	{
		Log("client no ==> (%s) \n", argv[1]);
		iClientNo = atoi(argv[1]);
	}
*/

	if(argc < 4)
	{
		printf("usage : ./client [id] [file_name] [DSM ip] \n");
		return -1;
	}
	iClientNo = atoi(argv[1]);
	strcpy(pchFileName, argv[2]);
	strcpy(pchIP, argv[3]);

	/***********************************************
	// DC <-> DSM
	***********************************************/
	// Connect to DSM
	//sockfd = Connect("116.124.253.119", 50000); // glory
	//sockfd = Connect("125.7.131.243", 50000);	// 

/*
while(1)
{

printf("연결한다. \n");
*/


/*
if(sockfd < 0)
{
	printf("connect error! \n");
	continue;
}
	printf("connect success! \n");
*/

/*
i = 0;
while(1)
{
	if(i++ > 300)
		break;
	sockfd = Connect(pchIP, 50000);	//
	sleep(1);
}
pause();
*/


/*
SendHelloToDSM(sockfd);
RecvHelloFromDSM(sockfd);

close(sockfd);
sleep(1);
}
*/

	// log file open
	sprintf(pchLogFile, "%s_%d.log", pchLogFile, iClientNo);
	OpenLogFile(pchLogFile);


	/*
	GetSndBufSize(sockfd);
	GetRcvBufSize(sockfd);
	GetBufSize(sockfd);
	return 0;
	*/
dsjob:
	sockfd = Connect(pchIP, 50000);	//

	SendHelloToDSM(sockfd);
	RecvHelloFromDSM(sockfd);

	SendGetDSInfo(sockfd);		
	iDSPort = RecvGetDSInfo(sockfd);		

	if(iDSPort < 0)
	{
		exit(1);
	}
	SendGoodBye(sockfd);
	RecvGoodBye(sockfd);
	close(sockfd);

Log("DS에 접속한다....... port=(%d) \n", iDSPort);

//dsjob:
	/***********************************************
	// DC <-> DS
	***********************************************/
	// Connect to DS
	//sockfd = Connect("127.0.0.1", iDSPort);
	//sockfd = Connect("116.124.253.119", iDSPort);
	sockfd = Connect(pchIP, iDSPort);

	if(sockfd < 0)
	{
		Log("DS에 접속실패 ...... (%d) \n", iDSPort);
	}

	/*
	if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0)) < 0)
    {
        Log("Fail SetBlock errno=(%d)", errno);
        return -1;
    }
	*/

	SendHello(sockfd, iClientNo);
	RecvHello(sockfd);
/*
close(sockfd);
goto dsjob;
*/

iClientNo = 9;		// speed
	SendGetFSize(sockfd, iClientNo, pchFileName);
	uint64_t nFileSize = RecvGetFSize(sockfd);
	if(nFileSize <= 0)
	{
		Log("File not exist!= (%d),(%s)", errno, strerror(errno));
		exit(1);
	}

Log("파일 전송 시작 ");
	clock_gettime(CLOCK_REALTIME,&start);

	uint64_t nOffset = 0;
	// get file
//int cnt = 0;
	while(1)
	{
		int iRecv = 0, iDownSize;
		
		iDownSize = DOWN_SIZE;
		if((nFileSize - nOffset) < DOWN_SIZE)
		{
			iDownSize = nFileSize - nOffset;	
		}

		SendReqGetFile(sockfd, nOffset, iClientNo, iDownSize, pchFileName);

Log("요청한 다운 size=(%d), filesize=(%llu) ", iDownSize, nFileSize);

		if((iRecv = RecvReqGetFile(sockfd, iDownSize)) < 0)
		{
			break;
		}
		nOffset += iRecv;

Log("Transfer offset=(%llu) ", nOffset);

//printf("Transfer offset=(%llu) \n", nOffset);

		if(nOffset >= nFileSize)
		{
			break;
		}
//if(cnt++ >1)
//	break;

	}

	close(sockfd);
	//goto dsjob;

	clock_gettime(CLOCK_REALTIME,&end);
	time_dif = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) / 1000000000 );
Log("Transfer complete! time=(%.2f) \n", time_dif);


	CloseLogFile();
	return 0;
}


