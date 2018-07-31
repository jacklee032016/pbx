#ifndef VFileBrowser_hxx
#define VFileBrowser_hxx
/*
* $Id: VFileBrowser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <pthread.h>
#include <list>
#include <string>

#ifndef WIN32
#include <utime.h>
#endif

#include "VFile.hxx"
#include "VIoException.hxx"

class VFileBrowser
{
    public:
        typedef list < VFile > DirList;

        ///Constructor initialises to the root directory
        VFileBrowser(const string& root);

        ///Constructor initialises to the root directory
        VFileBrowser(): _root("/")
        { }
        ;

        ///Returns list of files in directory, relative to the root directory
        DirList directory(const string& dirName) throw (VIoException&);

        ///Makes a directory defined by the path, NOP when dir exits
        void mkDir(const string& dirPath) throw (VIoException&);

        ///Writes a file in the given path, relative to _root
        void writeFile(const string& filePath, const string& contents)
        throw (VIoException);

        ///Writes a file in the given path, relative to _root
        // sets modification time to be timestamp
        void writeFile(const string& filePath, const string& contents,
                       time_t timestamp)
        throw (VIoException);

        ///Writes a file in the given path, relative to _root
        // retunrs modification time as timestamp
        void writeFile(const string& filePath, const string& contents,
                       time_t *timestamp)
        throw (VIoException);

        //Atomic write, write the contents to a temporary location first
        //and then move the file to the actual location
        void atomicWrite(const string& fileName, const string& contents);


        ///Checks if a directory exists
        bool dirExists(const string& dirName);

        ///Rename a file
        void rename(const string& oldPath, const string& newPath)
        throw (VIoException&);

        ///Remove a file
        void remove(const string& filePath) throw (VIoException&);

        ///Remove a file/directory and anything under it
        void removeAll(const string& filePath) throw (VIoException&);

        static bool fileExists(const VFile& file);
        static int fileSize(const VFile& file);

    private:

        static int pftw(const char* fName, const struct stat* sb, int flag);
        static int addnode(const char* fName, const struct stat* sb, int flag);

        string _root;
        static DirList _dirList;
        static pthread_mutex_t _lock;

        static list < string > _toDel;

};
#endif
