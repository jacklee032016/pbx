/*
 * $Id: FileStat.cxx,v 1.3 2007/03/03 18:38:55 lizhijie Exp $
 */

#include "FileStat.hxx"

using Vocal::IO::FileStat;

FileStat::FileStat (const char* filename)
    : myFilename(filename)
{
    reStat();
}


FileStat::FileStat (const Data& filename)
    : myFilename(filename)
{
    reStat();
}


FileStat::~FileStat()
{
}

void
FileStat::reStat()
{
    LocalScopeAllocator lo;
    isValid = (::stat(myFilename.getData(lo), &myStat) == 0);
}


time_t 
FileStat::lastModified() const
{
    if(isValid)
    {
        return myStat.st_mtime;
    }
    else
    {
        return 0; // always bad value
    }
}


bool
FileStat::isFile() const
{
    if(isValid)
    {
        return S_ISREG(myStat.st_mode);
    }
    else
    {
        return false;
    }
}


bool
FileStat::statOK() const
{
    return isValid;
}


