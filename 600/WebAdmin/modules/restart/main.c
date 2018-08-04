/*$Id: main.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#include "cgi_lib.h"
#include "cgi_services.h"

int main()
{
	network_info	*netInfo;
	interface_info *wanInfo, *lanInfo;

	netInfo = cgi_get_network_info( 0);
	wanInfo = &netInfo->wanInfo;
	lanInfo = &netInfo->lanInfo;

	cgi_services_stop_all();

	cgi_boa_rewrite_configuration( wanInfo);
	cgi_boa_rewrite_configuration( lanInfo);

	/* other configuration files need to added here */
	
	return 0;	
}

