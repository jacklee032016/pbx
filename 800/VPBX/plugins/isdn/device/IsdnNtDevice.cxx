/*
* $Id: IsdnNtDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"

#include "IsdnNtDevice.hxx"
#include "IsdnDeviceMgr.hxx"
#include "MsgDecoderNT.hxx"
#include "MsgEncoderNT.hxx"
#include "IsdnMsgFactory.hxx"
#include "EndPoint.hxx"
#include "IsdnEndPoint.hxx"
#include "PbxTimerEvent.hxx"
#include "PbxAgent.hxx"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "asnt_nins.h"
#if __ARM_IXP__
#include <arm/bitops.h>
#else
#include <asm/bitops.h>
#endif

#include "l3ins.h"
#include "as_isdn.h"
#include "helper.h"
#include "vpbx_globals.h"

#include "IsdnEndPoint.hxx"

#include "IeCause.hxx"
#include "MsgReleaseComplete.hxx"

#include "IsdnEvent.hxx"


using namespace Assist;

IsdnNtDevice::IsdnNtDevice(DeviceMgr *_parent, int fd, int _portNum, int _ptp )
	:IsdnCtrlDevice(_parent, fd, _portNum, _ptp, "NT-Device" ),
	myNtStack(0)
{
	cpLog(LOG_DEBUG, "ISDN TE Init %s\n", getName() );
	myUppestLayer =3;//  2;
	myUppestProtocol = ISDN_PID_L3_INS_NET;//ISDN_PID_L2_LAPD_NET;
	myDecoder = new MsgDecoderNT();
	myEncoder = new MsgEncoderNT();
	ntMode = ISDN_DEV_TYPE_NET;
	if( init() )
		assert(0);

}

IsdnNtDevice::~IsdnNtDevice()
{
	cpLog(LOG_DEBUG_STACK,  "Deconstructing NT device...\n");

	if (myNtStack) /* to see if initialized */
	{
		cpLog(LOG_DEBUG_STACK,  "the folowing messages are ok: one L3 process always exists (broadcast process) and some L2 instances (broadcast + current telephone's instances)\n");
		asnt_l3_cleanup(myNtStack);

		myNtStack->phd_downmsg_dinfo = 0;
		msg_queue_purge(&myNtStack->down_queue);
#if 0		
		msg_queue_purge(&myNtStack->rqueue);
		msg_queue_purge(&myNtStack->wqueue);
//		if (_nst->phd_down_msg)
//			as_msg_free(_nst->phd_down_msg);

		ret = sem_destroy(&myNtStack->work);
		if (ret)
		{
			cpLog(LOG_DEBUG_STACK, "cannot destroy semaphore ret(%d) %d %s\n", ret, errno, strerror(errno));
		}
		
		ret = pthread_mutex_destroy(&myNtStack->lock);
		if (ret)
		{
			cpLog(LOG_DEBUG_STACK, "cannot destroy mutex ret(%d) %s\n",ret, strerror(ret));
		}
#endif

		while(myNtStack->nrlist)
		{
			nr_list_t *nr = myNtStack->nrlist;

			REMOVE_FROM_LISTBASE(nr, myNtStack->nrlist);
			free(nr);
		}
		free(myNtStack);
		
	}

}

void IsdnNtDevice::myInit( stack_info_t *stack_info)
{
//	int i = 0;
 
	assert( (stack_info->pid.protocol[0] & ~ISDN_PID_FEATURE_MASK) == ISDN_PID_L0_NT_S0);
	assert( (stack_info->pid.protocol[1] & ~ISDN_PID_FEATURE_MASK) == ISDN_PID_L1_NT_S0);

		
	/* NT mode */
	if (stack_info->pid.protocol[1] != ISDN_PID_L1_NT_S0)
	{
		cpLog(LOG_ERR, "Given device %s: Missing layer 1 NT-mode protocol.\n", getName() );
	}
#if 0
	if (stack_info->pid.protocol[2] != 0 )
	{
		cpLog(LOG_ERR, "Given device %s: Layer 2 protocol 0x%08x is detected, but not allowed for NT lib.\n", getName(), stack_info->pid.protocol[2]);
		delUpperLayer(2);
	}
#endif
	if (stack_info->pid.protocol[3] != 0 )
	{
		cpLog(LOG_ERR, "Given device %s: Layer 3 protocol 0x%08x is detected, but not allowed for NT lib.\n", getName(), stack_info->pid.protocol[3]);
		delUpperLayer(3);
	}

	if ( myPtp!=0 )
	{
		cpLog(LOG_DEBUG_STACK,  "\tPort is point-to-point.\n");
	}
	
}

void IsdnNtDevice::initStack()
{
	int		ret;

	myNtStack = (NTSTACK *)malloc(sizeof(NTSTACK));
	assert (myNtStack != 0);
	memset(myNtStack, 0, sizeof(NTSTACK));

	myNtStack->device = myMainFd;
	myNtStack->cardnr = myPortNum;
	myNtStack->d_stid = myStackId;
	myNtStack->l2_id = myLowerId;
	myNtStack->l3_id = myUpperId;
	myNtStack->phd_downmsg_dinfo = 0;

//	myNtStack->hwSendFrame = asnt_write_frame;
//	myNtStack->hwSendMsg = asnt_write_msg;
	
	msg_queue_init(&myNtStack->down_queue);
#if 0
	msg_queue_init(&myNtStack->rqueue);
	msg_queue_init(&myNtStack->wqueue);

	pthread_mutex_init(&myNtStack->lock, NULL);
	ret = sem_init (&myNtStack->work, 0, 0);
	if (ret)
	{
		eprint("cannot init semaphore ret(%d) %d %s\n", ret, errno, strerror(errno));
		return ;//(ret);
	}
#endif

	myNtStack->priv = (void *)this;
	myNtStack->feature = FEATURE_NET_HOLD;
	if ( myPtp!= 0)
		myNtStack->feature |= FEATURE_NET_PTP;
	
	myNtStack->l3_2_mgr = asnt_upper_output;

	myNtStack->if_l3_2_l2 = asnt_write_msg;
	
	
	if(( ret =asnt_l3_init(myNtStack) )!= 0)
	{
		cpLog(LOG_ERR, "ISDN NT Stack Layer 3 startup failed");
		return ;
	}
}

int IsdnNtDevice::activate()
{
#if 0
	/* if ntmode, establish L1 to send the tei removal during start */
	{
		iframe_t act;
		/* L1 */
		act.prim = PH_ACTIVATE | REQUEST; 
#if 0		
		act.addr = (dev->upper_id & IF_ADDRMASK) | IF_DOWN;
#else
		act.addr = ( myUpperId & IF_ADDRMASK) | IF_DOWN;
#endif
		act.dinfo = 0;
		act.len = 0;
		ask_isdn_write(mainFd, &act, AS_ISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
	}

	if ( ptp  )
	{/* if ptp AND nt-mode, pull up the link */
		msg_t *dmsg;
		/* L2 */

		dmsg = createL2Msg(DL_ESTABLISH | REQUEST, 0, 0);
		if ( nst.mgr_2_l3(&nst, dmsg))
			as_msg_free(dmsg);
	}

	/* initially, we assume that the link is up (TE) and down (NT) */
	l2Link = (ptp)?0:1;
#endif

	return 0;
}

/* DL layer in D Channel of this port device */
int IsdnNtDevice::maintainDataLink( )
{
#if 0
	msg_t *dmsg;

	if (dev->l1timeout && now>dev->l1timeout)
	{
		cpLog(LOG_DEBUG_STACK,  "the L1 establish timer expired, we release all pending messages.\n", dev->portnum);
		dev->l1timeout = 0;
	}	
#endif

#if 0
	if (l2Establish)
	{
//		if (now-l2Establish > 5)
		{
			cpLog(LOG_DEBUG_STACK, "the L2 establish timer expired, we try to establish the link NT portnum=%d.\n", getPortNumber());
			dmsg = createL2Msg(myAddress,  DL_ESTABLISH | REQUEST, 0, 0);
			if (myNtMgr->nst->mgr_2_l3(myNtMgr->nst, dmsg))
				as_msg_free(dmsg);
		}
		return 1;
	}
#endif

	return 0;

}

/* return msg handle state : 0, msg not handled by this function; others, handled */
int IsdnNtDevice::myIsdnHardwareMain( msg_t *msg)
{
	int ret;
	iframe_t *frm = (iframe_t *)msg->data;

	cpLog(LOG_DEBUG_STACK,"%s: prim(%x) addr(%x)\n", __FUNCTION__,frm->prim, frm->addr);

#if 0	
	if (frm->prim == (MGR_TIMER | INDICATION))
	{
//		ask_isdn_write_frame(myNtMgr->nst->device, msg->data, frm->addr, MGR_TIMER | RESPONSE, 0, 0, NULL, TIMEOUT_1SEC);
		hwSendOutFrame(frm->addr, MGR_TIMER | RESPONSE, 0, 0, NULL);

		ret = asnt_if_handle_timer(myNtStack, frm->addr);
		return (AS_MSG_NEED_FREE);
	}
#endif

	if (frm->prim == (MGR_DELLAYER | CONFIRM))
	{
		cpLog(LOG_DEBUG_STACK,"%s: MGR_DELLAYER CONFIRM addr(%x)\n", __FUNCTION__,frm->addr);
		return (AS_MSG_NEED_FREE);
	}
//	if ((frm->addr & IF_ADDRMASK) == myAddress)
	if ((frm->addr & IF_ADDRMASK) == (unsigned int)myNtStack->l3_id)
	{
//		cpLog(LOG_DEBUG_STACK, "Found...Enter into Layer 3 of NT Stack");
		if (myNtStack->if_l2_2_l3)
		{/* msg is freed by NT stack */
//			cpLog(LOG_DEBUG_STACK, "Enter into Layer 3 of NT Stack");
			ret = myNtStack->if_l2_2_l3(myNtStack, msg);
			return (AS_MSG_HANDLED);
		}
		else
			cpLog(LOG_ERR, "No IF for Layer 1 of NT Stack");
	}
	else
	{
		cpLog(LOG_DEBUG_STACK, "%s: unhandled msg(%d) prim(%x) addr(%x) dinfo(%x)\n", __FUNCTION__, frm->len, frm->prim, frm->addr, frm->dinfo);
	}

	return (AS_MSG_NEED_FREE);
}

/* return msg handle state : 0, msg not handled by this function; others, handled 
* never free msg in this function
*/
int IsdnNtDevice::kernelTimerMsg( msg_t *msg)
{
//	int ret;
 	iframe_t *frm = (iframe_t *)msg->data;
	
	if ((frm->prim==(MGR_TIMER | INDICATION)) )
	{/* NT timer */
		cpLog(LOG_DEBUG_STACK, "timer-indication  in device %s\n",  getName());
#if 0
		itimer_t *it = myNtStack->tlist;

		/* find timer */
		while(it)
		{
			if (it->id == (int)frm->addr)
				break;
			it = it->next;
		}

		if (it)
		{
//			ask_isdn_write_frame(mainFd, msg->data, frm->addr, MGR_TIMER | RESPONSE, 0, 0, NULL, TIMEOUT_1SEC);
			hwSendOutFrame(frm->addr, MGR_TIMER | RESPONSE, 0, 0, NULL);
			test_and_clear_bit(FLG_TIMER_RUNING, (long unsigned int *)&it->Flags);
			it->function(it->data);
			return AS_MSG_HANDLED;
		}
#endif
	}
	return AS_MSG_CONTINUE;
}


/* create msg that send to driver */
msg_t *IsdnNtDevice::createL2Msg(int addr, int prim, int dinfo, int size) /* NT only */
{
	msg_t *msg = NULL;
	iframe_t *frm;

	/* allocate enough space for frame's header, q931_info_t and IEs */
	msg = as_msg_alloc(size+256+AS_ISDN_HEADER_LEN+DEFAULT_HEADROOM*2);
	if (msg!=0)
	{
		memset(msg_put( msg, AS_ISDN_HEADER_LEN), 0, AS_ISDN_HEADER_LEN);
		frm = (iframe_t *)msg->data;
		frm->prim = prim;
		frm->addr = addr;//myUpperId & IF_ADDRMASK | IF_DOWN;
		frm->dinfo = dinfo;
	}
	else
		assert(0);

	return(msg);
}

/* create msg that send to user space stack, mt is not used in user space NT stack */
msg_t *IsdnNtDevice::myCreateL3Msg(int prim, int mt, int size, Sptr <IsdnEndPoint> isdnEp)
{
	msg_t *msg;
	msg = prep_l3data_msg(prim, isdnEp->getFrameInfo(), sizeof(SETUP_t), 256, NULL);
	assert(msg != 0);
	return msg;
}

/* send out to user space NT stack */
/* send to User Space NT stack for NT device, L3 msg(CC_XX) are the content of msg_t  */
int IsdnNtDevice::enterStatckfromUpper(msg_t *msg)
{
	if( myNtStack->mgr_2_l3( myNtStack, msg ) != 0)
	{
		cpLog(LOG_ERR, "%s send to User Space NT Stack failed", getName() );
	}
	cpLog(LOG_DEBUG, "%s return from User Space NT Stack", getName() );
	return 0;
}

int IsdnNtDevice::sendOut(msg_t *msg, Sptr <EndPoint> ep)
{
#if 0
	PbxDeviceEvent *devEvent = new PbxDeviceEvent();
	devEvent->setEndPoint( ep );
	devEvent->setPrivate((void *)msg);

	myParent->reportEvent(devEvent);
#endif
	Sptr <IsdnDeviceMgr> mgr = NULL;
	mgr.dynamicCast( getParent() );
	assert(msg != 0);/* ep will be null when report raw msg from device monitor*/
	mgr->reportMsgEvent(msg, ep);
	
	return 0;
}

/* RX L3 call msg(CC_XXX) from User Space NT stack */
int IsdnNtDevice::myL3MsgHandler(msg_t *msg)
{
	ASNT_ISDNUSER_HEADER	*hh;
	IsdnMsg  *isdnMsg;
	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
//	iframe_t *frm = (iframe_t *)msg->data;
	cpLog(LOG_DEBUG_STACK, "%s: prim(%x) dinfo(%x) msg->len(%d) \n", __FUNCTION__, hh->prim, hh->dinfo, msg->len );

	switch(hh->prim) 
	{	
		case (DL_ESTABLISH | CONFIRM):
		case (DL_ESTABLISH | INDICATION):
		{
			cpLog(LOG_DEBUG_STACK,  "establish data link (DL) NT device=%s TEI=%d\n", getName(), hh->dinfo);
			if ( myPtp && hh->dinfo == 0)
			{
				if (l2Establish)
				{
					l2Establish = 0;
					cpLog(LOG_DEBUG_STACK, "the link became active before l2establish timer expiry.\n");
				}
				l2Link = 1;
				l1Link = 1; /* this is a hack, we also assume L1 to be active */
			}
			as_msg_free(msg);
			return AS_MSG_HANDLED;
			break;
		}	
		case (DL_RELEASE | CONFIRM):
		case (DL_RELEASE | INDICATION):
		{
			cpLog(LOG_DEBUG_STACK,  "release data link (DL) NT %s TEI=%d\n", getName(), hh->dinfo);
			l2Link = 0;
#if 0
			if ( myPtp && hh->dinfo == 0)
			{
				l2Link = 0;
				time(&l2Establish);
				cpLog(LOG_DEBUG_STACK, "because we are ptp, we set a l2establish timer.\n");
			}
			break;
#endif			
			as_msg_free( msg);
			return AS_MSG_HANDLED;
			break;
		}		
		case (CC_SETUP |CONFIRM):
		{/* feedback by NT L3 stack */
			if(msg->len > 0)
				dhexprint(DBGM_BC, "CC SIGNAL", msg->data, msg->len);
			ASNT_ISDNUSER_HEADER *hh = (ASNT_ISDNUSER_HEADER *)(msg->data);
			int *p=(int *)(msg->data);
			
//			if(replaceEpFrameInfo(hh->dinfo, ( (*(p+2))>>16)&0xFFFF ) !=AS_OK)
			if(replaceEpFrameInfo(hh->dinfo, *(p+2) ) !=AS_OK)
				cpLog(LOG_ERR, "repleace frameInfo in output NT device failed");
			cpLog(LOG_DEBUG_STACK,  "CC_SETUP_CONFIRM feedback in %s\n", getName());
			return 0;
		}
		case (CC_NEW_CR | INDICATION):
		{/* after CONNECT_ACK is send to L3,this msg is reply by L3 */
			cpLog(LOG_DEBUG_STACK,  "CC_NEW_CR is feedback  for CONNECT_ACK in %s, ignored\n", getName());
			return 0;
		}

		default:
		{
			if(msg->len > 0)
				dhexprint(DBGM_BC, "CC SIGNAL", msg->data, msg->len);

			isdnMsg = IsdnMsgFactory::instance()->decodeRawMsg(msg, this);
			assert(isdnMsg != 0);

			Sptr <EndPoint> ep = lookupEPByFrame(  isdnMsg->getFrameInfo() );
			if(ep==NULL && isdnMsg->getType() != ISDN_TYPE_SETUP)
			{
				cpLog(LOG_DEBUG, "No EP are available for this ISDN MSG :frameInfo of MSG is 0x%x. Ignore this msg" ,isdnMsg->getFrameInfo());
		//		assert(0);
				return AS_MSG_NEED_FREE;
			}
			
			if( ep==NULL && isdnMsg->getType()==ISDN_TYPE_SETUP)
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
			else
			{
				cpLog(LOG_DEBUG, "Frame for a exist call in EndPoint %s (MSG '%s')" , ep->getName(),isdnMsg->getName());

			}


			IsdnEvent *devEvent = new  IsdnEvent( );

			devEvent->setEndPoint(ep );
			devEvent->setIsdnMsg( isdnMsg);

			PbxAgent::instance()->getGateway()->reportEvent(devEvent);

			cpLog(LOG_DEBUG, "Frame for a exist call in EndPoint %s (MSG '%s')" , ep->getName(), isdnMsg->getName());
			return AS_MSG_NEED_FREE;
			break;
		}	
	}
	

#if 0
	if (hh->prim == (CC_SETUP | INDICATION))
	{/* CC_SETUP_IND : retrieve available B channel */
		SETUP_t				*setup;
		RELEASE_COMPLETE_t	*rc;
		unsigned char			cause[4];

		setup = (SETUP_t*)(msg->data + ASNT_ISDNUSER_HEAD_SIZE);
		pthread_mutex_lock(&mgr->bc[0].lock);
		if (mgr->bc[0].cstate == BC_CSTATE_NULL)
		{/* when available, send msg into bc->workq too, just as app2bc */
			mgr->bc[0].cstate = BC_CSTATE_ICALL;
			msg_queue_tail(&mgr->bc[0].workq, msg);
			pthread_mutex_unlock(&mgr->bc[0].lock);
			sem_post(&mgr->bc[0].work);
			return(0);
		}
		pthread_mutex_unlock(&mgr->bc[0].lock);
		
		pthread_mutex_lock(&mgr->bc[1].lock);
		if (mgr->bc[1].cstate == BC_CSTATE_NULL)
		{
			mgr->bc[1].cstate = BC_CSTATE_ICALL;
			msg_queue_tail(&mgr->bc[1].workq, msg);
			pthread_mutex_unlock(&mgr->bc[1].lock);
			sem_post(&mgr->bc[1].work);
			return(0);
		}
		pthread_mutex_unlock(&mgr->bc[1].lock);

		/* No channel available */
		cause[0] = 2;
		cause[1] = 0x80 | CAUSE_LOC_PNET_LOCUSER;
		if (setup->CHANNEL_ID)
			cause[2] = 0x80 | CAUSE_CHANNEL_UNACCEPT;
		else
			cause[2] = 0x80 | CAUSE_NO_CHANNEL;
		
		prep_l3data_msg(CC_RELEASE_COMPLETE | REQUEST, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 3, msg);
		rc = (RELEASE_COMPLETE_t *)(msg->data + ASNT_ISDNUSER_HEAD_SIZE);
		rc->CAUSE = msg_put(msg, 3);
		memcpy(rc->CAUSE, &cause, 3);
		
		if (_asnt_mgr_mgr2stack(mgr->nst, msg))
			as_msg_free(msg);
		
	}
	else if (hh->dinfo == mgr->bc[0].l3id)
	{
		msg_queue_tail(&mgr->bc[0].workq, msg);
		sem_post(&mgr->bc[0].work);
	}
	else if (hh->dinfo == mgr->bc[1].l3id)

	{
		msg_queue_tail(&mgr->ctrl->workq, msg);
		sem_post(&mgr->ctrl->work);
	}
//	else
	{
		wprint("%s: prim(%x) dinfo(%x) msg->len(%d) not handled\n", __FUNCTION__,	hh->prim, hh->dinfo, msg->len);
		return(-ESRCH);
	}
#endif
	return(0);
}


/* send to DeviceCommandThread, execute after NT user space stack process */
int IsdnNtDevice::hwSendOut(msg_t *msg)
{
	PbxDeviceEvent *devEvent = new PbxDeviceEvent();
	devEvent->setEndPoint( myEndPoints[0]);
	devEvent->setPrivate((void *)msg);

TRACE;
	PbxAgent::instance()->getGateway()->sendHwCommand(devEvent);
TRACE;	
	return 0;
}

/* send iframe_t to HW command Thread
 * address can be other address, such as timer_t->id
*/
int IsdnNtDevice::hwSendOutFrame(int addr, int prim, int  dinfo, int dlen, void *dbuf )
{
	msg_t *msg = NULL;
	iframe_t *frm;
	int		len = AS_ISDN_HEADER_LEN;
//	int		ret;

	cpLog(LOG_DEBUG_STACK, "%s: addr(0x%x)len(%d) pr(%x) di(%x)\n", __FUNCTION__, addr, dlen, prim, dinfo);
	if ((dlen > 0) && !dbuf) 
	{
		errno = EINVAL;
		return (-1);
	}

	msg = createL2Msg(addr, prim, dinfo, dlen );
	assert(msg!= 0);
	if (!msg)
	{
		errno = EINVAL;
		return(-1);
	}

	frm->len  = dlen;
	if (dlen>0) 
	{
		len += dlen;
		memcpy(&frm->data.i, dbuf, dlen);	/* overflow ??? */
	}

	return hwSendOut( msg);
}


int IsdnNtDevice::hwSendOutMsg(msg_t *msg)
{
	iframe_t	*frm;
	ASNT_ISDNUSER_HEADER	*hh;
//	NTSTACK *nst = myNtMgr->nst;

	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
	cpLog(LOG_DEBUG_STACK, "%s: msg(%p) len(%d) pr(%x) di(%x)\n", __FUNCTION__, msg, msg->len, hh->prim, hh->dinfo);
	
	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	frm = (iframe_t *)msg_push(msg, AS_ISDN_HEADER_LEN);
	frm->prim = hh->prim;
	frm->dinfo = hh->dinfo;
	frm->addr = (myUpperId & IF_ADDRMASK) | IF_DOWN;
	frm->len = msg->len - AS_ISDN_HEADER_LEN;

#if 0	
	if (frm->prim == PH_DATA_REQ)
	{
		frm->dinfo = (int)msg;
		if (myNtStack->phd_downmsg_dinfo != 0)
		{
			msg_queue_tail(&myNtStack->down_queue, msg);
			return(0);
		}
		myNtStack->phd_downmsg_dinfo = frm->dinfo;
	}
#endif	

#if 0	
	ask_isdn_write(nst->device, msg->data, msg->len, -1);
	as_msg_free(msg);
#endif

	TRACE;
	return hwSendOut( msg);
}

int IsdnNtDevice::timerEventExpired(Sptr <PbxTimerEvent> timerEvent)
{
	cpLog(LOG_DEBUG, "NT Timer Expired");
	assert(timerEvent != 0);

	/* NT timer */
	itimer_t *timer = (itimer_t *)timerEvent->getPrivate();
	assert( timer != 0 );

	if (timer)
	{
		cpLog(LOG_DEBUG_STACK, "timer-indication  in device %s it=%p\n",  getName(), timer );
/* timerEvent is return as Sptr, so it is destructed automatically
		test_and_clear_bit(FLG_TIMER_RUNING, (long unsigned int *)&it->Flags);
		it->function(it->data);
*/		
		asnt_timer_handle(timer);
		return AS_MSG_HANDLED;
	}
	return 0;
}

PbxTimerEvent *IsdnNtDevice::createNtTimer( )
{
	PbxTimerEvent *timerEvent = new PbxTimerEvent(getParent()->getQueue());
	timerEvent->setEndPoint( getNextEP(NULL) );

	return timerEvent;
}

