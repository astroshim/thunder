#include "../include/SharedMemory.h"

int main(int argc, char* argv[])
{
	SharedMemory sm((key_t)1828, 1024*sizeof(struct scoreboard_file));
   	if(!sm.IsStarted())
    {
        printf("SharedMemory 생성 실패1 \n");
		return -1;
	}
	struct scoreboard_file *stInfo;
	stInfo = (struct scoreboard_file *)sm.GetDataPoint();
    if(stInfo == NULL)
    {
        printf("Shm is NULL \n");
        return -1;
    }
	int iCnt = atoi(argv[2]);
	int iSessionCnt = 0;
	uint64_t iKcps = 0;
	
	for(int i= 0; i < iCnt; i++)
	{
		if(stInfo[i].cUse == ON)
		{
/*
			printf("stInfo[%d].cUse 	= (%d)\n", i, stInfo[i].cUse);
			printf("stInfo[%d].comcode	= (%d)\n", i, stInfo[i].comcode);
*/
			printf("===============================================================\n");
			printf("stInfo[%d].billno 	= (%d)\n", i, stInfo[i].billno);
			printf("stInfo[%d].kcps 	= (%d)\n", i, stInfo[i].kcps);
			printf("stInfo[%d].id 		= (%s)\n", i, stInfo[i].id);
			printf("stInfo[%d].filename = (%s)\n", i, stInfo[i].filename);
			iKcps += stInfo[i].kcps;
			iSessionCnt++;
		}
	}

	printf("*. Total Session Count = (%d) \n", iSessionCnt);
	printf("*. Total kcps = (%llu) \n", iKcps);

	return 0;
}

