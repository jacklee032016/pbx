/*
* $Id: firmwareUpload.c,v 1.1 2007/06/01 20:59:02 lizhijie Exp $
*/

#include "cgi_upload_file.h"

static llist		uploadCgiVariables;

int main(int argc, char *argv[])
{
	int 		result;

	list_create(&uploadCgiVariables);
	cgi_html_header();
	result = upload_read_cgi_input(&uploadCgiVariables, WEB_TYPE_PBX_FIRMWARE);
	if( result <= 0 ) /* success */
	{
		return 0;
	}

	cgi_info_page(gettext("Upload PBX Firmware"),  upload_status_list(WEB_TYPE_PBX_FIRMWARE), upload_add_buttons(WEB_TYPE_PBX_FIRMWARE) );

	return 0;
}

