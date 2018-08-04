/*
* $Id: IsdnMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"
#include <strstream>

#include "IsdnMsg.hxx"

#include "MsgAlerting.hxx"
#include "MsgCCNew.hxx"
#include "MsgConnect.hxx"
#include "MsgDisConnect.hxx"
#include "MsgFacility.hxx"
#include "MsgHold.hxx"
#include "MsgInfo.hxx"
#include "MsgNotify.hxx"
#include "MsgProceeding.hxx"
#include "MsgRelease.hxx"
#include "MsgReleaseComplete.hxx"
#include "MsgResume.hxx"
#include "MsgSetup.hxx"
#include "MsgSuspend.hxx"

//#include "MsgSetupAck.hxx"
//#include "MsgRetrieve.hxx"

#include "IeBearer.hxx"
#include "IeCause.hxx"
#include "IeCallId.hxx"
#include "IeCallState.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeKeypad.hxx"
#include "IeCallingPN.hxx"
#include "IeCalledPN.hxx"
#include "IeCallSub.hxx"
#include "IeRedirNR.hxx"

#include "IsdnParserMode.hxx"
#if WITH_Q931_DEBUG
#include "IsdnMsgFactory.hxx"
#endif
#include "MsgDecoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

#include "PbxDeviceEvent.hxx"
#include "PbxAgent.hxx"
#include "GatewayMgr.hxx"

//#include "MsgDecoderTE.hxx"
#include "as_isdn.h"
#include "asnt_nins.h"

using namespace Assist;

IsdnMsgParserException::IsdnMsgParserException( const string& msg, const string& file,  const int line,  const int error )
	: VException( msg, file, line,  error),
	value(error)
{
}

string IsdnMsgParserException::getName( void ) const
{
	return "ISDN MSG ParserException";
}

IsdnMsg::IsdnMsg(const msg_t *_msg, IsdnCtrlDevice *ctrl) throw(IsdnMsgParserException &)
	:myCtrl(ctrl)
{

	if( ctrl->getNtMode() ==ISDN_DEV_TYPE_USER)
	{
		iframe_t *frm = (iframe_t *)_msg->data;
		frameInfo = frm->dinfo;		
	}
	else
	{
		ASNT_ISDNUSER_HEADER *hh = (ASNT_ISDNUSER_HEADER *)_msg->data;
		frameInfo = hh->dinfo;
	}
#if 0	
	Q931_info_t *qi = MsgDecoderTE().getQ931Info(_frm);
	frameInfo = _frm->dinfo;
	cpLog(LOG_DEBUG_STACK, "frameInfo 0x%x in ISDN MSG", frameInfo );

	if(_frm->len>0)
		callRef = qi->cr;
	else
		callRef = -1;
	cpLog(LOG_DEBUG_STACK, "frameInfo 0x%x, callRef 0x%x in ISDN MSG", frameInfo, callRef );
#endif

#if WITH_Q931_DEBUG
//	IsdnMsgFactory::instance()->debugQ931Info( _frm);
#endif
#if 0
#endif	
}

IsdnMsg::IsdnMsg(IsdnCtrlDevice *ctrl)
	:myCtrl(ctrl)
{
}

    
IsdnMsg::IsdnMsg(const IsdnMsg& src)
{ 

}

IsdnMsg::~IsdnMsg()
{
	cpLog(LOG_DEBUG, "IsdnMsg deconstructing.....");
}

IsdnMsg& IsdnMsg::operator=(const IsdnMsg& newMsg)
{
	if ( &newMsg != this) 
	{
	}
	return *(this);
}

bool IsdnMsg::operator<(const IsdnMsg& src) const
{
	cpLog(LOG_INFO,  "not verified!");
	assert(0);
	return false;
}
   
bool IsdnMsg::operator==(const IsdnMsg& src) const
{
	bool equal = true;
	if(this->myCtrl != src.myCtrl)
		equal = false;
	cpLog(LOG_DEBUG_STACK, "IsdnMsg::operator== returns %s",  equal ? "true" : "false");
	return equal;
}


Sptr<IsdnMsg> IsdnMsg::clone()
{
	return copyPtrtoSptr(this);
}

Sptr<IsdnMsg> Assist::copyPtrtoSptr(IsdnMsg* isdnMsg)
{
	assert(isdnMsg);
	Sptr<IsdnMsg> sptrIsdnMsg;

	switch(isdnMsg->getType())
	{
		case ISDN_TYPE_ALERTING:
		{
			MsgAlerting* ackMsg = dynamic_cast<MsgAlerting*>(isdnMsg);
			if (ackMsg)
			{
				sptrIsdnMsg = new MsgAlerting(*ackMsg);
			}
			break;
		}

		case ISDN_TYPE_CC_NEW:
		{
			MsgCCNew  *newMsg = dynamic_cast<MsgCCNew  *>(isdnMsg);
			if ( newMsg)
			{
				sptrIsdnMsg = new MsgCCNew( *newMsg);
			}
			break;
		}

		case ISDN_TYPE_CONNECT:
		{
			MsgConnect* connMsg = dynamic_cast<MsgConnect*>(isdnMsg);
			if (connMsg)
			{
				sptrIsdnMsg = new MsgConnect (*connMsg);
			}
			break;
		}

		case ISDN_TYPE_DISCONNECT:
		{
			MsgDisConnect* disConnMsg = dynamic_cast<MsgDisConnect*>(isdnMsg);
			if ( disConnMsg)
			{
				sptrIsdnMsg = new MsgDisConnect(*disConnMsg);
			}
			break;
		}

		case ISDN_TYPE_FACILITY:
		{
			MsgFacility *faciMsg = dynamic_cast<MsgFacility *>(isdnMsg);
			if ( faciMsg)
			{
				sptrIsdnMsg = new MsgFacility (*faciMsg);
			}
			break;
		}

		case ISDN_TYPE_HOLD:
		{
			MsgHold *holdMsg  = dynamic_cast<MsgHold *>(isdnMsg);
			if ( holdMsg)
			{
				sptrIsdnMsg = new MsgHold ( *holdMsg);
			}
			break;
		}

		case ISDN_TYPE_INFO:
		{
			MsgInfo *infoMsg  = dynamic_cast<MsgInfo *>(isdnMsg);
			if ( infoMsg)
			{
				sptrIsdnMsg = new MsgInfo ( *infoMsg);
			}
			break;
		}

		case ISDN_TYPE_NOTIFY:
		{
			MsgNotify *notiMsg  = dynamic_cast<MsgNotify *>(isdnMsg);
			if ( notiMsg)
			{
				sptrIsdnMsg = new MsgNotify ( *notiMsg);
			}
			break;
		}

		case ISDN_TYPE_PROCEEDING:
		{
			MsgProceeding* proMsg = dynamic_cast<MsgProceeding*>(isdnMsg);
			if (proMsg)
			{
				sptrIsdnMsg = new MsgProceeding (*proMsg);
			}
			break;
		}

		case ISDN_TYPE_RELEASE:
		{
			MsgRelease  *releMsg  = dynamic_cast<MsgRelease *>(isdnMsg);
			if ( releMsg)
			{
				sptrIsdnMsg = new MsgRelease( *releMsg);
			}
			break;
		}

		case ISDN_TYPE_RELEASE_COMPLETE_IND:
		{
			MsgReleaseComplete  *relComMsg  = dynamic_cast<MsgReleaseComplete *>(isdnMsg);
			if ( relComMsg)
			{
				sptrIsdnMsg = new MsgReleaseComplete( *relComMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME:
		{
			MsgResume* resMsg  = dynamic_cast<MsgResume*>(isdnMsg);
			if ( resMsg)
			{
				sptrIsdnMsg = new MsgResume (*resMsg);
			}
			break;
		}

		case ISDN_TYPE_SETUP:
		{
			MsgSetup* setupMsg  = dynamic_cast<MsgSetup *>(isdnMsg);
			if (setupMsg)
			{
				sptrIsdnMsg = new MsgSetup (*setupMsg);
			}
			break;
		}

#if 0
		/* folllowing MSG are not defined or not offered in INS */
		case ISDN_TYPE_RETRIEVE:
		{
			MsgRetrieve  *retMsg  = dynamic_cast<MsgRetrieve *>(isdnMsg);
			if ( retMsg)
			{
				sptrIsdnMsg = new MsgRetrieve (*retMsg);
			}
			break;
		}

		case ISDN_TYPE_SETUP_ACK:
		{
			MsgSetupAck *msg  = dynamic_cast<MsgSetupAck*>(isdnMsg);
			if (msg)
			{
				sptrIsdnMsg = new MsgSetupAck (*msg);
			}
			break;
		}
#endif
		case ISDN_TYPE_SUSPEND:
		{
			MsgSuspend* suspMsg  = dynamic_cast<MsgSuspend*>(isdnMsg);
			if ( suspMsg)
			{
				sptrIsdnMsg = new MsgSuspend (*suspMsg);
			}
			break;
		}

#if 0
		case ISDN_TYPE_CONNECT_ACK:
		{
			MsgConnectAck* connMsg  = dynamic_cast<MsgConnectAck*>(isdnMsg);
			if ( connMsg)
			{
				sptrIsdnMsg = new MsgConnectAck (*connMsg);
			}
			break;
		}

		case ISDN_TYPE_SUSPEND_ACK:
		{
			MsgSuspendAck* suspMsg  = dynamic_cast<IsdnMsgSuspendAck*>(isdnMsg);
			if ( suspMsg)
			{
				sptrIsdnMsg = new MsgSuspendAck (*suspMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME_ACK:
		{
			MsgResumeAck* referMsg  = dynamic_cast<MsgResumeAck*>(isdnMsg);
			if (referMsg)
			{
				sptrIsdnMsg = new MsgResumeAck (*referMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME_REJECT:
		{
			MsgResumeReject* resMsg  = dynamic_cast<MsgResumeReject*>(isdnMsg);
			if ( resMsg)
			{
				sptrIsdnMsg = new MsgResumeReject (*resMsg);
			}
			break;
		}
#endif
		default:
		{
			cpLog(LOG_ERR, "IsdnMsg type '%d' is not support", isdnMsg->getType() );
			assert(0);
/*			
			UnknownExtensionMsg* unknownExtensionMsg = dynamic_cast<UnknownExtensionMsg*>(isdnMsg);
			if (unknownExtensionMsg)
			{
				sptrIsdnMsg = new UnknownExtensionMsg (*unknownExtensionMsg);
			}
*/			
			break;
		}
	}

	assert (sptrIsdnMsg != 0);

	return sptrIsdnMsg;
}

ostream& IsdnMsg::brief(ostream& strm) const
{
	assert(this);
#if 0	
	switch (getType())
	{
		case SIP_UNKNOWN:
			strm <<  "UNKNOWN";
			break;
		case SIP_INVITE:
			strm <<  "INVITE";
			break;
		case SIP_ACK:
			strm <<  "ACK";
			break;
		case SIP_OPTIONS:
			strm <<  "OPTIONS";
			break;
		case SIP_BYE:
			strm <<  "BYE";
			break;
		case SIP_CANCEL:
			strm <<  "CANCEL";
			break;
		case SIP_REGISTER:
			strm <<  "REGISTER";
			break;
		case SIP_INFO:
			strm <<  "INFO";
			break;
		case SIP_TRANSFER:
			strm <<  "TRANSFER";
			break;
		case SIP_REFER:
			strm <<  "REFER";
			break;
		case SIP_NOTIFY:
			strm <<  "NOTIFY";
			break;
		case SIP_SUBSCRIBE:
			strm <<  "SUBSCRIBE";
			break;
		case SIP_STATUS:
		{
			const IsdnStatusMsg* p = dynamic_cast<const IsdnStatusMsg*>(this);
			assert (p != 0);
			strm << "STATUS (" << p->getStatusLine().getStatusCode() << ")";
			break;
		}
		case SIP_MESSAGE:
			strm <<  "MESSAGE ";
			if (getNumContentData())
			{
				Sptr<SipContentData> data = getContentData(0);
				int l;
				strm << "(" << data->encodeBody(l) << ")";
			}
			break;
		default:
			strm << "TYPE NOT PRINTABLE";
			break;
	}
	strm << " ["  << getCallId().getCallIdData() << " : " << getCSeq().getMethod() << " " << getCSeq().getCSeq() << "]";
#endif

	return strm;
}

#if 0
void IsdnMsg::setFrameInfo(int _frameInfo)
{
	frameInfo = _frameInfo;
}
#endif

int IsdnMsg::getFrameInfo()
{
	return frameInfo;
}

#if WITH_DEBUG
void IsdnMsg::debugInfo()
{
	cpLog(LOG_DEBUG_STACK, "frameInfo \t : %x", frameInfo );
}
#endif

#if 0
void IsdnMsg::setCallRef(int _callRef)
{
	callRef = _callRef;
}

int IsdnMsg::getCallRef()
{
	return callRef;
}
#endif

/* send to DeviceCommandThread */
int IsdnMsg::sendOut(Sptr <EndPoint> ep)
{
	msg_t *msg;
	assert(ep != 0);
	
	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast( ep);
	assert(isdnEp != 0);
	
	Sptr <CtrlDevice> ctrl = isdnEp->getCtrlDevice();
	Sptr <IsdnCtrlDevice> isdnCtrl = NULL;
	isdnCtrl.dynamicCast( ctrl);
	
	msg = isdnCtrl->createL3Msg( getCCType() |REQUEST, getMsgType(), getMsgHeaderLength(), isdnEp );

	encode(msg);

	myCtrl->sendOut(msg, ep);
	return 0;
}

IsdnCtrlDevice *IsdnMsg::getCtrlDevice()
{
	return myCtrl;
}

int IsdnMsg::getMsgHeaderLength()
{
	if( myCtrl->getNtMode() != 0)
	{
		ISDN_METHOD type = getType();
		switch(type)
		{
			case ISDN_TYPE_STATUS:
				return sizeof(STATUS_t);
				break;
			case ISDN_TYPE_CC_TIMEOUT:
				return 0;
				break;
			case ISDN_TYPE_CC_NEW:
				return 0;
				break;
			case ISDN_TYPE_SETUP:
				return sizeof(SETUP_t);
				break;
			case ISDN_TYPE_INFO:
				return sizeof(INFORMATION_t);
				break;
			case ISDN_TYPE_SETUP_ACK:
				return sizeof(SETUP_ACKNOWLEDGE_t);
				break;
			case ISDN_TYPE_PROCEEDING:
				return sizeof(CALL_PROCEEDING_t);
				break;
			case ISDN_TYPE_PROGRESS:
				return sizeof(PROGRESS_t);
				break;
			case ISDN_TYPE_ALERTING:
				return sizeof(ALERTING_t);
				break;
			case ISDN_TYPE_CONNECT:
				return sizeof(CONNECT_t);
				break;
			case ISDN_TYPE_CONNECT_ACK:
				return sizeof(CONNECT_ACKNOWLEDGE_t);
				break;
			case ISDN_TYPE_DISCONNECT:
				return sizeof(DISCONNECT_t);
				break;
			case ISDN_TYPE_RELEASE:
				return sizeof(RELEASE_t);
				break;
			case ISDN_TYPE_RELEASE_COMPLETE_IND:
			case ISDN_TYPE_RELEASE_COMPLETE_CFM:
				return sizeof(RELEASE_COMPLETE_t);
				break;
			case ISDN_TYPE_NOTIFY:
				return sizeof(NOTIFY_t);
				break;
			case ISDN_TYPE_HOLD:
				return sizeof(HOLD_t);
				break;
			case ISDN_TYPE_RETRIEVE:
				return sizeof(RETRIEVE_t);
				break;
			case ISDN_TYPE_SUSPEND:
				return sizeof(SUSPEND_t);
				break;
			case ISDN_TYPE_SUSPEND_ACK:
				return sizeof(SUSPEND_ACKNOWLEDGE_t);
				break;
			case ISDN_TYPE_SUSPEND_REJECT:
				return sizeof(SUSPEND_REJECT_t);
				break;
			case ISDN_TYPE_RESUME:
				return sizeof(RESUME_t);
				break;
			case ISDN_TYPE_RESUME_ACK:
				return sizeof(RESUME_ACKNOWLEDGE_t);
				break;
			case ISDN_TYPE_RESUME_REJECT:
				return sizeof(RESUME_REJECT_t);
				break;
			case ISDN_TYPE_FACILITY:
				return sizeof(FACILITY_t);
				break;
			case ISDN_TYPE_RELEASE_CR:
				return 0;
				break;
			case ISDN_TYPE_NEW_CR:
				return 0;
				break;
			case ISDN_TYPE_UNKNOWN:
				return -1;
				break;
			default:
				return -1;
				break;
				
		}
	}
	else
		return 64;
}

