/*
* $Id: fwOps.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/
#include "cgi_lib.h"
#include "cgi_iptables.h"

int	iptables_activate_snat(FW_INFO *info, iptables_item *item)
{
	char	cmd[256];
	if(!strcasecmp( item->action, IPTABLES_KEYWORD_MASQUERADE ) )
	{
		sprintf(cmd,IPTABLES_EXE" -F -t nat"); /* clean nat(POSTROUTING) */
		system(cmd);

		memset(cmd,0,256);
		sprintf(cmd, IPTABLES_EXE" -F" );	/* clean all filter */
		system(cmd);

		sprintf(cmd, IPTABLES_EXE" -t nat -A POSTROUTING -s %s/%s -o %s -j %s",
			info->netInfo->lanInfo.netAddress, info->netInfo->lanInfo.netmask, info->netInfo->wanInfo.name,  IPTABLES_KEYWORD_MASQUERADE );
		system(cmd);
	}
	else
	{
		sprintf(cmd, IPTABLES_EXE" -t nat -A POSTROUTING -s %s -o %s -p %s -j %s --to-source %s",
			item->interAddr, info->netInfo->wanInfo.name,item->protocol, IPTABLES_KEYWORD_SNAT, info->netInfo->wanInfo.addr);
//		sprintf(cmd, IPTABLES_EXE" -t nat -A POSTROUTING %s %s -p %s %s -j %s %s%s",localaddress,eth,item->protocol,
//			localport, IPTABLES_KEYWORD_SNAT, outaddress, outport);
	
	}

	sprintf(cmd, IPTABLES_SAVE" -t nat > %s", IPTABLES_NAT_SAVE);
	system(cmd);

	return 0;
}


int	iptables_activate_default_masqurade(FW_INFO *info)
{
	char	cmd[256];
	
	sprintf(cmd,IPTABLES_EXE" -F -t nat"); /* clean nat(POSTROUTING) */
	system(cmd);

//	memset(cmd,0,256);
	sprintf(cmd, IPTABLES_EXE" -F" );	/* clean all filter */
	system(cmd);

	sprintf(cmd, IPTABLES_EXE" -t nat -A POSTROUTING -s %s/%s -o %s -j %s",
		info->netInfo->lanInfo.netAddress, info->netInfo->lanInfo.netmask, info->netInfo->wanInfo.name,  IPTABLES_KEYWORD_MASQUERADE );
	system(cmd);

	sprintf(cmd, IPTABLES_SAVE" -t nat > %s", IPTABLES_NAT_SAVE);
	system(cmd);

	return 0;
}


