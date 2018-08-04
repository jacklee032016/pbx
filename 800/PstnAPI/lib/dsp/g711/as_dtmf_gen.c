/*
 * $Log: as_dtmf_gen.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.3  2006/05/27 10:52:58  lizhijie
 * dd
 *
 * Revision 1.2  2006/03/24 11:07:46  lizhijie
 * add a C-language file for DTMF and silence data and compile it into libpstn.so
 *
 * Revision 1.1  2006/03/24 11:06:04  lizhijie
 * no message
 *
 * $Id: as_dtmf_gen.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include <math.h>

#include "g711.h"

#define DTMF_RAW_DURATION			200 	/* ms */
#define SILENCE_RAW_DURATION		20	/* 20ms, 160 bytes */

#define RAW_DATA_FILE				"dtmf_u.c"

#define		MU		1

typedef struct
{
	float		freq1;
	float		freq2;
	unsigned char		name[32];
}double_float;

double_float  dtmf_chars[] =
{
		{ 697.0, 1209.0 , "1"},	/* '1' */
		{ 697.0, 1336.0 , "2"},	/* '2' */
		{ 697.0, 1477.0 , "3"},	/* '3' */
		{ 697.0, 1633.0 , "A"},	/* 'A' */

		{ 770.0, 1209.0 , "4"},	/* '4' */
		{ 770.0, 1336.0 , "5"},	/* '5' */
		{ 770.0, 1477.0 , "6"},	/* '6' */
		{ 770.0, 1633.0 , "B"},	/* 'B' */

		{ 852.0, 1209.0 , "7"},	/* '7' */
		{ 852.0, 1336.0 , "8"},	/* '8' */
		{ 852.0, 1477.0 , "9"},	/* '9' */
		{ 852.0, 1633.0 , "C"},	/* 'C' */

		{ 941.0, 1209.0 , "*"},	/* '*' */
		{ 941.0, 1336.0 , "0"},	/* '0' */
		{ 941.0, 1477.0 , "#"},	/* '#' */
		{ 941.0, 1633.0 , "D"},	/* 'D' */
};


typedef struct 
{
	float						ci;		/* ci for local (non-fsk) tone generation */
	float						cr;		/* cr for local (non-fsk) tone generation */
	float						ci1;		/* ci1 for local (non-fsk) tone generation */
	float						cr1;		/* cr1 for local (non-fsk) tone generation */
}gen_cal_t;

/* this function is used to calclate the table of lin2a */
unsigned char   as_lib_law_linear2alaw (short linear)
{
#define LAW_AMP_BITS  		9		/* bits in amplitude -- can't be 16, */

#define A_LAW_AMI_MASK 0x55  /* A law constant is 86 */
	int mask;
	int seg;
	int pcm_val;
	static int seg_end[8] =
	{
		0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF
	};
    
	pcm_val = linear;
	if (pcm_val >= 0)
	{/* Sign (7th) bit = 1 , passtive */
		mask = A_LAW_AMI_MASK | 0x80;
	}
	else
	{/* Sign bit = 0 , negative */
		mask = A_LAW_AMI_MASK;
		pcm_val = -pcm_val;
	}

    /* Convert the scaled magnitude to segment number. */
	for (seg = 0;  seg < 8;  seg++)
	{/* which segement it belone to */
		if (pcm_val <= seg_end[seg])
			break;
	}
    /* Combine the sign, segment, and quantization bits. */
	return  ((seg << 4) | ((pcm_val >> ((seg)  ?  (seg + 3)  :  4)) & 0x0F)) ^ mask;
}

/* change a short integer into a char */
unsigned char  as_lib_law_linear2ulaw(short sample)
{
/*
** This routine converts from linear to ulaw
** Input: Signed 16 bit linear sample
** Output: 8 bit ulaw sample
*/
#define MU_LAW_ZEROTRAP    /* turn on the trap as per the MIL-STD */
#define MU_LAW_BIAS 0x84   /* define the add-in bias for 16 bit samples */
#define MU_LAW_CLIP 32635/* 8159*4 , 8159 is the value of last segment , less than 2**15=32768  */

	static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
                             4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
	int sign, exponent, mantissa;
	unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
	sign = (sample >> 8) & 0x80;          /* set aside the sign */
	if (sign != 0) 
		sample = -sample;              /* get magnitude */
	if (sample > MU_LAW_CLIP) 
		sample = MU_LAW_CLIP;             /* clip the magnitude */

  /* Convert from 16 bit linear to ulaw. */
	sample = sample + MU_LAW_BIAS;
	exponent = exp_lut[(sample >> 7) & 0xFF];
	mantissa = (sample >> (exponent + 3)) & 0x0F;
	ulawbyte = ~(sign | (exponent << 4) | mantissa);
#ifdef MU_LAW_ZEROTRAP
	if (ulawbyte == 0) 
		ulawbyte = 0x02;   /* optional CCITT trap */
#endif
	if (ulawbyte == 0xff) 
		ulawbyte = 0x7f;   /* never return 0xff */
	
	return(ulawbyte);
}

/* get sine sample value with 2 frequencies */
static float gen_localtone_df(gen_cal_t *cal, float ddr,float ddi,float ddr1, float ddi1)
{
	float t;
	
	t =cal->cr*ddr-cal->ci*ddi;
	cal->ci=cal->cr*ddi+cal->ci*ddr;
	cal->cr=t;
	
	t=2.0-(cal->cr*cal->cr+cal->ci*cal->ci);
	cal->cr*=t;
	cal->ci*=t;
	
	t =cal->cr1*ddr1-cal->ci1*ddi1;
	cal->ci1=cal->cr1*ddi1+cal->ci1*ddr1;
	cal->cr1=t;
	
	t=2.0-(cal->cr1*cal->cr1+cal->ci1*cal->ci1);
	cal->cr1*=t;
	cal->ci1*=t;
	
	return cal->cr + cal->cr1; 
};

static void gen_reset_coeff(gen_cal_t *cal)
{
	cal->cr = 1.0;	/* initialize cr value for local tone genration */
	cal->ci = 0.0;	/* initialize ci value for local tone generation */
	cal->cr1 = 1.0;	/* initialize cr value for local tone genration */
	cal->ci1 = 0.0;	/* initialize ci value for local tone generation */
}

int gen_arbtones(FILE *f , float f0 , float f1 , int len , int law )
{
	float	ddr,ddi,ddr1,ddi1;
	int	i ;
	gen_cal_t	cal;
	float			sine;
	unsigned char  data;
	int length = 8*len;

	ddr = cos(f0*2.0*M_PI/8000.0);
	ddi = sin(f0*2.0*M_PI/8000.0);
	ddr1 = cos(f1*2.0*M_PI/8000.0);
	ddi1 = sin(f1*2.0*M_PI/8000.0);

	gen_reset_coeff( &cal);

	for(i = 0; i <length; i++) 
	{
		sine = gen_localtone_df(&cal, ddr,ddi,ddr1,ddi1);

		if(law = MU)
		{
//			data = as_lib_law_linear2alaw( (short)rint(8192.0 * sine) );
			data = linear2ulaw( (short)rint(8192.0 * sine) );
		}	
		else
		{
//			data = as_lib_law_linear2alaw((short)rint(8192.0 * sine) );
			data = linear2alaw((short)rint(8192.0 * sine) );
		}
		
		fprintf(f, "\t0x%x", data);
		if(i != (length-1))
			fprintf(f, "," );
		if((i+1)%8 ==0 && i!=0 )
			fprintf(f, "\r\n" );
	}	

	return i;
}


int main(int argc, char *argv[])
{
	FILE *f;
	int delay = 300;

	float f1, f2;
	double_float *df = dtmf_chars;
	int i;

	printf( "To send DSP Software DTMF Test");
	if ((f = fopen(RAW_DATA_FILE,  "w") )==0) 
	{
		printf("ERROR : not create header file %s : %s",RAW_DATA_FILE, strerror(errno) );
		return 0;
	}

	fprintf(f, "/* DTMF RAW Tones used by the Assist DSP Library, in static tables.\n"
			   "   Generated automatically from program.  Do not edit by hand. Li Zhijie  */\n"); 
	
	fprintf(f, "\r\nunsigned char dtmf_raw_data[][%d]=\r\n{\r\n" , DTMF_RAW_DURATION*8); 
	for(i=0; i< 16; i++)
	{
		df = &(dtmf_chars[i]);
		fprintf(f, "{/* data for DTMF '%s' */\n", df->name);
		gen_arbtones( f, df->freq1, df->freq2, DTMF_RAW_DURATION/*ms*/, MU);
		fprintf(f, "},/* End of DTMF '%s' */\n\n\n", df->name);
		
	}
	fprintf(f, "};/* END dtmf_chars */\n" );
	
	fprintf(f, "\r\nunsigned char silence_raw_data[]=\r\n{/* silence data */\r\n");// , DTMF_RAW_DURATION*8); 
	gen_arbtones( f, 0.0, 0.0, SILENCE_RAW_DURATION/*ms*/, MU);
	fprintf(f, "};/* END dtmf_chars */\n\n" );

	fclose(f);
	printf("DSP_DTMF_GEN: DSP Tone Test ended successfully!\n");
	return 0;
}

