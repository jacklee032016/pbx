/*
* $Id: meshMain.c,v 1.5 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_mesh.h"

int	mesh_fwd_tbl_ctrl(WIFI_INFO *info);

int main()
{
	char 	*cmd=NULL;
	WIFI_INFO		*info = cgi_wireless_init(CGI_WIRELESS_MESH);

	if( !info )
		return 1;

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGI_WIRELESS_OP_MODE) )
	{
		return cgi_mesh_mode(info);
	}
	if(!strcasecmp(cmd, CGI_MESH_OP_PORTAL) )
	{
		return mesh_portal_ctrl(info);
	}
	if(!strcasecmp(cmd, CGI_MESH_OP_CTRL_FWD)||!strcasecmp(cmd, CGI_MESH_OP_STAT_FWD ) )
	{
		return mesh_fwd_tbl_ctrl(info);
	}
	if(!strcasecmp(cmd, CGI_MESH_OP_NIC)||!strcasecmp(cmd, CGI_MESH_OP_NIC_DHCP) )
	{
		return cgi_lan_setup(&info->cgiVariables, CGI_LAN_MESH, CGI_LAN_WHERE_MESH);
	}
	if(!strcasecmp(cmd, CGI_MESH_OP_STAT_DEVICE) )
	{
		return mesh_status_infos(info);
	}
#if 0	
#if 1	
	if(!strcasecmp(cmd, CGI_WIFI_OP_SCAN) || !strcasecmp(cmd, CGI_WIFI_OP_ACL)   )
	{
		return cgi_future_page(cmd, gettext("WiFi") );
	}
#endif	
	if(!strcasecmp(cmd, CGI_MESH_OP_CTRL_FWD) )
	{
		return wifi_Aodv_Ctrl(info);
	}
#endif	
	else
		
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


