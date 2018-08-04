/*
 * $Id: as_fsk_encoder_buf.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"

#if 0
static int __as_fsk_buf_finish_audio( fsk_t *dsp )
{
	int length;
	int count = dsp->modemLength/AS_DRIVER_BUFFER_LENGTH;

	printf("modem length is %d\n" ,dsp->modemLength);
	if( (count*AS_DRIVER_BUFFER_LENGTH) == dsp->modemLength )
		return 0;
	length = (count+1)*AS_DRIVER_BUFFER_LENGTH;

	if( length >= FSK_MODEM_BUFFER_LENGTH )
	{
		printf("Length of modulation Caller FSK is more than %d. Enlarge the buffer please!", FSK_MODEM_BUFFER_LENGTH);
		exit(1);
	}

	printf("count : %d  length : %d\n", count, length);
	for(count= dsp->modemLength; count < length; count++)
	{		
		 dsp->modem_buf[ dsp->modemLength++ ] = LINEAR2XLAW(0, dsp->law /*U_LAW_CODE*/); 
	}
	
	return (0);
}
#endif

static int __as_fsk_buf_put_audio_sample( fsk_t *dsp,  float y)
{
	short sample;

	sample = (short)rint(8192.0 * y);

	dsp->modem_buf[dsp->modemLength++]=LINEAR2XLAW(sample, dsp->law /*U_LAW_CODE*/);
	if(dsp->modemLength >= FSK_MODEM_BUFFER_LENGTH )
	{
		printf("Length of modulation Caller FSK is more than %d. Enlarge the buffer please!", FSK_MODEM_BUFFER_LENGTH);
		exit(1);
	}
	return (0);
}

/* send out a millisecond of CLID mark tone */
static int __as_fsk_buf_put_a_ms_mark(fsk_t *dsp )
{
	int	i;
	float f;
	
	/* do 8 samples of it */
	for(i = 0; i < 8; i++)
	{
		f = __as_fsk_get_carrier(dsp, AS_FSK_MARK);
		__as_fsk_buf_put_audio_sample(dsp, f);
	}	
	return (0);
}

/* put out a single baud (bit) of FSK data (for CLID) , which is AS_FSK_MARK|SPACE */
static int __as_fsk_buf_put_baud_bit(fsk_t *dsp, int which)
{
	float clidsb;
	float f;

	clidsb = 8000.0 / 1200.0;  /* fractional baud time */
	for(;;)  /* loop until no more samples to send */
	{
		  /* output the sample */
		f = __as_fsk_get_carrier(dsp, which);
		__as_fsk_buf_put_audio_sample(dsp, f );

		dsp->encoder.scont += 1.0;  /* bump by 1 sample time */
		if (dsp->encoder.scont > clidsb)  /* if greater then 1 baud in real time */
		{
			dsp->encoder.scont -= clidsb;  /* leave remainder around for next one */
			break;
		}
	}
	return(0);
}

/* Output a character ( first 8 bit in int 'by'of CLID FSK data */
static int __as_fsk_buf_put_a_char(fsk_t *dsp, unsigned char by)
{
	int j,k;

	/* send start bit */ 
	__as_fsk_buf_put_baud_bit(dsp, AS_FSK_SPACE);

	/* send a byte */
	for (j=0;j<8;j++) 
	{
		k=by&1;
		by>>=1;
		__as_fsk_buf_put_baud_bit(dsp,k);
	}

	/* send stop bit */
	__as_fsk_buf_put_baud_bit(dsp, AS_FSK_MARK);

	return 0;
}

/* encode a Caller*ID string in MDMF format. Returns 0 */
unsigned char *as_dsp_lib_encode_chinese_callerid(fsk_t *dsp,  unsigned char *number , unsigned char *name, int *returnLength )
{
	int	i,j,k;
	unsigned char	msg[100];
	time_t	t;
	struct	tm *tm;
	unsigned char crc =0;
	int length = 0 ;

	dsp->encoder.scont = 0.0; /* initialize fractional time accumulator */
	dsp->modemLength = 0;

	/* get current time */
	time(&t);
	/* put into usable form */
	tm = localtime(&t);
	
	/* put date into message : 0x01 : msg of date type, 0x08 : length of date msg*/
	sprintf(msg,"\001\010%02d%02d%02d%02d",tm->tm_mon + 1,tm->tm_mday, tm->tm_hour,tm->tm_min);
	/* if number absence */
	length = 10;

	if ((!number) || (number[0] == 'O') || (number[0] == 'P'))
	{
		strcat(msg,"\004\001"); /* indicate number absence */
		if (number && (number[0] == 'O')) 
			strcat(msg,"O");
		else 
			strcat(msg,"P");
	}
	else /* if its there */
	{
		i = strlen(number);  /* get len of number */
		if (i > FSK_CALL_ID_MAX_LENGTH) 
			i = FSK_CALL_ID_MAX_LENGTH;  /* limit at FSK_CALL_ID_MAX_LENGTH */
		sprintf(msg + strlen(msg),"\002%c", i);  /* indicate number sequence */
//		sprintf(msg + strlen(msg),"%d%c", FSK_MSG_TYPE_NUMBER, i);  /* indicate number sequence */
		j = strlen(msg);
		  /* add number to end of message */
		for(k = 0; k < i; k++) 
			msg[k + j] = number[k];
		
		msg[k + j] = 0;  /* terminate string */
	}
	   /* if name absence */
	if ((!name) || (((name[0] == 'O') || (name[0] == 'P')) && !name[1]))
	{
		strcat(msg,"\010\001"); /* indicate name absence */
		if (name && (name[0] == 'O')) 
			strcat(msg,"O");
		else 
			strcat(msg,"P");
	}
	else /* if its there */
	{
		i = strlen(name);  /* get len of name */
		if (i > 16) 
			i = 16;  /* limit at 16 */
		
		sprintf(msg + strlen(msg),"\007%c", i );  /* indicate name sequence */
//		sprintf(msg + strlen(msg),"%d%c", FSK_MSG_TYPE_NAME, i );  /* indicate name sequence */
		j = strlen(msg);
		  /* add name to end of message */
		for(k = 0; k < i; k++) 
			msg[k + j] = name[k];
		msg[k + j] = 0;  /* terminate string */
	}

	/* output 30 0x55's : 200ms channel used ID */
	for(i = 0; i < 30; i++) 
	{
		__as_fsk_buf_put_a_char(dsp, 0x55 );
	}

	/* 150ms of markage. This is mandidate  */
	for(i = 0; i < 150; i++)
	{
		__as_fsk_buf_put_a_ms_mark(dsp);
	}

	/* put out 0x80 indicting MDMF format : Multi Data Message Format  */
	__as_fsk_buf_put_a_char(dsp, FSK_FORMAT_MDMF );

	/* put out length of entire message */
	__as_fsk_buf_put_a_char(dsp, strlen(msg));

	/* go thru entire message */
	for(i = 0;  msg[i];  i++)
	{
		__as_fsk_buf_put_a_char(dsp, msg[i]);  /* output this byte */
		crc += msg[i];/* update checksum */
	}
	
	crc += FSK_FORMAT_MDMF; /* add message type to checksum */
	crc += strlen(msg);  /* add message length to checksum */
//	printf("CRC = 0x%x\n", 256 - (crc & 255) );
	__as_fsk_buf_put_a_char(dsp, 256 - (crc & 255));  /* output checksum (twos-complement) */

	*returnLength = dsp->modemLength;
	
	return dsp->modem_buf;
}

/* return : buffer with FSK modulated data */
unsigned char *as_dsp_lib_encode_ntt_callerid(fsk_t *dsp, unsigned char *number , int *returnLength )
{
	int	i, k = 0;
	unsigned char	msg[NTT_FSK_BLOCK_LENGTH];
	unsigned short crc =0;
	int phoneLength = 0;
	int frameLength = 0;

	dsp->encoder.scont = 0.0; /* initialize fractional time accumulator */
	dsp->modemLength = 0;

	/* the first DLE and the SOH is not CRCed, total 6 octets header */
	sprintf(msg,"%c%c%c%c%c%c", 
		NTT_FSK_DLE|NTT_FSK_PARITY_BIT, NTT_FSK_SOH|NTT_FSK_PARITY_BIT, 
		NTT_FSK_HEADER,  NTT_FSK_DLE, NTT_FSK_STX, NTT_FSK_SERVICE_TYPE);

#if 0	
	if ((!number) || (number[0] == 'O') || (number[0] == 'P'))
	{
		strcat(msg,"\004\001"); /* indicate number absence */
		if (number && (number[0] == 'O')) 
			strcat(msg,"O");
		else 
			strcat(msg,"P");
		return 0;
	}
#endif

	phoneLength = strlen(number);  /* get len of number */
	if (phoneLength > FSK_CALL_ID_MAX_LENGTH) 
		phoneLength = FSK_CALL_ID_MAX_LENGTH;

	frameLength = NTT_FSK_HEADER_LENGTH;	/*FSK Frame header length */
	/* if length is 16(DLE), must delimited it */
	frameLength += sprintf(msg + frameLength ,"%c%c%c", phoneLength+2, NTT_FSK_PARAM_NUMBER, phoneLength);  /* indicate number sequence */

	  /* add number to end of message */
	for(k = 0; k <phoneLength; k++) 
		msg[frameLength ++ ] = number[k];
	
	msg[frameLength++] = NTT_FSK_DLE;
	msg[frameLength++] = NTT_FSK_EXT;

	crc = as_fsk_crc(msg+2, frameLength-2);//msgLength+6+2)
#if 1	
	msg[frameLength++] = crc>>8;
	msg[frameLength++] = crc&0xff;
#else	
	msg[frameLength++] = ((crc>>4)&0xf)|((crc<<4)&0xf0);
	msg[frameLength++] = ((crc>>12)&0x0f) |((crc>>4)&0xf0) ;
#endif	
	msg[frameLength+1] = 0;
	
	/* at least 100ms of markage */
	for(i = 0; i < NTT_FSK_MARK_DURATION +50; i++)
	{
		__as_fsk_buf_put_a_ms_mark(dsp);
	}

	/* go thru entire frame */
	for(i = 0 ; i < frameLength;  i++)
	{
#if FSK_DEBUG
		printf("%02x ", msg[i]);
		if(i%15==0 && i!= 0)
			printf("\n");
#endif
		__as_fsk_buf_put_a_char(dsp, msg[i]);  /* output this byte */
	}
#if FSK_DEBUG
	printf("\n");
#endif

	*returnLength = dsp->modemLength;
	
	return dsp->modem_buf;
}

