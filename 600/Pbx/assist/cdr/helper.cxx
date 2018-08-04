#include "helper.hxx"
#include <unistd.h>
#include <dirent.h>
#include <stddef.h>
#include <stdarg.h>

//const char* fname = "/status";
//const char* pfn = "/proc/";

void cdr_debug_info(int pri, const char* file, int line, const char* frmt,...)
{
	va_list ap;
	va_start(ap, frmt);
	char buf[1024] = {0};
	vsnprintf(buf, 1024, frmt, ap);
	va_end(ap);
	
	printf("%s: %d  %s\n",file, line, buf);
}


int get_processid(const char* proname)
{	
	const char* fname = "/status";
      const char* pfn = "/proc/";
	  
	DIR *dp;
  	struct dirent *ep;
	dp = opendir (pfn);

	if (dp == NULL)
	{
	     perror("open dir");
	     return -1;
	}

	bool opened = false;

      while ((ep = readdir (dp)))
	{ 
             if (ep->d_name[0] < '0' || ep->d_name[0] > '9' )
			continue;

		pid_t mypid = getpid();
		if (mypid == (pid_t)atoi(ep->d_name))
		{
			//cpDebug(LOG_DEBUG, "this is me, continue!");
			continue;
		}
			 
		char fb[128] = {0};
	      strcpy(fb, pfn);
	      strcat(fb, ep->d_name);
	      strcat(fb, fname);

		FILE *fp = fopen(fb, "rb");
		if (fp == NULL)
		{
			perror("open file");
			continue;
		}

		opened = true;
		
		char buffer[1024] = {0};
	      char proc[128] = {0};
		
		char *pp = proc;
		char *pb = buffer;
		bool nst = false;
		
		while (!feof(fp))
		{
			
			char c = getc(fp);
			if (c == ' ' || c == '\t')	continue;
			if (!nst)*pb++ = c;
			if (strcmp(buffer, "Name:") == 0 && !nst)nst = true;
			else if ((c == '\r' || c == '\n' )&& nst)break; //update 2005/05/25 <--  if (c == '\r' || c == '\n' && nst)
			else if (nst) *pp++ = c;
		}

		fclose(fp);
	
		if (strcmp(proc, proname) == 0)
		{		
			int id = atoi(ep->d_name);
#if 0			
			pid_t pid = (pid_t)id;
#endif
			return id;
		}
		
      	}

  
	(void) closedir (dp); 
	if (opened)
		return -2;
	else
		return -1;
    	     
}


