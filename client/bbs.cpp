#include "./Include.h"
#include "./Packet.h"

int BBS_DS_DOWNFINISH_REQ(int _fd, int _iComCode, int _iBillNo)
{
    T_PACKET sndpacket;
    Tcmd_BBS_DS_DOWNFINISH_REQ *sndbody = (Tcmd_BBS_DS_DOWNFINISH_REQ *)sndpacket.data;;
    sndpacket.header.command    = cmd_BBS_DS_DOWNFINISH_REQ;
    sndpacket.header.length     = sizeof(Tcmd_BBS_DS_DOWNFINISH_REQ);

  sndbody->nComCode = _iComCode;
  sndbody->nBillNo = _iBillNo;

    return write(_fd, (char *)&sndpacket, sndpacket.header.length+PDUHEADERSIZE);
}

int BBS_DS_DOWNFINISH_RES(int _fd)
{
    T_PACKET rcvpacket;
    Tcmd_BBS_DS_DOWNFINISH_RES *rcvbody = (Tcmd_BBS_DS_DOWNFINISH_RES *)&rcvpacket.data;;

    if(read(_fd, (char *)&rcvpacket, PDUHEADERSIZE+sizeof(Tcmd_BBS_DS_DOWNFINISH_RES)) <= 0)
    {
        printf("error ");
        return 0;
    }

printf("BBS_DS_DOWNFINISH_RES nRet=(%d), nDownSize=(%llu) \n",
                    rcvbody->nRet, rcvbody->nDownSize);

    return 0;
}

int Connect(char *_pchIP, int _iPort)
{
  int sockfd;
  struct sockaddr_in address;
  int len;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
printf("Client Socket ====> (%d) \n", sockfd);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(_pchIP);
  address.sin_port = htons((u_short)_iPort);
  len = sizeof(address);

  if(connect(sockfd, (struct sockaddr *)&address, len) < 0) {
    printf("connect error!\n");
    exit(1);
  }
  else
    puts("socket create!!");

  return sockfd;
}


int main(int argc, char *argv[])
{
  int sockfd;
  int iComCode, iBillNo;


/*
  if(argc < 3)
  {
    printf("usage : bbs [comcode] [billno] \n");
    return 0;
  }
  iComCode = atoi(argv[1]);
  iBillNo = atoi(argv[2]);
*/


  /***********************************************
  // DC <-> DSM
  ***********************************************/
  // Connect to DSM
  sockfd = Connect("127.0.0.1", 50000);

int i = 1;
while(1)
{
  if(i++ > 1000)
  {
    i = 1;
  }

  iComCode = i%10;
  iBillNo = i%10;

  if(i%2 == 0)
  {
    BBS_DS_DOWNFINISH_REQ(sockfd, iComCode, iBillNo);
    BBS_DS_DOWNFINISH_RES(sockfd);
  }
usleep(500000);
}


  close(sockfd);

  return 0;
}

