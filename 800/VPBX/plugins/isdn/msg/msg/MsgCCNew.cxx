/*
* $Id: MsgCCNew.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgCCNew.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgCCNew::MsgCCNew(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
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

MsgCCNew::MsgCCNew(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{}

MsgCCNew::MsgCCNew(const MsgCCNew& src)
        : IsdnMsg(src)
{
}

MsgCCNew::~MsgCCNew()
{
}


MsgCCNew& MsgCCNew::operator =(const MsgCCNew& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
	}
	return (*this);
}
    
bool MsgCCNew::operator ==(const MsgCCNew& src)
{
	return true;
}


ISDN_METHOD MsgCCNew::getType() const
{
	return ISDN_TYPE_CC_NEW;
}

int MsgCCNew::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	return AS_OK;
}

#if WITH_DEBUG
void MsgCCNew::debugInfo()
{
	cpLog(LOG_DEBUG, "L3 MSG : CC NEW");
	
	IsdnMsg::debugInfo();
}
#endif

int MsgCCNew::getMsgType() const
{
//	cpLog(LOG_DEBUG, "CC NEW MT not defined");
	return ISDN_L3_MT_INVALIDATE;
}


int MsgCCNew::getCCType() const
{
	return CC_NEW_CR;
}

void MsgCCNew::encode( msg_t * msg)
{
//	msg_t *msg = 0;//IsdnMsg::encode(isdnEp);
	assert(msg != 0);

	return;
}

const char *MsgCCNew::getName()
{
	return "ISDN-NEW_CR";
}


