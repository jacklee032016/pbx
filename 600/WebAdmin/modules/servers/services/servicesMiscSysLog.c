/*
* $Id: servicesMiscSysLog.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"

#define	SYSLOGD_CFG_KEYWORD_MAX_SIZE			"log_maxsize"
#define	SYSLOGD_CFG_KEYWORD_HOME_DIR			"home_dir"

#define	SYSLOGD_CFG_DEFAULT_HOME_DIR			"/var/adm"

#define	SYSLOGD_FACILITY_CRON					"cron"
#define	SYSLOGD_FACILITY_FTP						"ftp"
#define	SYSLOGD_FACILITY_KERNEL					"kern"
#define	SYSLOGD_FACILITY_LPR						"lpr"
#define	SYSLOGD_FACILITY_MAIL						"mail"
#define	SYSLOGD_FACILITY_SYSLOG					"syslog"

#define	SYSLOGD_LEVEL_ALL							"*"
#define	SYSLOGD_LEVEL_EMERG						"emerg"
#define	SYSLOGD_LEVEL_ALERT						"alert"
#define	SYSLOGD_LEVEL_CRIT						"crit"
#define	SYSLOGD_LEVEL_ERROR						"err"
#define	SYSLOGD_LEVEL_WARN						"warn"
#define	SYSLOGD_LEVEL_NOTICE						"notice"
#define	SYSLOGD_LEVEL_INFO						"info"
#define	SYSLOGD_LEVEL_DEBUG						"debug"
#define	SYSLOGD_LEVEL_NOLOG						"none"

typedef	struct
{
	char		*key;
	char		*value;
}log_info_t;

log_info_t	_logLevels[]=
{
	{SYSLOGD_LEVEL_ALL		,	CGI_STR_ALL			},
	{SYSLOGD_LEVEL_EMERG	,	_("Emergence")		},
	{SYSLOGD_LEVEL_ALERT	,	_("Alert")				},
	{SYSLOGD_LEVEL_CRIT	,	_("Critical")			},
	{SYSLOGD_LEVEL_ERROR	,	CGI_STR_ERROR		},
	{SYSLOGD_LEVEL_WARN	,	CGI_STR_WARN		},
	{SYSLOGD_LEVEL_NOTICE	,	_("Notice")			},
	{SYSLOGD_LEVEL_INFO		,	_("Information")		},
	{SYSLOGD_LEVEL_DEBUG	,	_("Debug")			},
	{SYSLOGD_LEVEL_NOLOG	,	_("None")			},
	{NULL, NULL}
};

log_info_t	_logActors[] =
{
	{SYSLOGD_FACILITY_KERNEL,	_("OS")		},
	{SYSLOGD_FACILITY_CRON, 	_("Crond")	},
	{SYSLOGD_FACILITY_FTP,		_("FTP")		},
	{SYSLOGD_FACILITY_LPR,		_("Printer")	},
	{SYSLOGD_FACILITY_MAIL,		_("Mail")		},
	{SYSLOGD_FACILITY_SYSLOG,	_("Syslog")	},
	{NULL, NULL }
};

#define	SYSLOGD_DELIMITER					'.'

static llist  	logCfgs;

#define _PATH_LOGCONF	"/usr/etc/syslog.conf"

static int __read_syslog_confg()
{
	char	*type;
	list_create(&logCfgs);

	if (readconfig(_PATH_LOGCONF, ITEM_DBL, NULL, &logCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}
	
	node *current = logCfgs.head;
	while(current)
	{
		type = strchr(current->entry.name, SYSLOGD_DELIMITER);
		if(!type)
		{
//			cgidebug("Syslog config item %s is invalidate\n", current->entry.name);
			current = current->next;
			continue;
		}

		cgidebug("raw : name : '%s' type : '%s'  value : '%s'\n", current->entry.name, current->entry.type, current->entry.value);
		current->entry.name[type - current->entry.name] = '\0';
		type++;
		if(type[0]== '=' )
			type++;
		sprintf(current->entry.type, "%s", current->entry.value );
		sprintf(current->entry.value, "%s", type);

		cgidebug("after : name : '%s' type : '%s'  value : '%s'\n", current->entry.name, current->entry.type, current->entry.value);
		current = current->next;
	};

	return 0;
}

static int __write_syslog_confg(SERVICE_INFO *info)
{
	char	target[256];
	FILE *fp;
	node *current = logCfgs.head;
	
	list_replace_with_new_list(&logCfgs, &info->cgiVariables );

	sprintf(target, "%s?%s=%s", WEB_URL_SERVICES, CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_SYSLOG);
	cgi_refresh_page(1, target, gettext("Save Syslog Config File"), gettext(CGI_STR_PLEASE_WAIT) );

	if(( fp=fopen(_PATH_LOGCONF,"w") )==NULL)
	{
		cgidebug( ": open file %s error : %s\n" ,_PATH_LOGCONF, strerror(errno) );
		return -1;
	}

	while(current!=NULL)
	{
		if(!strcasecmp(current->entry.name, SYSLOGD_CFG_KEYWORD_MAX_SIZE) || 
			!strcasecmp(current->entry.name, SYSLOGD_CFG_KEYWORD_HOME_DIR) )
			fprintf(fp, "%s\t\t%s\n", current->entry.name, current->entry.value);
		else if(!strcasecmp(current->entry.value, SYSLOGD_LEVEL_ALL) || !strcasecmp(current->entry.value, SYSLOGD_LEVEL_NOLOG) )
			fprintf(fp, "%s.%s\t\t%s\n", current->entry.name, current->entry.value, current->entry.type);
		else
			fprintf(fp, "%s.=%s\t\t%s\n", current->entry.name, current->entry.value, current->entry.type);

		current = current->next;
	}
	
	fclose(fp);

	return 0;	
}

static char *__servicesMisc_syslogd_status()
{
	char 	buf[4096];
	int 		length = 0;
	
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("OS Log Daemon Status"), cgi_service_get_state_name(SERVICE_NAME_KLOGD), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("SysLog Daemon Status"), cgi_service_get_state_name(SERVICE_NAME_SYSLOGD), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *__servicesMisc_syslogd_buttons()
{
	char 	buf[8192];
	int 		length = 0;
	char		*value, *type;
	node	*node;
	log_info_t	*actor = _logActors;
	log_info_t	*level = _logLevels;

	length += CGI_SPRINTF(buf, length, "<form name=\"syslogForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_SYSLOG);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=3><strong>%s:</strong>\r\n<input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">(KB)</TD></TR>\r\n", 
		gettext("MAX Size of File"), SYSLOGD_CFG_KEYWORD_MAX_SIZE, GET_VALUE( &logCfgs, SYSLOGD_CFG_KEYWORD_MAX_SIZE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=3><strong>%s:</strong>\r\n<input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">(KB)</TD></TR><TR><TD><br></TD></TR>\r\n", 
		gettext("Log Root Directry"), SYSLOGD_CFG_KEYWORD_HOME_DIR, GET_VALUE( &logCfgs, SYSLOGD_CFG_KEYWORD_HOME_DIR) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"center\"><strong>%s</strong></TD>\r\n<TD align=\"center\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Facility"), gettext(CGI_STR_LOG_LEVEL), gettext(CGI_STR_LOG_FILE) );

	for( actor = _logActors;actor->key; actor++ )
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n", gettext( actor->value) );

		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:120px;'>\n", actor->key );

		
//		value = GET_VALUE( &logCfgs, actor->key );
		node = list_find_node_by_key( &logCfgs, actor->key );
		value = (node)?node->entry.value:"";
		type = (node)?node->entry.type:"";
		for(level = _logLevels; level->key; level++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",level->key, (!strcasecmp( value, level->key) )?"selected":"", gettext(level->value) );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD><TD align=\"center\"><input name=\"filename\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\r\n", type );

	}

	length += CGI_SPRINTF(buf,length, "<TR><TD><br><p></TD></TR><TR><TD align=\"center\" colspan=3>%s %s</TD></TR>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_help_button(CGI_HELP_MSG_SERVICES_SYSLOG));

	return strdup(buf);
}

int	servicesMisc_Syslogd(SERVICE_INFO *info)
{
	char		*action;

	__read_syslog_confg();
	
	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Syslogd Configuration"), __servicesMisc_syslogd_status(),__servicesMisc_syslogd_buttons() );
		return 0;
	}
	else
	{
		__write_syslog_confg( info);
		return 0;
	}
	
	return 0;
}

