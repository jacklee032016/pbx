/*$Id: wtd.c,v 1.2 2007/01/29 15:14:07 lizhijie Exp $*/

#include "wtd.h"

static void wtd_visit(entrytype *item, void *object)
{
	int pid;
	pid_t cpid;
	char *argv[64],**pa,**p;
	char tmp[LINE4RAW],*q=tmp;
	int ret;

	if(item->name[0]=='#')
	{
		cgidebug("return %s\n",item->name);
		return;
	}
	
	cgidebug("Enter %s\n",item->name);
	list_extract_cmdline(item);
	pid=cgi_get_pid_by_name(item->name);
	cgidebug("name=%s,value=%s,pid=%d\n",item->name,item->value,pid);

	if(pid<0)
	{
		if((cpid=fork())<0)
		{
			printf("[wtd]%s() fork error,cpid=%d",__FUNCTION__,cpid);
		}
		else if(cpid==0)
		{
			strcpy(tmp,item->value);
			p=&q;
			pa=argv;
			while((*pa++=strsep(p," \t\n")));
#if DEBUG
			pa=argv;
			while(*pa)
			{
				cgidebug("argv=%s,len=%d,*pa=%p\t",*pa,strlen(*pa),*pa);
				pa++;
			}
			if(!*pa)
				cgidebug("last argv NULL\n");
#endif
			printf("[wtd]start %s\n",argv[0]);
			ret=execv(argv[0],argv);
			if(ret<0){char e[LINE4RAW];sprintf(e,"[wtd]can't start %s",argv[0]);perror(e);exit(-1);}
		}
	}
	else
	{
		printf("[wtd]proc %s is running ...\n",item->name);
	}
}

static void wtd_proc_check()
{
	llist plist;
	
	printf("[wtd]proc check...\n");
	list_create(&plist);
	
	readconfig_raw(CONF_PROC/*"/root/tangr/webadmin/proclist.conf"*/, &plist);

	list_traverse(&plist, wtd_visit, NULL);

	list_clear(&plist);
}

static void wtd_wait()
{
	int status;
	pid_t child;

	child=wait(&status);
	if(child<0)
	{
		perror("[wtd]");
		sleep(6);
		return;
	}
	
	printf("[wtd]proc %d got down ",child);
	if(WIFEXITED(status))
		printf("by exit(%d)\n",WEXITSTATUS(status)&0xff);
	else if(WIFSIGNALED(status))
		printf("by singal %d\n",WTERMSIG(status));
	else if(WIFSTOPPED(status))
		printf("by singal %d\n",WSTOPSIG(status));
	
	return;
}

static void wtd_sigchild_handler(int sig,siginfo_t *info,void *obsolete)
{
	wtd_wait();
}

int main(int argc , char  ** argv)
{

	struct sigaction sa;
//	ver_log();
//	ver_opt(argc,argv);
	printf("end.\n");
	sa.sa_sigaction=wtd_sigchild_handler;
	sa.sa_flags=SA_SIGINFO;//|SA_NOCLDWAIT;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGCHLD,&sa,NULL);


	while(1)
	{
		static int i=0;

		if(i<6)
			i+=2;
		
		wtd_proc_check();
		sleep(i);

	}
	return 0;
}

