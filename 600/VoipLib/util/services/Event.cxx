/*
 * $Id: Event.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "Event.hxx"


using Vocal::Services::Event;


Event::Event()
{
}


Event::~Event()
{
}


ostream &
Event::writeTo(ostream & out) const
{
    return ( out << name() );
}
