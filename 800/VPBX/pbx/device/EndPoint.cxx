/*
* $Id: EndPoint.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "EndPoint.hxx"
#include "EpStateMachine.hxx"
#include "CommonMsg.hxx"
#include "CallInfo.hxx"
#include "PbxManager.hxx"
#include "DigitEvent.hxx"
#include "PbxAgent.hxx"
#include "DeviceMgr.hxx"
#include "CtrlDevice.hxx"

using namespace Assist;


EndPoint::EndPoint( CtrlDevice *_ctrlDev)
	:PbxStateObject(_ctrlDev->getParent() ),
	myIndex(-1),
	mySelf(0),
	myBuildProcess(false),
	isExt(false)
{
	myCtrlDevice = _ctrlDev;
	myAudio = 0;
	memset( myPhoneNumber, 0, PHONE_NUMBER_LENGTH);
	memset( myName, 0, VPBX_NAME_LENGTH);

	dialing = "";
	mySelf = this;
}

EndPoint::~EndPoint(void)
{
	cpLog(LOG_DEBUG, "Deconstructing EndPoint %s.....", getName() );
	
}

void EndPoint::setPhoneNumber(const char *_number)
{
	memset(myPhoneNumber, 0, PHONE_NUMBER_LENGTH);
	sprintf( myPhoneNumber, "%s", _number);
}

void EndPoint::setCallId(Sptr <CallId> _callId)
{
	if(_callId==0)
		cpLog(LOG_DEBUG, "clear CallId in EndPoint %s", getName() );
	if (myBuildProcess)
		myBackupCallId = _callId;
	else
		myCallId = _callId;
}

Sptr <CallId>	EndPoint::getCallId()
{

#if 0
	if (mySecondCallId != 0) 
	{
		return mySecondCallId;
	}
#endif 


	if (myBuildProcess)
	{
		//backupCallId();
		return myBackupCallId;
	}

	return myCallId;
}

#if WITH_DEBUG
const char *EndPoint::debugInfo()
{
	char *tmp;
	int length = 0;
	
	tmp = (char *)malloc(4096);
	assert( tmp != 0);
	memset(tmp, 0, sizeof(tmp) );
	
	length += sprintf(tmp+length, "\r\n\tEndPoint Index \t : %d", getIndex() );
	length += sprintf(tmp+length, "\r\n\t\tStatus \t\t : %s", getState()->name() );
	length += sprintf(tmp+length, "\r\n\t\tCtrl Device \t : %s", ((getCtrlDevice())!=0)? getCtrlDevice()->getName():"Error, No CtrlDevice available" );
	length += sprintf(tmp+length, "\r\n\t\tAudio Channel \t : %s", (getAudio()!=0)? getAudio()->getName():"No AudioChannel available" );
	length += sprintf(tmp+length, "\r\n\t\tPhone Number \t : %s", getPhoneNumber() );
//	length += sprintf(tmp+length, "\r\n\\t\tCallId %d ", getCallId() );
	return tmp;
}
#endif

void EndPoint::setIndex(int _index)
{
	myIndex = _index;
	sprintf( myName, "%s-%d" , myName, _index);
}

int EndPoint::getIndex()
{
	return myIndex;
}

const char *EndPoint::getName(void)
{
	return myName;
}

#if 0
void EndPoint::setCallInfo(Sptr <CallInfo> call)
{
	mycall = call;
}
Sptr <CallInfo> EndPoint::getCallInfo()
{
	return mycall;
}

void EndPoint::backupCall(Sptr<CallInfo> newcall)
{
	myBackupCall = newcall;
}
Sptr<CallInfo> EndPoint::getBackupCall() const 
{
	return myBackupCall;
}
#endif

#if 0
/* has defined in PbxStateObject */
Sptr <PbxTimerEvent> EndPoint::getTimer()
{
	return myTimer;
}
void EndPoint::setTimer(Sptr <PbxTimerEvent> timer)
{
	myTimer = timer;
}
#endif


void EndPoint::genDtmfEvent(char dtmf)
{
	assert(mySelf != NULL);
	cpLog(LOG_DEBUG, "Generate DTMF event with %c", dtmf);
	Sptr <DigitEvent> digit = new DigitEvent();
	assert(digit != NULL);

	/* check the dtmf char is validate */
	digit->setEndPoint(mySelf); 
	digit->myDigit = dtmf;
	PbxAgent::instance()->getGateway()->reportEvent(digit);

}

void EndPoint::setAudio(Sptr <AudioChannel> _audio)
{
	//cpLog( LOG_DEBUG, "AudioChannel can not be set in PSTN EndPoint" );
#if 0
	audioMutex.lock()
	audioMutex.unlock();
#endif 

	if (_audio != 0)
	{
		myAudio = _audio;
	}
}

Sptr <AudioChannel> EndPoint::getAudio()
{
	return myAudio;
#if 0	
	if (!myBuildProcess)
	{
//		cpLog(LOG_DEBUG, "---Return ordinary myAudio");
		return myAudio;
	}
	
	assert(myAudio != 0);
	cpLog(LOG_DEBUG, "---Return myAdiou's Peer: %s", myAudio->getAudioPeer()->getName());
//	return myAudio->getAudioPeer();
	return myAudio;
#endif
}

int EndPoint::callToConsole()
{
	return PbxAgent::instance()->getGateway()->callToConsole( mySelf);
}

void EndPoint::reset()
{
	resetDialing();
	setCallId( 0);

	Sptr <AudioChannel> audio = getAudio();
	if(audio != 0)
	{/* this is reply for DISCONNECT */
		audio->deactivate();
		audio->setEndPoint( 0);
		setAudio(0);
	}

	/* audiochannel deactivate can not be used here, it must be synchronized with audio thread in RELEASE_COMPLETE msg */	
}

