/*
* $Id: IsdnCtrlDevice.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Sptr.hxx"
#include "cpLog.h"

#include "PbxConfiguration.hxx"
#include "DeviceMgr.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnAudioChannel.hxx"

#include "asnt_isdn.h"
#include "as_isdn.h"
#include "vpbx_globals.h"

#include "asnt_nins.h"
#include "asnt_isdn.h"

#if __ARM_IXP__
#include <arm/bitops.h>
#else
#include <asm/bitops.h>
#endif

#include <unistd.h>
#include <assert.h>

#include "IsdnMsg.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"
#include "IeChanId.hxx"

#include "IsdnCtrlDevice.hxx"

#include "l3ins.h"

using namespace Assist;

IsdnCtrlDevice::IsdnCtrlDevice(DeviceMgr  *_parent , int _fd,  int _portnum, int _ptp, char *_name )
	:CtrlDevice(_parent, _portnum-1,  _name),
	myPtp(_ptp),
	myPortNum(_portnum)
//	,myCallRef(0)
{
	myMainFd = _fd;
}

IsdnCtrlDevice::~IsdnCtrlDevice()
{
	unsigned char buf[32];
	
	cpLog(LOG_DEBUG, "Destorying.....");
	/* free stacks : NT */
	cpLog(LOG_DEBUG_STACK,  "freeing d-stack.\n");
	if (myStackId)
	{
		ask_isdn_clear_stack( myMainFd, myStackId);
#if 0
		if (myLowerId)
			ask_isdn_write_frame(devMgr->getFd(), buf, myLowerId, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
#else
		/* delete new layer just created in user space, 2005.08.27 */
		if ( myUpperId)
			ask_isdn_write_frame( myMainFd , buf, myUpperId, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
#endif
	}

}

int IsdnCtrlDevice::init( )
{
	int ret;
	int i;
	unsigned char buff[1025];
	//Sptr <IsdnCtrlDevice> self = this;
	
	iframe_t *frm = (iframe_t *)buff;
	stack_info_t *stack_info;

	cpLog(LOG_DEBUG,  "***********Info of port %d******* \n", myPortNum);
	/* allocate ressources of port */
//	msg_queue_init(&downqueue);
	memset(buff, 0, sizeof(buff) );
	ret = ask_isdn_get_stack_info(myMainFd , myPortNum, buff, sizeof(buff));
	assert(ret >= 0);
	
	cpLog(LOG_DEBUG,  "***********Info of port %d******* \n", myPortNum);
	stack_info = (stack_info_t *)&frm->data.p;

	cpLog(LOG_DEBUG_STACK,  "stack ID of device %s is %x\n", getName(), stack_info->id);

	cpLog(LOG_DEBUG,  "***********Info of port %d******* \n", myPortNum);
	myStackId = stack_info->id;

	myInit(stack_info);
	
	cpLog(LOG_DEBUG,  "***********Info of port %d******* \n", myPortNum);
	assert( !newUpperLayer( )  );

	for(i=0; i< B_CHAN_PERPORT/*stack_info->childcnt*/; i++)
	{
		cpLog(LOG_DEBUG_STACK,  "bStackId[%d] = 0x%x.\n", i, stack_info->child[i] );

		/* B Channel ID must be CHAN_ID_B1_CHANNEL or CHAN_ID_B2_CHANNEL */
		myAudioChans[i] = new IsdnAudioChannel(this, i+1 );//, stack_info->child[i] );

		myEndPoints[i] = new IsdnEndPoint( this,(ntMode==ISDN_DEV_TYPE_NET)? 1: 0);
//		parent->addAudioChannel(audioChans[i]);
	}

	initStack();

	cpLog(LOG_DEBUG, "IsdnCtrlDevice::init( ) Over!");
	return 0;
}


int IsdnCtrlDevice::delUpperLayer(int upperLayer)
{
	int ret;
	int layerId;
	char layerName[64];

	cpLog(LOG_INFO,"Clear data of layer %d",upperLayer);
	
	layerId = ask_isdn_get_layerid(myMainFd, myStackId, upperLayer); // id of uppermost layer (nt=1, te=3)
	if (layerId  <= 0)
	{
		cpLog(LOG_ERR, "Cannot get layer(%d) id of port %s\n", upperLayer, getName() );
		return(-EINVAL);
	}
	sprintf(layerName, "%s-%d", getName(), upperLayer);
	cpLog(LOG_INFO,"Got layer id. layerName %s layerid %x",layerName,layerId);

	ret=ask_isdn_del_layer(myMainFd, layerId);	
//	ret=ask_isdn_write_frame( myMainFd , buf,layerId, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if(ret)
		cpLog(LOG_ERR,"failed in clear");
	else
		cpLog(LOG_INFO,"cleared");

	return ret;
}

int IsdnCtrlDevice::newUpperLayer()
{
	layer_info_t li;
	interface_info_t ii;
	int ret;
	char layerName[64];

	myLowerId = ask_isdn_get_layerid(myMainFd, myStackId, myUppestLayer-1); // id of uppermost layer (nt=1, te=3)
	if (myLowerId <= 0)
	{
		cpLog(LOG_ERR, "Cannot get layer(%d) id of port %s\n", myUppestLayer-1, getName() );
		return(-EINVAL);
	}

	sprintf(layerName, "%s-%d", getName(), myUppestLayer);
	memset(&li, 0, sizeof(li));
	UCPY(&li.name[0], layerName );
	li.object_id = -1;
	li.extentions = 0;
/*	
	li.pid.protocol[myUppestLayer-1] = myUppestProtocol;
*/	
	li.pid.protocol[myUppestLayer ] = myUppestProtocol;
	li.pid.layermask = ISDN_LAYER(( myUppestLayer ));
	li.st = myStackId;
	
	myUpperId = ask_isdn_new_layer(myMainFd , &li);
	if ( myUpperId <= 0)
	{
		cpLog(LOG_ERR, "Cannot add layer %d of port %s\n", myUppestLayer, getName() );
		return(-EINVAL);
	}
	cpLog(LOG_DEBUG_STACK,  "Layer %s created in kernel device. Layer ID: %x Lower ID: %x\n", li.name, myUpperId, myLowerId);

	myAddress = (myUpperId& IF_ADDRMASK) | IF_DOWN;
	
	memset(&ii, 0, sizeof(ii));
	ii.extentions = EXT_IF_EXCLUSIV;
	ii.owner = myUpperId;
	ii.peer = myLowerId;
	ii.stat = IF_DOWN;
	ret = ask_isdn_connect(myMainFd, &ii);
	if (ret)
	{
		cpLog(LOG_ERR, "Cannot connect layer %d of port %s exclusively.\n", myUppestLayer-1, getName() );
		return(ret);
	}
	cpLog(LOG_DEBUG_STACK,  "Layer %d of port %s added.\n", myUppestLayer, getName() );

	return 0;
}

/* called by DeviceManager */
int IsdnCtrlDevice::hardwareMain(void *params)
{
	msg_t *msg = (msg_t *)params;
	int ret;
	
//	cpLog(LOG_DEBUG, "kernel command");
	if( (ret= kernelTimerMsg( msg) ) != AS_MSG_CONTINUE)
		return ret;

	if( (ret=myIsdnHardwareMain( msg) ) != AS_MSG_CONTINUE )
		return ret;

	return AS_MSG_CONTINUE;
}

/* send HW command to device, called by DeviceCommandThread */
void IsdnCtrlDevice::sendHwCommand(Sptr <PbxEvent> hwCommand)
{
	msg_t *msg;
//	myActionMutex.lock();
	cpLog(LOG_DEBUG, "%s", getName());

	maintainDataLink();

TRACE;
	if(hwCommand->getPrivate()==0)
	{
		cpLog(LOG_ERR, "No params in HW command for ISDN device");
		return ;
	}

TRACE;

	msg = (msg_t *)hwCommand->getPrivate() ;
	HandleDownMsg(msg );
//	myActionMutex.unlock();

	return;
}

#if 0
/* return <0, no handle need to continue, return; 0, 1, B Channel number */
int IsdnCtrlDevice::kernelBChMsg(msg_t *msg)
{
	Sptr< AudioChannel> audioCh = NULL;
 	iframe_t *frm = (iframe_t *)msg->data;

	audioCh = lookupAudioCh( frm);
	assert(audioCh!=0);

	Sptr <IsdnAudioChannel> bChan = NULL;
	bChan.dynamicCast(audioCh);
	assert(bChan!=0);
	
	return bChan->kernelCommand(frm);
}
#endif

//msg_t *IsdnCtrlDevice::createL3Msg(int prim, int mt, int dinfo, int size)
msg_t *IsdnCtrlDevice::createL3Msg(int prim, int mt,  int size,  Sptr <IsdnEndPoint> isdnEp)
{
//	int i = 0;
	msg_t *dmsg = NULL;

//	while(i < 10)
	{
		dmsg = myCreateL3Msg( prim, mt, size, isdnEp);
		if(dmsg==0)
		{
			cpLog(LOG_DEBUG, "Memory Allocation Failed\n");
			exit(1);
//			break;
		}

//		if (!i)
//			cpLog(LOG_ERR, "cannot allocate memory, trying again...\n");
//		i++;
//		usleep(300000);
	}

	assert(dmsg);
//	showSMessage( prim, 1);
	
	return dmsg;
}

#if 0
void IsdnCtrlDevice::addIsdnMsgQueue( msg_t *msg)
{
	msg_queue_tail(&downqueue,msg);
}
#endif

#if WITH_DEBUG
const char *IsdnCtrlDevice::debugInfo()
{
	char *buf, *tmp;
	int length = 0;

	buf = (char *)malloc(4096);
	assert(buf != 0);
	memset(buf, 0 ,sizeof(buf) );

	length += sprintf(buf+length, "\r\n\tCtrlDevice %s(index : %d) Info :\n", getName(), getIndex() );
	length += sprintf(buf+length, "\t\tStack ID \t : %x \n", getStackId());
	length += sprintf(buf+length, "\t\tAddress \t : %x(Upper ID : %x) \n", getAddress(), myUpperId );
	length += sprintf(buf+length, "\t\tMode \t\t : %s ", (getNtMode()!=0)?"NT":"TE") ;

	tmp = (char *)CtrlDevice::debugInfo();
	length += sprintf(buf+length, "%s", tmp);
	free(tmp);

	return buf;
}
#endif


Sptr <EndPoint> IsdnCtrlDevice::getNextEP( Sptr <EndPoint> _ep)
{
	if(_ep == 0)
	{	
		return myEndPoints[0];
	}
	else if(_ep->getIndex() == myEndPoints[0]->getIndex() )
	{	
		return myEndPoints[1];
	}
	else
	{	
		return 0;
	}
}

Sptr <AudioChannel> IsdnCtrlDevice::getNextAudio( Sptr <AudioChannel> _audio)
{
	if(_audio == 0)
		return myAudioChans[0];
	else if(_audio->getIndex() == myAudioChans[0]->getIndex() )
		return myAudioChans[1];
	else
		return 0;
}


Sptr < AudioChannel> IsdnCtrlDevice::getAudioChByChanId(int chanId)
{
	Sptr <AudioChannel>  audioCh = NULL;
	Sptr <IsdnAudioChannel> bCh;

	if(chanId !=CHAN_ID_B1_CHANNEL && chanId != CHAN_ID_B2_CHANNEL && chanId != CHAN_ID_ANY_CHANNEL)
	{
		cpLog(LOG_ERR, "IsdnAudioChannel ID %d is not validate");
		assert(0);
		return 0;
	}

	while( (audioCh = getNextAudio(audioCh) )!=0 )
	{
		bCh.dynamicCast( audioCh );
//		if(chanId== CHAN_ID_ANY_CHANNEL)
//			return bCh;
//		else 
		if ( bCh->getChanId() == chanId )
		{
			return bCh;
		}
	}

	return NULL;
}

Sptr < AudioChannel> IsdnCtrlDevice::allocateBChan(int chanId)
{
	Sptr <AudioChannel>  audioCh = getAudioChByChanId(chanId);

	if(audioCh->getState() != AUDIO_STATE_IDLE)
	{
		cpLog(LOG_ERR, "IsdnAudioChannel %d is busy now", chanId);
		return 0;
	}
	
	audioCh->setState(AUDIO_STATE_ACTIVATING);
	return audioCh;
}

Sptr < AudioChannel> IsdnCtrlDevice::allocateAnyBChan( )
{
	Sptr <AudioChannel>  audioCh = NULL;

	while( (audioCh = getNextAudio(audioCh) )!=0 )
	{
		if ( audioCh->getState() == AUDIO_STATE_IDLE )
		{
			audioCh->setState( AUDIO_STATE_ACTIVATING);
			return audioCh;
		}
	}

	return NULL;
}

#if 0
Sptr < AudioChannel> IsdnCtrlDevice::lookupAudioCh(msg_t *msg)
{
	iframe_t *frm = (iframe_t *)msg->data;
	return lookupAudioCh( frm);
}

Sptr < AudioChannel> IsdnCtrlDevice::lookupAudioCh(iframe_t *frm)
{
	return lookupAudioCh( frm->addr);
}

Sptr < AudioChannel> IsdnCtrlDevice::lookupAudioCh(int _address)
{
	Sptr <AudioChannel>  audioCh = NULL;
	Sptr <IsdnAudioChannel> bCh;

	while( (audioCh = getNextAudio(audioCh) ) != 0 )
	{
		bCh.dynamicCast( audioCh );
		if ( bCh->getAddress() == _address )
		{
			return bCh;
		}
	}

	return NULL;
}
#endif

Sptr <EndPoint> IsdnCtrlDevice::lookupFreeEP()
{
	Sptr <EndPoint> ep = NULL;
	while( (ep=getNextEP(ep))!= 0)
	{
//		if(ep->getState()->getStateType()==ISDN_EP_STATE_IDLE )
		Sptr <IsdnEndPoint> isdnEp = NULL;
		isdnEp.dynamicCast(ep);
		assert( isdnEp != 0);
		if( isdnEp->getFrameInfo() == FRAME_INFO_NOT_EXIST )
		{	
			return ep;
		}
	}
	
	cpLog(LOG_DEBUG, "LookupFreeEP");
	return 0;
}

Sptr <EndPoint> IsdnCtrlDevice::lookupEPByFrame(int _frameInfo)
{
	Sptr <EndPoint> ep = NULL;
	while( (ep=getNextEP(ep))!= 0)
	{
		Sptr <IsdnEndPoint> isdnEp = NULL;
		isdnEp.dynamicCast(ep);
		assert(isdnEp != 0);
#if WITH_DEBUG
		cpLog(LOG_DEBUG, "FrameInfo of EndPoint is %x, lookup frame is %x", isdnEp->getFrameInfo(), _frameInfo);
#endif
		if(isdnEp->getFrameInfo()==_frameInfo)
			return isdnEp;
	}

#if 0	
	/* get a free EP or NULL EP */
	return  lookupFreeEP();
#endif
	return 0;
}


/* this is a default implement use for End device with phone number, such as FXS 
* for FXO, TE and NT, it must be overload 
*/
Sptr <EndPoint> IsdnCtrlDevice::lookupEPByPhoneNumber(const char *_number)
{
	int position = PbxConfiguration::instance()->getAccessLength();
	TRACE;
	if(strncmp(myPhoneNumber, _number + position, strlen(myPhoneNumber) ) == 0)
	{
		TRACE;
		if(myEndPoints[0]->getState()->getStateType() != 0)/* busy */
			return myEndPoints[1];
		return myEndPoints[0];
	}

	return NULL;
}


#if 0
int IsdnCtrlDevice::getCallRef()
{
	myCallRef++;
	return myCallRef;
}
#endif

Sptr <MsgDecoder> IsdnCtrlDevice::getDecoder() const
{
	return myDecoder;
}

Sptr <MsgEncoder> IsdnCtrlDevice::getEncoder() const
{
	return myEncoder;
}

int IsdnCtrlDevice::getNtMode()
{
	return ntMode;
}


int IsdnCtrlDevice::HandleDownMsg(msg_t *msg)
{
#if 0
	msg_t *dmsg;
	if ((dmsg = msg_dequeue(&downqueue)))
	{
		iframe_t *frm;
		frm = (iframe_t *)dmsg->data;
		frm->addr = myUpperId & IF_ADDRMASK | IF_DOWN;
		frm->len = (dmsg->len) - AS_ISDN_HEADER_LEN;
		cpLog(LOG_DEBUG_STACK,  "sending queued TE l3-down-message: prim(0x%x) dinfo(0x%x) msg->len(%d)\n", frm->prim, frm->dinfo, dmsg->len);
		ask_isdn_write( getFd(), dmsg->data, dmsg->len, TIMEOUT_1SEC);
		as_msg_free(dmsg);
		return(1);	/*not continue */
	}
#endif

	assert(msg != 0);

	iframe_t *frm;
	frm = (iframe_t *)msg->data;
	frm->len = (msg->len) - AS_ISDN_HEADER_LEN;
	cpLog(LOG_DEBUG, "frame Info : prim(0x%x) addr(0x%x) dinfo(0x%x) length(%d)" , 
			frm->prim, frm->addr, frm->dinfo, frm->len);
	ask_hex_dump(msg->data, msg->len);

	
#if WITH_DEBUG
//	IsdnMsgFactory::instance()->debugQ931Info((iframe_t *) msg->data);
#endif

#if 0
	cpLog(LOG_DEBUG, "myFd in ISDN-TE %d msg length =%d", getFd() ,msg->len );
	int fd = open("a.dat", O_RDWR | O_NONBLOCK);
	if(write(fd, msg->data, msg->len)<=0)
		cpLog(LOG_ERR, "send ISDN L3 msg to hardware failed : %s",  strerror(errno) );
	TRACE;
	TRACE;
	close(fd);
#endif	
	ask_isdn_write_without_lock( getFd(), msg->data, msg->len, TIMEOUT_1SEC);

//	as_msg_free( msg);

	TRACE;
	return 0;
}

int IsdnCtrlDevice::replaceEpFrameInfo(int oldFrameInfo, int newFrameInfo)
{
	cpLog(LOG_DEBUG_STACK, "replace FrameInfo : new : 0x%x , old : 0x%x" , newFrameInfo, oldFrameInfo);
	Sptr <IsdnEndPoint> isdnEp = NULL;
	isdnEp.dynamicCast( lookupEPByFrame(oldFrameInfo) );
	
	if(isdnEp==0)
	{
		return AS_FAIL;
	}
	isdnEp->setFrameInfo( newFrameInfo);
	cpLog(LOG_DEBUG_STACK, "replace FrameInfo in ISDN EndPoint %s" , isdnEp->getName() );
	return AS_OK;
}

int IsdnCtrlDevice::timerEventExpired(Sptr <PbxTimerEvent> timerEvent)
{
	return 0;
}

