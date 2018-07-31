/*
* $Id: MsgResume.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
 
#include "global.h"
#include "cpLog.h"

#include "MsgResume.hxx"

#include "IeCallId.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgResume::MsgResume(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &)
	:IsdnMsg(msg, ctrl)
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

MsgResume::MsgResume(IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgResume::MsgResume(const MsgResume& src)
        : IsdnMsg(src)
{
}

MsgResume::~MsgResume()
{
}

MsgResume& MsgResume::operator=( const MsgResume& src )
{
	if ( &src != this )
	{
		IsdnMsg::operator=( src );
		this->ieCallId = src.ieCallId;
	}

	return ( *this );
}


bool MsgResume::operator ==(const MsgResume& src) 
{
	if(this->ieCallId==src.ieCallId)
		return true;
	return false;
}


ISDN_METHOD MsgResume::getType() const
{
	return ISDN_TYPE_RESUME;
}

Sptr <IeCallId>  MsgResume::getCallId() const
{
	return ieCallId;
}

void MsgResume::setCallId(Sptr <IeCallId> callId)
{
	ieCallId = callId;
}

int MsgResume::decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p = NULL;

	try
	{
//		p = myCtrl->getDecoder()->getChanId4ProceedingMsg( frm);
		if (p != NULL)
			ieCallId = new IeCallId( p);
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode RESUME msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_ERR, "failed in decode RESUME msg" );
	}
	
	return AS_OK;
}


#if WITH_DEBUG
void MsgResume::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : CALL ID");
	IsdnMsg::debugInfo();

	if( ieCallId != 0)
	{
		p = ieCallId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	
}
#endif

int MsgResume::getMsgType() const
{
	return MT_RESUME;
}

int MsgResume::getCCType() const
{
	return CC_RESUME;
}

void MsgResume::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	/* this is a optional IE */
	if( ieCallId!= 0)
	{
		myCtrl->getEncoder()->encCallId4ResumeMsg(header, ieCallId->encode( msg, ntMode) );
	}

	return ;
}

const char *MsgResume::getName()
{
	return "ISDN-RESUME";
}

