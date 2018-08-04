/*
 * $Author: lizhijie $
 * $Log: as_dsp_gain.c,v $
 * Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
 * PSTN test library
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.1  2004/12/20 03:27:44  lizhijie
 * add DSP engine and gain code into CVS
 *
 * $Revision: 1.1.1.1 $
*/
#include "assist_lib.h"
#include "as_lib_dsp.h"

/* gain is float in the form of dB */
int as_dsp_set_gain(as_dsp_gen_engine *dsp, int fd, float rxgain, float txgain)
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

