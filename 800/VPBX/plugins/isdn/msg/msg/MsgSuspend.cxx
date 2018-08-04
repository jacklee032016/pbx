/*
* $Id: MsgSuspend.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
 
#include "global.h"
#include "cpLog.h"

#include "MsgSuspend.hxx"

#include "IeCallId.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgSuspend::MsgSuspend(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
	:IsdnMsg(msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(msg) )
	{
		cpLog(LOG_DEBUG_STACK, "Failed in parse IsdnMsg (prim :0x%x) on device %s", ctrl->getDecoder()->getPrim, ctrl->getName() );
		throw  IsdnMsgParserException("failed in decode ISDN MSG", __FILE__, __LINE__, res );
	}
#endif
	unsigned char *data = myCtrl->getDecoder()->getL3Data(msg);

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

MsgSuspend::MsgSuspend(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgSuspend::MsgSuspend(const MsgSuspend& src)
        : IsdnMsg(src)
{
}

#if 0
MsgSuspend::MsgSuspend(Sptr <IeCallState> _callState)
        : IsdnMsg()
{
	ieCallState = _callState;
}
#endif

MsgSuspend::~MsgSuspend( )
{
}

MsgSuspend& MsgSuspend::operator=( const MsgSuspend& src )
{
	if ( &src != this )
	{
		IsdnMsg::operator=( src );
		this->ieCallId = src.ieCallId;
	}

	return ( *this );
}


bool  MsgSuspend::operator ==(const MsgSuspend& src) 
{
	return (  *static_cast < IsdnMsg* > (this) == src);  
	if(this->ieCallId == src.ieCallId )
		return true;
	return false;
}

ISDN_METHOD MsgSuspend::getType() const
{
	return ISDN_TYPE_SUSPEND;
}


Sptr <IeCallId>  MsgSuspend::getCallId() const
{
	return ieCallId;
}

void MsgSuspend::setCallId(Sptr <IeCallId> callId)
{
	ieCallId = callId;
}

int MsgSuspend::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
//		p = myCtrl->getDecoder()->getChanId4ProceedingMsg(iesData);
		if( p != NULL)
			ieCallId = new IeCallId(p);
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode SUSPEND msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		/* this is a optional IE in SUSPEND msg */
		cpLog(LOG_ERR, "failed in decode SUSPEND msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgSuspend::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : SUSPEND");
	IsdnMsg::debugInfo();

	if(ieCallId != 0)
	{
		p = ieCallId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
}
#endif

int MsgSuspend::getMsgType() const
{
	return MT_SUSPEND;
}

int MsgSuspend::getCCType() const
{
	return CC_SUSPEND;
}

void MsgSuspend::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* this is a optional IE */
	if( ieCallId!= 0)
	{
		myCtrl->getEncoder()->encCallId4SuspendMsg(header, ieCallId->encode( msg, ntMode) );
	}

	return ;
}

const char *MsgSuspend::getName()
{
	return "ISDN-SUSPEND";
}

