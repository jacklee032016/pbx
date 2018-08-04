/*
 * $Id: as_fsk_demodule_buf.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"



#define	FSK_CHAR_LENGTH			67		/* minium length of PCM data for a FSK char */

//		if(engine->stage < FSK_STAGE_PARSING_PARAM) 
#if 0
#define  NTT_REFRESH_PCM(engine) \
	do{ \
		if( (engine->ns - engine->bp) < FSK_CHAR_LENGTH ){ \
			engine->ns -= engine->bp; \
			return FSK_IO_ERROR; } \
	}while(0)
#else
#define  NTT_REFRESH_PCM(engine) \
	do{ ;\
		}while (0)
#endif
		
void __as_fsk_buf_reset_engine(fsk_t *dsp)
{
	fsk_decoder_t *engine = &(dsp->decoder);

	engine->fskd.spb=7;			/* 1200 baudios */
	engine->fskd.hdlc=0;			/* Modo asíncrono */
	engine->fskd.nbit = 8;			/* Ascii */
	engine->fskd.nstop= 1;			/* Bits de Stop */
	engine->fskd.paridad= 0;		/* Paridad ninguna */
	engine->fskd.bw=1;			/* Filtro 800 Hz */
	engine->fskd.f_mark_idx=2;	/* f_MARK = 1200 Hz */
	engine->fskd.f_space_idx=3;	/* f_SPACE = 2200 Hz */
	engine->fskd.pcola=0;			/* Colas vacías */
	engine->cont = 0; 				/* reset DPLL thing */

	engine->stage = FSK_STAGE_INITED;
	engine->parseType = NTT_FSK_PARSE_DISABLE;
	engine->msgLength = 0;
	engine->dataLength = 0;
	engine->position = 0;

	engine->param.called[0] = 0;
	engine->param.caller[0] = 0;
	engine->param.result = 0;

	engine->ns = 0;
	engine->bp = 0;
	
}

/* consume a PCM char */
static int __as_fsk_buf_demodulator(fsk_t *dsp, float *retval)
{
	float x,xS,xM;
	fsk_decoder_t *engine = &(dsp->decoder);

	if (engine->bp==engine->ns) 
	{
		engine->ns = 0;
		printf("ERROR : break demodulation\n");
		return (FSK_IO_ERROR);
	}

	x = (float)(XLAW(dsp->modem_buf[engine->bp++], dsp->law ) / 256);

	engine->fskd.cola_in[engine->fskd.pcola]=x;
	
	xS=filtroS(engine,x);
	xM=filtroM(engine,x);

	engine->fskd.cola_filtro[engine->fskd.pcola]=xM-xS;

	x=filtroL(engine, xM*xM - xS*xS);
	
	engine->fskd.cola_demod[engine->fskd.pcola++]=x;
	engine->fskd.pcola &= (NCOLA-1);

	*retval = x;
	return 0;
}

/* get a FSK bit */
static int __get_bit_raw_buf(fsk_t *dsp )
{
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
		if (__as_fsk_buf_demodulator(dsp, &x)) 
			return (FSK_IO_ERROR);
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
		engine->x0 = x;
		engine->cont += 1.;
		if (engine->cont > spb) 
		{
			engine->cont -= spb;
			break;
		}
	}

	/* 0x80 : B7 is 1 */
	f=(x>0)?0x80:0;
	return(f);
}

/* get a FSK char */
int as_fsk_buf_serie(fsk_t *dsp)
{
	int a;
	float x1,x2;
	int i,j,n1,r;
	fsk_decoder_t *engine = &(dsp->decoder);	
	
	do 
	{
		NTT_REFRESH_PCM(engine);
		if (__as_fsk_buf_demodulator(dsp, &x1)) 
			return (FSK_IO_ERROR);

		for(;;)
		{
			
		NTT_REFRESH_PCM(engine);
			if (__as_fsk_buf_demodulator(dsp, &x2)) 
				return (FSK_IO_ERROR);
			if (x2 < -0.5) 
				break; 
		}

		i=engine->fskd.spb/2;
		for(; i; i--) 
		{
			NTT_REFRESH_PCM(engine);
			
			if (__as_fsk_buf_demodulator(dsp, &x1)) 
				return (FSK_IO_ERROR);	
		}	

	}while (x1>0);

		
	j=engine->fskd.nbit;
	for (a=n1=0;j;j--) 
	{/* n DATA bits, from b0 to b7 */
		i = __get_bit_raw_buf(dsp );
		if (i == FSK_IO_ERROR) 
			return (FSK_IO_ERROR);
		if (i) 
			n1++; /* parity check*/
		a>>=1;
		a|=i;
	}
	
	j=8 - engine->fskd.nbit;
	a>>=j;

	if (engine->fskd.paridad) 
	{/* parity check : PARITY bits */
		i = __get_bit_raw_buf(dsp ); 
		if (i == FSK_IO_ERROR) 
			return (FSK_IO_ERROR);
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
	
	/*  bits STOP.  ser 1 */
	for (j=engine->fskd.nstop;j;j--) 
	{
		r = __get_bit_raw_buf(dsp );
		if (r == FSK_IO_ERROR) 
			return (FSK_IO_ERROR);
		if (!r) /* STOP bit must be 1. So if it is 0, invalidate it */
			a|=0x200;/* invalidate */
	}

	return a;
}


static int __as_fsk_buf_decode_gettdd( fsk_t *dsp)
{
	int	b,c;
	fsk_decoder_t *engine = &(dsp->decoder);

	for(;;)
	{
		/* get baud char : FSK char */
		b = as_fsk_buf_serie(dsp);
		if (b == FSK_IO_ERROR)
		{
			return FSK_IO_ERROR;
		}
		
		/* if null, do it again */
		if (!b) 
			continue;
		
		/* if 8 data bits, return directly */
		if (engine->fskd.nbit == 8) 
			return(b);
		
		/* if error, do it again */
		if (b > 0x7f)
			continue;

		c = __baudot(engine, b); /* translate to ascii */
		  /* if a valid char */
		if ((c > 0) && (c < 127)) 
			break;
	}
	
	return(c);
}

#define NTT_GET_TDD_BUF(_dsp, m)  \
	do { int tmp; \
	tmp = __as_fsk_buf_decode_gettdd((_dsp) ); \
	if( tmp== FSK_IO_ERROR ) return FSK_IO_ERROR; \
	else m = tmp&0x7F ;\
		}while(0)


int __as_fsk_buf_parse_param(fsk_t *dsp, unsigned char *paramBuf)
{
	fsk_decoder_t *engine = &(dsp->decoder);
	int c;
	
	for(; engine->position < engine->dataLength; engine->position++)
	{
		NTT_GET_TDD_BUF(dsp, c);
#if FSK_DEBUG		
		printf("(0x%x) " ,  c );
#endif
		paramBuf[engine->position] = c;
	}
#if FSK_DEBUG		
	printf("\n" );
#endif
	
	paramBuf[engine->position] = 0; /* terminate the string */
//	if(engine->position == engine->dataLength)
	{
		engine->msgLength -= engine->dataLength;
		engine->msgLength -= 2;	/* parameter type and length */
		engine->dataLength = 0;
		if(engine->msgLength <= 0)
		{
			engine->parseType = NTT_FSK_PARSE_END;
			engine->cont = 0; /* reset DPLL thing */
			engine->stage = FSK_STAGE_END;
			engine->msgLength = 0;
			return 0;	/* end of demodulation */
		}	
		else
		{
			engine->parseType = NTT_FSK_PARSE_HEADER;
			return 1;	/* continue */
		}	
	}

	return 0;
}

int __as_fsk_buf_demod(fsk_t *dsp)
{
	int	m = 0;
	fsk_decoder_t *engine = &(dsp->decoder);

	printf("total %d bytes PCM data used\n", engine->ns );
	for(;;)
	{

		switch(engine->stage )
		{
			case FSK_STAGE_FIRST_DEL:
			{
				NTT_GET_TDD_BUF(dsp, m);
				if(m==NTT_FSK_DLE)
				{
					engine->stage = FSK_STAGE_PARSING;
#if FSK_DEBUG		
					printf("FSK msg NTT_FSK_DLE(frame header)\n");
#endif
				}
				else 
					printf("Unknown FSK char 0x%x in stage of INITED\n", m);
				break;
			}
			case FSK_STAGE_PARSING:
			{
				NTT_GET_TDD_BUF(dsp, m);
#if 1
				if(m == NTT_FSK_SOH )
					printf("FSK msg NTT_FSK_SOH\n");
				else if(m == NTT_FSK_HEADER)
					printf("FSK msg NTT_FSK_HEADER\n");
				else if(m == NTT_FSK_DLE)
					printf("FSK msg NTT_FSK_DLE\n");
				else if(m == NTT_FSK_STX)
				{
					printf("FSK msg NTT_FSK_STX\n");
				}	
				else 
#endif					
					if(m == NTT_FSK_SERVICE_TYPE )
				{
					printf("FSK msg NTT_FSK_SERVICE_TYPE\n");
					engine->parseType = NTT_FSK_PARSE_HEADER;
					break;
				}
				else 
					printf("Unknown FSK char 0x%x in stage of parsing frame header\n", m);

				if(engine->parseType == NTT_FSK_PARSE_HEADER)
				{
					engine->stage = FSK_STAGE_PARSING_PARAM;
					engine->msgLength = m;
#if FSK_DEBUG		
					printf("Msg Content Length %d\n", engine->msgLength );
#endif
				}		
				break;
			}
			case FSK_STAGE_PARSING_PARAM:
			{
				char *msg = NULL;
				
				if(engine->parseType == NTT_FSK_PARSE_HEADER)
				{
					NTT_GET_TDD_BUF(dsp, m);
					if(m == NTT_FSK_PARAM_NUMBER )
					{
#if FSK_DEBUG		
						printf("FSK msg NTT_FSK_PARAM_NUMBER\n");
#endif
						engine->parseType = NTT_FSK_PARSE_NUMBER;
						engine->param.result |= AS_CALL_PARAM_CALLERID;
					}	
					else if(m == NTT_FSK_PARAM_CALLED_NUMBER)
					{
#if FSK_DEBUG		
						printf("FSK msg NTT_FSK_CALLED_NUMBER\n");
#endif
						engine->parseType = NTT_FSK_PARSE_CALLED;
						engine->param.result |= AS_CALL_PARAM_CALLED;
					}
					break;
				}

				if(engine->dataLength==0)
				{
					NTT_GET_TDD_BUF(dsp, engine->dataLength );
#if FSK_DEBUG		
					printf("Length %d\n", engine->dataLength );
#endif
					engine->position = 0;
					break;
				}
				
				if(engine->parseType == NTT_FSK_PARSE_CALLED)
				{
					msg = engine->param.called;
				}	
				else if(engine->parseType == NTT_FSK_PARSE_NUMBER)
				{
					msg = engine->param.caller;
				}	

				if (msg)
				{
					if(__as_fsk_buf_parse_param(dsp, msg) == FSK_IO_ERROR )
						return FSK_IO_ERROR;
					
				}	
				
				break;	
			}
			case FSK_STAGE_END:
			{
				engine->stage = FSK_STAGE_UNINIT;
				return 0;
			}
			
			default:
				break;
		}
		
	}

	return 0;
}

pstn_param_t *as_fsk_buf_decode_ntt_clid(fsk_t *dsp, unsigned char *rawbuf, int length )
{

#define AS_FSK_SILENCE_THRESHOLD 			1000 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */

	fsk_decoder_t *engine = &(dsp->decoder);
	int bufLength = length;
	int copyLength = 0;
	int position = 0;

	int x; 
	int accum = 0;
	int res;

	if (!engine)
	{
		errno= EINVAL; 
		return 0;
	}

	if(engine->stage == FSK_STAGE_UNINIT )
		__as_fsk_buf_reset_engine( dsp);

#if 0
	if(engine->stage < FSK_STAGE_PARSING )
	{/* silence detect */
		for (x=0; x< length; x++) 
			accum += abs( XLAW(rawbuf[x], dsp->law) );
		accum /= length;
		printf("AUDIO ACCUM = %d\n", accum);
		if(accum < AS_FSK_SILENCE_THRESHOLD )
			return 0;
	}

	while(1)//bufLength > 0 )
	{
		copyLength = engine->ns;// - engine->bp;
		if(copyLength && (engine->bp != 0) )
		{/* left bytes is copied */
			memcpy(engine->audio_buf, engine->audio_buf + engine->bp, copyLength);
			engine->ns = copyLength;
			engine->bp = 0;
#if FSK_DEBUG		
			printf("copy %d left bytes\n", copyLength);
#endif
		}
		
		copyLength = AS_DTMF_BLOCKSIZE - engine->ns;
		copyLength = (copyLength >bufLength )? bufLength : copyLength;
		memcpy(engine->audio_buf + engine->ns, rawbuf + (length-bufLength), copyLength );
#if FSK_DEBUG		
		printf("add %d bytes\n", copyLength);
#endif
		bufLength -= copyLength;
		engine->ns += copyLength;

		if(engine->ns >= AS_DTMF_BLOCKSIZE )
		{
			engine->ns = AS_DTMF_BLOCKSIZE;
			engine->bp = 0;
			if (__as_fsk_buf_demod(dsp) == 0) 
				break;/* demodulation ended*/ 
		}
		else
		{
			return 0;
		}
		/* go on */
	}
#else
/* silence detect */

#define  FSK_MODEM_MINIUM_LENGTH		1000

		if(length >0 )
		{
			for (x=0; x< length; x++) 
				accum += abs( XLAW(rawbuf[x], dsp->law) );
			accum /= length;
		}	
		
		printf("AUDIO ACCUM = %d\n", accum);
		if(accum < AS_FSK_SILENCE_THRESHOLD )
		{
			if(engine->stage == FSK_STAGE_FIRST_DEL && engine->ns > FSK_MODEM_MINIUM_LENGTH)
			{
				res =__as_fsk_buf_demod(dsp);
				//if(res== 0) 
					return &(engine->param);
			}
			return 0;
		}
		else
		{
			if(engine->stage < FSK_STAGE_FIRST_DEL)
			{
				engine->stage = FSK_STAGE_FIRST_DEL;
			}
			memcpy( dsp->modem_buf + engine->ns, rawbuf , length );
			engine->ns += length;
			if(engine->ns >= FSK_MODEM_BUFFER_LENGTH || length < 160 )
			{
				res = __as_fsk_buf_demod(dsp);
				//if(res== 0 ) 
					return &(engine->param);
			}

			return 0;
		}	

#endif

	return 0;
}

