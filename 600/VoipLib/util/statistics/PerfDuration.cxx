/*
 * $Id: PerfDuration.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "PerfDuration.hxx"
#include "PerformanceDb.hxx"


using Vocal::Statistics::PerfDuration;
using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::PerformanceDb;
using Vocal::Statistics::Statistic;

 
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
