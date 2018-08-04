#ifndef __ASSIST_DSP_VAD_H
#define __ASSIST_DSP_VAD_H

#ifdef __cplusplus
	extern "C"
	{
#endif
///////// macros /////////

#define S2byte	short
#define S4byte	long
#define U2byte	unsigned short
#define U4byte	unsigned long
#define boolean  int

#define abs16(data) ((data < 0) ? -data : data)
#define MIN_16 0x8000
#define MAX_16 0x7fff

#define TRUE	1
#define FALSE	0

#define LOCAL static 

#define DATA_FRAME_LENGTH    (5*8)

/* 
 * x = 1.5sec * 1000ms/sec * 8 samples/1 ms
 * samples => +1dB every x samples 
 */
#define VAD_NOISEFLOOR_CNT_INIT      (int)(8*1500)
#define VAD_SIGNALMAX_CNT_INIT       (int)(8*1500)

/* Power Thresholds */
#define VAD_NOISE_TH_BASE        (float) 10.00   /*  10.00 dB Noise Threshold */
#define VAD_NOISE_FLOOR_INIT     (float)-74.00   /* -74.00 dB Initial Noise Floor */
#define VAD_SIGNAL_MAX_INIT      (float)-80.00   /* -80.00 dB Initial Noise Max */
#define VAD_NOISE_TH_MIN         (float)  1.00   /*   1.00 dB Minimum Noise Threshold */

/* High Pass Filter for getting rid of background noise from
 * input signal before energy calculations */
/* Butter : */
#define vhpfB0   (S2byte) 14339
#define vhpfB1   (S2byte)-28678
#define vhpfB2   (S2byte) 14339
#define vhpfA1   (S2byte)-28422
#define vhpfA2   (S2byte) 12550

/* Number of samples of silence before we declare silence period */
/* #samples = 8 samples/ms * 500ms */            
#define VAD_HANGOVER_CNT_INIT        (int)(8*500)

/////////types /////////

typedef struct _biquad {

  S2byte negb12;
  S2byte a12;
  S2byte negb11;
  S2byte a11;
  S2byte a0;
  S2byte dn_2;
  S2byte dn_1;
  
} t_biquad;

typedef enum {
  VadState_Silence = 0,
  VadState_Speech,
  VadState_Unknown
} t_VadState;

typedef struct _vad {
  
	  boolean       enabled;
	  
	  /* Saved STA between input frames */
	  U4byte     sta;

	  /* state == 1 if VOICE
	   * state == 0 if SILENCE */
	  t_VadState state;
	  
	  /* Countdown of consecutive frames before we declare silence */
	  int     hangoverCnt;

	  /* Threshold above which a signal is considered to be speech */
	  float      noiseTH;

	  /* Countdown after which the noise floor is 
	   * incremented by 1dB */
	  int     noiseFloorCnt;
	  
	  /* Noise floor in dB */
	  float      noiseFloor;
	  
	  /* Countdown after which the signal max is
	   * decremented by 1dB */
	  int     signalMaxCnt;

	  /* Signal max in dB */
	  float      signalMax;  
	    
	  /* STARise == 1 if sta is rising
	   * STARise == 0 if sta is falling */   
	  int     STARise;

	  int     stateTxCount;
	  
	  /* High Pass Filter for input signal */
	  t_biquad  *bq;
    
} t_vad;

///////// prototypes /////////

extern S2byte sature16(S4byte data);                         
extern void bqProcess (t_biquad *bq, S2byte *datain, S2byte *dataout, int n);
extern void bqInit (t_biquad *bq, S2byte a0, S2byte a1, S2byte a2, S2byte b1, S2byte b2);                      
extern void calcPower ( int length, U4byte *datain, float *logout); // formerly called log10_32()
extern void vadInit();
/*
LOCAL U4byte computeSTA(S2byte *pdata, int length, U4byte *minSta);
LOCAL void computeNFE(float minpower, float maxpower, int length);
LOCAL boolean vadSubProcess(S2byte *data, int length);
*/
boolean vadProcess(S2byte *data, int length);
#ifdef __cplusplus
};
#endif

#endif	

