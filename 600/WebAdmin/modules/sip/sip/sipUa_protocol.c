/*
* $Id: sipUa_protocol.c,v 1.13 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

typedef	struct	_sip_conf_t
{
	char		key[64];
	char		type[64];
	char		value[64];
	char		rule[64];
}sipua_conf_t;

#define	SIP_SERVER_TEST			"192.168.3.112"
#define	SIP_SERVER_RELEASE		"61.175.234.13"

sipua_conf_t	_confs[] = 
{
	{SIPUA_DEVICE_NAME, 		SIPYA_TYPE_STRING, 	SIPUA_DEVICE_TYPE_PCM, ""},
	{SIPUA_USER_NAME, 			SIPYA_TYPE_STRING, 	"8999", ""},
	{SIPUA_DISPLAY_NAME, 		SIPYA_TYPE_STRING, 	"phone", ""},
	{SIPUA_PASS_WORD, 			SIPYA_TYPE_STRING, 	"test", ""},
	
	{SIPUA_LOCAL_SIP_PORT, 		SIPYA_TYPE_STRING, 	"5060", ""},
	{SIPUA_SIP_TRANSPORT, 		SIPYA_TYPE_STRING, 	SIPUA_TRANSPORT_TYPE_UDP, ""},
	{SIPUA_MIN_RTP_PORT, 		SIPYA_TYPE_STRING, 	"10000", ""},
	{SIPUA_MAX_RTP_PORT, 		SIPYA_TYPE_STRING, 	"10099", ""},

	{SIPUA_REGISTER_ON, 		SIPYA_TYPE_BOOL, 	SIPUA_VALUE_TRUE, ""},
	/* register_to and register_from refer to the local domain, which often is equal  */
	{SIPUA_REGISTER_FROM, 		SIPYA_TYPE_STRING, 	"192.168.3.9", ""},
	{SIPUA_REGISTER_TO, 		SIPYA_TYPE_STRING, 	"192.168.3.9", ""},
	{SIPUA_REGISTER_SERVER, 	SIPYA_TYPE_STRING, 	SIP_SERVER_TEST, ""},
	{SIPUA_REGISTER_PORT, 		SIPYA_TYPE_STRING, 	"9060", ""},
	{SIPUA_REGISTER_EXPIRE, 		SIPYA_TYPE_INT, 		"3600", ""},
	{SIPUA_REGISTER_ACTION, 	SIPYA_TYPE_STRING, 	"proxy", ""},
	
	{SIPUA_PROXY_SERVER, 		SIPYA_TYPE_STRING, 	SIP_SERVER_TEST, ""},
	{SIPUA_PROXY_PORT, 			SIPYA_TYPE_STRING, 	"9060", ""},
	{SIPUA_AUTHENTICATION, 		SIPYA_TYPE_STRING, 	"Off", ""},
	{SIPUA_LOG_FILE_NAME, 		SIPYA_TYPE_STRING, 	"/tmp/log/pbx.log", ""},
	
	{SIPUA_START_CDR_MODE, 	SIPYA_TYPE_INT, 		"0", ""},
	
	{SIPUA_TIMEOUT_INIT, 		SIPYA_TYPE_INT, 		"28000", ""},
	{SIPUA_TIMEOUT_INTER, 		SIPYA_TYPE_INT, 		"28000", ""},
	
	{SIPUA_PSTN_COUNT, 			SIPYA_TYPE_INT, 		"4", ""},
	{SIPUA_CONSOLE_COUNT, 		SIPYA_TYPE_INT, 		"2", ""},
	{SIPUA_CONSOLE_PHONE_NUMBER, 		SIPYA_TYPE_STRING, 		"99", ""},
	{SIPUA_LOG_LEVEL, 			SIPYA_TYPE_INT, 		"4", ""},
	
	{SIPUA_MAX_DROP_TIME, 		SIPYA_TYPE_INT, 		"15", ""},
	{SIPUA_WATCH_DOG_TIME, 	SIPYA_TYPE_INT, 		"15", ""},
	{SIPUA_JITTER_BUFFER, 		SIPYA_TYPE_INT, 		"1", ""},	/* close */
	{SIPUA_UA_TIMEOUT, 			SIPYA_TYPE_STRING, 	"180", ""},
	{SIPUA_ANSWER_START_TIME, 	SIPYA_TYPE_INT, 		"8000", ""},

	/* no answer transfer branch */
	{SIPUA_LOCAL_EXCHANGE, 		SIPYA_TYPE_STRING, 	"80", ""},
	{SIPUA_VAD_START, 			SIPYA_TYPE_BOOL, 	SIPUA_VALUE_FALSE, ""},
	{SIPUA_CNG_START, 			SIPYA_TYPE_BOOL, 	SIPUA_VALUE_FALSE, ""},

	{SIPUA_LOCAL_DEVICE, 		SIPYA_TYPE_STRING, 	"60"	, ""},
	{SIPUA_LOCAL_COUNTRY, 		SIPYA_TYPE_STRING, 	""	, ""},
	{SIPUA_LOCAL_DISTRICT, 		SIPYA_TYPE_STRING, 	""	, ""},
	{SIPUA_LOCAL_BRANCH, 		SIPYA_TYPE_STRING, 	"0"	, ""}
#if 0	
	{, SIPYA_TYPE_STRING, "", ""},
	{, SIPYA_TYPE_STRING, "", ""},
	{, SIPYA_TYPE_STRING, "", ""},
	{, SIPYA_TYPE_STRING, "", ""},
	{, SIPYA_TYPE_STRING, "", ""},
	{, SIPYA_TYPE_STRING, "", ""},
#endif	
};


int sipUa_Default(llist *globalCfgs, llist *cgiVariable)
{
	int	i;
	llist	_defaults;

	list_create(&_defaults);

	for(i=0; i< sizeof(_confs)/sizeof(_confs[0]); i++)
	{
		list_append_raw(&_defaults, _confs[i].key, _confs[i].value, _confs[i].type, _confs[i].rule);
	}

	list_replace_with_new_list(globalCfgs, &_defaults);
	
//	return sipUa_write_config_file(globalCfgs, SIPUA_OP_PROTOCOL);
	return sipUa_write_config_file(globalCfgs, GET_VALUE(cgiVariable, CGI_KEYWORD_OPERATOR) );
}

static llist  	protocolCfgs;

static char *__sipUa_protocol_status()
{
	char 	buf[8192*5];
	int 		length = 0;
	char		target[128];
	int		isTrue = 0;
	int		log = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_PROTOCOL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE);

/* Protocol Config */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext(CGI_STR_BASIC_CONFIG) );

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("appointed the port of SIP used"),gettext("Local SIP Port"), SIPUA_LOCAL_SIP_PORT, GET_VALUE(&protocolCfgs,SIPUA_LOCAL_SIP_PORT) );
	isTrue = strcasecmp(GET_VALUE(&protocolCfgs, SIPUA_SIP_TRANSPORT), SIPUA_TRANSPORT_TYPE_TCP);
	length += CGI_SPRINTF(buf, length, "<TD  align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("Transport Protcol"), SIPUA_SIP_TRANSPORT );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		SIPUA_TRANSPORT_TYPE_TCP, (isTrue==0)?"selected":"", SIPUA_TRANSPORT_TYPE_TCP );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		SIPUA_TRANSPORT_TYPE_UDP,(isTrue==0)?"":"selected", SIPUA_TRANSPORT_TYPE_UDP);
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("RTP"),gettext("Min RTP Port"), SIPUA_MIN_RTP_PORT, GET_VALUE(&protocolCfgs,SIPUA_MIN_RTP_PORT) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("RTP"),gettext("Max RTP Port"), SIPUA_MAX_RTP_PORT, GET_VALUE(&protocolCfgs,SIPUA_MAX_RTP_PORT) );

	isTrue = (strcasecmp(GET_VALUE(&protocolCfgs, SIPUA_VAD_START), SIPUA_VAD_ENABLE)==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD>\r\n", 
		gettext("VAD Mode"), SIPUA_VAD_START, SIPUA_VAD_ENABLE, (isTrue==1)?"checked":"", gettext(CGI_STR_ON),SIPUA_VAD_START, SIPUA_VAD_DISABLE, (isTrue==1)?"":"checked", gettext(CGI_STR_OFF) );

	isTrue = (strcasecmp(GET_VALUE(&protocolCfgs, SIPUA_CNG_START), SIPUA_CNG_ENABLE)==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext("CNG Mode"), SIPUA_CNG_START, SIPUA_CNG_ENABLE, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), SIPUA_CNG_START, SIPUA_CNG_DISABLE, (isTrue==1)?"":"checked", gettext(CGI_STR_OFF));

	isTrue = (strcasecmp(GET_VALUE(&protocolCfgs, SIPUA_JITTER_BUFFER), "1")==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s</TD>\r\n", 
		gettext("Jitter Buffer"), SIPUA_JITTER_BUFFER, SIPUA_JITTER_BUFFER_ENABLE, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), SIPUA_JITTER_BUFFER, SIPUA_JITTER_BUFFER_DISABLE,  (isTrue==1)?"":"checked", gettext(CGI_STR_OFF) );

	log = atoi(GET_VALUE(&protocolCfgs, SIPUA_START_CDR_MODE) );
	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("CDR Start Mode"), SIPUA_START_CDR_MODE );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		SIPUA_CDR_NOT_STARTUP, (log==SIPUA_CDR_NOT_STARTUP)?"selected":"", gettext("Not Start") );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		SIPUA_CDR_MANUAL_STARTUP, (log==SIPUA_CDR_MANUAL_STARTUP)?"selected":"", gettext("Manual"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		SIPUA_CDR_AUTO_STATUP, (log==SIPUA_CDR_AUTO_STATUP)?"selected":"", gettext(CGI_STR_AUTO));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

/* Misc Config*/
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Other Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Log File address"),gettext(CGI_STR_LOG_FILE), SIPUA_LOG_FILE_NAME, GET_VALUE(&protocolCfgs,SIPUA_LOG_FILE_NAME) );

	log = atoi(GET_VALUE(&protocolCfgs, SIPUA_LOG_LEVEL) );
	length += CGI_SPRINTF(buf, length, "<TD  align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_LOG_LEVEL), SIPUA_LOG_LEVEL);
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		0, (log==0)?"selected":"", gettext("Emerg") );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		2, (log==2)?"selected":"", gettext("Critical"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		3, (log==3)?"selected":"", gettext(CGI_STR_ERROR));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		4, (log==4)?"selected":"", gettext(CGI_STR_WARN));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		7, (log==7)?"selected":"", gettext("Debug"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
		8, (log==8)?"selected":"", gettext("Stack"));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD>\r\n", 
		gettext("How long to Stater busy units:ms"),gettext("Offhook Timeout"), SIPUA_TIMEOUT_INIT, GET_VALUE(&protocolCfgs,SIPUA_TIMEOUT_INIT), gettext(CGI_STR_MSECONDS) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("the overtime of two border upon number units:ms"),gettext("Dial Timeout"), SIPUA_TIMEOUT_INTER, GET_VALUE(&protocolCfgs,SIPUA_TIMEOUT_INTER), gettext(CGI_STR_MSECONDS) );


	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD>\r\n", 
		gettext("Fault Timeout"), SIPUA_MAX_DROP_TIME, GET_VALUE(&protocolCfgs,SIPUA_MAX_DROP_TIME), gettext(CGI_STR_SECONDS) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("WatchDog Timeout"), SIPUA_WATCH_DOG_TIME, GET_VALUE(&protocolCfgs,SIPUA_WATCH_DOG_TIME), gettext(CGI_STR_SECONDS) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD>\r\n", 
		gettext("Request Timeout"), SIPUA_UA_TIMEOUT, GET_VALUE(&protocolCfgs,SIPUA_UA_TIMEOUT), gettext(CGI_STR_SECONDS) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("Answer Time"), SIPUA_ANSWER_START_TIME, GET_VALUE(&protocolCfgs,SIPUA_ANSWER_START_TIME), gettext(CGI_STR_MSECONDS) );
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );


	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	sprintf(target, "%s?%s=%s&%s=%s",WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_PROTOCOL, CGI_ACTION, CGI_ACTION_DEFAULT);
	length += CGI_SPRINTF(buf,length, "\t%s\t%s</TD></TR></FORM>\r\n", cgi_button(gettext(CGI_STR_RESTORE_DEFAULT), target) , cgi_help_button(CGI_HELP_MSG_SIPUA_PROTOCOL) );

	return strdup(buf);
}


int	sipUa_protocol(llist *cgiVariables)
{
	char		*action;

	list_create(&protocolCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &protocolCfgs) != 0)
	{
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext(CGI_STR_BASIC_CONFIG), gettext("Please select action which you want to"), __sipUa_protocol_status() );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_SAVE)  )
	{
		//char		target[256];
		
		list_replace_with_new_list(&protocolCfgs, cgiVariables );
#if 1
		return sipUa_write_config_file(&protocolCfgs, SIPUA_OP_PROTOCOL);
#else
		writeconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &protocolCfgs);
		
		sprintf(target, "%s?%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_PROTOCOL );

		cgi_refresh_page(1, target, gettext("Writing SIP UA Configuration File"), gettext(CGI_STR_PLEASE_WAIT) );
		return 0;
#endif		
	}
	else if(!strcasecmp(action, CGI_ACTION_DEFAULT)  )
	{
		sipUa_Default( &protocolCfgs, cgiVariables );
		
		return 0;
	}

	return 0;
}


