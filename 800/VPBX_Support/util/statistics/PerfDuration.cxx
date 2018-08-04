/*
* $Id: PerfDuration.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "PerfDuration.hxx"
#include "PerformanceDb.hxx"


using Assist::Statistics::PerfDuration;
using Assist::Statistics::DurationStatistic;
using Assist::Statistics::PerformanceDb;
using Assist::Statistics::Statistic;

 
PerfDuration::PerfDuration(const Data & p_label, u_int32_t p_window)
    :	DurationStatistic(PerformanceDb::instance(), p_label, p_window)
{
}


PerfDuration::PerfDuration(const PerfDuration & src) 
    :	DurationStatistic(src)
{
}


const PerfDuration & 
PerfDuration::operator=(const PerfDuration & src)
{
    DurationStatistic::operator=(src);
    return ( *this );
}


PerfDuration::~PerfDuration()
{
}


Statistic * 	
PerfDuration::copy() const
{
    return ( new PerfDuration(*this) );
}
