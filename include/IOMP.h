#ifndef __IOMP_H
#define __IOMP_H

class IOMP
{
  public:
    IOMP() {;};
    virtual ~IOMP() {;};

    virtual void  AddFd(const int iFd)=0;
    virtual void  DelFd(const int iFd)=0;
    virtual const int   Polling()=0;
    virtual const int CheckEvent(const int iFd)=0;

    //  virtual void  InitRFdSet()=0;
};

#endif

