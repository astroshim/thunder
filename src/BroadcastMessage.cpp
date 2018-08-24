#include "../include/BroadcastMessage.h"
#include "../include/Client.h"

BroadcastMessage::BroadcastMessage()
    : message(NULL),
      socket(0),
      message_size(0)
      // client(NULL)
{
  message = (char *)new char[MessageSize];
}

BroadcastMessage::~BroadcastMessage()
{
  delete message;
}

void BroadcastMessage::SetMessage(char *_message)
{
  memcpy(message, _message, MessageSize);
}

const char* BroadcastMessage::GetMessage()
{
  return message;
}

const int BroadcastMessage::GetSocketFd()
{
  return socket;
}

const int BroadcastMessage::GetMessageSize()
{
  return message_size;
}

void BroadcastMessage::SetSocketFd(int fd)
{
  socket = fd;
}

void BroadcastMessage::SetMessageSize(int _message_size)
{
  message_size = _message_size;
}