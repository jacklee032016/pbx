/*
* $Id: cgiDataUtils.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <time.h>

#include "aslib.h"
#include "cgi_lib.h"
#include "cgi_data.h"

/*
    *Get backup file name
    *If backup to local,then:	  name = assist_back_yyyy_mm_dd.tar
    *If backup to TFTP server,then:  name = assist_back_deviceID.tar 
 *RETURNS:  	error:-1;	success:0
  */
int cgidata_get_package_filename(char *filename, const char *command, char *devID)
{
	time_t  	t;	
	struct tm *now_time;

	if(!strcmp(command,CGIDATA_OP_BACK_LOCAL) )
	{
		//Get system current time
		time(&t);	
		now_time = localtime(&t);
		sprintf(filename,"%s_%02d-%02d-%02d.tar",
			  CGIDATA_DATA_BACKUP_PREFIX, now_time->tm_year%100,
			  now_time->tm_mon+1, now_time->tm_mday);
	}
	else if( !strcmp(command, CGIDATA_OP_BACK_SERVER) ||
		!strcmp(command, CGIDATA_OP_RESTORE_SERVER) )
	{
		sprintf(filename,"%s_%s.tar", CGIDATA_DATA_BACKUP_PREFIX, devID);
	}
	else if(!strcmp(command, CGIDATA_OP_AUTO_UPDATE ) )
	{
		sprintf(filename,"%s_%s.tar", CGIDATA_SOFT_UPLOAD_PREFIX, devID);
	}
	
	return 0;
}

/* Server IP Input box for both restore and backup*/
char *cgidata_serverip_button(DATA_INFO *info, char *opName)
{
#define CGIDATA_IS_BACKUP(opname) \
	(!strcasecmp(CGIDATA_OP_BACK_SERVER,opname) )
	
	char 	buf[8192*5];
	int 		length = 0;
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td><strong>%s %s<p></strong></TD></TR>\n", gettext("Input IP Address of Server which"), (CGIDATA_IS_BACKUP(opName))? gettext("save your backup"):gettext("get your backup") );

	length += CGI_SPRINTF(buf, length, "<form name=\"backtoServerForm\" method=\"post\" action=\"%s\">\n", WEB_URL_DATA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, opName);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong>", 
		gettext("Server IP Address") );
	length += sprintf(buf+length, "<input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		CGIDATA_KEYWORD_SERVER_IP, CGIDATA_KEYWORD_SERVER_IP , GET_VALUE(&info->globalCfgs, CGIDATA_KEYWORD_SERVER_IP) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s%s\r\n</TD></TR></FORM>\r\n",
		cgi_submit_button( gettext(CGI_STR_NEXT) ), cgi_help_button(CGI_HELP_MSG_BACKUP_SERVER) );

	return strdup(buf);
}

/* return path string or null */
char *cgidata_init_filename(char *opName)
{
	char 	path[256] = "\0";
	char		msg[1024];

	if(!strcasecmp(CGIDATA_OP_BACK_LOCAL, opName) )
	{
		sprintf(path,"%s/%s", ROOT_PATH_ADMIN, CGIDATA_BACKUP_DATA_DIR );
//		sprintf(path,"%s/", ROOT_PATH_ADMIN );
		cgidata_get_package_filename(dataFilename, opName, NULL);
	}
	else
	{
		char		devID[32];
		
		sprintf(path,"%s", "/tmp" );
		sprintf(msg, "%s.....", gettext("Read Device ID from Flash Device"));
		CGI_INSTANCE_INFO(msg, "#0000ff");
		
		if(as_read_serial_no( devID, sizeof(devID) ) )
		{
			sprintf(msg,"%s, %s!", gettext("Device ID read failed"), gettext("Please contact the support of Assist Corp.") );
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);

			return NULL;
		}
		
		
		sprintf(msg, "%s '%s'", gettext("Device ID is"), devID );
		CGI_INSTANCE_INFO(msg, CGI_COLOR_BLUE );

		cgidata_get_package_filename(dataFilename, opName, devID);
	}

	return strdup(path);
}

int cgidata_write_config_file(DATA_INFO *info, char *key, char *value)
{
	list_replace_value_by_key(&info->globalCfgs, key, value);
	return writeconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &info->globalCfgs);
}

