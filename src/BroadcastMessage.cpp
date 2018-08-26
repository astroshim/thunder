#include "../include/BroadcastMessage.h"
#include "../include/Client.h"

BroadcastMessage::BroadcastMessage()
    : message(NULL),
      socket(0),
      messageSize(0),
      messageType(CLIENT_CHAT_MESSAGE)
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

const ENUM_MESSAGE_TYPE BroadcastMessage::GetMessageType()
{
  return messageType;
}

const int BroadcastMessage::GetMessageSize()
{
  return messageSize;
}

void BroadcastMessage::SetSocketFd(int fd)
{
  socket = fd;
}

void BroadcastMessage::SetMessageSize(int _messageSize)
{
  messageSize = _messageSize;
}

void BroadcastMessage::SetMessageType(ENUM_MESSAGE_TYPE _messageType)
{
  messageType = _messageType;
}