/*
* $Id: Statistic.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "Statistic.hxx"
#include "Data.hxx"
#include <cassert>


using Assist::Statistics::Statistic;
using Assist::Statistics::StatisticsDb;
using Assist::Statistics::StatisticsDbKey;


Statistic::Statistic(StatisticsDb & p_db, const Data & p_name)
    :	myDb(&p_db),
    	myKey(INVALID_STAT_DB_KEY)
{
    myKey = myDb->key(p_name);

    assert( myDb != 0 );
    assert( myKey != INVALID_STAT_DB_KEY );
}


Statistic::Statistic(StatisticsDb & p_db, StatisticsDbKey p_key)
    :	myDb(&p_db),
    	myKey(p_key)
{    
    assert( myDb != 0 );
    
    StatisticsDbKey nextKey = myDb->nextKey();
    
    if ( myKey >= nextKey )
    {
        assert( myKey < nextKey );
        myKey = INVALID_STAT_DB_KEY;
    }

    assert( myKey != INVALID_STAT_DB_KEY );
}


Statistic::Statistic(const Statistic & src)
    :	myDb(src.myDb),
    	myKey(src.myKey)
{
}


const Statistic &
Statistic::operator=(const Statistic & src)
{
    if ( this != &src )
    {
    	myDb = src.myDb;
	myKey = src.myKey;
    }
    return ( *this );
}


Statistic::~Statistic()
{
}


void	    	    	    	    
Statistic::record()
{
    myDb->record(*this);
}


StatisticsDbKey
Statistic::key() const
{
    return ( myKey );
}


const Data &
Statistic::name() const
{
    return ( myDb->keyName(myKey) );
}


const StatisticsDb &	
Statistic::db() const
{
    return ( *myDb );
}
