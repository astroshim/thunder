#include "./Include.h"
#include <sys/shm.h>
#include <time.h>

extern int g_Call;

int savesysinfo(TCpuUsage *_pCpuUsage)
{
  	FILE *finfo;
  
  	if ((finfo = fopen(TMP_FILE, "wb")) == NULL) 
	{
    	perror("error loading " TMP_FILE);
    	return 1;
  	}
  	fwrite(_pCpuUsage, sizeof(TCpuUsage), 1, finfo);
  	fclose(finfo);
 	return 0;
}

int loadsysinfo(TCpuUsage *_pCpuUsage)
{
	FILE *finfo;

	if ((finfo = fopen(TMP_FILE, "rb")) == NULL) 
	{
		return 1;
	}

	if (fread(_pCpuUsage, 1, sizeof(TCpuUsage), finfo) != sizeof(TCpuUsage)) 
	{
		printf("tmpfile size mismatch\n");
		fclose(finfo);
		return 1;
	}

	fclose(finfo);
	return 0;
}


int GetDf3(TDF *_tDf)
{

}

int GetDf2(TDF *_tDf)
{
    FILE *fp=NULL;
    char pchLine[1024+1];
    int iFlag = 0;

	char pchMount[100];
	unsigned long long nKTotal, nKUsed, nKAvail;

    fp = popen("df -a", "r");
    if(fp == NULL)
    {
//printf("here 1\n");
        return -1;
    }

    while(1)
    {
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }
//fprintf(stdout, "~~~~~~~~~~>>In POPEN=> (%s)\n", pchLine);
/*
        if(strstr(pchLine, _pchkProgram))
        {
            iFlag = 1;
        }
*/
		if(strstr(pchLine, "none") || strstr(pchLine, "Available"))
		{
			continue;
		}

		nKTotal = nKUsed = nKAvail = 0;
		sscanf(pchLine, "%s %llu %llu %llu",
                           pchMount,
                           &nKTotal,
                           &nKUsed,
                           &nKAvail);

//printf("mount=(%s), total=(%llu), used=(%llu), avail=(%llu)\n", pchMount, nKTotal, nKUsed,nKAvail);

    }

    pclose(fp);
    return iFlag;
}


int GetDf(TDF *_tDf, const char* _pchDir)
{
	unsigned long long aaaa = 0;
//	int iDiskSize = 0, iAvail = 0;
    FILE *fp = NULL;
    struct mntent *mm = NULL;
    struct statfs *buf = NULL;

    //fs_open(2);
    fp = setmntent(MOUNTED, "r");
    while((mm = getmntent(fp)))
    {
//printf("mnt_fsname => (%s) dir=(%s)\n", mm->mnt_fsname, mm->mnt_dir);
        if(!strcmp(mm->mnt_fsname, "none") || (strcmp(mm->mnt_dir, _pchDir) != 0))
            continue;
        else
            break;
    }
    if(fp)
        endmntent(fp);
    //fs_close(2);
    
    if(!mm)
    {   
        printf("here 1 \n");
        return -1;
    }

    buf = (struct statfs *)malloc(sizeof(struct statfs) * 1);
    if(statfs(mm->mnt_dir, buf))
    {   
        free(buf);
        return -1;
    }
	free(buf);

/*
	_tDf->iDiskSize = ((unsigned long long)(buf->f_blocks) * (unsigned long long)(buf->f_bsize))/(1024*1024);
	_tDf->iAvail = ((unsigned long long)(buf->f_bavail) * (unsigned long long)(buf->f_bsize))/(1024*1024);
*/
	_tDf->iDiskSize = ((unsigned long long)(buf->f_blocks) * (unsigned long long)(buf->f_bsize))/(1024);
	_tDf->iAvail = ((unsigned long long)(buf->f_bavail) * (unsigned long long)(buf->f_bsize))/(1024);
	//aaaa = ((unsigned long long)(buf->f_blocks) * (unsigned long long)(buf->f_bsize));
	
/*
	printf("buf->f_bsize ==> (%d) \n", buf->f_bsize);
	printf("buf->f_blocks ==> (%d) \n", buf->f_blocks);
	printf("buf->f_bavail ==> (%d) \n", buf->f_bavail);
	printf("disk ==> (%lu) \n", _tDf->iDiskSize );
	printf("avail ==> (%lu) \n", _tDf->iAvail);
*/

    return 0;
}

int GetCpuIdle(TCpuUsage *_tCpuUsage)
{
    FILE *fp = NULL;
    char line[80];
    char cpuid[32];
/*
    int totaldiff;
    int diff[3];
*/
/*
	int iUserOld, iNiceOld, iSystemOld, iIdleOld;
	unsigned long long iUser, iNice, iSystem, iIdle;
	unsigned long long iSum;
*/
//    memset((void *)_tCpuUsage, 0x00, sizeof(TCpuUsage));

    // /proc/stat파일을 읽어서 jiffies값을 계산한다. 
    fp = fopen("/proc/stat", "r");
    if(fp == NULL)
    {
        printf("Cannot open stat\n");
        return -1;
    }
	_tCpuUsage->iCpuCnt == 0;
    while(1)
    {
        fgets(line,80, fp);
        if (strstr(line, "cpu") == NULL)
        {
            //fclose(fp);
            // cpu 갯수를 세팅하고 리턴한다. 
            //_tCpuUsage->countcpu = cpu_num -1;
			break;
        }
        else
        {
//			if(strstr(line, "cpu") != NULL && iCpuCnt > 0)
			if(_tCpuUsage->iCpuCnt == 0)
			{
				sscanf(line, "%s %llu %llu %llu %llu %llu %llu",
							   cpuid,
							   &_tCpuUsage->nUser,
							   &_tCpuUsage->nNice,
							   &_tCpuUsage->nSystem,
							   &_tCpuUsage->nIdle,
							   &_tCpuUsage->nIO,
							   &_tCpuUsage->nIRQ);
/*
							   &(_tCpuUsage->nUser[iCpuCnt]),
							   &nulldata,
							   &(_tCpuUsage->nSystem[iCpuCnt]),
							   &(_tCpuUsage->nIdle[iCpuCnt]));
*/
			}
           	_tCpuUsage->iCpuCnt ++;
        }
    }
    if(fp != NULL)
        fclose(fp);

	_tCpuUsage->ctime = time(0);
	_tCpuUsage->iCpuCnt--;
	return 0;
}

int GetMem(TMemory *_tMem)
{
    FILE *fp = NULL;
    int index = 0;
    char line[128];
//    TMemory lmem_data;
    int nulldata;
    char null[12];

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        printf("Meminfo open error \n");
        return -1;
    }
    
	fscanf(fp, "MemTotal: %u kB\n", &_tMem->mtotal);
	fscanf(fp, "MemFree:  %u kB\n", &_tMem->mfree);
	fscanf(fp, "Buffers:  %u kB\n", &_tMem->mbuffer);
	fscanf(fp, "Cached:   %u kB\n", &_tMem->mcached);

/*
    while(1)
    {
        fgets(line, 128, fp);
        if(index == 0)
        {
            index ++;
            continue;
        }
        else if(index == 1)
        {
            sscanf(line, "%s %lu %d %lu", null, &_tMem->real, &nulldata, &_tMem->free
);  
        }
        else if(index == 2)
        {
            sscanf(line, "%s %lu %lu %lu", null, &nulldata, &_tMem->swap_use,
                                        &_tMem->swap_free);
        }
        else
        {
            fclose(fp);
            return 0;
        }
        index ++;
    }
*/
    if(fp != NULL)
		fclose(fp);

	return 0;
}

int GetLoadAvg(TLoadAvg *_tLoadAvg)
{           
    FILE *fp = NULL;
                           
    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)        
    {                      
        printf("TLoadAvg open error \n");
        return -1;
    }
    
  	fscanf(fp, "%f %f %f", &_tLoadAvg->avg1, &_tLoadAvg->avg5, &_tLoadAvg->avg15);
    if(fp != NULL)
		fclose(fp);

	return 0;
}

int GetDNServerVersion(const char *_pDNServer, char *_pVersion)
{
    FILE *fp=NULL;
    char pchLine[64+1];
    //int iVersion = 0;

	char *pS, *pE;

/*
#ifdef _VHARD
    fp = popen("/root/DownloadServer_vhard/VhardDN -v", "r");
#elif _JJANGFILE
    fp = popen("/home/wizmaster/DownloadServer/DNServer -v", "r");
#else
    fp = popen("/root/DownloadServer/DNServer -v", "r");
#endif
*/
    memset(pchLine, 0x00, sizeof(pchLine));
    sprintf(pchLine, "%s -v", _pDNServer);
    fp = popen(pchLine, "r");

    if(fp == NULL)
    {
        return -1;
    }

    while(1)
    {
    	memset(pchLine, 0x00, sizeof(pchLine));
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		pS = strchr(pchLine, '[');
		if(pS != NULL)
		{
			pE = strchr(pS, ']');
			strncpy(_pVersion, pS+1, (pE-pS)-1);
		}
		else
		{
			strncpy(_pVersion, pchLine, strlen(pchLine)-1);
		}
//printf("GetDNServerVersion => (%s) \n", _pVersion);
		//iVersion = atoi(pchLine);
		break;
    }
    pclose(fp);
    //return iVersion;
	return 0;
}

int GetFtpCount()
{
    FILE *fp=NULL;
    char pchLine[1024+1];
    int iConnCnt = 0;

    fp = popen("ps -ef | grep ftp | wc -l", "r");
    if(fp == NULL)
    {
        return -1;
    }

    while(1)
    {
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		iConnCnt = atoi(pchLine);
		break;
    }
    pclose(fp);
    return iConnCnt;
}

int GetDSMgrConnCnt(const int _nPort)
{
    FILE *fp=NULL;
    char pchLine[1024+1];
    int iConnCnt = 0;

    memset(pchLine, 0x00, sizeof(pchLine));
    sprintf(pchLine, "netstat -an | grep %d |grep -v 127.0.0.1|grep ESTABLISHED|wc -l", _nPort);
    fp = popen(pchLine, "r");

    if(fp == NULL)
    {
        return -1;
    }

    while(1)
    {
    	memset(pchLine, 0x00, sizeof(pchLine));
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		iConnCnt = atoi(pchLine);
		break;
    }
    pclose(fp);
    return iConnCnt;
}

/*
my ($sec, $min, $hour, $mday, $mon, $year, $wday) = gmtime($time);
my @days = ("Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun");
my @months = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");
$year += 1900;
$wday = $days[$wday];
$mon  = $months[$mon];
*/

char *pchDays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char *pchMonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//int GetFtpTraffic(int *_hour, unsigned long long *_total_cps)
int GetFtpTraffic(char *_pchDate, unsigned long long *_total_cps)
{
    time_t tval;
    struct tm t;
	char pchDate[32];

    time(&tval);
    localtime_r(&tval, &t);

/*
	if(t.tm_min%10 != 0)
	{
		return 0;
	}
*/

	memset(pchDate, 0x00, sizeof(pchDate));

	sprintf(_pchDate, "%02d:%02d", t.tm_hour, t.tm_min);

	//*_hour = t.tm_hour;
	//sprintf(pchDate, "%s %s %02d %02d:%02d",  pchDays[t.tm_mday%6], pchMonths[t.tm_mon], t.tm_mday, *_hour, t.tm_min);
	sprintf(pchDate, "%s %s %02d %s",  pchDays[t.tm_mday%6], pchMonths[t.tm_mon], t.tm_mday, _pchDate);

    FILE *fp=NULL;
    char pchLine[1024+1];

	memset(pchLine, 0x00, sizeof(pchLine));
	sprintf(pchLine, "grep \"%s\" /var/log/xferlog", pchDate);

//printf("command = (%s) \n", pchLine);
    fp = popen(pchLine, "r");
    if(fp == NULL)
    {
        return -1;
    }

	//unsigned long long total_trans = 0;
    while(1)
    {
		memset(pchLine, 0x00, sizeof(pchLine));
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		char null[24];
		char ip[24];
		memset(ip, 0x00, sizeof(ip));
		unsigned long long trans_amount = 0;
        sscanf(pchLine, "%s %s %s %s %s %s %s %llu",
                null,
                null,
                null,
                null,
                null,
                null,
				ip,
				&trans_amount);

		*_total_cps += trans_amount;
//printf("ip=(%s), trans=(%llu)\n", ip, trans_amount);
    }

printf("total amount = (%llu)\n", *_total_cps);
    pclose(fp);
	return 0;
}

int GetDownloadConnCnt(const int _nPort)
{
    FILE *fp=NULL;
    char pchLine[1024+1];
    int iConnCnt = 0;

	memset(pchLine, 0x00, sizeof(pchLine));
	sprintf(pchLine, "netstat -an | grep %d |grep ESTABLISHED | wc -l", _nPort);
    fp = popen(pchLine, "r");

    if(fp == NULL)
    {
        return -1;
    }

    while(1)
    {
		memset(pchLine, 0x00, sizeof(pchLine));
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		iConnCnt = atoi(pchLine);
		break;
    }
    pclose(fp);
    return iConnCnt;
}

//int GetDownloadUser(TNetstat *_tNetstat)
int CheckDNServer(const int _nPort)
{
    FILE *fp=NULL;
    char pchLine[1024+1];
	int iRetVal = 0;

    memset(pchLine, 0x00, sizeof(pchLine));
    sprintf(pchLine, "netstat -an | grep %d |grep LISTEN", _nPort);
    fp = popen(pchLine, "r");
    if(fp == NULL)
    {
        return iRetVal;
    }

    while(1)
    {
		memset(pchLine, 0x00, sizeof(pchLine));
        if(fgets(pchLine, 1024, fp) == NULL)
        {
            break;
        }

		iRetVal = 1;
		break;
    }
    pclose(fp);
    return iRetVal;
}

const unsigned int MAX_COMPANY = 100;
struct TStatistics {
    unsigned int comcode;
    unsigned long long kcps;
};    
const int GetKcps2(int _nKey, char *_pchCps)
{
	struct TStatistics *ptData;
	int iShmId = 0;
	int i;

    if((iShmId = shmget ((key_t)_nKey, sizeof(struct TStatistics), IPC_CREAT | 0666)) < 0)
    {
    	perror("SharedMemory error");
		return -1;
    }

	if((ptData = (struct TStatistics *)shmat (iShmId, 0, 0)) < 0)
	{
	    perror("SharedMemory attach ");
	    return -1;
    }   

	sprintf(_pchCps, "kcps=%llu&kcps2=", ptData[0].kcps);
	for(i = 1; i < MAX_COMPANY; i++)
	{
		if(ptData[i].comcode == 0)
		{
			break;
		}
		sprintf(_pchCps, "%s%d,%llu|", _pchCps, ptData[i].comcode, ptData[i].kcps);
	}
	shmdt(ptData);

	return 0;
}

