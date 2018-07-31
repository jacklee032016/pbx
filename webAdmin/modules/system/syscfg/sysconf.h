/*$Id: sysconf.h,v 1.2 2006/12/11 08:27:37 lizhijie Exp $*/

#ifndef __SYSCONF_H__
#define	__SYSCONF_H__

#include "cgi_syscfg.h"

typedef	struct
{
	llist 				cgiVariables;
	
	llist				wanCfgs;
	llist				pppoeCfgs;
	
	llist				usersCfgs;			/* content of /etc/web/data.conf */
	
	llist				descCfgs;		/*added by Bob, for camera descrition info*/
#if 0
	interface_info		*wanInfo;
	interface_info		*lanInfo;
#endif	
}SYSCFG_INFO;

// void sysconf_add_dns(entrytype *item, void *object);


int sysconfig_hostname(SYSCFG_INFO *info);
int sysconfig_lan(SYSCFG_INFO *info);
int sysconfig_wan(SYSCFG_INFO *info);
int sysconfig_pppoe(SYSCFG_INFO *info);
int sysconfig_reboot(SYSCFG_INFO *info);
int	sysconfig_user(SYSCFG_INFO *info);
int sysconfig_descrip(SYSCFG_INFO *info);
char *syscfg_pppoe_checkup();


int read_dhcp_conf(char *filename,char *startname,char *dhcpmask,int u,llist l);

#endif
