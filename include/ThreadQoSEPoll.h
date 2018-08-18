#ifndef __THREADQOSEPOLL_H
#define __THREADQOSEPOLL_H

#include "./ThreadQoS.h"

class DownloadServer;
class Client;
class IOMP_EPoll;

class ThreadQoSEPoll: public ThreadQoS
{
private :
	//DownloadServer *m_pMainProcess;
	IOMP_EPoll *m_pIOMP;

public:
	ThreadQoSEPoll();
	ThreadQoSEPoll(DownloadServer* const _pMainProcess);
	~ThreadQoSEPoll();

    //const int AddQoS(Client* const _pClient);
    const int AddQoS(Client* const _pClient, const unsigned int _uiEvents);
    const int RemoveQoS(Client* const _pClient);

/*
	const int UpdateEPoll(Client* const _pClient, const unsigned int _uiEvents);
	const int AddEPoll(Client* const _pClient, const unsigned int _uiEvents);
*/

	void Run();
};

#endif

