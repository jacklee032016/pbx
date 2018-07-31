/*
 * $Id: IsdnOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOperator.hxx"

using namespace Assist;

IsdnOperator::IsdnOperator(const string& name)
	:EpOperator(name)
{
}

IsdnOperator::~IsdnOperator()
{
}

Sptr <IsdnMsg> IsdnOperator::isIsdnMsg( const Sptr <PbxEvent> event )
{
	assert(event!=0);
	doCpLog(event);

	Sptr <IsdnEvent> pbxMsg;
	pbxMsg.dynamicCast( event );
	if ( pbxMsg == 0 )
	{
		return 0;
	}

	Sptr <IsdnMsg> isdnMsg = pbxMsg->getIsdnMsg();
	assert( isdnMsg != 0 );

	cpLog(LOG_DEBUG, "ISDN msg : %s", isdnMsg->getName() );
	return isdnMsg;
}

ISDN_METHOD IsdnOperator::checkIsdnMsgType( const Sptr <PbxEvent> event )
{
	Sptr <IsdnMsg> isdnMsg = isIsdnMsg(event);
	if( isdnMsg == 0 )
	{
		return ISDN_TYPE_UNKNOWN;
	}

	return isdnMsg->getType();
}

IsdnCtrlDevice *IsdnOperator::getIsdnCtrlDevice( const Sptr <PbxEvent> event )
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);

	Sptr <IsdnCtrlDevice> ctrl = NULL;
	ctrl.dynamicCast( ep->getCtrlDevice() );
	assert(ctrl != 0);

	return ctrl.getPtr();
}

