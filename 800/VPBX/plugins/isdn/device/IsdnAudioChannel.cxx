/*
* $Id: IsdnAudioChannel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxAgent.hxx"

#include "IeChanId.hxx"

#include <sys/stat.h>
#include <sys/ioctl.h>

#include "IsdnCtrlDevice.hxx"
/* kernel header */
#include "as_isdn_ctrl.h"

using namespace Assist;

IsdnAudioChannel::IsdnAudioChannel(IsdnCtrlDevice  *_parent, int _bChanId)//, int _stackId)
	:AudioChannel(_parent, "BChannel" )
{
	char devfilename[128];

	Sptr <IsdnCtrlDevice> isdnCtrl;
	isdnCtrl.dynamicCast(myParent);
	assert(isdnCtrl!= 0);
	
	assert(_bChanId== CHAN_ID_B1_CHANNEL ||_bChanId==CHAN_ID_B2_CHANNEL);
	myBChIndex = _bChanId;

	
	sprintf(devfilename, "/dev/astel/%d", B_RAWDEV+_bChanId-1+(isdnCtrl->getPortNumber()-1)*2);
	cpLog(LOG_DEBUG, "device file for raw B channel is %s(index is %d)\n", devfilename, myBChIndex );	

	myFd = open(devfilename, O_RDWR|O_NONBLOCK);
//	myFd = open(devfilename, O_RDWR);
	if( myFd<=0)
	{
		cpLog(LOG_ERR, "Raw Device file open failed :%s on Audio Channel %s" ,strerror(errno) , getName());
		assert( 0 );
	}

	myState = AUDIO_STATE_IDLE;
}


IsdnAudioChannel::~IsdnAudioChannel()
{
	cpLog(LOG_DEBUG, "IsdnAudioChannel Destorying.....");
}

/* activate/deactivate bchannel */
void IsdnAudioChannel::activate( )
{
	int x = 0;
	int res = 0;
	
	res = ioctl(myFd,  AS_ISDN_CTL_ACTIVATE, &x);

	if (res)
	{
		cpLog(LOG_DEBUG, "IsdnAudioChannel %s activated failed\n" , getName() );
#if 0
		switch(errno)
		{
			case EBUSY:
			case EINTR:
				/* Wait just in case */
				cpLog(LOG_DEBUG, "ring the phone BUSY\n" );
				usleep(10000);
			break;
			case EINPROGRESS:
				cpLog(LOG_DEBUG,  "ring the phone IN PROGRESS\n" );
				res = 0;
				break;
			default:
				cpLog(LOG_DEBUG, "Couldn't ring the phone: %s\n", strerror(errno));
				break;
		}
#endif
		return;
	}
	cpLog(LOG_DEBUG, "IsdnAudioChannel %s activated\n" , getName() );
	AudioChannel::activate();

}

void IsdnAudioChannel::deactivate()
{
	int x = 0;
	int res = 0;
	AudioChannel::deactivate();
	res = ioctl(myFd,  AS_ISDN_CTL_DEACTIVATE, &x);

	if (res)
	{
		cpDebug(LOG_DEBUG, "IsdnAudioChannel %s deactivated failed\n" , getName() );
	}
	else
		cpDebug(LOG_DEBUG, "IsdnAudioChannel %s deactivated\n" , getName() );
}

#if WITH_DEBUG
const char *IsdnAudioChannel::debugInfo()
{
	char *buf, *tmp;
	int length = 0;

	buf = (char *)malloc(4096);
	memset(buf, 0, sizeof(buf) );
	assert(buf);

	tmp = (char *)AudioChannel::debugInfo();
	length += sprintf(buf + length , "%s" , tmp );
	free(tmp);
	
	length += sprintf(buf + length , "\r\n\t\tChan ID \t : %d" , myBChIndex );

	return buf;
}
#endif

int IsdnAudioChannel::getChanId()
{
	return myBChIndex;
}

int IsdnAudioChannel::ReadVData(char * buffer)
{
	assert(buffer);

	int ret = read(myFd, buffer, AUDIO_BUFFER_SIZE);
	cpDebug(LOG_DEBUG, "%s read %d bytes",getName(), ret);

	return ret;
}

int IsdnAudioChannel::WriteVData(char * buffer, int length)
{
	if(length <= 0)
		return 0;

	int ret =  write(myFd, buffer, length );
	cpDebug(LOG_DEBUG, "%s write %d bytes(buffer length is %d)", getName(), ret, length  );

	return ret;
}

void IsdnAudioChannel::setEndPoint(Sptr <EndPoint> ep)
{
	if(ep==NULL)
		cpDebug(LOG_DEBUG, "Clear EndPoint in IsdnAudioChannel");
	myEndPoint = ep;
}

