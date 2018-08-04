/*
* $Id: pbx_cfg_data.c,v 1.9 2007/09/21 21:16:02 lizhijie Exp $
*/
#include "pbx.h"
#include "cgi_sigdef.h"

typedef	struct
{
	pbx_cfg_t	*cfgs;
	int			count;
}pbx_cfg_iter_t;

pbx_cfg_t pbx_global_cfgs[] = 
{
	/* Global basic configuration */
	{0xc9, PBX_CFG_TYPE_STATIC, 
		"PbxData", "Begin", "" },
	{0xca, PBX_CFG_TYPE_STATIC, 
		"PbxData", "Ended", ""},

	{0xa4, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_NODIAL_ACTION, PBX_NODIAL_AUTO, ""},
	{0xa5, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_NODIAL_ACTION, PBX_NODIAL_BROKEN, "" },

	{0xa6, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_LINE_2_LINE,	CGI_STR_OFF, ""},
	{0xa7, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_LINE_2_LINE,	CGI_STR_ON, "" },

	{0xa8, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_DIFFER_RING,	CGI_STR_OFF, ""},
	{0xa9, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_DIFFER_RING,	CGI_STR_ON, "" },

	{0xaa, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_CALLID_MODE,	PBX_CALLID_FSK/* 0 */, ""},
	{0xab, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_CALLID_MODE,	PBX_CALLID_DTMF/* 1 */, "" },

	{0xb1, PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_GLOBAL_PSTN_PREFIX,	"", ""},
	{0xb2, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_PSTN_PREFIX,	"", "" },	/* PSTN prefix is null */

	{0xd2, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_SPECIAL_FIRST,	CGI_STR_ON, "" },	/* 1 */
	{0xd3, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_SPECIAL_FIRST,	CGI_STR_OFF, "" },/* 0 */

	{0xda, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_GROUP_CALL,	CGI_STR_ON, "" },	/* 0 */
	{0xdb, PBX_CFG_TYPE_STATIC, 
		PBX_GLOBAL_GROUP_CALL,	CGI_STR_OFF, "" },		/* 1 */

	{0xd4, PBX_CFG_TYPE_RELOAD_VALUE|PBX_CFG_TYPE_RELOAD_VALUE_INTEGER, 
		PBX_GLOBAL_IP_PREFIX,		"" },

	{0xE4, PBX_CFG_TYPE_RELOAD_VALUE|PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED, 
		PBX_GLOBAL_VERSION_RELEASE,		"" },
	/* no system password and password configuration in Global configuration */

};

#if 0
char		*extemsion_basic_keywords[] = 
{
	"NUM", 
	"LENGTH", 
	"SCOPE", 
	"DIALOUT", /* extension phone number */
	"DELAY_2_LINE", /* extension phone number */
	"CALLID", /* extension phone number */
	"CALLWAIT", /* extension phone number */
	"NODISTURB" /* extension phone number */
};
#endif

pbx_cfg_t	pbx_ext_basic_template[] =
{
	/* Phone Number, bc01x  */
	{0xbf, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_EXT_B_PHONE_NUMBER, "" , ""},

	/* Call ID, bc08x */
	{0xb5, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_STATIC, 
		PBX_EXT_B_CALLID, CGI_STR_OFF, ""},
	{0xb6, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_STATIC, 
		PBX_EXT_B_CALLID, CGI_STR_ON, "" },

	/* dialout mode, */
	{0xc2, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE
		|PBX_CFG_TYPE_RELOAD_VALUE_IP_PREFIX, 
		PBX_EXT_B_DIALOUT, "", "" },/* dial out mode for IP(0-->a) and PSTN(1-->b) */
	{0xc1, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_EXT_B_DIALOUT, "", "" },/* dial out mode for others */

	/* scope, such as international, bc03x */
	{0xaf, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_EXT_B_SCOPE, "" , ""},

	/* time length, bc04x  */
	{0xb0, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE
		|PBX_CFG_TYPE_RELOAD_VALUE_DIGIT, 
		PBX_EXT_B_LENGTH, "" , ""},

	/* delay to line, bc09x */
	{0xbe, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_EXT_B_DELAY2LINE, "", "" },

	/* No Disturb, bc022x */
	{0xde, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_B_NODISTURB, CGI_STR_ON, ""},
	{0xdf, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_B_NODISTURB, CGI_STR_OFF, ""},

	/* Call Wait, bc026x  */
	{0xd0, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_B_CALLWAIT, CGI_STR_ON , ""},
	{0xd1, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_B_CALLWAIT, CGI_STR_OFF, "" },
	
};

pbx_cfg_t	pbx_ext_special_template[] =
{
	/* No Response Transfer extension, bcs06x  */
	{0xb8, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_NORESPONSE, "" , ""},/* just get index */
	{0xb7, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT|PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED, 
		PBX_EXT_S_NORESPONSE, "" , ""},/* get phone number */

	/* No Response Time, bcs15x  */
	{0xd9, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE
		|PBX_CFG_TYPE_RELOAD_VALUE_DIGIT, 
		PBX_EXT_S_NORESTIME, "" , ""},

	/* Secret Transfer extension, bcs07x  */
	{0xba, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_SECRET, "" , ""},/* just get the index */
	{0xb9, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT, 
		PBX_EXT_S_SECRET, "" , ""},/* get phone number by index*/

	/* Busy Transfer extension, bcs05x  */
	{0xbc, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_BUSY, "" , ""},/* just index */
	{0xbb, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT, 
		PBX_EXT_S_BUSY, "" , ""},/* phone number by index */

	/* Night Transfer extension, bcs10x  */
	{0xcd, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_NIGHT, "" , ""},/* just index */
	{0xcb, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT|PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED, 
		PBX_EXT_S_NIGHT, "" , ""},/* phone number by index */

	/* unknown Transfer extension, bcs12x ????  */
	{0xcf, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_DENYCALLIN, "" , ""},/* just index */
	{0xce, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT|PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED, 
		PBX_EXT_S_DENYCALLIN, "" , ""},/* phone number */

	/* No Conditional Transfer extension, bcs14x  */
	{0xd8, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_EXT_S_NOCONDITION, "" , ""},/* just index */
	{0xd7, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE|
		PBX_CFG_TYPE_TRANSFER_EXT|PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED, 
		PBX_EXT_S_NOCONDITION, "" , ""},/* phone number */

	/* Differ Ringing, bcs25x (contain bcs17x(group) as sub element) 
	* '0xddi is in format of 'Branch_Index|Differ_Ring|Group'. When command is write to
	* PBX, it is same as that format */
	{0xdd, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_EXT_S_DIFFERTING, "" , ""},

};


pbx_cfg_t	pbx_lines_cfgs[] =
{
	/* Line Status, lc01x  */
	{0xa0, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_LINE_STATUS, CGI_STR_OFF , ""},
	{0xa1, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_LINE_STATUS, CGI_STR_ON, "" },
	
	/* Line Console, lc02x : when default, no console info for line 4 is return */
	{0xac, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_LINE_CONSOLE, "", "" },

	/* Line Group, lc03x : this command is not feedback by PBX */
	{0xe2, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_LINE_GROUP, "", "" },

};

pbx_cfg_t	pbx_trunck_cfgs[] =
{
	/* Input Trunck, cc01  */
	{0xa2, PBX_CFG_TYPE_STATIC, 
		PBX_TRUNCK_MODE, PBX_TRUNCK_CONSOLE , ""},/* console : 0 */
	{0xa3, PBX_CFG_TYPE_STATIC, 
		PBX_TRUNCK_MODE, PBX_TRUNCK_DIRECT, "" },/* direct dial : 1 */
};

pbx_cfg_t	pbx_account_cfgs[] =
{/* no ac02(account password is provided by serial port command) */

	/* Account Authority, ac03  */
	{0xae, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_ACCOUNT_AUTH, "6", ""},
	{0xad, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_ACCOUNT_AUTH, "", "" },
};

pbx_cfg_t	pbx_specialservice_cfgs[] =
{/* no ac02(account password is provided by serial port command) */

	/* Sepcial, sc02x  */
	{0xd6, PBX_CFG_TYPE_RELOAD_KEY, 
		PBX_SPECIAL_SERVICE, "", ""},
	{0xd5, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_SPECIAL_SERVICE, "", ""},
	{0xae, PBX_CFG_TYPE_RELOAD_KEY|PBX_CFG_TYPE_RELOAD_VALUE, 
		PBX_SPECIAL_SERVICE, "", ""},
};


pbx_cfg_iter_t cfg_iters[] = 
{
	{pbx_global_cfgs, sizeof(pbx_global_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_ext_basic_template, sizeof(pbx_ext_basic_template)/sizeof(pbx_cfg_t) },
	{pbx_ext_special_template, sizeof(pbx_ext_special_template)/sizeof(pbx_cfg_t) },
	{pbx_lines_cfgs, sizeof(pbx_lines_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_trunck_cfgs, sizeof(pbx_trunck_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_account_cfgs, sizeof(pbx_account_cfgs)/sizeof(pbx_cfg_t) },
	{pbx_specialservice_cfgs, sizeof(pbx_specialservice_cfgs)/sizeof(pbx_cfg_t) },
};

pbx_cfg_t *__find_cfg_item(pbx_cfg_t *cfgs, int count, unsigned char op)
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

pbx_cfg_t *pbx_find_cfg_item(PBX_COMMAND *cmd)
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

