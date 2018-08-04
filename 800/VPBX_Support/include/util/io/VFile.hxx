#ifndef VFile_hxx
#define VFile_hxx
/*
* $Id: VFile.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "global.h"
#include <string>
//#include <ftw.h>
#define UNKNOWN -1
#include <time.h>

class VFile
{
    public:
        ///Constructor
        VFile(const string& path, const string& fileName);

        ///Constructor
        VFile(const string& fullFilePath);
        VFile(const string& fullFilePath, int flag, time_t time);
        VFile(const VFile& other)
        {
            _path = other._path;
            _fileName = other._fileName;
            _time = other._time;
            _type = other._type;
        }

        VFile& operator=(const VFile& other)
        {
            if (&other != this)
            {
                _path = other._path;
                _fileName = other._fileName;
                _time = other._time;
                _type = other._type;
            }
            return *this;
        }

        const string& getName() const
        {
            return _fileName;
        };
        const string& getPath() const
        {
            return _path;
        };
        const string getFullName() const
        {
            return (_path + _fileName);
        };
        int getType() const
        {
            return _type;
        }
        time_t getTime() const
        {
            return _time;
        }

        bool operator<(const VFile& other)
        {
            if (getFullName() < other.getFullName()) return true;
            return false;
        }

        bool operator==(const VFile& other)
        {
            if (getFullName() == other.getFullName()) return true;
            return false;
        }
    private:
        string _fileName;
        string _path;
        int _type;
        time_t _time;
};

#endif
