
#include "servicesMisc.h"
#include "cgi_services.h"

static char *__servicesMisc_ipset_status()
{
	char 	buf[4096];
	int 		length = 0;
	

	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_NAME), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("IPSET"), cgi_service_get_state_name(SERVICE_NAME_IPSET), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );
		return strdup(buf);
}

int	servicesMisc_Ipset(SERVICE_INFO *info)
{
	cgi_info_page(gettext("IPSET Configuration"), __servicesMisc_ipset_status(), "" );
	return 0;
}

