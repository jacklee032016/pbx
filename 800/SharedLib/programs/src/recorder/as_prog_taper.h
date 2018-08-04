#ifndef	__AS_PROG_H__
#define  __AS_PROG_H__

#include "as_taper.h"

#define PCM_RECORDER_QEQUEST 0x19
#define PCM_RECORDER_RESPONSE 0x9A
#define PCM_RECORDER_END    0x1A

void filter_c(char *source, char *filter);
void erase(char *buf, int pos, int nums);
void parser(recorder_parameter_t*);
int  get_conf(const char*, recorder_parameter_t*);
int wait_offhook();
int wait_pcm_offhook();
int wait_slic_offhook();
int event_get(int);
int wait_pbx_response();
void clean_all();
int recorder_send_cmd(unsigned char , int);


#endif
