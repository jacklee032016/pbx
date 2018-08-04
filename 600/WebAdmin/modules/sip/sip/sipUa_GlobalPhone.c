/*
* $Id: sipUa_GlobalPhone.c,v 1.4 2007/03/15 16:54:21 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

static llist  	protocolCfgs;

static char *__sipUa_global_status(llist *cgiVars)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		isTrue = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_VALUE(cgiVars,CGI_KEYWORD_OPERATOR));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE);

/* device config */
	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	/* device type and device prefix */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("Device Type") );
	isTrue = strcasecmp(GET_VALUE(&protocolCfgs, SIPUA_DEVICE_NAME), SIPUA_DEVICE_TYPE_PCM);
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' disabled>\n", 
		SIPUA_DEVICE_NAME );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>PCM</option>\n", 
		SIPUA_DEVICE_TYPE_PCM, (isTrue==0)?"selected":"" );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>SLIC</option>\n", 
		SIPUA_DEVICE_TYPE_SLIC, (isTrue==0)?"":"selected"  );
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Local Name"), SIPUA_LOCAL_DEVICE, GET_VALUE(&protocolCfgs,SIPUA_LOCAL_DEVICE) );

#if 0	
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Country Number"), SIPUA_LOCAL_COUNTRY, GET_VALUE(&protocolCfgs,SIPUA_LOCAL_COUNTRY) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("District Number"), SIPUA_LOCAL_DISTRICT, GET_VALUE(&protocolCfgs,SIPUA_LOCAL_DISTRICT) );
#endif

#if 0
	/* PSTN line number and call Prefix of PSTN */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("PSTN Lines Number"), gettext("PSTN Lines Number"), SIPUA_PSTN_COUNT, GET_VALUE(&protocolCfgs,SIPUA_PSTN_COUNT) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Branch Number"), SIPUA_LOCAL_BRANCH, GET_VALUE(&protocolCfgs,SIPUA_LOCAL_BRANCH) );
#endif

	/* number of virtual console for IP */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Number of Virtaul IP Console"),gettext("Console Number"), SIPUA_CONSOLE_COUNT, GET_VALUE(&protocolCfgs,SIPUA_CONSOLE_COUNT) );

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s%s. %s.<br>* %s, %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), 
		gettext("IP Phone Number is Phone Number dialed within IP Network"),
		gettext(" and registered on SIP Server"),gettext("it is made of 'Device ID'+'IP Branch|Console Phone Number'"),
		gettext("When dialed within IP network, only these 2 parts is needed"), 
		gettext("When dialed from AS600 telephone, 'IP Prefix' must be dialed firstly"));


	return strdup(buf);

}


int	sipUa_global(llist *cgiVariables)
{
	char		*action;

	list_create(&protocolCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &protocolCfgs) != 0)
	{
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Global Number"), gettext("Please select action which you want to"), __sipUa_global_status(cgiVariables) );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_SAVE)  )
	{
		list_replace_with_new_list(&protocolCfgs, cgiVariables );
		return sipUa_write_config_file(&protocolCfgs, GET_VALUE(cgiVariables,CGI_KEYWORD_OPERATOR) );
	}

	return 0;
}


