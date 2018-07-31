/*
* $Id: MsgAlerting.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "MsgAlerting.hxx"

#include "IeChanId.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeSignal.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgAlerting::MsgAlerting(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) 
        : IsdnMsg(msg, ctrl)
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

MsgAlerting::MsgAlerting(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgAlerting::MsgAlerting(const MsgAlerting& src)
	: IsdnMsg(src)
{
}

MsgAlerting::~MsgAlerting()
{
}

MsgAlerting& MsgAlerting::operator =(const MsgAlerting& src)
{
	if ( &src != this)
	{
		IsdnMsg::operator=( src );
		this->ieChanId = src.ieChanId;
		this->ieProgress = src.ieProgress;
	}
	return (*this);
}


bool MsgAlerting::operator ==(const MsgAlerting& src) 
{
	if(this->ieChanId == src.ieChanId && this->ieProgress== src.ieProgress )
		return true;
	return false;
}

ISDN_METHOD MsgAlerting::getType() const
{
	return ISDN_TYPE_ALERTING;
}

void MsgAlerting::setChanId( Sptr <IeChanId> chanId)
{
	ieChanId = chanId;
};

Sptr <IeChanId> MsgAlerting::getChanId()
{
	return ieChanId;
}

void MsgAlerting::setProgress(Sptr <IeProgress> progress)
{
	ieProgress = progress;
}

Sptr <IeProgress> MsgAlerting::getProgress()
{
	return ieProgress;
}

int MsgAlerting::decode(const unsigned char *ieDatas)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		p = myCtrl->getDecoder()->getChanId4AlertingMsg( ieDatas );
		if(p != NULL)
			ieChanId = new IeChanId( p);
		
		p = myCtrl->getDecoder()->getProgress4AlertingMsg( ieDatas);
		if( p != NULL )
			ieProgress= new IeProgress(p);
		
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode ALERTING msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		/* no mandatory IE in ALERTING msg, so no exception is thrown */
		cpLog(LOG_DEBUG, "Option IEs decode failed ALERTING msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgAlerting::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : ALERTING");

	IsdnMsg::debugInfo();

	if(ieChanId != 0)
	{
		p = ieChanId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p);
		free((void *)p);
	}	

	if( ieProgress != 0)
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

}
#endif

int MsgAlerting::getMsgType() const
{
	return MT_ALERTING;
}

int MsgAlerting::getCCType() const
{
	return CC_ALERTING;
}

void MsgAlerting::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	if(ieChanId != 0)
	{
		myCtrl->getEncoder()->encChanId4AlertingMsg( header, ieChanId->encode( msg, ntMode) );
	}
	if( ieProgress != 0)
	{
		myCtrl->getEncoder()->encProgress4AlertingMsg( header, ieProgress->encode(msg, ntMode) );
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4AlertingMsg(header, ieDisplay->encode( msg, ntMode) );
	}
	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4AlertingMsg(header, ieSignal->encode( msg, ntMode) );
	}

	return ;
}

const char *MsgAlerting::getName()
{
	return "ISDN-ALERTING";
}

