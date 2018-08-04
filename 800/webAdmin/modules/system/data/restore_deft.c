/*
* $Id: restore_deft.c,v 1.2 2006/12/19 17:32:00 lizhijie Exp $
* When device start up,  if flag((/etc/renew.flg) exist,
* This program will be execute. It Read configuration data from 
* flash memory and write to system files.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "libCmn.h"

#include "cgiLib.h"
#include "cgi_data.h"
#include "cgi_syscfg.h"

#define RENEW_FLAG        "/etc/renew.flg" 

//Add user or fix password on system(linux&busybox)
static void add_sys_usr(char *name, char *pass)
{
	char cmd[256];
	
	sprintf(cmd, "/bin/adduser -p0 %s", name);
	system(cmd);
	sprintf(cmd, "/usr/bin/passwd -p %s %s",pass,name);
	system(cmd);
}
	
#if 0
static int  restore_hosts(char *LAN_ip_addr, char *WAN_ip_addr,  char *host_name)
{
	char		cmd[256];
	FILE      	*fp;

	//backup hosts file
	snprintf(cmd, sizeof(cmd),"/bin/mv %s %s.bak", HOSTNAME_CONF, HOSTNAME_CONF);
	system(cmd);
	//delete script
	system("/bin/rm -f /etc/hostname.script");
	
	//open file and it is truncated
	if ((fp = fopen(HOSTNAME_CONF, "w+") ) == (FILE *)NULL) 
	{
		fprintf(stderr, "Restore_deft:Open %s is failed.\n", HOSTNAME_CONF);
		return -1;
	}
	fprintf(fp, "%s\t\t%s\n%s\t\t%s\n127.0.0.1\t\tlocalhost\n", WAN_ip_addr,  host_name, LAN_ip_addr, host_name);
	fclose(fp);

	return 0;
}
#endif

static int restore_eth_cfg(CMN_FLASH_INFO_T *factory, int isWan)
{
	char		filename[128];
	FILE      	*fp;
	char		*dev, *hwaddr;
	char		*ipStr, *maskStr;
	struct in_addr ip,mask,network,broadcast;

	if(isWan )
	{
		dev = ETH0;
		hwaddr = factory->wanMac;
		ipStr = factory->wanIp;
		maskStr = factory->wanMask;
	}	
	else
	{
		dev = ETH1;
		hwaddr = factory->lanMac;
		ipStr = factory->lanIp;
		maskStr = factory->lanMask;
	}	

	snprintf(filename, sizeof(filename), "%s/hwcfg-%s", SYS_CFG_PATH, dev );
	if ((fp = fopen(filename, "w+") ) == (FILE *)NULL) 
	{
		fprintf(stderr, "Open %s failed when restore Factory Default.\n", filename);
		return -1;
	}
	
	fprintf(fp, "# restore Factory Default from NVRAM(Flash)\n\n" );
	fprintf(fp, "%s=%s\n%s=%s\n",NIC_KEYWORD_DEVICE, dev,NIC_KEYWORD_HWADDR, hwaddr);
	fclose(fp);
	
	snprintf(filename, sizeof(filename), "%s/ifcfg-%s", SYS_CFG_PATH, dev );
	if ((fp = fopen(filename, "w+") ) == (FILE *)NULL) 
	{
		fprintf(stderr, "Open %s failed when restore Factory Default.\n", filename);
		return -1;
	}

	if(!inet_aton(ipStr, &ip) )
	{
		fprintf(stderr, "'%s' is not a validate IP address", ipStr);
		return -2;
	}
	if(!inet_aton(maskStr, &mask) )
	{
		fprintf(stderr, "'%s' is not a validate IP Network Mask", maskStr);
		return -2;
	}

	network.s_addr = ip.s_addr&mask.s_addr;
	broadcast.s_addr = ip.s_addr|~mask.s_addr;
	
	fprintf(fp, "# restore Factory Default from NVRAM(Flash)\n\n" );
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_DEVICE, dev);
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC);
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_IP_ADDRESS, ipStr);
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_NETMASK, maskStr );
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_BROADCAST, inet_ntoa(broadcast));
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_NET_ADDRESS, inet_ntoa(network));
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_ON_BOOT, NIC_VALUE_YES);
	fprintf(fp, "%s=%s\n",NIC_KEYWORD_DHCP_SERVICE, NIC_VALUE_NO );
	fclose(fp);
	
	return 0;
}
	
	
//main func
int main()
{
	llist 				cfgs;
	CMN_FLASH_INFO_T	factory;
	
	if (readconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &cfgs ) != 0)
	{
		printf("No Restore configuration file\n");
		return -1;
	}
	/*
	//if renew.flag not exist, That not need to restore data.  
	if( access(RENEW_FLAG,F_OK) <0)
		return 0;
	*/
	//Delete flag.
	remove(RENEW_FLAG);
	
	list_replace_value_by_key(&cfgs, CGIDATA_KEYWORD_RESET_FLAG, CGIDATA_KEYWORD_RESET_NO );
	writeconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &cfgs );

	
	if ( as_default_read( &factory))
	{
		fprintf(stderr, "Read data from flash error!May Flash is not initialized correctly!\n");
		return -1;
	}	
 
	if (restore_eth_cfg(&factory, 1) )
			return -1;

	if (restore_eth_cfg(&factory, 0) )
			return -1;

	//Restore user account(root). 
	add_sys_usr(factory.userName, factory.passwd );
	
#if 0
	if (restore_hosts(factory.lanIp, factory.wanIp, "IXP4XX") < 0)
		return -1;
#endif
	
	as_beep_short(1);
	printf("Restore default is successful, reboot device now.\n");
	system("reboot");

	return 0;	
}

