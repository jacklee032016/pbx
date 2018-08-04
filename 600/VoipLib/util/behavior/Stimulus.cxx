/*
 * $Id: Stimulus.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "Stimulus.hxx"


using Vocal::Behavioral::Stimulus;
using Vocal::Signaling::Event;


Stimulus::Stimulus(
    u_int32_t	    id,
    const char *    name
)
    :	myId(id),
	myName(name ? name : "unknown"),
    	myEvent(0)
{
}


Stimulus::Stimulus(const Stimulus & src)
    :	myId(src.myId),
	myName(src.myName),
    	myEvent(src.myEvent)
{
}


Stimulus::~Stimulus()
{
}


Stimulus &  	
Stimulus::operator=(const Stimulus & src)
{
    if ( this != &src )
    {
    	myId 	= src.myId;
	myName 	= src.myName;
	myEvent  = src.myEvent;
    }
    return ( *this );
}


bool	    	
Stimulus::operator==(const Stimulus & src) const
{
    return ( myId == src.myId );
}


bool	    	
Stimulus::operator<(const Stimulus & src) const
{
    return ( myId < src.myId );
}


void	    	
Stimulus::event(Sptr<Event> evt)
{
    myEvent = evt;
}


Sptr<Event> 	
Stimulus::event() const
{
    return ( myEvent );
}


ostream &       
Stimulus::writeTo(ostream & out) const
{
    return ( out << myName );
}
