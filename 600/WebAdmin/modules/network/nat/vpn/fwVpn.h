/*$Id: fwVpn.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__FW_VPN_H__
#define	__FW_VPN_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"

typedef	enum
{
	VPN_PPTP_IN = 0,
	VPN_PPTP_OUT,
	VPN_PPTP_BOTH
}vpn_pptp_type_t;

#define	VPN_PPTP_TYPE_IN			_("IN")
#define	VPN_PPTP_TYPE_OUT			_("OUT")
#define	VPN_PPTP_TYPE_BOTH		_("BOTH")


#define	VPN_CFG_TYPE				"Pptp_Type"
#define	VPN_CFG_GATEWAY			"Pptp_Gateway"
#define	VPN_CFG_LOCAL_IP			"Pptp_LocalIp"

typedef struct 
{
	vpn_pptp_type_t	type;
	char				*description;
}pptp_type;


#endif

