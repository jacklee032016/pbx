/*
 * $Id: assist_dsp_gain.c,v 1.2 2007/09/08 18:21:40 lizhijie Exp $
*/
#include "assist_globals.h"
#include "assist_dsp.h"
#include "pbx_globals.h"
#include "math.h"

#if 0
/* gain is float in the form of dB */
int assist_dsp_set_gain(assist_dsp_t *dsp, int fd, float rxgain, float txgain)
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
		k = (int)(((float)FULLXLAW(j, dsp->law )) * lrxgain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.rxgain[j] = LINEAR2XLAW(k, dsp->law );
		
		k = (int)(((float)FULLXLAW(j,dsp->law )) * ltxgain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.txgain[j] = LINEAR2XLAW(k, dsp->law );
	}
		
	return(ioctl(fd, AS_CTL_SETGAINS, &g));
}

/* support both SLIC and PCM device */
int assist_dsp_channel_gain_setup(assist_dsp_t *dsp, int fd)
{
	float tx_db, rx_db;
	tx_db = 20.0*log10( (float)dsp->txGain );
	rx_db = 20.0*log10( (float)dsp->rxGain );

	return assist_dsp_set_gain(dsp, fd,  rx_db, tx_db);
}
#else
int assist_dsp_channel_gain_setup(assist_dsp_t *dsp, int fd)
{
	struct	as_gains g;
	int 		j,k;
	
	g.chan = 0;/* no use in user space*/

	for (j=0;j<256;j++) 
	{
		k = (int)((FULLXLAW(j, dsp->law )) * dsp->rxGain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.rxgain[j] = LINEAR2XLAW(k, dsp->law );
		
		k = (int)((FULLXLAW(j,dsp->law )) * dsp->txGain);
		if (k > 32767) 
			k = 32767;
		if (k < -32767) 
			k = -32767;
		g.txgain[j] = LINEAR2XLAW(k, dsp->law );
	}
		
	return(ioctl(fd, AS_CTL_SETGAINS, &g));
}
#endif

