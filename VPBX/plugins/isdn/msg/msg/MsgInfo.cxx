/*
* $Id: MsgInfo.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "cpLog.h"

#include "MsgInfo.hxx"

#include "IeKeypad.hxx"
#include "IeDisplay.hxx"
#include "IeSignal.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgInfo::MsgInfo(const msg_t  *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &)
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

MsgInfo::MsgInfo(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
}

MsgInfo::MsgInfo(const MsgInfo& src)
        : IsdnMsg(src)
{
}

MsgInfo::~MsgInfo()
{
}

MsgInfo& MsgInfo::operator =(const MsgInfo& src)
{
	//copy details contained in the base class.
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
		this->ieKeypad = src.ieKeypad;
		this->ieDisplay = src.ieDisplay;
		this->ieSignal = src.ieSignal;
#if 0
		this->ieComplete = src.ieComplete;
#endif
	}
	
	return (*this);
}
    
bool MsgInfo::operator ==(const MsgInfo &src)
{
	if(ieKeypad == src.ieKeypad )// && ieCalledPN== src.ieCalledPN )//&& ieComplete==src.ieComplete)
		return  true;
	return false;
}

ISDN_METHOD MsgInfo::getType() const
{
	return ISDN_TYPE_INFO;
}

void MsgInfo::setDisplay(Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

Sptr <IeDisplay> MsgInfo::getDisplay()
{
	return ieDisplay;
}

void MsgInfo::setKeypad(Sptr <IeKeypad> keypad)
{
	ieKeypad = keypad;
}

Sptr <IeKeypad> MsgInfo::getKeypad()
{
	return ieKeypad;
}

void MsgInfo::setSignal(Sptr <IeSignal> _signal)
{
	ieSignal = _signal;
}

Sptr <IeSignal> MsgInfo::getSignal()
{
	return ieSignal;
}

#if 0
void MsgInfo::setComplete(Sptr <IeComplete> complete)
{
	ieComplete = complete;
}

Sptr <IeComplete> MsgInfo::getComplete()
{
	return ieComplete;
}
#endif

int MsgInfo::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		p = myCtrl->getDecoder()->getKeypad4InfoMsg(iesData);
		if( p != NULL)
			ieKeypad = new IeKeypad( p);
		p = NULL;
//		p = myCtrl->getDecoder()->getDisplay4InfoMsg(iesData);
		if(p!= NULL)
			ieDisplay = new IeDisplay( p);

		p = NULL;
//		p = myCtrl->getDecoder()->getSignal4InfoMsg(iesData);
		if(p != NULL)
			ieSignal = new IeSignal( p);

	}
	catch (IsdnIeParserException&)
	{/* No mandatory IEs  */
		cpLog(LOG_DEBUG, "failed in Decode INFORMATION msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgInfo::debugInfo()
{
	const char *p;
	
	cpLog(LOG_DEBUG, "L3 MSG : INFO");
	IsdnMsg::debugInfo();

	if(ieKeypad != 0)
	{
		p = ieKeypad->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if(ieSignal != 0)
	{
		p = ieSignal->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	if(ieDisplay != 0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

}
#endif

int MsgInfo::getMsgType() const
{
	return MT_INFORMATION;
}

int MsgInfo::getCCType() const
{
	return CC_INFORMATION;
}

void MsgInfo::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* following only used in n-->u */
	if( ieKeypad != 0)
	{
		myCtrl->getEncoder()->encKeypad4InfoMsg(header, ieKeypad->encode(msg, ntMode) );
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4InfoMsg(header, ieDisplay->encode(msg, ntMode) );
	}
	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4InfoMsg(header, 	ieSignal->encode(msg,ntMode) );
	}

	return ;
}

const char *MsgInfo::getName()
{
	return "ISDN-INFO";
}

