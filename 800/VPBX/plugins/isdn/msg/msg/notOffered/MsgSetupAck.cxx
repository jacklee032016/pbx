/*
* $Id: MsgSetupAck.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
 
#include "global.h"
#include "cpLog.h"

#include "MsgSetupAck.hxx"

#include "IeChanId.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeSignal.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

using namespace Assist;

MsgSetupAck::MsgSetupAck(const iframe_t *frm)   throw(IsdnMsgParserException &)
	:IsdnMsg( frm)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(_msg) )
	{
		cpLog(LOG_DEBUG_STACK, "Failed in parse IsdnMsg (prim :0x%x) on device %s", ctrl->getDecoder()->getPrim, ctrl->getName() );
		throw  IsdnMsgParserException("failed in decode ISDN MSG", __FILE__, __LINE__, res );
	}
#endif
	unsigned char *data = myCtrl->getDecoder()->getL3Data( _msg);

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

MsgSetupAck::MsgSetupAck(const MsgSetupAck& src)
        : IsdnMsg(src)
{
}

MsgSetupAck::MsgSetupAck(Sptr <IeChanId> chanId, Sptr <IeProgress> progress)
        : IsdnMsg()
{
	ieChanId = chanId;
	ieProgress = progress;
}


MsgSetupAck::MsgSetupAck()
        : IsdnMsg()
{
}

MsgSetupAck::~MsgSetupAck()
{
}

const MsgSetupAck&  MsgSetupAck::operator=(const MsgSetupAck& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
		this->ieChanId = src.ieChanId;
		this->ieProgress = src.ieProgress;
		this->ieDisplay = src.ieDisplay;
		this->ieSignal = src.ieSignal;
	}

	return (*this);
}
    
    
bool MsgSetupAck::operator ==(const MsgSetupAck& src)
{
	if(	this->ieChanId == src.ieChanId && this->ieProgress == src.ieProgress
	 &&	this->ieDisplay == src.ieDisplay && this->ieSignal == src.ieSignal )
	 	return true;
	return false;
}
        

Assist::ISDN_METHOD MsgSetupAck::getType() const
{
	return ISDN_TYPE_SETUP_ACK;
}


Sptr <IeChanId>  MsgSetupAck::getChanId() const
{
	return ieChanId;
}

void MsgSetupAck::setChanId(Sptr <IeChanId> chanId)
{
	ieChanId = chanId;
}


Sptr <IeProgress>  MsgSetupAck::getProgress() const
{
	return ieProgress;
}

void MsgSetupAck::setProgress(Sptr <IeProgress> progress)
{
	ieProgress = progress;
}


Sptr <IeDisplay>  MsgSetupAck::getDisplay() const
{
	return ieDisplay;
}

void MsgSetupAck::setDisplay(Sptr <IeDisplay> display)
{
	ieDisplay = display;
}


Sptr <IeSignal>  MsgSetupAck::getSignal() const
{
	return ieSignal;
}

void MsgSetupAck::setSignal(Sptr <IeSignal> signal)
{
	ieSignal = signal;
}

int MsgSetupAck::decode(const iframe_t *frm)  throw(IsdnMsgParserException &)
{
	const char *p = NULL;

	try
	{
		p = IsdnMsgFactory::instance()->getTeDecoder()->getChanId4SetupAckMsg( frm);
		if(p != NULL)
			ieChanId = new IeChanId( p);
		p = IsdnMsgFactory::instance()->getTeDecoder()->getProgress4SetupAckMsg( frm);
		if(p != NULL)
			ieProgress = new IeProgress( p);
	}
	catch (IsdnIeParserException&)
	{
		throw  IsdnMsgParserException("failed in decode SETUP_ACK msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgSetupAck::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : SETUP ACK");
	IsdnMsg::debugInfo();

	p = ieChanId->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
	
	p = ieProgress->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
	
	p = ieDisplay->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
	
	p = ieSignal->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
}
#endif

int MsgSetupAck::getMsgType() const
{
	return MT_SETUP_ACKNOWLEDGE;
}

const char *MsgSetupAck::getName()
{
	return "ISDN-SETUP_ACK";
}

