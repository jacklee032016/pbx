/*
* $Id: MsgHold.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
 
#include "global.h"
#include "cpLog.h"

#include "MsgHold.hxx"

/* No IEs are used */

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgHold::MsgHold(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) 
        : IsdnMsg(msg, ctrl)
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

MsgHold::MsgHold(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
//	setRegisterDetails();
}

MsgHold::MsgHold(const MsgHold& src)
        : IsdnMsg(src)
{
}

MsgHold::~MsgHold()
{
}

MsgHold& MsgHold::operator=( const MsgHold& src )
{
	if ( &src != this )
	{
		IsdnMsg::operator=( src );
		// Copy data member here
	}

	return ( *this );
}

bool MsgHold::operator ==(const MsgHold& src) 
{
	return (  *static_cast < IsdnMsg* > (this) == src);
}


ISDN_METHOD MsgHold::getType() const
{
	return ISDN_TYPE_HOLD;
}

int MsgHold::decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &)
{
	return AS_OK;
}

#if WITH_DEBUG
void MsgHold::debugInfo()
{
	cpLog(LOG_DEBUG, "L3 MSG : HOLD");
	IsdnMsg::debugInfo();
}
#endif

int MsgHold::getMsgType() const
{
	return MT_HOLD;
}


int MsgHold::getCCType() const
{
	return CC_HOLD;
}

void MsgHold::encode(msg_t *msg )
{
	assert(msg != 0);

	return ;
}

const char *MsgHold::getName()
{
	return "ISDN-HOLD";
}

