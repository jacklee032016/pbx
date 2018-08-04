/*
 * $Id: FileDataStore.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include <cassert>
#include <cerrno>
#include <cstdio>

#include "FileDataStore.hxx"
#include "VIoException.hxx"


FileDataStore::FileDataStore( int numBinsP, const string &fileRootP )
        : DataStore(numBinsP * 200),
        fileRoot(fileRootP),
        numBins(numBinsP)
{
    assert( numBins < 1000 );
    assert( numBins >= 0 );
    assert( fileRoot.length() > 0 );

    lock.WriteLock();
    try
    {
        // check the root file system exists
        if ( !VFileSystem::dirExists( fileRoot ) )
        {
            throw VIoException( strerror(errno),
                                __FILE__,
                                __LINE__,
                                errno );
        }

        // check all the groups dirs are created
        for ( int bin = 0; bin < numBins; bin++ )
        {
            string dir = rootName( bin );
            if ( !VFileSystem::dirExists( dir ) )
            {
                VFileSystem::createDir( dir );
            }
        }
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();
}


FileDataStore::~FileDataStore()
{}



string
FileDataStore::getBin( int i ) const
{
    // This turns i into a 3 digit base 16 number and
    // is prepresented with a A for 0, B for 1 ...
    assert( i < 16*16*16 );
    assert( i >= 0 );

    int i3 = i % 16;
    i = i / 16;
    int i2 = i % 16;
    i = i / 16;
    int i1 = i % 16;
    i = i / 16;

    char c[4];
    c[0] = 'A' + i1;
    c[1] = 'A' + i2;
    c[2] = 'A' + i3;
    c[3] = 0;

    string res(c);

    return res;
}


string
FileDataStore::getBin( const string& group, const string& name ) const
{
    int h = hash( group, name );
    return getBin( h );
}


string
FileDataStore::rootName( int bin ) const
{
    string ret = fileRoot;

    if (numBins != 0)
    {
        ret += "/";
        ret += getBin(bin);
    }
    return ret;
}


string
FileDataStore::dirName(const string& group, const string& name ) const
{
    return dirName( group, hash(group, name) );
}


string
FileDataStore::dirName(const string& group, int bin ) const
{
    string ret = rootName( bin );
    ret += "/";

    string g = group;

    if ( numBins != 0 )
    {
        // flatten the group name - replace / with #
        for (unsigned int i = 0; i < g.length(); i++)
        {
            if ( g[i] == '/' )
            {
                g[i] = '#';
            }
        }
    }

    ret += g;
    return ret;
}


string
FileDataStore::fileName(const string& group, const string& name ) const
{
    string ret = dirName(group, name);
    ret += "/";
    ret += name;
    ret += ".xml";
    return ret;
}


int
FileDataStore::hash(const string& group, const string& name ) const
{
    // There are no doubt better hashes but hey this looks like it will work
    int h1 = 0;
    int h2 = 0;

    for ( unsigned int i = 0; i < name.length(); i++ )
    {
        h1 += name[i];
        h2 += (h2 << 4) + name[i];
    }

    int h = (abs(h1 + h2)) % numBins;

    assert( h >= 0 );
    assert( h < numBins );

    return h;
}


string
FileDataStore::getItem( const string& group, const string& name )
throw(VException&)
{
    string ret;
    lock.ReadLock();
    try
    {
        ret = VFileSystem::readFile( fileName(group, name) );
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}


TimeStamp
FileDataStore::getItemTimeStamp( const string& group, const string& name )
throw(VException&)
{
    TimeStamp ret;

    lock.ReadLock();
    try
    {
        ret = VFileSystem::getFileTime( fileName(group, name) );
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}

int
FileDataStore::getItemSize( const string& group, const string& name )
throw(VException&)
{
    int ret;

    lock.ReadLock();
    try
    {
        ret = VFileSystem::getFileSize( fileName(group, name) );
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}


void
FileDataStore::putItem( const string& group,
                        const string& name,
                        const string& data,
                        TimeStamp timeStamp)
throw(VException&)
    // timestamp defaults to 0
{
    if (!isGroup(group))
    {
        addGroup(group);
    }
    string fName( fileName(group, name) );
    lock.WriteLock();
    try
    {
        VFileSystem::writeFile( fName, data );

        if (timeStamp != 0 )
        {
            VFileSystem::setFileTime( fName, timeStamp );
        }

    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();
}


void
FileDataStore::removeItem( const string& group, const string& name)
throw(VException&)
{
    lock.WriteLock();
    try
    {
        VFileSystem::removeFile( fileName(group, name) );
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();
}


bool
FileDataStore::isItem( const string& group, const string& name)
throw(VException&)
{
    bool ret;
    lock.ReadLock();
    try
    {
        ret = VFileSystem::fileExists( fileName(group, name) );
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}


StringList
FileDataStore::listItems(const string& group)
throw(VException&)
{
    StringList list;

    lock.ReadLock();
    try
    {

        for ( int bin = 0; bin < numBins; bin++ )
        {
            StringList dirList( VFileSystem::readDir( dirName(group, bin) ));

            StringList::iterator i = dirList.begin();
            while ( i != dirList.end() )
            {
                string s = *i;
                i++;

                // filter out "hidden" items in the directory
                static string cvs("CVS");
                if ( s == cvs ) continue;

                // strip .xml off end
                size_t location = s.rfind(".xml");
                if (location != string::npos)
                {
                    string t = s.substr(0, location);
                    list.push_back(t);
                }
                else
                {
                    list.push_back(s);
                }
            }
        }
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return list;
}


void
FileDataStore::addGroup( const string& group )
throw(VException&)
{
    lock.WriteLock();
    try
    {

        for ( int bin = 0; bin < numBins; bin++ )
        {
            VFileSystem::createDir( dirName(group, bin) );
        }
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }

    lock.Unlock();
}


void
FileDataStore::removeGroup( const string& group)
throw(VException&)
{
    lock.WriteLock();
    try
    {

        for ( int bin = 0; bin < numBins; bin++ )
        {
            VFileSystem::removeDir( dirName(group, bin) );
        }

    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();
}


bool
FileDataStore::isGroup( const string& group)
throw(VException&)
{
    bool ret;

    lock.ReadLock();
    try
    {

        ret = VFileSystem::dirExists( dirName(group, 0) );

#ifndef NDEBUG

	char err_buff[256];

        // check the groups exists in all bins
        for ( int bin = 1; bin < numBins; bin++ )
        {
            bool v = VFileSystem::dirExists( dirName(group, bin) );
            if ( v != ret )
            {
                sprintf (err_buff, "Error: Number of hash bins has changed: %s",
                         strerror(errno) );
                throw VIoException( err_buff,
                                    __FILE__,
                                    __LINE__,
                                    errno );
                // We have an invalid group directory structure
            }

        }
#endif
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}


StringList
FileDataStore::listGroups()
{
    StringList ret;

    if (numBins == 0 )
    {
        // this is not implemented - left as an exersise to the reader ...
        // it would only be needed to upgrade old system to new
        assert(0);
    }

    lock.ReadLock();
    try
    {
        // loop over all the groups found in bin 0
        StringList dirList( VFileSystem::readDir( rootName(0) ));

        StringList::iterator i = dirList.begin();
        while ( i != dirList.end() )
        {
            string s = *i;
            i++;

            // unflatten group names - replace # with /
            for (unsigned int i = 0; i < s.length(); i++)
            {
                if ( s[i] == '#' )
                {
                    s[i] = '/';
                }
            }

            // filter out "hidden" items in the directory
            static string cvs("CVS");
            if ( s == cvs ) continue;

            ret.push_back(s);
        }
    }
    catch (...)
    {
        lock.Unlock();
        throw;
    }
    lock.Unlock();

    return ret;
}
