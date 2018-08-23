#include "./include/Include.h"
#include "./include/ChatManager.h"
#include "./include/ChatServer.h"
#include "./include/Properties.h"
#include "./include/NPLog.h"
#include "./include/NPDebug.h"

static const char *g_Version = "1.0.1";

void SetLimit()
{
  struct rlimit new_rl;

  new_rl.rlim_cur = 20000;
  new_rl.rlim_max = 20000;

  if (setrlimit(RLIMIT_NOFILE, &new_rl) < 0)
  {
    Assert(false, "can't set file no limit ");
  }
}

void sig_handler(int signo)
{
  CNPLog::GetInstance().Log("I got the Signal (%d) ", signo);
}

void SetSignal()
{
  struct sigaction act;

  act.sa_handler = sig_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  /*
     sigset_t newmask;
     sigemptyset(&newmask);
     sigaddset(&newmask, SIGTERM);
     sigaddset(&newmask, SIGHUP);
     sigaddset(&newmask, SIGINT);
     sigaddset(&newmask, SIGPIPE);
     */
  sigaction(SIGTERM, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGHUP, &act, NULL);
  sigaction(SIGPIPE, &act, NULL);
  sigaction(SIGUSR2, &act, NULL);
}

int main(int _argc, char *_argv[])
{
  if (_argc > 1)
  {
    if (strcmp(_argv[1], "-v") == 0)
    {
      //printf("DNServer version [%s] \n", g_Version);
      printf("%s\n", g_Version);
    }
    exit(1);
  }

  SetSignal();
  //SetLimit();

  /**
   *   Properties Load
   */
  Properties managerProperties;
  if (managerProperties.Load(MANAGER_PROPERTIES) < 0)
  {
    Assert(false, "Properties Load Error! ");
  }

  Properties chatProperties;
  if (chatProperties.Load(CHATSERVER_PROPERTIES) < 0)
  {
    Assert(false, "Properties Load Error! ");
  }
  ChatServer *pChatServer = new ChatServer(chatProperties);
  ChatManager *pChatManager = new ChatManager(managerProperties);

  pChatManager->DoFork(pChatServer);

  delete pChatServer;
  delete pChatManager;

  return 0;
}
