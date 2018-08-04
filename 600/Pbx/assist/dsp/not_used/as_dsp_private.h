/*
 * $Id: as_dsp_private.h,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#ifndef  __AS_DSP_PRIVATE_H__
#define __AS_DSP_PRIVATE_H__


float as_dsp_get_localtone(as_dsp_engine *dsp, float ddr,float ddi);
float as_dsp_get_localtone_df(as_dsp_engine *dsp, float ddr,float ddi,float ddr1, float ddi1);
int as_dsp_put_audio_sample(as_dsp_engine *dsp, int fd,float y,int flag);
void as_dsp_coeff_reset(as_dsp_engine *dsp);
int as_dsp_arbtones(as_dsp_engine *dsp, int fd , float f0 , float f1 , int len ,int flag );
int as_dsp_finish_audio(as_dsp_engine *dsp, int fd,int flag);
int as_dsp_silence(as_dsp_engine *engine, int fd ,int len );

#endif

