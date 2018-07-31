#ifndef DATASTORE_HXX
#define DATASTORE_HXX
/*
* $Id: DataStore.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VFileSystem.hxx"


/// Class that specifies access signature foe abstract data storage
class DataStore
{
    protected:

    ///
    DataStore( int aproxNumUsersP );


    public:

    ///
    virtual ~DataStore();
    
    
    /// get the specified item
    virtual string
    getItem( const string& group, const string& name )
        throw(VException&) = 0;
    
    /// get the last modification time for the specfied item
    virtual TimeStamp
    getItemTimeStamp( const string& group, const string& name )
        throw(VException&) = 0;
    
    /// get the size in bytes of the data in the specified item
    virtual int
    getItemSize( const string& group, const string& name )
        throw(VException&) = 0;
    
    /**
     * store the data into the specified item, 
     * it time is not specifeid, current time is used
     */
    virtual void
    putItem( const string& group,
             const string& name,
             const string& data,
             TimeStamp timeStamp = 0 )
        throw(VException&) = 0;
    
    /// Remove the specified item
    virtual void
    removeItem( const string& group, const string& name)
        throw(VException&) = 0;
    
    /// Return true if the specified item exists
    virtual bool
    isItem( const string& group, const string& name)
        throw(VException&) = 0;
    
    /// get list of all items in this group
    virtual StringList
    listItems(const string& group)
        throw(VException&) = 0;
    
    /// add a new group
    virtual void
    addGroup( const string& group )
        throw(VException&) = 0;
    
    /// remove a group
    virtual void
    removeGroup( const string& group)
        throw(VException&) = 0;
    
    /// return true if the group exists
    virtual bool
    isGroup( const string& group) = 0;
    
    /// get a list of all groups
    virtual StringList
    listGroups() = 0;
    
protected:
    /// best gueses on total number of users
    const int aproxNumUsers;

};

#endif
