/*$Id: netQos.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__NET_QOS_H__
#define	__NET_QOS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"


#define	QOS_CFG_BANDWIDTH		"Bandwidth"

#define	QOS_BW_128K				"128Kbit"
#define	QOS_BW_256K				"256Kbit"
#define	QOS_BW_512K				"512Kbit"
#define	QOS_BW_1M					"1024Kbit"
#define	QOS_BW_2M					"2048Kbit"
#define	QOS_BW_5M					"5120Kbit"
#define	QOS_BW_10M				"10240Kbit"


#define	QOS_BW_NAME_128K			"128K"
#define	QOS_BW_NAME_256K			"256K"
#define	QOS_BW_NAME_512K			"512K"
#define	QOS_BW_NAME_1M			"1M"
#define	QOS_BW_NAME_2M			"2M"
#define	QOS_BW_NAME_5M			"5M"
#define	QOS_BW_NAME_10M			"10M"


typedef struct 
{
	char				*bandwidth;
	char				*description;
}qos_bandwidth_t;

#endif

