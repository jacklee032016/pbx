/*
 * $Id: assist_dsp.c,v 1.2 2007/09/08 18:21:40 lizhijie Exp $
*/

#include "assist_globals.h"
#include "assist_dsp.h"

int assist_dsp_channel_gain_setup(assist_dsp_t *dsp, int fd);
/* sample_length is measured by 'unsiged short' */
#if 0
assist_dtmf_detector_t  *assist_dsp_dtmf_detect_init( int law, int sample_length);
void assist_dsp_dtmf_detect_detsory(assist_dtmf_detector_t  *dsp);

int assist_dsp_dtmf_detect_run(assist_dtmf_detector_t  *dsp, char *buf, int length, char *callerId, int max);
int assist_dsp_dtmf_detect_step(assist_dtmf_detector_t  *dsp, char *buf, int length);
int assist_dsp_dtmf_detect_get(assist_dtmf_detector_t *dsp, char *callerId, int max);
	

int assist_dtmf_get(assist_dsp_t *dsp, char *callerId, int max)
{
	return assist_dsp_dtmf_detect_get( dsp->dtmf,  callerId,  max);
}

int assist_dtmf_step_decode(assist_dsp_t  *dsp, char *buf, int length)
{
	return assist_dsp_dtmf_detect_step(dsp->dtmf, buf, length);
}

int assist_dtmf_decode(assist_dsp_t  *dsp, char *buf, int length, char *callerId, int max)
{
	return assist_dsp_dtmf_detect_run( dsp->dtmf,  buf, length, callerId, max);
}
#endif

assist_dsp_ops_t		dsp_ops = 
{
#if 0
	dtmf_decode		:	assist_dtmf_decode,
	dtmf_decode_step	:	assist_dtmf_step_decode,
	dtmf_decode_get	:	assist_dtmf_get,
#endif	
	gain_setup		:	assist_dsp_channel_gain_setup	
};


/* law : A_LAW_CODE(0) or U_LAW_CODE(1) */
assist_dsp_t *assist_dsp_init(as_law_type_t law ,int rxGain, int txGain, int dtmf_decode_buf_length )
{
#if 0
	float fsk_space_f,fsk_mark_f;
	int res;
#endif
	assist_dsp_t *dsp;
	dsp = (assist_dsp_t *)malloc(sizeof(assist_dsp_t) );
	if(!dsp)
	{
		printf("No critical memory %s", __FUNCTION__);
		exit(1);
	}
	memset(dsp, 0, sizeof(assist_dsp_t) );

	dsp->law = law;
	dsp->rxGain = rxGain;
	dsp->txGain = txGain;	
	dsp->ops = &dsp_ops;
	
#if 0
	dsp->dtmf = assist_dsp_dtmf_detect_init( law, dtmf_decode_buf_length);
	assert(dsp->dtmf!=0);
	res = as_dsp_init_config(&(dsp->config), fd, law);
	if(res)
	{
		free(dsp);
		return NULL;
	}
		
	fsk_space_f=2200.0;   /* space tone for CLID generation */
	fsk_mark_f=1200.0;   /* mark tone for CLID generation */
	dsp->fsk.dr[0]=cos(fsk_space_f*2.0*M_PI/8000.0);
	dsp->fsk.di[0]=sin( fsk_space_f*2.0*M_PI/8000.0);
	dsp->fsk.dr[1]=cos(fsk_mark_f*2.0*M_PI/8000.0);
	dsp->fsk.di[1]=sin(fsk_mark_f*2.0*M_PI/8000.0);

	dsp->cal.ci = 1.0;
	dsp->cal.cr = 0.0;
	dsp->cal.ci1 = 1.0;
	dsp->cal.cr1 = 0.0;

	dsp->fsk_decoder.bp = 0;		/* fsk rx buffer pointer */
	dsp->fsk_decoder.ns = 0;		/* silly fsk read buffer size */
	dsp->fsk_decoder.x0 = 0.0;		/* stuff for dpll */
	dsp->fsk_decoder.cont = 0.0;	/* stuff for dpll */
	dsp->fsk_decoder.scont = 0.0;	/* stuff for clid sending */

//	dsp->fsk_decoder.fskd
	dsp->fsk_decoder.fskd.spb=176;	/* 45.5 baudios */
	dsp->fsk_decoder.fskd.hdlc=0;		/* Modo asíncrono */
	dsp->fsk_decoder.fskd.nbit=5;		/* Baudot */
	dsp->fsk_decoder.fskd.nstop=1.5;	/* Bits de Stop */
	dsp->fsk_decoder.fskd.paridad=0;	/* Paridad ninguna */
	dsp->fsk_decoder.fskd.bw=0;		/* Filtro 75 Hz */
	dsp->fsk_decoder.fskd.f_mark_idx=0;	/* f_MARK = 1400 Hz */
	dsp->fsk_decoder.fskd.f_space_idx=1;	/* f_SPACE = 1800 Hz */
	dsp->fsk_decoder.fskd.pcola=0;		/* Colas vacías */
#endif
	return dsp;
}


void assist_dsp_destory(assist_dsp_t *dsp)
{
#if 0
	if(dsp->config.tone_data.buf)
	{
		free(dsp->config.tone_data.buf);
		dsp->config.tone_data.buf = NULL;
	}
	if(dsp->config.voice_data.buf)
	{
		free(dsp->config.voice_data.buf);
		dsp->config.voice_data.buf = NULL;
	}
	assist_dsp_dtmf_detect_detsory( dsp->dtmf);
#endif
	free(dsp);
	dsp = NULL;
}

