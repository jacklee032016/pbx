/*
 * $Author: lizhijie $
 * $Log: as_tonezone.h,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.1  2006/03/13 02:44:58  lizhijie
 * no message
 *
 * $Revision: 1.1.1.1 $
*/

#ifndef __AS_TONEZONE_H__
#define __AS_TONEZONE_H__

#include "as_dsp.h"

#define LEVEL -10

struct tone_zone_sound 
{
	int toneid;
	char data[256];				/* Actual zone description */
	/* Description is a series of tones of the format:
	   [!]freq1[+freq2][/time] separated by commas.  There
	   are no spaces.  The sequence is repeated back to the 
	   first tone description not preceeded by !.  time is
	   specified in milliseconds */
};

struct tone_zone 
{
	int zone_id;					/* Zone number */
	char country[10];			/* Country code */
	char description[40];		/* Description */
	int ringcadence[AS_MAX_CADENCE];	/* Ring cadence */
	struct tone_zone_sound tones[AS_TONE_MAX];
};

extern struct tone_zone builtin_zones[];


#define USED_ZONE_INDEX	7	/* 7  for Japan*/

#endif
