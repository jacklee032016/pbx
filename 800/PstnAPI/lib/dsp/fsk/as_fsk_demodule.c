/*
 * $Log: as_fsk_demodule.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.3  2006/05/19 08:46:59  lizhijie
 * add fsk demodulation in buffer
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
 * $Id: as_fsk_demodule.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"

static int __as_fsk_get_audio_sample_from_fd(fsk_t  *dsp, int fd )
{
	int len;
	fsk_tone_data_t  *data = &(dsp->config->voice_data);
	
	//	printf("count =%d bufp =%d\r\n" ,count, data->bufp);
	if (data->bufp == dsp->config->buf_size/*AS_DTMF_BLOCKSIZE*/) 
	{
		data->bufp=0;
//		len = read(fd, data->buf, dsp->config->buf_size*2);
//		if ( len != dsp->config->buf_size*2)
		len = read(fd, data->buf, dsp->config->buf_size);
		if ( len != dsp->config->buf_size)
		{
			printf("READ length is error, should be %d bytes, but only %d bytes\r\n", dsp->config->buf_size, len);
			if (errno == AS_ELAST && dsp->config->buf_type == FSK_BUFFER_PSTN) 
				return FSK_IO_AGAIN;
			return FSK_IO_ERROR;
		}
	}
	else
	{
		printf("ALERT : Voice data from device is not used up, left %d bytes now\r\n", 
			dsp->config->buf_size - data->bufp);
		return -1;
	}

	return(0);
}

int __get_audio_sample_from_config(fsk_t *dsp, int fd)
{
//	int position;
	int res;
	int len = 0, left = 0;
	fsk_decoder_t *engine = &(dsp->decoder);
	fsk_tone_data_t  *data = &(dsp->config->voice_data);

	if(dsp->config->buf_type == FSK_BUFFER_PSTN)
	{
		if(data->bufp < dsp->config->buf_size)
		{
			len = dsp->config->buf_size -data->bufp;
			if(len>AS_DTMF_BLOCKSIZE)
				len = AS_DTMF_BLOCKSIZE;
//			memcpy(engine->audio_buf, data->buf + data->bufp, len*2);
			memcpy(engine->audio_buf, data->buf + data->bufp, len);
			data->bufp += len;
		}

		if(len < AS_DTMF_BLOCKSIZE)
		{
			left = AS_DTMF_BLOCKSIZE - len;
			res = __as_fsk_get_audio_sample_from_fd(dsp, fd);
			if(res)
				return len;
			
//			memcpy(engine->audio_buf +len, data->buf, left*2);
			memcpy(engine->audio_buf +len, data->buf, left );
			data->bufp += left;
		}
		return AS_DTMF_BLOCKSIZE;
	}

//	len = read(fd, engine->audio_buf, AS_DTMF_BLOCKSIZE*2);
	len = read(fd, engine->audio_buf, AS_DTMF_BLOCKSIZE );
		printf("read %d from file\r\n" ,len );
	return len;

}


static int __as_fsk_get_audio_sample(fsk_t *dsp, int fd, float *retval)
{
	fsk_decoder_t *engine = &(dsp->decoder);

	if (engine->bp==engine->ns) 
	{
		engine->ns=__get_audio_sample_from_config( dsp,  fd);
		engine->bp=0;
		if (engine->ns != AS_DTMF_BLOCKSIZE)
		{
			engine->ns = 0;
			return(-1);
		}
	}

	*retval = (float)(XLAW(engine->audio_buf[engine->bp++], dsp->law ) / 256);
//	*retval = ((float)(as_dsp_law_ulaw2linear(engine->audio_buf[engine->bp++] ) ) )/ 256.0;
//	*retval = (float)( engine->audio_buf[engine->bp++] ) ;
	return(0);
}

static int __as_fsk_demodulator(fsk_t *dsp, int fd, float *retval)
{
	float x,xS,xM;
	fsk_decoder_t *engine = &(dsp->decoder);

	if (__as_fsk_get_audio_sample(dsp, fd, &x)) 
		return(-1);
	engine->fskd.cola_in[engine->fskd.pcola]=x;
	
	xS=filtroS(engine,x);
	xM=filtroM(engine,x);

	engine->fskd.cola_filtro[engine->fskd.pcola]=xM-xS;

	x=filtroL(engine, xM*xM - xS*xS);
	
	engine->fskd.cola_demod[engine->fskd.pcola++]=x;
	engine->fskd.pcola &= (NCOLA-1);

	*retval = x;
	return(0);
}


static int __get_bit_raw(fsk_t *dsp , int fd)
{
	/* Esta funcion implementa un DPLL para sincronizarse con los bits */
	float x,spb,spb2,ds;
	int f;
	fsk_decoder_t *engine = &(dsp->decoder);
	
	spb=engine->fskd.spb; 
	if (engine->fskd.spb == 7) 
		spb = 8000.0 / 1200.0;
	ds=spb/32.;
	spb2=spb/2.;

	for (f=0;;)
	{
		if (__as_fsk_demodulator(dsp, fd, &x)) 
			return(-1);
		if ((x*engine->x0)<0) 
		{	/* Transicion */
			if (!f) 
			{
				if (engine->cont<(spb2)) 
					engine->cont+=ds; 
				else 
					engine->cont-=ds;
				f=1;
			}
		}
		engine->x0=x;
		engine->cont+=1.;
		if (engine->cont>spb) 
		{
			engine->cont-=spb;
			break;
		}
	}
	
	f=(x>0)?0x80:0;
	return(f);
}

int as_fsk_serie(fsk_t *dsp, int fd)
{
	int a;
	float x1,x2;
	int i,j,n1,r;
	fsk_decoder_t *engine = &(dsp->decoder);	
	
	/* Esperamos bit de start	*/
	do 
	{
/* this was jesus's nice, reasonable, working (at least with RTTY) code
to look for the beginning of the start bit. Unfortunately, since TTY/TDD's
just start sending a start bit with nothing preceding it at the beginning
of a transmission (what a LOSING design), we cant do it this elegantly */
/*
		if (demodulador(zap,&x1)) return(-1);
		for(;;) {
			if (demodulador(zap,&x2)) return(-1);
			if (x1>0 && x2<0) break;
			x1=x2;
		}
*/
/* this is now the imprecise, losing, but functional code to detect the
beginning of a start bit in the TDD sceanario. It just looks for sufficient
level to maybe, perhaps, guess, maybe that its maybe the beginning of
a start bit, perhaps. This whole thing stinks! */

		if (__as_fsk_demodulator(dsp, fd, &x1)) 
			return(-1);
		for(;;)
		{
			if (__as_fsk_demodulator(dsp, fd, &x2)) 
				return(-1);
			if (x2 < -0.5) 
				break; 
		}
		/* Esperamos 0.5 bits antes de usar DPLL */
		i=engine->fskd.spb/2;
		for(;i;i--) 
			if (__as_fsk_demodulator(dsp, fd, &x1)) 
				return(-1);	

		/* x1 debe ser negativo (confirmación del bit de start) */

	}while (x1>0);
	
	/* Leemos ahora los bits de datos */
	j=engine->fskd.nbit;
	for (a=n1=0;j;j--) 
	{
		i = __get_bit_raw(dsp, fd);
		if (i == -1) 
			return(-1);
		if (i) 
			n1++;
		a>>=1; a|=i;
	}
	
	j=8 - engine->fskd.nbit;
	a>>=j;

	/* Leemos bit de paridad (si existe) y la comprobamos */
	if (engine->fskd.paridad) 
	{
		i = __get_bit_raw(dsp, fd); 
		if (i == -1) 
			return(-1);
		if (i) 
			n1++;
		if (engine->fskd.paridad==1) 
		{	/* paridad=1 (par) */
			if (n1&1) 
				a|=0x100;		/* error */
		} 
		else 
		{			/* paridad=2 (impar) */
			if (!(n1&1)) 
				a|=0x100;	/* error */
		}
	}
	
	/* Leemos bits de STOP. Todos deben ser 1 */
	
	for (j=engine->fskd.nstop;j;j--) 
	{
		r = __get_bit_raw(dsp, fd);
		if (r == -1) 
			return(-1);
		if (!r) 
			a|=0x200;
	}

	/* Por fin retornamos  */
	/* Bit 8 : Error de paridad */
	/* Bit 9 : Error de Framming */
	
	return a;
}

