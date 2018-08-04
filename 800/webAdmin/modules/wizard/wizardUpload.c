/*
* $Id: wizardUpload.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "wizard.h"

int  wizard_cgi_upload(WIZARD_INFO *info )
{
	char 	alert[2048];
	char 	leftButton[2048];
	char 	rightButton[2048];
	char		*uploadFileName = GET_VALUE(&info->cgiVariables, WIZARD_KEY_UPLOAD_FILE) ;

	int 		length = 0;
#if 1	
	as_disk_t  *disk = wizard_find_disk( info);
	if(!disk )
		return 0;
	
	as_partition_t *part = disk->parts;
	if(!part)
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("No Partition is not found now" ));
		return 0;
	}
#endif

	length = 0;
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s of %s %s %s\n</td></tr>", part->name, disk->descrition, gettext("is mount on"), part->mountPoint );

	length += CGI_SPRINTF(alert, length, "<tr><td>'<strong>%s</strong>' %s!\n", uploadFileName, gettext("has been upload onboard") );
	length += CGI_SPRINTF(alert, length, "<tr><td>%s.\n", gettext("Now, it is time for extract File System image file transfered just now"));
	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	length += CGI_SPRINTF(alert, length, "</table>\n" );

	length += CGI_SPRINTF(alert, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", WIZARD_KEY_UPLOAD_FILE, uploadFileName);
	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(WIZARD_STR_UPDATE) ) );

	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext(CGI_STR_UPLOAD), WEB_URL_WIZARD"?"CGI_KEYWORD_OPERATOR"="WIZARD_OP_FORMAT) );
	
	return wizard_page(gettext(CGI_STR_UPLOAD), "",  alert, leftButton, rightButton, info);
}


static int __wizard_extract_file(WIZARD_INFO *info)
{
	char		cmdline[256];
	char		statusMsg[1024];
	char		rootpath[128];
	char		*uploadFileName = GET_VALUE(&info->cgiVariables, WIZARD_KEY_UPLOAD_FILE) ;

	sprintf(rootpath, "%s", WIZARD_FS_MOUNT_POINT);

	if(strstr(uploadFileName,".tar.gz"))
	{
#if 0//DEBUG_UPLOAD
		sprintf(cmdline,"tar zxvf %s/%s -C %s > /tmp/upload.txt ",UPLOADDIR,uploadFileName, rootpath);
#else
		sprintf(cmdline,"tar zvxf %s/%s -C %s",UPLOADDIR, uploadFileName, rootpath);
#endif
	}
	else
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("No FileSystem File uploaded is found" ));
		return -1;
	}
	
	sprintf(statusMsg, "'%s' %s.....", uploadFileName, gettext("is extracting"));
	CGI_INSTANCE_INFO(statusMsg, "#0000ff");
	
	system(cmdline);
	
	sprintf(cmdline,"rm -f %s/%s",UPLOADDIR, uploadFileName);

	sprintf(statusMsg, "'%s' %s.....", uploadFileName, gettext("is removing"));
	CGI_INSTANCE_INFO(statusMsg, "#0000ff");
	system(cmdline);

	return 0;
}

int  wizard_cgi_update(WIZARD_INFO *info )
{
	char 	alert[2048];
	char 	rightButton[2048];

	int 		length = 0;
	printf("%s", wizard_page_begin(WIZARD_STR_UPDATE, info) );
//	printf("%s", wizard_page_status_begin( info));
	fflush(stdout);

	__wizard_extract_file(info);

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s.\n", gettext("Now, it is time for reboot with new File System just update") );
	length += CGI_SPRINTF(alert, length, "<tr><td><strong>%s</strong> : %s!\n",gettext(CGI_STR_NOTE), gettext("The device just used must be use as root device"));

	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	length += CGI_SPRINTF(alert, length, "</table>\n" );
	printf("%s", wizard_page_alert(alert, info) );
	
	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(CGI_STR_REBOOT) ) );

	printf("%s", wizard_page_end(gettext(WIZARD_STR_TITLE), rightButton,  info) );

	return 0;
}

int wizard_cgi_reboot(WIZARD_INFO *info)
{
	char alert[2048];
	int 	length = 0;
	char target[2048];

	printf("%s", wizard_page_begin(gettext(CGI_STR_REBOOT), info) );

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s!\n", gettext("Reboot will cost 1 minuts or more") );
	length += CGI_SPRINTF(alert, length, "<tr><td>%s\n", gettext("it is determined by the services you selected") );
	length += CGI_SPRINTF(alert, length, "<tr><td><strong>%s</strong> : %s, %s!\n",CGI_STR_NOTE, 
		gettext("If you can not return the WebAdmin of Newly installed System"), gettext("please repeat the install process again") );

	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	length += CGI_SPRINTF(alert, length, "</table>\n" );
	printf("%s", wizard_page_alert(alert, info) );
	
	sprintf(target,"%s", "/" );
	printf("%s", cgi_refresh_script(target, 75) );
	
	printf("%s", wizard_page_end(gettext("Good Luck for you! Thanks!"), "",  info) );
	fflush(stdout);
	
	system("reboot");

	return 0;
}


int	wizard_upload_callback(char *uploadFileName, void *data)
{
	char 	status[2048*2];
	char 	alert[2048*2];
	char 	leftButton[2048];
	char 	rightButton[2048];
	char		target[256];
	int 		length = 0;
	WIZARD_INFO *info = (WIZARD_INFO *)data;
	if(! info )
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("Not initial data is found in WIZARD") );
		return -1;
	}

	if(!strstr(uploadFileName, WIZARD_FS_FILESYSTEM_IMAGE_PREFIX) )
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("This is not a File System Image File") );
		return -2;
	}
	else if(!strstr(uploadFileName, WIZARD_FS_FILESYSTEM_IMAGE_SURFIX) )
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("File System Image is not packaged correctly!" ));
		return -2;
	}

	length += CGI_SPRINTF(status,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(status, length, "<tr><td align=\"center\" >'<strong>%s</strong>' %s.....</td></tr>\n", 
		uploadFileName, gettext("is upload now") );
	length += CGI_SPRINTF(status, length, "</table>\n" );

//	list_append_raw(&info->cgiVariables, CGI_KEYWORD_OPERATOR, WIZARD_OP_UPLOAD,NULL, NULL);

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s, %s.\n", gettext("The System Image is about 16M bytes"),gettext("so it will cast some time to complete this transfer") );
	length += CGI_SPRINTF(alert, length, "<tr><td>%s!\n", gettext("How long it will cast will be determined by the network condition") );
	length += CGI_SPRINTF(alert, length, "<tr><td>%s!\n", gettext("Please Waiting more patiently"));

	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	
	sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_WIZARD,CGI_KEYWORD_OPERATOR, WIZARD_OP_UPDATE, 
		WIZARD_KEY_UPLOAD_FILE, uploadFileName);
	
	length += CGI_SPRINTF(alert, length, "%s</table>\n", cgi_refresh_script(target, WIZARD_FS_TRANSFER_DELAY) );
	
	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext(CGI_STR_UPLOAD), WEB_URL_WIZARD"?"CGI_KEYWORD_OPERATOR"="WIZARD_OP_FORMAT) );

	sprintf(rightButton, "%s.....", WIZARD_STR_UPDATE );
	wizard_page(gettext(CGI_STR_UPLOAD), status, alert, leftButton, rightButton, info);

	fflush(stdout);
	return 0;
}

