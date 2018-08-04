#ifndef __CGI_CFG_H__
#define	__CGI_CFG_H__
/* 
* $Id: cgi_cfg.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

int cgi_write_config_item_with_collon(char *key, char *value,  FILE*out);
int cgi_write_config(FILE *fp, llist *rawList, llist *modifiedList, int(*write_special_fields)(char *, char *, FILE*));

#endif

