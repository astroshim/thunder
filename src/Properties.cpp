#include "../include/Properties.h"
#include "../include/NPUtil.h"

Properties::Properties() 
{ }

Properties::~Properties() { }

int Properties::Load(char *_pchfStreamName)
{
  char pchLine[MAX_LINE_SIZE];
  m_strFileName = _pchfStreamName;

    fstream fStream(m_strFileName.c_str(), ios::in);    // only read

    if(!fStream.is_open())
  {
        return -1;
  }

    while(fStream.getline(pchLine, MAX_LINE_SIZE))
    {
        /// 
        if(pchLine[0] == '\n' || pchLine[0] == '#' || pchLine[0] == 0x00)
        {
            continue;
        }

        //ClsWSpace(pchLine);
        AddMap(pchLine);
    }

  fStream.close();
  return 0;
}

/**
* add key=value .
*/
void Properties::SetProperty(string _strKey, string _strValue)
{
  m_mapProperties.insert(mapValType(_strKey, _strValue));
}

/**
*/
void Properties::Flush()
{
  fstream fStream(m_strFileName.c_str(), ios::in | ios::out);   // read and write

  if(!fStream.is_open())
  {
    return;
  }

  for(myMapItor itor = m_mapProperties.begin(); 
            itor != m_mapProperties.end(); ++itor)
  {
    char pchLine[MAX_LINE_SIZE];
    memset(pchLine, 0x00, MAX_LINE_SIZE);
    snprintf(pchLine, MAX_LINE_SIZE, "%s=%s\n", (*itor).first.c_str(), (*itor).second.c_str());

//    fStream.write(pchLine, MAX_LINE_SIZE);
    fStream<<pchLine;
  }

    fStream.close();
}

string Properties::GetProperty(string _strKey)
{
    myMapItor itor;
//    map<string, string>::itorator itor;

  itor = m_mapProperties.find(_strKey);
  if(itor != m_mapProperties.end())
  {
    return (*itor).second;  // same return itor->second;
  }
  
  return "";
}

void Properties::ClsWSpace(char *str)
{
  char *start;

  start = str;
  do {
    while (*str == ' ' || *str == '\t') str++;
    *start++=*str;
  } while (*str++);
}

/*
void LTrim(char *str)
{
    char* tmp;
    for(tmp=str; tmp[0] && isspace(tmp[0]); tmp++);
    strcpy(str,tmp);
}
*/

/**
* add key=value in the map.
*/
void Properties::AddMap(char *_pchLine)
{
    char *delimitor;
    char pchName[MAX_KEY_SIZE];
    char pchValue[MAX_VALUE_SIZE];

    if ((delimitor = strchr(_pchLine, '=')) == NULL) return;

    *delimitor = '\0';

    strncpy(pchName, _pchLine, MAX_KEY_SIZE);
    strncpy(pchValue, delimitor+1, MAX_VALUE_SIZE);

    if(!strlen(pchName) || !strlen(pchValue)) return;

    /**
    strName     = pchName;
    strValue    = pchValue;
    */
  CNPUtil::LTrim(pchName);
  CNPUtil::RTrim(pchName);
  CNPUtil::LTrim(pchValue);
  CNPUtil::RTrim(pchValue);
  //LTrim(pchValue);
    m_mapProperties[pchName] = pchValue;

//fprintf(stdout, " [%s] = [%s] \n", pchName, pchValue);
    return;
}

void Properties::AddMap(char *_pchLine, myMap &_mapConfig)
{
  char *delimitor;
  char pchName[MAX_KEY_SIZE];
  char pchValue[MAX_VALUE_SIZE];

  if ((delimitor = strchr(_pchLine, '=')) == NULL) return;

  *delimitor = '\0';

  strncpy(pchName, _pchLine, MAX_KEY_SIZE);
  strncpy(pchValue, delimitor+1, MAX_VALUE_SIZE);

  if(!strlen(pchName) || !strlen(pchValue)) return;
  
  /**
  strName   = pchName;
  strValue  = pchValue;
  */

  _mapConfig[pchName] = pchValue;
  return;
}

