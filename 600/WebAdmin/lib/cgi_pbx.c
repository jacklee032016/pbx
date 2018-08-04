/*
* $Id: cgi_pbx.c,v 1.1 2007/05/16 18:37:50 lizhijie Exp $
*/

#include "cgi_pbx.h"

cgi_transfer_type_t cgi_transfer_types[3] =
{
#if 0
	{
		EXT_TRANSFER_TYPE_LOCAL,
		"branch",	
	},
#endif	
	{
		EXT_TRANSFER_TYPE_IP,
		"IP",
	},
	{
		EXT_TRANSFER_TYPE_PSTN,
		"PSTN"	
	},
	{
		EXT_TRANSFER_TYPE_UNKNOWN,
		"Cancel"	
	},
};

char *stc_tranfer_type_name(int type)
{
	int i ;

	for(i=0; i<sizeof(cgi_transfer_types)/sizeof(cgi_transfer_type_t); i++)
	{
		if(cgi_transfer_types[i].type == type)
			return cgi_transfer_types[i].descriptor;
	}

	return cgi_transfer_types[2].descriptor;
}

int stc_tranfer_type(char *descriptor)
{
	int i ;

	for(i=0; i<sizeof(cgi_transfer_types)/sizeof(cgi_transfer_type_t); i++)
	{
		if( !strcasecmp(cgi_transfer_types[i].descriptor, descriptor) )
			return cgi_transfer_types[i].type;
	}

	return EXT_TRANSFER_TYPE_UNKNOWN;
}

