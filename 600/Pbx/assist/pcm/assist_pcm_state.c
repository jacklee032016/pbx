/*
* $Id: assist_pcm_state.c,v 1.17 2007/09/14 20:41:18 lizhijie Exp $
*/
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/msg.h>
#include "assist_globals.h"
#if __WITH_NEW_STC__
#include "uart_cmd.h"
#endif

typedef	struct
{
	int	sipCmds;		/* count of command send from SIP to UARTD */
	int	uartCmds;		/* count of command rx from UATD to SIP */

	int	sipDigits;		/* count of dialed digits from SIP to UARTD */
	int	uartDigits;		/* count of dialed digits from UARTD to SIP */
}uart_stats;

uart_stats	_uartStats;

static pthread_mutex_t mutPcm = PTHREAD_MUTEX_INITIALIZER;

static CHANNEL_INFO bufChanInfo[AS_DEVICE_MAX];
static int fds[AS_DEVICE_MAX];

static int send_pbx_cmd(char op, as_device_t *dev, char *buf )
{
	int i;
	char chanel;
	PBX_COMMAND   cmd;	
	
	cmd.op = op;
	cmd.id = 0;

	
#if __WITH_NEW_STC__
	if( cmd.op == CMD_CALL_DIALING)
#else
	if( cmd.op == START_RING_CMD)
#endif
	{
		chanel = dev->id;
	}
	else
	{
		pthread_mutex_lock(&mutPcm);
		chanel = bufChanInfo[dev->phyId].chan;
		pthread_mutex_unlock(&mutPcm);
	}
	memset(cmd.value, 0XFF, CMD_VALUE_MAX);

	cmd.value[0] = (chanel==0xFF)?0:chanel;

	AS_MSG_DEBUG(AS_LOG_INFO, "send channel is '%d'", chanel);
		
	i = 0;
	
	if(buf)
	{
		while(buf[i] != 0xFF)
		{
			cmd.value[i+1] = buf[i];
			i++;
		}
		cmd.value[i+1] = 0xFF;
	}

	AS_MSG_DEBUG(AS_LOG_INFO, "%d command send to UARTD", ++_uartStats.sipCmds );
	return as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SIP);
	
}

static void pcm_init()
{
	int i;
	PBX_COMMAND cmd;
	char deviceName[64];

	as_msg_queue_init( 0);
	
	memset(bufChanInfo, 0x00, sizeof(bufChanInfo));
	for(i=0; i<AS_DEVICE_MAX; i++)
	{
		bufChanInfo[i].state = AS_DEVICE_STATE_ONHOOK;
		bufChanInfo[i].logState = AS_DEVICE_STATE_INVALIDATE;
		bufChanInfo[i].chan = AS_DEVICE_NOT_DEFINED;
		bufChanInfo[i].iEnableDial = 0;
		
		//make the channel 
		sprintf(deviceName, "%s/%d",  AS_DEVICE_NAME, i);
		fds[i] = open(deviceName, O_RDWR);
	}
	
	cmd.id = 0;
#if __WITH_NEW_STC__
	cmd.op = CMD_CALL_RESET_ALL;
#else
	cmd.op = IP_RESET_CMD;
#endif
	cmd.value[0] = 0;
	cmd.value[1] = 0;
	cmd.value[2] = 0xFF;
	as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SIP);
	
}
static void report_offhook(unsigned char chanId, unsigned char phoneId)
{
 	int i;
	int hasUsed = 0;
	PBX_COMMAND   cmd;	
 	
	pthread_mutex_lock(&mutPcm);
	if(chanId <AS_DEVICE_MAX && bufChanInfo[phoneId].state == AS_DEVICE_STATE_ONHOOK)
	{
		//check if the chanid is used by other offhook device
		for(i = 0; i<AS_DEVICE_MAX; i++)
		{
			if(i == phoneId)
				continue;

			if(bufChanInfo[i].chan == chanId && bufChanInfo[i].state == AS_DEVICE_STATE_OFFHOOK)
			{
				hasUsed = 1;
			}
		}
		
		if(fds[chanId]>0 && !hasUsed)
		{
			bufChanInfo[phoneId].iEnableDial = 1;
			bufChanInfo[phoneId].telCode.iLen = 0 ;
			bufChanInfo[phoneId].chan = chanId;
			bufChanInfo[phoneId].state = AS_DEVICE_STATE_OFFHOOK;
			AS_MSG_DEBUG(AS_LOG_INFO, "PBX : the %dth phone OFFHOOK on PCM channel %d", phoneId, chanId );
		}
		//notify the device busy
		else
		{
			cmd.id = 0;
#if __WITH_NEW_STC__
			cmd.op = CMD_CALL_BUSY_HERE;
#else
			cmd.op = IP_BUSY_CMD;
#endif
			cmd.value[0] = chanId;
			cmd.value[1] = 0xFF;
			as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SIP);
		}
			
	}
	else
	{
		cmd.id = 0;
#if __WITH_NEW_STC__
		cmd.op = CMD_CALL_BUSY_HERE;
#else
		cmd.op = IP_BUSY_CMD;
#endif
		cmd.value[0] = chanId;
		cmd.value[1] = 0xFF;
		as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SIP);
	}
	pthread_mutex_unlock(&mutPcm);
	
}
/*
value 0 channelId
value 1 phoneId
2...n dial codes
*/
void *pcm_Thread(void *P)
{
	int i;
	char code;
	int iLen = 0;
	unsigned char buf[CMD_VALUE_MAX-3];

	PBX_COMMAND *cmd;

	AS_MSG_DEBUG(AS_LOG_INFO, "PID of PCM Thread is %d", getpid() );
	memset(&_uartStats, 0 , sizeof(uart_stats) );
		
	pcm_init();
	
	while(1)
	{
		cmd = as_cmd_sip_from_uart( );
		
		if(cmd)
		{
			AS_MSG_DEBUG(AS_LOG_INFO, "PBX PCM Thread recevied %d MSG", ++_uartStats.uartCmds);
			AS_MSG_DEBUG(AS_LOG_INFO, "channelId is %d \tphoneId is %d", cmd->value[0], cmd->value[1]);
			
			//invalid phoneid meets ,ignore it
			if(cmd->value[1]>= (AS_DEVICE_MAX/2+PSTN_DEVICES_EPOCH))
				continue;
			else if(cmd->value[1]>=AS_DEVICE_MAX/2 && cmd->value[1]<PSTN_DEVICES_EPOCH)
				continue;

			if(cmd->value[1]>=PSTN_DEVICES_EPOCH)
				cmd->value[1] = cmd->value[1]-PSTN_DEVICES_EPOCH+AS_DEVICE_MAX/2;
				
			switch(cmd->op)
			{
#if __WITH_NEW_STC__
				case CMD_CALL_DIALING:
#else
				case AS_PBX_CMD_TYPE_CALLER_OFFHOOK:
#endif					
					AS_MSG_DEBUG(AS_LOG_INFO, "PBX CALLER : the %dth phone OFFHOOK on PCM channel %d", cmd->value[1], cmd->value[0] );
					report_offhook(cmd->value[0], cmd->value[1]);				
					break;
					
#if __WITH_NEW_STC__
				case CMD_CALL_OFFHOOK:
#else
				case AS_PBX_CMD_TYPE_CALLEE_OFFHOOK:
#endif
					/* no pcm_channel eg. phone is needed when CALLEE_OFFHOOK */
					AS_MSG_DEBUG(AS_LOG_INFO, "PBX CALLEE : the %dth phone OFFHOOK on PCM channel %d", cmd->value[1], cmd->value[0] );
					report_offhook(bufChanInfo[cmd->value[1]].chan, cmd->value[1]);
					break;

				/*caller onhook*/
#if __WITH_NEW_STC__
				case CMD_CALL_ONHOOK:
#else
				case AS_PBX_CMD_TYPE_CALLER_ONHOOK:
#endif
					AS_MSG_DEBUG(AS_LOG_INFO, "PBX CALLE/CALLEE : the %dth phone ONHOOK on PCM channel %d", cmd->value[1], cmd->value[0] );
					pthread_mutex_lock(&mutPcm);
#if __WITH_NEW_STC__					
					/*Li Zhijie, 2007.09.11
					* added to support IP call to EXT and EXT flashed to other device
					*/
					for(i=0; i<AS_DEVICE_MAX; i++)
					{
						if(bufChanInfo[i].chan == cmd->value[0])
						{
							bufChanInfo[i].state = AS_DEVICE_STATE_ONHOOK;
						}
					}
#else
					bufChanInfo[cmd->value[1]].state = AS_DEVICE_STATE_ONHOOK;
#endif
					pthread_mutex_unlock(&mutPcm);
					break;

#if __WITH_NEW_STC__
				case CMD_CALL_PHONE_DIGITS:
#else
				case AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT:
#endif
					AS_MSG_DEBUG(AS_LOG_INFO, "dial DIGIT msg Received");
					//only when in offhook state ,dial code can be acceptable;
					iLen = 0;				

					pthread_mutex_lock(&mutPcm);
					if(bufChanInfo[cmd->value[1]].iEnableDial)
					{
						i = 2;
						while(cmd->value[i] != 0xFF && (i+1)<CMD_VALUE_MAX)
						{
							if(cmd->value[i]<10)
								code = cmd->value[i]+'0';
							else if(cmd->value[i] == 0x0A)
								code = '0';
							else if(cmd->value[i] == 11)
								code = '*';
							else	if(cmd->value[i] == 12)
								code ='#';
							else
							{
								i++;
								continue;
							}
							i++;
							buf[iLen++] = code;
						}
						
						memcpy(bufChanInfo[cmd->value[1]].telCode.buf+bufChanInfo[cmd->value[1]].telCode.iLen, buf, iLen);
						bufChanInfo[cmd->value[1]].telCode.iLen += iLen;

						buf[iLen] = '\0';
						AS_MSG_DEBUG(AS_LOG_NOTICE, "Count of %d Phone DIGIT '%s'", ++_uartStats.uartDigits, buf);
					}
					else
					{
						AS_MSG_LOG(AS_LOG_NOTICE, "%d channel is not enabled", cmd->value[1] );
					}
					pthread_mutex_unlock(&mutPcm); 
					break;	

#if __WITH_NEW_STC__
				case CMD_CALL_TRYING:
				case CMD_CALL_CHANNEL_INDEX:	
#else
				case AS_PBX_CMD_TYPE_CALLEE_SEND_PCM_CHANNEL:
#endif
					AS_MSG_DEBUG(AS_LOG_INFO, "callee send channel msg");

					pthread_mutex_lock(&mutPcm);
					//invalid chanelId meets, ignore it
					if(cmd->value[0]<AS_DEVICE_MAX && bufChanInfo[cmd->value[1]].state==AS_DEVICE_STATE_ONHOOK)
					{
						bufChanInfo[cmd->value[1]].logState = AS_DEVICE_STATE_IDLE;
						bufChanInfo[cmd->value[1]].chan = cmd->value[0];
					}		
					else
						bufChanInfo[cmd->value[1]].logState = AS_DEVICE_STATE_BUSY;
					pthread_mutex_unlock(&mutPcm);
					break;
					
#if __WITH_NEW_STC__
				case CMD_CALL_BUSY_HERE:
#else
				case AS_PBX_CMD_TYPE_CALLEE_BUSY_HERE:
#endif
					AS_MSG_DEBUG(AS_LOG_INFO, "callee busy msg");
  					pthread_mutex_lock(&mutPcm);
					//bufChanInfo[cmd->value[1]].chan = AS_DEVICE_NOT_DEFINED;
					bufChanInfo[cmd->value[1]].logState = AS_DEVICE_STATE_BUSY;
					pthread_mutex_unlock(&mutPcm);
					break;
				case AS_PBX_CMD_TYPE_PCM_RESET:
					/* this signal is not implemented in STC PBX, lizhijie, 2007.05.06*/
					AS_MSG_DEBUG(AS_LOG_INFO, "pcm reset msg");
					for(i=0; i<AS_DEVICE_MAX; i++)
					{
						bufChanInfo[i].state = AS_DEVICE_STATE_ONHOOK;
						bufChanInfo[i].logState = AS_DEVICE_STATE_ONHOOK;
						//bufChanInfo[i].chan = AS_DEVICE_NOT_DEFINED;
					}
					break;
					
#if __WITH_NEW_STC__
				case CMD_CALL_CANCEL:
					AS_MSG_LOG(AS_LOG_INFO, "CANCEL signal is not implemented in IP side");
					break;
				case CMD_CALL_CALLERID_DIGITS:
					AS_MSG_LOG(AS_LOG_INFO, "CALLER_DIGITS signal is not implemented in IP side");
					break;
					
#endif
				default:
					AS_MSG_LOG(AS_LOG_WARNING, "rcv msg Without corresponding handler");
			
			}
			
			free(cmd);
		}
#if WITH_MSG_QUEUE_NOWAIT
		/* message queue is implemented as blocked, lizhijie, 2007.03.20 */
		usleep(5000); 
#endif
	}
}


/* send IP caller phone number to PBX */
int pcm_dev_dial_code(as_device_t *dev, const char *callerId)
{
	int i = 0;
	int iCount = 0;
	char code;
	char buf[CMD_VALUE_MAX-1];

	buf[0] = dev->id;	/*phone id*/
	buf[1] = 0;	/*number counts*/

	//value 0 channel id
	//value 1 phone id
	//value 2 number counts 
	//value 3...N
	//value N+1 0xFF
	if(callerId[0] == '0' )
		i = 1;
	
	while(callerId[i])
	{
		code = callerId[i++];

		AS_MSG_DEBUG(AS_LOG_INFO, "%d Dialed Digits '%c' send to UART", ++_uartStats.sipDigits, code);
		if(code > '0' && code<='9')
			code = code-'0';
		else if(code == '*')
			code = 11;
		else	if(code == '#')
			code = 12;
		else	if(code == '0')
			code = 0x0a;
		else	
			continue;

		if(iCount%2)
			buf[2+(iCount/2)] = (buf[2+(iCount/2)]&0xF0) |code;
		else
			buf[2+(iCount/2)] = (code<<4)|0x0F;
		
		iCount++;

		if(iCount%2)
		{
			if((3+(iCount/2))>=(CMD_VALUE_MAX-1))
				break;
		}
		else
		{
			if((2+(iCount/2))>=(CMD_VALUE_MAX-1))
				break;	
		}
	}	

	buf[1] = iCount;

	if(iCount%2)
	{
		buf[3+(iCount/2)] = 0xFF;
	}
	else
	{
		buf[2+(iCount/2)] = 0xFF;
	}
	
//	AS_MSG_LOG(AS_LOG_INFO, "pcm dial CallerId '%c'", buf+'0' );
#if __WITH_NEW_STC__
	return send_pbx_cmd(CMD_CALL_CALLERID_DIGITS, dev, buf);
#else
	return send_pbx_cmd(DIAL_CODE_CMD, dev, buf);
#endif
}

int pcm_ip_onhook(as_device_t *dev )
{
	AS_MSG_DEBUG(AS_LOG_INFO, "pcm ip ONHOOK");
#if __WITH_NEW_STC__
	return send_pbx_cmd(CMD_CALL_ONHOOK, dev, NULL) ;
#else
	return send_pbx_cmd(IP_ONHOOK_CMD, dev, NULL) ;
#endif
}

int pcm_dev_tone_dial_play(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Note PBX to play dial");
	AS_MSG_DEBUG(AS_LOG_INFO, "No Operation is needed when prompt PBX send out dial tone");
	return 0;
}

int pcm_dev_tone_dial_stop(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Note PBX to stop dial");
	AS_MSG_DEBUG(AS_LOG_INFO, "No Operation is needed when prompt PBX to stop dial tone");
	return 0;
}

int pcm_dev_tone_ringback_play(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Note PBX to play ringback");
	AS_MSG_DEBUG(AS_LOG_INFO, "pcm ip idle(0X14), used to note PBX begin to ringback");
	
	//when caller listens callback audio, dial is disabled
	pthread_mutex_lock(&mutPcm);
	bufChanInfo[dev->phyId].iEnableDial = 0;
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	pthread_mutex_unlock(&mutPcm);	

#if __WITH_NEW_STC__
	return send_pbx_cmd(CMD_CALL_TRYING, dev, NULL);
#else
	return send_pbx_cmd(IP_IDLE_CMD, dev, NULL);
#endif
}

int pcm_dev_tone_ringback_stop(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Note PBX to stop ringback and open voice channel");
#if __WITH_NEW_STC__
#if 1
	/* no operation is need, lizhijie, 2007.06.19 */
	return send_pbx_cmd(CMD_CALL_OFFHOOK, dev, NULL);
#else
	return 0;
#endif
#else
	return send_pbx_cmd(OPEN_CHANEL_CMD, dev, NULL);
#endif
}

int pcm_dev_tone_busy_play(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Note PBX to play busy");
	AS_MSG_DEBUG(AS_LOG_INFO, "pcm ip busy(0X13): Note PBX busy here(Phone number is not exist ot others)");
	pthread_mutex_lock(&mutPcm);
	bufChanInfo[dev->phyId].state = AS_DEVICE_STATE_ONHOOK;
	//when caller listens busy audio, dial is disabled
	bufChanInfo[dev->phyId].iEnableDial = 0;
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	pthread_mutex_unlock(&mutPcm);	

	pthread_mutex_unlock(&mutPcm);
#if __WITH_NEW_STC__
	return send_pbx_cmd(CMD_CALL_ONHOOK, dev, NULL) ;
#else
	return send_pbx_cmd(IP_BUSY_CMD, dev, NULL) ;
#endif
}

int pcm_dev_tone_fastbusy_play(as_device_t *dev)
{
	pthread_mutex_lock(&mutPcm);
	bufChanInfo[dev->phyId].state = AS_DEVICE_STATE_ONHOOK;
	//when caller listens busy audio, dial is disabled
	bufChanInfo[dev->phyId].iEnableDial = 0;
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	pthread_mutex_unlock(&mutPcm);
	AS_MSG_DEBUG(AS_LOG_INFO, "pcm tone dev play fastbusy");
#if __WITH_NEW_STC__
	return send_pbx_cmd(CMD_CALL_ONHOOK, dev, NULL) ;
#else
	return send_pbx_cmd(IP_BUSY_CMD, dev, NULL) ;
#endif
}

int pcm_dev_tone_callwait_play(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "pcm tone dev play callwait");

	pthread_mutex_lock(&mutPcm);
	//when caller listens call wait, dial is disabled
	bufChanInfo[dev->phyId].iEnableDial = 0;
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	pthread_mutex_unlock(&mutPcm);
	return 0;
}

int pcm_dev_tone_play_stop(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Not PBX to stop tone");
	return 0;
}

unsigned char  pcm_dev_dtmf_get_digit(as_device_t *dev)
{
	unsigned char code = 0xFF;
	
	pthread_mutex_lock(&mutPcm);
	if(bufChanInfo[dev->phyId].telCode.iLen)
	{
		code = bufChanInfo[dev->phyId].telCode.buf[0];
		bufChanInfo[dev->phyId].telCode.iLen--;
		memmove(bufChanInfo[dev->phyId].telCode.buf,  \
		bufChanInfo[dev->phyId].telCode.buf+1,  \
		bufChanInfo[dev->phyId].telCode.iLen); 
	}
	pthread_mutex_unlock(&mutPcm);	

	return code;
}

as_state_t pcm_dev_check_status(as_device_t *dev )
{
	as_state_t state;

	pthread_mutex_lock(&mutPcm);
	state = bufChanInfo[dev->phyId].state;
	pthread_mutex_unlock(&mutPcm);
	return state;
		
}

as_state_t pcm_dev_get_status(as_device_t *dev )
{
	as_state_t state;

	pthread_mutex_lock(&mutPcm);
	state = bufChanInfo[dev->phyId].logState;
	if(state != AS_DEVICE_STATE_INVALIDATE)
		bufChanInfo[dev->phyId].logState = AS_DEVICE_STATE_INVALIDATE;
	pthread_mutex_unlock(&mutPcm);
	return state;
		
}

int  pcm_dev_query_status(as_device_t *dev)
{
	/*onhook means idle, offhook means busy*/
	AS_MSG_DEBUG(AS_LOG_INFO, "send ring cmd to PBX querying status of phone");
#if __WITH_NEW_STC__
	send_pbx_cmd(CMD_CALL_DIALING, dev, NULL);
#else
	send_pbx_cmd(START_RING_CMD, dev, NULL);
#endif	
	return 1;

}

int pcm_dev_trans_telnumber(as_device_t *dev, const char *bufTelNumber)
{
	int i = 0;
	int iCount = 0;
	char code;
	char buf[CMD_VALUE_MAX-1];

	buf[0] = dev->id;	/*phone id*/
	buf[1] = 0;	/*number counts*/

	//value 0 channel id
	//value 1 phone id
	//value 2 number counts 
	//value 3...N
	//value N+1 0xFF
	if(bufTelNumber[0] == '0' )
		i = 1;
	
	while(bufTelNumber[i])
	{
		code = bufTelNumber[i++];

		AS_MSG_DEBUG(AS_LOG_INFO, "%d Dialed Digits '%c' send to UART", ++_uartStats.sipDigits, code);
		if(code > '0' && code<='9')
			code = code-'0';
		else if(code == '*')
			code = 11;
		else	if(code == '#')
			code = 12;
		else	if(code == '0')
			code = 0x0a;
		else	
			continue;

		if(iCount%2)
			buf[2+(iCount/2)] = (buf[2+(iCount/2)]&0xF0) |code;
		else
			buf[2+(iCount/2)] = (code<<4)|0x0F;
		
		iCount++;

		if(iCount%2)
		{
			if((3+(iCount/2))>=(CMD_VALUE_MAX-1))
				break;
		}
		else
		{
			if((2+(iCount/2))>=(CMD_VALUE_MAX-1))
				break;	
		}
	}	

	buf[1] = iCount;

	if(iCount%2)
	{
		buf[3+(iCount/2)] = 0xFF;
	}
	else
	{
		buf[2+(iCount/2)] = 0xFF;
	}
	
#if __WITH_NEW_STC__
	send_pbx_cmd(CMD_CALL_PHONE_DIGITS, dev,  buf);
#else
	send_pbx_cmd(DIAL_CODE_CMD, dev,  buf);
#endif
	return 1;
	
}
int pcm_dev_ring_start(as_device_t *dev)
{
	AS_MSG_DEBUG(AS_LOG_INFO, "Perform ring start operation");
	//send_pbx_cmd(START_RING_CMD, dev, NULL);
	//when callee ring start, dial code is disabled
	pthread_mutex_lock(&mutPcm);
	bufChanInfo[dev->phyId].iEnableDial = 0;
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	pthread_mutex_unlock(&mutPcm);
	pcm_dev_dial_code(dev, dev->callerId );
	memset(dev->callerId, MAX_CALLERID_LENGTH, 0);

	return 1;
}

int pcm_dev_ring_stop(as_device_t *dev)
{
	as_state_t state;
	
	state = pcm_dev_check_status(dev);

	if(state != AS_DEVICE_STATE_OFFHOOK)	
	{
#if __WITH_NEW_STC__
		return send_pbx_cmd(CMD_CALL_CANCEL, dev, NULL) ;
#else
		return send_pbx_cmd(IP_CANCEL_REQUEST, dev, NULL) ;
#endif
	}

	AS_MSG_LOG(AS_LOG_WARNING, "Can not stop ring(cancel call request) on device %s, it is offhook", dev->name);
	return -1;
}


int pcm_dev_get_channel_id(as_device_t *dev)
{
	int channel = AS_DEVICE_NOT_DEFINED;

	int fd = -1;
	pthread_mutex_lock(&mutPcm);
	channel = bufChanInfo[dev->phyId].chan;
	fd = fds[channel];
	
	//when the audio channel setup, dial code is enabled 
	bufChanInfo[dev->phyId].iEnableDial = 1;	
	bufChanInfo[dev->phyId].telCode.iLen = 0;
	
	pthread_mutex_unlock(&mutPcm);	
	
	AS_MSG_DEBUG(AS_LOG_INFO, "the gotten channel for phyid %d is %d", dev->phyId, channel);
	
	return fd;
}

