/*
 * $Log: as_fsk.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.7  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.6  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.5  2006/05/19 08:46:59  lizhijie
 * add fsk demodulation in buffer
 *
 * Revision 1.4  2006/04/19 12:28:39  lizhijie
 * add CAR support and others
 *
 * Revision 1.3  2006/04/19 12:24:26  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.1  2006/03/20 06:36:48  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.1  2006/03/16 08:37:58  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_fsk.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"


/* send silence 
 * len :  length in ms 
 */
/* 
int as_dsp_silence(fsk_t *engine, int fd ,int len )
{
	int	i,r;
	int flag = 0;
	
	for(i = 0; i < (len * 8); i++) 
		if ((r = as_dsp_put_audio_sample(engine, fd, 0, flag))) 
			return(r);
	return 0;
}
*/

/* return 0, success, others, failed , law , 0 :a-law, 1 : mu-law */
static int __as_fsk_init_config(fsk_t  *dsp )
{
	fsk_config_t *config;

	config = (fsk_config_t *)malloc(sizeof(fsk_config_t));
	memset(config, 0, sizeof( fsk_config_t) );

	config->buf_size = AS_DRIVER_BUFFER_LENGTH;//info.bufsize;
//	config->tone_data.buf = (unsigned short *)malloc(config->buf_size*sizeof(unsigned short) );
	config->tone_data.buf = (unsigned char *)malloc(config->buf_size );
	assert(config->tone_data.buf);

//	config->voice_data.buf = (unsigned short *)malloc(config->buf_size*sizeof(unsigned short));
	config->voice_data.buf = (unsigned char *)malloc(config->buf_size);
	assert(config->voice_data.buf);

	dsp->config = config;
	
	return 0;
}

static void __as_fsk_destroy_config( fsk_t *dsp)
{

	if(dsp->config->tone_data.buf)
	{
		free(dsp->config->tone_data.buf);
		dsp->config->tone_data.buf = NULL;
	}
	if(dsp->config->voice_data.buf)
	{
		free(dsp->config->voice_data.buf);
		dsp->config->voice_data.buf = NULL;
	}

	free( dsp->config);
}

/* law : A_LAW_CODE(0) or U_LAW_CODE(1) */
fsk_t *as_dsp_new_fsk( as_dsp_t *root )
{
#if 0
	float	f0,f1;
/* Global variables set up for the fsk decoding / encoding */
	int spb,spb1;  /* these dont change, so they are okay as globals */
#endif

#if __ARM__
#else
	float fsk_space_f,fsk_mark_f;
#endif
	fsk_t *dsp;
	int res;
#if 0	
	f0=1800.0;   /* space tone for TDD generation */
	f1=1400.0;   /* mark tone for TDD generation */
	spb = 176;
	spb1 = 264;
#endif	

	dsp = (fsk_t *)malloc(sizeof( fsk_t) );
	assert( dsp);
	memset(dsp, 0, sizeof( fsk_t) );
	dsp->root = root;

	res = __as_fsk_init_config(dsp );
	if(res)
	{
		free(dsp);
		return NULL;
	}

	dsp->law = dsp->root->config.law;

#if __ARM__
	/* following float point numbers are caculation result of 2200Hz and 1200HZ
	* these paremeters are calculated in PC, Li ZHijie
	*/
	dsp->encoder.dr[0]= FREQ_SPARE_REAL;
	dsp->encoder.di[0]=  FREQ_SPARE_IMAGE;
	dsp->encoder.dr[1]= FREQ_MARK_REAL;
	dsp->encoder.di[1]= FREQ_MARK_IMAGE;
#else
	fsk_space_f= 2100.0;   /* space tone for CLID generation */
	fsk_mark_f= 1300.0;   /* mark tone for CLID generation */
	dsp->encoder.dr[0]= cos(fsk_space_f/8000.0*2.0*M_PI);
	dsp->encoder.di[0]= sin( fsk_space_f/8000.0*2.0*M_PI);
	dsp->encoder.dr[1]= cos(fsk_mark_f/8000.0*2.0*M_PI);
	dsp->encoder.di[1]= sin(fsk_mark_f/8000.0*2.0*M_PI);
	printf("'%20e'  '%20e'  '%20e'  '%20e'\n", dsp->encoder.dr[0], dsp->encoder.di[0], dsp->encoder.dr[1], dsp->encoder.di[1]);
#endif


	dsp->cal.ci = 1.0;
	dsp->cal.cr = 0.0;

	dsp->decoder.bp = 0;				/* fsk rx buffer pointer */
	dsp->decoder.ns = 0;				/* silly fsk read buffer size */
	dsp->decoder.x0 = 0.0;				/* stuff for dpll */
	dsp->decoder.cont = 0.0;			/* stuff for dpll */
	dsp->decoder.scont = 0.0;			/* stuff for clid sending */

	dsp->decoder.fskd.spb=176;			/* 45.5 baudios */
	dsp->decoder.fskd.hdlc=0;			/* Modo asíncrono */
	dsp->decoder.fskd.nbit= 5;			/* Baudot */
	dsp->decoder.fskd.nstop= 1.5;		/* Bits de Stop */
	dsp->decoder.fskd.paridad= 0;		/* Paridad ninguna */
	dsp->decoder.fskd.bw= 0;			/* Filtro 75 Hz */
	dsp->decoder.fskd.f_mark_idx= 0;	/* f_MARK = 1400 Hz */
	dsp->decoder.fskd.f_space_idx= 1;	/* f_SPACE = 1800 Hz */
	dsp->decoder.fskd.pcola=0;			/* Colas vacías */

	dsp->decoder.stage = FSK_STAGE_UNINIT;

	as_fsk_crc_init();
	
	return dsp;
}


void as_dsp_destroy_fsk(fsk_t *dsp)
{
	__as_fsk_destroy_config(dsp);
//	memset(dsp, 0 ,sizeof(fsk_t) );
	free(dsp);
	dsp = 0;
	return;
}

void __as_fsk_config_buffer(fsk_t *dsp, int fd)
{
	int res;
	struct as_bufferinfo info;

	res = ioctl( fd, AS_CTL_GET_BUFINFO, &info);
	if(res<0)
	{
//		printf("error in get bufinfo, this file descriptor is not a device interface : %s\r\n", strerror(errno) );
		dsp->config->buf_type = FSK_BUFFER_NORMAL_FILE;
//		return res;
	}
	else
		dsp->config->buf_type = FSK_BUFFER_PSTN;

}

int as_fsk_decode(fsk_t *dsp, int fd, char *number, char *name )
{
	int res;

	__as_fsk_config_buffer( dsp, fd);
	res = as_fsk_decode_clid_from_fd( dsp, fd, number,  name);

	return res;
}

int as_fsk_ntt_decode(fsk_t *dsp, int fd, char *number)
{
	int res;

	__as_fsk_config_buffer( dsp, fd);
	res = as_fsk_decode_ntt_clid_from_fd( dsp, fd, number);

	return res;
}

#if 0
pstn_param_t *as_fsk_ntt_decode_buf(fsk_t *dsp, unsigned char *rawbuf, int length)
{
//	__as_fsk_config_buffer( dsp, 0);
	return as_fsk_buf_decode_ntt_clid( dsp, rawbuf, length);
}
#endif

int as_fsk_ring_with_callerid(fsk_t *dsp, int fd , char *number , char *name )
{
	int res;

	__as_fsk_config_buffer( dsp, fd);
	res = as_dsp_lib_ring_with_fsk_callerid(dsp, fd , number , name );

	return res;
}

int as_fsk_ntt_ring_with_callerid(fsk_t *dsp, int fd , char *number  )
{
	int res;

	__as_fsk_config_buffer( dsp, fd);
	res = as_dsp_lib_ring_with_ntt_callerid(dsp, fd , number);

	return res;
}


