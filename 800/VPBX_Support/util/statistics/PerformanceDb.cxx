/*
* $Id: PerformanceDb.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "PerformanceDb.hxx"
#include "PerfTotal.hxx"
#include "PerfDuration.hxx"


using Assist::Statistics::PerformanceDb;
using Assist::Statistics::PerfTotal;
using Assist::Statistics::PerfDuration;



PerformanceDb *	PerformanceDb::m_instance = 0;


PerformanceDb::PerformanceDb()
    :	StatisticsDb("PerformanceDb")
{
}


PerformanceDb & 
PerformanceDb::instance()
{
    if ( m_instance == 0 )
    {	
    	m_instance = new PerformanceDb;
    }
    return ( *m_instance );
}


PerformanceDb::~PerformanceDb()
{
}


PerfDuration * 
PerformanceDb::find(const PerfDuration & stat) const
{
    return ( dynamic_cast<PerfDuration *>(StatisticsDb::find(stat)) );
}


PerfTotal * 
PerformanceDb::find(const PerfTotal & stat) const
{
    return ( dynamic_cast<PerfTotal *>(StatisticsDb::find(stat)) );
}
