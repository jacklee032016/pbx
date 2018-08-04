/*
* $Author: lizhijie $
* $Revision: 1.2 $
*/

#include "cgi_sigdef.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

int cgi_directory_filt_by_number(const struct dirent *namelist)
{
	char  *c;
	int isnum=1;

	c = (char *)namelist->d_name;
	while( *c != '\0' )
	{
		if((*c)<48 || (*c)>57)/*'0'=48,'9'=57*/
		{
			isnum=0;
			break;
		}
		c++;
	}
	return isnum;
}

#if 0
int  cgi_get_pid_by_name(char * appName)
{
	struct dirent **namelist;
	int n;
	char file[256], cmdline[256];
	FILE *fp ;

	n = scandir("/proc", &namelist, cgi_directory_filt_by_number, alphasort);
	while(n--)
	{
		sprintf(file,"/proc/%s/cmdline",namelist[n]->d_name);
		if((fp=fopen(file,"r"))==NULL)
		{
			perror("open cmdline file");
			continue;
		}

		/*get the server name*/
		memset(cmdline,0,sizeof(char)*256);
		fgets(cmdline, 255, fp);
		if(strstr(cmdline,appName)!=NULL)
		{
			break;
		}
	}

	return n<0?0:atoi(namelist[n]->d_name);
}
#endif

int cgi_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *))
{
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;
	act.sa_sigaction=sig_handler;

	return sigaction(sigkey, &act, NULL);
}

void cgi_sig_send(pid_t pid, int sigid, int opcode)
{
	union sigval sval;

	sval.sival_int=opcode;
	sigqueue(pid, sigid, sval);
}

int cgi_get_pid_by_name(char *prog_name)
{
	struct dirent **namelist;
	int n;
	char file[256];
	FILE *fp;
	int pid = -1;
	
	struct info_type
	{
		char waste[16];
		char value[64];
	}serverinfo;
	
	n = scandir("/proc", &namelist,  cgi_directory_filt_by_number, alphasort);
	while(n--)
	{
		sprintf(file,"/proc/%s/status", namelist[n]->d_name);
		if((fp=fopen(file,"r"))==NULL)
		{
			cgidebug("open status file '%s' fail:%s\n", file, strerror(errno) );
			continue;
		}
	
		memset(&serverinfo, 0, sizeof(struct info_type));
			
		/*get the server name*/
		fscanf(fp, "%s %s", serverinfo.waste, serverinfo.value);
		if(!strcmp( serverinfo.value, prog_name) )
			pid = atoi( namelist[n]->d_name );
		if(!strcmp(serverinfo.waste,"State:"))
			if(strchr(serverinfo.value,'Z'))
				pid=-1;
		fclose(fp);
		free(namelist[n]);
	}
		
	free(namelist);
	return pid;
}

void cgi_seng_signal_to_process_by_name(int sig, char *prog_name)
{
	pid_t pid;

	pid = (pid_t)cgi_get_pid_by_name( prog_name);
	if( pid <0 )
		return;

	kill(pid, sig);
}

void fork_cmd(const char *cmd) 
{
//	printf("Commad : %s\n" , cmd);
	
	if(!fork()) 
	{
		system(cmd);
		exit(0);
	}
	sleep(1); /* one second */
	return;
}

