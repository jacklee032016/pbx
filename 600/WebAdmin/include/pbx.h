/*
* $Id: pbx.h,v 1.13 2007/09/21 21:16:02 lizhijie Exp $
* header file for analog PBX configuration
*/
#ifndef	__PBX_H__
#define	__PBX_H__

#define	DIAL_DIGITAL_MAX				20
#define	CMD_VALUE_MAX					14

#define	CGI_MSGTYPE					2
#define	SIP_MSGTYPE					1    

#define	PBX_CFG_TYPE_STATIC 			0x00000001
#define	PBX_CFG_TYPE_RELOAD_VALUE	0x00000002
#define	PBX_CFG_TYPE_RELOAD_KEY		0x00000010
#define	PBX_CFG_TYPE_BLACKLIST		0x00001000

#define	PBX_CFG_TYPE_RELOAD_VALUE_DIGIT				0x01000000	/* get digital value */
#define	PBX_CFG_TYPE_RELOAD_VALUE_IP_PREFIX		0x02000000	/* dial out mode for IP and PSTN prefix */
#define	PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED		0x04000000	/* Compressed, eg. one digit in coded in 4 bits  */
#define	PBX_CFG_TYPE_RELOAD_VALUE_INTEGER			0x08000000	/* reload value, but the value can be null, such as Special Service4|5|6  */

#define	PREFIX_PSTN	"a"
#define	PREFIX_IP		"b"

/* transfer EXT of ext special configuration : PBX return the index of transfered dest, 
* But in our configuration file, phone number of dest ext is saved
*/
#define	PBX_CFG_TYPE_TRANSFER_EXT	0x10000000	
typedef struct
{
	unsigned char		op;
	int				type;
	char				*configKey;
	char				configValue[CMD_VALUE_MAX*3];
	
#if __WITH_NEW_STC__
	char				onValue[32];
	char				offValue[32];
#endif

	char				indexKey[32];

	int				tranferType;
}pbx_cfg_t;


/* PBX Basic Configuration Keyword, 10 items */
#define	PBX_GLOBAL_DIFFER_RING		"Differ_Ring"
#define	PBX_GLOBAL_SPECIAL_FIRST		"Special_First"
#define	PBX_GLOBAL_CALLID_MODE		"CallID_Mode"
#define	PBX_GLOBAL_NODIAL_ACTION		"NoDial_Action"
#define	PBX_GLOBAL_LINE_2_LINE		"Line_2_Line"
#define	PBX_GLOBAL_GROUP_CALL		"Group_Call"
#define	PBX_GLOBAL_PSTN_PREFIX		"PSTN_Prefix"
#define	PBX_GLOBAL_IP_PREFIX			"IP_Prefix"
#define	PBX_GLOBAL_NIGHT_TIME		"Night_Service_Time"
#define	PBX_GLOBAL_SYSTEM_PASSWD	"System_Passwd"
#define	PBX_GLOBAL_PASSWORD			"Password"
#define	PBX_GLOBAL_SYSTEM_TIME		"System_Time"
#define	PBX_GLOBAL_VERSION_RELEASE	"Version_Release"
#define	PBX_GLOBAL_SIGNAL_TYPE		"Signal_Type"

#if __WITH_NEW_STC__
#else
#define	PBX_CALLID_FSK					"FSK"	/* 1 */
#define	PBX_CALLID_DTMF				"DTMF"	/* 0 */
#define	PBX_EXT_S_DENYCALLIN			"Branch_S_DenyCallIn"		/* deny some phone number call in*/
#endif

#define	PBX_NODIAL_AUTO				"AUTO"		/* 1 */
#define	PBX_NODIAL_BROKEN				_("BROKEN")	/* 0 */

#define	PBX_LINE2LINE_OPEN			_("OPEN")		/* 1 */
#define	PBX_LINE2LINE_FORBIDDEN		_("Forbidden")	/* 0 */

/* PBX Extension Basic Configuration Keyword, 8 items */
#define	PBX_EXT_B_PHONE_NUMBER		"Branch_B_Phone"		/* extension phone number */
#define	PBX_EXT_B_LENGTH				"Branch_B_Length"		/* extension Call Time limit */
#define	PBX_EXT_B_SCOPE				"Branch_B_Scope"		/* Internation|PSTN etc */
#define	PBX_EXT_B_DIALOUT				"Branch_B_DialOut"
#define	PBX_EXT_B_DELAY2LINE			"Branch_B_Delay2Line"
#define	PBX_EXT_B_CALLID				"Branch_B_CallID"
#define	PBX_EXT_B_CALLWAIT			"Branch_B_CallWait"
#define	PBX_EXT_B_NODISTURB			"Branch_B_NoDisturb"

/* PBX Extension Special Configuration Keyword, 8 items */
#define	PBX_EXT_S_SECRET				"Branch_S_Secret"		/* extension phone number */
#define	PBX_EXT_S_BUSY				"Branch_S_BusyTransfer"		/* extension Call Time limit */
#define	PBX_EXT_S_NOCONDITION		"Branch_S_NoConditional"		/* Internation|PSTN etc */
#define	PBX_EXT_S_NORESPONSE			"Branch_S_NoResponse"
#define	PBX_EXT_S_NORESTIME			"Branch_S_NoResTime"
#define	PBX_EXT_S_NIGHT				"Branch_S_Night"
#define	PBX_EXT_S_GROUP				"Branch_S_Group"
#define	PBX_EXT_S_DIFFERTING			"Branch_S_DifferRing"
#define	PBX_EXT_S_TRANSFER_TYPE		"Branch_Transfer_Type"

#define	PBX_BLACKLIST					"BlackList"		/* deny some phone number call in*/

/* PBX Lines Configuration Keyword, 3 items */
#define	PBX_LINE_STATUS				"Line_Status"			/*  */
#define	PBX_LINE_CONSOLE				"Line_Console"		/*  */
#define	PBX_LINE_GROUP				"Line_Group"			/*  */

/* PBX Input Trunck Configuration Keyword, 1 items */
#define	PBX_TRUNCK_MODE				"Trunck_Mode"		/*  */

#define	PBX_TRUNCK_CONSOLE			"Console"			/* 0 */
#define	PBX_TRUNCK_DIRECT				"Direct"				/* 1 */

#define	PBX_AGENT_PHONE_NUMBER		"Agent_PhoneNumber"
#define	PBX_AGENT_BRANCH_INDEX		"Agent_Branch"

/* PBX Account Configuration Keyword, 2 items */
#define	PBX_ACCOUNT_PASSWD			"Account_Passwd"		/* no serial command for acccount passwd field */
#define	PBX_ACCOUNT_AUTH				"Account_Auth"		/*  */

/* PBX Special Service Phone Number Configuration Keyword, 1 items */
#define	PBX_SPECIAL_SERVICE			"Special_Service"		/* no serial command for acccount passwd field */


/* PBX configuration for WebAdmin */
#define	PBX_WEBADMIN_EXT_NUM		"Ext_Number"
#define	PBX_WEBADMIN_LINE_NUM		"Line_Number"
#define	PBX_WEBADMIN_SPECIAL_NUM	"Special_Number"
#define	PBX_WEBADMIN_GROUP_NUM		"Group_Number"

#define	START_HOUR					"Start_Hour"
#define	START_MINUTE					"Start_Minute"
#define	END_HOUR						"End_Hour"
#define	END_MINUTE					"End_Minute"

#define	LEN_CMD						16
#define   BASE_YEAR   					100
#if __WITH_NEW_STC__
	#include "uart_cmd.h"
#else
/* global command */
#define	CMD_PC_TONE_NODIFF			0x28	/* 0 */
#define	CMD_PC_TONE_DIFF				0x29	/* 1 */

#define	CMD_PC_DISP_FSK				0x2a	/* 1 */
#define	CMD_PC_DISP_DTMF				0x2b	/* 0 */

#define	CMD_PC_NODIAL_PHONE			0x24	/* 1, to console*/
#define	CMD_PC_NODIAL_OFF			0x25	/* 0, busy, eg. broken  */

#define	CMD_PC_L2L_YES				0x27	/* 1 */
#define	CMD_PC_L2L_NO					0x26	/* 0 */

#define	CMD_PC_LNET_HEAD				0x31
#define	CMD_PC_LNET_NOHEAD			0x32

#define	CMD_PC_GROUP					0x58
#define	CMD_PC_NOGROUP				0x59

#define	CMD_PC_SPECIAL_FIRST			0x50
#define	CMD_PC_NO_SPECIAL				0x51

#define	CMD_PC_IP_PREFIX				0x52

#define	CMD_PC_PASSWORD				0x46
#define	CMD_PC_SYS_PASS				0x48

#define	CMD_PC_GET_CONFIG			0x49	/* notify PBX that we want config info */
#define	CMD_PC_GET_DATE				0x5a	/* get date from PBX */
#define	CMD_PC_RESET					0x3d	/* reset all default configuration of PBX, such as IP prefix is 17909 */

#define	CMD_BC_OUTFWD_SET			0x37
#define	CMD_BC_NSNUM_SET				0x4a
#define	CMD_BC_DENYNUM_SET			0x4c

#define		DIAL_DIGIT_ZERO					10			/* 0x0a, '0' */
#define		DIAL_DIGIT_ASTREIC				11			/* 0x0b, '*' */
#define		DIAL_DIGIT_SPECIAL					12			/* 0x0c, '#' */

#define		STC_BLACKLIST_COUNT			6
#endif

#include "cgi_llist.h"
#include "aslib.h"

pbx_cfg_t *pbx_find_cfg_item(PBX_COMMAND *cmd);

#endif

