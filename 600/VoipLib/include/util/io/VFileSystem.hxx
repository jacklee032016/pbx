#ifndef VFILESYSTEM_HXX
#define VFILESYSTEM_HXX
/*
 * $Id: VFileSystem.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include <list>
#include <string>

#include "VException.hxx"

/// List of strings
typedef list < string > StringList;

/// TimeStame used for setting file modification data
typedef unsigned long TimeStamp;


/// Class to abstact file system operations
class VFileSystem
{
    public:
        /// read in a file
        static string
        readFile( const string& fileName )
        throw(VException&);

        /// Write data to the given file
        static void
        writeFile( const string& fileName, const string& data )
        throw(VException&);

        /// Set the time stamp on a file
        static void
        setFileTime( const string& fileName, TimeStamp time )
        throw(VException&);

        /// Get the time stamp of when a file was last modified
        static TimeStamp
        getFileTime( const string& fileName)
        throw(VException&);

        /// Get the size of a file
        static int
        getFileSize( const string& fileName)
        throw(VException&);

        /// Remove a file
        static void
        removeFile( const string& fileName )
        throw(VException&);

        /// Check if a file exists
        static bool
        fileExists( const string& fileName )
        throw(VException&);

        /// Create a directory
        static void
        createDir( const string& dirName )
        throw(VException&);

        /// Remove a directory
        static void
        removeDir( const string& dirName )
        throw(VException&);

        /// Check if a directory exists
        static bool
        dirExists( const string& dirName )
        throw(VException&);

        /// Find all the entires in a dirctory (ignores . and ..)
        static StringList
        readDir( const string& dirName)
        throw(VException&);
};

#endif
