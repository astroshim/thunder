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

const unsigned int MAX_LINE_SIZE 	= 1024;
const unsigned int MAX_KEY_SIZE 	= 512;
const unsigned int MAX_VALUE_SIZE 	= 512;

/**
//. �ڵ��� �Է°� �������� ���� ���� typedef��
typedef map<int, string> isMap;
typedef isMap::value_type mapValType;
typedef isMap::iterator isMapItor;

isMap c;

//. Ű-�� �ֵ��� ����
c.insert(mapValType(100, "One Hundered"));
c.insert(mapValType(3, "Three"));
c.insert(mapValType(150, "One Hundred Fifty"));
c.insert(mapValType(99, "Ninety Nine"));

//. ��� Ű��� ������ ���
for(isMapItor itor = c.begin(); itor != c.end(); ++itor)
	cout << "Key = " << (*itor).first << ", Value = " << (*itor).second << endl;
*/

typedef map<string, string> myMap;
typedef myMap::value_type 	mapValType;
typedef myMap::iterator 	myMapItor;

class Properties
{
private:
	string 	m_strFileName;
//	map<string, string> m_mapProperties;
	myMap 	m_mapProperties;

public:
	Properties();
	~Properties();

	/// �־��� ���ϸ����� propertiy list�� �д´�.
	int Load(char *_pchileName);

	/// m_mapProperties ������ ���� ����Ѵ�.
	void SetProperty(string _strKey, string _strValue);

	/// map�� �ִ� ������ property ���Ͽ� ����Ѵ�.
	void Flush();

	/// key�� �־��� value�� ��´�.
	string GetProperty(string _strKey);

private:
	/// ���ڿ����� ������ ���ش�.
	void ClsWSpace(char *str);

	/// �־��� map ������ �� key=value �� ����.
//	void AddMap(char *_pchLine, map<string,string> &_mapConfig);
	void AddMap(char *_pchLine, myMap &_mapConfig);

	/// m_mapProperties �� key=value �� ����.
	void AddMap(char *_pchLine);
};

#endif

