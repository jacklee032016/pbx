/*
* $Id: www_upload.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#include "cgi_upload_file.h"

static llist		uploadCgiVariables;

int main(int argc, char *argv[])
{
	int 		result;

	list_create(&uploadCgiVariables);
	cgi_html_header();
	result = upload_read_cgi_input(&uploadCgiVariables, WEB_TYPE_WWW);
	if( result <= 0 ) /* success */
	{
		return 0;
	}

	cgi_info_page(gettext("Upload WWW Pages"),  upload_status_list(WEB_TYPE_WWW), upload_add_buttons(WEB_TYPE_WWW) );

	return 0;
}

