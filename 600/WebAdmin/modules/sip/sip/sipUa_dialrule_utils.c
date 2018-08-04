/*
* $Id: sipUa_dialrule_utils.c,v 1.2 2007/03/13 20:15:01 lizhijie Exp $
*/

#include "sipUa.h"


int sipUa_write_config_file(llist *uaCfgs, char *operator)
{
	char		target[512];
	
	writeconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, uaCfgs);
	
	sprintf(target, "%s?%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, operator );

	cgi_refresh_page(1, target, gettext("Writing SIP UA Configuration File"), gettext(CGI_STR_PLEASE_WAIT) );
	return 0;

}

