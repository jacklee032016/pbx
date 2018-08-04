#ifndef FILEDATASTORE_HXX
#define FILEDATASTORE_HXX
/*
 * $Id: FileDataStore.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "DataStore.hxx"
#include "VRWLock.hxx"

/// Class that store data in file system
class FileDataStore : public DataStore
{
    public:
        ///
        FileDataStore( int numBinsP, const string &fileRootP );

        ///
        virtual ~FileDataStore();

        /// get the specified item
        virtual string
        getItem( const string& group, const string& name )
        throw(VException&);

        /// get the last modification time for the specfied item
        virtual TimeStamp
        getItemTimeStamp( const string& group, const string& name )
        throw(VException&);

        /// get the size in bytes of the data in the specified item
        virtual int
        getItemSize( const string& group, const string& name )
        throw(VException&);

        /**
         * store the data into the specified item, 
         * it time is not specifeid, current time is used
         */
        virtual void
        putItem( const string& group,
                 const string& name,
                 const string& data,
                 TimeStamp timeStamp = 0 )
        throw(VException&);

        /// Remove the specified item
        virtual void
        removeItem( const string& group, const string& name)
        throw(VException&);

        /// Return true if the specified item exists
        virtual bool
        isItem( const string& group, const string& name)
        throw(VException&);

        /// get list of all items in this group
        virtual StringList
        listItems(const string& group)
        throw(VException&);

        /// add a new group
        virtual void
        addGroup( const string& group )
        throw(VException&);

        /// remove a group
        virtual void
        removeGroup( const string& group)
        throw(VException&);

        /// return true if the group exists
        virtual bool
        isGroup( const string& group)
        throw(VException&);

        /// get a list of all groups
        virtual StringList
        listGroups();

    protected:

    private:

        /// get the bin name for a particular bin
        string
        getBin( int i ) const;

        /// get the bin name for a given item
        string
        getBin( const string& group, const string& name ) const;

        /// get the full path for the root of the data store
        string
        rootName( int bin ) const;

        /// get the full path directoy name for a given group
        string
        dirName(const string& group, const string& name ) const;

        /// get the full path directoy name for a given group
        string
        dirName(const string& group, int bin ) const;

        /// get the full path file name for a given item
        string
        fileName(const string& group, const string& name ) const;

        /// take stuff and return number from 0 to numBins-1
        int
        hash(const string& group, const string& name ) const;

        /// path to root of file system in use
        string fileRoot;

        /// number of directory bins currently in use
        int numBins;

        /// lock to block concurent file access
        VRWLock lock;
};

#endif
