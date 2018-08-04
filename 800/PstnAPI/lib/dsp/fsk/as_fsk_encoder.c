/*
 * $Log: as_fsk_encoder.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.6  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.5  2006/05/23 02:54:17  lizhijie
 * dd
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
 * $Id: as_fsk_encoder.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
 #include "as_dev_ctl.h"

#include "assist_lib.h"
#include "as_fsk.h"
#include "as_dsp.h"


/* finish writing out the audio buffer (if any). Returns 0 if not interrupted 
by DTMF, 1 if interrupted by DTMF, or -1 if error, or -2 if hung up .  */
static int __as_fsk_finish_audio( fsk_t *dsp, int fd,int flag)
{
	int length;
	fsk_tone_data_t  *data = &(dsp->config->tone_data);
	
	if ( data->bufp)  /* if something to write */
	{
		while( data->bufp < dsp->config->buf_size /*AS_DTMF_BLOCKSIZE*/)   /* pad with silence */
		{		
			 data->buf[ data->bufp] = LINEAR2XLAW(0, dsp->law /*U_LAW_CODE*/); 
			 data->bufp++;
		}

		
		data->bufp = 0;
		  /* write the block */
		length = write(fd, data->buf, dsp->config->buf_size );
		if(length != dsp->config->buf_size )
		{
			printf("write error in last block data\n");
			/* hangup */
			if ( (errno == AS_ELAST) && (dsp->config->buf_type == FSK_BUFFER_PSTN) )
			{
				if ( as_lib_event_get( fd) == AS_EVENT_RINGOFFHOOK  ) 
				{
					printf("Write failed : Some event is still in firmware queue(RingerOff)\n");
					return FSK_IO_AGAIN;
				}	
			}	
			return FSK_IO_ERROR;
		}
#if FSK_DEBUG
//		printf("write %d bytes into file\n", length);
#endif
	}
	
	/* if not in DTMF mode, ignore */
	return(0);
}

static int __as_fsk_put_audio_sample( fsk_t *dsp, int fd, float y, int flag)
{
	int length;
	fsk_tone_data_t  *data = &(dsp->config->tone_data);
	short sample;

	sample = (short)rint(8192.0 * y);

	data->buf[data->bufp]=LINEAR2XLAW(sample, dsp->law /*U_LAW_CODE*/);
	data->bufp++;
	
	if (data->bufp== dsp->config->buf_size/*AS_DTMF_BLOCKSIZE*/) 
	{
		data->bufp=0;
		length = write(fd, data->buf, dsp->config->buf_size);
		if(length!= dsp->config->buf_size )
		{
			printf("write length is error\r\n");
			if ( (errno == AS_ELAST) && (dsp->config->buf_type == FSK_BUFFER_PSTN) )
			{
				if ( as_lib_event_get( fd) == AS_EVENT_RINGOFFHOOK  ) 
				{
					printf("Write failed : Some event is still in firmware queue\n");
					return FSK_IO_AGAIN;
				}	
			}	
			return FSK_IO_ERROR;
		}
#if FSK_DEBUG
//		printf("write %d bytes into file\n", length);
#endif

	}

	return (0);
}

float __as_fsk_get_carrier(fsk_t *dsp, fsk_carrier_type type)
{
	float t;
	fsk_encoder_t *gen = &(dsp->encoder);
	fsk_calculator_t *cal = &(dsp->cal);
	
	t = cal->cr*gen->dr[type] - cal->ci*gen->di[type];
	cal->ci= cal->cr*gen->di[type]+cal->ci*gen->dr[type];
	cal->cr=t;
	
	t=2.0-(cal->cr*cal->cr + cal->ci*cal->ci);
	cal->cr*=t;
	cal->ci*=t;
	
	return cal->cr;
};

/* send out a millisecond of CLID mark tone */
static int __as_fsk_put_a_ms_mark(fsk_t *dsp , int fd)
{
	int	i,r;
	float f;
	
	/* do 8 samples of it */
	for(i = 0; i < 8; i++)
	{
		f = __as_fsk_get_carrier(dsp, AS_FSK_MARK);
		if ((r = __as_fsk_put_audio_sample(dsp, fd, f, 0))) 
			return(r);
	}	
	return(0);
}

/* put out a single baud (bit) of FSK data (for CLID) , which is AS_FSK_MARK|SPACE */
static int __as_fsk_put_baud_bit(fsk_t *dsp, int fd, int which)
{
	int r;
	float clidsb;
	float f;
#if 0	
	int i=0;
#endif

	clidsb = 8000.0 / 1200.0;  /* fractional baud time */
	for(;;)  /* loop until no more samples to send */
	{
		  /* output the sample */
		f = __as_fsk_get_carrier(dsp, which);
		if ((r = __as_fsk_put_audio_sample(dsp, fd ,f, 0 ))) 
			return(r);
#if 0		
		i++;
		if(i==8)
			break;
#else
		dsp->encoder.scont += 1.0;  /* bump by 1 sample time */
		if (dsp->encoder.scont > clidsb)  /* if greater then 1 baud in real time */
		{
			dsp->encoder.scont -= clidsb;  /* leave remainder around for next one */
			break;
		}
#endif		
	}
	return(0);
}

/* Output a character ( first 8 bit in int 'by'of CLID FSK data */
static int __as_fsk_put_a_char(fsk_t *dsp, int fd,unsigned char by)
{
	int j,k,r;

	/* send start bit */ 
	if ((r = __as_fsk_put_baud_bit(dsp, fd, AS_FSK_SPACE))) 
		return(r);

	/* send a byte */
	for (j=0;j<8;j++) 
	{
		k=by&1;
		by>>=1;
		if ((r = __as_fsk_put_baud_bit(dsp, fd, k))) 
			return(r);
	}

	/* send stop bit */
	if ((r = __as_fsk_put_baud_bit(dsp, fd, AS_FSK_MARK))) 
		return(r);

	return 0;
}

/* ring the phone and send a Caller*ID string in MDMF format. Returns 0 if caller did not
answer during this function, 1 if caller answered after receiving CLID, and
2 if caller answered before receiving CLID */
/* pointer to buffer in which to return caller's number */
/* pointer to buffer in which to return caller's name */
int as_dsp_lib_ring_with_fsk_callerid(fsk_t *dsp, int fd , unsigned char *number , unsigned char *name )
{
	int	i,j,k,r;
	unsigned char	msg[100];
	time_t	t;
	struct	tm *tm;
	unsigned char crc =0;
	int length = 0 ;

	if (fd <0 ) /* if NULL arg */
	{
		errno= EINVAL;  /* set to invalid arg */
		printf("fd is error %d\n ", fd);
		return(-1);  /* return with error */
	}
	dsp->encoder.scont = 0.0; /* initialize fractional time accumulator */

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

	if(dsp->config->buf_type == FSK_BUFFER_PSTN)
	{
		as_ring_on_hook(fd );
		as_lib_wait_ringer_off( fd);
	}
	usleep(RINGOFF_BEFORE_FSK*1000);  /* sleep at least 1/2 sec */

	/* output 30 0x55's : 200ms channel used ID */
	for(i = 0; i < 30; i++) 
	{
		r = __as_fsk_put_a_char(dsp, fd, 0x55 );
		if(r )
			return r;
	}
	
	/* 150ms of markage */
	for(i = 0; i < 150; i++)
	{
		r = __as_fsk_put_a_ms_mark(dsp, fd);
		if (r ) 
			return r;
	}

	/* put out 0x80 indicting MDMF format : Multi Data Message Format  */
	r = __as_fsk_put_a_char(dsp, fd, FSK_FORMAT_MDMF );
	if (r) 
		return r;

	/* put out length of entire message */
	r = __as_fsk_put_a_char(dsp, fd, strlen(msg));
	if (r) 
		return r;

	/* go thru entire message */
	for(i = 0,j = 0;  msg[i];  i++)
	{
		r = __as_fsk_put_a_char(dsp, fd, msg[i]);  /* output this byte */
		if (r ) 
			return r;
		j += msg[i]; /* update checksum */
		crc += msg[i];
	}
	
	crc += FSK_FORMAT_MDMF; /* add message type to checksum */
	crc += strlen(msg);  /* add message length to checksum */
	r = __as_fsk_put_a_char(dsp, fd,  256 - (crc & 255));  /* output checksum (twos-complement) */
	if (r ) 
		return r;

#if   1
	/* send marks for 50 ms to allow receiver to settle */
	for(i = 0; i < 50; i++)
	{
		r = __as_fsk_put_a_ms_mark(dsp, fd);  /* send mark for a millisecond */
		if (r ) 
			return r; /* return if error */
	}
#endif

	/* finish writing audio */
	i = __as_fsk_finish_audio(dsp, fd, 0);
	if ( i ) 
		return  i; /* return if error */

#if  1
	/* calculate how long (in microseconds) its been since end of ringy 
	* 8333: 8ms, about 8 ms for every byte modulated by FSK
	*/
	i = ((strlen(msg) + 33) * 8333) + 700000 + (102 * 125); /* half of 204 */
	k = RING_OFF_LENGTH*1000 - i;	/* gap between ring signal , not requirement */
	while(k > 0)  /* do rest of ring silence period */
	{
		k -= 125;  /* 125 usec per sample at 8000HZ */
		  /* output 1 sample of silence */
		i = __as_fsk_put_audio_sample(dsp, fd, 0, 0);
		if (i ) 
			return i;
	}
#endif

/*
	if(dsp->config->buf_type == FSK_BUFFER_PSTN)
	{
		i = AS_START;
		k = ioctl(fd, AS_CTL_HOOK, &i);
	}
*/

	return 0;;
}

int as_dsp_lib_ring_with_ntt_callerid(fsk_t *dsp, int fd , unsigned char *number  )
{
	int	i, k = 0,r;
	unsigned char	msg[NTT_FSK_BLOCK_LENGTH];
	unsigned short crc =0;
	int phoneLength = 0;
	int frameLength = 0;

	if (fd <0 ) /* if NULL arg */
	{
		errno= EINVAL;  /* set to invalid arg */
		printf("fd is error %d\n ", fd);
		return(-1);  /* return with error */
	}
	dsp->encoder.scont = 0.0; /* initialize fractional time accumulator */

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
		r = __as_fsk_put_a_ms_mark(dsp, fd);
		if (r ) 
			return r;
	}

	/* go thru entire frame */
	for(i = 0 ; i < frameLength;  i++)
	{
	
#if FSK_DEBUG
		printf("%02x ", msg[i]);
		if(i%15==0 && i!= 0)
			printf("\n");
#endif
		r = __as_fsk_put_a_char(dsp, fd, msg[i]);  /* output this byte */
		if (r ) 
			return r;
	}
#if FSK_DEBUG
	printf("\n");
#endif

	/* finish writing audio */
	i = __as_fsk_finish_audio(dsp, fd, 0);
	if ( i ) 
		return  i;

	return 0;;
}


