#include "../include/BroadcastMessage.h"
#include "../include/Client.h"

BroadcastMessage::BroadcastMessage()
    : message(NULL),
      socket(0)
      // client(NULL)
{
  // CNPLog::GetInstance().Log("BroadcastMessage Construct");
}

// BroadcastMessage::BroadcastMessage(Client *const _client, char *_message)
BroadcastMessage::BroadcastMessage(int _socket, char *_message)
    // : client(_client),
    : socket(_socket)
{
  message = (char *)new char[MessageSize];
  memcpy(message, _message, MessageSize);
  // CNPLog::GetInstance().Log("BroadcastMessage Construct");
}

BroadcastMessage::~BroadcastMessage()
{
  delete message;
  // free(message);
  // CNPLog::GetInstance().Log("BroadcastMessage Destruct");
}

const char* BroadcastMessage::GetMessage()
{
  return message;
}

const int BroadcastMessage::GetSocketFD()
{
  return socket;
}