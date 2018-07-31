/*
* $Id: MsgDisConnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "MsgDisConnect.hxx"

#include "IeCause.hxx"
#include "IeProgress.hxx"
#include "IeCharge.hxx"
#include "IeDisplay.hxx"
#include "IeSignal.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgDisConnect::MsgDisConnect(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
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

MsgDisConnect::MsgDisConnect(IsdnCtrlDevice *ctrl) 
	: IsdnMsg(ctrl)
{
	ieProgress = new IeProgress;
}

MsgDisConnect::MsgDisConnect(Sptr <IeCause> _ieCause, IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl),
	ieCause(_ieCause)
{
	ieProgress = new IeProgress;
}

MsgDisConnect::MsgDisConnect(const MsgDisConnect& src)
        : IsdnMsg(src)
{
}

MsgDisConnect::~MsgDisConnect()
{
}

MsgDisConnect&  MsgDisConnect::operator =(const MsgDisConnect& src)
{
	if ( &src != this)
	{
		*static_cast < IsdnMsg* > (this) = src;
		this->ieCause = src.ieCause;
		this->ieProgress = src.ieProgress;
	}
	return (*this);
}

bool MsgDisConnect::operator ==(const MsgDisConnect& src) 
{
	if(this->ieCause==src.ieCause && this->ieProgress==src.ieProgress)
		return true;
	return false;
}

ISDN_METHOD MsgDisConnect::getType() const
{
	return ISDN_TYPE_DISCONNECT;
}

void MsgDisConnect::setCause(Sptr <IeCause> cause)
{
	ieCause = cause;
}

Sptr <IeCause> MsgDisConnect::getCause()
{
	return ieCause;
}
		
void MsgDisConnect::setProgress(Sptr <IeProgress> _progress)
{
	ieProgress = _progress;
}

Sptr <IeProgress> MsgDisConnect::getProgress()
{
	return ieProgress;
}

void MsgDisConnect::setCharge(Sptr <IeCharge> _charge)
{
	ieCharge = _charge;
}

Sptr <IeCharge> MsgDisConnect::getCharge()
{
	return ieCharge;
}

		
int MsgDisConnect::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		p = myCtrl->getDecoder()->getCause4DisConnMsg( iesData);
		if( p != NULL)
		/* this IE is mandatory, so must be decode */
		/* TE send DISCONNECT without cause when no response from NT, lizhijie,2005.12.28*/
			ieCause = new IeCause( p);
		
	}
	catch (IsdnIeParserException&)
	{
		throw  IsdnMsgParserException("failed in decode DISCONNECT msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
	}

	try
	{
		p = myCtrl->getDecoder()->getProgress4DisConnMsg( iesData );
		if( p != NULL)
			ieProgress = new IeProgress( p);
	}
	catch (IsdnIeParserException&)
	{
		cpLog(LOG_DEBUG, "Optional IE is not used ");
	}

TRACE;
	if( !myCtrl->getNtMode() )
	{/* only rx Charge Info when in TE mode */
		try
		{
			p = myCtrl->getDecoder()->getCharge4DisConnMsg( iesData );
			if( p != NULL)
				ieCharge = new IeCharge(p);
		}
		catch (IsdnIeParserException&)
		{
			cpLog(LOG_DEBUG, "Optional IE is not used in ISDN Switch");
		}
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgDisConnect::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : DIS_CONNECT");
	IsdnMsg::debugInfo();

	if(ieCause != 0)
	{
		p = ieCause->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
	else
	{
		cpLog(LOG_ERR, "No CAUSE IE in DISCONNECT L3 msg");
	}

	if(ieProgress != 0)
	{
		p = ieProgress->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
	
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

	if(ieCharge != 0)
	{
		p = ieCharge->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
}
#endif

int MsgDisConnect::getMsgType() const
{
	return MT_DISCONNECT;
}

int MsgDisConnect::getCCType() const
{
	return CC_DISCONNECT;
}

void MsgDisConnect::encode( msg_t *msg )
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* following only used in n-->u */
	if( ieCause != 0)
	{
		myCtrl->getEncoder()->encCause4DisConnMsg(header, ieCause->encode(msg, ntMode) );
	}
	else
	{	
		cpLog(LOG_ERR, "No CAUSE IE in DISCONNECT L3 msg");
	}
	
	if( ieProgress != 0)
	{
		myCtrl->getEncoder()->encProgress4DisConnMsg(header, ieProgress->encode( msg, ntMode) );
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4DisConnMsg(header, ieDisplay->encode(msg, ntMode) );
	}
	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4DisConnMsg(header, ieSignal->encode( msg, ntMode) );
	}

	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4DisConnMsg(header, ieSignal->encode( msg, ntMode) );
	}

	if( myCtrl->getNtMode() && ieCharge != 0 )
	{/* only rx Charge Info when in TE mode */
		myCtrl->getEncoder()->encCharge4DisConnMsg(header, ieCharge->encode( msg, ntMode) );
	}
	
	return ;
}

const char *MsgDisConnect::getName()
{
	return "ISDN-DISCONNECT";
}

