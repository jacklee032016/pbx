/*
*$Id: api.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __API_H__
#define __API_H__

#include "global.h"
#include <pthread.h>

#ifdef  __cplusplus
	extern "C"
	{
#endif



typedef enum
{ 
	API_TYPE_MSG,//common msg
	API_TYPE_RES, //result
	API_TYPE_QUE, //query
	API_TYPE_SET
}api_type_t;

typedef  enum
{
	API_CALL_SETUP = 0,
	API_CALL_SETUP_ACK,
	API_CALL_INFORMATION,
	API_CALL_PROCEEDING,
	API_CALL_ALERTING,
	API_CALL_CONNECT,
	API_CALL_CONNECT_ACK,
	API_CALL_DISCONNECT,
	API_CALL_RELEASE,
	API_CALL_RELEASE_COMPLETE,
	API_CALL_UNKNOW = 99
}api_call_method_t;

typedef struct
{
	api_call_method_t msgm;
	int peerepi;
	int devindex;
	char *epn;
	char *src;
	char *dst;
}api_struc_msg;

typedef enum
{
	API_SEND_RESULT_OK,
	API_SEND_RESULT_NO_ROUTING,
	API_SEND_RESULT_BUSY,
	API_SEND_RESULT_CALLER_CANCEL,
	API_SEND_RESULT_ERROR
}api_result_t;

typedef struct
{
	api_result_t result;
}api_struc_res;

//API msg
typedef struct
{
	api_type_t type;
	int epi;//ep index
	int seq;//seq for result check
	union
	{
		api_struc_msg c;
		api_struc_res r;
		//todo add new type
		
	} info;
	
}api_msg_t;

//msg macro
#define intf_ep_index			 epi
#define intf_type				type
#define intf_seq				seq
#define intf_peer_ep_index 	info.c.peerepi
#define intf_ep_name			 info.c.epn
#define intf_msg_method		 info.c.msgm
#define intf_from 				info.c.src
#define intf_to 				info.c.dst
#define intf_dev_index		 info.c.devindex

//result macro
#define intf_result 			info.r.result


//general funcs for upper and lower invoking

//lower funcs

//upper funcs
void api_init(char *progname);
void api_th_init(void  * ( * th) (void *));
api_result_t api_send_msg(api_call_method_t method,int ep_index,char *to);
api_msg_t * api_receive_msg();
api_result_t api_audio_start(int ep_src, int ep_dst);
api_result_t api_audio_stop(int ep_src, int ep_dst);
int api_audio_read(char *buffer, int audio_len, int ep_index);
int api_audio_write(char *buffer, int audio_len, int ep_index);


//void api_process(api_msg_t *apiMsg);


#ifdef __cplusplus
	};
#endif


#endif
