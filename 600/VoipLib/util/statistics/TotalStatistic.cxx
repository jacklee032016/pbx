/*
 * $Id: TotalStatistic.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "TotalStatistic.hxx"
#include <cassert>


using Vocal::Statistics::TotalStatistic;
using Vocal::Statistics::Statistic;
using Vocal::Statistics::StatisticsDb;


TotalStatistic::TotalStatistic(StatisticsDb & p_db, const Data & p_name)
    :	Statistic(p_db, p_name),
    	m_total(0)
{
}


TotalStatistic::TotalStatistic(const TotalStatistic & src)
    :	Statistic(src),
    	m_total(src.m_total)
{
}


const TotalStatistic & 
TotalStatistic::operator=(const TotalStatistic & src)
{
    if ( this != &src )
    {
    	Statistic::operator=(src);
	
	m_total = src.m_total;
    }
    return ( *this );
}


TotalStatistic::~TotalStatistic()
{
}


void
TotalStatistic::combine(const Statistic & src)
{
    if ( key() != src.key() )
    {
    	assert( key() == src.key() );
	return;
    }

    const TotalStatistic *  srcStat 
    	= dynamic_cast<const TotalStatistic *>(&src);

    if ( srcStat == 0 )
    {
    	assert( srcStat != 0 );
	return;
    }
    
    m_total += srcStat->m_total;
    
    return;
}


Statistic *
TotalStatistic::copy() const
{
    return ( new TotalStatistic(*this) );
}


void
TotalStatistic::set(u_int32_t total)
{
    m_total = total;
    record();
}


const TotalStatistic &    
TotalStatistic::operator++()
{
    m_total = 1;
    record();
    
    return ( *this );
}


const TotalStatistic &    
TotalStatistic::operator++(int)
{
    m_total = 1;
    record();

    return ( *this );
}


const TotalStatistic &    
TotalStatistic::operator+=(u_int32_t inc)
{
    m_total = inc;
    record();

    return ( *this );
}


u_int32_t   
TotalStatistic::get() const
{
    return ( m_total );
}


ostream & 
TotalStatistic::writeTo(ostream & out) const
{
    return ( out << name().logData() << ": " << m_total );
}

