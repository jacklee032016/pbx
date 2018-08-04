/*
* $Id: IsdnMsgFactory.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"

#include "IsdnEvent.hxx"
#include "EndPoint.hxx"
#include "IsdnMsgFactory.hxx"
#include "IsdnMsg.hxx"

#include "MsgAlerting.hxx"
#include "MsgCCNew.hxx"
#include "MsgCCRelease.hxx"
#include "MsgCCTimeout.hxx"
#include "MsgConnect.hxx"
#include "MsgConnectAck.hxx"
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
//#include "IeCallState.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeKeypad.hxx"
#include "IeSignal.hxx"
#include "IeCallingPN.hxx"
#include "IeUserUser.hxx"
#include "IeCalledPN.hxx"
//#include "IeConnectPN.hxx"
//#include "IeComplete.hxx"
#include "IeRedirDN.hxx"
#include "IeRedirNR.hxx"

#include "MsgDecoder.hxx"
#include "MsgDecoderTE.hxx"
#include "MsgDecoderNT.hxx"
#include "IsdnCtrlDevice.hxx"

#include "CallMsgBuilder.hxx"


using namespace Assist;


#if 0
IsdnParserException::IsdnParserException( const string& msg, const string& file, const int line, const int error )
	:VException( msg, file, line, error )
{
}

string IsdnParserException::getName( void ) const
{
	return "IsdnParserException";
}
#endif

IsdnMsgFactory 	*IsdnMsgFactory::myInstance = 0;

IsdnMsgFactory *IsdnMsgFactory::instance()
{
	if(myInstance==0)
	{
		myInstance = new IsdnMsgFactory();
	}

	return myInstance;
}

IsdnMsgFactory::IsdnMsgFactory()
{
}

IsdnMsgFactory::~IsdnMsgFactory()
{
}


IsdnMsg* IsdnMsgFactory::decodeRawMsg(const msg_t  *msg, IsdnCtrlDevice *ctrl )
{
	IsdnMsg *isdnMsg = NULL;
	ISDN_METHOD isdnMethod;

	assert( msg!=0);

#if WITH_Q931_DEBUG
//	debugQ931Info( frm);
#endif

	isdnMethod  = ctrl->getDecoder()->getType(msg);

	if(isdnMethod == ISDN_TYPE_UNKNOWN)
	{
		cpLog(LOG_DEBUG_STACK, "ISDN MT is not validate" );
		return NULL;
	}

	switch(isdnMethod)
	{
		case ISDN_TYPE_ALERTING:
			try
			{
				isdnMsg = new MsgAlerting(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the ALERTING msg");
			}
			break;
			
		case ISDN_TYPE_CC_NEW:
		case ISDN_TYPE_NEW_CR:/* new Call Reference */	
			try
			{
				isdnMsg = new MsgCCNew(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the CC_NEW msg");
			}
			break;
		case ISDN_TYPE_RELEASE_CR:	
			try
			{
				isdnMsg = new MsgCCRelease(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the CC_RELEASE msg");
			}
			break;

		case ISDN_TYPE_CC_TIMEOUT:
			try
			{
				isdnMsg = new MsgCCTimeout(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the CC_TIMEOUT msg");
			}
			break;
		case ISDN_TYPE_CONNECT:
			try
			{
				isdnMsg = new MsgConnect(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the CONNECT msg");
			}
			break;
		case ISDN_TYPE_CONNECT_ACK:
			try
			{
				isdnMsg = new MsgConnectAck(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the CONNECT_ACK msg");
			}
			break;
		case ISDN_TYPE_DISCONNECT:
			try
			{
				isdnMsg = new MsgDisConnect(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the DIS-CONNECT msg");
			}
			break;


		case ISDN_TYPE_FACILITY:
			try
			{
				isdnMsg = new MsgFacility(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the FACILITY msg");
			}
			break;
		case ISDN_TYPE_HOLD:
			try
			{
				isdnMsg = new MsgHold(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the HOLD msg");
			}
			break;
		case ISDN_TYPE_INFO:
			try
			{
				isdnMsg = new MsgInfo(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the INFO msg");
			}
			break;
		case ISDN_TYPE_NOTIFY:
			try
			{
				isdnMsg = new MsgNotify(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the NOTIFY msg");
			}
			break;
		case ISDN_TYPE_PROCEEDING:
			try
			{
				isdnMsg = new MsgProceeding(msg, ctrl);
			}
			catch (IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the PROCEEDING msg");
			}
			break;
		case ISDN_TYPE_RELEASE:
			try
			{
				isdnMsg = new MsgRelease(msg, ctrl); 
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the RELEASE msg");
			}
			break;
		case ISDN_TYPE_RELEASE_COMPLETE_IND:
		case ISDN_TYPE_RELEASE_COMPLETE_CFM:
			try
			{
				isdnMsg = new MsgReleaseComplete(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the RELEASE_COMPLETE msg");
			}
			break;
		case ISDN_TYPE_RESUME:
			try
			{
				isdnMsg = new MsgResume(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the RESUME msg");
			}
			break;


		case ISDN_TYPE_SETUP:
			try
			{
				isdnMsg = new MsgSetup(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the SETUP msg");
			}
			break;
#if 0
		case ISDN_TYPE_RETRIEVE:
			try
			{
				isdnMsg = new MsgRetrieve(frm);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the RETRIEVE msg");
			}
			break;
		case ISDN_TYPE_SETUP_ACK:
			try
			{
				isdnMsg = new MsgSetupAck(frm);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the SETUP-ACK msg");
			}
			break;
#endif
		case ISDN_TYPE_SUSPEND:
			try
			{
				isdnMsg = new MsgSuspend(msg, ctrl);
			}
			catch(IsdnMsgParserException&)
			{
				cpLog(LOG_DEBUG_STACK, "Failed in parsing the SUSPEND msg");
			}
			break;

		default:
			cpLog(LOG_DEBUG_STACK, "NOT A KNOWN ISDN L3 MESSAGE");
			assert(0);

	}

	if(isdnMsg != 0)
	{
//		isdnMsg->setFrameInfo( frm->dinfo );
		cpLog(LOG_DEBUG_STACK, " In IsdnMsgFactory: after decoding: %s", isdnMsg->getName() );
		isdnMsg->debugInfo();
	}

	return isdnMsg;
}


Sptr <IsdnMsg> copyPtrtoSptr(IsdnMsg* isdnMsg)
{
	assert(isdnMsg);
	Sptr <IsdnMsg> sptrIsdnMsg;

	switch( isdnMsg->getType() )
	{
		case ISDN_TYPE_ALERTING:
		{
			MsgAlerting* alertMsg = dynamic_cast<MsgAlerting*>( isdnMsg);
			if (alertMsg)
			{
				sptrIsdnMsg = new MsgAlerting(*alertMsg);
			}
			break;
		}

		case ISDN_TYPE_PROCEEDING:
		{
			MsgProceeding* proceedingMsg = dynamic_cast<MsgProceeding*>(isdnMsg);
			if (proceedingMsg)
			{
				sptrIsdnMsg = new MsgProceeding (*proceedingMsg);
			}
			break;
		}

		case ISDN_TYPE_CONNECT:
		{
			MsgConnect* connectMsg = dynamic_cast<MsgConnect*>(isdnMsg);
			if (connectMsg)
			{
				sptrIsdnMsg = new MsgConnect (*connectMsg);
			}
			break;
		}

		case ISDN_TYPE_SUSPEND:
		{
			MsgSuspend* suspendMsg  = dynamic_cast<MsgSuspend*>(isdnMsg);
			if (suspendMsg)
			{
				sptrIsdnMsg = new MsgSuspend (*suspendMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME:
		{
			MsgResume* resumeMsg  = dynamic_cast<MsgResume*>(isdnMsg);
			if (resumeMsg)
			{
				sptrIsdnMsg = new MsgResume (*resumeMsg);
			}
			break;
		}

		case ISDN_TYPE_SETUP:
		{
			MsgSetup* msg  = dynamic_cast<MsgSetup*>(isdnMsg);
			if (msg)
			{
				sptrIsdnMsg = new MsgSetup (*msg);
			}
			break;
		}

#if 0
		case ISDN_TYPE_SETUP_ACK:
		{
			MsgSetupAck* setupAcKMsg  = dynamic_cast<MsgSetupAck*>(isdnMsg);
			if (setupAcKMsg)
			{
				sptrIsdnMsg = new MsgSetupAck (*setupAcKMsg);
			}
			break;
		}

		case ISDN_TYPE_CONNECT_ACK:
		{
			MsgConnectAck* connectAckMsg  = dynamic_cast<MsgConnectAck*>(isdnMsg);
			if (connectAckMsg)
			{
				sptrIsdnMsg = new MsgConnectAck (*connectAckMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME_ACK:
		{
			MsgResumeAck* resumeMsg  = dynamic_cast<MsgResumeAck*>(isdnMsg);
			if (resumeMsg)
			{
				sptrIsdnMsg = new MsgResumeAck (*resumeMsg);
			}
			break;
		}

		case ISDN_TYPE_RESUME_REJECT:
		{
			MsgResumeReject* resumeRejMsg  = dynamic_cast<MsgResumeReject*>(isdnMsg);
			if (resumeRejMsg)
			{
				sptrIsdnMsg = new MsgResumeReject (*resumeRejMsg);
			}
			break;
		}

		case ISDN_TYPE_STATUS:
		{
			IsdnStatusMsg* statusMsg = dynamic_cast<IsdnStatusMsg*>(isdnMsg);
			if (statusMsg)
			{
				sptrIsdnMsg = new IsdnStatusMsg (*statusMsg);
			}
			break;
		}

		case ISDN_TYPE_SUSPEND_ACK:
		{
			IsdnMsgSuspendAck* suspendAckMsg  = dynamic_cast<IsdnMsgSuspendAck*>(isdnMsg);
			if (suspendAckMsg)
			{
				sptrIsdnMsg = new IsdnMsgSuspendAck (*suspendAckMsg);
			}
			break;
		}
		case ISDN_TYPE_PROGRESS:
		{
			IsdnMsgProgress* progressMsg = dynamic_cast<IsdnMsgProgress*>(isdnMsg);
			if (progressMsg)
			{
				sptrIsdnMsg = new IsdnMsgProgress(*progressMsg);
			}
			break;
		}
#endif

		default:
		{
#if 0			

			UnknownExtensionMsg* unknownExtensionMsg = dynamic_cast<UnknownExtensionMsg*>(isdnMsg);
			if (unknownExtensionMsg)
			{
				sptrIsdnMsg = new UnknownExtensionMsg (*unknownExtensionMsg);
			}
#endif
			cpLog(LOG_DEBUG, "Unknown Msg Type" );
			assert(0);
			break;
		}
	}

	assert (sptrIsdnMsg != 0);
	return sptrIsdnMsg;
}


#if WITH_Q931_DEBUG
static char* q931MtName[] = 
{
	"STATUS",	
	"CC_TIMEOUT",	
	"CC_NEW",
	"SETUP",
	"INFO",
	"SETUP_ACK",
	"PROCEEDING",
	"PROGRESS",
	"ALERTING",
	"CONNECT",
	"CONNECT_ACK",
	"DISCONNECT",
	"RELEASE",
	"RELEASE_COMPLETE_IND",
	"RELEASE_COMPLETE_CFM",
	"NOTIFY",
	"HOLD",
	"RETRIEVE",
	"SUSPEND",
	"SUSPEND_ACK",
	"SUSPEND_REJECT",
	"RESUME",
	"RESUME_ACK",
	"RESUME_REJECT",
	"FACILITY",
	"RELEASE_CR",
	"NEW_CR",
	"UNKNOWN"	
} ;



/* only support TE mode */
void IsdnMsgFactory::debugQ931Info(const iframe_t *_frm)
{
	ISDN_METHOD type = ISDN_TYPE_UNKNOWN;//getType(_frm);
	cpLog(LOG_DEBUG_STACK, "Frame Info : Address-0x%x Prim-0x%x dInfo-0x%x Length-%d" , _frm->addr, _frm->prim, _frm->dinfo, _frm->len );
#if 0	
	unsigned char *p = (unsigned char *)((unsigned char *)_frm + AS_ISDN_HEADER_LEN/*16*/ );
	
	Q931_info_t *qi = (Q931_info_t *)p ; //((unsigned char *)_frm->data+2);
	cpLog(LOG_DEBUG_STACK, "Q931 Info : Frame length %d(q931 length=%d)" , _frm->len, sizeof(Q931_info_t) );
#else
	Q931_info_t *qi = MsgDecoderTE().getQ931Info(_frm);
#endif

	ask_hex_dump((void *)qi, _frm->len);
	cpLog(LOG_DEBUG_STACK, "\tMT\t : %s", (type>=0 && type<ISDN_TYPE_UNKNOWN)?q931MtName[type]:"incorrect type index" );
	if(_frm->len == 0)
	{
		cpLog(LOG_DEBUG_STACK, "\tData Length in FRAME in zero" );
		return;
	}

	cpLog(LOG_DEBUG_STACK, "\tMT \t\t : 0x%x" , qi->type );
	cpLog(LOG_DEBUG_STACK, "\tCR Length\t : %d\t\tCR\t :0x%x", qi->crlen, qi->cr);
#if 0	
	cpLog(LOG_DEBUG_STACK, "\tBEARER\t\t : %d", (qi->bearer_capability ) );
	cpLog(LOG_DEBUG_STACK, "\tCallID\t\t : %d", qi->call_id);
	cpLog(LOG_DEBUG_STACK, "\tCAUSE\t\t : %d", qi->cause);
	cpLog(LOG_DEBUG_STACK, "\tCall State\t : %d", qi->call_state);
	cpLog(LOG_DEBUG_STACK, "\tChannel ID\t : %d", qi->channel_id);
	cpLog(LOG_DEBUG_STACK, "\tFACILITY\t : %d", qi->facility);
	cpLog(LOG_DEBUG_STACK, "\tPROGRESS\t : %d", qi->progress);
	cpLog(LOG_DEBUG_STACK, "\tNET FACI\t : %d", qi->net_fac);
	cpLog(LOG_DEBUG_STACK, "\tNOTIFY\t\t : %d", qi->notify);
	cpLog(LOG_DEBUG_STACK, "\tDISPLAY\t\t : %d", qi->display);
	cpLog(LOG_DEBUG_STACK, "\tDATE\t\t : %d", qi->date);
	cpLog(LOG_DEBUG_STACK, "\tKEYPAD\t\t : %d", qi->keypad);
	cpLog(LOG_DEBUG_STACK, "\tSIGNAL\t\t : %d", qi->signal);
	cpLog(LOG_DEBUG_STACK, "\tINFO RATE\t : %d", qi->info_rate);
	cpLog(LOG_DEBUG_STACK, "\tE2E Transit\t : %d", qi->end2end_transit);
	cpLog(LOG_DEBUG_STACK, "\tCLOSED USER\t : %d", qi->closed_userg);
	cpLog(LOG_DEBUG_STACK, "\tCONNECT NR\t : %d", qi->connected_nr);
	cpLog(LOG_DEBUG_STACK, "\tCONNECT SUB\t : %d", qi->connected_sub);
	cpLog(LOG_DEBUG_STACK, "\tCALLING NR\t : %d", qi->calling_nr);
	cpLog(LOG_DEBUG_STACK, "\tCALLING SUB\t : %d", qi->calling_sub);
	cpLog(LOG_DEBUG_STACK, "\tCALLED NR\t : %d", qi->called_nr);
	cpLog(LOG_DEBUG_STACK, "\tCALLED SUB\t : %d", qi->called_sub);
	cpLog(LOG_DEBUG_STACK, "\tREDIR NR\t : %d", qi->redirect_nr);
	cpLog(LOG_DEBUG_STACK, "\tTRANS NET\t : %d", qi->transit_net_sel);
	cpLog(LOG_DEBUG_STACK, "\tRESTART IND\t : %d", qi->restart_ind);
	cpLog(LOG_DEBUG_STACK, "\tUSERUSER\t : %d", qi->useruser);
	cpLog(LOG_DEBUG_STACK, "\tMORE DATA\t : %d", qi->more_data);
	cpLog(LOG_DEBUG_STACK, "\tCOMPLETE\t : %d", qi->sending_complete);
	cpLog(LOG_DEBUG_STACK, "\tCONGESTION\t : %d", qi->congestion_level);

	cpLog(LOG_DEBUG_STACK, "\tTransit Delay Sel\t : %d" ,qi->transit_delay_sel);
	cpLog(LOG_DEBUG_STACK, "\tPKT PARA\t : %d", qi->pktl_bin_para);
	cpLog(LOG_DEBUG_STACK, "\tPKT WINDOW\t : %d", qi->pktl_window);
	cpLog(LOG_DEBUG_STACK, "\tPKT SIZE\t : %d", qi->pkt_size);
	cpLog(LOG_DEBUG_STACK, "\tLLC\t\t : %d", qi->llc);
	cpLog(LOG_DEBUG_STACK, "\tHLC\t\t : %d", qi->hlc);

	cpLog(LOG_DEBUG_STACK, "\tFILL\t : %d", qi->fill1);
#endif

}
#endif

