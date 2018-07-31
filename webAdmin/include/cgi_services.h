/*
* $Author: lizhijie $
* $Id: cgi_services.h,v 1.7 2007/08/31 20:30:50 lizhijie Exp $
*/

#ifndef __CGI_SERVICES_H__
#define  __CGI_SERVICES_H__
#include <errno.h>

#include "cgiLib.h"

#define SERVICES_LIST_FILE			"/etc/web/services.conf"
#define SERVICES_LIST_SCRIPTS		"/sbin/scripts/services.sh"

typedef enum// statenum
{
	SERVICE_STATE_INVALIDATE 		= -1 ,
	SERVICE_STATE_SLEEPING 		= 0 ,
	SERVICE_STATE_RUNNING
}service_state_t;

#define	SERVICE_ACTION_START			"Start"
#define	SERVICE_ACTION_STOP			"Stop"

#define	SERVICE_NAME_SMTPD			"master"
#define	SERVICE_NAME_POP_IMAP		"dovecot"
#define	SERVICE_NAME_ADMIN			"boa"
#define	SERVICE_NAME_WWW			"www"

#define	SERVICE_NAME_DHCP			"udhcpd"
#define	SERVICE_NAME_FTP				"vsftpd"
//#define	SERVICE_NAME_UPNP			"upnpd"
#define	SERVICE_NAME_IPSET			"ipsetd"
#define	SERVICE_NAME_UPNP			"upnpd"
#define	SERVICE_NAME_UPNP_NEIGHBOUR	"uDev"					/* UPnP Device */
#define	SERVICE_NAME_UPNP_FORWARD	"uCtrl"					/* UPnP Control Point */

#define	SERVICE_NAME_SYSLOGD			"syslogd"
#define	SERVICE_NAME_KLOGD			"klogd"

#define	SERVICE_NAME_VPNSERVER		"pptpd"
#define	SERVICE_NAME_VPNCLIENT		"pptp"

#define	SERVICE_NAME_DDNSD			"inadyn"

#define	SERVICES_DEBUG					0 /*1*/

struct _own_node 
{
	char 			name[64];
	char 			display_name[128];	
	char 			command[256];
	int 				pid;
	int				startup;				/* weather startup when boot */
	
	service_state_t 	state;
};

typedef struct _own_node ServiceNode;

struct _proclist 
{
	ServiceNode		*node;
	struct _proclist 	*next;
//	struct _proclist 	*head;
};

typedef struct _proclist proclist;

proclist *cgi_services_list();
service_state_t cgi_service_get_state(char *command /* first field in config file*/);
service_state_t cgi_service_start(char *command /* first field in config file*/);
service_state_t cgi_service_stop(char *command /* first field in config file*/);
service_state_t cgi_service_restart(char *command /* first field in config file*/);

void cgi_services_stop_all();

char *cgi_service_get_state_name(char *command);
service_state_t  cgi_get_program_state( char *name ,char *command);
char *cgi_get_program_state_name(char *name);
int  cgi_get_program_pid_with_param(char *cmd, char *param);
char *cgi_service_state_name(service_state_t state);

#endif

