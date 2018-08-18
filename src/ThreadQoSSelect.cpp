#include "../include/ThreadQoSSelect.h"
#include "../include/DownloadServer.h"
#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/Client.h"
#include "../include/ClientUserDN.h"
#include "../include/NPLog.h"
#include "../include/NPUtil.h"

#include "../include/IOMP_Select.h"

ThreadQoSSelect::ThreadQoSSelect(DownloadServer* const _pMainProcess)
							:ThreadQoS(_pMainProcess)
{
	this->SetStarted(true);
    pthread_mutex_init(&m_lockClient, NULL);
	CNPLog::GetInstance().Log("ThreadQoSSelect Construct");
}

ThreadQoSSelect::ThreadQoSSelect()
{
	this->SetStarted(false);
    pthread_mutex_init(&m_lockClient, NULL);
}

ThreadQoSSelect::~ThreadQoSSelect()
{
	this->SetStarted(false);
	CNPLog::GetInstance().Log("ThreadQoSSelect Destruct");
}

const int ThreadQoSSelect::AddQoS(Client* const _pClient, const unsigned int _uiEvents)
{
//CNPLog::GetInstance().Log("epoll_Add (%p)", _pClient);
    pthread_mutex_lock(&m_lockClient);
	m_lstClient.push_back((Client*)_pClient);
    m_pIOMP->AddWriteFd(_pClient->GetSocket()->GetFd());
    pthread_mutex_unlock(&m_lockClient);
	return 0;
}

const int ThreadQoSSelect::RemoveQoS(Client* const _pClient)
{
	m_pIOMP->DelWriteFd(_pClient->GetSocket()->GetFd());
	m_pMainProcess->PutSendQueue(_pClient);
	return 0;
}

void ThreadQoSSelect::Run()
{
	 m_pIOMP = new IOMP_Select(0, 1000);

	while(1)
	{
        int iEventCount;

        if((iEventCount = m_pIOMP->WritePolling()) <= 0)
        {
            //CNPLog::GetInstance().Log("epoll_wait error errno=%d, strerror=(%s)", errno, strerror(errno));
            continue;
        }

    	pthread_mutex_lock(&m_lockClient);
    	std::list<Client*>::iterator itrClient;
		itrClient = m_lstClient.begin();
		while( itrClient != m_lstClient.end() && iEventCount > 0)
		{
			Client *pClient = static_cast<Client *>(*itrClient);

			if(m_pIOMP->CheckEvent( pClient->GetSocket()->GetFd() ))
			{
                if(static_cast<ClientUserDN*>(pClient)->GetSendTime() < CNPUtil::GetMicroTime())
                {
//CNPLog::GetInstance().Log("ThreadQos POLLOUT Send to Sender=>>(%p)", pClient);
/*
                    m_pIOMP->DelWriteClient(etSocket()->GetFd());
                    m_pMainProcess->PutSendQueue(pClient);
*/
    				//pthread_mutex_lock(&m_lockClient);
					itrClient = m_lstClient.erase(itrClient);	
					RemoveQoS(pClient);
    				//pthread_mutex_unlock(&m_lockClient);
                }
			}

			itrClient++;
			if(--iEventCount <= 0) break;
		}
		pthread_mutex_unlock(&m_lockClient);

		CNPUtil::NanoSleep(1000000L);   // 0.1
	}

	pthread_exit(NULL);
}

