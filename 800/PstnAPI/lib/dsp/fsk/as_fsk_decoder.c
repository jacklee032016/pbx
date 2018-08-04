/*
 * $Log: as_fsk_decoder.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
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
 * $Id: as_fsk_decoder.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "assist_lib.h"
#include "as_fsk.h"


/*    Baudot Receive translation module  */
unsigned char __baudot(fsk_decoder_t *engine,unsigned char data)	/* Decodifica el código BAUDOT	*/
{
	static char letras[32]={'<','E','\n','A',' ','S','I','U',
				'\n','D','R','J','N','F','C','K',
				'T','Z','L','W','H','Y','P','Q',
				'O','B','G','^','M','X','V','^'};
	static char simbol[32]={'<','3','\n','-',' ',',','8','7',
				'\n','$','4','\'',',','·',':','(',
				'5','+',')','2','·','6','0','1',
				'9','7','·','^','.','/','=','^'};
	unsigned char d;
	d=0;			/* Retorna 0 si el código no es un caracter	*/
	switch (data) 
	{
		case 0x1f :
			engine->modo=0; 
			break;
		case 0x1b : 
			engine->modo=1; 
			break;
		default:	
			if (engine->modo==0) 
				d=letras[data]; 
			else 
				d=simbol[data]; 
			break;
	}
	return d;
}


/*
Get an ASCII character from a TTY/TDD device
    Upon Successful completion, returns character received in ASCII.
    Returns -1 if error, with error in errno, or -2 if user disconnected.
*/
static int __as_fsk_decode_gettdd( fsk_t *dsp, int fd)
{
	int	b,c;
	fsk_decoder_t *engine = &(dsp->decoder);

	for(;;)
	{
		/* get baudot char */
		b = as_fsk_serie(dsp,  fd);
		if (b == -1)
		{
			if ( (errno == ELAST) && (dsp->config->buf_type == FSK_BUFFER_PSTN) ) 
				return FSK_IO_AGAIN;
			return FSK_IO_ERROR;
		}
		  /* if null, do it again */
		if (!b) 
			continue;
		  /* if in Caller*ID mode, return with Ascii */
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

/* get a Caller*ID string */
/* pointer to channel structure 
 pointer to buffer in which to return caller's number 
 pointer to buffer in which to return caller's name */
int as_fsk_decode_clid_from_fd( fsk_t *dsp, int fd, char *number, char *name )
{
	int	c,cs,m,n,i,j,rv;
	unsigned char msg[256];
	fsk_decoder_t *engine = &(dsp->decoder);

//	unsigned char crc;

	if (!engine) /* if NULL arg */
	{
		errno= EINVAL;  /* set to invalid arg */
		return(-1);  /* return with error */
	}
	
//	as_dsp_set_gain(zap->fd, 0, zap->txgain, zap->rxgain + 9.0, zap->law);

	if(dsp->config->buf_type == FSK_BUFFER_PSTN)
	{
		as_lib_wait_offhook( fd);
	}	

	/* Caller*ID read values setup */
	/* Valores por defecto */
	engine->fskd.spb=7;	/* 1200 baudios */
	engine->fskd.hdlc=0;		/* Modo asíncrono */
	engine->fskd.nbit=8;		/* Ascii */
	engine->fskd.nstop=1;	/* Bits de Stop */
	engine->fskd.paridad=0;	/* Paridad ninguna */
	engine->fskd.bw=1;		/* Filtro 800 Hz */
	engine->fskd.f_mark_idx=2;	/* f_MARK = 1200 Hz */
	engine->fskd.f_space_idx=3;	/* f_SPACE = 2200 Hz */
	engine->fskd.pcola=0;		/* Colas vacías */
	i = 0; /* reset flag-found flag */
	engine->cont = 0; /* reset DPLL thing */
	rv = 0; /* start with happy return value */
	errno = 0;

	for(;;)
	{
		  /* clear output buffers, if specified */
		if (name) 
			*name = 0;
		if (number) 
			*number = 0;

		m = __as_fsk_decode_gettdd(dsp, fd);  /* get first byte */

#if FSK_DEBUG		
		if(m == FSK_FORMAT_MDMF )
			printf("FSK msg MDMF\n");
		else if(m == FSK_FORMAT_SDMF)
			printf("FSK msg SDMF\n");
		else
			printf("Unknown FSK format '0x%x'\n", m);
#endif
		
		if (m < 0) /* if error, return as such */
		{
			rv = -1;
			break;
		}
		if (m == 'U')  /* if a flag byte, set the flag */
		{
			i = 1; /* got flag */
			continue;
		}
		
		if (!i) 
			continue; /* if no flag, go on */

		/* if not message lead-in, go on */
		if ((m != 4) && (m != 0x80)) 
			continue;

		n = __as_fsk_decode_gettdd(dsp, fd); /* get message size */
#if FSK_DEBUG		
		printf("FSK msg length %d\n" , n);
#endif
		if (n < 0)  /* if error, return as such */
		{
			rv = -1;
			break;
		}
		
		j = m + n; /* initialize checksum with first 2 bytes */
		  /* collect the entire message */
		for(i = 0; i < n; i++)
		{
			c = __as_fsk_decode_gettdd(dsp, fd);  /* get next byte */
			if (c < 0)  /* if error, stop */
			{
				rv = -1;
				break;
			}
#if FSK_DEBUG		
//			printf("%c(0x%x) " , c , c );
			printf("(0x%x) " ,  c );
#endif
			msg[i] = c; /* save it */
			j += c;  /* add it to checksum */
		}
#if FSK_DEBUG		
		printf("\n" );
#endif
		if (rv == -1) 
			break; /* if error, return as such */
		msg[i] = 0; /* terminate the string */
		
		cs = __as_fsk_decode_gettdd(dsp, fd); /* get checksum byte from message */
		if (cs < 0) /* if error, return as such */
		{
			rv = -1;
			break;
		}
		  /* if bad checksum, try it again */
		if (cs != 256 - (j & 255))
		{
			printf("CRC error : 0x%x\n", cs);
			i = 0; /* reset flag-found flag */
			engine->cont = 0; /* reset DPLL thing */
			continue;  /* do it again */
		}

		if (m == 0x80) /* MDMF format */
		{
			printf("MDMF handled\r\n");
			for(i = 0; i < n;)  /* go thru whole message */
			{
				switch(msg[i++]) /* message type */
				{
					case 1: /* date */
						break;
					case 2: /* number */
					case 4: /* number */
						if (number) /* if number buffer specified */
						{ /* copy it */
							strncpy(number,msg + i + 1,msg[i]);
							/* terminate it */
							number[msg[i]] = 0;
						}
						break;
				    case 7: /* name */
				    case 8: /* name */
						if (name) /* if name buffer specified */
						{ /* copy it */
							strncpy(name,msg + i + 1,msg[i]);
							/* terminate it */
							name[msg[i]] = 0;
						}
						break;
				}
				/* advance index to next message element */
				i += msg[i] + 1; 
			}
		}
		else /* must be SDMF then */
		{ /* if number buffer specified */
			printf("SDMF\r\n");
			if (number) 
				strcpy(number, msg + 8); /* copy number */
		}
		rv = 0; /* good, happy return */
		break;
	}
	
	/* TDD read values setup */
	/* Valores por defecto */
	engine->fskd.spb=176;	/* 45.5 baudios */
	engine->fskd.hdlc=0;		/* Modo asíncrono */
	engine->fskd.nbit=5;		/* Baudot */
	engine->fskd.nstop=1.5;	/* Bits de Stop */
	engine->fskd.paridad=0;	/* Paridad ninguna */
	engine->fskd.bw=0;		/* Filtro 75 Hz */
	engine->fskd.f_mark_idx=0;	/* f_MARK = 1400 Hz */
	engine->fskd.f_space_idx=1;	/* f_SPACE = 1800 Hz */
	engine->fskd.pcola=0;		/* Colas vacías */
	  /* put original gains back */
//	as_dsp_set_gain(ap->fd, 0, ap->txgain, ap->rxgain, ap->law);
	  /* if hung up */
	if ((rv == -1) && (errno == ELAST)) 
		return(-2);
	
	return(rv); /* return the return value */
}

/* remove parity bit */
#define NTT_GET_TDD(_dsp, _fd)  \
	(__as_fsk_decode_gettdd((_dsp), (_fd)) &0x7F )

int as_fsk_decode_ntt_clid_from_fd( fsk_t *dsp, int fd, char *number )
{
	int	c,cs,m,n = 0,i,j,rv;
	unsigned char msg[256];
	fsk_decoder_t *engine = &(dsp->decoder);
	int first = 1;

//	unsigned char crc;

	if (!engine) /* if NULL arg */
	{
		errno= EINVAL;  /* set to invalid arg */
		return(-1);  /* return with error */
	}
	
	if(dsp->config->buf_type == FSK_BUFFER_PSTN)
	{
		as_lib_wait_offhook( fd);
	}	

	/* Caller*ID read values setup */
	/* Valores por defecto */
	engine->fskd.spb=7;			/* 1200 baudios */
	engine->fskd.hdlc=0;			/* Modo asíncrono */
	engine->fskd.nbit = 8;			/* Ascii */
	engine->fskd.nstop=1;			/* Bits de Stop */
	engine->fskd.paridad= 0;		/* Paridad ninguna */
	engine->fskd.bw=1;			/* Filtro 800 Hz */
	engine->fskd.f_mark_idx=2;	/* f_MARK = 1200 Hz */
	engine->fskd.f_space_idx=3;	/* f_SPACE = 2200 Hz */
	engine->fskd.pcola=0;			/* Colas vacías */
	i = 0; 						/* reset flag-found flag */
	engine->cont = 0; 				/* reset DPLL thing */
	rv = 0; 						/* start with happy return value */
	errno = 0;

	for(;;)
	{
		if (number) 
			*number = 0;

		m = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd);  /* get first byte */

#if FSK_DEBUG		
		if(m == NTT_FSK_DLE )
			printf("FSK msg NTT_FSK_DLE\n");
		else if(m == NTT_FSK_SOH)
			printf("FSK msg NTT_FSK_SOH\n");
		else if(m == NTT_FSK_HEADER)
			printf("FSK msg NTT_FSK_HEADER\n");
		else if(m == NTT_FSK_STX && first)
		{
			first = 0;
			printf("FSK msg NTT_FSK_STX\n");
		}	
		else if(m == NTT_FSK_SERVICE_TYPE)
		{
			printf("FSK msg NTT_FSK_SERVICE_TYPE\n");
			n = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd); /* get message size */
			printf("Msg Content Length %d\n", n );
		}	
		else if(m == NTT_FSK_PARAM_NUMBER && !first )
		{
			printf("FSK msg NTT_FSK_PARAM_NUMBER\n");
			n = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd); /* get message size */
			printf("Data Content Length %d\n", n );
			i = 1;
		}	
		else
			printf("Unknown FSK format '0x%x'\n", m);
		
#endif
		
		if (m < 0) /* if error, return as such */
		{
			rv = -1;
			break;
		}
#if 0		
		if (m == 'U')  /* if a flag byte, set the flag */
		{
			i = 1; /* got flag */
			continue;
		}
#endif		
		if (!i) 
			continue; /* if no flag, go on */

//		n = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd); /* get message size */
#if FSK_DEBUG		
		printf("FSK msg length %d\n" , n);
#endif
		if (n < 0)  /* if error, return as such */
		{
			rv = -1;
			break;
		}
		
		j = m + n; /* initialize checksum with first 2 bytes */
		  /* collect the entire message */
		for(i = 0; i < n; i++)
		{
			c = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd);  /* get next byte */
			if (c < 0)  /* if error, stop */
			{
				rv = -1;
				break;
			}
#if FSK_DEBUG		
//			printf("%c(0x%x) " , c , c );
			printf("(0x%x) " ,  c );
#endif
			msg[i] = c; /* save it */
			j += c;  /* add it to checksum */
		}
#if FSK_DEBUG		
		printf("\n" );
#endif
		if (rv == -1) 
			break; /* if error, return as such */
		msg[i] = 0; /* terminate the string */
		
		cs = NTT_GET_TDD(dsp, fd);//__as_fsk_decode_gettdd(dsp, fd); /* get checksum byte from message */
		if (cs < 0) /* if error, return as such */
		{
			rv = -1;
			break;
		}
		  /* if bad checksum, try it again */
		if (cs != 256 - (j & 255))
		{
			printf("CRC error : 0x%x\n", cs);
			i = 0; /* reset flag-found flag */
			engine->cont = 0; /* reset DPLL thing */
//			continue;  /* do it again */
			
		}

		if (number) 
			strcpy(number, msg ); /* copy number */
		rv = 0; /* good, happy return */
		break;
	}
	
	/* TDD read values setup */
	/* Valores por defecto */
	engine->fskd.spb=176;	/* 45.5 baudios */
	engine->fskd.hdlc=0;		/* Modo asíncrono */
	engine->fskd.nbit=5;		/* Baudot */
	engine->fskd.nstop=1.5;	/* Bits de Stop */
	engine->fskd.paridad=0;	/* Paridad ninguna */
	engine->fskd.bw=0;		/* Filtro 75 Hz */
	engine->fskd.f_mark_idx=0;	/* f_MARK = 1400 Hz */
	engine->fskd.f_space_idx=1;	/* f_SPACE = 1800 Hz */
	engine->fskd.pcola=0;		/* Colas vacías */
	  /* put original gains back */
//	as_dsp_set_gain(ap->fd, 0, ap->txgain, ap->rxgain, ap->law);
	  /* if hung up */
	if ((rv == -1) && (errno == ELAST)) 
		return(-2);
	
	return(rv); /* return the return value */
}

