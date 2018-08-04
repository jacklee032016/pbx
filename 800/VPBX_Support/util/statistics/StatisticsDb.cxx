/*
* $Id: StatisticsDb.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "StatisticsDb.hxx"
#include "Statistic.hxx"
#include "Lock.hxx"
#include <cassert>


using Assist::Statistics::StatisticsDb;
using Assist::Statistics::StatisticsDbKey;
using Assist::Statistics::Statistic;
using Assist::Threads::WriteLock;
using Assist::Threads::ReadLock;


const StatisticsDbKey  Assist::Statistics::INVALID_STAT_DB_KEY = 0;


StatisticsDb::StatisticsDb(const char * name)
    :	myName(name ? name : "StatisticsDb"),
    	myNextKey(INVALID_STAT_DB_KEY+1)
{
    myStatNames.push_back(Data("INVALID_STAT_DB_KEY"));
}


StatisticsDb::~StatisticsDb()
{
    clear();
}


StatisticsDbKey
StatisticsDb::key(const Data & key)
{
    WriteLock    lock(myLock);
    
    StatisticsDbKey   result = INVALID_STAT_DB_KEY;

    StatisticRegistrations::iterator	regIter;

    regIter = myStatRegistrations.find(key);
    
    if ( regIter == myStatRegistrations.end() )
    {
    	result = myStatRegistrations[key] = myNextKey++;
	myStatNames.push_back(key);
	
	assert( myStatNames.size() == myNextKey );
    }
    else
    {
    	result = regIter->second;
    }
    
    return ( result );
}


const Data &	    	
StatisticsDb::keyName(StatisticsDbKey index) const
{
    ReadLock  lock(myLock);
    
    if ( index > myStatNames.size() )
    {
    	assert( index <= myStatNames.size() );

	index = INVALID_STAT_DB_KEY;
    }
    
    return ( myStatNames[index] );
}


StatisticsDbKey
StatisticsDb::nextKey() const
{
    return ( myNextKey );
}

void	
StatisticsDb::record(const Statistic & statistic)
{
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return;
    }

    WriteLock    lock(myLock);
    
    StatisticsDbKey key = statistic.key();
    
    Database::iterator	dbIter = m_db.find(key);
    
    if ( dbIter == m_db.end() )
    {
    	// Memory allocation
	//
    	m_db[key] = statistic.copy();	
    }
    else
    {
    	dbIter->second->combine(statistic);
    }
}


Statistic *
StatisticsDb::find(const Statistic & statistic) const
{
    Statistic * result = 0;
    
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return ( result );
    }

    ReadLock    lock(myLock);

    Database::const_iterator	dbIter = m_db.find(statistic.key());
    
    if ( dbIter != m_db.end() )
    {
    	result = dbIter->second;
    }
    
    return ( result );
}


void	
StatisticsDb::erase(const Statistic & statistic)
{
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return;
    }

    WriteLock    lock(myLock);

    Database::iterator	dbIter = m_db.find(statistic.key());
    
    if ( dbIter != m_db.end() )
    {
    	// Memory freed
	//
    	delete dbIter->second;
    	m_db.erase(dbIter);
    }
    
    return;
}


void	
StatisticsDb::clear()
{
    WriteLock    lock(myLock);

    for (   Database::iterator dbIter = m_db.begin();
    	    dbIter != m_db.end();
	    dbIter++
	)
    {
    	// Memory freed
	//
    	delete dbIter->second;
    }

    m_db.clear();
}


ostream &   
StatisticsDb::writeTo(ostream & out) const
{
    ReadLock    lock(myLock);

    out << '\n' << myName.logData() << '\n';
    
    for (   Database::const_iterator dbIter = m_db.begin();
    	    dbIter != m_db.end();
	    dbIter++
	)
    {
    	out << *(dbIter->second) << '\n';
    }
    
    return ( out );
}


const Data &	
StatisticsDb::name() const
{
    return ( myName );
}


bool 
StatisticsDb::operator==(const StatisticsDb & src) const
{
    return ( this == &src );
}


bool 
StatisticsDb::operator<(const StatisticsDb & src) const
{
    return ( this < &src );
}
