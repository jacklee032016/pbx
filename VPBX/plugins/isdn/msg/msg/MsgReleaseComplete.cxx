/*
* $Id: MsgReleaseComplete.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "MsgReleaseComplete.hxx"

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

MsgReleaseComplete::MsgReleaseComplete(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
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

MsgReleaseComplete::MsgReleaseComplete(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgReleaseComplete::MsgReleaseComplete(Sptr <IeCause> _cause, IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
	ieCause = _cause;
}

MsgReleaseComplete::MsgReleaseComplete(const MsgReleaseComplete& src)
        : IsdnMsg(src)
{
}

MsgReleaseComplete::~MsgReleaseComplete()
{
}


MsgReleaseComplete& MsgReleaseComplete::operator =(const MsgReleaseComplete& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
		this->ieCause = src.ieCause;
	}
	return (*this);
}
    
bool MsgReleaseComplete::operator ==(const MsgReleaseComplete& src)
{
	if(this->ieCause == src.ieCause)
		return true;
	return false;
}


ISDN_METHOD MsgReleaseComplete::getType() const
{
	return ISDN_TYPE_RELEASE_COMPLETE_IND;
}


Sptr <IeCause> MsgReleaseComplete::getCause() const
{
	return ieCause;
}

void MsgReleaseComplete::setCause(Sptr <IeCause> cause)
{
	ieCause = cause;
}
		

Sptr <IeDisplay> MsgReleaseComplete::getDisplay()
{
	return ieDisplay;
}

void MsgReleaseComplete::setDisplay(Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

Sptr <IeSignal> MsgReleaseComplete::getSignal()
{
	return ieSignal;
}

void MsgReleaseComplete::setSignal(Sptr <IeSignal> _signal )
{
	ieSignal = _signal;
}

int MsgReleaseComplete::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		
		p = myCtrl->getDecoder()->getCause4ReleaseCompleteMsg(iesData);
		if(p != NULL)
			ieCause = new IeCause( p);
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode RELEASE_COMPLETE msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_DEBUG, "failed in decode RELEASE_COMPLETE msg"  );
	}
	
	return AS_OK;
}


#if WITH_DEBUG
void MsgReleaseComplete::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : RELEASE_COMPLETE");
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

int MsgReleaseComplete::getMsgType() const
{
	return MT_RELEASE_COMPLETE;
}


int MsgReleaseComplete::getCCType() const
{
	return CC_RELEASE_COMPLETE;
}

void MsgReleaseComplete::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* this is a mandatory IE */
	if( ieCause!= 0)
	{
		myCtrl->getEncoder()->encCause4ReleaseCompleteMsg(header, ieCause->encode( msg, ntMode));
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4ReleaseCompleteMsg(header, ieDisplay->encode( msg, ntMode) );
	}

	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4ReleaseCompleteMsg(header, ieSignal->encode( msg, ntMode) );
	}
	return ;
}

const char *MsgReleaseComplete::getName()
{
	return "ISDN-RELEASE_COMPLETE";
}

