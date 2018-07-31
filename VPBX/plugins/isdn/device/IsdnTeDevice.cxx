/*
* $Id: IsdnTeDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "Sptr.hxx"

//#include "InsPort.hxx"
#include "IsdnMsg.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoderTE.hxx"
#include "MsgEncoderTE.hxx"

#include "IsdnTeDevice.hxx"

#include "IsdnEndPoint.hxx"

#include "IeCause.hxx"
#include "MsgReleaseComplete.hxx"

#include "PbxAgent.hxx"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#include "l3ins.h"
#include "as_isdn.h"
#include "vpbx_globals.h"

#include "IsdnEvent.hxx"

#include "EpState.hxx"

using namespace Assist;



IsdnTeDevice::IsdnTeDevice(DeviceMgr *_parent, int fd,int _portNum, int _ptp )
	:IsdnCtrlDevice(_parent, fd, _portNum, _ptp, "TE-Device" )
{
	myUppestLayer = 4;
	myUppestProtocol = ISDN_PID_L4_CAPI20;
	myDecoder = new MsgDecoderTE();
	myEncoder = new MsgEncoderTE();
	ntMode = ISDN_DEV_TYPE_USER;

	cpLog(LOG_DEBUG, "ISDN TE Init %s\n", getName() );
	if( init() )
		assert(0);
	cpLog(LOG_DEBUG, "ISDN TE Init %s\n", getName() );
}

IsdnTeDevice::~IsdnTeDevice()
{
	cpLog(LOG_DEBUG, "Destorying.....");
}


void IsdnTeDevice::myInit(stack_info_t *stack_info)
{
	cpLog(LOG_DEBUG, "ISDN TE Init %s\n", getName() );
	
	if (stack_info->pid.protocol[1] == 0)
	{
		cpLog(LOG_ERR, "Given device %s : Missing layer 1 protocol.\n", getName());
	}
	if (stack_info->pid.protocol[2] == 0)
	{
		cpLog(LOG_ERR, "Given device %s : Missing layer 2 protocol.\n", getName());
	}
	if (stack_info->pid.protocol[3] == 0)
	{
		cpLog(LOG_ERR, "Given device %s : Missing layer 3 protocol.\n", getName());
	}
	else
	{
		switch(stack_info->pid.protocol[3] & ~ISDN_PID_FEATURE_MASK)
		{
			case ISDN_PID_L3_DSS1USER:
				cpLog(LOG_DEBUG_STACK, "device %s : Layer 3 DSS1 User\n", getName());
				break;

			case ISDN_PID_L3_INS_USER:
				cpLog(LOG_DEBUG_STACK, "device %s : Layer 3 INS User(Japan)\n", getName());
				break;

			default:
				cpLog(LOG_ERR, "Given device %s : own protocol 0x%08x\n", getName(), stack_info->pid.protocol[3]);
		}

		cpLog(LOG_ERR, "Given device %s : own L3 protocol 0x%08x\n", getName(), stack_info->pid.protocol[3]);
	}
	
	if (stack_info->pid.protocol[4])
	{
		cpLog(LOG_ERR, "Given device %s : Layer 4 protocol %x not allowed.\n", getName(),stack_info->pid.protocol[4] );
//		assert(0);
		delUpperLayer(4);
	}

	if ((stack_info->pid.protocol[2]&ISDN_PID_L2_DF_PTP) )
	{
		cpLog(LOG_DEBUG_STACK, "\tPort is point-to-point.\n");
		myPtp = 1;
	}
	
}

void IsdnTeDevice::initStack()
{
}

int IsdnTeDevice::activate()
{
	/* if ptp AND te-mode, query state link */
//	if (/* dev->ptp && */ )
	{
		iframe_t act;
		/* L2 */
#ifdef DL_STATUS
		act.prim = DL_STATUS | REQUEST; 
#else
		act.prim = DL_ESTABLISH | REQUEST; 
#endif
		act.prim = DL_ESTABLISH | REQUEST; 
//act.addr = (myUpperId & IF_ADDRMASK) | IF_DOWN;
		act.addr = (myLowerId& IF_ADDRMASK) | IF_DOWN;
		act.dinfo = 0;
		act.len = 0;
		ask_isdn_write_without_lock( getFd() , &act, AS_ISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
	}

#if 0
	int ret = 0;
	char buf[4096];
	iframe_t *frm = (iframe_t *)buf;
	ret = ask_isdn_read( getFd(), buf, 1024, TIMEOUT_10SEC);
	if (ret>0)
	{
		cpLog(LOG_ERR, "D Channel activated , return prim is %x", frm->prim);
		if (frm->prim != (DL_ESTABLISH | CONFIRM))
		{
			cpLog(LOG_ERR, "D Channel activated failed" );
		}	
	}
	else
	{
		cpLog(LOG_ERR, "D Channel not activated now");
		assert(0);
	}
#endif
	l2Link = 1;

	return 0;
}

/* DL layer in D Channel of this port device */
int IsdnTeDevice::maintainDataLink( )
{
#if 0	
	if (dev->l1timeout && now>dev->l1timeout)
	{
		PDEBUG(DEBUG_ISDN, "the L1 establish timer expired, we release all pending messages.\n", dev->portnum);
		dev->l1timeout = 0;
	}		
#endif	

TRACE;
	if (l2Establish)
	{
TRACE;	
//		if (now-l2Establish > 5)
		{
			iframe_t act;

			cpLog(LOG_DEBUG_STACK,  "the L2 establish timer expired, we try to establish the link TE portnum=%d.\n", getPortNumber());
			l2Establish = 0;
			/* establish */
			act.prim = DL_ESTABLISH | REQUEST; 
			act.addr = myUpperId & IF_ADDRMASK | IF_DOWN;
			act.dinfo = 0;
			act.len = 0;
			ask_isdn_write_without_lock( getFd(), &act, AS_ISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
		}
		return 1;
	}
	return 0;
}

/* return msg handle state : 0, msg not handled by this function; others, handled */
/* all message below L3 is processed here */
int IsdnTeDevice::myIsdnHardwareMain( msg_t *msg)
{
 	iframe_t *frm = (iframe_t *)msg->data;
	if ((frm->addr&IF_ADDRMASK) == (unsigned int)myUpperId)
	{/* d-message */
		switch(frm->prim)
		{
			case PH_ACTIVATE | CONFIRM:
			case PH_ACTIVATE | INDICATION:
				cpLog(LOG_DEBUG_STACK,  "Received PH_ACTIVATE for %s.\n", getName());
				l1Link = 1;
				setupDownQueue( 1);
				goto l1_msg;

			case PH_DEACTIVATE | CONFIRM:
			case PH_DEACTIVATE | INDICATION:
				cpLog(LOG_DEBUG_STACK,  "Received PH_DEACTIVATE for %s.\n", getName());
				l1Link = 0;
				setupDownQueue( 0);
				goto l1_msg;

			case PH_CONTROL | CONFIRM:
			case PH_CONTROL | INDICATION:
				cpLog(LOG_DEBUG_STACK,  "Received PH_CONTROL for %s.\n", getName());
				return AS_MSG_NEED_FREE;

#ifdef DL_STATUS
			case DL_STATUS | INDICATION:
			case DL_STATUS | CONFIRM:
				switch(frm->dinfo)
				{
					case SDL_ESTAB:
						goto dl_estab;
					case SDL_REL:
						goto dl_rel;
				}
				return AS_MSG_NEED_FREE;
#endif

			case DL_ESTABLISH | INDICATION:
			case DL_ESTABLISH | CONFIRM:
#ifdef DL_STATUS
dl_estab:
#endif
				cpLog(LOG_DEBUG_STACK, "DL-ESTABLISH (0x%x) on %s", frm->prim, getName());
				if ( l2Establish)
				{
					l2Establish = 0;
					cpLog(LOG_DEBUG_STACK,  "the link became active before l2establish timer expiry.");
				}
				l2Link = 1;
				return AS_MSG_NEED_FREE;

			case DL_RELEASE | INDICATION:
			case DL_RELEASE | CONFIRM:
#ifdef DL_STATUS
dl_rel:
#endif
				cpLog(LOG_DEBUG_STACK,  "addr 0x%x releases data link (DL) TE portnum=%d\n", frm->addr, getPortNumber());
				l2Link = 0;
				if ( myPtp)
				{
					time(&l2Establish);
					cpLog(LOG_DEBUG_STACK,  "because we are ptp, we set a l2establish timer.\n");
				}
				return AS_MSG_NEED_FREE;

			default:
l1_msg:
				cpLog(LOG_DEBUG_STACK,  "GOT d-msg from device %s prim 0x%x dinfo 0x%x addr 0x%x\n", getName(), frm->prim, frm->dinfo, frm->addr);
				if (frm->dinfo==(signed long)0xffffffff && frm->prim==(PH_DATA|CONFIRM))
				{
					cpLog(LOG_ERR, "SERIOUS BUG, dinfo == 0xffffffff, prim == PH_DATA | CONFIRM !!!!\n");
				}
				
				/* all Call Message for D channel is passed to WorkerThread */


				return myL3MsgHandler( msg);
		}
		return AS_MSG_NEED_FREE;
	}

	/* go on to B Channel msgHandler */
	return AS_MSG_CONTINUE;
}

int IsdnTeDevice::myL3MsgHandler( msg_t *msg)
{
	IsdnMsg  *isdnMsg = IsdnMsgFactory::instance()->decodeRawMsg(msg, this);
	assert(isdnMsg != 0);

	Sptr <EndPoint> ep = lookupEPByFrame(  isdnMsg->getFrameInfo() );
	if(ep==NULL && isdnMsg->getType() != ISDN_TYPE_CC_NEW )
	{
		cpLog(LOG_DEBUG, "No EP are available for this ISDN MSG :frameInfo of MSG is 0x%x. Ignore this msg" ,isdnMsg->getFrameInfo());
//		assert(0);
		return AS_MSG_NEED_FREE;
	}
	
	if( ep==NULL && isdnMsg->getType()==ISDN_TYPE_CC_NEW)
	{
		ep = lookupFreeEP();
		if(ep == NULL)
		{
			IeCause *cause = new IeCause(CAUSE_LOCATION_LPN, CAUSE_T_NOCHANNEL_CIRCUIT_AVAILABLE);
			MsgReleaseComplete  *relCom = new MsgReleaseComplete(cause, this);
			relCom->sendOut( myEndPoints[0] );
			return AS_MSG_NEED_FREE;
		}
		Sptr <IsdnEndPoint> isdnEp = NULL;
		isdnEp.dynamicCast(ep);
		assert(isdnEp != 0);
		isdnEp->setFrameInfo( isdnMsg->getFrameInfo() );
		cpLog(LOG_DEBUG, "A new call has been received in EndPoint %s", isdnEp->getName() );
	}
#if 0	
	else if(ep->getState()->getStateType() == ISDN_EP_STATE_IDLE)
	{
		Sptr <IsdnEndPoint> isdnEp = NULL;
		isdnEp.dynamicCast(ep);
		assert(isdnEp != 0);
		cpLog(LOG_DEBUG, "found Free EndPoint %s for new call", isdnEp->getName() );
		
//		activate();
		

		/* set Call Reference (get from NEW_CR msg ) into this EndPoint */
		isdnEp->setFrameInfo( isdnMsg->getFrameInfo() );

#if WITH_DEBUG
		cpLog(LOG_DEBUG, "FrameInfo of EndPoint %s is %x, lookup frame is %x",isdnEp->getName(), isdnEp->getFrameInfo(), isdnEp->getFrameInfo() );
//		cpLog(LOG_DEBUG, "CallRef of EndPoint %s is %x, CallRef of ISDN %x",isdnEp->getName(), isdnEp->getCallRef(), isdnMsg->getCallRef() );
#endif
//		ep->setState( ep->findState(ISDN_EP_STATE_IN_SETUP) );
	}
#endif	
	else
	{
		cpLog(LOG_DEBUG, "Frame for a exist call in EndPoint %s (MSG '%s')" , ep->getName(),isdnMsg->getName());

	}

#if 0			
	/* only set CallRef of EndPoint when SETUP */
	if( isdnMsg->getType() == ISDN_TYPE_SETUP && ep != 0)
	{	
		Sptr <IsdnEndPoint> isdnEp = NULL;
		isdnEp.dynamicCast(ep);
		assert(isdnEp != 0);
//		cpLog(LOG_DEBUG, "Set CallRef for EndPoint %s with %x", isdnEp->getName(), isdnMsg->getCallRef() );
//		isdnEp->setCallRef( isdnMsg->getCallRef() );
	}
#endif

	IsdnEvent *devEvent = new  IsdnEvent( );

	devEvent->setEndPoint(ep );
	devEvent->setIsdnMsg( isdnMsg);

	PbxAgent::instance()->getGateway()->reportEvent(devEvent);

	cpLog(LOG_DEBUG, "Frame for a exist call in EndPoint %s (MSG '%s')" , ep->getName(), isdnMsg->getName());
	return AS_MSG_NEED_FREE;
}


int IsdnTeDevice::kernelTimerMsg( msg_t *msg)
{/* No kernel timer setup for TE protocol, so continue */
	return AS_MSG_CONTINUE;
}


/* sending message that were queued during L1 activation
 * or releasing port if link is down  */
void IsdnTeDevice::setupDownQueue( int link)
{
//	class Port *port = NULL;
#if 0
	class InsPort *port = NULL;
	message_t *mesg;


	/* check all port objects for pending message */
	while((port=(class InsPort *)parent->getNextActor( port) )  )
	{
		if ((port->getPortType()&PORT_CLASS_ISDN_MASK) == PORT_CLASS_ISDN_INS)
		{
//			pins = (class InsPort *)port;
			if (port->dev == this )
			{
				if (port->queue)
				{
					if (link)
					{
						cpLog(LOG_DEBUG_STACK, "the L1 became active, so we send queued message for portnum=%d (%s).\n", getDeviceNumber(), port->getName());

//???

						/* LAYER 1 is up, so we send */
//						port->message_setup(port->queue->id_from, port->queue->type, &port->queue->param);

						message_free(port->queue);
						port->queue = NULL;
					}
					else
					{
						cpLog(LOG_DEBUG_STACK, "the L1 became NOT active, so we release port for portnum=%d (%s).\n", getDeviceNumber(), port->getName());
//						mesg = pins->createMesg( pins->queue->id_from, MESSAGE_RELEASE);
						mesg = port->createMesg( port->queue->id_from, MESSAGE_RELEASE);
						mesg->param.disconnectinfo.cause = 27;
						mesg->param.disconnectinfo.location = LOCATION_PRIVATE_LOCAL;
						message_put(mesg);
						port->setState(PORT_STATE_RELEASE);
						port->deleteState = 1;
					}
				}
			}
		}

	}
#endif

}


//msg_t *IsdnTeDevice::myCreateL3Msg(int prim, int mt, int dinfo, int size)
msg_t *IsdnTeDevice::myCreateL3Msg(int prim, int mt, int size, Sptr <IsdnEndPoint> isdnEp)
{
	msg_t *dmsg = NULL;
	Q931_info_t *qi;
	iframe_t *frm;

	/* allocate enough space for frame's header, q931_info_t and IEs */
	dmsg = as_msg_alloc(size+256+AS_ISDN_HEADER_LEN+DEFAULT_HEADROOM);
	if (dmsg)
	{
		/* move 'tail' pointer */
//		memset(msg_put(dmsg,size+AS_ISDN_HEADER_LEN), 0, size+AS_ISDN_HEADER_LEN);
		memset(msg_put(dmsg, AS_ISDN_HEADER_LEN), 0, AS_ISDN_HEADER_LEN);
		frm = (iframe_t *)dmsg->data;
		frm->prim = prim;
		frm->addr = myUpperId & IF_ADDRMASK | IF_DOWN;
		frm->dinfo = isdnEp->getFrameInfo() ;

		if(mt != ISDN_L3_MT_INVALIDATE )
		{
			/* move 'tail' pointer */
			memset(msg_put(dmsg, sizeof(Q931_info_t) ), 0, sizeof(Q931_info_t ) );		
			qi = (Q931_info_t *)(dmsg->data + AS_ISDN_HEADER_LEN);

		/* move 1 byte for mt after Q931_info_t */
			unsigned char *p = msg_put(dmsg, 1);
			p[0] = mt;
			qi->type = mt;
			qi->crlen = 1;
//			qi->cr = isdnEp->getCallRef();
#if WITH_DEBUG	
//			cpLog(LOG_DEBUG_STACK, "after code, CallRef=0x%x CallRef in EP=0x%x(%s)", qi->cr,isdnEp->getCallRef(),isdnEp->getName() );
			cpLog(LOG_DEBUG_STACK, "after code, CallRef=0x%x (%s)", qi->cr,isdnEp->getName() );
#endif
		}
#if 0		
		else
		{
			cpLog(LOG_DEBUG, "invalidate L3 message type");
			assert(0);
		}
#endif		
	}
	else 
		assert(0);
	return dmsg;	
}

#if 0
Sptr <EndPoint> IsdnTeDevice::lookupEPByFrame(Sptr <IsdnMsg> msg ) 
{
	return lookupEPByFrame( msg->getFrameInfo() );
}
#endif

int IsdnTeDevice::sendOut(msg_t *msg, Sptr <EndPoint> ep)
{
	PbxDeviceEvent *devEvent = new PbxDeviceEvent();
	devEvent->setEndPoint( ep );
	devEvent->setPrivate((void *)msg);

	PbxAgent::instance()->getGateway()->sendHwCommand(devEvent);
	return 0;
}

