/*
* $Id: cgi_iptables.h,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#ifndef	__CGI_IPTABLES_H__
#define	__CGI_IPTABLES_H__

#define	IPTABLES_DEBUG		1

#define		IPTABLES_EXE							"/usr/sbin/iptables"
#define		IPTABLES_SAVE							"/usr/sbin/iptables-save"
#define		IPTABLES_RESTORE						"/usr/sbin/iptables-restpre"

#define		IPTABLES_CONFIG_DIR					"/etc/iptables/"

#define		IPTABLES_CONFIG_FILE					IPTABLES_CONFIG_DIR"iptables.conf"
#define		IPTABLES_RULE_CONFIG					IPTABLES_CONFIG_DIR"rules.conf"
#define		IPTABLES_TIMER_CONFIG					IPTABLES_CONFIG_DIR"timers.conf"
#define		IPTABLES_VPN_CONFIG					IPTABLES_CONFIG_DIR"vpn.conf"

#define		IPTABLES_NAT_SAVE						"/tmp/iptables_save.nat"

#define		DEFAULT_MASQURADE					"DEFAULT_MASQURADE"

#define	FIREWALL_OP_SNAT							"SNAT"
#define	FIREWALL_OP_MIDDLE						"MIDDLE"
#define	FIREWALL_OP_EXPERT						"EXPERT"
#define	FIREWALL_OP_TIMER							"TIMER"
#define	FIREWALL_OP_TRACK							"TRACK"		/* track connection */

#define	FIREWALL_OP_VPN							"VPN"
#define	FIREWALL_OP_QOS							"QOS"

#define	FIREWALL_SUB_OP_DEL_RULE					"Del_Rule"

#define	IPTABLES_KEYWORD_SNAT						_("SNAT")
#define	IPTABLES_KEYWORD_MASQUERADE				_("MASQUERADE")

#define	IPTABLES_KEYWORD_TBL_NAT					"NAT"
#define	IPTABLES_KEYWORD_TBL_MANGLE					"Mangle"
#define	IPTABLES_KEYWORD_TBL_FILTER					"Filter"

#define	IPTABLES_KEYWORD_CHAIN_INPUT				"INPUT"
#define	IPTABLES_KEYWORD_CHAIN_OUTPUT				"OUTPUT"
#define	IPTABLES_KEYWORD_CHAIN_FORWARD			"FORWARD"
#define	IPTABLES_KEYWORD_CHAIN_PREROUTE			"PREROUTE"
#define	IPTABLES_KEYWORD_CHAIN_POSTROUTE			"POSTROUTING"

#define	IPTABLES_KEYWORD_RULE_ACCEPT				"Accept"
#define	IPTABLES_KEYWORD_RULE_DROP					"Drop"
#define	IPTABLES_KEYWORD_RULE_LOG					"Log"
#define	IPTABLES_KEYWORD_RULE_REJECT				"Reject"
#define	IPTABLES_KEYWORD_RULE_JUMP					"Jump"


#define	IPT_SERVICE_WWW						"www"
#define	IPT_SERVICE_FTP						"FTP"
#define	IPT_SERVICE_EMAIL						"E_mail"
#define	IPT_SERVICE_QQ							"QQ"

#define	IPT_KW_TARGET					"target"
#define	IPT_KW_PROTOCOL				"protocol"
#define	IPT_KW_RULE					"rule"

/* Internal network : LAN */
#define	IPT_KW_InterDevice				"InterDevice"
#define	IPT_KW_InterAddress			"InterAddress"
#define	IPT_KW_InterPortBegin			"InterPortBegin"
#define	IPT_KW_InterPortEnd			"InterPortEnd"

/* external network : WAN */
#define	IPT_KW_ExtDevice				"ExtDevice"
#define	IPT_KW_ExtAddress				"ExtAddress"
#define	IPT_KW_ExtAddressBegin		"ExtAddressBegin"
#define	IPT_KW_ExtAddressEnd			"ExtAddressEnd"
#define	IPT_KW_ExtPortBegin			"ExtPortBegin"
#define	IPT_KW_ExtPortEnd				"ExtPortEnd"

#define	IPT_KW_MAC_ADDRESS			"MacAddress"

#define	IPT_TIMER_MINUTE				"MinuteBegin"
#define	IPT_TIMER_HOUR				"HourBegin"
#define	IPT_TIMER_DAY_BEGIN			"DayBegin"
#define	IPT_TIMER_DAY_END				"DayEnd"
#define	IPT_TIMER_MONTH_BEGIN		"MonthBegin"
#define	IPT_TIMER_MONTH_END			"MonthEnd"
#define	IPT_TIMER_WEEK_BEGIN			"WeekBegin"
#define	IPT_TIMER_WEEK_END			"WeekEnd"
#define	IPT_TIMER_WEEK_END			"WeekEnd"

#define	IPT_TIMER_ALL					"*"

#define	IPT_TIMER_TASK					"*"



//#define	IPT_CFG_NAT_RULE				"NAT"
//#define	IPT_CFG_OTHER_RULE			"RULE"

#define	IPT_CFG_RULE_NAME				_("RULE_NAME")
#define	IPT_CFG_RULE_TYPE				_("RULE_TYPE")
#define	IPT_CFG_RULE_DESCRIPTOR		_("RULE_DESCRIPTOR")

#define	IPT_RULE_CFG_ITEM_NUM		"RULE_NUM"


typedef	enum
{
	IPTABLES_PROTOCOL_ID_ALL		=	0,		/* ID for all protocol */
	IPTABLES_PROTOCOL_ID_ICMP	=	1,
	IPTABLES_PROTOCOL_ID_TCP		=	6,
	IPTABLES_PROTOCOL_ID_UDP		=	17
}iptables_protocol_t;


#define	IPTABLES_PROTOCOL_NAME_ICMP		"icmp"
#define	IPTABLES_PROTOCOL_NAME_TCP		"tcp"
#define	IPTABLES_PROTOCOL_NAME_UDP		"udp"


typedef	struct
{
	iptables_protocol_t		type;
	char					*descriptor;
}ipt_protocol;


typedef	struct
{
	int		type;
	char		*decriptor;
}nat_type_t;

struct _iptables_item
{
	char 			interDevice[5];
	char 			extDevice[5];
	
	char 			protocol[5];
	char 			rule[15];
	char 			action[15];				/* target after -j command*/
	
	char 			interAddr[45];			/* Address of internal LAN */
	char 			interPortBegin[20];		/* start port of Internal LAN */
	char 			interPortEnd[10];			/* End Port of Internal LAN */
	
	char 			extAddrBegin[50];
	char 			extAddrEnd[64];
	char 			extPortBegin[20];
	char 			extPortEnd[10];
	
	char 			state1[15];
	char 			state2[15];
	
	char 			mac[20];

	struct	_iptables_item 	*next;
};


typedef	enum
{
	NAT_DEFAULT	=	0,	/* default MASQURADE */
	NAT_MASQURADE,		/* MASQURADE for every host in LAN */
	NAT_SNAT				/* when WAN is allocated static IP Address , MASQURADE also be used in this state */	
}NAT_enum_t;

typedef struct _iptables_item 	iptables_item;

typedef	struct
{
	llist 				cgiVariables;
	
	llist				globalCfgs;
	llist				ruleCfgs;
	llist				timerCfgs;

	network_info		*netInfo;
	
}FW_INFO;


#define	GET_FIELD_NAME(value, list, field, name, index ) \
	do {		if(index>= 0) sprintf(field, "%s_%d", name, index);	\
	else	 sprintf(field,"%s", name);	\
	sprintf(value, "%s", GET_VALUE(list ,field) ); \
	}while(0)


extern	nat_type_t	nat_types[];
extern	ipt_protocol 	iptables_protocols[];


int cgiFwLookup(FW_INFO *fwInfo);
int cgiFw_Nat(FW_INFO *fwInfo);
int cgiFw_Middle(FW_INFO *info);
int cgiFw_Expert(FW_INFO *info);
int cgiFw_Timer(FW_INFO *info);
int cgiFw_VPN(FW_INFO *info);

int cgiNet_QoS(FW_INFO *info);

int cgiFw_ConnTrack(FW_INFO *fwInfo);

int	iptables_activate_snat(FW_INFO *info, iptables_item *item);

int	fw_save_rule_name(FW_INFO *info);
char *fw_rule_name_buttons(FW_INFO *info );

iptables_item *fw_parse_rule_config(llist *cfgList);
iptables_item *fw_get_rule(llist *cgiOrCfgs, int index);
iptables_item *fw_parse_rule_config_file(char *name);

iptables_item  *cgifw_read_iptables_actives();

int fwNat_save_rule(iptables_item *cfgRules, FW_INFO *info);

#endif

