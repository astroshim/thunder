#ifndef __CLIENTDN_H
#define __CLIENTDN_H

#include "./Client.h"

class ClientDN: public Client
{
private:
	int m_iPort;
	char m_pchIpAddr[MAX_IP_LEN];

	// Download Server information
/*
	int	m_iSeq;
	int	m_iPid;
	int	m_iMaxUser;
	int	m_iShmKey;
*/

public:	
	ClientDN();
	ClientDN(Socket* const _cSocket);
	virtual ~ClientDN();

    // BBS command
    void WorkDownloadFinishBBS(const T_PACKET &_tPacket);

    // DC->DSM command
    void WorkDSHello(const T_PACKET &_tPacket);
    void WorkUserClose(const T_PACKET &_tPacket);

    // DC->DSM command
    void WorkDSMHello(const T_PACKET &_tPacket);
    void WorkGetDSInfo(const T_PACKET &_tPacket);
    void WorkDSMPing(const T_PACKET &_tPacket);
    void WorkGoodBye(const T_PACKET &_tPacket);

/*
    void WorkInformFile(const T_PACKET &_tPacket);
    void WorkDownSize(const T_PACKET &_tPacket);
*/

	//const int ExecuteCommand();
	const int ExecuteCommand(Thread *_pThread);
};

#endif

