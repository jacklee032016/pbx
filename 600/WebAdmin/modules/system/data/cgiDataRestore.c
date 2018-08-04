/*
* $Id: cgiDataRestore.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include <fcntl.h>
#include "cgi_data.h"

int __restore_get_package(char *serverip)
{
	char 	cmd[256] = "\0";
	char 	curr_path[256] = "\0";;
	char		path_name[256] = "\0";
	int 		fd;
	char		msg[1024];
	int		len = 0;
	struct stat data_stat;


	snprintf(cmd, sizeof(cmd), "%s -r %s -g %s", TFTP_EXE, dataFilename, serverip);
	chdir("/tmp");
	len += CGI_SPRINTF(msg,len, "'%s' %s %s.....",dataFilename, gettext("is getting from"), serverip);
	CGI_INSTANCE_INFO(msg, "#0000ff");

	if (system(cmd) == -1)
	{/* can try next time */
		len += CGI_SPRINTF(msg,len, "'%s' %s!",serverip, gettext("Access File Failed"));
		CGI_INSTANCE_INFO(msg, "#FF0000");
		return -2;
	}
	
 	//NULL方式调用不推荐
	//curr_path=getcwd(NULL,128);
	//Check file size, if file not empty, it's legal.
	len += CGI_SPRINTF(msg,len, "<br>%s!", gettext("File Transfer ended") );
	CGI_INSTANCE_INFO(msg, "#0000ff");
	getcwd(curr_path, sizeof(curr_path));
	snprintf(path_name, sizeof(path_name), "%s/%s", curr_path, dataFilename);
	
	len += CGI_SPRINTF(msg,len, "'%s' %s.....", path_name, gettext("local file is checking"));
	CGI_INSTANCE_INFO(msg, "#0000ff");
	if((fd = open(path_name,O_RDONLY)) < 0)
	{/* can not try next time */
		len += CGI_SPRINTF(msg,len, "'%s' %s!",path_name, gettext("Access File Failed"));
		CGI_INSTANCE_INFO(msg, "#FF0000");
		return -1;
	}
	fstat(fd,&data_stat);
	close(fd);
	
	if(data_stat.st_size <= 0)
	{/* can not try next time */
		len += CGI_SPRINTF(msg,len, "'%s' %s!%s!",path_name, gettext("is invalidate"), gettext("So removed"));
		CGI_INSTANCE_INFO(msg, "#FF0000");
		//Delete empty files.
		snprintf(cmd, sizeof(cmd), "/bin/rm -f %s", path_name);
		system(cmd);
		return -1;
	}
		
	return 0;
}


static int __restore_release_tar(char *filename)
{
	char cmd[256]="\0";
	char		msg[1024];
	int		len = 0;

	chdir("/tmp");
	if( access(filename,F_OK) != 0)
	{
		fprintf(stderr, "Error: %s not exist!\n", filename);
		return -1;
	}
	
	len += CGI_SPRINTF(msg,len, "'%s' %s.....", filename, gettext("is unpackaging"));
	CGI_INSTANCE_INFO(msg, "#0000ff");
	if( strstr(filename,".tar.gz"))
		sprintf(cmd,"/bin/tar zxf %s -C /",filename);
	else if (strstr(filename,".tar") )
		sprintf(cmd,"/bin/tar xf %s -C /",filename);
	else
	{
		len += CGI_SPRINTF(msg,len, "'%s' %s!",filename, gettext("is not *.tar or *.tar.gz") );
		CGI_INSTANCE_INFO(msg, "#FF0000");
		return -1;
	}

	system("/bin/mkdir cnf_bak;/bin/cp /etc/sys/* cnf_bak/");
	
	if (system(cmd) ==-1)
	{
		fprintf(stderr, "Release %s error.\n",  filename);
		system("/bin/mv cnf_bak/* /etc/sys;/bin/rm -rf cnf_bak");
		return -1;
	}

	//Delete .tar
	snprintf(cmd, sizeof(cmd), "/bin/rm -f %s;/bin/rm -rf cnf_bak",filename);
	system(cmd);

	return 0;
}

static int __restoredata_from_server(char *serverip)
{
	char		cmd[256] = "\0";
	int 		ret = -1;
	int		len = 0;
	char		msg[1024];
	int 		i;

	char		*path = cgidata_init_filename(CGIDATA_OP_RESTORE_SERVER);
	if(!path)
		return -1;
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo '%s' >/etc/service_ip", serverip);
	system(cmd);
	

	while(i < CGIDATA_TRY_MAX)
	{
		len += CGI_SPRINTF(msg,len, "'%s' %d(%d) %s.....",gettext("Trying"), i+1, CGIDATA_TRY_MAX, gettext("Times"));
		CGI_INSTANCE_INFO(msg, "#0000ff");
		ret = __restore_get_package(serverip) ;
		if( ret== 0 )
		{
			ret=__restore_release_tar(dataFilename);
			break;
		}
		else if(ret== -1)
			break;
		
		sleep(2);
		i++;
	}

	if( ret )
	{
		return  -1;
	}

#if 1
	//Delete temp file. (.tar)
	snprintf(cmd, 256,"/bin/rm -f /tmp/%s", dataFilename);
	system(cmd);
#endif

	if(ret == 0)
	{
		len += CGI_SPRINTF(msg,len, "%s %s!",gettext("Data has been restored from server"), serverip);
		CGI_INSTANCE_INFO(msg, "#FF0000");
	}
	
	return 0;
}

int cgiData_RestoreServer(DATA_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Data Restore from Server"), "", cgidata_serverip_button(info, CGIDATA_OP_RESTORE_SERVER) );
		return 0;
	}
 	
	if(!strcasecmp(action, CGI_ACTION_ADD )  )
	{
		char *serverIp = GET_VALUE(&info->cgiVariables, CGIDATA_KEYWORD_SERVER_IP );

		cgidata_write_config_file(info, CGIDATA_KEYWORD_SERVER_IP, serverIp);

		__restoredata_from_server( serverIp);
	}
	return 0;	
}

