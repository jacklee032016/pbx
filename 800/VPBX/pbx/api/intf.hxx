/*
*$Id: intf.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __INTF_H__
#define __INTF_H__

#include "api.h"

#ifdef API
#ifdef  __cplusplus
	extern "C"
	{
#endif

#define API_QUEUE_LEN	128

typedef enum
{
	API_QUEUE_UP = 0,
	API_QUEUE_DOWN = 1
}api_queue_direction;

//API msg queue
typedef struct
{
	pthread_mutex_t api_mutex;
	int api_flag;
	int api_head;//for put
	int api_tail; //for get
	api_msg_t *api_queue[API_QUEUE_LEN];
}api_queue_t;

//lower funcs
void intf_init(char *progname);
api_queue_t  * intf_queue_init(api_queue_direction direction);
api_msg_t  *intf_queue_get(api_queue_t *queue);
int intf_queue_put(api_queue_t *queue,api_msg_t *msg);

api_result_t intf_read(api_msg_t *msg);
void intf_clear_msg(api_msg_t *msg);
int  intf_audio_read(char *buffer, int audio_len, int ep_index);
int  intf_audio_write(char *buffer, int audio_len, int ep_index);
api_result_t  intf_audio_start(int ep_src, int ep_dst);
api_result_t intf_audio_stop(int ep_src, int ep_dst);


#ifdef __cplusplus
	};
#endif

#ifdef __cplusplus
#endif

#endif

#endif
