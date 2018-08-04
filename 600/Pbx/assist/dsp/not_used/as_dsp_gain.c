/*
 * $Id: as_dsp_gain.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include "assist_globals.h"
#include "as_user_dsp.h"
#include "pbx_globals.h"

/* gain is float in the form of dB */
int __as_dsp_set_gain(as_dsp_engine *dsp, int fd, float rxgain, float txgain)
{
	struct	as_gains g;
	float 	ltxgain;
	float 	lrxgain;
	int 		j,k;
	
	g.chan = 0;/* no use in user space*/

	/* caluculate linear value of tx gain */
	ltxgain = pow(10.0,txgain / 20.0);
	  /* caluculate linear value of rx gain */
	lrxgain = pow(10.0,rxgain / 20.0);
	for (j=0;j<256;j++) 
	{
		k = (int)(((float)FULLXLAW(j, dsp->config.law_type )) * lrxgain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.rxgain[j] = LINEAR2XLAW(k, dsp->config.law_type);
		
		k = (int)(((float)FULLXLAW(j,dsp->config.law_type)) * ltxgain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.txgain[j] = LINEAR2XLAW(k, dsp->config.law_type);
	}
		
	return(ioctl(fd, AS_CTL_SETGAINS, &g));
}


void  as_dsp_set_gain(int fd, int law, float rx_gain_db, float tx_gain_gb )
{
	as_dsp_engine *dsp;
	int res;
	
	dsp = as_dsp_init_dsp(fd, law);

	res = __as_dsp_set_gain(dsp, fd, rx_gain_db, tx_gain_gb);
	as_dsp_detsory_dsp( dsp);
}

