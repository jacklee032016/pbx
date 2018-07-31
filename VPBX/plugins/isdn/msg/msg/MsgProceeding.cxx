/*
* $Id: MsgProceeding.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "MsgProceeding.hxx"

#include "IeChanId.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeFacility.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgProceeding::MsgProceeding(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
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

MsgProceeding::MsgProceeding(IsdnCtrlDevice *ctrl) 
	: IsdnMsg(ctrl)
{
}

MsgProceeding::MsgProceeding(const MsgProceeding& src) 
	: IsdnMsg(src)
{
}

#if 0
MsgProceeding::MsgProceeding( Sptr <IeChanId> chanId,  Sptr <IeProgress> progress)
	: IsdnMsg()
{
	ieChanId = chanId;
	ieProgress = progress;
}
#endif

MsgProceeding::~MsgProceeding( )
{
}

MsgProceeding& MsgProceeding::operator=(const MsgProceeding& src)
{
	if ( &src != this)
	{
		*static_cast < IsdnMsg* > (this) = src;
		this->ieChanId = src.ieChanId;
		this->ieProgress = src.ieProgress;
	}
	return (*this);
}

bool MsgProceeding::operator ==(const MsgProceeding& src) 
{
	if(this->ieChanId==src.ieChanId && this->ieProgress == src.ieProgress )
		return true;
	return false;
}

ISDN_METHOD MsgProceeding::getType() const
{
	return ISDN_TYPE_PROCEEDING;
}

Sptr <IeChanId> MsgProceeding::getChanId() const
{
	return ieChanId;
}

void MsgProceeding::setChanId(Sptr < IeChanId > chanId)
{
	ieChanId = chanId;
}

Sptr <IeProgress> MsgProceeding::getProgress() const
{
	return ieProgress;
}

void MsgProceeding::setProgress(Sptr <IeProgress> progress)
{
	ieProgress = progress;
}
		
Sptr <IeDisplay> MsgProceeding::getDisplay() const
{
	return ieDisplay;
}

void MsgProceeding::setDisplay(Sptr <IeDisplay> display)
{
	ieDisplay = display;
}
		
Sptr <IeFacility> MsgProceeding::getFacility() const
{
	return ieFacility;
}

void MsgProceeding::setFacility(Sptr <IeFacility> facility)
{
	ieFacility = facility;
}
		
int MsgProceeding::decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
		p = myCtrl->getDecoder()->getChanId4ProceedingMsg(iesData);
		if( p!= NULL)
			ieChanId = new IeChanId( p);
		
		p = myCtrl->getDecoder()->getProgress4ProceedingMsg(iesData);
		if( p!= NULL)
			ieProgress = new IeProgress( p);
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode PROCEEDING msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		/* No mandatory IE in this L3 msg, so no exception are thrown out */
		cpLog(LOG_DEBUG, "failed in decode PROCEEDING msg" );
	}
	
	return AS_OK;
}


#if WITH_DEBUG
void MsgProceeding::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : PROCEEDING");
	IsdnMsg::debugInfo();

	if(ieChanId != 0)
	{
		p = ieChanId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if(ieProgress != 0 )
	{	
		p = ieProgress->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	if(ieDisplay != 0 )
	{	
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}
}
#endif

int MsgProceeding::getMsgType() const
{
	return MT_CALL_PROCEEDING;
}

int MsgProceeding::getCCType() const
{
	return CC_PROCEEDING;
}

void MsgProceeding::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	if(ieChanId != 0)
	{
		myCtrl->getEncoder()->encChanId4ProceedingMsg(header, ieChanId->encode( msg, ntMode) );
	}
	if( ieProgress != 0)
	{
		myCtrl->getEncoder()->encProgress4ProceedingMsg(header, ieProgress->encode( msg, ntMode) );
	}
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4ProceedingMsg(header, ieDisplay->encode( msg, ntMode) );
	}

	if( myCtrl->getNtMode() && ieFacility!= 0)
	{
		myCtrl->getEncoder()->encFacility4ProceedingMsg(header, ieFacility->encode( msg, ntMode) );
	}
	
	return ;
}

const char *MsgProceeding::getName()
{
	return "ISDN-PROCEEDING";
}

