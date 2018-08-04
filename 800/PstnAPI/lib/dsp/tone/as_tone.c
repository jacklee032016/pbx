/*
 * $Log: as_tone.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.7  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.6  2006/05/28 10:14:58  lizhijie
 * dd
 *
 * Revision 1.5  2006/03/24 11:07:46  lizhijie
 * add a C-language file for DTMF and silence data and compile it into libpstn.so
 *
 * Revision 1.4  2006/03/24 11:06:30  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/23 09:35:32  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.1  2006/03/20 06:37:26  lizhijie
 * no message
 *
 * Revision 1.4  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.3  2006/03/16 08:38:23  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_tone.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "assist_lib.h"
#include "as_dsp.h"
#include "as_tone.h"

extern	unsigned char dtmf_raw_data[][1600];
extern 	unsigned char silence_raw_data[];

#if 0
extern float dtmf_row[4];
extern float dtmf_col[4];
#endif

static int __as_dsp_get_silence(unsigned char *data , int len , int law )
{
#define TONE_COPY_TIME			20			/* 20ms */
	int	j;
	int count = len/TONE_COPY_TIME;

	/* copy left data in 20ms step(160bytes) */
	for(j=0; j< count; j++)
	{
		memcpy( data+ j*8*TONE_COPY_TIME, silence_raw_data, TONE_COPY_TIME*8) ;
	}
	//printf("\n" );

	return j*8*TONE_COPY_TIME;
}

/* for both IIT(Incoming Identification) Tone */
static int _as_dsp_iit_init(tone_t *dsp, iit_tone_t *iit)
{
	audio_t		*tone;
	tone_data_t  	*data;
	int ln_mark, ln_spare;
	int duration_of_a = 60;		/* duration of 'a' in NTT p.22, about 125ms  */
	int length = 440;				/* 2*duration_of_a + length =560 */
	int part_a =500;
	int part_b = 500;

	iit->length = part_a + part_b;; /* begin part is composed by 2 parts p.22 */
	iit->beginPart = (unsigned char *)malloc( iit->length *8);
	assert(iit->beginPart);

	/* mark part is composed by 3 parts */
	/* part 'a' , 500ms mark */
	if( as_dsp_tone_arbtones( iit->beginPart, 400.0, 16.0, part_a, dsp->law) != part_a*8)
	{
		assert(0);
	}

#if 0
	/* part 'b' of beginpart */
	if( as_dsp_tone_arbtones( iit->beginPart + part_a*8, 0.0, 0.0, part_b, dsp->law) != part_b*8)
	{
		assert(0);
	}
#else

	if( __as_dsp_get_silence(iit->beginPart + part_a*8,  part_b, dsp->law) != part_b*8 )
	{
		assert(0);
	}

#endif
	
	tone = &(iit->tone);
	tone->dsp = dsp;
	tone->index = 0;

	tone->duration = 4000;
	tone->mark = 560;			/* contains 125ms space in the middle of 380ms */
	
	tone->freq1 = 400.0;
	tone->freq2 = 400.0;

	tone->readPosition = 0;
	
	tone->data = (tone_data_t *)malloc(sizeof(tone_data_t));
	assert( tone->data);
	data = tone->data;
		
	data->length = dsp->length ;

	ln_mark  = tone->mark;
	ln_spare = tone->duration - ln_mark;
	data->mark = (unsigned char *)malloc( sizeof(unsigned char)*ln_mark *8 );
	assert(data->mark != 0);

	/* mark part is composed by 3 parts */
	if( as_dsp_tone_arbtones( data->mark, tone->freq1, tone->freq2, duration_of_a, dsp->law) != duration_of_a*8)
	{
		assert(0);
	}

#if 0
	/* gap between 'a' , about 140 ms */
	if( as_dsp_tone_arbtones( data->mark + duration_of_a*8, 0.0, 0.0, length, dsp->law) != length*8)
	{
		assert(0);
	}
#else

	if(__as_dsp_get_silence(data->mark + duration_of_a*8, length, dsp->law ) != length*8 )
	{
		assert(0);
	}
#endif

	if( as_dsp_tone_arbtones( data->mark + (duration_of_a+length)*8, tone->freq1, tone->freq2, duration_of_a, dsp->law) != duration_of_a*8)
	{
		assert(0);
	}

	/* spare part for IIT */
	data->spare = (unsigned char *)malloc( sizeof(unsigned char)*ln_spare*8  );
	assert( data->spare != 0);
#if 0
//	if( as_dsp_tone_arbtones( data->spare, 0.0, 0.0, ln_spare, dsp->law) != ln_spare*8 )
	if( as_dsp_tone_silence( data->spare,  ln_spare, dsp->law) != ln_spare*8 )
	{
		assert(0);
	}
#else

	if( __as_dsp_get_silence(data->spare, ln_spare, dsp->law) != ln_spare *8 )
	{
		assert(0);
	}

#endif

	return 0;
}


/* for both NFT(Notification) and CPT(Acceptance) */
static int _as_dsp_nft_init(tone_t *dsp, audio_t *nft)
{
	tone_data_t  *data;
	int ln_mark, ln_spare;
	int duration_of_a = 120;		/* duration of 'a' in NTT p.22, about 125ms  */
	int length = 140;				/* 2*duration_of_a + length =380 */
	
	nft->dsp = dsp;
	nft->index = 0;

	nft->duration = 1000;
	nft->mark = 380;			/* contains 125ms space in the middle of 380ms */
	
	nft->freq1 = 400.0;
	nft->freq2 = 400.0;

	nft->readPosition = 0;
	
	nft->data = (tone_data_t *)malloc(sizeof(tone_data_t));
	assert( nft->data);
	data = nft->data;
		
	data->length = dsp->length ;

	ln_mark  = nft->mark;
	ln_spare = nft->duration - ln_mark;
	data->mark = (unsigned char *)malloc( sizeof(unsigned char)*ln_mark *8 );
	assert(data->mark != 0);

	/* mark part is composed by 3 parts */
	if( as_dsp_tone_arbtones( data->mark, nft->freq1, nft->freq2, duration_of_a, dsp->law) != duration_of_a*8)
	{
		assert(0);
	}

#if 0
	/* gap between 'a' , about 140 ms */
	if( as_dsp_tone_arbtones( data->mark + duration_of_a*8, 0.0, 0.0, length, dsp->law) != length*8)
	{
		assert(0);
	}
#else
	if(__as_dsp_get_silence(data->mark + duration_of_a*8, length, dsp->law ) != length*8)
	{
		assert(0);
	}
#endif

	if( as_dsp_tone_arbtones( data->mark + (duration_of_a+length)*8, nft->freq1, nft->freq2, duration_of_a, dsp->law) != duration_of_a*8)
	{
		assert(0);
	}

	/* spare part for NFT */
	data->spare = (unsigned char *)malloc( sizeof(unsigned char)*ln_spare*8  );
	assert( data->spare != 0);
#if 0
	if( as_dsp_tone_arbtones( data->spare, 0.0, 0.0, ln_spare, dsp->law) != ln_spare*8 )
	{
		assert(0);
	}
#else
	if( __as_dsp_get_silence( data->spare, ln_spare, dsp->law )!= ln_spare*8)
	{
		assert(0);
	}
#endif

	return 0;
}

/* length is in the unit of ms*8, ms must be below 100 ms */
static int as_dsp_init_tone(tone_t *dsp)
{
	
	int i = 0;
	tone_data_t	*dtmf_data;
	int length;

	dsp->law = dsp->root->config.law;
	dsp->freq1 = 400.0;
	dsp->freq2 = 400.0;
	dsp->length = dsp->root->config.toneLength;					/* 20ms, 160 bytes */

	length = dsp->length*8;
	dsp->data = (tone_data_t *)malloc(sizeof(tone_data_t));
	assert(dsp->data);
	dsp->data->length = dsp->length;
	
	dsp->data->mark = (unsigned char *)malloc( sizeof(unsigned char)*length );
	assert(dsp->data->mark != 0);
	if( as_dsp_tone_arbtones(dsp->data->mark, dsp->freq1, dsp->freq2, dsp->length, dsp->law) != length)
		assert(0);
	
	dsp->data->spare = (unsigned char *)malloc( sizeof(unsigned char)*length );
	assert(dsp->data->spare != 0);
#if 0	
	if( as_dsp_tone_arbtones(dsp->data->spare, 0.0, 0.0, dsp->length, dsp->law) != length)
		assert(0);
#else
	if(__as_dsp_get_silence(dsp->data->spare, dsp->length, dsp->law ) != length )
	{
		assert(0);
	}
		
#endif
	
	/* tone defination, refer to specs, p.21 */
	dsp->busyTone.duration 	= 1000;		/*1000 ms */
	dsp->busyTone.mark 		= 500;
	dsp->busyTone.data 		= dsp->data;
	dsp->busyTone.dsp		= dsp;
	dsp->busyTone.readPosition	= 0;

	dsp->ringBack.duration 	= 3000;		/* 3 s */
	dsp->ringBack.mark 		= 1000;		/* 1 s */
	dsp->ringBack.data 		= dsp->data;
	dsp->ringBack.dsp		= dsp;
	dsp->ringBack.readPosition	= 0;

	dsp->dialTone.duration 	= 1000;
	dsp->dialTone.mark 		= 1000;
	dsp->dialTone.data 		= dsp->data;
	dsp->dialTone.dsp			= dsp;
	dsp->dialTone.readPosition	= 0;

	/* Second Dial Tone */
	dsp->secondDT.duration 	= 250;			
	dsp->secondDT.mark 		= 140;			/* NTT specs, p.22 0.125~0.15 sec */
	dsp->secondDT.data 		= dsp->data;
	dsp->secondDT.dsp		= dsp;
	dsp->secondDT.readPosition	= 0;

	_as_dsp_nft_init(dsp, &(dsp->notifyTone) );

	_as_dsp_iit_init(dsp, &(dsp->iitTone) );

	for(i=0; i< 16; i++)
	{
		dsp->dtmf_chars[i].law = dsp->law;
		dsp->dtmf_chars[i].dsp = dsp;
		dsp->dtmf_chars[i].readPosition	= 0;

#if 0
		dsp->dtmf_chars[i].index = 5;
		dsp->dtmf_chars[i].duration = ms;
		dsp->dtmf_chars[i].mark = 
			(ms / (10 / dsp->dtmf_chars[i].index)) - ((10%dsp->dtmf_chars[i].index)*10);

		dsp->dtmf_chars[i].freq1 = dtmf_row[i/4];
		dsp->dtmf_chars[i].freq2 = dtmf_col[i%4];
#else
		dsp->dtmf_chars[i].index = 0;

		dsp->dtmf_chars[i].duration = dsp->root->config.dtmfDuration;//DTMF_TONE_DURATION_CHINA;
		dsp->dtmf_chars[i].mark = dsp->root->config.dtmfMarkTime;//DTMF_TONE_MARK_CHINA;

#if 0
		if( dsp->root->config.standard == PSTN_STANDARD_CHINA)
		{
			dsp->dtmf_chars[i].duration = DTMF_TONE_DURATION_CHINA;
			dsp->dtmf_chars[i].mark = DTMF_TONE_MARK_CHINA;
		}	
		else
		{
			dsp->dtmf_chars[i].duration = DTMF_TONE_DURATION_JAPAN;
			dsp->dtmf_chars[i].mark = DTMF_TONE_MARK_JAPAN;
		}
#endif

#endif

		dsp->dtmf_chars[i].data = (tone_data_t *)malloc(sizeof(tone_data_t));
		assert(dsp->dtmf_chars[i].data);
		dtmf_data = dsp->dtmf_chars[i].data;
		
		dtmf_data->length = dsp->length ;

		int ln_mark  = dsp->dtmf_chars[i].mark;
		int ln_spare = dsp->dtmf_chars[i].duration - ln_mark;
		dtmf_data->mark = (unsigned char *)malloc( sizeof(unsigned char)*ln_mark *8 );
		assert(dtmf_data->mark != 0);

#if 0
		if( as_dsp_tone_arbtones(dtmf_data->mark, dsp->dtmf_chars[i].freq1, dsp->dtmf_chars[i].freq2,
			                                ln_mark, dsp->law) != ln_mark*8)
		{
			assert(0);
		}
#else	
		memcpy(dtmf_data->mark, dtmf_raw_data[i],  ln_mark*8 );
#endif

		dtmf_data->spare = (unsigned char *)malloc( sizeof(unsigned char)*ln_spare*8  );
		assert(dtmf_data->spare != 0);
#if 0		
//		if( as_dsp_tone_arbtones(dtmf_data->spare, 0.0, 0.0, ln_spare, dsp->law) != ln_spare*8)
		if( as_dsp_tone_silence(dtmf_data->spare,  ln_spare, dsp->law) != ln_spare*8)
		{
			assert(0);
		}
#else
		if(__as_dsp_get_silence(dtmf_data->spare, ln_spare, dsp->law ) != ln_spare*8)
		{
			assert(0);
		}
#endif

	}
	
	//printf("as_dsp_tone_init over!\n");
	return 0;
}

static void _as_dsp_destroy_data(tone_data_t  *data)
{
	free(data->mark);
	data->mark = 0;
	free(data->spare);
	data->spare = 0;

	free(data);
//	data = 0;
}

tone_t *as_dsp_new_tones(as_dsp_t *root)
{
	tone_t *dsp;
	int res ;

	dsp = (tone_t *)malloc(sizeof(tone_t) );
	memset(dsp, 0, sizeof(tone_t) );

	dsp->root = root;
	res = as_dsp_init_tone(dsp);

	return dsp;
}

void as_dsp_destroy_tone(tone_t	*dsp)
{
	int i;

	_as_dsp_destroy_data(dsp->data);
	_as_dsp_destroy_data(dsp->notifyTone.data);
	_as_dsp_destroy_data(dsp->iitTone.tone.data);
	free(dsp->iitTone.beginPart);

	for(i=0; i< 16; i++)
	{
		_as_dsp_destroy_data(dsp->dtmf_chars[i].data);
	}

	free(dsp);
	dsp = 0;
}


int as_tone_read(audio_t *tone,  char *buffer)
{
#if 1
	int ln_mark = 0;
	int ln_spare = 0;
	int length = tone->dsp->length;	/* return length */
	ln_mark = length;
//	int ln_mark = length*(tone->index/10);

//	trace;
	if(tone->readPosition < tone->mark)
	{
		if( length + tone->readPosition > tone->mark )
		{
			ln_mark =  tone->mark - tone->readPosition;
		}
		memcpy(buffer, tone->data->mark + tone->readPosition*8, ln_mark *8);
	}
	else
		ln_mark = 0;

	tone->readPosition += ln_mark;
	if(tone->readPosition >= tone->duration )
		tone->readPosition = 0;

#if 0//AS_DSP_DEBUG
	printf("tone readposition =%d\n", tone->readPosition);
#endif

	if(ln_mark < length )
	{
		ln_spare = length - ln_mark;

		if( tone->readPosition +ln_spare > tone->duration )
		{
			ln_spare = tone->duration -tone->readPosition;
		}
	
		memcpy(buffer+ln_mark, tone->data->spare + (tone->readPosition - tone->mark)*8, ln_spare *8 );

		tone->readPosition += ln_spare; 
		if(tone->readPosition >= tone->duration )
			tone->readPosition = 0;
	}

	/* return length of buffer in unit of byte */
	return (ln_mark+ln_spare)*8;
#else 

trace;
	int length = tone->data->length*8;
	int lm = tone->mark*8;
	int ls = length - lm;
	
trace;
	memcpy(buffer, tone->data->mark, lm);
trace;
	memcpy(buffer+lm, tone->data->spare, ls);
trace;

	return length;
#endif
}

/* read tone data for DT, BT, RBT, SDT */
int as_tone_read_DT(audio_t *audio,  char *buffer)
{
	int length = audio->data->length*8;
	if(audio->readPosition < audio->mark )		
		memcpy(buffer, audio->data->mark, length );
	else
		memcpy( buffer, audio->data->spare, length );

	audio->readPosition = (audio->readPosition + audio->data->length)%audio->duration;

	return length;
}

