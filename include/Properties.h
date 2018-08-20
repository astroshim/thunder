#ifndef __PROPERTIES_H
#define __PROPERTIES_H

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include <string.h>
using namespace std;

const unsigned int MAX_LINE_SIZE  = 1024;
const unsigned int MAX_KEY_SIZE   = 512;
const unsigned int MAX_VALUE_SIZE   = 512;

/**
//. 코드의 입력과 가독성을 돕기 위한 typedef들
typedef map<int, string> isMap;
typedef isMap::value_type mapValType;
typedef isMap::iterator isMapItor;
isMap c;
//. 키-값 쌍들을 삽입
c.insert(mapValType(100, "One Hundered"));
c.insert(mapValType(3, "Three"));
c.insert(mapValType(150, "One Hundred Fifty"));
c.insert(mapValType(99, "Ninety Nine"));
//. 모든 키들과 값들을 출력
for(isMapItor itor = c.begin(); itor != c.end(); ++itor)
cout << "Key = " << (*itor).first << ", Value = " << (*itor).second << endl;
 */

typedef map<string, string> myMap;
typedef myMap::value_type   mapValType;
typedef myMap::iterator   myMapItor;

class Properties
{
  private:
    string  m_strFileName;
    //  map<string, string> m_mapProperties;
    myMap   m_mapProperties;

  public:
    Properties();
    ~Properties();

    /// 주어진 파일명으로 propertiy list를 읽는다.
    int Load(char *_pchileName);

    /// m_mapProperties 변수에 값을 등록한다.
    void SetProperty(string _strKey, string _strValue);

    /// map에 있는 변수를 property 파일에 등록한다.
    void Flush();

    /// key로 주어진 value를 얻는다.
    string GetProperty(string _strKey);

  private:
    /// 문자열에서 공백을 없앤다.
    void ClsWSpace(char *str);

    /// 주어진 map 변수에 에 key=value 로 삽입.
    //  void AddMap(char *_pchLine, map<string,string> &_mapConfig);
    void AddMap(char *_pchLine, myMap &_mapConfig);

    /// m_mapProperties 에 key=value 로 삽입.
    void AddMap(char *_pchLine);
};

#endif


