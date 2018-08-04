/*
* $Id: PerfTotal.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "PerfTotal.hxx"
#include "PerformanceDb.hxx"


using Assist::Statistics::PerfTotal;
using Assist::Statistics::TotalStatistic;
using Assist::Statistics::PerformanceDb;
using Assist::Statistics::Statistic;


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
