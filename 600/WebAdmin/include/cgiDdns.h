#ifndef	__CGI_DDNS_H__
#define	__CGI_DDNS_H__

#define	DDNS_DEFAULT_BUFFER_LENGTH		256

#include "aslib.h"
#include "cgi_lib.h"

#define	PEANUT_CONFIG_FILE				"/etc/web/peanut.conf"

#define	PEANUT_OP_CONFIG					"config"
#define	PEANUT_OP_TEST					"test"


#define	PEANUT_KEYWORD_LOG_LEVEL		"LOG_LEVEL"
#define	PEANUT_KEYWORD_LOG_TYPE			"LOG_TYPE"
#define	PEANUT_KEYWORD_LOG_FILE			"LOG_FILE"

#define	PEANUT_KEYWORD_USERNAME		"USER_NAME"
#define	PEANUT_KEYWORD_PASSWD			"PASSWORD"
#define	PEANUT_KEYWORD_LOCAL_UDP		"LOCAL_UDP"

#define	PEANUT_KEYWORD_FQDN				"FQDN"

#define	PEANUT_KEYWORD_SERVER			"SERVER_ADDRESS"
#define	PEANUT_KEYWORD_PORT				"SERVER_PORT"


#define 	PEANUT_LOG_TYPE_SYSLOG			"SYSLOG"
#define 	PEANUT_LOG_TYPE_CONSOLE			"CONSOLE"
#define 	PEANUT_LOG_TYPE_FILE				"FILE"

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
}PEANUT_INFO;




char	*PeanutTestRegisterResult(char *hostName);
char *asInetNtoa(long ip);
int PeanutInitConfig(PEANUT_INFO *info);


int cgiPeanutConfig(PEANUT_INFO *info);
int cgiPeanutTest(PEANUT_INFO *info);

#endif

