 /*
 * $Id: MsgRelease.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
 
#include "global.h"
#include "cpLog.h"

#include "MsgRelease.hxx"

#include "IeCause.hxx"
#include "IeSignal.hxx"
#include "IeDisplay.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgRelease::MsgRelease(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
        : IsdnMsg(msg, ctrl)
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

MsgRelease::MsgRelease(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgRelease::MsgRelease(Sptr <IeCause> _cause,IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
	ieCause = _cause;
}

MsgRelease::MsgRelease(const MsgRelease& src)
        : IsdnMsg(src)
{
}

MsgRelease::~MsgRelease()
{
}

MsgRelease& MsgRelease::operator =(const MsgRelease& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
		this->ieCause = src.ieCause;
	}
	return (*this);
}
    
bool MsgRelease::operator ==(const MsgRelease& src)
{
	if(this->ieCause==src.ieCause)
		return true;
	return false;
}


ISDN_METHOD MsgRelease::getType() const
{
	return ISDN_TYPE_RELEASE;
}

Sptr <IeCause> MsgRelease::getCause() const
{
	return ieCause;
}

void MsgRelease::setCause(Sptr <IeCause> cause)
{
	ieCause = cause;
}

Sptr <IeDisplay> MsgRelease::getDisplay()
{
	return ieDisplay;
}

void MsgRelease::setDisplay(Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

Sptr <IeSignal> MsgRelease::getSignal()
{
	return ieSignal;
}

void MsgRelease::setSignal(Sptr <IeSignal> _signal )
{
	ieSignal = _signal;
}

int MsgRelease::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		
		p = myCtrl->getDecoder()->getCause4ReleaseMsg(iesData);
		if( p != NULL)
			ieCause = new IeCause( p);
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode RELEASE msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_DEBUG, "failed in decode RELEASE msg"  );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgRelease::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : RELEASE");
	IsdnMsg::debugInfo();

	if(ieCause != 0)
	{
		p = ieCause->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if(ieDisplay != 0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
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

int MsgRelease::getMsgType() const
{
	return MT_RELEASE;
}

int MsgRelease::getCCType() const
{
	return CC_RELEASE;
}

void MsgRelease::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* this is a mandatory IE */
	if( ieCause!= 0)
	{
		myCtrl->getEncoder()->encCause4ReleaseMsg(header, ieCause->encode( msg, ntMode) );
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4ReleaseMsg(header, 	ieDisplay->encode( msg, ntMode) );
	}

	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4ReleaseMsg(header, ieSignal->encode( msg, ntMode) );
	}
	return ;
}

const char *MsgRelease::getName()
{
	return "ISDN-RELEASE";
}

