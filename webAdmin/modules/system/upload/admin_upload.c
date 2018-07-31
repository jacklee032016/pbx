#include "cgi_upload_file.h"

static llist		uploadCgiVariables;

int main(int argc, char *argv[])
{
	int 		result;
	char		*cmd;
	char		*title;

	cgi_html_header();
	result = upload_read_cgi_input(&uploadCgiVariables, WEB_TYPE_ADMIN);
	if(result <= 0) /* success */
	{
		return 0;
//		cgi_info_page(gettext("Upload Admin Package"),  upload_status_list(WEB_TYPE_ADMIN), upload_add_buttons(WEB_TYPE_ADMIN) );
	}


	cmd = GET_CGI_OP(&uploadCgiVariables );
	if(!strcasecmp(cmd, CGI_UPLOAD_RESTORE ) )
		title = gettext("Upload backup file saved in your computer");
	else
		title = gettext("Upload Admin Package");
	cgi_info_page(title,  upload_status_list(WEB_TYPE_ADMIN), upload_add_buttons(WEB_TYPE_ADMIN) );

	return 0;
}

