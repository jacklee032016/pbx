/*
*
*/

#ifndef	__NET_ROUTES_H__
#define	__NET_ROUTES_H__

#include <stdio.h>
#include <string.h>
#include "cgiLib.h"

#define	NET_ROUTE_COMMAND					"/sbin/route"

#define	NET_ROUTE_CONF						"/etc/sys/routes.conf"

#define	NET_ROUTE_STR_CONFIGURATION			_("Network Route Configuration")
#define	NET_ROUTE_STR_STEP					CGI_STR_STEP


#define	NET_ROUTE_TYPE						"ROUTE_TYPE"
#define	NET_ROUTE_TYPE_NET					"net"
#define	NET_ROUTE_TYPE_HOST					"host"
#define	NET_ROUTE_TYPE_DEFAULT				"default"


#define	NET_ROUTE_OP_ADD						"add"
#define	NET_ROUTE_OP_DEL						"del"
#define	NET_ROUTE_OP_SAVE					"save"		/* just save kernel route in config file : /etc/sys/routes.conf */

#define	NET_ROUTE_NAME_DESTINATION			"DESTIN"		/* not used in default ROUTE */
#define	NET_ROUTE_NAME_GATEWAY				"GATEWAY"
#define	NET_ROUTE_NAME_INTERFACE			"INTERFACE"

#define	NET_ROUTE_NAME_NETMASK				"NETMASK"	/* only used in network route */

#define	ROUTE_NAME_LENGTH		32
struct _cgi_route_entry
{
	char		dest[ROUTE_NAME_LENGTH];
	char		gateway[ROUTE_NAME_LENGTH];
	char		mask[ROUTE_NAME_LENGTH];
	char		flag[ROUTE_NAME_LENGTH];
	char		metric[ROUTE_NAME_LENGTH];
	char		ref[ROUTE_NAME_LENGTH];
	char		use[ROUTE_NAME_LENGTH];
	char		iface[ROUTE_NAME_LENGTH];
	
	char		type[ROUTE_NAME_LENGTH];

	char		op[ROUTE_NAME_LENGTH];

	struct _cgi_route_entry	*next;
};

typedef struct _cgi_route_entry 	cgi_route_entry_t;


int net_routes_read_route_table( );
int net_routes_update(cgi_route_entry_t *route);
int net_routes_write_conf();

extern	cgi_route_entry_t		*runtimeRoutes;


typedef	struct
{
	llist  				routeCfgs;
	
	llist				cgiVariables;

	network_info		*netInfo;
}ROUTE_INFO;

#endif

