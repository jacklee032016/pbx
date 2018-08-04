/*
* $Id: as_record.h,v 1.6 2007/03/27 11:15:52 lizhijie Exp $
*/

#ifndef	__AS_RECORD_H__ 
#define __AS_RECORD_H__
/* Interface */

#define VOICE_RATE						160

#define ERR_UNABLE						-1
#define ERR_SUCCESS						0  // 0 successful
#define ERR_OPEN_F						1  //  open file error
#define ERR_OPEN_D						2
#define ERR_OPEN_C						3
#define ERR_REGIST						4 //  register signal error

#define ERR_OVERTIME					0xFF

#define	PSTN_DEVICE_HOME_DIR			"/dev/astel/"

#define	ALERT_VOICE_HOME_DIR			"/usr/pbx/Tone/"
#define	ALERT_START_VOICE				ALERT_VOICE_HOME_DIR"begin.u"
#define	ALERT_END_VOICE				ALERT_VOICE_HOME_DIR"end.u"

#define	RECORD_CONFIG_FILE			"/etc/pbx/record.conf"


typedef	enum	
{
	RECORD_STATE_WAIT_OFFHOOK, 
	RECORD_STATE_WORKING,
	RECORD_STATE_RECORDING,
	RECORD_STATE_ENDING
}record_state_t;

typedef	enum	
{
	RECORD_TYPE_PCM, 
	RECORD_TYPE_SLIC
}record_type_t;

typedef	enum	
{
	RECORD_ACTION_RECORD, 
	RECORD_ACTION_PLAY
}record_action_t;

typedef	enum	
{
	RECORD_APP_MSQ, 
	RECORD_APP_STANDALONE
}record_app_t;

typedef	enum	
{
	RECORD_MSG_OFFHOOK_TIMEOUT, 
	RECORD_MSG_BEGIN,		/* begin of play or record */
	RECORD_MSG_END		/* end of play fo record */
}record_msg_t;


typedef	struct
{
	record_app_t		app;
	
	record_type_t		type;
	record_action_t	action;		/* 0 : record; 1 : play */

	record_state_t		state;

	int				timeLength;		/* how long voice data file recorded */

	int				devIndex;	/* static determined in SLIC device; get from UARTD cmd when PCM */
	int				waitTime;

	char				fileName[256];
}RECORD_T;



int	record_wait_offhook(RECORD_T *record);
int	record_start_work(RECORD_T *record);
void	record_clean_all(RECORD_T *record);

int	record_msg_cmd(record_msg_t msgType);
PBX_COMMAND *record_wait_msg();
int	as_record_time_handler(void *data);

int	as_record_init(RECORD_T *record);

#define	RECORD_EVENT_TIMEOUT()		\
	record_msg_cmd(RECORD_MSG_OFFHOOK_TIMEOUT)
#define	RECORD_EVENT_WORKS_BEGIN()	\
	record_msg_cmd(RECORD_MSG_BEGIN)
#define	RECORD_EVENT_WORKS_END()	\
	record_msg_cmd(RECORD_MSG_END)


#endif

