/*
* $Id: sipUa_dial_parttern.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "_sipUa.h"

#define	BLACK_VIEW_PHONE_NUMBER				"PHONE_NUMBER"

#define	BLACK_VIEW_ACTION_TYPE_ADD			"add"
#define	BLACK_VIEW_ACTION_TYPE_DELETE		"delete"

static llist  	dialPartternCfgs;

typedef struct
{
	char 				*strType;
	dial_parttern_state_t	state;
	dial_parttern_t		parttern;
}dial_type_t;


static dial_type_t sip_dials[] =
{	
	{
		DIAL_PARTTERN_STR_DREICT,
		DIAL_STATE_ENABLE,
		DIAL_PARTTERN_SPEED_DIAL
	},
	{
		CGI_STR_INTERNAL_DIAL,
		DIAL_STATE_DISABLE,
		DIAL_PARTTERN_SPECIAL_KEY_DIAL
	},
	{
		DIAL_PARTTERN_STR_SERVER,
		DIAL_STATE_ENABLE,
		DIAL_PARTTERN_NORMAL_DIAL
	},
	{
		CGI_STR_LOCAL_DIAL,
		DIAL_STATE_ENABLE,
		DIAL_PARTTERN_LOCAL_DIAL
	},
	{
		CGI_STR_CALL_RETURN,
		DIAL_STATE_ENABLE,
		DIAL_PARTTERN_CALL_RETURN
	},
	{
		CGI_STR_URL_DIAL,
		DIAL_STATE_DISABLE, 
		DIAL_PARTTERN_URL_DIAL
	},
	{
		CGI_STR_PSTN_DIAL,
		DIAL_STATE_DISABLE,
		DIAL_PARTTERN_PSTN_DIAL
	},
	{
		CGI_STR_DIRECT_DIAL,
		DIAL_STATE_DISABLE,
		DIAL_PARTTERN_DIRECT_DIAL
	},
	{
		NULL,
		-1,
		-1
	}
};

#define	FORM_NAME		"dialPatternForm"


static char *__sipUa_dial_parttern_status(llist *cgiVars)
{
	char 	buf[4096];
	int 		length = 0;
	int		index =0;
	char		formname[128];
#if 0	
	char		targetModify[256];
	char		targetDelete[256];
#endif
	node	*current = dialPartternCfgs.head;
	if(!current )
		return "";
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_INDEX), gettext("Parttern Type"),gettext("Dial Parttern"), gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name, SIPUA_OP_DIAL_PATTERN) )
		{
			int			parttern = atoi(current->entry.value);
			int			isEnable = 0;
			dial_type_t	*type = sip_dials;
			while(type->parttern != -1)
			{
				if(type->parttern == parttern)
				{
					if(type->state == DIAL_STATE_ENABLE )
					{
						isEnable = 1;
						break;
					}
				}

				type++;
			}

			if(!isEnable)
				break;
			 
			sprintf(formname, "%s%d", FORM_NAME, index);
#if 0
			sprintf(targetDelete, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_DIAL_PATTERN, CGI_ACTION,  CGI_ACTION_DEL,
				DIAL_RULE_TRI, current->entry.value, DIAL_RULE_FOURTH, current->entry.rule );
			sprintf(targetModify, "%s?%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_DIAL_PATTERN, CGI_ACTION,  CGI_ACTION_DEL);
			length += CGI_SPRINTF(buf,length, "\n\t<TR><TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t\t%s\n\t</TD></TR>\n", 
				++index, current->entry.value,current->entry.rule,
				cgi_button_alert(gettext(CGI_STR_DELETE), targetDelete, gettext(CGI_STR_ARE_YOU_SURE)),
				cgi_button(gettext(CGI_STR_MODIFY), targetModify));
			unescape_url(targetDelete);
#else
			length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", formname, WEB_URL_SIP_UA );
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR,  GET_CGI_OP(cgiVars) );
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_TRI, current->entry.value);
			length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_FOURTH, current->entry.rule);

			length += CGI_SPRINTF(buf,length, "\n\t<TR><TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t</TD></TR></FORM>\n", 
				++index, gettext( (type )->strType ), current->entry.rule,
				cgi_submit_button_with_alert(gettext(CGI_STR_MODIFY), gettext(CGI_STR_ARE_YOU_SURE), formname ) );
//				cgi_button_alert(gettext("Delete"), targetDelete, gettext(CGI_STR_ARE_YOU_SURE)) );
#endif
		}
		current = current->next;
	}


	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* %s</strong> : %s.<br><strong>* %s</strong> : %s.<br><strong>* %s</strong> : %s.<br>* %s, %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), 
		gettext(DIAL_PARTTERN_STR_DREICT), gettext("used to defined IP Phone Number destinated to a remote host"),
		gettext(DIAL_PARTTERN_STR_SERVER),  gettext("used to defined IP Phone Number registered on SIP Server"),
		gettext(CGI_STR_LOCAL_DIAL), gettext("used to defined Local IP Branch and Console telephone number"), 
		gettext("where,'^' is begin of dial rule, '#' is '*' button in Dial Panel"), gettext("'[0-9]' indicate a digit between 0 and 9, etc."));

	return strdup(buf);

}

#if SIP_DIALPLAN_ADD
static char *__sipUa_dial_parttern_buttons()
{
	char 	buf[4096];
	int 		length = 0;
	int		i;
	dial_type_t	*type = sip_dials;

	length += CGI_SPRINTF(buf, length, "<form name=\"dialViewForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_DIAL_PATTERN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext("Parttern Type"), DIAL_RULE_TRI );

	while(type->parttern != -1)
	{
		if(type->state == DIAL_STATE_ENABLE)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%s</option>\n", type->parttern, gettext(type->strType) );
		}
		type++;
	}	

	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n", gettext("Dial Parttern") );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		DIAL_RULE_FOURTH);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_SIPUA_DIAL_RULE) );

	return strdup(buf);
}
#endif

char *sipUa_dial_modify_buttons(llist *cgiVars)
{
	char 	buf[4096];
	int 		length = 0;
	int		isFound = 0;
	char		*triValue = GET_VALUE(cgiVars,DIAL_RULE_TRI );

	int			parttern = atoi( triValue);
	dial_type_t	*type = sip_dials;
	while(type->parttern != -1)
	{
		if(type->parttern == parttern)
		{
			if(type->state == DIAL_STATE_ENABLE )
			{
				isFound = 1;
				break;
			}
		}

		type++;
	}
	if(!isFound)
	{
		return gettext("Dial Rule is not found");
	}

	length += CGI_SPRINTF(buf, length, "<form name=\"dialViewForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(cgiVars));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DIAL_RULE_TRI, triValue );

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD>\r\n", 
		gettext(CGI_STR_TYPE), gettext(type->strType) );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("Regular Express is used to define Dial Rule"),gettext("Dial Rule"), DIAL_RULE_FOURTH, GET_VALUE(cgiVars,DIAL_RULE_FOURTH) );
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR></FORM>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	return strdup(buf);
}

/* update Dial_Pattern defined ua.cfg */
int sipUa_update_dial_patterns(llist *rawCfgs, llist *cgiVariables)
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
			!strcasecmp(current->entry.value, triValue) )
		{
			if( strcasecmp(action, CGI_ACTION_SAVE) )
			{
				if(!strcasecmp( current->entry.rule, forthValue) )
				{
					cgitrace;
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
			sprintf(current->entry.rule, "%s", forthValue);
	}
	else
	{/* add */
		list_append_raw(rawCfgs, ruleType,triValue, "string", forthValue );
	}

	return sipUa_write_config_file(rawCfgs, ruleType);
}



int	sipUa_dial_parttern(llist *cgiVars)
{
	char		*action;

	list_create(&dialPartternCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &dialPartternCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_CGI_ACTION(cgiVars);
	if(!action || !strlen(action) )
	{
#if SIP_DIALPLAN_ADD
		return cgi_info_page(gettext("IP Dialing"), __sipUa_dial_parttern_status(cgiVars),__sipUa_dial_parttern_buttons() );
#else
		return cgi_info_page(gettext("IP Dialing"), __sipUa_dial_parttern_status(cgiVars),"");
#endif
	}
	else if(!strcasecmp(action, CGI_ACTION_MODIFY) )
	{
		return cgi_info_page(gettext("IP Dialing"), sipUa_dial_modify_buttons(cgiVars), "");
	}
	else
	{
		return sipUa_update_dial_patterns(&dialPartternCfgs, cgiVars);
	}

	return 0;
}

