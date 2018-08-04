/*$Id*/

#ifndef __CGI_PBX_H__
#define __CGI_PBX_H__

#include <string.h>
#include "cgi_sigdef.h"
#include "cgi_lib.h"
#include "cgi_llist.h"

#include "pbx.h"

// refresh or append "pc06" to siprules.conf
#define SIG_SIP_RULES 		51

#define	CGIPBX_OP_GLOBAL_DEVICE						"GlobalDevice"			/* Global configuration */
#define	CGIPBX_OP_BASE_CONFIG						"Global"			/* Global configuration */
#define	CGIPBX_OP_GLOBAL_NUMBER						"GlobalNumber"	/* Global Phone Number for PBX */
#define	CGIPBX_OP_BRANCH_NUMBER						"BranchNumber"	/* Phone Number Config of branch */
#define	CGIPBX_OP_BRANCH_BASE						"BranchBase"		/* Base Config of branch */
#define	CGIPBX_OP_BRANCH_SPEC						"BranchSpec"		/* special Config of branch */
#define	CGIPBX_OP_BRANCH_EXT_TRANSFER				"BranchTransfer"		/* special Config of branch */
#define	CGIPBX_OP_BRANCH_CALL_TRANSFER				"CallTransfer"		/* special Config of branch */
#define	CGIPBX_OP_BRANCH_NIGHT_SERVICE				"NightService"		/* special Config of branch */
#define	CGIPBX_OP_LINES								"Lines"			/* external Lines of PSTN */
#define	CGIPBX_OP_TRUNCK								"Trunck"			/* input trunck */
#define	CGIPBX_OP_ACCOUNT							"Account"			/* account */
#define	CGIPBX_OP_SERVICES_SPEC						"Services"		/* special services */

#define	CGIPBX_OP_BLACKLIST							"BlackList"

#define	CGIPBX_OP_BILL									"Bill"
#define	CGIPBX_OP_CONFERENCE							"Conference"

#define	CGIPBX_SUB_OP_PHONE_INDEX					"PhoneIndex"

/* global basic config of PBX */
#define	PBX_CFG_RESET									"pc99"	/* reset the PBX */

#define	PBX_CFG_DIFFER_CALL							"pc01"	/* ring differently for phone and lines */
#define	PBX_CFG_CALLID_METHOD						"pc02"	/* DTMF is 0, FSK is 1 */
#define	PBX_CFG_NO_DIAL_PROCESS						"pc03"	/* how to process the case of no dial after dial in, 0 : broken, 1: auto to console  */
#define	PBX_CFG_LINE_2_LINE							"pc04"	/* disable or enable */
#define	PBX_CFG_PASSWORD								"pc05"	/* this is string variable */
#define	PBX_CFG_PSTN_PREFIX							"pc06"	/* prefix of PSTN call : 0~9 */
#define	PBX_CFG_SYSTEM_PASSWORD						"pc07"	/* */
#define	PBX_CFG_SPEC_NUM_FIRST						"pc08"	/* first for special phone number */
#define	PBX_CFG_GROUP_CALL							"pc09"	/* 0 :enabled; 1 : disabled */
#define	PBX_CFG_IP_PREFIX								"pc10"	/* prefix of phone number which is target to IP */

#define	PBX_CFG_RESET_PBX_CMD						"pc99"


#define	PBX_CFG_GROUP_CALL_ON						"0"
#define	PBX_CFG_GROUP_CALL_OFF						"1"

/* Basic config of Branch */
#define	PBX_CFG_BRANCH_BASIC							"bc0"		/* */
#define	PBX_CFG_BRANCH_PHONENUM						10
#define	PBX_CFG_BRANCH_TIMEOUT						40
#define	PBX_CFG_BRANCH_AUTHORITY					30
#define	PBX_CFG_BRANCH_DIAL_OUT						20
#define	PBX_CFG_BRANCH_DELAY_TO_LINE				90
#define	PBX_CFG_BRANCH_CALLID						80
#define	PBX_CFG_BRANCH_BASIC_0						"bc"		/* */
#define	PBX_CFG_BRANCH_CALL_WAIT					260
#define	PBX_CFG_BRANCH_NO_DISTURB					220


#define	PBX_CFG_BRANCH_SPEC							"bcs0"		/* */
#define	PBX_CFG_BRANCH_SPEC_SECRET					70		/* */
#define	PBX_CFG_BRANCH_SPEC_BUSY					50		/* */
#define	PBX_CFG_BRANCH_SPEC_LEFT						60		/* */
#define	PBX_CFG_BRANCH_SPEC_0						"bcs"		/* */
#define	PBX_CFG_BRANCH_SPEC_NIGHT					100		/* */
#define	PBX_CFG_BRANCH_SPEC_UNCONDITIONAL			140		/* */
#define	PBX_CFG_BRANCH_SPEC_LEFT_TIME				150		/* */
#define	PBX_CFG_BRANCH_SPEC_GROUP					170		/* */
#define	PBX_CFG_BRANCH_SPEC_DIAL						250		/* */


#define	PBX_CFG_LINES									"lc0"		/* */
#define	PBX_CFG_LINES_STATUS							10		/* */
#define	PBX_CFG_LINES_CONSOLE						20		/* */
#define	PBX_CFG_LINES_GROUP							30		/* */

#define	PBX_CFG_TRUNCK								"cc01"		/* */

#define	PBX_CFG_ACCOUNT								"ac0"		/* */
#define	PBX_CFG_ACCOUNT_PASSWD						20		/* */
#define	PBX_CFG_ACCOUNT_AUTHORITY					30		/* */

#define	PBX_CFG_SPECIAL_SERVICE						"sc02"		/* */

#define	PBX_BRANCH_MAX								8			/* max 8 branch */
#define	PBX_LINE_MAX									4			/* max 4 lines */
#define	PBX_GROUP_MAX									4			/* max 4 groups */
#define	PBX_ACCOUNT_MAX								2			/* max 4 groups */
#define	PBX_SPECIAL_MAX								6			/* max 4 groups */

#define	PBX_CMDSET										"PbxCmdSet"
#define	PBX_CMDSET_RESET								"PbxCmdReset"		/* reset PBX */
#define	PBX_CMDSET_BASIC								"PbxCmdGlobal"
#define	PBX_CMDSET_BRANCH_BASIC						"PbxCmdExtBase"
#define	PBX_CMDSET_BRANCH_SPEC						"PbxCmdExtSpe"
#define	PBX_CMDSET_LINES								"PbxCmdLines"
#define	PBX_CMDSET_TRUNCK							"PbxCmdTrunck"
#define	PBX_CMDSET_ACCOUNT							"PbxCmdAccount"
#define	PBX_CMDSET_SPECIAL							"PbxCmdSpecial"
#define	PBX_CMDSET_BILL								"PbxCmdBill"
#define	PBX_CMDSET_CONFERENCE						"PbxCmdConference"


typedef	struct
{
	char		key[32];
	char		value[32];
}default_value_t;

typedef	struct	
{
	int		extsCount;
	int		linesCount;
	int		specialServiceCount;
	int		groupCount;

	llist		cgiVariables;
	llist		cfgs;

	pid_t 	pid;				/* of process cgipbx */
	int 		pbxReplyCode;

}PBX_INFO;

#if __WITH_NEW_STC__
typedef		struct
{
	EXT_TRANSFER_TYPE		type;
	char						*descriptor;//[32];
}cgi_transfer_type_t;

extern	cgi_transfer_type_t cgi_transfer_types[3];
#endif

void sighandler(int signo, siginfo_t *info,void * val);

int	cgiPbx_BasicConfig(PBX_INFO *info);
int	cgiPbx_BranchBasic(PBX_INFO *info);
int	cgiPbx_BranchSpecial(PBX_INFO *info);
int	cgiPbx_Line(PBX_INFO *info);
int	cgiPbx_Trunck(PBX_INFO *info);
int	cgiPbx_Account(PBX_INFO *info);
int	cgiPbx_SpecialService(PBX_INFO *info);

int	cgiPbx_BranchPhoneNumber(PBX_INFO *info);
int	cgiPbx_GlobalDialPlan(PBX_INFO *info);
int	cgiPbx_BranchCallTransfer(PBX_INFO *info);
int	cgiPbx_BranchExtTransfer(PBX_INFO *info);
int	cgiPbx_BranchNightService(PBX_INFO *info);
int	cgiPbx_BlackList(PBX_INFO *info);
char *transfer_button(PBX_INFO *info, int extIndex, char *transferType);


char *stc_tranfer_type_name(int type);
int stc_tranfer_type(char *descriptor);

#endif

