#ifndef __BROADCASTMESSAGE_H
#define __BROADCASTMESSAGE_H

#include <stdio.h>
#include "./NonCopyable.h"

const int MAX_MESSAGE_SIZE = 1024;

typedef enum
{
  CLIENT_CHAT_MESSAGE = 0,      //  
  RELAYED_MESSAGE,    // 
}ENUM_MESSAGE_TYPE;

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
    const ENUM_MESSAGE_TYPE GetMessageType();

    void SetMessageType(ENUM_MESSAGE_TYPE _messageType);
    void SetMessageSize(int _messageSize);
    void SetSocketFd(int _socket);
    void SetMessage(char *_message);
    void SetMessage(char *_message, int _size);

  private:
    ENUM_MESSAGE_TYPE  messageType;
    int  socket;
    char *message;
    int  messageSize;
};

#endif

