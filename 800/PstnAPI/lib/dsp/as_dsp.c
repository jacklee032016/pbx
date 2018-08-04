/*
* $Id: as_dsp.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
* $Aothor$
*/

/*
* Common Interface routines for Assist DSP Library
*/
#include "assert.h"
#include <stdlib.h>
#include <string.h>
 #include <stdio.h>
 #include <unistd.h>

#include "assist_lib.h"
#include "as_dsp.h"
#include "as_version.h"

#define DSP_ENGINE(engine ,dsp) \
	as_dsp_priv_t *engine = (as_dsp_priv_t *)(dsp->priv); \
	assert(dsp); \
	assert(engine)
	

#define  TONE_ENGINE(toneEngine, dsp) \
	as_dsp_priv_t *engine = (as_dsp_priv_t *)(dsp->priv); \
	assert(dsp); \
	assert(engine); \
	tone_t  *toneEngine = engine->tones 

as_dsp_t *as_dsp_init(as_dsp_config_t *dspConfig)
{
	as_dsp_priv_t *engine;
	as_dsp_t		*dsp;

	fsk_t		*fsk;
	busy_t		*busy;
	progress_t	*progress;
	tone_t		*tones;
	dtmf_t		*dtmf;
	as_dtmf_decoder_t		*consoleDecoder;
	
	printf( "Concurrent DSP Engine instance(Assist Corp.) ...\n");
	dsp = (as_dsp_t *)malloc( sizeof(as_dsp_t) );
	assert(dsp);
	memset(dsp, 0, sizeof(as_dsp_t) );
	if( dspConfig->standard!=PSTN_STANDARD_CHINA && dspConfig->standard!= PSTN_STANDARD_JAPAN)
	{
		printf("PSTN standard is invalidate, so default is Japanese standard\n");
		dspConfig->standard = PSTN_STANDARD_JAPAN;
	}
	dsp->config.standard = dspConfig->standard;
	dsp->config.law = dspConfig->law;
	dsp->config.toneLength = dspConfig->toneLength;

	if(dspConfig->dtmfMarkTime > 200 || dspConfig->dtmfMarkTime < 40 )
	{
		printf("ERROR : DTMF Mark time %d ms is not validate(must be 40~200ms)\n", dspConfig->dtmfMarkTime);
		assert(0);
	}
	if(dspConfig->dtmfDuration < dspConfig->dtmfMarkTime || dspConfig->dtmfDuration<100 )
	{
		printf("ERROR : DTMF Duration %d ms is not validate(must be bigger than MarkTime(%d ms) and more than 100ms\n", dspConfig->dtmfDuration, dspConfig->dtmfMarkTime);
		assert(0);
	}

	dsp->config.dtmfMarkTime = dspConfig->dtmfMarkTime/20*20;	/*in unit of 20 ms */
	dsp->config.dtmfDuration = dspConfig->dtmfDuration/20*20;

	engine = (as_dsp_priv_t *)malloc( sizeof(as_dsp_priv_t) );
	assert(engine);
	memset(engine, 0, sizeof(as_dsp_priv_t) );
	dsp->priv = engine;
	
	tones = as_dsp_new_tones( dsp);
	assert(tones);
	engine->tones = tones;
	
	fsk = as_dsp_new_fsk( dsp);
	assert(fsk);
	engine->fsk = fsk;

	dtmf = as_dsp_new_dtmf(dsp);
	assert(dtmf);
	engine->dtmf = dtmf;

	consoleDecoder = as_dtmf_new_decoder(dsp);
	assert(consoleDecoder);
	engine->dtmfDecoder = consoleDecoder;

	busy = as_dsp_new_busydetect(dsp);
	assert(busy);
	engine->busy = busy;

	progress = as_dsp_new_progress(dsp);
	assert(progress);
	engine->progress = progress;

	printf( AS_VERSION_INFO("Assist DSP Library"));
	return dsp;
}


as_dsp_t *as_dsp_init_no_cfg(law_t law, pstn_standard_t standard, int ms)
{
	as_dsp_priv_t *engine;
	as_dsp_t		*dsp;

	fsk_t		*fsk;
	busy_t		*busy;
	progress_t	*progress;
	tone_t		*tones;
	dtmf_t		*dtmf;
	as_dtmf_decoder_t		*consoleDecoder;
	
	printf( "Concurrent DSP Engine instance(Assist Corp.) ...\n");
	dsp = (as_dsp_t *)malloc( sizeof(as_dsp_t) );
	assert(dsp);
	memset(dsp, 0, sizeof(as_dsp_t) );

	dsp->config.standard = standard;
	dsp->config.law = law;
	dsp->config.toneLength = ms;

	if(standard == PSTN_STANDARD_CHINA)
	{
		dsp->config.dtmfMarkTime = DTMF_TONE_MARK_CHINA;
		dsp->config.dtmfDuration = DTMF_TONE_DURATION_CHINA;
	}
	else
	{
		dsp->config.dtmfMarkTime = DTMF_TONE_MARK_JAPAN;
		dsp->config.dtmfDuration = DTMF_TONE_DURATION_JAPAN;
	}

	engine = (as_dsp_priv_t *)malloc( sizeof(as_dsp_priv_t) );
	assert(engine);
	memset(engine, 0, sizeof(as_dsp_priv_t) );
	dsp->priv = engine;
	
	tones = as_dsp_new_tones( dsp);
	assert(tones);
	engine->tones = tones;

	fsk = as_dsp_new_fsk( dsp);
	assert(fsk);
	engine->fsk = fsk;

	dtmf = as_dsp_new_dtmf(dsp);
	assert(dtmf);
	engine->dtmf = dtmf;

	consoleDecoder = as_dtmf_new_decoder(dsp);
	assert(consoleDecoder);
	engine->dtmfDecoder = consoleDecoder;

	busy = as_dsp_new_busydetect(dsp);
	assert(busy);
	engine->busy = busy;

	progress = as_dsp_new_progress(dsp);
	assert(progress);
	engine->progress = progress;

	printf( AS_VERSION_INFO("Assist DSP Library"));
	return dsp;
}

void as_dsp_destroy(as_dsp_t *dsp)
{
	as_dsp_priv_t *engine = (as_dsp_priv_t *)dsp->priv;
	assert(engine);
	
	as_dsp_destroy_tone(engine->tones);
	as_dsp_destroy_fsk(engine->fsk);
	as_dsp_destroy_dtmf(engine->dtmf);
	as_dsp_destroy_busydetect(engine->busy);
	as_dsp_destroy_progress(engine->progress );

	free(engine);
	engine = 0;

	free(dsp);
	dsp = 0;
}

/* return 0 : not detect; other AS_PROGRESS_XXX */
int as_dsp_progress_detect(as_dsp_t *dsp, unsigned char *buff,int length)
{
	progress_t	*progress;
	DSP_ENGINE(engine ,dsp);
	progress = engine->progress;

	return as_progressdetect(progress,  buff, length,dsp->config.standard);
}

int as_dsp_progress_detect_busy(as_dsp_t *dsp, unsigned char *buff,int length)
{
	progress_t	*progress;
	DSP_ENGINE(engine ,dsp);
	progress = engine->progress;

	return as_progressdetect_busy(progress,  buff, length,dsp->config.standard);
}

/* return 0 : not detect; 1 : detect BT */
int as_dsp_busydetect(as_dsp_t *dsp, unsigned char *buff,int length)
{
	busy_t	*busy;
	DSP_ENGINE(engine ,dsp);
	busy = engine->busy;

	return as_busydetect(busy,  buff, length);
}

/* return 1 : FSK modem data, used in decode; 0 : silence */
int as_dsp_fsk_isVAD(as_dsp_t *dsp, unsigned char *buf, int length)
{
#define AS_FSK_SILENCE_THRESHOLD 			200 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */
	int i;
	int accum = 0;
	fsk_t	*fsk;
	DSP_ENGINE(engine ,dsp);
	fsk = engine->fsk;

	if(length<= 0)
		return 0;

	for (i=0; i< length; i++) 
		accum += abs( XLAW( buf[i], fsk->law) );
	accum /= length;

#if 0			
	printf("\t\t : accum : %d\n", accum);
#endif

	if(accum >= AS_FSK_SILENCE_THRESHOLD )
		return 1;
	
	return 0;
}

int as_dsp_dtmf_detect(as_dsp_t *dsp, unsigned char *sample, int length, char *callId, int max)
{
	dtmf_t	*dtmf;
	DSP_ENGINE(engine ,dsp);
	dtmf = engine->dtmf;

	return as_dtmf_detect(dtmf, sample, length, callId, max);
}

int as_dsp_dtmf_console_decode(as_dsp_t *dsp, unsigned char *sample, int length, char *callId, int max)
{
	as_dtmf_decoder_t	*dtmf;
	DSP_ENGINE(engine ,dsp);
	dtmf = engine->dtmfDecoder;

	return as_dtmf_decoding(dtmf, sample, length, callId, max);
}

void as_dsp_dtmf_decoder_reset(as_dsp_t *dsp)
{
	as_dtmf_decoder_t	*dtmf;
	DSP_ENGINE(engine ,dsp);
	dtmf = engine->dtmfDecoder;

	as_dtmf_reset_decoder(dtmf);
}

int as_dsp_fsk_decode_fd(as_dsp_t *dsp, int fd, char *number, char *name )
{
	fsk_t	*fsk;
	DSP_ENGINE(engine ,dsp);
	fsk = engine->fsk;
	if(dsp->config.standard == PSTN_STANDARD_CHINA)
		return as_fsk_decode(fsk, fd, number, name);
	
	return as_fsk_ntt_decode(fsk, fd, number);
}

pstn_param_t *as_dsp_fsk_decode(as_dsp_t *dsp, unsigned char *rawdata, int length)
{
	fsk_t	*fsk;
	DSP_ENGINE(engine ,dsp);
	fsk = engine->fsk;
	
	if(dsp->config.standard == PSTN_STANDARD_CHINA)
		return as_fsk_decode_chinese_clid(fsk, rawdata, length);
	return as_fsk_decode_ntt_clid(fsk, rawdata, length);
}

/* encode FSK data into a buffer which is allocated statically, so no free is must. Length of this buffer is 'returnLength' */
unsigned char *as_dsp_fsk_encode(as_dsp_t *dsp, char *number , char *name , int *returnLength )
{
	fsk_t	*fsk;
	DSP_ENGINE(engine ,dsp);
	fsk = engine->fsk;
	if(dsp->config.standard == PSTN_STANDARD_CHINA)
		return as_dsp_lib_encode_chinese_callerid(fsk, number, name, returnLength );
	return as_dsp_lib_encode_ntt_callerid(fsk, number, returnLength );
}

int as_dsp_fsk_ring_with_callerid(as_dsp_t *dsp, int fd , char *number , char *name )
{
	fsk_t	*fsk;
	DSP_ENGINE(engine ,dsp);
	fsk = engine->fsk;

	if(dsp->config.standard == PSTN_STANDARD_CHINA)
		return as_fsk_ring_with_callerid(fsk, fd, number, name);
	return as_fsk_ntt_ring_with_callerid(fsk, fd, number);
}

int as_dsp_readDialTone(as_dsp_t *dsp, char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dialTone );

	return as_tone_read_DT(audio, buffer);
}

int as_dsp_readRingBackTone(as_dsp_t *dsp, char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->ringBack);

	return as_tone_read_DT(audio,  buffer);
}

int as_dsp_readBusyTone(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->busyTone );

	return as_tone_read_DT(audio, buffer);
}

/* for SDT(Second Dial Tone) */
int as_dsp_readSDTone(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->secondDT );

	return as_tone_read_DT(audio, buffer);
}

/* for both Notification(NFT) and Acceptance(CPT) */
int as_dsp_readNFTTone(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->notifyTone );

	return as_tone_read(audio, buffer);
}

/* for IIT(Incoming Identification Tone) */
int as_dsp_readIITone(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->iitTone.tone );

	return as_tone_read(audio, buffer);
}

int as_dsp_readSilence(as_dsp_t *dsp,char *buffer)
{
	memcpy(buffer, tone_SILENCE, AS_TONE_SILENCE_SIZE);
	return AS_TONE_SILENCE_SIZE;
}


int as_dsp_readDtmf_1(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[0]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_2(as_dsp_t *dsp, char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[1]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_3(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[2]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_A(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[3]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_4(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[4]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_5(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[5]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_6(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[6]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_B(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[7]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_7(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[8]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_8(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[9]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_9(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[10]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_C(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[11]);
	return as_tone_read(audio, buffer);
}

/* '*' */
int as_dsp_readDtmf_asterisk(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[12]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_0(as_dsp_t *dsp,char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[13]);
	return as_tone_read(audio, buffer);
}

/* '#' */
int as_dsp_readDtmf_shark(as_dsp_t *dsp, char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[14]);
	return as_tone_read(audio, buffer);
}

int as_dsp_readDtmf_D(as_dsp_t *dsp, char *buffer)
{
	TONE_ENGINE(toneEngine, dsp);
	audio_t *audio = &(toneEngine->dtmf_chars[15]);
	return as_tone_read(audio, buffer);
}

typedef int (*as_dsp_read_tonedata_t)(as_dsp_t *dsp, char* buffer);

static int __as_dsp_tone_play(as_dsp_t *dsp,int fd, int ms, as_dsp_read_tonedata_t  read_func)
{
	char buf[102400];
	int length;
	int total = 0;

	while(1)
	{
//		trace;
		length = (read_func)(dsp, buf);
		if(length < 0 || length > 1024)
		{
			printf("Buffer overflow\n");
			exit(1);
		}

#if 0//AS_DSP_DEBUG
		printf("total length =%d read length =%d\n", total, length);
#endif

		total += length;
		if(total> ms*8)
			break;
		
		length = write(fd, buf, length);

	};
	
	return length;
}


int as_dsp_tone_play_dial(as_dsp_t *dsp, int fd, int ms ) 
{
	return __as_dsp_tone_play(dsp, fd ,ms,  as_dsp_readDialTone );
}

int as_dsp_tone_play_busy(as_dsp_t *dsp,  int  fd , int ms) 
{
	return __as_dsp_tone_play(dsp, fd ,ms , as_dsp_readBusyTone);
}

int as_dsp_tone_play_ringback(as_dsp_t *dsp,  int  fd, int ms ) 
{
	return __as_dsp_tone_play(dsp,  fd, ms, as_dsp_readRingBackTone);
}

/* NFT(Notification Tone) */
int as_dsp_tone_play_notify_tone(as_dsp_t *dsp,  int  fd, int ms ) 
{
	return __as_dsp_tone_play(dsp, fd, ms, as_dsp_readNFTTone);
}

/* CPT(Acceptance Tone) */
int as_dsp_tone_play_accept_tone(as_dsp_t *dsp,  int  fd, int ms ) 
{
	return __as_dsp_tone_play(dsp, fd, ms, as_dsp_readNFTTone);
}

/* SDT */
int as_dsp_tone_play_second_dial_tone(as_dsp_t *dsp,  int  fd, int ms ) 
{
	return __as_dsp_tone_play(dsp, fd, ms, as_dsp_readSDTone);
}

/* IIT */
int as_dsp_tone_play_incoming_id_tone(as_dsp_t *dsp,  int  fd, int ms ) 
{
	return __as_dsp_tone_play(dsp, fd, ms, as_dsp_readIITone);
}

int __as_dsp_play_dtmf_char(as_dsp_t *dsp, int fd, int ms, char signal)
{
	as_dsp_read_tonedata_t read_func;

	switch(signal)
	{
		case '0':
			read_func = as_dsp_readDtmf_0;
			break;
		case '1':
			read_func = as_dsp_readDtmf_1;
			break;
		case '2':
			read_func = as_dsp_readDtmf_2;
			break;
		case '3':
			read_func = as_dsp_readDtmf_3;
			break;
		case '4':
			read_func = as_dsp_readDtmf_4;
			break;
		case '5':
			read_func = as_dsp_readDtmf_5;
			break;
		case '6':
			read_func = as_dsp_readDtmf_6;
			break;
		case '7':
			read_func = as_dsp_readDtmf_7;
			break;
		case '8':
			read_func = as_dsp_readDtmf_8;
			break;
		case '9':
			read_func = as_dsp_readDtmf_9;
			break;
		case '*':
			read_func = as_dsp_readDtmf_asterisk;
			break;
		case '#':
			read_func = as_dsp_readDtmf_shark;
			break;
		case 'A':
			read_func = as_dsp_readDtmf_A;
			break;
		case 'B':
			read_func = as_dsp_readDtmf_B;
			break;
		case 'C':
			read_func = as_dsp_readDtmf_C;
			break;
		case 'D':
			read_func = as_dsp_readDtmf_D;
			break;
		default:
			printf("Call ID '%c' is not a validate charactor\n", signal);
			return -1;
			break;
	}

	return __as_dsp_tone_play(dsp, fd, ms, read_func);
}

int as_dsp_play_dtmf(as_dsp_t *dsp, int fd, char *callerId)
{
	int totalChars;
	int i = -1;
	char number[32];
//	char signal;
#if 0
	int length = 0;

	if(dsp->costandard == PSTN_STANDARD_CHINA)
	{
		length = DTMF_TONE_DURATION_CHINA;
	}
	else
	{
		length = DTMF_TONE_DURATION_JAPAN;
	}
#endif

	printf("DTMF Dial with : \n");
	memset(number, 0 ,sizeof(number) );
	sprintf(number, "%s", callerId);

	totalChars = strlen(number);
	if( totalChars <=0 || totalChars >=32 )
	{
		printf("DTMF ERROR : length is %d\n", totalChars);
		return -1;
	}

	printf("DTMF Dial with : \n");
	for(i=0; i< totalChars; i++)
	{
//		signal = [i];
		printf("DTMF Dial with : %c \n", number[i]);
		__as_dsp_play_dtmf_char(dsp, fd, dsp->config.dtmfDuration,  number[i]);
	}
	printf("\n");

	return i;
}


