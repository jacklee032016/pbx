#ifndef	__CGI_DDNS_H__
#define	__CGI_DDNS_H__

#define	DDNS_DEFAULT_BUFFER_LENGTH		256

#include "libCmn.h"
#include "cgiLib.h"

#define	DDNS_CONFIG_FILE					"/etc/inadyn.conf"

#define	DDNS_OP_CONFIG					"config"
#define	DDNS_OP_TEST						"test"


#define	DDNS_KEYWORD_LOG_LEVEL			"LOG_LEVEL"
#define	DDNS_KEYWORD_LOG_TYPE			"LOG_TYPE"
#define	DDNS_KEYWORD_LOG_FILE			"LOG_FILE"

#define	DDNS_KEYWORD_USERNAME			"username"
#define	DDNS_KEYWORD_PASSWD				"password"
#define	DDNS_KEYWORD_LOCAL_UDP			"LOCAL_UDP"

#define	DDNS_KEYWORD_FQDN				"FQDN"

#define	DDNS_KEYWORD_SERVER			"SERVER_ADDRESS"
#define	DDNS_KEYWORD_PORT				"SERVER_PORT"


#define 	DDNS_LOG_TYPE_SYSLOG			"SYSLOG"
#define 	DDNS_LOG_TYPE_CONSOLE			"CONSOLE"
#define 	DDNS_LOG_TYPE_FILE				"FILE"

typedef struct 
{
	llist 			configs;
	llist 			cgiVariables;

	char			*username;
	char			*password;
	int			localUdpPort;

#if 0
	char			*fqdn;
#else
	char			fqdn[DDNS_DEFAULT_BUFFER_LENGTH];
#endif
	char			serverIp[DDNS_DEFAULT_BUFFER_LENGTH];
	int			port;


	log_style_t	logStyle;
	log_level_t	logLevel;
}DDNS_INFO;




char	*DdnsTestRegisterResult(char *hostName);
char *asInetNtoa(long ip);
int DdnsInitConfig(DDNS_INFO *info);


int cgiDdnsConfig(DDNS_INFO *info);
int cgiDdnsTest(DDNS_INFO *info);

#endif

