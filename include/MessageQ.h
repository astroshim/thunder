#ifndef  __MESSAGEQ_H
#define  __MESSAGEQ_H

#include "./Include.h"

const int  MAX_DATA_SIZE = 256;



/**
 *   UboneQManager Class 에서 명령을 받아 들이는 메세지큐
 */
//const key_t MQKEY_uboneQManager = 10001;

/**
 *   TrProcess 에게 필요한인자(Queue/shm Key, ip, port 등)을 넘기기 위한 메세지큐
 */
/*
   const key_t MQKEY_TrProcess   = 10002;
 */

/**
 *   UboneQManager process ID
 */
const int P_uboneQManager   = 1000;
const int P_Console     = 1001;

/*
   const int P_uboneQManager2  = 1010;
   const int P_TrProcess   = 1011;
 */

const int Q_BLOCK = 0;

class MessageQ
{
  public:
    struct extend_msg_t
    {
      int iReceiver;
      int iSender;
      int iCommand;
      int iLength;
      /*
         long int iReceiver;
         long int iSender;
         long int iCommand;
         long int iLength;
       */
      char chData[MAX_DATA_SIZE];
    }__attribute__((packed));

    struct base_msg_t
    {
      int   iLength;
      char  chData[MAX_DATA_SIZE];  /* Data */
    } __attribute__((packed));

    MessageQ();
    MessageQ(const key_t _key);
    MessageQ(const key_t _key, bool _bCreator);
    virtual ~MessageQ();

    bool  IsStarted();
    key_t   GetKey();
    int   GetQid();   /* console에서 큐를 삭제 하기 위해 제공한다. */

    bool EnQueue(int _sender, int _receiver, int _command, int _msg_length, char *_msg, int falg, int _timeout);
    bool DeQueue(int *_sender, int _receiver,int *_command, int *_msg_length, char *_msg, int falg, int _timeout);

    bool EnQueue(char *_msg, int _msg_length, int falg, int _timeout);
    bool DeQueue(char *_msg, int *_msg_length, int falg, int _timeout);

  private:
    void SetDeleteOK(bool _bDelete);
    void CreateMQ(const key_t _key);

    key_t   m_Key;
    int   m_QID;
    bool  m_IsStarted;
    bool  m_bDeleteOK;
};
#endif
