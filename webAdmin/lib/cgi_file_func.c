/*
* $Author: lizhijie $
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cgiLib.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

llist entries;


void cgi_read_local_config(char *filename,void(*parseLocalConfig)(char *, void *data), void *data)
{
	FILE *cfgfp;
	char line[PARSE_FILE_MAX_LENGTH];
    //    char value[SIP_VALUE_MAX_LENGTH];
//	int keyCount, curParsePos=0;
	int i,lineLen;

	if((cfgfp=fopen(filename,"r"))==NULL)
	{
		fprintf(stderr, "Open file '%s' failure :%s!\n", filename, strerror(errno) );
		return;
	}

	if(entries.head!=NULL)
		list_clear(&entries);

	while (fgets(line,PARSE_FILE_MAX_LENGTH,cfgfp))
	{
		lineLen = strlen(line);
		if(lineLen<1)
			continue;

		if(line[0]=='#'||line[0]=='\n'||line[0]=='\r')
			continue;
        /*tram line end, delete blank and '\t' '\n' '\r' char*/
		for(i=lineLen-1;i>0;i--)
		{
			if(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r')
				line[i]='\0';
			else
				break;
		}
		if(parseLocalConfig)
			(*parseLocalConfig)(line, data);
        //        fprintf(stderr,"%s too few paramters!\n",key);
	}
	fclose(cfgfp);
}


int cgi_domain_name_write(char *domainname)
{
	FILE *fp;
	
	if(( fp=fopen(CGI_DOMAIN_FILE,"w"))==NULL)
	{
		fprintf(stderr, "Open file %s failure!", CGI_DOMAIN_FILE );
		return -1;
	}
	
	fprintf(fp, "#domain for AS600\n\n");
	fprintf(fp, "DOMAIN_NAME=%s\n" , domainname);

	fclose(fp);

	return 0;
}

char *cgi_domain_name_read()
{
	llist list = {NULL};

	if (!readconfig(CGI_DOMAIN_FILE,ITEM_EQU, "DOMAIN_NAME", &list) )
	{
		return strdup(list.head->entry.value);
	}

	cgidebug( "No DomainName is read in file of %s!" , CGI_DOMAIN_FILE );
	return "";

}

int cgi_host_name_write(char *hostname)
{
	FILE *fp;
	
	if(( fp=fopen(CGI_HOST_NAME_FILE,"w"))==NULL)
	{
		cgidebug("Open file %s failure!", CGI_DOMAIN_FILE );
		return -1;
	}
	
	fprintf(fp, "#hostname for AS600\n\n");
	fprintf(fp, "HOST_NAME=%s\n" , hostname);

	fclose(fp);

	return 0;
}

char *cgi_host_name_read()
{
	llist list ={NULL};

	if (!readconfig(CGI_HOST_NAME_FILE,ITEM_EQU, "HOST_NAME", &list) )
	{
		cgidebug( "No DomainName isof %s!" , CGI_DOMAIN_FILE );
		cgidebug( "No DomainName is read in file of %s!" , list.head->entry.value );
		return strdup(list.head->entry.value);
	}

	cgidebug( "No DomainName is read in file of %s!" , CGI_DOMAIN_FILE );
	return strdup("SONiX");
}

