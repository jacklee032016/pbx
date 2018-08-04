#ifndef __CGI_UPLOAD_FILE_H__
#define __CGI_UPLOAD_FILE_H__

#include <string.h>
#include "cgi_lib.h"
#include "cgi_llist.h"

#define UPLOADPATH		"/tmp"


#define		MAX_UPLOAD_FILE_SIZE_10M						10		/* 10M bytes */	

#define		CGI_UPLOAD_RESTORE							"restore"
#define		CGI_UPLOAD_UPDATE							"update"

#define		CGI_UPLOAD_SUCCESS							0
#define		CGI_UPLOAD_ERROR_NO_CONTENT				-1
#define		CGI_UPLOAD_ERROR_FORMAT_NOT_SUPPORT		-2
#define		CGI_UPLOAD_ERROR_TOO_BIG					-3
#define		CGI_UPLOAD_ERROR_FILE_NOT_EXIST				-4


int up_read_cgi_input(llist* entries);
int upload_file_deal_http_request();

int upload_parse_form_endcode(llist* entries, web_type_t type);

int upload_read_cgi_input(llist* entries, web_type_t type);
char *upload_add_buttons(web_type_t type);
char *upload_status_list( web_type_t type);

#endif

