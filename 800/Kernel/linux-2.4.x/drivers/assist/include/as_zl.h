/*
 * Interface used by other driver module, eg. Tiger driver, Li Zhijie 2005.05.21
 * $Log: as_zl.h,v $
 * Revision 1.1.1.1  2006/11/30 16:27:58  lizhijie
 * AS800 Kernel
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.1  2005/05/26 05:09:41  lizhijie
 * add zarlink 5023x driver into CVS
 *
 * $Id: as_zl.h,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
*/
#ifndef  __AS_ZL_H__
#define __AS_ZL_H__
/* only symbol export by zarlink driver is declared here */
void zl_ch_bypass_enable(unsigned int chan_no);
void zl_ch_bypass_disable(unsigned int chan_no);
void zl_ch_clear_coeffs(unsigned int chan_no);


void zl_ch_mute_receive(unsigned int chan_no);
void zl_ch_on_receive(unsigned int chan_no);
void zl_ch_mute_send(unsigned int chan_no);
void zl_ch_on_send(unsigned int chan_no);

#endif


