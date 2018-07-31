/*
* $Id: MsgFacility.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
 
#include "global.h"
#include "cpLog.h"

#include "MsgFacility.hxx"

//#include "IeFacility.hxx"
#include "IeDisplay.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgFacility::MsgFacility(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
        : IsdnMsg( msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(msg) )
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

MsgFacility::MsgFacility(IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{}

MsgFacility::MsgFacility(const MsgFacility& src)
        : IsdnMsg(src)
{
}

MsgFacility::~MsgFacility()
{
}


MsgFacility& MsgFacility::operator =(const MsgFacility& src)
{
	if ( &src != this )
	{
		IsdnMsg::operator=(src);
#if 0
		this->ieFacility = src.ieFacility;
#endif
		this->ieDisplay = src.ieDisplay;
	}
	return (*this);
}
    
bool MsgFacility::operator ==(const MsgFacility& src)
{
	if(/* this->ieFacility==src.ieFacility && */this->ieDisplay==src.ieDisplay)
		return true;
	return false;
}

ISDN_METHOD MsgFacility::getType() const
{
	return ISDN_TYPE_FACILITY;
}

#if 0
Sptr <IeFacility> MsgFacility::getFacility() const
{
	return ieFacility;
}

void MsgFacility::setFacility( Sptr <IeFacility> _facility)
{
	ieFacility = _facility;
}
#endif

Sptr <IeDisplay> MsgFacility::getDisplay() const
{
	return ieDisplay;
}

void MsgFacility::setDisplay( Sptr <IeDisplay> _display)
{
	ieDisplay = _display;
}

		
int MsgFacility::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
#if 0	
		p = myCtrl->getDecoder()->getFacilityPoint4FacilityMsg(iesData);
		ieFacility = new IeFacility( p);
#endif		
		p = NULL;
//		p = myCtrl->getDecoder()->getCause4DisConnMsg(iesData);
		if ( p != NULL)
			ieDisplay = new IeDisplay( p);
		
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode FACILITY msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_ERR, "failed in decode FACILITY msg");
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgFacility::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : FACILITY");
	IsdnMsg::debugInfo();

#if 0
	p = ieFacility->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
#endif
	if(ieDisplay!=0)
	{
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p);
		free((void *)p);
	}	
}
#endif

int MsgFacility::getMsgType() const
{
	return MT_FACILITY;
}

int MsgFacility::getCCType() const
{
	return CC_FACILITY;
}

void MsgFacility::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4FacilityMsg(header, ieDisplay->encode( msg, ntMode) );
	}

	return ;
}

const char *MsgFacility::getName()
{
	return "ISDN-FACILITY";
}

