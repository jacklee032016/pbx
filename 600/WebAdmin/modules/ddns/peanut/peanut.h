/*
* $Id: peanut.h,v 1.4 2006/12/04 10:48:50 lizhijie Exp $
*/
#ifndef  __PEANUT_H__
#define	__PEANUT_H__

#include "aslib.h"
#include "cgi_lib.h"

#define	PEANUT_CONFIG_FILE				"/etc/web/peanut.conf"

#define	PEANUT_KEYWORD_LOG_LEVEL		"LOG_LEVEL"
#define	PEANUT_KEYWORD_LOG_TYPE			"LOG_TYPE"
#define	PEANUT_KEYWORD_LOG_FILE			"LOG_FILE"

#define	PEANUT_KEYWORD_USERNAME		"USER_NAME"
#define	PEANUT_KEYWORD_PASSWD			"PASSWORD"

#define	PEANUT_KEYWORD_FQDN				"FQDN"

#define	PEANUT_KEYWORD_PORT				"SERVER_PORT"


#define 	PEANUT_LOG_TYPE_SYSLOG			"SYSLOG"
#define 	PEANUT_LOG_TYPE_CONSOLE			"CONSOLE"
#define 	PEANUT_LOG_TYPE_FILE				"FILE"

typedef struct 
{
	llist 		configs;

	char		*username;
	char		*password;

	char		*fqdn;

	int		port;
}PEANUT_INFO;

typedef unsigned long int UINT4;

void Transform (UINT4 *buf,UINT4 *in);
void hmac_md5 (unsigned char *text,int text_len,unsigned char * key,int key_len,unsigned char * digest);

char * DecodeBase64(char * Source);
char * EncodeBase64(char * Source);
unsigned char FindInTable(unsigned char);


#endif

