/*
* $Id: ddnsUtils.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgiDdns.h"
#include <netdb.h>

int PeanutInitConfig(PEANUT_INFO *info)
{
	list_create(&info->configs);
	if(readconfig(PEANUT_CONFIG_FILE, ITEM_EQU,NULL, &info->configs))
	{
		printf("Without PEANUT-HULL configuration file : %s\n", PEANUT_CONFIG_FILE);
		return -1;
	}

	sprintf(info->serverIp, "%s",  GET_VALUE(&info->configs, PEANUT_KEYWORD_SERVER));
	info->username = GET_VALUE(&info->configs, PEANUT_KEYWORD_USERNAME );
	info->password = GET_VALUE(&info->configs, PEANUT_KEYWORD_PASSWD);
	info->localUdpPort = atoi(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOCAL_UDP) );
#if 0
	info->fqdn = GET_VALUE(&info->configs, PEANUT_KEYWORD_FQDN );
#else
	sprintf(info->fqdn, "%s.%s",  cgi_host_name_read(), cgi_domain_name_read() );
#endif
	info->port = atoi(GET_VALUE( &info->configs, PEANUT_KEYWORD_PORT)  );

	if(!strlen(info->username) || !strlen(info->password) || !strlen(info->fqdn) )
	{
		printf("Error in PEANUT-HULL Configuration File(Field of UserName, Password and FQDN)\n");
		return -2;
	}

	if(!strcasecmp(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_TYPE), PEANUT_LOG_TYPE_FILE ) )
	{
		info->logStyle = USE_FILE;
	}
	else if(!strcasecmp(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_TYPE), PEANUT_LOG_TYPE_CONSOLE ) )
	{
		info->logStyle = USE_CONSOLE;
	}
	
	info->logLevel = atoi(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_LEVEL) );
	if(info->logLevel< AS_LOG_EMERG || info->logLevel > AS_LOG_DEBUG)
		info->logLevel = AS_LOG_NOTICE;
		

	return 0;
}

char *asInetNtoa(long ip)
{
	struct in_addr addr;
	addr.s_addr = ip;
	return inet_ntoa(addr);
}

/* return IP Address with number and dot format */
char	*PeanutTestRegisterResult(char *hostName)
{
	struct hostent * hostbyname;
	struct sockaddr_in address;
	
	hostbyname = gethostbyname( hostName);
	if ( hostbyname == NULL )
	{
		printf( "SendPacket get \"%s\" hostbyname failed. \n", hostName );
		return NULL;
	}
	
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	memcpy( &address.sin_addr.s_addr, hostbyname->h_addr, sizeof( address.sin_addr.s_addr ) );

	return asInetNtoa(address.sin_addr.s_addr);
}

