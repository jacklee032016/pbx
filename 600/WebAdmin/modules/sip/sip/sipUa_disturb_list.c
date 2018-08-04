/*
* $Id: sipUa_disturb_list.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"

#define	DISTURBLIST_PHONE_NUMBER			"Phone_Number"
#define	DISTURBLIST_START_TIME				"Start_Time"
#define	DISTURBLIST_END_TIME					"End_Time"
#define	DISTURBLIST_TIME_TYPE					"Timer_Type"

typedef enum
{
	DISTURBLIST_TYPE_EVERYDAY	=	0,	/* every day in a week */
	DISTURBLIST_TYPE_MON_2_FRI	=	1,	/* monday to friday */
	DISTURBLIST_TYPE_MON_2_SAT	=	2	/* monday to saturday  */
}_dis_type_t;

typedef	struct
{
	char			*descriptor;
	_dis_type_t	type;
}disturb_timer_t;

typedef struct
{
	char			pattern[15];
	char			phone_number[3];
	char			start_time[20];
	char			end_time[20];
	char			time_type[5];
}disturb_list_t;

disturb_timer_t  dis_timers[] =
{
	{
		_("Every Day in a Week"),
		DISTURBLIST_TYPE_EVERYDAY	
	},
	{
		_("Monday to Friday"),
		DISTURBLIST_TYPE_MON_2_FRI	
	},
	{
		_("Monday to Saturday"),
		DISTURBLIST_TYPE_MON_2_SAT	
	}
};

static llist  	disturbCfgs;

static char *__sipUa_disturb_list_status()
{
	char 	buf[4096];
	int 		length = 0;
	int		index =0;
	char		targetModify[256];
	char		targetDelete[256];
	disturb_list_t	*disturb = NULL;
	
//	current = disturbCfgs.head;
	if(!disturb )
		return "";
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_PHONE_NUMBER),gettext("Start Time"), gettext("End Time"), gettext("Timer Type"),gettext(CGI_STR_MANAGEMENT) );

	while (disturb != 0) 
	{
//		if(!strcasecmp(current->entry.name, ruleKeyword) )
		{
			sprintf(targetModify, "%s?%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_DIAL_PATTERN, CGI_ACTION, CGI_ACTION_MODIFY );
			sprintf(targetDelete, "%s?%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_DIAL_PATTERN, CGI_ACTION, CGI_ACTION_DEL );
			length += CGI_SPRINTF(buf,length, "<TR>\n\t<TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">\n\t\t%s\n\t\t%s\n\t</TD>\r\n</TR>", 
				++index, disturb->phone_number, disturb->start_time, disturb->end_time, disturb->time_type, /* dis_timers[disturb->time_type].descriptor, */
				cgi_button_alert(gettext(CGI_STR_DELETE), targetDelete, gettext(CGI_STR_ARE_YOU_SURE)),
				cgi_button(gettext(CGI_STR_MODIFY), targetModify));
		}
//		current = current->next;
	}
	return strdup(buf);

}

static char *__sipUa_disturb_list_buttons()
{
	char 			buf[4096];
	int 				length = 0;
	int				i;
	disturb_timer_t	*type;

	length += CGI_SPRINTF(buf, length, "<form name=\"disturbForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_SPEED_DIAL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<input name=\"%s\" type=\"text\" size=\"15\" value=\"\" onKeyPress=\"checkNumber(this)\" style=\"\" maxlength=\"16\"></TD></TR>\n", 
		gettext(CGI_STR_PHONE_NUMBER), DISTURBLIST_PHONE_NUMBER );
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<input name=\"%s\" type=\"text\" size=\"15\" value=\"\" onKeyPress=\"checkNumber(this)\" style=\"\" maxlength=\"40\"></TD></TR>\n", 
		gettext("Start Time"), DISTURBLIST_START_TIME);
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<input name=\"%s\" type=\"text\" size=\"15\" value=\"\" onKeyPress=\"checkNumber(this)\" style=\"\" maxlength=\"40\"></TD></TR>\n", 
		gettext("End Time"), DISTURBLIST_END_TIME );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext("Timer Type"), DISTURBLIST_TIME_TYPE );
	type = dis_timers;
	for(i=0; i< sizeof(dis_timers)/sizeof(dis_timers[0]); i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%s</option>\n", 
			type->type, gettext(type->descriptor) );
		type++;
	}	

	length += CGI_SPRINTF(buf,length, "</TD></TR>\n\t<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_SIPUA_DIAL_RULE) );

	return strdup(buf);
}


int	sipUa_disturb_list(llist *cgiVariables )
{
	char		*action;

	list_create(&disturbCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &disturbCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("No Disturb List Management"), __sipUa_disturb_list_status(),__sipUa_disturb_list_buttons() );
		return 0;
	}
#if 0	
	else if(!strcasecmp(action, BLACK_VIEW_ACTION_TYPE_ADD) || !strcasecmp(action, BLACK_VIEW_ACTION_TYPE_DELETE) )
	{
		char		target[256];
		
		/* write config file */
		
		sprintf(target, "%s?%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_BLACKLIST_MGMT);
		cgi_refresh_page(1, target, gettext("Save BlackList File"), gettext(CGI_STR_PLEASE_WAIT) );
	}
#endif

	return 0;
}

