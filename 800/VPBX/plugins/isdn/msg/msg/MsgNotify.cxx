/*
* $Id: MsgNotify.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "MsgNotify.hxx"

#include "IeNotify.hxx"
#include "IeDisplay.hxx"
//#include "IeRedirDN.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgNotify::MsgNotify(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) 
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

MsgNotify::MsgNotify( IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
}

MsgNotify::MsgNotify(Sptr <IeNotify> _ieNotify, IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
	ieNotify = _ieNotify;
}

MsgNotify::MsgNotify(const MsgNotify& src)
        : IsdnMsg(src)
{
}

MsgNotify::~MsgNotify()
{
}

MsgNotify& MsgNotify::operator =(const MsgNotify& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
		this->ieNotify = src.ieNotify;
		this->ieDisplay = src.ieDisplay;
#if 0
		this->ieRedirDN = src.ieRedirDN;
#endif
	}
	return (*this);
}
    
bool MsgNotify::operator ==(const MsgNotify& src)
{
	if(this->ieNotify==src.ieNotify && this->ieDisplay==src.ieDisplay) //this->ieRedirDN==src.ieRedirDN)
		return true;
	return false;
}

ISDN_METHOD MsgNotify::getType() const
{
	return ISDN_TYPE_NOTIFY;
}

Sptr <IeNotify> MsgNotify::getNotify()
{
	return ieNotify;
}

void MsgNotify::setNotify(Sptr<IeNotify> _notify)
{
	ieNotify = _notify;
}

Sptr <IeDisplay> MsgNotify::getDisplay()
{
	return ieDisplay;
}

void MsgNotify::setDisplay(Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

int MsgNotify::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		p = myCtrl->getDecoder()->getNotify4NotifyMsg(iesData);
		if( p!= NULL)
			ieNotify = new IeNotify( p);
	}
	catch (IsdnIeParserException&)
	{
		throw  IsdnMsgParserException("failed in decode NOTIFY msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
	}
	
	try
	{
		p = NULL;
//		p = IsdnMsgFactory::instance()->getTeDecoder()->getDisplay4NotifyMsg( frm);
		if( p != NULL)
			ieDisplay = new IeDisplay( p);
	}
	catch (IsdnIeParserException&)
	{
		cpLog(LOG_ERR, "failed in decode NOTIFY msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgNotify::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : NOTIFY");
	IsdnMsg::debugInfo();

	if( ieNotify != 0)
	{	
		p = ieNotify->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
	else
	{
		cpLog(LOG_ERR, "No NOTIFY IE(mandatory) in this MOTIFY msg" );
	}

	if(ieDisplay != 0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

}
#endif

int MsgNotify::getMsgType() const
{
	return MT_NOTIFY;
}

int MsgNotify::getCCType() const
{
	return CC_NOTIFY;
}

void MsgNotify::encode(msg_t *msg )
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* this is a mandatory IE */
	if( ieNotify!= 0)
	{
		myCtrl->getEncoder()->encNotify4NotifyMsg(header, ieNotify->encode( msg, ntMode) );
	}
	else
	{	
		cpLog(LOG_ERR, "No NOTIFY IE in NOTIFY L3 msg");
		assert(0);
	}
	
	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4NotifyMsg(header, ieDisplay->encode( msg, ntMode));
	}

	return ;
}

const char *MsgNotify::getName()
{
	return "ISDN-NOTIFY";
}

