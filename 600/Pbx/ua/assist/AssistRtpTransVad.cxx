#include <AssistRtpTransVad.hxx>
#include "rtpCodec.h"
#include "rtpTools.hxx"
#include "RtpTransmitter.hxx"
#include <math.h>

using namespace Vocal;

AssistRtpTransVad::AssistRtpTransVad(RtpSession *audioStack,RtpPayloadType apiType,RtpPayloadType netType,int sampleRate)
	:AssistRtpTransmitter(audioStack),
	apiFormat(apiType),
	networkFormat(netType),
	networkFormat_perSampleSize(sampleRate)
{
	tran=myAudioStack->getRtpTran();
	assert(tran);
	network_pktSampleSize=tran->getNetworkPktSampleSize();
	networkFormat_payloadSize=network_pktSampleSize*networkFormat_perSampleSize;
	ssrc=tran->getSSRC();
	
	silence=false;
	silenceDuration=1;
	outPos=0;
	recPos=0;
	memset (outBuff, 0, OUT_BUFFER_SIZE);
	seedRtpTime = generate32();
	prevRtpTime = seedRtpTime;
	stopCount = 32000;
	count = 0;
	prev = 1700;
	timR=0;
	vadInit();
}


AssistRtpTransVad::~AssistRtpTransVad()
{
}

int AssistRtpTransVad::transmitRaw(char *buf, int cc)
{
	
	int len=cc;
	char *p=vadBuffer;

	timR++;
	if(timR < MULTIWAY_THRESHOLD)
		if(silence)
		{
			silenceDuration++;
			return 0;
		}
		else return vadTransmitter(buf,cc);
	if(timR > MULTIWAY_THRESHOLD)timR=0;

	if(apiFormat != rtpPayloadL16_8k_mono && apiFormat != rtpPayloadL16_mono)
	{
		len = convertCodec(apiFormat, rtpPayloadL16_8k_mono, buf, vadBuffer, cc);
		assert(len > 0);
	}
	else
		p=buf;

	if(!vadProcess((S2byte *)p, len/2))
	{
		//silence detected	
		if(!silence)
		{
			cpDebug(LOG_DEBUG, "[VAD] ::: silence detected :::");
			silence=true;
		}
		silenceDuration++;
		return 0;
	}
	else
	{
		//speech
		if(silence)
		{
			cpDebug(LOG_DEBUG, "[VAD] ::: speech :::");
			silence=false;
		}
		return vadTransmitter(buf,cc);
	}
	
}

int AssistRtpTransVad::vadTransmitter (char* data, int len)
{
    assert (data);
    assert(len >= 0);

    int len1;

    char *data_buf = 0;
    char *buffer = 0;
    // convert codec
    if( apiFormat != networkFormat)
    {
        buffer = new char[1012];
        len = convertCodec(apiFormat, networkFormat, data, buffer, len);
        data_buf = buffer;
    }
    else
    {
       data_buf = data;
    }

    assert (data_buf);

    // write packet to output buffer
    if( (outPos + len) < OUT_BUFFER_SIZE)
    {
        memcpy (outBuff + outPos, data_buf, len);
        outPos += len;
    }
    else
    {
        // circular memory copy
        len1 = OUT_BUFFER_SIZE - outPos;
        memcpy (outBuff + outPos, data_buf, len1);
        memcpy (outBuff, data_buf + len1, len - len1);
        outPos = len - len1;
    }


    // check if enough data to send out packet
    int packetSize = networkFormat_payloadSize;
    if(  (outPos == 0) && (recPos == 0) )
    {
        cpLog (LOG_DEBUG_STACK, "Buffer is empty");
        return 0;
    }
    if(  ((outPos + OUT_BUFFER_SIZE - recPos) % OUT_BUFFER_SIZE) < packetSize )
    {
        cpLog (LOG_DEBUG_STACK, "Not enough data for a network packet.  Need %d",
               packetSize);
        return 0;
    }


    // send out packets from buffer
    int result = 0;

    // create packet
    RtpPacket* p = new RtpPacket (networkFormat_payloadSize);
    assert (p);
    p->setSSRC (ssrc);
    p->setPayloadType (networkFormat);
    p->setPayloadUsage (packetSize);

    //fill packets
    while ( ((outPos + OUT_BUFFER_SIZE - recPos) % OUT_BUFFER_SIZE) >= packetSize )
    {
        if(  (recPos + packetSize) < OUT_BUFFER_SIZE)
        {
            memcpy (p->getPayloadLoc(), outBuff + recPos, packetSize);
            recPos += packetSize;
        }
        else
        {
            // circular memory write
            len1 = OUT_BUFFER_SIZE - recPos;
            memcpy (p->getPayloadLoc(), outBuff + recPos, len1);
            memcpy (p->getPayloadLoc() + len1, outBuff, packetSize - len1);
            recPos = packetSize - len1;
        }
        // finish packet
	RtpTime currRtpTime=prevRtpTime + network_pktSampleSize * silenceDuration;
	p->setRtpTime( currRtpTime);
	prevRtpTime=currRtpTime;
	silenceDuration=1;
        result += tran->transmit(p);
    }
    if( p) delete p;
    p = 0;

    if( buffer != 0 )
    {
	delete [] buffer;
    }

    // exit with success
    return result;
}

S2byte 
AssistRtpTransVad::sature16(S4byte data)
{
  if ( data > (S2byte)0x7fff )
    {
    	return (S2byte)MAX_16;
    }
  if ( data < (S2byte)0xffff8000 )
    {
    	return (S2byte)MIN_16;
    }
  return (S2byte)data; 
}

void
AssistRtpTransVad::bqInit (t_biquad *bq, S2byte a0, S2byte a1, S2byte a2, S2byte b1, S2byte b2)
{
   bq->a0 = a0;
   bq->a11 = a1;
   bq->a12 = a2; 
   bq->negb11 = -b1;
   bq->negb12 = -b2;
   bq->dn_1 = 0;
   bq->dn_2 = 0;
}

void
AssistRtpTransVad::bqProcess(t_biquad *bq, S2byte *datain, S2byte *dataout, int n) 
{
    int i;

	for (i = 0; i < n; i++)
	{
		/* do biquad processing */
	    
		dataout[i] = datain[i] - prev;
		prev = datain[i];
	}
    
}

void 
AssistRtpTransVad::calcPower ( int length, U4byte *login, float *logout)	// formerly called log10_32()
{
    int i;

	for (i = 0; i < length; i++)
	{
		logout[i] = (float) (10.0 *( (float)log10((float)login[i]) - 9.332)); 
	}
    
}


void 
AssistRtpTransVad::vadInit()
{ 

  vadd.enabled          = TRUE;
  vadd.bq               = &vadbq;
  vadd.sta              = 10000;
  vadd.noiseTH          = VAD_NOISE_TH_BASE;
  vadd.state            = VadState_Unknown;
  vadd.noiseFloorCnt    = VAD_NOISEFLOOR_CNT_INIT;      
  vadd.noiseFloor       = VAD_NOISE_FLOOR_INIT;                         
  vadd.hangoverCnt      = VAD_HANGOVER_CNT_INIT;
  vadd.STARise          = 1;                                
  
  vadd.stateTxCount     = 0;

  vadd.signalMax        = VAD_SIGNAL_MAX_INIT;
  vadd.signalMaxCnt     = VAD_SIGNALMAX_CNT_INIT;
    
  bqInit(vadd.bq, vhpfB0, vhpfB1, vhpfB2, vhpfA1, vhpfA2);
}
   
U4byte 
AssistRtpTransVad::computeSTA(S2byte *pdata, int length, U4byte *minSta)
{
  int  i;
  S4byte  acc0,acc1;
  U4byte  maxSta;
  
  *minSta = vadd.sta;
  maxSta = vadd.sta;
  
  for (i = 0; i < length; i++)
    {
      
      /* q.15 * q.15 = q.30 */       
      acc1  = pdata[i] * pdata[i];
      
      if ( vadd.STARise )
        {
          acc0  = -1 * (S4byte)(vadd.sta >> 6);
          acc1  = acc1 >> 5;
        }
      else
        {
          acc0  = -1 * (S4byte)(vadd.sta >> 9);
          acc1  = acc1 >> 8;
        } /* if */
          
      acc0 += acc1;
      vadd.STARise = ( 0 >= acc0 ) ? 0 : 1;
      vadd.sta += acc0;      

      if ( vadd.sta > maxSta )
        {
          maxSta = vadd.sta;	// arijit - i added the cast
        }
      else if ( vadd.sta < *minSta )
        {
          *minSta = vadd.sta;
        }
                
    } /* for */
  
  return maxSta;
  
}

void 
AssistRtpTransVad::computeNFE(float minpower, float maxpower, int length)
{
    
  if ( minpower <= vadd.noiseFloor ) 
    { 
      vadd.noiseFloor = minpower;
      vadd.noiseFloorCnt = VAD_NOISEFLOOR_CNT_INIT;
    }
  else
    { 
      if ( vadd.noiseFloorCnt < length )
        {
          vadd.noiseFloor += 1;
          vadd.noiseFloorCnt = 
            (VAD_NOISEFLOOR_CNT_INIT + vadd.noiseFloorCnt - length);
        }
      else
        {
          vadd.noiseFloorCnt -= length;
        }
    }
 
}

boolean 
AssistRtpTransVad::vadSubProcess(S2byte *data, int length)
{
  boolean  SpeechDetected;
  boolean  FrameSpeechFlag;
  S2byte   tmpData[DATA_FRAME_LENGTH];
  U4byte   sta[2];
  float    power[2];
  
  SpeechDetected  = TRUE;
  FrameSpeechFlag = FALSE;
   
  bqProcess(vadd.bq, data, tmpData, length);
 
  sta[1] = computeSTA(tmpData, length, &sta[0]);
  
  calcPower(2, sta, power);
  
  computeNFE(power[0], power[1], length);

  count += length;
  if (count >= stopCount)
	  {
		  count = 0;
	  }

  if (power[1] > (vadd.noiseFloor + vadd.noiseTH)) 
    { 
      FrameSpeechFlag = TRUE;  
    } 
          
  if  ( FrameSpeechFlag == FALSE) 
    { 
      if ( vadd.hangoverCnt < length ) 
        {
          SpeechDetected  = FALSE;
          vadd.hangoverCnt = 0;
		  if ( vadd.state != VadState_Silence )
		  {
			  vadd.stateTxCount++;
		  }
          vadd.state = VadState_Silence;
        }
      else
        { 
          vadd.hangoverCnt -= length;
        }
    }
  else 
    {
      vadd.hangoverCnt = VAD_HANGOVER_CNT_INIT;
	  if ( vadd.state == VadState_Silence )
		  {
			  vadd.stateTxCount++;
		  }
      vadd.state = VadState_Speech;
    }
      
  return SpeechDetected;
  
}

/* 
 * Returns:  true for speech
 *          false for silence 
 */  
boolean 
AssistRtpTransVad::vadProcess(S2byte *data, int length)
{ 

  /* vadProcess locals */
  int idx;
  int step;
  boolean ret;

  ret = FALSE;
    
  if ( vadd.enabled == TRUE )
    {
//	printf("vadProcess: origin ret %d\t",ret);
	  /* Cut up the frame into 5ms chunks for processing purposes */
      for (idx = 0; length > 0; length -= step)
        {  
		  step = (length < DATA_FRAME_LENGTH) ? length : DATA_FRAME_LENGTH;
          ret |= vadSubProcess(&data[idx], step);
//	printf("ret = %d \t ",ret);
          idx += step;
        }

  }
  else
    {
      ret = TRUE;
    }
//  printf("\n");
  return ret;

}

