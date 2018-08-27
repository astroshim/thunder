#include "../include/BroadcastMessage.h"
#include "../include/Client.h"

BroadcastMessage::BroadcastMessage()
    : 
      messageType(CLIENT_CHAT_MESSAGE),
      socket(0),
      message(NULL),
      messageSize(0)
{
  message = (char *)new char[MAX_MESSAGE_SIZE];
}

BroadcastMessage::~BroadcastMessage()
{
  delete message;
}

void BroadcastMessage::SetMessage(char *_message)
{
  memset(message, 0x00, sizeof(char)*MAX_MESSAGE_SIZE);
  memcpy(message, _message, MAX_MESSAGE_SIZE);
}

void BroadcastMessage::SetMessage(char *_message, int _size)
{
  memcpy(message, _message, _size);
  message[strcspn(message, "\r\n")] = 0;
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