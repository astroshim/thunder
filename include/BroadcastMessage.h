#ifndef __BROADCASTMESSAGE_H
#define __BROADCASTMESSAGE_H

#include <stdio.h>
#include "./NonCopyable.h"

const int MessageSize = 1024;

/**
 * NonCopyable Class
 */
class BroadcastMessage : private NonCopyable
{
  public:
    BroadcastMessage();
    virtual ~BroadcastMessage();

    const char* GetMessage();
    const int GetSocketFd();
    const int GetMessageSize();

    void SetMessageSize(int _message_size);
    void SetSocketFd(int _socket);
    void SetMessage(char *_message);

  private:
    int  socket;
    char *message;
    int  message_size;
};

#endif

