/*
 * $Id: api.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "api.h"
#include "intf.hxx"

int send_seq = 0;
api_queue_t  *qreceive=NULL;


void api_th_init(void * (* th)(void *))
{
	pthread_t api_th;
	if(pthread_create(&api_th,NULL,th,NULL))
	{
		abort();
	}

}

void api_init(char *progname)
{
	qreceive = intf_queue_init(API_QUEUE_UP);
//	qsend = api_queue_init(API_QUEUE_DOWN);
		
	intf_init( progname);
	
	return;
}

//all msg-sending api in one,
//ep_index < 0 when no ep_index specified
//to == NULL when no callee number specified
//return: api_result_t
api_result_t api_send_msg(api_call_method_t method,int ep_index,char *to)
{
	api_msg_t *msg;
	
	msg = (api_msg_t *)malloc(sizeof(api_msg_t));
	assert(msg);
	memset(msg,0,sizeof(api_msg_t));

	msg->type = API_TYPE_MSG;
	
	msg->intf_msg_method = method;
	msg->intf_ep_index = ep_index;
	if(to)
	{
		msg->intf_to = (char *)malloc(64);
		assert(msg->intf_to);
		memset(msg->intf_to,0,64);
		
		strncpy(msg->intf_to,to,64-1);
	}

	msg->seq = send_seq++;
	
	return intf_read( msg);


}

//with block
api_msg_t * api_receive_msg()
{
	api_msg_t *msg;

	msg = intf_queue_get(qreceive);

	return msg;
}
api_result_t api_audio_start(int ep_src, int ep_dst)
{
	return intf_audio_start(ep_src, ep_dst);
}

api_result_t api_audio_stop(int ep_src, int ep_dst)
{
	return intf_audio_stop( ep_src,  ep_dst);
}

int api_audio_read(char *buffer, int audio_len, int ep_index)
{
	return intf_audio_read( buffer, audio_len,ep_index);
}

int api_audio_write(char *buffer, int audio_len, int ep_index)
{
	return intf_audio_write( buffer, audio_len,ep_index);
}

