/*
* $Id: ConsoleAudioChannel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "vpbx_globals.h"

#include "TaskEvent.hxx"
#include "PbxConfiguration.hxx"

#include "ConsoleAudioChannel.hxx"
#include "ConsoleEndPoint.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


using namespace Assist;

extern void *console_decode_task(void *param);

ConsoleAudioChannel::ConsoleAudioChannel(CtrlDevice  *_parent , console_dev_t  isMaster)
	:AudioChannel(_parent, "ConsoleAudio")
{
	myDevType = isMaster;
	myState = AUDIO_STATE_IDLE;

	isDecoding = false;

	const char *greetingFile = PbxConfiguration::instance()->getGreetingFileName().c_str();
	struct stat greetingState;
	int greetingFd;
	int res= 0;
	
	assert( stat(greetingFile, &greetingState) == 0 );
	myGreetingSize = greetingState.st_size;
	if(myGreetingSize <= 0)
	{
		cpLog(LOG_ERR, "Greeting File %s is too small" , greetingFile);
		assert(0);
	}

	myGreetingSize = myGreetingSize/AS_DRIVER_BUFFER_LENGTH*AS_DRIVER_BUFFER_LENGTH;
	myGreetingData = (char *)malloc(myGreetingSize);
	assert(myGreetingData != 0);

	greetingFd = open(greetingFile, O_RDONLY);
	if( greetingFd <=0 )
	{
		cpLog(LOG_ERR, "Raw Device file open failed :%s on Audio Channel %s" ,strerror(errno) , getName());
		assert( 0 );
	}

	res = read(greetingFd, myGreetingData, myGreetingSize);
	if(res != myGreetingSize)
	{
		cpLog(LOG_ERR, "Greeting File Read Error : read %d bytes, wish %d bytes", res, myGreetingSize);
		assert(0);
	}
	close(greetingFd);
	myGreetingSize /= AS_DRIVER_BUFFER_LENGTH;
	myGreetingIndex = 0;


	dtmfBufSize = AS_DTMF_DECOCDER_BUFFER_LENGTH;	/* 40 ms*/
	dtmfBufCount = 3000;								/* 1000*40 = 40 seconds */
	
	dtmfBufOutIndex = -1;
	dtmfBufInIndex = -1;
	dtmfBufInPos = 0;
	dtmfBufs = NULL;

	if(isMaster == CONSOLE_MASTER)
	{
		myDsp = as_dsp_init(PbxConfiguration::instance()->getDspConfig() );
//		mySlave = new ConsoleAudioChannel(_parent, CONSOLE_SLAVE);
	}

}

ConsoleAudioChannel::~ConsoleAudioChannel()
{
	as_dsp_destroy(myDsp);
	if(dtmfBufs != 0)
		free(dtmfBufs);
	free(myGreetingData);
	cpLog(LOG_DEBUG, "ConsoleAudioChannel Destorying.....");
}

void ConsoleAudioChannel::activate( )
{
	AudioChannel::activate();
	gettimeofday (&prevTime, 0);

	myCAudioState = console_audio_greeting;

	isDecoding = true;
	dtmfBufOutIndex = -1;
	dtmfBufInIndex = -1;
	dtmfBufInPos = 0;

	if(dtmfBufs == NULL)
		dtmfBufs = (unsigned char *)malloc( sizeof(unsigned char)*dtmfBufCount*dtmfBufSize);
	
	assert(dtmfBufs != 0);

	if(myDevType == CONSOLE_MASTER && getEndPoint()->getPeerSignalType() == dtmf_signal_in_band )
	{
		new TaskEvent(console_decode_task, (void *)this);
	}
	cpLog(LOG_DEBUG, "ConsoleAudioChannel %s activated\n" , getName() );
	
}

void ConsoleAudioChannel::deactivate()
{
	AudioChannel::deactivate();
	myCAudioState = console_audio_no;

#if 0
	free(dtmfBufs);
	dtmfBufs = 0;
#endif
	getEndPoint()->setPeerSignalType(dtmf_signal_out_band);
	cpLog(LOG_DEBUG, "ConsoleAudioChannel %s deactivated\n" , getName() );

}

int ConsoleAudioChannel::ReadVData(char * buffer)
{
	if(myDevType == CONSOLE_SLAVE)
		return 0;
	assert(buffer);

	int interval;
	struct timeval nowTime;
	int length = 0;

	if(myPeer!= 0 &&  strncmp(myPeer->getName(), "VOIP", 4) == 0)
	{
		gettimeofday (&nowTime, 0);
		interval = 1000000*(nowTime.tv_sec-prevTime.tv_sec) + nowTime.tv_usec - prevTime.tv_usec;
		if(interval<17000)
		{
			return 0;
		}
	}

	if(myCAudioState == console_audio_greeting )
	{
//		myFd = myGreetingFd;
		length = readGreeting( buffer);
	}
	else if(myCAudioState == console_audio_dial_tone )
	{	
//		cpLog(LOG_DEBUG, "%s play dial tone", getName());
		//--return as_dsp_readDialTone( buffer);
	}	
	else if( myCAudioState == console_audio_parse_dtmf)
	{	
//		cpLog(LOG_DEBUG, "%s is parsing DTMF, no data is send", getName());
		length = 0;
	}
	else if( myCAudioState == console_audio_silence )
	{		
//		cpLog(LOG_DEBUG, "%s play silence", getName());
		length = as_dsp_readSilence(myDsp, buffer);
	}
	#if 1
	else if( myCAudioState == console_audio_ringback)
	{	
//		cpLog(LOG_DEBUG, "%s play ringback tone", getName() );
		length = as_dsp_readRingBackTone(myDsp, buffer);
	}
	#endif 
	else if( myCAudioState == console_audio_busy)
	{	
//		cpLog(LOG_DEBUG, "%s play busy tone", getName() );
		length = as_dsp_readBusyTone(myDsp,  buffer);
	}	
	else
	{	
//		cpLog(LOG_DEBUG, "%s no audio is play", getName());
		length = 0;
	}

	prevTime = nowTime;
	return length;
 }

int ConsoleAudioChannel::WriteVData(char * buffer, int length)
{
	if( getEndPoint()->getPeerSignalType() == dtmf_signal_out_band )
		return length;
#if 0	
	if (supRFC2833)
	{
		return 0;
	}
#endif

	if( myDevType == CONSOLE_SLAVE)
		return 0;
	
	if( (myPeerState == PEER_SLAVE) )// && (myCAudioState == console_audio_dial_tone||myCAudioState == console_audio_parse_dtmf) )
	{
		if(myEndPoint->getState()->getStateType() == CONSOLE_EP_STATE_IN_GREETING)
		{

#if 0
			if(dtmfBufInIndex == dtmfBufOutIndex && dtmfBufOutIndex != -1 )
			{
				cpLog(LOG_ERR, "%s : DTMF parse slow, so data was ignored, please enlarge the DTMT Buffer,InIndex:%d, OutIndex:%d", getName(), dtmfBufInIndex, dtmfBufOutIndex);
				return length;
			}
#endif			

			if( (dtmfBufInPos+length) > dtmfBufSize*dtmfBufCount)
			{
				dtmfBufInPos = 0;
				dtmfBufInIndex = 0;
			}	
			
			memcpy(dtmfBufs+dtmfBufInPos, buffer, length) ;
			dtmfBufInPos += length;
			if( dtmfBufInPos >= (dtmfBufInIndex+1)*dtmfBufSize )
			{
				dtmfBufInIndex = (dtmfBufInIndex + 1)%dtmfBufCount;
				cpDebug(LOG_DEBUG, "populate data into %d Buffer\n\n", dtmfBufInIndex);
			}

			return length;
		}	
	}

	return 0;
}


int ConsoleAudioChannel::readGreeting(char *buffer)
{
//	if(myGreetingIndex<myGreetingSize)
	{
		memcpy(buffer, myGreetingData+myGreetingIndex*AS_DRIVER_BUFFER_LENGTH, AS_DRIVER_BUFFER_LENGTH);
		myGreetingIndex = (myGreetingIndex+1)%myGreetingSize;
		return AS_DRIVER_BUFFER_LENGTH;
	}
}

void ConsoleAudioChannel::setAudioState( console_audio_state_t audioState)
{
	myCAudioState = audioState;

	if(audioState == console_audio_silence && isDecoding)
		isDecoding = false;
}

Sptr <ConsoleAudioChannel> ConsoleAudioChannel::getSlave()
{
	return mySlave;
}

void ConsoleAudioChannel::setSlave(Sptr <ConsoleAudioChannel> slave)
{
	mySlave = slave;
}

#if 0
void ConsoleAudioChannel::stopDialTone()
{
	myCAudioState = console_audio_parse_dtmf;
}
#endif 

void ConsoleAudioChannel::emitDtmfSignal(char dtmf)
{
	supRFC2833 = true;
	myEndPoint->genDtmfEvent(dtmf);
}

void ConsoleAudioChannel::stopDialTone()
{
	myCAudioState = console_audio_silence;
}

