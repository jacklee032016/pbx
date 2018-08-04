/*
* $Id: Event.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Event.hxx"


using Assist::Services::Event;


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
