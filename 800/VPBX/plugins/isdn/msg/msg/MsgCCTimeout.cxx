/*
* $Id: MsgCCTimeout.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgCCTimeout.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgCCTimeout::MsgCCTimeout(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
	:IsdnMsg(msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(_msg) )
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

MsgCCTimeout::MsgCCTimeout(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{}

MsgCCTimeout::MsgCCTimeout(const MsgCCTimeout& src)
        : IsdnMsg(src)
{
}

MsgCCTimeout::~MsgCCTimeout()
{
}

MsgCCTimeout& MsgCCTimeout::operator =(const MsgCCTimeout& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
	}
	return (*this);
}
    
bool MsgCCTimeout::operator ==(const MsgCCTimeout& src)
{
	return true;
}


ISDN_METHOD MsgCCTimeout::getType() const
{
	return ISDN_TYPE_CC_TIMEOUT;
}

int MsgCCTimeout::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	return AS_OK;
}

#if WITH_DEBUG
void MsgCCTimeout::debugInfo()
{
	cpLog(LOG_DEBUG, "L3 MSG : CC TIMEOUT");
	
	IsdnMsg::debugInfo();
}
#endif

int MsgCCTimeout::getMsgType() const
{
//	cpLog(LOG_DEBUG, "CC TIMEOUT MT not defined");
	return ISDN_L3_MT_INVALIDATE;
}


int MsgCCTimeout::getCCType() const
{
	return CC_TIMEOUT;
}

void MsgCCTimeout::encode( msg_t * msg)
{
//	msg_t *msg = 0;//IsdnMsg::encode(isdnEp);
	assert(msg != 0);

	return;
}

const char *MsgCCTimeout::getName()
{
	return "ISDN-CC_TIMEOUT";
}

