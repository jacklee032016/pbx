/*
* $Author: lizhijie $
* $Id: sipUa.h,v 1.12 2007/09/03 21:35:49 lizhijie Exp $
*/

#ifndef __SIP_UA_H__
#define __SIP_UA_H__

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include	<sys/sem.h>
#include "cgiLib.h"
#include "cgi_llist.h"
#include "sigdef.h"

/* top level menu for SIP UA */
#define		SIPUA_OP_PROTOCOL				"protocol"
#define		SIPUA_OP_SERVER				_("SipServer") /* config Proxy and Register Server */
#define		SIPUA_OP_BLACKLIST_MGMT		"back_list"	/* view, add and delete BlackList */
#define		SIPUA_OP_BLACKLIST			_("blackList")
#define		SIPUA_OP_NO_DISTURB_FUNC	_("NoDisturbF")

#define		SIPUA_OP_GAIN_CONTROL		"GainControl"

#define		SIPUA_OP_IP_DEVICE			"IP_Device"

/* dial rule of SIP UA */
#define		SIPUA_OP_DIAL_PATTERN		"Dial_Pattern"
#define		SIPUA_OP_SPEED_DIAL			"Speed_Dial"
#define		SIPUA_OP_LOCAL_DIAL			"Local_Dial"
#define		SIPUA_OP_CALL_RETURN			"Call_Return"
#define		SIPUA_OP_DISTURBLIST			"DisturbList"
#define		SIPUA_OP_PSTN_LINE			"Pstn_Lines"

#define		SIPUA_OP_VOICE				"voice"

#define	SIPUA_DEFAULT_GREETING_FILE			"greeting.u"
#define	SIPUA_GREETING_FILE					"Greeting_File"


typedef enum
{
	DIAL_PARTTERN_SPEED_DIAL		=	0,		/* phone number to URL */
	DIAL_PARTTERN_SPECIAL_KEY_DIAL	=	1,	/* not used now */
	DIAL_PARTTERN_NORMAL_DIAL	=	2,		/* send phone number to proxy server */
	DIAL_PARTTERN_LOCAL_DIAL		=	3,		/* eg. internal_ip_dial. Call a local branch */
	DIAL_PARTTERN_CALL_RETURN	=	4,		/* call return */
	DIAL_PARTTERN_URL_DIAL		=	5,		/* URL call, not used  */
	DIAL_PARTTERN_PSTN_DIAL		=	6,		/*  */
	DIAL_PARTTERN_DIRECT_DIAL	=	7		/* call with device Id etc, not used now */
}dial_parttern_t;

#define	DIAL_PARTTERN_MAX		DIAL_PARTTERN_DIRECT_DIAL

typedef enum
{
	DIAL_STATE_DISABLE		=	0,
	DIAL_STATE_ENABLE		
}dial_parttern_state_t;


#define	DIAL_RULE_TRI						"TriValue"		/* the third item in Dial_Pattern or Dial_Rule */
#define	DIAL_RULE_FOURTH					"FourthValue"		/* the fourth item in Dial_Pattern or Dial_Rule */

#define	PBX_PID_FILE						"/etc/sip/pbx.pid"

#define 	SIP_UA_CONFIG 						"/usr/etc/ua.cfg"


int sip_readconfig(char *filename,int itemnum,char *ctype,llist *l);

#endif
