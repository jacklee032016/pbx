/*
 * $Id: IsdnEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "IsdnEvent.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

IsdnEvent::IsdnEvent()
	:PbxEvent( )
{
}

IsdnEvent::IsdnEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo)
	: PbxEvent( fifo)
{
}

IsdnEvent::~IsdnEvent()
{
	cpLog(LOG_DEBUG, "IsdnEvent deconstructing....." );
}

void IsdnEvent::setIsdnMsg( Sptr <IsdnMsg> _msg)
{
	myIsdnMsg = _msg;
}

Sptr <IsdnMsg> IsdnEvent::getIsdnMsg( )
{
	return myIsdnMsg;
}

Sptr <IsdnCtrlDevice> IsdnEvent::getCtrlDevice()
{
	Sptr <CtrlDevice> ctrl = getEndPoint()->getCtrlDevice();
	Sptr <IsdnCtrlDevice> isdnCtrl = NULL;
	isdnCtrl.dynamicCast(ctrl);
	assert(isdnCtrl !=0);

	return isdnCtrl;
}


