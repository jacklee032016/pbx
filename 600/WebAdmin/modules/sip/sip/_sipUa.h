/*
* $Id: _sipUa.h,v 1.10 2007/09/03 21:35:49 lizhijie Exp $
*/
#ifndef	___SIP_UA_H__
#define	___SIP_UA_H__

#include "sipUa.h"

#define	SIP_DIALPLAN_ADD							0

/* keyword in config file of /usr/etc/ua.cfg */
#define	SIPUA_DEVICE_NAME							"Device_Name"
#define	SIPUA_PSTN_COUNT							"Pstn_Count"
#define	SIPUA_PSTN_HW_STATUS					"Pstn_Hw_Status"

#define	SIPUA_USER_NAME							"User_Name"
#define	SIPUA_DISPLAY_NAME						"Display_Name"
#define	SIPUA_PASS_WORD							"Pass_Word"
#define	SIPUA_LOCAL_SIP_PORT						"Local_SIP_Port"
#define	SIPUA_SIP_TRANSPORT						"SIP_Transport"

#define	SIPUA_REGISTER_ON							"Register_On"
#define	SIPUA_REGISTER_FROM						"Register_From"
#define	SIPUA_REGISTER_TO							"Register_To"
#define	SIPUA_REGISTER_SERVER						"Register_Server"
#define	SIPUA_REGISTER_PORT						"Register_Port"
#define	SIPUA_REGISTER_EXPIRE						"Register_Expires"
#define	SIPUA_REGISTER_ACTION					"Register_Action"

#define	SIPUA_LOCAL_DEVICE						"Local_Device"
#define	SIPUA_AUTHENTICATION						"Authentication"
#define	SIPUA_PROXY_SERVER						"Proxy_Server"
#define	SIPUA_PROXY_PORT							"Proxy_Port"
#define	SIPUA_MIN_RTP_PORT						"Min_RTP_Port"
#define	SIPUA_MAX_RTP_PORT						"Max_RTP_Port"

#define	SIPUA_LOG_FILE_NAME						"LogFilename"
#define	SIPUA_START_CDR_MODE						"Start_CdrServer"

#define	SIPUA_TIMEOUT_INIT						"Initial_Digit_Timeout"
#define	SIPUA_TIMEOUT_INTER						"Inter_Digit_Timeout"

#define	SIPUA_CONSOLE_PHONE_NUMBER				"IP_Console_Phone"
#define	SIPUA_CONSOLE_COUNT						"Pbx_Count"
#define	SIPUA_LOG_LEVEL							"Log_Level"

#define	SIPUA_MAX_DROP_TIME						"Max_DropTime"
#define	SIPUA_WATCH_DOG_TIME					"WatchDog_Time"

#define	SIPUA_JITTER_BUFFER						"JitterBuffer"
#define	SIPUA_UA_TIMEOUT							"Ua_Timeout"

#define	SIPUA_ANSWER_START_TIME					"AnswerRestartTimer"
#define	SIPUA_LOCAL_EXCHANGE						"Local_Exchange"			/* transfer call to this branch autoly */

#define	SIPUA_VAD_START							"VadString"
#define	SIPUA_CNG_START							"CngString"

#define	SIPUA_LOCAL_COUNTRY						"Local_Country"
#define	SIPUA_LOCAL_DISTRICT						"Local_District"

#define	SIPUA_LOCAL_BRANCH						"Local_Branch"

#define	SIPUA_G711_LAW							"G711_Law"
#define	SIPUA_TX_GAIN								"Gain_TX"
#define	SIPUA_RX_GAIN								"Gain_RX"

/* value for the keyword */
#define	SIPUA_DEVICE_TYPE_PCM					"PCM"
#define	SIPUA_DEVICE_TYPE_SLIC					"SLIC"

#define	SIPUA_TRANSPORT_TYPE_UDP				"UDP"
#define	SIPUA_TRANSPORT_TYPE_TCP					"TCP"

#define	SIPUA_AUTHEN_TYPE_LOCAL					"Local"
#define	SIPUA_AUTHEN_TYPE_OFF					"Off"

#define	SIPUA_REGISTER_ACTION_REDIRECT			"redirect"
#define	SIPUA_REGISTER_ACTION_PROXY				"proxy"


#define	SIPUA_CDR_NOT_STARTUP					0
#define	SIPUA_CDR_MANUAL_STARTUP				1
#define	SIPUA_CDR_AUTO_STATUP					2

#define	SIPUA_JITTER_BUFFER_ENABLE				1
#define	SIPUA_JITTER_BUFFER_DISABLE				-1

#define	SIPUA_VAD_ENABLE							"True"
#define	SIPUA_VAD_DISABLE							"False"

#define	SIPUA_CNG_ENABLE							"True"
#define	SIPUA_CNG_DISABLE							"False"

#define	SIPUA_VALUE_TRUE							"True"
#define	SIPUA_VALUE_FALSE							"False"

#define	SIPYA_TYPE_STRING							"string"
#define	SIPYA_TYPE_BOOL							"bool"
#define	SIPYA_TYPE_INT								"int"

#define	DIAL_PARTTERN_STR_DREICT			_("Direct Dial")
#define	DIAL_PARTTERN_STR_SERVER			_("Server Dial")

int	sipUa_protocol(llist *cgiVariables);
int	sipUa_dial_parttern(llist *cgiVariables);
int	sipUa_dial_rules(llist *cgiVariables, char *cmdKeyword);
int	sipUa_disturb_list(llist *cgiVariables );
int	sipUa_backlist_mgmt(llist *cgiVariables);

/* update Dial_Pattern and other Dial Rules defined ua.cfg */
int sipUa_update_dial_rules(llist *rawCfgs, llist *cgiVariables);
int sipUa_write_config_file(llist *uaCfgs, char *operator);

int	sipUa_Server(llist *cgiVariables);
int sipUa_Default(llist *globalCfgs, llist *cgiVariable);
int	sipUa_global(llist *cgiVariables);

int	sipUa_PstnStatus(llist *cgiVariables);

int	sipUa_gain_control(llist *cgiVariables);

#endif

