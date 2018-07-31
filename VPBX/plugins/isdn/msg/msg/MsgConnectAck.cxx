/*
* $Id: MsgConnectAck.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgConnectAck.hxx"

#include "IeSignal.hxx"
#include "IeDisplay.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgConnectAck::MsgConnectAck(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) 
        : IsdnMsg( msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(msg) )
	{
		cpLog(LOG_DEBUG_STACK, "Failed in parse IsdnMsg (prim :0x%x) on device %s", ctrl->getDecoder()->getPrim, ctrl->getName() );
		throw  IsdnMsgParserException("failed in decode ISDN MSG", __FILE__, __LINE__, res );
	}
#endif
	unsigned char *data = myCtrl->getDecoder()->getL3Data( msg);

	try
	{
		decode(data);
	}
	catch (IsdnMsgParserException&)
	{
		if (IsdnParserMode::isdnParserMode())
		{
			throw  IsdnMsgParserException("failed in decode ISDN msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		}
	}
}

MsgConnectAck::MsgConnectAck(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl )
{
}

MsgConnectAck::MsgConnectAck(const MsgConnectAck& src)
        : IsdnMsg(src)
{
}

MsgConnectAck::~MsgConnectAck()
{
}

MsgConnectAck& MsgConnectAck::operator =(const MsgConnectAck& src)
{
	if ( &src != this)
	{
		*static_cast < IsdnMsg* > (this) = src;
		this->ieSignal = src.ieSignal;
		this->ieDisplay = src.ieDisplay;
	}
	return (*this);
}

bool MsgConnectAck::operator ==(const MsgConnectAck& src) 
{
	if(this->ieSignal==src.ieSignal && this->ieDisplay == src.ieDisplay )
	{
			return true;
	}
	return false;
}

ISDN_METHOD MsgConnectAck::getType() const
{
	return ISDN_TYPE_CONNECT_ACK;
}

void MsgConnectAck::setDisplay(Sptr <IeDisplay> display)
{
	ieDisplay = display;
}

Sptr <IeDisplay> MsgConnectAck::getDisplay()
{
	return ieDisplay;
}

void MsgConnectAck::setSignal(Sptr <IeSignal> signal)
{
	ieSignal= signal;
}

Sptr <IeSignal> MsgConnectAck::getSignal()
{
	return ieSignal;
}

int MsgConnectAck::decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
//		p = myCtrl->getDecoder()->getChanId4ConnectAckMsg( iesData);
		if( p != NULL)
			ieDisplay = new IeDisplay( p);
//		p = IsdnMsgFactory::instance()->getTeDecoder()->getConnectPN4ConnectAckMsg( frm);
		if(p != NULL)
			ieSignal = new IeSignal( p);
		
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode CONNECT_ACK msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_DEBUG, "failed in decode optional IEs of CONNECT_ACK msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgConnectAck::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : CONNECT_ACK");
	
	IsdnMsg::debugInfo();

	if(ieDisplay != 0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p);
		free((void *)p);
	}	

	if(ieSignal != 0)
	{
		p = ieSignal->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

}
#endif

int MsgConnectAck::getMsgType() const
{
	return MT_CONNECT_ACKNOWLEDGE;
}

int MsgConnectAck::getCCType() const
{
	return CC_CONNECT_ACKNOWLEDGE;
}

void MsgConnectAck::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4ConnectAckMsg(header, 	ieDisplay->encode( msg, ntMode) );
	}
	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4ConnectAckMsg(header, ieSignal->encode(msg, ntMode) );
	}

	return ;
}

const char *MsgConnectAck::getName()
{
	return "ISDN-CONNECT_ACK";
}

