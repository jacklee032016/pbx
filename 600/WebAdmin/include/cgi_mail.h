/*
* $Id: cgi_mail.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/
#ifndef __CGI_MAIL_H__
#define __CGI_MAIL_H__

#define	CGI_MAIL_OP_SMTP					"Smtp"
#define	CGI_MAIL_OP_POPIMAP				"PopImap"
#define	CGI_MAIL_OP_ALIAS					"Alias"

#define	MAIL_OP_DEL_ALIAS_IN_GROUP		"delAliasInGroup"
#define	MAIL_OP_ADD_ALIAS_IN_GROUP		"addAliasInGroup"

#define	MAIL_OP_ADD_ALIAS					"addAlias"
#define	MAIL_OP_DEL_ALIAS					"delAlias"
#define	MAIL_OP_ADD_GROUP				"addGroup"
#define	MAIL_OP_DEL_GROUP				"delGroup"

#define	MAIL_OP_ACTIVATE					"activate"

#define	MAIL_OP_TYPE						"type"
#define	MAIL_OP_TYPE_ALIAS				"aliases"
#define	MAIL_OP_TYPE_GROUP				"group"

#define	MAIL_OP_SMTP_CONFIG				"smtpConfig"

#define	MAIL_SMTP_KEYWORD_PORTNUM		"portNum"
#define	MAIL_SMTP_KEYWORD_FQDN			"myhostname"	/* keyword in main.cf */

#define	MAIL_ALIAS_KEYWORD_ALIAS		"alias"
#define	MAIL_ALIAS_KEYWORD_GROUP		"group"
#define	MAIL_ALIAS_KEYWORD_SYS_USER		"sysUser"

typedef enum
{
	MAIL_DB_TYPE_ALIASES = 0,
	MAIL_DB_TYPE_GROUP
}mail_db_type_t;

typedef enum
{
	MAIL_OP_ADD = 0,
	MAIL_OP_DEL	
}mail_op_t;


#include "cgi_services.h"
#include "cgi_cfg.h"

typedef	struct
{
	llist 				cgiVariables;
	
	llist				globalCfgs;

	network_info		*netInfo;
	
}MAIL_INFO;

int cgi_mail_reactivate_account();

int mail_Smtp(MAIL_INFO *info);
int mail_PopImap(MAIL_INFO *info);
int mail_Alias(MAIL_INFO *info);

#endif

