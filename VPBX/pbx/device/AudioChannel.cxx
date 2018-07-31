/*
* $Id: AudioChannel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include <assert.h>

#include "EndPoint.hxx"
#include "CtrlDevice.hxx"
#include "AudioChannel.hxx"


using namespace Assist;

#define MAX_STEP 512

AudioChannel::AudioChannel(CtrlDevice *_parent, const char *_name)
	:myState(AUDIO_STATE_IDLE),
	myIndex(-1),
	myPeerState(PEER_NO),
	myPeer(0),
	supRFC2833(false),
	myBuildProcess(false)
{
	myParent = _parent;
	myEndPoint = 0;
	
	memset(myName, 0, VPBX_NAME_LENGTH);
	sprintf(myName, "%s", _name);
	
	mySelf = this;
}


AudioChannel::~AudioChannel()
{
	cpLog(LOG_DEBUG, "AudioChannel %s Destorying....." ,getName() );
}

/* activate/deactivate bchannel */
void AudioChannel::activate( )
{
	/* we must activate if we are deactivated */
#if 0	
	if (myState == AUDIO_STATE_IDLE)
	{
		/* activate bchannel */
		cpLog(LOG_DEBUG_STACK,  "activating bchannel (index %d), because currently idle (address 0x%x).\n",  myIndex );
		myState = AUDIO_STATE_ACTIVATING;
		return;
	}

	/* if we are active, we configure our channel */
	if ( myState == AUDIO_STATE_ACTIVE)
	{
#ifdef DSP_MODULE
		cpLog(LOG_DEBUG_STACK,  "activating bchannel (index %d), because currently idle (address 0x%x).\n",  myIndex );
#endif
	}
#endif
	myState = AUDIO_STATE_ACTIVE;
}

void AudioChannel::deactivate()
{
#if 0
	cpLog(LOG_DEBUG_STACK,  "deactivating audiochannel (index ), because currently active.\n" );
	if ( myState == AUDIO_STATE_ACTIVE)
	{
		/* deactivate bchannel */
		cpLog(LOG_DEBUG_STACK,  "deactivating bchannel (index %d), because currently active.\n", myIndex );
		myState = AUDIO_STATE_DEACTIVATING;
		return;
	}
	supRFC2833 = false;
#endif
	myState = AUDIO_STATE_IDLE;
}

int AudioChannel::dspRxTx()
{
	return 1;
}

#if WITH_DEBUG
const char *AudioChannel::debugInfo()
{
	char *buf;
	int length = 0;

	buf = (char *)malloc(2049);
	assert(buf);

	length += sprintf(buf + length , "\r\n\tAudioChannel \t : %s \t index : %d" , getName(), getIndex() );
	length += sprintf(buf + length , "\r\n\t\tState \t\t : %d" , getState());
	length += sprintf(buf + length , "\r\n\t\tPeer \t\t : " , (myPeer != 0)?myPeer->getName() :"NO");
		
	return buf;
}
#endif

bool AudioChannel::canProcess()
{
	myMutex.lock();
	if( myState == AUDIO_STATE_ACTIVE && myPeerState== PEER_MASTER )
	{
		myMutex.unlock();
		return true;
	}
	else
	{
		myMutex.unlock();
		return false;
	}	
}

audio_state_t AudioChannel::getState()
{
	return myState;
}
void AudioChannel::setState(audio_state_t  _audioState)
{
	myState = _audioState;
}

/* master AudioChannel call this function */
void AudioChannel::setAudioPeer( const Sptr <AudioChannel> audioPeer )
{
	assert(audioPeer != 0);
	cpDebug(LOG_DEBUG, "%s is Master, peer is %s", getName(), audioPeer->getName() );
	
	myMutex.lock();
//	setAudioPeer(audioPeer, PEER_MASTER);
//	myState = AUDIO_STATE_ACTIVE;
#if 0
      if (false)
      	{
      		
      	}
	else
#endif		
	{
	
		myPeer = audioPeer;
		myPeerState = PEER_MASTER;
		myState = AUDIO_STATE_ACTIVE;

	//	audioPeer->setAudioPeer(this, PEER_SLAVE);
		audioPeer->myPeer = mySelf;
		audioPeer->myPeerState = PEER_SLAVE;
		audioPeer->myState = AUDIO_STATE_ACTIVE;
	}
	myMutex.unlock();

}

void AudioChannel::releaseAudioPeer( )
{
	myMutex.lock();
	if( myPeer == NULL)
	{
		myMutex.unlock();
		cpLog(LOG_ERR, "No AudioPeer is found in AudioChannel %s", getName() );
		return;
	}
	cpDebug(LOG_ERR, "AudioPeer of %s is %s cleared", getName() , myPeer->getName() );

//	myPeer->deactivate(); /* enter into deactivating state */
	myPeer->myState = AUDIO_STATE_IDLE;
	myPeer->myPeerState = PEER_NO;
	myPeer->myPeer = NULL;

//	deactivate();
	myState = AUDIO_STATE_IDLE;
	myPeerState = PEER_NO;
	myPeer = NULL;

	myMutex.unlock();
}


#if 1
void AudioChannel::processAudio()
{
	static char buf[AUDIO_BUFFER_SIZE];

	int c1, c2;
	myMutex.lock();
	if ( myPeerState != PEER_MASTER)
	{
		myMutex.unlock();
		return ;
	}

	assert( myPeer != 0);
	
	c1 = myPeer->ReadVData(buf);
	if(c1> 0)
		c2 = WriteVData(buf, c1);
	
	c1 = ReadVData(buf);
	if(c1>= 0)
		c2 = myPeer->WriteVData(buf, c1);
	
	myMutex.unlock();
}
#else

void AudioChannel::processAudio()
{
	int c1, c2;

	myMutex.lock();

#if 1
	
	static char bufa[MAX_STEP*2];	
//	char *tempa = bufa;
	static unsigned int rpos0 = 0;
	static unsigned int wpos0 = 0;
	static unsigned int overplus0 = 0;

	//printf("wpos0 : %d\n", wpos0);

	if (rpos0 > MAX_STEP  || overplus0 > MAX_STEP)
	{
		//assert(tempa == bufa);
		//cpLog(LOG_DEBUG, "wpos0 : %d", wpos0);
		//cpLog(LOG_DEBUG, "overplus0 : %d", overplus0);
		//cpLog(LOG_DEBUG, "rpos0 : %d", rpos0);
		//assert(wpos0 < (MAX_STEP*2));
		//assert(overplus0 < 160);
		//assert(overplus0 < wpos0);
		
		//memcpy(bufa, bufa+wpos0, overplus0);
		c2 = myPeer->WriteVData(bufa+wpos0, overplus0);
		while (c2 < 0)
		{
			c2 = myPeer->WriteVData(bufa+wpos0, overplus0);
		}
		wpos0 = 0;
		rpos0 = 0; //overplus0
		overplus0 = 0;
	}

	assert(rpos0 < (1024-320));
	//assert(rpos0 < (1024-160));
	c1 = ReadVData(bufa+rpos0);
	assert(c1 <= 320);
	//assert(c1 != -1);
	if (c1 > 0)
	{
		rpos0 += c1;
		assert(wpos0 < 1024);
		assert((c1+overplus0) < 1024);
		//printf("c1 : %d\n", c1);
		//printf("write len : %d\n", c1+overplus0);
		c2 = myPeer->WriteVData(bufa+wpos0, c1+overplus0);
		//printf("c2 : %d\n", c2);
#if 0
		if (c2 < 0)
		{
			c2 = myPeer->WriteVData(bufa+wpos0, c1+overplus0);
		}
#endif
		//assert(c2 <= 160);
		if (c2 < 0)
			c2 = 0;
		wpos0 += c2;
		overplus0 = c1+overplus0-c2;
		assert(overplus0 >= 0);
		//assert(overplus0 < MAX_STEP);
		/*if (overplus0 > MAX_STEP )
		{
			printf("Some bytes lost in  overplus0 - %d \n", overplus0-MAX_STEP);
			overplus0 = MAX_STEP;
		}*/
	}

	//printf("%d\n",c1);

	//return;
#endif 
	/**
	**********************************
	*/
//V2:

	static char bufb[MAX_STEP*2];
//	char *tempb = bufb;
	static unsigned int rpos1 = 0;
	static unsigned int wpos1 = 0;
	static unsigned int overplus1 = 0;
	//printf("wpos1 : %d\n", wpos1);

	if (rpos1 > MAX_STEP ||  overplus1 > MAX_STEP)
	{
		//assert(wpos1)
		//assert(tempb == bufb);
		//cpLog(LOG_DEBUG, "wpos1 : %d", wpos1);
		//cpLog(LOG_DEBUG, "overplus1 : %d", overplus1);
		//assert(wpos1 < (MAX_STEP*2));
		//assert(overplus1 < 160);
		//assert(overplus1 < wpos1);
		//memcpy(bufb, bufb+wpos1, overplus1);
		c2 = WriteVData(bufb+wpos1, overplus1);
		while (c2 < 0)
		{
			c2 = WriteVData(bufb+wpos1, overplus1);
		}
		wpos1 = 0;
		rpos1 = 0;
		overplus1 = 0;
	}

	assert(rpos1 < (1024-320));
	assert(rpos1 < (1024-160));
	c1 = myPeer->ReadVData(bufb+rpos1);
	//assert(c1 != -1);
	if (c1 > 0)
	{
		rpos1 += c1;
		assert(wpos1 < 1024);
		assert((c1+overplus1) < 1024);
		c2 = WriteVData(bufb+wpos1, c1+overplus1);
#if 0
		if (c2 < 0)
		{
			c2 = WriteVData(bufb+wpos1, c1+overplus1);
		}
#endif 
		if (c2 < 0)
			c2 = 0;
		wpos1 += c2;
		overplus1 = c1+overplus1-c2;
		assert(overplus1 >= 0);
		//assert(overplus1 < MAX_STEP);
		/*if (overplus1 > MAX_STEP )
		{
			printf("Some bytes lost in  overplus1 - %d \n", overplus1-MAX_STEP);
			overplus1 = MAX_STEP;
		}*/
	}
	
	myMutex.unlock();
	
}
#endif 

void AudioChannel::setIndex(int _index)
{
	myIndex = _index;
	int len = strlen(myName);
	sprintf(myName+len, "-%d", myIndex);
}

void AudioChannel::setEndPoint(Sptr <EndPoint> ep)
{
	if (ep != 0)
	{
		myEndPoint = ep;
	}
}

Sptr <EndPoint> AudioChannel::getEndPoint()
{
	return myEndPoint;
}

Sptr <CtrlDevice>	AudioChannel::getParent()
{
	return myParent;
}

int AudioChannel::ReadVData(char* buffer)
{
	return -1;
}
int AudioChannel::WriteVData(char* buffer, int length)
{
	return -1;
}

