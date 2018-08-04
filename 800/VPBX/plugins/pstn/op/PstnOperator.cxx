/*
 * $Id: PstnOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOperator.hxx"
#include "PbxEvent.hxx"
#include "CallEvent.hxx"
#include "CallId.hxx"
#include "DigitTimeoutEvent.hxx"
#include "EpTimerEvent.hxx"

using namespace Assist;

Sptr < Fifo < Sptr <PbxEvent> > >PstnOperator::myBackFifo = 0;

PstnOperator::PstnOperator(const string& name)
	:EpOperator(name)
{
}

PstnOperator::~PstnOperator()
{}

Sptr <PstnEvent> PstnOperator::isPstnEvent( const Sptr <PbxEvent> event )
{
	assert(event!=0);
	doCpLog(event);

	Sptr <PstnEvent> pstnEvent;
	pstnEvent.dynamicCast( event );
	if ( pstnEvent == 0 )
	{
		return 0;
	}

	return pstnEvent;
}

DeviceEventType  PstnOperator::getPstnEventType(Sptr <PbxEvent> event )
{
	DeviceEventType eventType = DeviceEventUndefined;

	Sptr <PstnEvent> pstnEvent = isPstnEvent(event);
	if(pstnEvent!=0)
		eventType = pstnEvent->type;

	return eventType;
}

void PstnOperator::sendCommand(Sptr <PbxEvent> event, DeviceSignalType cmd)
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	
	Sptr <PbxHwEvent> signal = new PbxHwEvent();
	signal->type = HardwareSignalType;
	signal->signalOrRequest.signal = cmd;
	
	signal->setEndPoint(ep);
	cpLog(LOG_DEBUG, "PstnStateMachine :  %s report a PbxHwEvent", name());

	sendDeviceCommand(signal);
}

