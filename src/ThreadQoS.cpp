#include "../include/ThreadQoS.h"
#include "../include/DownloadServer.h"
#include "../include/TcpSocket.h"
#include "../include/ServerSocket.h"
#include "../include/Client.h"
#include "../include/ClientUserDN.h"
#include "../include/NPLog.h"
#include "../include/NPUtil.h"

ThreadQoS::ThreadQoS(DownloadServer* const _pMainProcess)
{
  m_pMainProcess = _pMainProcess;
}

ThreadQoS::ThreadQoS()
{
}

ThreadQoS::~ThreadQoS()
{
}

const int ThreadQoS::AddQoS(Client* const _pClient, const unsigned int _uiEvents)
{   
    return 0;
}

const int ThreadQoS::RemoveQoS(Client* const _pClient)
{
    return 0;
}

void ThreadQoS::Run()
{
  ;
}

