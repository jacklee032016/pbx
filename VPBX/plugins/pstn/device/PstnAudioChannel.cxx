/*
* $Id: PstnAudioChannel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "TaskEvent.hxx"
#include "PstnEndPoint.hxx"
#include "PstnAudioChannel.hxx"
#include "PstnCtrlDevice.hxx"
#include "pstn.hxx"

using namespace Assist;

#if 0
static const int slicrate = 160;
static int crile = 0;
#endif

void *fxoDetectBT(void *param);

PstnAudioChannel::PstnAudioChannel(PstnCtrlDevice  *_parent, int fd )
	: AudioChannel(_parent, "PSTN-Audio")
{
	myEndPoint = _parent->getNextEP(0);
	assert(myEndPoint != 0);
	myFd = fd;

	myState = AUDIO_STATE_IDLE;
}


PstnAudioChannel::~PstnAudioChannel()
{
	cpLog(LOG_DEBUG, "PstnAudioChannel Destorying.....");
}


/* activate/deactivate bchannel */
void PstnAudioChannel::activate( )
{
#if 0
	Sptr <PstnEndPoint> pep = NULL;

	pep.dynamicCast(getEndPoint());
	assert(pep !=  0);

	if(! pep->checkExt() )
	{
		as_device_t *dev = pep->getMyDev();
		dev->btStage = BT_DETECT_STAGE_INPUT;
		dev->busyData = (char *)malloc(dev->btBufCount*AS_DRIVER_BUFFER_LENGTH);/* about 1200ms, 9600 byte */
		dev->btBufIndex = 0;
		new TaskEvent(fxoDetectBT, (void *)dev );
	}
#endif

}

void PstnAudioChannel::deactivate()
{
#if 0
	Sptr <PstnEndPoint> pep = NULL;
	pep.dynamicCast(getEndPoint());
	assert(pep !=  0);

	if(! pep->checkExt() )
	{
		as_device_t *dev = pep->getMyDev();
		dev->btStage = BT_DETECT_STAGE_INVALIDATE;
		if( dev->busyData != 0) 
		{
			free(dev->busyData);
			dev->busyData = 0;
		}
	}
#endif

}

int PstnAudioChannel::ReadVData(char * buffer)
{
	assert(buffer);
	int n;

	n = read (myFd, buffer, VOICE_RATE);
	cpDebug(LOG_DEBUG, "%s read %d bytes", getName(), n );
//	pep->busyDetect(buffer, n);
	
	return n;
}


int PstnAudioChannel::WriteVData(char * buffer, int length)
{
	if(length <= 0)
		return length;
	
	int len = 0, i = 0;
	for (i = 0; i < length / VOICE_RATE; ++i)
	{
		len += write (myFd, buffer+(VOICE_RATE*i), VOICE_RATE);
	}
#if 0
	if ((length % VOICE_RATE) > 0)
		len += write(myFd, buffer+(VOICE_RATE*i), length - (VOICE_RATE*i));
#endif
	cpDebug(LOG_DEBUG, "%s write %d bytes(Buffer Length is %d)", getName(), len, length);

	Sptr <PstnEndPoint> pep = NULL;
	pep.dynamicCast(getEndPoint());
	assert(pep !=  0);
	if(pep->checkExt() )
	{
		return len;
	}
	
	as_device_t *dev = pep->getMyDev();

	if(! pep->checkExt() && dev->btStage == BT_DETECT_STAGE_INPUT )
	{
		int copyLength = dev->btBufCount*AS_DRIVER_BUFFER_LENGTH - dev->btBufIndex;
		copyLength = (length< copyLength)?length:copyLength;
		memcpy(dev->busyData +dev->btBufIndex, buffer, copyLength );
		dev->btBufIndex += copyLength;
		if(dev->btBufIndex > dev->btBufCount*AS_DRIVER_BUFFER_LENGTH-1 )
		{
			dev->btBufIndex = 0;
			dev->btStage = BT_DETECT_STAGE_CHECK;
		}	
		
	}

	return len;
}

void PstnAudioChannel::processAudio()
{
	AudioChannel::processAudio();

#if 0
	if (!playBusy)
	{	
		AudioChannel::processAudio();
		return;
	}

	struct timeval tv1,tv2;
	static bool trvalflag = false;
	char busytoneData[slicrate];
	int cc;
	int len;
	if (!trvalflag)
	{
		 gettimeofday(&tv1, NULL);
		 trvalflag = !trvalflag;
	}
	else
	{
		gettimeofday(&tv2, NULL);
		if (tv2.tv_sec - tv1.tv_sec > 5)
		{
			//trvalflag = false;
			goto VOICEOVER;
		}
	}

	
	cc = read(tonefd, busytoneData, slicrate);
	while ( cc < slicrate)
      {
           	lseek(tonefd, 0, SEEK_SET);
             cc += read( tonefd, busytoneData + cc, slicrate - cc);
		 ++crile;
		if (crile > 1)
		{
			goto VOICEOVER;
		#if 0
			crile = 0;
			playBusy = false;
			myState = AUDIO_STATE_IDLE;
			assert(myEndPoint != 0);
			Sptr <PstnEndPoint> pep;
			pep.dynamicCast(myEndPoint);
			assert(pep != 0);
			cpLog(LOG_DEBUG, "--- BusyTone play over then Hookup");
			pep->getMyDev()->span->ops->tone_fastbusy_start(pep->getMyDev());
			//assert();
		#endif
		}
      }        

	len = WriteVData(busytoneData, slicrate);
	if (0 >= len)
	{
		cpLog(LOG_DEBUG, "Write Error!");
	}

VOICEOVER:
	crile = 0;
	playBusy = false;
	trvalflag = false;
	myState = AUDIO_STATE_IDLE;
	assert(myEndPoint != 0);
	Sptr <PstnEndPoint> pep;
	pep.dynamicCast(myEndPoint);
	assert(pep != 0);
	cpLog(LOG_DEBUG, "--- BusyTone play over then Hookup");
	pep->getMyDev()->span->ops->tone_fastbusy_start(pep->getMyDev());
#endif 
	//printf("wirted %d\n", len);
}


