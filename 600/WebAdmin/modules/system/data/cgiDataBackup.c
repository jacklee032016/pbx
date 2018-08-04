/*
* $Id: cgiDataBackup.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_data.h"

int __backup_package(char *opName)
{
	char		msg[1024];
	int		len = 0;
	char 	cmd[256] = "\0";
	char 	ret = -1;
	char		*path = cgidata_init_filename(opName);
	if(!path)
		return -1;
		
	chdir(path);
	sprintf(cmd,"/bin/tar cf %s -C %s", dataFilename, CGIDATA_BACKUP_DIRS);

	len += CGI_SPRINTF(msg,len, "'%s' %s.....",dataFilename, gettext("is packaging"));
	CGI_INSTANCE_INFO(msg, "#0000ff");
	
	ret = system(cmd);
	
	if(0 != ret)
	{
		len += CGI_SPRINTF(msg,len, "%s!", gettext("Packaging Data Failed"));
		CGI_INSTANCE_INFO(msg, "#FF0000");
//		error_flag=ERROR_TAR;
		return -2;
	}
	
	fflush(stdout);

	return 0;
}

/*
  *Pack files(/etc/ and /usr/etc/) with tar, send to TFTP
  *server. 
  *Filename like assist_backup_data_deviceID.tar.
  *RETURN:N/A
  */
static int __backupdata_to_server(char *serverip)
{
	char		cmd[256] = "\0";
	int 		ret = -1;
	int		len = 0;
	char		msg[1024];

	if(__backup_package(CGIDATA_OP_BACK_SERVER))
	{
		return -1;
	}
	ret = -1;

	len += CGI_SPRINTF(msg,len, "'%s' %s %s.....", dataFilename, gettext("Backup package is sending to server"), serverip );
	CGI_INSTANCE_INFO(msg, "#FF0000");

	sprintf(cmd,"/usr/bin/tftp -l %s -p %s > /dev/null ", dataFilename, serverip );
	ret=system(cmd);
	if( 0 != ret)
	{
		len += CGI_SPRINTF(msg,len, "%s!", gettext("TFTP transfer failed") );
		CGI_INSTANCE_INFO(msg, "#FF0000");
	}

#if 1
	//Delete temp file. (.tar)
	snprintf(cmd, 256,"/bin/rm -f /tmp/%s", dataFilename);
	system(cmd);
#endif

	if(ret == 0)
	{
		len += CGI_SPRINTF(msg,len, "%s %s!",gettext("Data has been back to server"), serverip);
		CGI_INSTANCE_INFO(msg, "#FF0000");
	}
	
	return 0;
}

int cgiData_BackupServer(DATA_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Data Backup to Server"), "", cgidata_serverip_button(info, CGIDATA_OP_BACK_SERVER) );
		return 0;
	}
 	
	if(!strcasecmp(action, CGI_ACTION_ADD )  )
	{
		char *serverIp = GET_VALUE(&info->cgiVariables, CGIDATA_KEYWORD_SERVER_IP );

		cgidata_write_config_file(info, CGIDATA_KEYWORD_SERVER_IP, serverIp);
		__backupdata_to_server( serverIp);
	}
	return 0;	
}

static char *__backup_local_button(char *_fileName)
{
	char 	buf[8192*5];
	int 		length = 0;

	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td colspan=2><strong>%s</strong><p>%s</TD></TR>\n", 
		gettext("Download Backup "),gettext("Please click following link") );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s:</strong></TD>", 
		gettext("ASSIST AS600 Configuration Backup") );

#if 1	
	length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><img height=10 src=\"/sp.gif\" width=10><br><a href='/%s/%s'  target='mainframe'> <IMG SRC='/pd.gif' BORDER=0>%s</a><br> </TD></TR></FORM>\r\n",
		CGIDATA_BACKUP_DATA_DIR, _fileName, _fileName );
#else
	length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><img height=10 src=\"/sp.gif\" width=10><br><a href='/%s'  target='mainframe'> <IMG SRC='/pd.gif' BORDER=0>%s</a><br> </TD></TR></FORM>\r\n",
		 _fileName, _fileName );
#endif
	return strdup(buf);
}

int cgiData_BackupLocal(DATA_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		char target[128];
		if(__backup_package(CGIDATA_OP_BACK_LOCAL))
		{
			return -1;
		}

		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_DATA, CGI_KEYWORD_OPERATOR, CGIDATA_OP_BACK_LOCAL,
			CGI_ACTION, dataFilename );
		cgi_refresh_page(1, target, gettext("Backup to Local"), gettext("Please waiting") );
		
		return 0;
	}

	cgi_info_page(gettext("Backup to Local"), "", __backup_local_button(action));
	return 0;	
}


