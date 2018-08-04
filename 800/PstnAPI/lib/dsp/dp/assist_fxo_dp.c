/*
* $Id: assist_fxo_dp.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
* $Author: lizhijie $
*/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include "assist_lib.h"
#include "as_dev_ctl.h"
#include <sys/ioctl.h>
#include <unistd.h>
 


typedef struct
{
	char    chr;   
	int 	   dp_no;
} AS_DP;
 
AS_DP as_fxo_dp[] = 
{
	{ '1',1 },
	{ '2',2 },
	{ '3',3 },
	{ '4',4 },
	{ '5',5 },
	{ '6',6 },
	{ '7',7 },
	{ '8',8 },
	{ '9',9 },
	{ '0',10 },
	{ '*',11 },
	{ '#',12 },
	{0,0}
} ;

int assist_dsp_fxo_send_pps(int fd,char *digitstring,pps_type pps)
{	
	int res,i,j,k;
	AS_DP *d;
	int flag;

	AS_DIAL_OPERATION dtmf_dial_str ;

	res = as_lib_offhook(fd);
	usleep(200000);
	
	k=0;
	if(pps == AS_DP_10PPS)
		dtmf_dial_str.op = AS_DIAL_OP_FOX_SEND_DP_10PPS;
	else if (pps == AS_DP_20PPS)
		dtmf_dial_str.op = AS_DIAL_OP_FOX_SEND_DP_20PPS;
	else
		return -1;
	
	for(i = 0; digitstring[i]; i++)
	{
		flag =0;
		d =  as_fxo_dp;
		
		for(j = 0; d[j].chr; j++)
		{
			if(d[j].chr == digitstring[i])
			{
				flag = 1;
				break;
			}
		}

		if(flag == 0)
			continue;

		dtmf_dial_str.dialstr[k+1] = d[j].dp_no;
		
		k++;

	}

	dtmf_dial_str.dialstr[0] = k;

	res = ioctl(fd,  AS_CTL_SET_DTMF_STR, &dtmf_dial_str);
	
	return 0;
}

