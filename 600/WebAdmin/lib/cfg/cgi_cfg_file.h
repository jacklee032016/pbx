/*
* $Id: cgi_cfg_file.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#ifndef  __CGI_CFG_FILE_H__
#define	__CGI_CFG_FILE_H__

extern	int cgi_write_config_item_with_collon(char *key, char *value,  FILE*out);

/* return number of items wrote to the file */
extern	int cgi_write_config(FILE *fp, llist *rawList, llist *modifiedList, int(*write_special_fields)(char *, char *, FILE*));



#endif

