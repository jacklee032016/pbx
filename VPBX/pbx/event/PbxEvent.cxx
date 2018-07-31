/*
 * $Id: PbxEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "PbxEvent.hxx"
#include "EndPoint.hxx"
#include "AudioChannel.hxx"
#include "CallId.hxx"

using namespace Assist;

PbxEvent::PbxEvent()
    :	myFifo(0),
	myId(0),
	myEndPoint(0),
	myObject(0)
{
}


PbxEvent::PbxEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo)
   :   myFifo(fifo),
       myId(0),
	myEndPoint(0)
{
}

PbxEvent::~PbxEvent()
{
	cpLog(LOG_DEBUG, "PbxEvent deconstructing...");
}

/* post a new event with myQueue */
void PbxEvent::postEvent(const Sptr <PbxEvent> newEvent)
{
	assert( newEvent != 0 );
	cpLog(LOG_DEBUG, "FIFO");
	assert( myFifo != 0 );

	myFifo->add(newEvent);
}

void PbxEvent::postEvent(const Sptr <PbxEvent> newEvent,  const Sptr < Fifo < Sptr <PbxEvent> > > newFifo ) const
{
	assert( newEvent != 0 );
	assert( newFifo != 0 );
	newFifo->add(newEvent);
}

Sptr < Fifo < Sptr <PbxEvent> > > PbxEvent::getFifo( ) const
{
	return ( myFifo );
}

void PbxEvent::setEndPoint(Sptr <EndPoint> _ep)
{
	myEndPoint = _ep;
}

Sptr <EndPoint> PbxEvent::getEndPoint()
{
	return myEndPoint;
}

void PbxEvent::setPrivate( void *_priv)
{
	priv = _priv;
}

void *PbxEvent::getPrivate( )
{
	return priv;
}

Sptr <PbxStateObject> PbxEvent::getStateObject()
{
	return myObject;
}


void PbxEvent::setStateObject(Sptr <PbxStateObject> _obj)
{
	myObject = _obj;
}

/* used only when reportEvent of queue*/
Sptr < Fifo < Sptr <PbxEvent> > > PbxEvent::getQueue() const
{
	return myFifo;
}

/* used only when emit a timerout event */
void PbxEvent::setQueue(Sptr < Fifo < Sptr <PbxEvent> > > _queue)
{
	myFifo = _queue;
}

