#ifndef __UNIQUEID_H
#define __UNIQUEID_H

#include <stdio.h>
#include <string.h>

class UniqueID
{
  private:
    static const size_t KEY_SIZE = 16;
    unsigned char m_pchKey[KEY_SIZE]; 

  public:
    UniqueID();
    UniqueID(const char *_pchSeed);
    UniqueID(const UniqueID& _cOther);
    ~UniqueID();

  public:
    bool operator == (const UniqueID& _cOther) const;
    bool operator < (const UniqueID& _cOther) const;
    bool operator > (const UniqueID& _cOther) const;
    bool operator <= (const UniqueID& _cOther) const;
    bool operator >= (const UniqueID& _cOther) const;
};

#endif
