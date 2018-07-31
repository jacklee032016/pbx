/*
* $Id: MsgSetup.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgSetup.hxx"

//#include "IeRepeat.hxx"
#include "IeBearer.hxx"
#include "IeChanId.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeKeypad.hxx"
#include "IeSignal.hxx"
#include "IeCallSub.hxx"
#include "IeCallingPN.hxx"
#include "IeCalledPN.hxx"
#include "IeRedirNR.hxx"
#ifdef CENTREX
#include "IeCentrex.hxx"
#endif
#include "IeFacility.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgSetup::MsgSetup(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
	:IsdnMsg( msg, ctrl)
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

MsgSetup::MsgSetup(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{}

MsgSetup::MsgSetup(const MsgSetup& src)
        : IsdnMsg(src)
{
}

MsgSetup::~MsgSetup()
{
}

MsgSetup& MsgSetup::operator =(const MsgSetup& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
	}
	return (*this);
}
    
bool MsgSetup::operator ==(const MsgSetup& src)
{
	if( this->myCtrl == src.myCtrl &&  this->ieCalledPN == src.ieCalledPN 
		&& this->ieCallingPN == src.ieCallingPN && this->ieChanId==src.ieChanId 
#ifdef CENTREX
		&& this->ieCentrex == src.ieCentrex 
#endif
#if 0
		&& this->ieUserUser==src.ieUserUser && this->ieComplete == src.ieComplete 
#endif
		&& this->ieCallingSub ==src.ieCallingSub && this->ieCalledSub == src.ieCalledSub
		&& this->ieBearer == src.ieBearer && this->ieRedirNR== src.ieRedirNR )
		return true;
	return false;
}

ISDN_METHOD MsgSetup::getType() const
{
	return ISDN_TYPE_SETUP;
}

void MsgSetup::setBearer(Sptr <IeBearer> bearer)
{
	ieBearer = bearer;
}
Sptr <IeBearer> MsgSetup::getBearer() const 
{
	return ieBearer;
}

Sptr <IeChanId> MsgSetup::getChanId() const 
{
	return ieChanId;
}
void MsgSetup::setChanId(Sptr <IeChanId> chanId)
{
	ieChanId = chanId;
}

Sptr <IeCallingPN> MsgSetup::getCallingPN() const
{
	return ieCallingPN;
}
void MsgSetup::setCallingPN(Sptr <IeCallingPN> callingPN)
{
	ieCallingPN = callingPN;
}
Sptr <IeCallSub> MsgSetup::getCallingSub() const
{
	return ieCallingSub;
}
void MsgSetup::setCallingSub(Sptr <IeCallSub> _callingSub)
{
	ieCallingSub = _callingSub;
}

Sptr <IeCalledPN> MsgSetup::getCalledPN() const
{
	return ieCalledPN;
}
void MsgSetup::setCalledPN(Sptr <IeCalledPN> calledPN)
{
	ieCalledPN = calledPN;
}
Sptr <IeCallSub> MsgSetup::getCalledSub() const
{
	return ieCalledSub;
}
void MsgSetup::setCalledSub(Sptr <IeCallSub> _calledSub)
{
	ieCalledSub = _calledSub;
}

#ifdef CENTREX
Sptr <IeCentrex>	MsgSetup::getCentrex() const
{
	return ieCentrex;
}

void MsgSetup::setCentrex(Sptr <IeCentrex> centrex)
{
	ieCentrex = centrex;
}

#endif

Sptr <IeProgress> MsgSetup::getProgress() const
{
	return ieProgress;
}
void MsgSetup::setProgress(Sptr <IeProgress> _pregress)
{
	ieProgress = _pregress;
}

Sptr <IeKeypad> MsgSetup::getKeypad() const
{
	return ieKeypad;
}
void MsgSetup::setKeypad(Sptr <IeKeypad> _keypad)
{
	ieKeypad = _keypad;
}

Sptr <IeDisplay> MsgSetup::getDisplay() const
{
	return ieDisplay;
}
void MsgSetup::setDisplay(Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

Sptr <IeSignal> MsgSetup::getSignal() const
{
	return ieSignal;
}
void MsgSetup::setSignal(Sptr <IeSignal> _signal)
{
	ieSignal = _signal;
}


#if 0
void MsgSetup::setComplete(Sptr <IeComplete> complete)
{
	ieComplete = complete;
}

Sptr <IeComplete> MsgSetup::getComplete() const 
{
	return ieComplete;
}
#endif

void MsgSetup::setRedirNR(Sptr <IeRedirNR> redirNR)
{
	ieRedirNR = redirNR;
}

Sptr <IeRedirNR> MsgSetup::getRedirNR() const 
{
	return ieRedirNR;
}


int MsgSetup::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;
	cpLog(LOG_DEBUG_STACK, "Parse ISDN SETUP msg");

	try
	{
		/* BERAER IE is mandatory */
		p = myCtrl->getDecoder()->getBearer4SetupMsg( iesData );
		ieBearer = new IeBearer( p);
		
		p = myCtrl->getDecoder()->getChanId4SetupMsg(iesData);
		if(p!= NULL)
			ieChanId = new IeChanId( p);
	}
	catch (IsdnIeParserException&)
	{
		throw  IsdnMsgParserException("failed in decode SETUP msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
	}
	
	try
	{
		/* this IE is provided in SETUP */
		p = myCtrl->getDecoder()->getCallingPN4SetupMsg(iesData);
		if(p != NULL)
			ieCallingPN = new IeCallingPN( p);

		p = myCtrl->getDecoder()->getCalledPN4SetupMsg(iesData);
		if(p != NULL )
			ieCalledPN = new IeCalledPN( p);
#ifdef CENTREX
	/* te-mode: CNIP (calling name identification presentation) */
//	if (!ntMode)
		p = myCtrl->getDecoder()->getCentrex4SetupMsg(iesData);
		if(p!= NULL )
			ieCentrex = new IeCentrex( p);
#endif

#if 0
		p = myCtrl->getDecoder()->getUserUser4SetupMsg(iesData);
		if(p != NULL)
			ieUserUser = new IeUserUser( p);

		p = myCtrl->getDecoder()->getRedirNR4SetupMsg(iesData);
		if(p != NULL)
			ieRedirNR = new IeRedirNR( p);
#endif

	}
	catch (IsdnIeParserException&)
	{
		cpLog(LOG_ERR, "failed in decode SETUP msg for optional IEs");
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgSetup::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : SETUP");
	IsdnMsg::debugInfo();

#if 0
	if( ieRepeat != 0)
	{
		p = ieRepeat->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
#endif

	if( ieBearer != 0 )
	{
		p = ieBearer->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
	else
	{
		cpLog(LOG_ERR, "IE Bearer is mandatory in SETUP msg" );
	}

	if( ieChanId != 0)
	{
		p = ieChanId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
	
	if( ieProgress != 0)
	{
		p = ieProgress->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if( ieDisplay != 0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
	if( ieKeypad != 0)
	{
		p = ieKeypad->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
	if( ieSignal != 0)
	{
		p = ieSignal->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if(ieCallingPN != 0 )
	{
		p = ieCallingPN->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
	if(ieCallingSub!= 0)
	{
		p = ieCallingSub->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	if(ieCalledPN != 0)
	{
		p = ieCalledPN->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
	if(ieCalledSub!= 0)
	{
		p = ieCalledSub->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	
#ifdef CENTREX
	p = ieCentrex->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
#endif

	if(ieRedirNR != 0)
	{
		p = ieRedirNR->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

}
#endif

int MsgSetup::getMsgType() const
{
	return MT_SETUP;
}


int MsgSetup::getCCType() const
{
	return CC_SETUP;
}

void MsgSetup::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

#if 0
	/* this is a optional IE */
	if( ieRepeat!= 0)
	{
		ieRepeatId->encode( msg);
	}
#endif

	if( ieBearer!= 0)
	{
		myCtrl->getEncoder()->encBearer4SetupMsg(header, ieBearer->encode( msg, ntMode ) );
	}
	else
	{
		cpLog(LOG_ERR, "No Bearer in SETUP msg, this IE is mandatory");
	}

#if 1
	if(ntMode==ISDN_DEV_TYPE_USER)
	{/* for User side device and PBX application */
		Sptr <IeFacility> facility = new IeFacility();
		myCtrl->getEncoder()->encFacility4SetupMsg(header, facility->encode( msg, ntMode) );
	}
#endif

	if( ieChanId!= 0)
	{
		myCtrl->getEncoder()->encChanId4SetupMsg(header,  ieChanId->encode( msg, ntMode  ) );
	}
	if( ieProgress!= 0)
	{
		myCtrl->getEncoder()->encProgress4SetupMsg(header,  ieProgress->encode( msg, ntMode  ) );
	}
	if( ieDisplay!= 0)
	{
		myCtrl->getEncoder()->encDisplay4SetupMsg(header, ieDisplay->encode( msg, ntMode ) );
	}
	if( ieKeypad!= 0)
	{
		myCtrl->getEncoder()->encKeypad4SetupMsg(header, ieKeypad->encode( msg, ntMode ) );
	}
	if( ieSignal!= 0)
	{
		myCtrl->getEncoder()->encSignal4SetupMsg(header, ieSignal->encode( msg, ntMode ) );
	}
	
	if( ieCallingPN!= 0)
	{
		myCtrl->getEncoder()->encCallingPN4SetupMsg(header, ieCallingPN->encode( msg, ntMode ) );
	}
	if( ieCallingSub!= 0)
	{
		myCtrl->getEncoder()->encCallingSub4SetupMsg(header, ieCallingSub->encode( msg, ntMode ) );
	}
	if( ieCalledPN!= 0)
	{
		myCtrl->getEncoder()->encCalledPN4SetupMsg(header, ieCalledPN->encode( msg, ntMode ) );
	}
	if( ieCalledSub!= 0)
	{
		myCtrl->getEncoder()->encCalledSub4SetupMsg(header, ieCalledSub->encode( msg, ntMode ) );
	}
	
	if( ieRedirNR!= 0)
	{
		myCtrl->getEncoder()->encRedirNR4SetupMsg(header, ieRedirNR->encode( msg, ntMode ) );
	}
	return ;
}

const char *MsgSetup::getName()
{
	return "ISDN-SETUP";
}

