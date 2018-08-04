/*
* $Id: VFile.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cstdio>
#include <iostream>

#include "VFile.hxx"

VFile::VFile(const string& path, const string& fileName)
        : _fileName(fileName), _path(path)
{

    _type = UNKNOWN;
    _time = -1;

    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}

VFile::VFile(const string& fullFilePath)
{

    _type = UNKNOWN;
    _time = -1;

    if (fullFilePath.find("/") == string::npos)
    {
        _path = "./";
        _fileName = fullFilePath;
    }
    else
    {
        string tStr = fullFilePath;
        if (tStr[tStr.size() - 1] == '/')
        {
            _path = tStr;
        }
        else
        {
            string::size_type pos = tStr.find_last_of('/');
            _path = tStr.substr(0, pos);
            _fileName = tStr.substr(pos + 1, tStr.size());
        }
    }
    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}

VFile::VFile(const string& fullFilePath, int flag, time_t time)
{

    _type = flag;
    _time = time;

    if (fullFilePath.find("/") == string::npos)
    {
        _path = "./";
        _fileName = fullFilePath;
    }
    else
    {
        string tStr = fullFilePath;
        if (tStr[tStr.size() - 1] == '/')
        {
            _path = tStr;
        }
        else
        {
            string::size_type pos = tStr.find_last_of('/');
            _path = tStr.substr(0, pos);
            _fileName = tStr.substr(pos + 1, tStr.size());
        }
    }
    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}


ostream& operator<<(ostream& os, const VFile& file)
{
    os << file.getFullName();
    return os;
}
