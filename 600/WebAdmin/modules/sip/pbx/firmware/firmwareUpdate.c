/*
* $Id: firmwareUpdate.c,v 1.1 2007/06/01 20:59:02 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>

#include "cgi_lib.h"

#include "cgi_services.h"
#include "cgi_syscfg.h"

typedef	struct
{
	llist			cgiVariables;
	
}UPDATE_INFO;

static UPDATE_INFO	_Info;

static int __init(UPDATE_INFO *info)
{
	int		length = 0;
	char		statusMsg[1024];
	
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);
	length += CGI_SPRINTF(statusMsg, length, "<br>%s!", gettext("Update PBX Firmware involved following 3 steps"));
	length += CGI_SPRINTF(statusMsg, length, "<p>* %s.", gettext("Stop UART Monitor"));
	length += CGI_SPRINTF(statusMsg, length, "<p>* %s.", gettext("Burn Firmware"));
	length += CGI_SPRINTF(statusMsg, length, "<p>* %s.", gettext("Restart UART Monitor"));

	cgi_refresh_page(10, WEB_URL_UPLOAD_FIRMWARE,gettext("Update PBX Firmware"), statusMsg );
	fflush(stdout);
	return 0;
}


static int __stop_uartd(int count)
{
	int pid;
	char		cmd[256];
	char		statusMsg[1024];

	pid = cgi_get_program_pid_with_param(SYS_PROC_UARTD, NULL);
	if(pid > 0)
	{
		sprintf(cmd, "kill -9 %d > /dev/null ", pid);
//		sprintf(statusMsg, "'%s' %s!%d %s.....('%s')",gettext(CGI_STR_CGIPBX), gettext(CGI_STR_RUNNING),count+1, gettext("Stop it now"), cmd);
		sprintf(statusMsg, "'%s' %s!%d %s.....",gettext(CGI_STR_CGIPBX), gettext(CGI_STR_RUNNING),count+1, gettext("Stop it now"));
		CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);
		
		system(cmd);

		sleep(5);
		return 1;
	}

	sprintf(statusMsg, "'%s' %s!",gettext(CGI_STR_CGIPBX), gettext(CGI_STR_STOP) );
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	return 0;
}

static void __update_firmware(void)
{
	char		cmd[256];
	char		statusMsg[1024];
	int		length = 0;
	char		filename[256];

	FILE *fp;

	sprintf(filename, "%s/PBX.BIN", PBX_FIRMWARE_HOME);
	if((fp=fopen(filename,"r"))==NULL)
	{
		length = CGI_SPRINTF(statusMsg, length, "<p>%s : %s!",gettext(CGI_STR_ERROR), gettext("PBX firmware file is not found"));
		CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_RED);
		return;
	}
	fclose(fp);

	sprintf(cmd, "/usr/bin/isp %s > /dev/null", filename);
	length += CGI_SPRINTF(statusMsg, length, "<br>%s!", gettext("Reburn PBX firmware, please waiting patiently"));
	length += CGI_SPRINTF(statusMsg, length, "<p>%s!", gettext("After ringing of our PBX phone, update is finished"));
	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", gettext("PBX firmware is updating"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);
	
	fork_cmd(cmd );
	sleep(50);

	return ;
}



static void __start_uartd(void)
{
	char		cmd[256];
	char		statusMsg[1024];

	sprintf(cmd, "/usr/bin/%s > /dev/null ", SYS_PROC_UARTD );
	sprintf(statusMsg, "<br>'%s' %s.....",gettext(CGI_STR_CGIPBX), gettext("is restarting"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);
	
	fork_cmd(cmd );
	sleep(2);

//	sprintf(statusMsg, "<br>'%s!", gettext("PBX Firmware Update Finished"));
//	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	sprintf(statusMsg, "'%s!", gettext("PBX Firmware Update Finished"));
	cgi_error_page(NULL, statusMsg, gettext("OK"));
	return ;
}

int main()
{
	UPDATE_INFO 	*info = &_Info;
	int count = 0;

	if(__init( info) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	while( count< 3)
	{
		if(__stop_uartd(count )==0)
			count = 4;
		else
			count++;
	}

	if(count==3)
	{
		return cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Can not stop UARTD. Please try later!"));
	}
	
	__update_firmware();
	
	__start_uartd();


//	cgi_refresh_script(WEB_URL_UPLOAD_FIRMWARE, 1);
//	cgi_info_page(gettext("WWW Configuration"), __www_status_list(info), __www_add_buttons()  );
	return 0;	
}

