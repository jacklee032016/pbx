/*
* $Id: MsgCCRelease.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgCCRelease.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgCCRelease::MsgCCRelease(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
	:IsdnMsg(msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(_msg) )
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

MsgCCRelease::MsgCCRelease(IsdnCtrlDevice *ctrl)
	: IsdnMsg( ctrl)
{}

MsgCCRelease::MsgCCRelease(const MsgCCRelease& src)
        : IsdnMsg(src)
{
}

MsgCCRelease::~MsgCCRelease()
{
}


MsgCCRelease& MsgCCRelease::operator =(const MsgCCRelease& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
	}
	return (*this);
}
    
bool MsgCCRelease::operator ==(const MsgCCRelease& src)
{
	return true;
}


ISDN_METHOD MsgCCRelease::getType() const
{
	return ISDN_TYPE_RELEASE_CR;
}

int MsgCCRelease::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	return AS_OK;
}

#if WITH_DEBUG
void MsgCCRelease::debugInfo()
{
	cpLog(LOG_DEBUG, "L3 MSG : CR RELEASE");
	
	IsdnMsg::debugInfo();
}
#endif

int MsgCCRelease::getMsgType() const
{
//	cpLog(LOG_DEBUG, "CR RELEASE MT not defined");
	return ISDN_L3_MT_INVALIDATE;
}


int MsgCCRelease::getCCType() const
{
	return CC_RELEASE_CR;
}

void MsgCCRelease::encode( msg_t * msg)
{
//	msg_t *msg = 0;//IsdnMsg::encode(isdnEp);
	assert(msg != 0);

	return;
}

const char *MsgCCRelease::getName()
{
	return "ISDN-RELEASE_CR";
}

