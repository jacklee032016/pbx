/*
 * $Id: PerfTotal.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "PerfTotal.hxx"
#include "PerformanceDb.hxx"


using Vocal::Statistics::PerfTotal;
using Vocal::Statistics::TotalStatistic;
using Vocal::Statistics::PerformanceDb;
using Vocal::Statistics::Statistic;


PerfTotal::PerfTotal(const Data & p_label)
    :	TotalStatistic(PerformanceDb::instance(), p_label)
{
}


PerfTotal::PerfTotal(const PerfTotal & src)
    :	TotalStatistic(src)
{
}


const PerfTotal & 
PerfTotal::operator=(const PerfTotal & src)
{
    TotalStatistic::operator=(src);
    
    return ( *this );
}


PerfTotal::~PerfTotal()
{
}


Statistic * 	
PerfTotal::copy() const
{	
    return ( new PerfTotal(*this) );
};
