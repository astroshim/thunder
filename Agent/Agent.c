#include "./Include.h"
/*
#include <stropts.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
*/
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <time.h>

int		connect_tcp_socket(char *, int);
int		timed_send(int, const char *, int, struct timeval *);

int g_Call;
// main �Լ� ���������� global ������ �Űܿ�..why? sysinfo���� ����Ϸ���
int  giComCode = 0;

/*
// use curl
#include <curl/curl.h>
int postdata(char *buf)
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
		curl_easy_setopt(curl, CURLOPT_URL, "http://gm.gridcdn.com/remote/getAgentInfo.php");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	} else {
		return -1;
	}
	return 0;
}
*/

//void GetIP(unsigned long *_piIp)
int GetIP(char *_pchIp, char *_pchMac)
{
    int sock, ret;
    struct ifreq ifr;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
		Log("In GetIP socket() error ! ");
        return -1;
    }
    strcpy(ifr.ifr_name, "eth0");
    // Get IP Adress
    ret = ioctl (sock, SIOCGIFADDR, &ifr);
    if (ret < 0)
    {
	    strcpy(ifr.ifr_name, "eth1");
    	ret = ioctl (sock, SIOCGIFADDR, &ifr);
		if(ret < 0)
		{
        	close(sock);
			Log("In GetIP ioctl() error! (%d)(%s)", errno, strerror(errno));
        	return -1;
		}
    }
	//strcpy(_pchIp, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr));
	strcpy(_pchIp, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));


	if(_pchMac != NULL)
	{
		// mac address
		ret = ioctl (sock, SIOCGIFHWADDR, &ifr);
		if (ret < 0)
		{
			close(sock);
			return 0;
		}
		struct sockaddr *sa = &(ifr.ifr_hwaddr);;
/*
		//memcpy((void *)_pchMac, (void *)&(sa->sa_data), 6);

		struct ether_addr haddr;
		memcpy((void *)&haddr, (void *)_piMac, 6);

*/
		strcpy(_pchMac, (char *)ether_ntoa ((struct ether_addr *)&(sa->sa_data)));
	}

    close(sock);

//    *_piIp = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
	return 0;
}

int timed_send(int sock, const char *buf, int size, struct timeval *timeout)
{
	fd_set set;
	int nset, cnt;
	int send_size;

	FD_ZERO(&set);
		nset = sock + 1;
	FD_ZERO(&set);
	FD_SET(sock, &set);


	if ((cnt = select(nset, 0, &set, 0, timeout)) == 0) return(-1);
	if (cnt < 0) {
		return(-1);
	}
	if (FD_ISSET(sock, &set))  {
		send_size = send(sock, buf, size, 0);
		return(send_size);
	}
	return(-1);
}

int timed_recv(int sock, char *buf, int size, struct timeval *timeout)
{
    fd_set set;
    int     iSet, iCnt;
    int     iRecvSize;

    if(sock < 0)
    {
        return -1;
    }

    iSet = sock + 1;
    FD_ZERO(&set);
    FD_SET(sock, &set);

    if( (iCnt = select(iSet, &set, 0, 0, timeout)) == 0)
    {
        return -1;
    }

    if( iCnt < 0)
    {
        return -1;
    }

/*
    ioctl( sock, FIONREAD, &_iSize );
    if( _iSize == 0 )   // ���� ����� ioctl�� nread�� 0�� �����մϴ�.
    {
        return -1;
    }
//printf("Socket buffer�� �ִ� data �� => [%d] \n", _iSize);
*/
    if(FD_ISSET(sock, &set))
    {
        iRecvSize = recv(sock, buf, size, 0);
        return iRecvSize;
    }

    return -1;
}

int connect_tcp_socket(char *addr, int port)
{
    int sockfd;
    struct sockaddr_in sin;
    /*
    ** Open a TCP socket (Internet stream socket)
    */
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
            Log("Can't get AF_INET sockfd, reason=%s", strerror(errno));
            return(-1);
    }
    /*
    ** Connect to the server
    */
    bzero((void*)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(addr);
    sin.sin_port = htons((u_short)port);

    if ( connect(sockfd, (struct sockaddr*)&sin, sizeof sin) < 0 ) {
            Log("Can't connect to %s port %d, reason=%s", addr, port,
                    strerror(errno));
            close(sockfd);
            return(-1);
    }

    Log("Connected to %s port %d", addr, port);
    return(sockfd);
}

const int CallWeb(const int _iComCode
					,const int nDNPort
					,const int nShmId
					,const char *pchDN
					,const char *_pchIP
					,const char *_pchMac
					,const unsigned long _nMemory
					,const float _fLoadAvg
					,const unsigned long long _iCpu
					,const unsigned long long _nDisk)
{
	int		sock;
	int		port;
	int		ret;
	char	send_data[1024*10];
	char	recv_data[1024*10];
	char	pchVersion[65];
	int		ResultCode;

	struct hostent * host_info;
	struct in_addr inAddr;
	struct timeval timeout;

	int iDNCnt = 0;

	timeout.tv_sec = (long)10;
	timeout.tv_usec = 0;

	host_info = gethostbyname("gm.gridcdn.com");
	port = 80;

	if( host_info != NULL )
		inAddr = *(struct in_addr *)host_info->h_addr;
	else	/*	get ip fail */
		return -1;

	memset(pchVersion, 0x00, sizeof(pchVersion));
	GetDNServerVersion(pchDN, pchVersion);

	//int hour = 0;
	//GetFtpTraffic(&hour, &ftp_cps);
	unsigned long long ftp_cps = 0;
	char pchDate[16];
	memset(pchDate, 0x00, sizeof(pchDate));
	if(g_Call == 1)
	{
		GetFtpTraffic(pchDate, &ftp_cps);
	}

	char pchCps[1024];
	memset(pchCps, 0x00, sizeof(pchCps));
	GetKcps2(nShmId, pchCps);

	iDNCnt = GetDownloadConnCnt(nDNPort);
	memset(send_data, 0x00, sizeof(send_data));
	sprintf(send_data,   "GET /remote/getAgentInfo.php?comcode=%d&version=%s&ip=%s&mac=%s&mem=%lu&load=%.2f&cpu=%llu&space=%llu&dm_client=%d&dn_client=%d&ftp_client=%d&%s&alive=%d&hour=%s&ftpcps=%llu HTTP/1.1\r\nAccept: */*\r\nConnection: Keep-Alive\r\nHost: gm.gridcdn.com\r\n\r\n",
							_iComCode,
							pchVersion,
							_pchIP,
							_pchMac,
							_nMemory,
							_fLoadAvg,
							_iCpu,
							_nDisk,
							GetDSMgrConnCnt(50000),
							iDNCnt,
							GetFtpCount(),
							pchCps,
							CheckDNServer(nDNPort),
							pchDate,
							ftp_cps);

	Log("Trying to connect to '%s' port=%d data=(%s)", inet_ntoa(inAddr), port, send_data);

	if ( (sock = connect_tcp_socket( inet_ntoa(inAddr), port)) == -1 ) {
		Log("In CallWeb, Connection NOT established");
		return -1;
	}

	ret = timed_send(sock, send_data, strlen(send_data), &timeout);
	if( ret <= 0 )
	{
		Log("In CallWeb, write ret = '%d', reason='%s'", ret, strerror(errno));
		close(sock);
		return -1;
	}

	ret = timed_recv(sock, recv_data, sizeof(recv_data), &timeout);
	if( ret <= 0 ) {
		Log("In CallWeb, read ret = '%d', reason='%s'", ret, strerror(errno));
		close(sock);
		return -1;
	}
	close(sock);
Log("ret => (%s)", recv_data);

/*
	if( (ResultCode = GetResultCode(recv_data)) == 0 )
		return 1;
	else if( ResultCode == 9999 )
		return 0;
	else
		return -1;
*/
	return 0;
}

void print_usage(char *argv0)
{
    printf("\n");
    printf("%s Build Date Time =[%s %s]\n", argv0, __DATE__, __TIME__);
    printf("\n");
    printf("USAGE : %s [comcode] [volumne] [ServerPort] [Statistic Number] [DownServer Full Name]\n", argv0);
    printf("[comcode] : grid_id \n");
    printf("[volume]  : VOLNAME in DS.properties\n");
    printf("[ServerPort]  : DNServer service port\n");
    printf("[Statistic Number]  : SHMKEY in DS.properties\n");
    printf("[DownServer Full Name] : Path where Download Server installed\n");
    printf("\n");

}

int main(int argc, char *argv[])
{
/*
	struct sysinfo cs;
	if (getsysinfo(&cs) == 0)
	{
		printf("memused=%d, TLoadAvg=%d, cpu=%d, freespace=%d \n", cs.muse, cs.avg, cs.cpu, cs.freespace);
	}
*/

/*
	unsigned long long nUser1, nSystem1, nIdle1, nSum1;
	unsigned long long nUser2, nSystem2, nIdle2, nSum2;
*/
	char send_data[1024*2];
	char pchMyIP[20], pchMac[24];
    char pchVolName[64];
    char pchLogName[PATH_MAX + 1];

	int nDNPort, nShmId;
    char pchDN[128];

	TLoadAvg tLoadAvg;
	TMemory tMem;
	TDF tDf;
	TCpuUsage tCpuUsage1, tCpuUsage2;
	unsigned long long nCpuUsage, nSum;
	int iResult = 0;
/*
unsigned long long ftpcps = 0;
int hour = 0;
GetFtpTraffic(&hour, &ftpcps);
return 0;
*/

	memset(&tLoadAvg, 0, sizeof(TLoadAvg));
	memset(&tMem, 0, sizeof(TMemory));
	memset(&tDf, 0, sizeof(TDF));
	memset(&tCpuUsage1, 0, sizeof(TCpuUsage));
	memset(&tCpuUsage2, 0, sizeof(TCpuUsage));
	nCpuUsage= nSum = 0;
	memset(&pchVolName, 0, sizeof(pchVolName));
	memset(&pchLogName, 0, sizeof(pchLogName));
	memset(pchDN, 0, sizeof(pchDN));


    if(argc != 6)
    {
        print_usage(argv[0]);
        return -1;
    }
    else
    {
        giComCode = atoi(argv[1]);
        snprintf(pchVolName, sizeof(pchVolName), "%s", argv[2]);
        nDNPort = atoi(argv[3]);
        nShmId = atoi(argv[4]);
        strcpy(pchDN, argv[5]);
    }

    // log file open
    sprintf(pchLogName, "./agent_%d.log", giComCode);
    OpenLogFile(pchLogName);

    time_t tval;
    struct tm t;
    time(&tval);
    localtime_r(&tval, &t);
	g_Call = 0;

    if(t.tm_min%10 == 0)
    {
		g_Call = 1;
    }
	Log("time : %02d:%02d g_Call=(%d)", t.tm_hour, t.tm_min, g_Call);

	// ip
	memset(pchMyIP, 0x00, sizeof(pchMyIP));
	memset(pchMac, 0x00, sizeof(pchMac));
	if(GetIP(pchMyIP, pchMac) < 0)
	{
		Log("GetIP Error ! ");
		CloseLogFile();
		return 0;
	}
	Log("MyIP =(%s) Mac=(%s) ", pchMyIP, pchMac);

    GetDf(&tDf, pchVolName);

	Log("TDF disk=(%llu) KB, avail=(%llu) KB", tDf.iDiskSize, tDf.iAvail);

	// memory
	GetMem(&tMem);
	//printf("mem mtotal=(%lu), mfree=(%lu), mbuffer=(%lu), mcached=(%lu)\n", tMem.mtotal, tMem.mfree, tMem.mbuffer, tMem.mcached);
	Log("Memory Use=(%lu) KB", tMem.mtotal- tMem.mfree- tMem.mbuffer- tMem.mcached);

	// load
	GetLoadAvg(&tLoadAvg);
	//Log("load avg =(%.2f)", (float)(tLoadAvg.avg1 * 100.0));
	Log("load avg =(%.2f)", (float)(tLoadAvg.avg1));

    //GetCpuIdle(&tCpuUsage1);
	// cpu
	// ���� �ð��� cpu ������ �о�´�.
    GetCpuIdle(&tCpuUsage1);

	// ������ �����ߴ� ������ �о�´�.
	iResult = loadsysinfo(&tCpuUsage2);
	// �о�� cpu������ �����Ѵ�.
	if(savesysinfo(&tCpuUsage1) != 0)
	{
		Log("CPU ���� ���� ���� ");
		nCpuUsage = 10000;
	}

	if(iResult != 0)
	{
		Log("����� .tmp ������ ����. ");
		nCpuUsage = 10000;
	}

	int tdiff = tCpuUsage1.ctime - tCpuUsage2.ctime;
	if(tdiff > 120 || tdiff < 0)
	{
		Log("time over ");
		nCpuUsage = 10000;
	}

	if(nCpuUsage >= 10000)
	{
		nCpuUsage = 0;
	}
	else
	{
		nSum = (tCpuUsage1.nUser-tCpuUsage2.nUser) +
				(tCpuUsage1.nSystem-tCpuUsage2.nSystem) +
				(tCpuUsage1.nIdle-tCpuUsage2.nIdle) ;
		if(nSum <= 0)
		{
			nCpuUsage = 0;
		}
		else
		{
			nCpuUsage = 10000 - (tCpuUsage1.nIdle-tCpuUsage2.nIdle)*10000/nSum;
		}
	}

    Log("1idle=(%llu), 2idle=(%llu), nSum=(%llu), cpu usage =(%d)%%", tCpuUsage1.nIdle,tCpuUsage2.nIdle, nSum, nCpuUsage);

	CallWeb(giComCode
			,nDNPort
			,nShmId
			,pchDN
			,pchMyIP
			,pchMac
			,tMem.mtotal- tMem.mfree- tMem.mbuffer- tMem.mcached
			,(float)(tLoadAvg.avg1)
			,nCpuUsage
			,tDf.iAvail);
	CloseLogFile();
	return 0;
}

