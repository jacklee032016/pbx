/*
* $Id: PstnEndPoint.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxAgent.hxx"
#include "PstnEndPoint.hxx"

using namespace Assist;

PstnEndPoint::PstnEndPoint(  CtrlDevice *_ctrlDev , as_device_t* asdev)
	: EndPoint( _ctrlDev ),
	  myDev(asdev)
{
	setState( findState(PSTN_EP_STATE_IDLE) );

	sprintf(myName, (asdev->phyType==as_fxs_device)? "FXS-EP":"FXO-EP");

	if(asdev->phyType == as_fxs_device )
		isExt = true;

}

PstnEndPoint::~PstnEndPoint(void)
{
	cpLog(LOG_DEBUG, "Destorying.....");
}


unsigned char  PstnEndPoint::reportDtmfDigit( )
{
	return ( myDev->ops->report_dtmf_digit)(myDev);
}

/**
* Send PstnEvent to Gateway Queue. This event consumed by PstnStateMachine
*/
void PstnEndPoint::reportPEvent(DeviceEventType event )
{
	PstnEvent *devEvent = new  PstnEvent();
	devEvent->type = event;

	assert( mySelf != 0 );
	devEvent->setEndPoint( mySelf);
	
	PbxAgent::instance()->getGateway()->reportEvent(devEvent);
}

