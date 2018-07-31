/*
*$Id: intf.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "PbxEvent.hxx"
//#include <IsdnStatusMsg.hxx>

#include "PbxAgent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"
#include "CallScheduler.hxx"
#include "CallContainer.hxx"
#include "CallStateMachine.hxx"
//#include "DigitTimeoutEvent.hxx"
#include "CallTimerEvent.hxx"
#include "PbxConfiguration.hxx"
#include "PbxStateMachine.hxx"
#include "EndPoint.hxx"
#include "CommonMsg.hxx"
#include "GatewayMgr.hxx"
#ifdef API
#include "api.h"
#include "intf.hxx"

using namespace Assist;


#ifdef  __cplusplus
	extern "C"
	{
#endif

// queues in charge of  api msgs
api_queue_t as_q_up,as_q_down;


//direction 0: to receive, 1: to send
api_queue_t * intf_queue_init(api_queue_direction direction)
{
	api_queue_t *queue = NULL;
	
	if(direction)
		queue = &as_q_down;
	else
		queue = &as_q_up;
	
	if(queue)
	{
		pthread_mutex_init(&queue->api_mutex,NULL);
		pthread_mutex_lock(&queue->api_mutex);
		queue->api_flag = 0;
		queue->api_head = 0;
		queue->api_tail = 0;
		memset(queue->api_queue,0,API_QUEUE_LEN*sizeof(api_msg_t *));
		pthread_mutex_unlock(&queue->api_mutex);
	}
	return queue;
}

api_msg_t *intf_queue_get(api_queue_t *queue)
{
	int got = 0;
	api_msg_t *msg = NULL;
	
	if(!queue)return NULL;
	while(!got)
	{
		pthread_mutex_lock(&queue->api_mutex);
		if(queue->api_flag > 0)
		{
			msg = queue->api_queue[queue->api_tail++];
			if(queue->api_tail % API_QUEUE_LEN  == 0)queue->api_tail = 0;
			queue->api_flag--;
			got = 1;
		}
		pthread_mutex_unlock(&queue->api_mutex);
		usleep(60);
	}
	return msg;
}

int intf_queue_put(api_queue_t *queue , api_msg_t *msg)
{
	
	pthread_mutex_lock(&queue->api_mutex);
	if(queue->api_flag == API_QUEUE_LEN)return -1;
	queue->api_queue[queue->api_head++] = msg;
	if(queue->api_head % API_QUEUE_LEN  == 0)queue->api_head = 0;
	queue->api_flag++;
	pthread_mutex_unlock(&queue->api_mutex);
	return 0;
}

void intf_init(char *progname)
{

	cpLogSetLabel( progname );
 
	cpLog( LOG_NOTICE, "Compiled at " __DATE__ " " __TIME__ );

	const string cfgStr = ISDNPBX_CONFIG_FILE;

	FILE *cfgFile = fopen( cfgStr.c_str(), "r");
	if ( cfgFile == 0 )
	{
		cerr << "can not open " << cfgStr << endl;
		exit( 0 );
	}
	else
	{
		fclose( cfgFile );
		PbxConfiguration::instance( cfgStr );
	}

	// if the config file has a log level, do something about it
	if(PbxConfiguration::instance()->getLogFilename() != "")
	{
		int retval = cpLogOpen(PbxConfiguration::instance()->getLogFilename().c_str());

		if(retval == 0)
		{
			cpLog(LOG_ALERT, "Could not open %s",   PbxConfiguration::instance()->getLogFilename().c_str());
		}
	}

	cpLogSetPriority( PbxConfiguration::instance()->getLogLevel() );

	if(PbxConfiguration::instance()->getLogLevel()==LOG_DEBUG)
		PbxConfiguration::instance()->show();

	PbxAgent::instance();

	PbxAgent::instance()->getScheduler()->set_queue_put(&as_q_up);
//	PbxAgent::instance()->getScheduler()->create_api_thread(&as_q_down);

//	isdn_init( "ISDN", PbxAgent::instance());

	PbxAgent::instance()->run();
	PbxAgent::instance()->join();

	return;

}

api_result_t intf_read(api_msg_t *msg)
{
	api_result_t  ret = API_SEND_RESULT_OK;
	
//	printf( "------ %s thread %d ------\n","Intf DOWN",getpid() );
//	while(1)
///	{

//		printf( "------ %s waiting msg..... ------\n", "Intf DOWN" );
//		msg = api_queue_get(&as_q_down);
//		printf( "------ %s Got it ------\n","Intf DOWN" );


//		api_process(msg);

		if(msg )
		{
			if(msg->type == API_TYPE_MSG)
			{

//					cpLog(LOG_EMERG,"--------- decode Intf DOWN msg ---------");
//					cpLog(LOG_EMERG,"method  :  %s",msg->intf_msg_method);
#if 0	
					printf("device  :  %s\n",msg->intf_dev_name);
					printf(" index  :  %d\n",msg->intf_ep_index);
					printf("  name  :  %s\n",msg->intf_ep_name);
					printf("  from  :  %s\n",msg->intf_from);
					printf("    to  :  %s\n",msg->intf_to);
					printf("--------- END ---------\n");
#endif
					//following to encapsulate call event

					ret = PbxAgent::instance()->getScheduler()->struc2Event( msg);
			}
#if 0
			else if(msg->type == API_TYPE_QUE)	
			{
#if 0
					printf("--------- decode Intf DOWN query ---------\n");
					printf("query method  :  %d\n",msg->intf_query_method);
					printf(" index  :  %d\n",msg->intf_ep_index);
					printf("--------- END ---------\n");
#endif
					if(msg->intf_ep_index < 0)msg->intf_ep_index = 0;
					Sptr <EndPoint> ep = PbxAgent::instance()->getGateway()->findEP(msg->intf_ep_index);

					if( ep != 0)
					{
						api_msg_t *apiMsg = (api_msg_t *)malloc(sizeof(api_msg_t));
						apiMsg->type = API_TYPE_RES;
						apiMsg->intf_ep_index = msg->intf_ep_index;
						Sptr <PbxState> st = ep->getState();
						if (st != 0)
							apiMsg->intf_response_state = (api_ep_state_t)st->getStateType();
						else
							apiMsg->intf_response_state = API_EP_STATE_IDLE;
						
						Sptr <AudioChannel> au = ep->getAudio();
						if (au != 0)
							apiMsg->intf_response_audio = (api_audio_state_t) au->getState();
						else
							apiMsg->intf_response_audio = API_AUDIO_STATE_IDLE;
						api_queue_put(PbxAgent::instance()->getScheduler()->get_putQ(),apiMsg);
					}
			}
			else if(msg->type == API_TYPE_SET)	
			{
					PbxAgent::instance()->getScheduler()->struc2SetAudio( msg);
			}
#endif

		//after processed, msg must be free
		intf_clear_msg(msg);
		}
		

//	}
	return ret;
}

void intf_clear_msg(api_msg_t *msg)
{
	if(msg->type == API_TYPE_MSG)
	{
//		if(msg->intf_msg_method)free(msg->intf_msg_method);
		if(msg->intf_ep_name)free(msg->intf_ep_name);
//		if(msg->intf_dev_name)free(msg->intf_dev_name);
		if(msg->intf_from)free(msg->intf_from);
		if(msg->intf_to)free(msg->intf_to);

	}

	free(msg);
}

int  intf_audio_read(char *buffer, int audio_len, int ep_index)
{
	
	Sptr <EndPoint> ep = PbxAgent::instance()->getGateway()->findEP(ep_index );

	if ( ep == 0 )
	{ 
		return -1;
	}
	if(ep->getAudio() == 0 )return -1;

	return read(ep->getAudio()->getAudioFd(),buffer,audio_len);

}

int  intf_audio_write(char *buffer, int audio_len, int ep_index)
{
	
	Sptr <EndPoint> ep = PbxAgent::instance()->getGateway()->findEP(ep_index );

	if ( ep == 0 )
	{ 
		return -1;
	}
	if(ep->getAudio() == 0 )return -1;
	if(strncmp(ep->getName(),"PSTN",4) == 0)
	{
		int len = 0, i = 0;
		for (i = 0; i < audio_len / 160; ++i)
		{
			len += write (ep->getAudio()->getAudioFd(), buffer+(160*i), 160);
		}
		return len;
	}
	return write(ep->getAudio()->getAudioFd(),buffer,audio_len);

}

api_result_t  intf_audio_start(int ep_src, int ep_dst)
{
	Sptr <EndPoint> srcEp = PbxAgent::instance()->getGateway()->findEP(ep_src );
	Sptr <EndPoint> destEp = PbxAgent::instance()->getGateway()->findEP(ep_dst );

	if (srcEp == 0 || destEp == 0 ) return API_SEND_RESULT_NO_ROUTING;
	
	Sptr <AudioChannel> srcAudio = srcEp->getAudio();
	Sptr <AudioChannel> destAudio = destEp->getAudio();
	if( srcAudio== 0 || destAudio == 0)return API_SEND_RESULT_ERROR;


	bool sisvoip = (strncmp(srcAudio->getName(), "VoipChannel", 11) == 0);
	bool disvoip = (strncmp(destAudio->getName(), "VoipChannel", 11) == 0);
	if (sisvoip && disvoip)
	{
		assert(0);
	}

	if (disvoip)
	{
		cpLog(LOG_DEBUG, "Dest AudioChannel is VoipChannel");
		destAudio->setAudioPeer(srcAudio, PEER_MASTER);
		destAudio->setState(AUDIO_STATE_ACTIVE);
		srcAudio->setAudioPeer( destAudio, PEER_SLAVE);
		srcAudio->setState(AUDIO_STATE_ACTIVE);
	}
	else
	{
		cpLog(LOG_DEBUG, "Dest is Not  VoipChannel");
		srcAudio->setAudioPeer( destAudio, PEER_MASTER);
		srcAudio->setState(AUDIO_STATE_ACTIVE);
		destAudio->setAudioPeer( srcAudio, PEER_SLAVE);
		destAudio->setState(AUDIO_STATE_ACTIVE);
	}

	return API_SEND_RESULT_OK;
}

api_result_t intf_audio_stop(int ep_src, int ep_dst)
{
	Sptr <EndPoint> srcEp = PbxAgent::instance()->getGateway()->findEP(ep_src );
	Sptr <EndPoint> destEp = PbxAgent::instance()->getGateway()->findEP(ep_dst );

	if (srcEp == 0 || destEp == 0 ) return API_SEND_RESULT_NO_ROUTING;
	
	Sptr <AudioChannel> srcAudio = srcEp->getAudio();
	Sptr <AudioChannel> destAudio = destEp->getAudio();
	if( srcAudio== 0 || destAudio == 0)return API_SEND_RESULT_ERROR;

	/* release audio and deactivate them */
#if 1
	srcAudio->releasePeer( );
	destAudio->releasePeer( );

	/* unbund now, can not execute before deactivate */
//	srcEp->setCallInfo(0);
//	destEp->setCallInfo(0);
	srcEp->setAudio(0);
	destEp->setAudio(0);
	srcAudio->setEndPoint( 0);
	destAudio->setEndPoint( 0);
#endif
	return API_SEND_RESULT_OK;
}

#ifdef __cplusplus
	};
#endif

#endif
