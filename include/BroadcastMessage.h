#ifndef __BROADCASTMESSAGE_H
#define __BROADCASTMESSAGE_H

#include <stdio.h>
#include "./NonCopyable.h"

const int MessageSize = 1024;

/**
 * NonCopyable Class
 */
// class Client;
class BroadcastMessage : private NonCopyable
{
  public:
    BroadcastMessage();
    BroadcastMessage(int _socket, char *_message);
    // BroadcastMessage(Client *_client, char *_message);
    virtual ~BroadcastMessage();

    const char* GetMessage();
    const int GetSocketFD();

  private:
    int  socket;
    // Client  *client;
    char    *message;
};

#endif

