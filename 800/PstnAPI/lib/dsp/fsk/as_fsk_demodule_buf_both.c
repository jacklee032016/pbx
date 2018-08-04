/*
 * $Id: as_fsk_demodule_buf_both.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"

#define	FSK_CHAR_LENGTH			67		/* minium length of PCM data for a FSK char */

int  __as_fsk_buf2_reset_engine(fsk_t *dsp, unsigned char *rawdata, int length)
{
	fsk_decoder_t *engine = &(dsp->decoder);

	if (!engine)
	{
		errno= EINVAL; 
		return FSK_IO_ERROR;
	}
	
	dsp->inputBuf = rawdata;
	dsp->modemLength = length;
	
	engine->fskd.spb=7;			/* 1200 baudios */
	engine->fskd.hdlc=0;			/* Modo asíncrono */
	engine->fskd.nbit = 8;			/* Ascii */
	engine->fskd.nstop= 0;			/* Bits de Stop */
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

	return 0;
}

/* consume a PCM char */
static int __as_fsk_buf2_demodulator(fsk_t *dsp, float *retval)
{
	float x,xS,xM;
	fsk_decoder_t *engine = &(dsp->decoder);

	if (engine->bp== dsp->modemLength ) 
	{
		engine->ns = 0;
		printf("ERROR : data used up when demodulation\n");
		return (FSK_IO_ERROR);
	}

	x = (float)(XLAW(dsp->inputBuf[engine->bp++], dsp->law ) / 256);

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
static int __get_bit_raw_buf2(fsk_t *dsp )
{
	float x,spb,spb2,ds;
	int f;
	fsk_decoder_t *engine = &(dsp->decoder);
	int res = 0;
	
	spb=engine->fskd.spb; 
	if (engine->fskd.spb == 7) 
		spb = 8000.0 / 1200.0;
	ds=spb/32.;
	spb2=spb/2.;

	for (f=0;;)
	{
		if ( (res= __as_fsk_buf2_demodulator(dsp, &x) ) != 0) 
			return res;
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
	return (f);
}

/* get a FSK char */
int as_fsk_buf2_serie(fsk_t *dsp)
{
	int a;
	float x1,x2;
	int i,j,n1,r;
	fsk_decoder_t *engine = &(dsp->decoder);	
	
	do 
	{
#if 0	
		if (__as_fsk_buf2_demodulator(dsp,&x1)) 
			return(-1);
		for(;;)
		{
			if (__as_fsk_buf2_demodulator(dsp,&x2)) 
				return(-1);
			if (x1>0 && x2<0) 
				break;
			x1=x2;
		}
#else	
		if (__as_fsk_buf2_demodulator(dsp, &x1)) 
			return (FSK_IO_ERROR);

		for(;;)
		{
			if (__as_fsk_buf2_demodulator(dsp, &x2)) 
				return (FSK_IO_ERROR);
			if (x2 < -0.5) /* for difference gain in line device */
				break; 
		}

		i=engine->fskd.spb/2;
		for(; i; i--) 
		{
			if (__as_fsk_buf2_demodulator(dsp, &x1)) 
				return (FSK_IO_ERROR);	
		}	
#endif

	}while (x1>0);

		
	j=engine->fskd.nbit;
	for (a=n1=0;j ; j--) 
	{/* n DATA bits, from b0 to b7 */
		i = __get_bit_raw_buf2(dsp );
		if (i == FSK_IO_ERROR) 
			return (FSK_IO_ERROR);
#if 0		
		if (i) 
			n1++; /* parity check*/
#endif		
		a>>=1;
		a|=i;
	}
	
#if 0
	j=8 - engine->fskd.nbit;
	a>>=j;

	if (engine->fskd.paridad) 
	{/* parity check : PARITY bits */
		i = __get_bit_raw_buf2(dsp ); 
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
#endif

	/*  bits STOP.  ser 1 */
	for (j=engine->fskd.nstop;j;j--) 
	{
		r = __get_bit_raw_buf2(dsp );
		if (r == FSK_IO_ERROR) 
			return (FSK_IO_ERROR);
		if (!r) /* STOP bit must be 1. So if it is 0, invalidate it */
			a|=0x200;/* invalidate */
	}

	return a;
}


static int __as_fsk_buf2_decode_gettdd( fsk_t *dsp)
{
	int	b,c;
#if 0
	fsk_decoder_t *engine = &(dsp->decoder);
#endif

	for(;;)
	{
		/* get baud char : FSK char */
		b = as_fsk_buf2_serie(dsp);
		if (b == FSK_IO_ERROR)
		{
			return FSK_IO_ERROR;
		}
		
		/* if null, do it again */
		if (!b) 
			continue;
		return b;
#if 0		
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
#endif

	}
	
	return(c);
}

pstn_param_t *as_fsk_decode_chinese_clid( fsk_t *dsp, unsigned char *rawbuf, int length )
{
	int	m, i;
	unsigned char *msg = 0;
	fsk_decoder_t *engine = &(dsp->decoder);
	int res;

	res = __as_fsk_buf2_reset_engine( dsp, rawbuf, length);
	if(res == FSK_IO_ERROR)
		return 0;

	for(;;)
	{
		m = __as_fsk_buf2_decode_gettdd(dsp);
		if(m== FSK_IO_ERROR)
			break;
#if FSK_DEBUG		
		printf("FSK char 0x%x\n", m);
#endif

		switch(engine->stage )
		{
			case FSK_STAGE_INITED:	/* parse msg format and msg length */
			{
		
				if(m == FSK_FORMAT_MDMF )
				{
#if FSK_DEBUG		
					printf("FSK msg MDMF\n");
#endif
					engine->stage = FSK_STAGE_PARSING_PARAM;
					engine->parseType = NTT_FSK_PARSE_HEADER;
				}	
				else if(m == FSK_FORMAT_SDMF)
				{
#if FSK_DEBUG		
					printf("FSK msg SDMF\n");
#endif
					engine->stage = FSK_STAGE_PARSING;
				}	
#if FSK_DEBUG		
				else
					printf("Unknown FSK format '0x%x'\n", m);
#endif
				if( engine->stage != FSK_STAGE_INITED )
				{
					engine->msgLength = __as_fsk_buf2_decode_gettdd(dsp ); /* get message size */
#if FSK_DEBUG		
					printf("FSK msg length %d\n" , engine->msgLength);
#endif
					if (engine->msgLength < 0)  /* if error, return as such */
					{
						res= -1;
						goto result;
					}
				}
				
				if (m == 'U')  /* if a flag byte, set the flag */
				{
					i = 1; /* got flag */
					continue;
				}
				break;
			}
			case FSK_STAGE_PARSING:	 /* parse SDMF format */
			{
#if FSK_DEBUG		
				printf("(0x%x) " ,  m);
#endif
				engine->param.caller[engine->dataLength ++] = m;
				if(engine->dataLength >= engine->msgLength)
				{	
					engine->param.caller[engine->dataLength] = 0;	/* terminate it */
					res = 0;
					goto result;
				}
				break;
			}	
			case FSK_STAGE_PARSING_PARAM:	 /* parse MDMF format */
			{
				switch(engine->parseType)
				{
					case NTT_FSK_PARSE_HEADER:
					{
						if(m== FSK_MSG_TYPE_DATE)
						{/* Date */
							engine->parseType = NTT_FSK_PARSE_DATE;
							engine->param.result |= AS_CALL_PARAM_DATE;
							msg = engine->param.date;
#if FSK_DEBUG		
							printf("decode MDMF DATE : ");
#endif
						}	
						else if(m == FSK_MSG_TYPE_NUMBER ||m== FSK_MSG_TYPE_NUMBER_NO )
						{/* caller ID*/
							engine->parseType = NTT_FSK_PARSE_NUMBER ;
							engine->param.result |= AS_CALL_PARAM_CALLERID;
							msg = engine->param.caller;
#if FSK_DEBUG		
							printf("decode MDMF CallerID : ");
#endif
						}
						else if(m== FSK_MSG_TYPE_NAME || m== FSK_MSG_TYPE_NAME_NO )
						{/* user name  */
							engine->parseType = NTT_FSK_PARSE_CALLED;
							engine->param.result |= AS_CALL_PARAM_CALLED;
							msg = engine->param.called;
#if FSK_DEBUG		
							printf("decode MDMF Called Phone Number : ");
#endif
						}	
						engine->dataLength =  __as_fsk_buf2_decode_gettdd(dsp );
						engine->position = 0;
#if FSK_DEBUG		
						printf(" data length : %d\n" ,engine->dataLength );
#endif
						break;
					}
					case NTT_FSK_PARSE_DATE:
					case NTT_FSK_PARSE_CALLED:
					case NTT_FSK_PARSE_NUMBER:
					{
						if(!msg)
							printf("ERROR : No buffer for Chinese FSK parameter decode\n");
						msg[engine->position++] = m;
						if( engine->position == engine->dataLength )
						{
#if FSK_DEBUG		
							printf("param ended\n");
#endif
							msg[engine->position ] = 0;
							engine->msgLength = engine->msgLength - engine->dataLength - 2;
							engine->position = 0;
							engine->dataLength = 0;
							engine->parseType = NTT_FSK_PARSE_HEADER;
							if( engine->msgLength <= 0)
							{
								res = 0;
#if FSK_DEBUG		
								printf("msg ended\n");
#endif
								goto result;
							}
						}
						break;
					}
					default:
						break;
				}
				break;
			}
			default:
				break;
		}		
	}
	
result:
	engine->stage = FSK_STAGE_UNINIT;
	
	if ( res == 0)
		return &(engine->param);

	return 0;
}


#define NTT_GET_TDD_BUF2(_dsp, m)  \
	do { int tmp; \
	tmp = __as_fsk_buf2_decode_gettdd((_dsp) ); \
	if( tmp== FSK_IO_ERROR ) return FSK_IO_ERROR; \
	else m = tmp&0x7F ;\
		}while(0)


int __as_fsk_buf2_parse_param(fsk_t *dsp, unsigned char *paramBuf)
{
	fsk_decoder_t *engine = &(dsp->decoder);
	int c;
	
	for(; engine->position < engine->dataLength; engine->position++)
	{
		NTT_GET_TDD_BUF2(dsp, c);
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

int __as_fsk_buf2_demod_ntt(fsk_t *dsp)
{
	int	m = 0;
	fsk_decoder_t *engine = &(dsp->decoder);

#if FSK_DEBUG		
	printf("total %d bytes PCM data used\n", dsp->modemLength );
#endif
	for(;;)
	{

		switch(engine->stage )
		{
			case FSK_STAGE_FIRST_DEL:
			{
				NTT_GET_TDD_BUF2(dsp, m);
				if(m==NTT_FSK_DLE)
				{
					engine->stage = FSK_STAGE_PARSING;
#if FSK_DEBUG		
					printf("FSK msg NTT_FSK_DLE(frame header)\n");
#endif
				}
				else 
					printf("Unknown FSK char 0x%x(%c) in stage of INITED\n", m, m);
				break;
			}
			case FSK_STAGE_PARSING:
			{
				NTT_GET_TDD_BUF2(dsp, m);
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
					NTT_GET_TDD_BUF2(dsp, m);
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
					NTT_GET_TDD_BUF2(dsp, engine->dataLength );
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
					if(__as_fsk_buf2_parse_param(dsp, msg) == FSK_IO_ERROR )
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

pstn_param_t *as_fsk_decode_ntt_clid(fsk_t *dsp, unsigned char *rawbuf, int length )
{
	fsk_decoder_t *engine = &(dsp->decoder);
	int res;

	res = __as_fsk_buf2_reset_engine( dsp, rawbuf, length);
	if(res == FSK_IO_ERROR)
		return 0;

	engine->stage = FSK_STAGE_FIRST_DEL;
	res = __as_fsk_buf2_demod_ntt(dsp);
	if(res== 0 )
		return &(engine->param);

	return 0;
}

