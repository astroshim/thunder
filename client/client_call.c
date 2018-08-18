#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int i, client_num;
    pid_t my_pid;
    char *args[5];

	char *pchID;
	char *pchFileName;
	char *pchIP;

	if(argc < 4) {
		puts("usage : ./client_call [접속수] [파일명] [dsm ip]");
		return 0;
	}

	pchID = (char *)malloc(sizeof(char)*20);
	memset(pchID, 0x00, sizeof(char)*20);

	pchFileName = (char *)malloc(sizeof(char)*128);
	memset(pchFileName, 0x00, sizeof(char)*128);

/*
	pchIP = (char *)malloc(sizeof(char)*20);
	memset(pchIP, 0x00, sizeof(char)*20);
*/

	
	client_num = atoi(argv[1]);

    for(i = 0; i < client_num; i++) {

        if((my_pid = fork()) < 0) {
            perror("fork error : ");
            return -1;
        }
        else if(my_pid == 0) {
//			system("./client");

			memset(pchFileName, 0x00, sizeof(char)*128);
			sprintf(pchFileName, "%s%d", argv[2], (i+1)%10);

            args[0] = "./client.exe";
			sprintf(pchID, "%d", i+1);
            args[1] = pchID;
            //args[2] = argv[2];		// file name
            args[2] = pchFileName;		// file name
            args[3] = argv[3];		// ip
            args[4] = NULL;

            if(execv(args[0], args) < 0)
            {
                perror("execv error : ");
                return 0;
            }
        }
//		usleep(1000);
    }

	free(pchID);
	free(pchFileName);
    return 0;
}

