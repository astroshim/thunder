#include "../include/ThreadAcceptor.h"
#include "../include/DownloadServer.h"
#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/Client.h"
#include "../include/ClientUserDN.h"
#include "../include/IOMP_Select.h"
#include "../include/NPLog.h"

ThreadAcceptor::ThreadAcceptor(DownloadServer* const _pMainProcess)
							:m_pMainProcess(_pMainProcess)
{
	this->SetStarted(true);
	CNPLog::GetInstance().Log("ThreadAcceptor Construct");
}

ThreadAcceptor::ThreadAcceptor()
{
	this->SetStarted(false);
}

ThreadAcceptor::~ThreadAcceptor()
{
	this->SetStarted(false);
	CNPLog::GetInstance().Log("ThreadAcceptor Destruct");
}

void ThreadAcceptor::Run()
{
	pthread_detach(pthread_self());
	//int iServerFd = ((Socket *)((Client*)m_pMainProcess->GetServerSocket())->GetSocket())->GetFd();
	int iServerFd = m_pMainProcess->GetServerSocket()->GetSocket()->GetFd();

	IOMP *pcIomp = new IOMP_Select(0, 1000);
	pcIomp->AddFd(iServerFd);

	while(1)
	{
/*
* block accept
	    Socket *pClientSocket;

        if(m_pMainProcess->GetCurrentUserCount() >= m_pMainProcess->GetMaxUser())
	    {
	        CNPLog::GetInstance().Log("Acceptor OverFlow (%d), (%d) ", 
							m_pMainProcess->GetCurrentUserCount(), 
							m_pMainProcess->GetMaxUser());
			sleep(5);
	        continue;
	    }

	    if((pClientSocket = ((ServerSocket *)((Client*)m_pMainProcess->GetServerSocket())->GetSocket())->Accept()) != NULL)
	    {
	        pClientSocket->SetNonBlock();
	        CNPLog::GetInstance().Log("Accept ====> ClientIp=(%s),CurrentUser=(%d),Max=(%d)" ,
	                            ((TcpSocket *)pClientSocket)->GetClientIp(), 
								m_pMainProcess->GetCurrentUserCount(), 
								m_pMainProcess->GetMaxUser());

	        CNPLog::GetInstance().Log("1.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
	        pClientSocket->SetSndBufSize(4*1024*1024);
	        CNPLog::GetInstance().Log("2.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
	        pClientSocket->SetTcpCORK(1);

			m_pMainProcess->AcceptClient(pClientSocket);
		}
*/

        int eventCnt = 0;
        if((eventCnt = pcIomp->Polling()) <= 0)
        {
            continue;
        }

	    if(pcIomp->CheckEvent(iServerFd) )
	    {
/*
        	if(m_pMainProcess->GetCurrentUserCount() >= m_pMainProcess->GetMaxUser())
			{
				CNPLog::GetInstance().Log("Acceptor Max User OverFlow! CurrentUser=(%d), MaxUser=(%d)", 
									m_pMainProcess->GetCurrentUserCount(), 
									m_pMainProcess->GetMaxUser());
				sleep(10);
				continue;
			}
*/
#ifdef _CLIENT_ARRAY
			int iClientFD;
	    	if((iClientFD = static_cast<ServerSocket *>(((Client*)m_pMainProcess->GetServerSocket())->GetSocket())->AcceptFD()) < 0)
			{
				close(iClientFD);
				continue;
			}
			if(m_pMainProcess->GetCurrentUserCount() >= m_pMainProcess->GetMaxUser())
			{
				CNPLog::GetInstance().Log("Acceptor Max User OverFlow! CurrentUser=(%d), MaxUser=(%d)", 
									m_pMainProcess->GetCurrentUserCount(), 
									m_pMainProcess->GetMaxUser());

				close(iClientFD);
				sleep(5);
				continue;
			}
			else
			{
				m_pMainProcess->AcceptClient(iClientFD);
			}
#else
			Socket *pClientSocket;
	    	//if((pClientSocket = m_pMainProcess->GetServerSocket())->GetSocket()->Accept() != NULL)
	    	if((pClientSocket = static_cast<ServerSocket *>(((Client*)m_pMainProcess->GetServerSocket())->GetSocket())->Accept()) != NULL)
			{
				// 20090225 위에서 여기로 이동..
				if(m_pMainProcess->GetCurrentUserCount() >= m_pMainProcess->GetMaxUser())
				{
					CNPLog::GetInstance().Log("Acceptor Max User OverFlow! CurrentUser=(%d), MaxUser=(%d)", 
										m_pMainProcess->GetCurrentUserCount(), 
										m_pMainProcess->GetMaxUser());

					delete pClientSocket;
					sleep(5);
					continue;
				}

				struct in_addr laddr;
				laddr.s_addr = static_cast<TcpSocket *>(pClientSocket)->GetClientIp();

				pClientSocket->SetNonBlock();
				CNPLog::GetInstance().Log("Accept ====> (%p)ClientIp=(%s),CurrentUser=(%d),Max=(%d)" ,
									pClientSocket, 
									inet_ntoa(laddr),
									m_pMainProcess->GetCurrentUserCount(), 
									m_pMainProcess->GetMaxUser());
				m_pMainProcess->AcceptClient(pClientSocket);
/*
				CNPLog::GetInstance().Log("1.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
				pClientSocket->SetSndBufSize(4*1024*1024);
				CNPLog::GetInstance().Log("2.----> GetSndBuff ==(%d)", pClientSocket->GetSndBufSize());
				pClientSocket->SetTcpCORK(1);
*/


/*
				if(pServerSocket->GetType() == SERVER_PORT)
				{
					pClient = new ClientUser(pClientSocket);
				}
	            else
	            {
	                CNPLog::GetInstance().Log("There is no platform!");
	                delete pClientSocket;
			        if(--eventCnt <= 0) break;
	                continue;
	            }
*/
	        }
#endif
		}
		else
		{
	    	CNPLog::GetInstance().Log("Acceptor EventCheck error!");
		}
	}

	pthread_exit(NULL);
}

