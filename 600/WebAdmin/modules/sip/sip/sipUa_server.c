/*
* $Id: sipUa_server.c,v 1.11 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

static llist  	serversCfgs;

static char *__sipUa_server_status()
{
	char 	buf[8192*5];
	int 		length = 0;
	char		target[128];
	int		isTrue = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_SERVER);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE);

/* table1 : proxy server, out-call */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Proxy Server Configuration(Dial Out)") );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Using IP Address '0.0.0.0' to disable Proxy Server"),gettext("IP/FQDN"), SIPUA_PROXY_SERVER, GET_VALUE(&serversCfgs,SIPUA_PROXY_SERVER) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" ><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_PORT), SIPUA_PROXY_PORT, GET_VALUE(&serversCfgs,SIPUA_PROXY_PORT) );
	length += CGI_SPRINTF(buf, length, "</TR></TABLE>\n" );

/* TABLE 2 : register server : in-call */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Registration Server Configuration(Dial In)") );

	isTrue = (strcasecmp(GET_VALUE(&serversCfgs, SIPUA_REGISTER_ON), SIPUA_VALUE_TRUE)==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD><TD colspan=2></TD></TR>\r\n", 
		gettext("Register"), SIPUA_REGISTER_ON, SIPUA_VALUE_TRUE, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), SIPUA_REGISTER_ON,SIPUA_VALUE_FALSE, (isTrue==1)?"":"checked" , gettext(CGI_STR_OFF) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Register Server's address or FQDN"),gettext("IP/FQDN"), SIPUA_REGISTER_SERVER, GET_VALUE(&serversCfgs,SIPUA_REGISTER_SERVER) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" ><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_PORT), SIPUA_REGISTER_PORT, GET_VALUE(&serversCfgs,SIPUA_REGISTER_PORT) );
/*
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Specify the domain of local"),gettext("Register To"), SIPUA_REGISTER_TO, GET_VALUE(&serversCfgs,SIPUA_REGISTER_TO) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("it is often equal to the Register To"),gettext("Register From"), SIPUA_REGISTER_FROM, GET_VALUE(&serversCfgs,SIPUA_REGISTER_FROM) );
*/
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD>\r\n", 
		gettext("The time of request Register to Server"),gettext("Expire Time"), SIPUA_REGISTER_EXPIRE, GET_VALUE(&serversCfgs,SIPUA_REGISTER_EXPIRE), gettext(CGI_STR_SECONDS) );
	isTrue = strcasecmp(GET_VALUE(&serversCfgs, SIPUA_REGISTER_ACTION), SIPUA_REGISTER_ACTION_PROXY);
	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("Server Type"), SIPUA_REGISTER_ACTION);
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		SIPUA_REGISTER_ACTION_PROXY, (isTrue==0)?"selected":"", SIPUA_REGISTER_ACTION_PROXY );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		SIPUA_REGISTER_ACTION_REDIRECT,  (isTrue==0)?"":"selected", SIPUA_REGISTER_ACTION_REDIRECT);
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );
	

/* TABLE 3 : authentication info */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Authentication Configuration") );

	isTrue = (strcasecmp(GET_VALUE(&serversCfgs, SIPUA_AUTHENTICATION), SIPUA_AUTHEN_TYPE_LOCAL)==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD>\r\n", 
		gettext(CGI_STR_AUTHENTICATION), SIPUA_AUTHENTICATION, SIPUA_AUTHEN_TYPE_LOCAL, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), SIPUA_AUTHENTICATION, SIPUA_AUTHEN_TYPE_OFF, (isTrue==1)?"":"checked" , gettext(CGI_STR_OFF) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"16\"></TD></TR>\r\n", 
		gettext("Device/Division Name"), SIPUA_DISPLAY_NAME, GET_VALUE(&serversCfgs,SIPUA_DISPLAY_NAME) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Define machines number,Propose use number"),gettext(CGI_STR_USER_NAME), SIPUA_USER_NAME, GET_VALUE(&serversCfgs,SIPUA_USER_NAME) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"password\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext(CGI_STR_PASSWORD), SIPUA_PASS_WORD,GET_VALUE(&serversCfgs,SIPUA_PASS_WORD));
	
	length += CGI_SPRINTF(buf, length, "</TR></TABLE>\n" );

/* TABLE 4 : Button info */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	sprintf(target, "%s?%s=%s&%s=%s",WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_SERVER, CGI_ACTION, CGI_ACTION_DEFAULT);
	length += CGI_SPRINTF(buf,length, "\t%s\t%s</TD></TR></FORM>\r\n", cgi_button(gettext(CGI_STR_RESTORE_DEFAULT), target) , cgi_help_button(CGI_HELP_MSG_SIPUA_PROTOCOL) );

/* table 5 : Notes */
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* %s : </strong>%s! %s. %s\r\n",
		gettext(CGI_STR_NOTE), gettext(CGI_STR_AUTHENTICATION), 
		gettext("here is defination of local authentication"), 
		gettext("eg. authenticate other SIP UA client when we play as SIP UA Server"), 
		gettext("authentication is determined by Server when use Register/Proxy Server"));
	length += CGI_SPRINTF(buf,length, "<br><strong>* %s : </strong>%s, %s!%s!\r\n",
		gettext("Device/Division Name"), 
		gettext("which is description for this device or sub-devision"), 
		gettext("can be viewed on SIP Server to make management easy"),
		gettext("Normally, no more than 16 characters is used") );
	length += CGI_SPRINTF(buf,length, "<br><strong>* %s/%s : </strong>%s!%s, %s. </TD></TR>\r\n",
		gettext(CGI_STR_USER_NAME), gettext(CGI_STR_PASSWORD),
		gettext("which are provided by SIP Server Administrator"), 
		gettext("Every device just need only one user name/password"),
		gettext("Multiple device can share the same user name/password pair") );

	return strdup(buf);

}


int	sipUa_Server(llist *cgiVariables)
{
	char		*action;

	list_create(&serversCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &serversCfgs) != 0)
	{
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Server Configuration of SIP UA"), gettext("Please select action which you want to"), __sipUa_server_status() );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_SAVE)  )
	{
		list_replace_with_new_list(&serversCfgs, cgiVariables );
		return sipUa_write_config_file(&serversCfgs, SIPUA_OP_SERVER);
	}
	else if(!strcasecmp(action, CGI_ACTION_DEFAULT)  )
	{
		sipUa_Default( &serversCfgs, cgiVariables);
		
		return 0;
	}

	return 0;
}


