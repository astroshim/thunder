#include "../include/ThreadSender.h"
#include "../include/DownloadServer.h"
#include "../include/Client.h"
#include "../include/ClientUserDN.h"
#ifdef _FREEBSD
#include "../include/IOMP_KQUEUE.h"
#else
#include "../include/IOMP_EPoll.h"
#endif

#include "../include/NPUtil.h"
#include "../include/NPLog.h"

#ifdef _FREEBSD
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#else
#include <sys/sendfile.h>
#endif

ThreadSender::ThreadSender()
					:m_pDownloadServer(NULL)
{
	CNPLog::GetInstance().Log("ThreadSender Construct");
}

ThreadSender::ThreadSender(DownloadServer* const _pDownloadServer)
					:m_pDownloadServer(_pDownloadServer)
{
	CNPLog::GetInstance().Log("ThreadSender Construct");
}

ThreadSender::~ThreadSender()
{
	this->SetStarted(false);
	CNPLog::GetInstance().Log("ThreadSender Destruct");
}

void ThreadSender::Run()
{
	while(1)
	{
		int iErrno = 0;
        ClientUserDN *pClient = (ClientUserDN *)m_pDownloadServer->GetSendQueue();

		// sendfile call

#ifdef _DEBUG
		CNPLog::GetInstance().Log("In ThreadSender [%p]thread Client Geted! (%p) fd=(%d)",
							this, pClient, pClient->GetSocket()->GetFd());
#endif

#ifdef _ONESHOT // edge trigger
	#ifdef _FREEBSD
		size_t iSendSize = 0;
	    off_t nTransmitted = 0;
		size_t iLeft = pClient->GetSendSize();
	    off_t nOffset = pClient->GetFileOffset();

		pClient->SetStartTime(CNPUtil::GetMicroTime());
		while(iLeft > 0)
		{
			iErrno = 0;
/*
CNPLog::GetInstance().Log("send =>(%p)iLeft=(%d) OffSet=(%d), TotalSendSize=(%d), SendSize=(%d), err=(%d)",
			pClient, iLeft, pClient->GetFileOffset(), pClient->GetTotalSendSize(), iSendSize, iErrno);
*/

            iSendSize = sendfile(pClient->GetFileFd()
                                ,pClient->GetSocket()->GetFd()
                                //,pClient->GetFileOffset()
                                ,nOffset
                                ,iLeft
                                ,NULL
                                ,&nTransmitted
                                ,0);

			if(iSendSize == -1)
			{
				if (errno == ETIMEDOUT || errno == EAGAIN || errno == EINTR)
				{
					iErrno = EAGAIN;
	#ifdef _DEBUG
					CNPLog::GetInstance().Log("EAGAIN (%s),comcode=(%d),billno=(%d), nTransmitted=(%d), nOffset=(%d), iLeft=(%d)",
							strerror(errno), pClient->GetComCode(), pClient->GetBillNo(), nTransmitted,nOffset,iLeft);
	#endif
					break;
				}
				else
				{
					iErrno = USER_CLOSE;
					CNPLog::GetInstance().Log("(%p)sendfile error, (%s) (%d)(%d),nTransmitted=(%llu),iLeft=(%llu),iSendSize=(%d)",
										pClient, strerror(errno), errno, EOVERFLOW,
										nTransmitted,iLeft,iSendSize);
/*
					if(EBADF == errno)	// client is already killed
					{
						iErrno = EBADF;
						break;
					}
*/
					if(EBADF == errno)	// client is already killed
					{
						break;
					}

	#ifdef _CLIENT_ARRAY
				    m_pDownloadServer->CloseClient(pClient->GetUserSeq());
	#else
				    m_pDownloadServer->CloseClient(pClient);
	#endif
					break;
				}

				iSendSize = 0;
			}
			else
			if(nTransmitted == 0)
			{
				CNPLog::GetInstance().Log("(%p)sendfile error 2, (%s) (%d)(%d),nTransmitted=(%d),iLeft=(%d),iSendSize=(%d)",
										pClient, strerror(errno), errno, EOVERFLOW,
										nTransmitted,iLeft,iSendSize);

				//iErrno = EBADF; // �߰��ұ� ����.....
	#ifdef _CLIENT_ARRAY
			    m_pDownloadServer->CloseClient(pClient->GetUserSeq());
	#else
			    m_pDownloadServer->CloseClient(pClient);
	#endif
					break;
			}

			nOffset += nTransmitted;
            iLeft -= nTransmitted;
/*
CNPLog::GetInstance().Log("================(%p)send after iLeft=(%d) iSendSize=(%d), nTransmitted=(%d), offset=(%d)",
            pClient, iLeft, iSendSize, nTransmitted, nOffset);
*/
		}

	#else // linux
		int iSendSize = 0;
	    // off64_t nTransmitted = pClient->GetFileOffset(); // linux
	    off_t nTransmitted = pClient->GetFileOffset(); // mac
		uint64_t iLeft = pClient->GetSendSize();

	#ifdef _DEBUG
CNPLog::GetInstance().Log("(%p)send�ؾ��� size=(%llu) OffSet=(%llu), TotalSendSize=(%llu), SendSize=(%d), Left=(%d),err=(%d)",
			pClient, iLeft, pClient->GetFileOffset(), pClient->GetTotalSendSize(), iSendSize, iLeft, iErrno);
	#endif

		pClient->SetStartTime(CNPUtil::GetMicroTime());
		iErrno = 0;
		while(iLeft > 0)
		{
			iSendSize = sendfile64(pClient->GetSocket()->GetFd(),
													pClient->GetFileFd(),
													// (off64_t *)&nTransmitted,
													(off_t *)&nTransmitted, // mac
													iLeft);
													//pClient->GetSendSize());
			if(iSendSize <= 0)
			{
				//iErrno = errno;
				if (errno == ETIMEDOUT || errno == EAGAIN)
				{
					iErrno = EAGAIN;
	#ifdef _DEBUG
					CNPLog::GetInstance().Log("EAGAIN (%s),comcode=(%d),billno=(%d)",
							strerror(errno), pClient->GetComCode(), pClient->GetBillNo());
	#endif
					break;
				}
				else
				{
					iErrno = USER_CLOSE;
					CNPLog::GetInstance().Log("(%p)sendfile error, (%s) (%d)(%d),nTransmitted=(%llu),iLeft=(%llu),iSendSize=(%d)",
										pClient, strerror(errno), errno, EOVERFLOW,
										nTransmitted,iLeft,iSendSize);
/*
					if(EBADF == errno)	// client is already killed
					{
						iErrno = EBADF;
						break;
					}
*/
					if(EBADF == errno)	// client is already killed
					{
						break;
					}

	#ifdef _CLIENT_ARRAY
				    m_pDownloadServer->CloseClient(pClient->GetUserSeq());
	#else
				    m_pDownloadServer->CloseClient(pClient);
	#endif
					break;
				}

				iSendSize = 0;
			}
			iLeft -= iSendSize;
		}
	#endif

#ifdef _FREEBSD
		if(iErrno != 0)
		{
			nOffset += nTransmitted;
			iLeft -= nTransmitted;
		}
#endif
		//if(EBADF == iErrno)
		if(USER_CLOSE == iErrno)
		{
			continue;
		}

		// ���� size
		iSendSize = pClient->GetSendSize() - iLeft;
		pClient->AddFileOffset(iSendSize);
		pClient->AddTotalSendSize(iSendSize);

		// sleep in order to speed control.
		pClient->SpeedControl(iLeft);

		// set next send size
		pClient->SetSendSize(iLeft);

/*
CNPLog::GetInstance().Log("1.sendfile left=(%d), sendsize=(%d)", iLeft, iSendSize);
CNPLog::GetInstance().Log("2.sendfile OffSet=(%d)", pClient->GetFileOffset());
CNPLog::GetInstance().Log("3.sendfile TotalSendSize=(%d)", pClient->GetTotalSendSize());
*/
		// 20090331 added
		// 20090527 update
		m_pDownloadServer->AddThroughput(pClient->GetComCodeIdx(), iSendSize);

		// Write client down size on Shm
		m_pDownloadServer->WriteUserInfo(pClient);

		// 20090402 added
		if((pClient->GetSendTime() > CNPUtil::GetMicroTime()) && iLeft != 0)
		{
			iErrno = EAGAIN;
		}

		if(iErrno == EAGAIN ||
			((pClient->GetSendTime() > CNPUtil::GetMicroTime()) && (iLeft != 0)))
		{
//CNPLog::GetInstance().Log("EAGAIN OR TIME available! (%p)", pClient);
			// if EAGAIN set ThreadQoS
			//m_pDownloadServer->AddQoS(pClient, EPOLLOUT|EPOLLET|EPOLLONESHOT);

	#ifdef _FREEBSD
			m_pDownloadServer->AddQoS(pClient, 0);
	#else
			m_pDownloadServer->AddQoS(pClient, EPOLLOUT);
	#endif
			continue;
		}

        //if(iErrno != USER_CLOSE)
        {
    #ifdef _FREEBSD
        	m_pDownloadServer->AddEPoll(pClient, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT|EV_ERROR);
    #else
        	m_pDownloadServer->UpdateEPoll(pClient, EPOLLIN|EPOLLET|EPOLLONESHOT);
    #endif
        }

#else	// level trigger
	    std::list<T_PACKET*> *lstPacket;
	    std::list<T_PACKET*>::iterator iter;

	    lstPacket = pClient->GetPacketList();
	    iter = lstPacket->begin();
	    while( iter != lstPacket->end() )
	    {
	        T_PACKET *pPacket = static_cast<T_PACKET *>(*iter);
			lstPacket->erase(iter);
	        //T_PACKET *pPacket = static_cast<T_PACKET *>(*(iter++));
			//pClient->RemovePacket(pPacket);

			if(pPacket->header.command == cmd_GET_FILE_DS_DC)
			{
				//double dStartTime = CNPUtil::GetMicroTime();
				pClient->SetStartTime(CNPUtil::GetMicroTime());

				// sendfile call
				int iSendSize = 0;
				//int iLeft = pClient->GetSendSize();
				uint64_t iLeft = 0;

				//iLeft = pClient->GetSendSize();
                off_t nTransmitted = pClient->GetFileOffset(); // mac
                // off64_t nTransmitted = pClient->GetFileOffset();

				// define have to send size.
				//iLeft = pClient->GetRealSendSize();
				iLeft = pClient->GetSendSize();

	#ifdef _DEBUG
CNPLog::GetInstance().Log("(%p)send�ؾ��� size=(%llu) OffSet=(%llu), TotalSendSize=(%llu), SendSize=(%d), Left=(%d),err=(%d)",
					pClient, iLeft, pClient->GetFileOffset(), pClient->GetTotalSendSize(), iSendSize, iLeft, iErrno);
	#endif

				///////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////
				iErrno = 0;
				while(iLeft > 0)
				{
					//iSendSize = sendfile(((Socket *)(pClient->GetSocket()))->GetFd(),
					//iSendSize = sendfile64(((Socket *)(pClient->GetSocket()))->GetFd(),
					iSendSize = sendfile64(pClient->GetSocket()->GetFd(),
															pClient->GetFileFd(),
															// (off64_t *)&nTransmitted,
															(off_t *)&nTransmitted, // mac
															iLeft);
															//pClient->GetSendSize());
					if(iSendSize <= 0)
					{
						iErrno = errno;
						if (errno == ETIMEDOUT || errno == EAGAIN)
						{
							iErrno = EAGAIN;
	#ifdef _DEBUG
							CNPLog::GetInstance().Log("EAGAIN (%s),comcode=(%d),billno=(%d)",
									strerror(errno), pClient->GetComCode(), pClient->GetBillNo());
	#endif
							pClient->AddPacketFront(pPacket);
							break;
						}
						else
						{
							iErrno = USER_CLOSE;
							CNPLog::GetInstance().Log("(%p)sendfile error, (%s) (%d)(%d),nTransmitted=(%llu),iLeft=(%llu),iSendSize=(%d)",
												pClient, strerror(errno), errno, EOVERFLOW,
												nTransmitted,iLeft,iSendSize);

							delete pPacket;
							break;
						}

						iSendSize = 0;
					}

					iLeft -= iSendSize;
				}

				//if(iErrno != EAGAIN)
				if(iErrno == 0)	// if you send amount you want.
				{
					delete pPacket;
				}

				// ���� size
				iSendSize = pClient->GetSendSize() - iLeft;
				pClient->AddFileOffset(iSendSize);
				pClient->AddTotalSendSize(iSendSize);

				// sleep in order to speed control.
				//pClient->SpeedControl(nOffSet);
				//pClient->SpeedControl(iSendSize);
				pClient->SpeedControl(iLeft);

				pClient->SetSendSize(iLeft);

	#ifdef _DEBUG
CNPLog::GetInstance().Log("sendfile end(%p) OffSet=(%llu), TotalSendSize=(%llu), SendSize=(%d), Left=(%llu),err=(%d)",
					pClient, pClient->GetFileOffset(), pClient->GetTotalSendSize(), iSendSize, iLeft, iErrno);
	#endif

				// 20090331 added
				m_pDownloadServer->AddThroughput(pClient->GetComCodeIdx(), iSendSize);

				// Write client down size on Shm
				m_pDownloadServer->WriteUserInfo(pClient);

				// 20090402 added
				if((pClient->GetSendTime() > CNPUtil::GetMicroTime()) && iLeft != 0)
				{
					iErrno = EAGAIN;
				}
				break;
			}
			else
			{
				//int iWriten = ((Socket *)(pClient->GetSocket()))->Write((char *)pPacket, PDUHEADERSIZE+pPacket->header.length);
				int iWriten = pClient->GetSocket()->Write((char *)pPacket, PDUHEADERSIZE+pPacket->header.length);

				Tcmd_GET_FSIZE_DC_DS *sndBody = (Tcmd_GET_FSIZE_DC_DS *)pPacket->data;

				if(pPacket->header.command == cmd_GET_FSIZE_DC_DS)
				{
	    			CNPLog::GetInstance().Log("ThreadSender cmd_GET_FSIZE_DS_DC (%p), iCmd=(%d),pchFileName=(%s),nFSize=(%lld),pchID=(%s), iWriten=(%d), (%d)",
								pClient, sndBody->iCmd, sndBody->pchFileName, sndBody->nFSize, sndBody->pchID, iWriten, PDUHEADERSIZE+pPacket->header.length);
				}
				else
				{
	    			CNPLog::GetInstance().Log("In ThreadSender NormalCommand (%p) commnad=(%d), Send count = (%d), Client=(%p)",
								pClient, pPacket->header.command, iWriten, pClient);
				}

	    		CNPLog::GetInstance().Log("In ThreadSender OH MY GOD!! (%p)", pClient);
				if(pClient->GetState() == STATE_CLOSED)
				{
					break;
				}

				delete pPacket;
			}

			iter++;
		}

	#ifdef _DEBUG
		CNPLog::GetInstance().Log("In ThreadSender [%p]thread updateEpoll! (%p) fd=(%d)",
							this, pClient, pClient->GetSocket()->GetFd());
	#endif


		m_pDownloadServer->AddEPoll(pClient, EPOLLIN|EPOLLOUT);
#endif
	}

	delete this;
	pthread_exit(NULL);
}

