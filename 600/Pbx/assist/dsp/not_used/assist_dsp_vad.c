#include <math.h>
#include "assist_dsp_vad.h"

LOCAL t_biquad vadbq;
LOCAL t_vad vadd;
unsigned long stopCount = 32000;

S2byte 
sature16(S4byte data)
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
bqInit (t_biquad *bq, S2byte a0, S2byte a1, S2byte a2, S2byte b1, S2byte b2)
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
bqProcess(t_biquad *bq, S2byte *datain, S2byte *dataout, int n) 
{
    int i;
    static S2byte prev = 1700;

	for (i = 0; i < n; i++)
	{
		/* do biquad processing */
	    
		dataout[i] = datain[i] - prev;
		prev = datain[i];
	}
    
}

void 
calcPower ( int length, U4byte *login, float *logout)	// formerly called log10_32()
{
    int i;

	for (i = 0; i < length; i++)
	{
		logout[i] = (float) (10 * log10(login[i]) - 10 * 9.332); 
	}
    
}

void
vadInit()
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
   
LOCAL U4byte
computeSTA(S2byte *pdata, int length, U4byte *minSta)
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

LOCAL void 
computeNFE(float minpower, float maxpower, int length)
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

LOCAL boolean
vadSubProcess(S2byte *data, int length)
{
  boolean  SpeechDetected;
  boolean  FrameSpeechFlag;
  S2byte   tmpData[DATA_FRAME_LENGTH];
  U4byte   sta[2];
  float    power[2];
  static unsigned long count = 0;
  
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
vadProcess(S2byte *data, int length)
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



