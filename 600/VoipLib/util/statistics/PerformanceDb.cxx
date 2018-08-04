/*
 * $Id: PerformanceDb.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "PerformanceDb.hxx"
#include "PerfTotal.hxx"
#include "PerfDuration.hxx"


using Vocal::Statistics::PerformanceDb;
using Vocal::Statistics::PerfTotal;
using Vocal::Statistics::PerfDuration;



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
