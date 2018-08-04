/*
* $Id: cgiStcCfgData.c,v 1.7 2007/09/14 09:11:10 lizhijie Exp $
*/
#include "_cgi_stc.h"
#include "cgi_sigdef.h"

typedef	struct
{
	pbx_cfg_t	*cfgs;
	int			count;
}pbx_cfg_iter_t;

pbx_cfg_t pbx_global_cfgs[] = 
{
	/* Global basic configuration */
	{CMD_CFG_PBX_IP_BEGIN, STC_CFG_TYPE_NONE, 
		"PbxDataBegin", "Begin", "", "", "" },
	{CMD_CFG_PBX_IP_END, STC_CFG_TYPE_NONE, 
		"PbxDataEnd", "Ended", "", "", ""},

	{CMD_CFG_NODIAL_ACTION, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_NODIAL_ACTION, PBX_NODIAL_AUTO,
		PBX_NODIAL_AUTO, PBX_NODIAL_BROKEN, ""},

	{CMD_CFG_LINE2LINE, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_LINE_2_LINE,	CGI_STR_OFF, 
		PBX_LINE2LINE_OPEN, PBX_LINE2LINE_FORBIDDEN , ""},

	{CMD_CFG_DIFFER_RINGING, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_DIFFER_RING,	CGI_STR_OFF, 
		CGI_STR_ON, CGI_STR_OFF, ""},

	{CMD_CFG_CALLER_ID_MODE, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_CALLID_MODE,	CGI_STR_ON/* 1 */, 
		CGI_STR_ON, CGI_STR_OFF, "" },

	{CMD_CFG_SPECIAL_FIRST, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_SPECIAL_FIRST,	CGI_STR_OFF, 
		CGI_STR_ON, CGI_STR_OFF, "" },/* 0 */

	{CMD_CFG_GROUP_CALL, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_GROUP_CALL,	CGI_STR_ON, 
		CGI_STR_ON, CGI_STR_OFF, "" },		/* 1 */

	{CMD_CFG_PSTN_PREFIX, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_GLOBAL_PSTN_PREFIX,	"", 
		"", "", ""},

	{CMD_CFG_IP_PREFIX, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_GLOBAL_IP_PREFIX,		"",
		"", "", "" },

	{CMD_CFG_NIGHT_SERVICE_TIME, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_TIME, 
		PBX_GLOBAL_NIGHT_TIME,		"",
		"", "", ""},

	{CMD_CFG_SYS_PASSWD, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_GLOBAL_SYSTEM_PASSWD,		"" },

	{CMD_CFG_LINE2LINE_PASSWD, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_GLOBAL_PASSWORD,		"" },

	{CMD_CFG_SYS_DATE, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_TIME, 
		PBX_GLOBAL_SYSTEM_TIME,		"" },

	{CMD_CFG_PBX_IP_VERSION, STC_CFG_TYPE_VALUE, 
		PBX_GLOBAL_VERSION_RELEASE,		"" },

	{CMD_CFG_SIGNAL_TYPE, STC_CFG_TYPE_BOOL, 
		PBX_GLOBAL_SIGNAL_TYPE,	CGI_STR_JAPANESE, 
		CGI_STR_CHINESE, CGI_STR_JAPANESE, "" },

};

pbx_cfg_t	pbx_ext_basic_template[] =
{
	/* Phone Number */
	{CMD_CFG_EXT_PHONENUMER, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_EXT_B_PHONE_NUMBER, "" , ""},

	/* Call ID */
	{CMD_CFG_EXT_CALLER_ID, STC_CFG_TYPE_KEY|STC_CFG_TYPE_BOOL, 
		PBX_EXT_B_CALLID, CGI_STR_ON, 
		CGI_STR_ON, CGI_STR_OFF, "" },

	/* dialout mode */
	{CMD_CFG_EXT_DIALOUT_MODE, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE
		|PBX_CFG_TYPE_RELOAD_VALUE_IP_PREFIX, 
		PBX_EXT_B_DIALOUT, "", 
		"", "", "" },/* dial out mode for IP(0-->a) and PSTN(1-->b) */

	/* scope, such as international */
	{CMD_CFG_EXT_AUTHORITY, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_B_SCOPE, "" , 
		"", "", "" },

	/* time length  */
	{CMD_CFG_EXT_TIMEOUT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE
		|PBX_CFG_TYPE_RELOAD_VALUE_DIGIT, 
		PBX_EXT_B_LENGTH, "" , 
		"", "", "" },

	/* delay to line */
	{CMD_CFG_EXT_DELAY_2_LINE, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_B_DELAY2LINE, "", 
		"", "", "" },

	/* No Disturb */
	{CMD_CFG_EXT_NODISTURB, STC_CFG_TYPE_KEY|STC_CFG_TYPE_BOOL, 
		PBX_EXT_B_NODISTURB, CGI_STR_ON,
		CGI_STR_ON, CGI_STR_OFF, ""},

	/* Call Wait  */
	{CMD_CFG_EXT_CALLWAIT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_BOOL, 
		PBX_EXT_B_CALLWAIT, CGI_STR_ON , 
		CGI_STR_ON, CGI_STR_OFF, ""},
};

pbx_cfg_t	pbx_ext_special_template[] =
{
	/* No Response Transfer extension */
	{CMD_CFG_EXT_TRANSFER_LEFT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE_TRANSFER|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_EXT_S_NORESPONSE, "" , 
		"", "", ""},/* just get index */

	/* No Response Time */
	{CMD_CFG_EXT_LEFT_TIME, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_S_NORESTIME, "" , 
		"", "", ""},

	/* Secret Transfer extension */
	{CMD_CFG_EXT_SECRECT_EXT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_S_SECRET, "" , 
		"", "", ""},

	/* Busy Transfer extension */
	{CMD_CFG_EXT_BUSY_EXT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_S_BUSY, "" , 
		"", "", ""},

	/* Night Transfer extension  */
	{CMD_CFG_EXT_TRANSFER_NIGHT, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE_TRANSFER|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_EXT_S_NIGHT, "" , 
		"", "", ""},

	{CMD_CFG_EXT_TRANSFER_DENY, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE_COMPRESSED|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_BLACKLIST, "" , 
		"", "", ""},

	/* No Conditional Transfer extension  */
	{CMD_CFG_EXT_TRANSFER_UNCONDITIONAL, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE_TRANSFER|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_EXT_S_NOCONDITION, "" , 
		"", "", ""},

#if WITH_EXT_DIFFER_RINGING
	/* Differ Ringing, as sub element) 
	* '0xddi is in format of 'Branch_Index|Differ_Ring|Group'. When command is write to
	* PBX, it is same as that format */
	{CMD_CFG_EXT_DIFFER_RINGING, STC_CFG_TYPE_KEY|STC_CFG_TYPE_BOOL, 
		PBX_EXT_S_DIFFERTING, CGI_STR_ON , 
		CGI_STR_ON, CGI_STR_OFF, ""},
#endif

	{CMD_CFG_EXT_GROUP, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_EXT_S_GROUP, "" , 
		"", "", ""},

};


pbx_cfg_t	pbx_lines_cfgs[] =
{
	/* Line Mask Flags, Off(0) is masked(can not be used)  */
	{CMD_CFG_PSTN_FLAG, STC_CFG_TYPE_KEY|STC_CFG_TYPE_BOOL, 
		PBX_LINE_STATUS, CGI_STR_OFF , 
		CGI_STR_ON, CGI_STR_OFF, ""},
	
	/* Line Console : when default, no console info for line 4 is return */
	{CMD_CFG_PSTN_CONSOLE, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_LINE_CONSOLE, "", 
		"", "", "" },

	/* Line Group : this command is not feedback by PBX */
	{CMD_CFG_PSTN_GROUP, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_LINE_GROUP, "", 
		"","", "" },

};

pbx_cfg_t	pbx_trunck_cfgs[] =
{
	/* Input Trunck */
	{CMD_CFG_TRACK_MODE, STC_CFG_TYPE_BOOL, 
		PBX_TRUNCK_MODE, PBX_TRUNCK_CONSOLE , 
		PBX_TRUNCK_DIRECT, PBX_TRUNCK_CONSOLE, ""},/* console : 0 */

	{CMD_CFG_CONSOLE_PHONENUMBER, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_AGENT_PHONE_NUMBER,		"" },

	{CMD_CFG_CONSOLE_EXT_INDEX, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_INTEGER, 
		PBX_AGENT_BRANCH_INDEX,		"" },

};

pbx_cfg_t	pbx_account_cfgs[] =
{/* no ac02(account password is provided by serial port command) */
	/* Account Authority */
	{CMD_CFG_ACCOUNT_AUTHOR, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE, 
		PBX_ACCOUNT_AUTH, "5", 
		"","", ""},

	{CMD_CFG_ACCOUNT_PASSWD, STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_ACCOUNT_PASSWD, "6", 
		"","", ""},
};

pbx_cfg_t	pbx_specialservice_cfgs[] =
{/* no ac02(account password is provided by serial port command) */
	{CMD_CFG_SPECIAL_SERVICE, STC_CFG_TYPE_VALUE|STC_CFG_TYPE_KEY|STC_CFG_TYPE_VALUE_DIGIT, 
		PBX_SPECIAL_SERVICE, "", 
		"","", ""},
};


static pbx_cfg_iter_t cfg_iters[] = 
{
	{pbx_global_cfgs, sizeof(pbx_global_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_ext_basic_template, sizeof(pbx_ext_basic_template)/sizeof(pbx_cfg_t) },
	{pbx_ext_special_template, sizeof(pbx_ext_special_template)/sizeof(pbx_cfg_t) },
	{pbx_lines_cfgs, sizeof(pbx_lines_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_trunck_cfgs, sizeof(pbx_trunck_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_account_cfgs, sizeof(pbx_account_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_specialservice_cfgs, sizeof(pbx_specialservice_cfgs)/sizeof(pbx_cfg_t) },
};

static pbx_cfg_t *__find_cfg_item(pbx_cfg_t *cfgs, int count, unsigned char op)
{
	pbx_cfg_t *found = cfgs;
	int	i;

	for(i= 0; i< count; i++)
	{
		if(found->op == op)
			return found;
		found++;
	}

	return NULL;
}

pbx_cfg_t *stc_find_cfg_item(PBX_COMMAND *cmd)
{
	pbx_cfg_t *cfg = NULL;
	int	i;
//	pbx_cfg_iter_t *iter;

	for(i=0; i< sizeof(cfg_iters)/sizeof(pbx_cfg_iter_t);i++)
	{
		cfg = __find_cfg_item( cfg_iters[i].cfgs, cfg_iters[i].count, cmd->op);
		if(cfg)
			return cfg;
	}

	return NULL;
}

