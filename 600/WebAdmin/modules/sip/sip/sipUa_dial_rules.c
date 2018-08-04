/*
* $Id: sipUa_dial_rules.c,v 1.11 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

typedef	struct
{
	dial_parttern_t	parttern;
	char 			*pageTitle;
	char				*title;
	char				*col1_title;
	char				*col2_title;
}rule_title_t;


static rule_title_t	_rule_titles[] =
{
	{
		DIAL_PARTTERN_SPEED_DIAL,
		DIAL_PARTTERN_STR_DREICT,	
		SIPUA_OP_SPEED_DIAL,
		CGI_STR_PHONE_NUMBER,
		CGI_STR_SIP_URL	
	},
	{
		DIAL_PARTTERN_LOCAL_DIAL,
		_("Local Number"),
		SIPUA_OP_LOCAL_DIAL,
		_("Branch Phone Number"),
		_("Telephone Port")	
	},
	{
		DIAL_PARTTERN_CALL_RETURN,
		CGI_STR_CALL_RETURN,
		SIPUA_OP_CALL_RETURN,
		_("Short Cut"),
		_("Last Call")	
	},
};

static rule_title_t	*title = NULL;
static llist  	dialRuleCfgs;

#define	FORM_NAME			"dialViewForm"

static char *__sipUa_dial_rules_status(char *ruleKeyword)
{
	char 	buf[8192*4];
	int 		length = 0;
	int		index =0;
	char		formname[128];
#if 0	
	char		targetModify[512];
	char		targetDelete[512];
#endif
	node	*current;
	int		isSpeedDial = 0;
	
	current = dialRuleCfgs.head;
	if(!current )
		return "";

	if(!strcasecmp(ruleKeyword, SIPUA_OP_SPEED_DIAL) )
	{
		isSpeedDial = 1;
	}
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_INDEX), gettext(title->col1_title),gettext(title->col2_title), gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name, ruleKeyword) )
		{

			sprintf(formname, "%s%d", FORM_NAME, index );
#if 0
			sprintf(targetDelete, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, ruleKeyword, CGI_ACTION, CGI_ACTION_DEL,
				DIAL_RULE_TRI, current->entry.value, DIAL_RULE_FOURTH, current->entry.rule );
			sprintf(targetModify, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, ruleKeyword, CGI_ACTION, CGI_ACTION_MODIFY );
			length += CGI_SPRINTF(buf,length, "<TR>\n\t<TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t\t%s\n\t</TD>\r\n</TR>", 
				++index, current->entry.value,current->entry.rule,
				cgi_button_alert(gettext(CGI_STR_DELETE), targetDelete, gettext(CGI_STR_ARE_YOU_SURE)),
				cgi_button(gettext(CGI_STR_MODIFY), targetModify));
			unescape_url(targetDelete);
#else
			length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", formname, WEB_URL_SIP_UA );
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, ruleKeyword);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, (isSpeedDial)?CGI_ACTION_DEL:CGI_ACTION_MODIFY);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_TRI, current->entry.value);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_FOURTH, current->entry.rule);

			length += CGI_SPRINTF(buf,length, "<TR>\n\t<TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t</TD>\r\n</TR></FORM>", 
				++index, current->entry.value,current->entry.rule,
				cgi_submit_button_with_alert(gettext((isSpeedDial)?CGI_STR_DELETE:CGI_STR_MODIFY), gettext(CGI_STR_ARE_YOU_SURE), formname ) );
//				cgi_button_alert(gettext("Delete"), targetDelete, gettext(CGI_STR_ARE_YOU_SURE)) );
#endif
		}
		current = current->next;
	}

	if(!strcasecmp(ruleKeyword, SIPUA_OP_LOCAL_DIAL) )
	{
		length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
		length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf, length, "<form name=\"form\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
		
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, ruleKeyword);
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );
		
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext("Virtual Console Phone Number"), SIPUA_CONSOLE_PHONE_NUMBER, GET_VALUE(&dialRuleCfgs,SIPUA_CONSOLE_PHONE_NUMBER) );
		/* transfered branch after re-dial failed */
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext("Transfered Branch"), SIPUA_LOCAL_EXCHANGE, GET_VALUE(&dialRuleCfgs,SIPUA_LOCAL_EXCHANGE) );
		
		length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
		length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM>\r\n"  );

		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s. <br>* %s.\r\n",
			gettext(CGI_STR_NOTE), 
			gettext("This is the second part of IP Phone Number"),
			gettext("Phone Number defined here must be obligated to Local Dial"),
			gettext("All IP Console shared a same phone number"));

		length += CGI_SPRINTF(buf,length, "<br>* %s, %s. %s, %s.\r\n</TD></TR>",
			gettext("Without SIP Server, with 'branch@host' or 'DeviceId+Branch@host'"),
			gettext("you can dial a Branch without differently. For example"),
			gettext("In order to dial the first branch on AS600 with address is 192.168.3.1"),
			gettext("you can input '80@192.168.3.1' or '6080@192.168.3.1' in software phone"));

	}
	else if(!strcasecmp(ruleKeyword, SIPUA_OP_SPEED_DIAL) )
	{
		length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
		length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s. <br>* %s.\r\n</TD></TR>",
			gettext(CGI_STR_NOTE), 
			gettext("Phone Number defined here must be obligated to Direct Dial"),
			gettext("Defaultly Number is begin with '#' and with other 3 digits"),
			gettext("Phone Number dialed is in format 'IP Prefix'+'#[0-9][0-9][0-9]'") );
	}
	else if(!strcasecmp(ruleKeyword, SIPUA_OP_CALL_RETURN) )
	{
		length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
		length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s.\r\n</TD></TR>",
			gettext(CGI_STR_NOTE), 
			gettext("Phone Number dialed is in format 'IP Prefix'+'*[0-9]'") );
	}
	
	return strdup(buf);

}

static char *__sipUa_dial_rules_buttons(char *ruleKeyword)
{
	char 	buf[4096];
	int 		length = 0;

	if(strcasecmp( ruleKeyword,SIPUA_OP_SPEED_DIAL ))
		return "";
	
	length += CGI_SPRINTF(buf, length, "<form name=\"dialViewForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, ruleKeyword);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<input name=\"%s\" type=\"text\" size=\"15\" value=\"\" onKeyPress=\"checkNumber(this)\" style=\"\" maxlength=\"16\"></TR>\n", 
		gettext(title->col1_title), DIAL_RULE_TRI );
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<input name=\"%s\" type=\"text\" size=\"15\" value=\"\" onKeyPress=\"checkNumber(this)\" style=\"\" maxlength=\"40\"></TR>\n", 
		gettext(title->col2_title), DIAL_RULE_FOURTH );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_SIPUA_DIAL_RULE) );

	return strdup(buf);
}


char *sipUa_dial_rule_modify_buttons(llist *cgiVars)
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"dialViewForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(cgiVars));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_FOURTH, GET_VALUE(cgiVars, DIAL_RULE_FOURTH) );

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD>\r\n", 
		gettext(title->col2_title), GET_VALUE(cgiVars, DIAL_RULE_FOURTH) );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext(title->col1_title), DIAL_RULE_TRI, GET_VALUE(cgiVars,DIAL_RULE_TRI) );
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR></FORM>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	return strdup(buf);
}

/* update Dial_Pattern and other Dial Rules defined ua.cfg */
int sipUa_update_dial_rules(llist *rawCfgs, llist *cgiVariables)
{
	char		*ruleType;
	char		*action;		/* add or delete */
	char		*triValue;
	char		*forthValue;
	node	*current;

	char		msg[1024];

	forthValue = GET_VALUE(cgiVariables, DIAL_RULE_FOURTH);
	triValue = GET_VALUE(cgiVariables, DIAL_RULE_TRI);
	action = GET_CGI_ACTION(cgiVariables);
	ruleType = GET_CGI_OP(cgiVariables );
	if(!forthValue ||!strlen(forthValue)||!triValue||!strlen(triValue) 
		|| !action ||!strlen(action) || !ruleType || !strlen(ruleType) )
	{
		sprintf(msg, "%s", gettext("Error Fields exist when update SIP UA configuration") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
		return -1;	
	}

	cgidebug("ruleType=%s\t%s\t%s\n", ruleType, triValue, forthValue);
	current = rawCfgs->head;

	while(current)
	{
		if(!strcasecmp(current->entry.name, ruleType) &&
			!strcasecmp(current->entry.rule, forthValue ) )
		{
			if( strcasecmp(action, CGI_ACTION_SAVE) )
			{
				if(!strcasecmp( current->entry.value, triValue ) )
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

	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(!current && !strcasecmp(action, CGI_ACTION_SAVE/*modify*/) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s', '%s')\" %s", ruleType?ruleType:"NULL", triValue?triValue:"NULL", 
			forthValue?forthValue:"NULL", (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(current)
	{/* delete or modify */
		if(! strcasecmp(action, CGI_ACTION_DEL) )
			list_delete_node(rawCfgs, current);
		else/* modify */
			sprintf(current->entry.value, "%s", triValue);
	}
	else
	{/* add */
		list_append_raw(rawCfgs, ruleType,triValue, "string", forthValue );
	}

	return sipUa_write_config_file(rawCfgs, ruleType);
}


int	sipUa_dial_rules(llist *cgiVariables, char *cmdKeyword)
{
	char			*action;
	int			i;
	rule_title_t	*_title;

	_title = _rule_titles;
	for(i=0;i<sizeof(_rule_titles)/sizeof(_rule_titles[0]); i++)
	{
		if(!strcasecmp(_title->title, cmdKeyword) )
			title = _title;
		_title++;	
	}

	if(!title)
	{
		char		msg[256];
		sprintf(msg, "'%s' %s", cmdKeyword, gettext("Dial Rule is not support") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	list_create(&dialRuleCfgs);
	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &dialRuleCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		return cgi_info_page(gettext(title->pageTitle), __sipUa_dial_rules_status(cmdKeyword),__sipUa_dial_rules_buttons(cmdKeyword) );
	}
	else if(!strcasecmp(action, CGI_ACTION_MODIFY) )
	{
		return cgi_info_page(gettext(title->pageTitle), sipUa_dial_rule_modify_buttons(cgiVariables), "");
	}
	else if(!strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{
		list_replace_with_new_list(&dialRuleCfgs, cgiVariables);
		return sipUa_write_config_file(&dialRuleCfgs, cmdKeyword);
	}
	else
	{
		return sipUa_update_dial_rules(&dialRuleCfgs, cgiVariables);
	}

	return 0;
}

