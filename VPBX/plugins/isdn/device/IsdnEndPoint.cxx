/*
* $Id: IsdnEndPoint.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnEndPoint.hxx"
#include "CtrlDevice.hxx"
#include "IsdnStateBuilder.hxx"

using namespace Assist;

IsdnEndPoint::IsdnEndPoint(  CtrlDevice *_ctrlDev , int isNt)
	:EndPoint( _ctrlDev )
{
	cpLog(LOG_DEBUG, "Constructing.....");
	setState( findState(ISDN_EP_STATE_IDLE) );
	sprintf(myName, "ISDN-EP" );
	myFrameInfo = FRAME_INFO_NOT_EXIST;
	if(isNt != 0 )
		isExt = true;
}

IsdnEndPoint::~IsdnEndPoint(void)
{
	cpLog(LOG_DEBUG, "Destorying.....");
}


void IsdnEndPoint::setCallIdByFrame( Sptr <CallId> callId, int _frameInfo )
{
	setCallId(callId );
	myFrameInfo = _frameInfo;
}

Sptr <CallId> IsdnEndPoint::getCallIdByFrame(int _frameInfo)
{
	if(this->myFrameInfo == _frameInfo )
		return getCallId();
	else
		return NULL;
}


void IsdnEndPoint::setAudio(Sptr <AudioChannel> _audio)
{
	cpLog( LOG_DEBUG, "set Audio" );

	if(_audio==0)
		cpLog(LOG_DEBUG, "clear Audio in EndPoint %s", getName() );
	myAudio = _audio;

	if( myAudio == 0 )
		cpLog(LOG_DEBUG, "Audio is cleared");
}

void IsdnEndPoint::setFrameInfo(int _frameInfo)
{
	myFrameInfo = _frameInfo;
}

int IsdnEndPoint::getFrameInfo()
{
	return myFrameInfo;
}

#if 0
void IsdnEndPoint::setCallRef(int _callRef )
{
	myCallRef = _callRef;
}

int IsdnEndPoint::getCallRef()
{
	return myCallRef;
}
#endif

#if WITH_DEBUG
const char *IsdnEndPoint::debugInfo()
{
	char *buf, *tmp;
	int length = 0;
	
	buf = (char *)malloc( 4096 );
	assert( buf != 0);
	memset(buf, 0 , sizeof(buf) );
	
	tmp = (char *)EndPoint::debugInfo();
	length += sprintf(buf+length, "%s", tmp );
	free(tmp);
	tmp = NULL;

	length += sprintf(buf+length, "\r\n\t\tFrameInfo \t : %x", getFrameInfo() );
//	length += sprintf(buf+length, "\r\n\t\tCallReference \t : %x", getCallRef() );
	return buf;
}
#endif

void IsdnEndPoint::reset()
{
	EndPoint::reset();

	Sptr <AudioChannel> audio = getAudio();
	if(audio != 0)
	{
		audio->deactivate();
		audio->setEndPoint( 0);
		setAudio(0);
	}
	
//	setFrameInfo( FRAME_INFO_NOT_EXIST);
}

