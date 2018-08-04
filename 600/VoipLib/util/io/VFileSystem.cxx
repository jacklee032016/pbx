/*
 * $Id: VFileSystem.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#ifdef __APPLE__
#include <stdint.h>
#endif
/* Change: Apple Mac OS X Support
   types.h needs to be included before dirent.h */
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include <unistd.h>
#include <utime.h>

#include "VFileSystem.hxx"
#include "VIoException.hxx"


string
VFileSystem::readFile( const string& fileName )
throw(VException&)
{
    FILE* file = fopen( fileName.c_str(), "rb" );
    if ( !file )
    {
        throw VIoException( strerror(errno),
                            __FILE__,
                            __LINE__,
                            errno );
    }

    const int bufSize = 4096;
    char buf[4096];
    string ret;

    while ( !feof(file) )
    {
        int s = fread( buf, sizeof(char), bufSize, file );

        if ( ferror(file) )
        {
            throw VIoException( strerror(errno),
                                __FILE__,
                                __LINE__,
                                errno );
        }

        ret.append(buf, s);
    }

    int err = fclose( file );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return ret;
}


void
VFileSystem::writeFile( const string& fileName, const string& data )
throw(VException&)
{
    FILE* file = fopen( fileName.c_str(), "wb" );
    if ( !file )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    const char* buf = data.c_str();
    unsigned int len = data.length();

    while ( len > 0 )
    {
        int e = fwrite( buf, sizeof(char), len, file );
        buf += e;
        len -= e;

        if ( ferror(file) )
        {
            throw VIoException( strerror(errno),
                                __FILE__ ,
                                __LINE__ ,
                                errno );
        }
    }

    int err = fclose( file );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }
}


void
VFileSystem::setFileTime( const string& fileName, TimeStamp time )
throw(VException&)
{
    struct utimbuf buf;
    buf.actime = time;
    buf.modtime = time;
    int err = utime(fileName.c_str(), &buf);
    if (err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__
                            , __LINE__ ,
                            errno );
    }
}


TimeStamp
VFileSystem::getFileTime( const string& fileName)
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    time_t t = buf.st_mtime;
    TimeStamp ret = t;

    return ret;
}


int
VFileSystem::getFileSize( const string& fileName)
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return buf.st_size;
}


void
VFileSystem::removeFile( const string& fileName )
throw(VException&)
{
    int err = unlink(fileName.c_str());
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }
}


bool
VFileSystem::fileExists( const string& fileName )
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EBADF:
            case ENOENT:
            case ENOTDIR:
            return false;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__,
                                    __LINE__,
                                    errno );
            }
        }
    }

    int isFile = S_ISREG(buf.st_mode);

    return isFile;
}


void
VFileSystem::createDir( const string& dirName )
throw(VException&)
{
    mode_t mode = 0777;
    int err = mkdir( dirName.c_str(), mode );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EEXIST:
            break;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__ ,
                                    __LINE__ ,
                                    errno );
            }
        }
    }
}


void VFileSystem::removeDir( const string& dirName )
throw(VException&)
{
    int err = rmdir(dirName.c_str());
    if ( err != 0 )
    {
        switch ( errno )
        {
            default:
            {
                throw VIoException(strerror(errno)
                                   , __FILE__ ,
                                   __LINE__
                                   , errno );
            }
        }
    }
}


bool
VFileSystem::dirExists( const string& dirName )
throw(VException&)
{
    struct stat buf;
    int err = stat( dirName.c_str(), &buf );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EBADF:
            case ENOENT:
            case ENOTDIR:
            return false;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__ ,
                                    __LINE__ ,
                                    errno );
            }
        }
    }

    int isDir = S_ISDIR(buf.st_mode);

    return isDir;
}


StringList
VFileSystem::readDir( const string& dirName)
throw(VException&)
{
    StringList list;

    DIR* dir = opendir(dirName.c_str());
    if ( dir == NULL )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    struct dirent * dirp = readdir(dir);
    dirp = readdir(dir);  // eat the "."
    dirp = readdir(dir);  // eat the ".."
    while ( dirp )
    {
        list.push_back( string(dirp->d_name) );
        dirp = readdir(dir);
    }

    int err = closedir(dir);
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return list;
}
