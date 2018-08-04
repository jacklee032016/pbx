/*
* $Id: ControlEvent.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ControlEvent.hxx"


using namespace Assist;
using Assist::Services::ControlEvent;
using Assist::Services::Event;
using Assist::Threads::Mutex;

Mutex	ControlEvent::myNextIdMutex;


int ControlEvent::myNextId = 0;


ControlEvent::ControlEvent(
    Type pType,
    FifoBase < Sptr < Event > > * pFifo
)
    :	myType(pType),
    	myReturnCode(-1),
    	myFifo(pFifo),
    	myId(0)
{
    myNextIdMutex.lock();
    myId = ++myNextId;
    myNextIdMutex.unlock();
}


ControlEvent::ControlEvent(
    Type    	    	    	    pType,
    FifoBase < Sptr < Event > > &   pFifo
)
    :	myType(pType),
    	myReturnCode(-1),
    	myFifo(&pFifo),
    	myId(0)
{
    myNextIdMutex.lock();
    myId = ++myNextId;
    myNextIdMutex.unlock();
}


ControlEvent::~ControlEvent()
{
}



const char* 
ControlEvent::name() const
{
    return ( "ControlEvent" );
}


ControlEvent::Type
ControlEvent::getType() const
{
    return ( myType );
}


void
ControlEvent::setReturnCode(int rc)
{
    myReturnCode = rc;
}


int
ControlEvent::getReturnCode() const
{
    return ( myReturnCode );
}


bool    
ControlEvent::isRequest() const
{
    return ( myType <= MAX_REQUEST );
}


bool    
ControlEvent::isResponse() const
{
    return ( MIN_RESPONSE <= myType && myType <= MAX_RESPONSE );
}


void    
ControlEvent::response()
{
    if ( isRequest() )
    {
        // *sigh*
        //
        myType = static_cast<Type>(
                    static_cast<int>(myType) + static_cast<int>(MIN_RESPONSE)
                );
    }
    assert( isResponse() );
}
        

void
ControlEvent::setReplyFifo(
    FifoBase< Sptr<Event> > & fifo
)
{
    myFifo = &fifo;
}


FifoBase< Sptr<Event> > *
ControlEvent::getReplyFifo() const
{
    return ( myFifo );
}


int
ControlEvent::getControlEventId() const
{
    return ( myId );
}


ostream &
ControlEvent::writeTo(ostream & out) const
{
    size_t  type = static_cast<size_t>(myType);
    size_t  nameIndex = ( type < TOTAL_TYPES ? type : TOTAL_TYPES );

    char *	controlEventName[TOTAL_TYPES+1] =
    {
    	"Start",
        "Stop",
        "Shutdown",
        "Start response",
        "Stop response",
        "Shutdown response",
        "Unknown"
    };

    return ( out << name() << ": " << controlEventName[nameIndex] );
}
