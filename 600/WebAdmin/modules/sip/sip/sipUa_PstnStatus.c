/*
* $Id: sipUa_PstnStatus.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

static llist  	pstnLineCfgs;

#define	FORM_NAME			"pstnLineForm"

static char *__sipUa_pstnline_status(llist *configs, llist *cgiVariables)
{
	char 	buf[8192*4];
	int 		length = 0;
	int		index =0;
	char		formname[128];

	node	*current;
	
	current = configs->head;
	if(!current )
		return "";

	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>PSTN %s %s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Lines"), gettext(CGI_STR_PORT), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name, SIPUA_PSTN_HW_STATUS) )
		{

			sprintf(formname, "%s%d", FORM_NAME, index++ );
			length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", formname, WEB_URL_SIP_UA );
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(cgiVariables));
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_TRI, current->entry.value);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_FOURTH, current->entry.rule);

			length += CGI_SPRINTF(buf,length, "<TR>\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t</TD>\r\n</TR></FORM>", 
				current->entry.value, (strcasecmp(current->entry.rule, SIPUA_VALUE_TRUE)==0 )?gettext(CGI_STR_ON):gettext(CGI_STR_OFF),
				cgi_submit_button_with_alert(gettext(CGI_STR_MODIFY), gettext(CGI_STR_ARE_YOU_SURE), formname ) );
		}
		current = current->next;
	}

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), 
		gettext("Define which PSTN Lines can be used when dialing PSTN from IP") );
	
	return strdup(buf);

}


char *sipUa_pstnline_modify_buttons(llist *configs, llist *cgiVars)
{
	char 	buf[4096];
	int 		length = 0;
	int		isTrue;

	length += CGI_SPRINTF(buf, length, "<form name=\"pstnLineForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(cgiVars));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_TRI, GET_VALUE(cgiVars, DIAL_RULE_TRI) );

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>PSTN %s %s:</strong></TD><TD align=\"left\">%s</TD>\r\n", 
		gettext("Lines"), gettext(CGI_STR_PORT), GET_VALUE(cgiVars, DIAL_RULE_TRI) );
	isTrue = (strcasecmp(GET_VALUE(cgiVars,DIAL_RULE_FOURTH), SIPUA_VALUE_TRUE)==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD><TD colspan=2></TD></TR>\r\n", 
		gettext(CGI_STR_STATUS), DIAL_RULE_FOURTH, SIPUA_VALUE_TRUE, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), DIAL_RULE_FOURTH,SIPUA_VALUE_FALSE, (isTrue==1)?"":"checked" , gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR></FORM>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	return strdup(buf);
}

/* update Dial_Pattern and other Dial Rules defined ua.cfg */
int sipUa_update_pstnlines_status(llist *rawCfgs, llist *cgiVariables)
{
	char		*action;		/* add or delete */
	char		*triValue;
	char		*forthValue;
	node	*current;

	char		msg[1024];

	forthValue = GET_VALUE(cgiVariables, DIAL_RULE_FOURTH);
	triValue = GET_VALUE(cgiVariables, DIAL_RULE_TRI);
	action = GET_CGI_ACTION(cgiVariables);
	if(!forthValue ||!strlen(forthValue)||!triValue||!strlen(triValue) 
		|| !action ||!strlen(action) )
	{
		sprintf(msg, "%s", gettext("Error Fields exist when update SIP UA configuration") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
		return -1;	
	}

	current = rawCfgs->head;
	while(current)
	{
		if(!strcasecmp(current->entry.name, SIPUA_PSTN_HW_STATUS) &&
			!strcasecmp(current->entry.value, triValue ) )
		{
			if( strcasecmp(action, CGI_ACTION_SAVE) )
			{
				if(!strcasecmp( current->entry.rule, forthValue ) )
				{
					break;
				}
			}	
			else
			{
				break;
			}
		}
		current = current->next;
	}

	if(	(!current && !strcasecmp(action, CGI_ACTION_SAVE/*modify*/) )    )
	{
		sprintf(msg, "\"PSTN %s('%s', '%s')\" %s", gettext("Lines"), triValue?triValue:"NULL", 
			forthValue?forthValue:"NULL", gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(current)
	{/* delete or modify */
		sprintf(current->entry.rule, "%s", forthValue);
	}

	return sipUa_write_config_file(rawCfgs, GET_CGI_OP(cgiVariables));
}


int	sipUa_PstnStatus(llist *cgiVariables)
{
	char			*action;
	char			buf[256];

	list_create(&pstnLineCfgs);
	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &pstnLineCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	CGI_SPRINTF(buf, 0, "PSTN %s %s", gettext("Lines"), gettext(CGI_STR_CONFIGURATION) );
	action = GET_VALUE(cgiVariables, CGI_ACTION );

	if(action && (!strcasecmp(action, CGI_ACTION_MODIFY)) )
	{
		return cgi_info_page(buf, sipUa_pstnline_modify_buttons(&pstnLineCfgs,cgiVariables), "");
	}
	else if(action && (!strcasecmp(action, CGI_ACTION_SAVE)) )
	{
		return sipUa_update_pstnlines_status(&pstnLineCfgs, cgiVariables);
	}
	
	cgi_info_page( buf, "", __sipUa_pstnline_status(&pstnLineCfgs, cgiVariables) );

	return 0;
}

