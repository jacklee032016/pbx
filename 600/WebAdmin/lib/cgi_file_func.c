/*
* $Author: lizhijie $
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cgi_lib.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

llist entries;

#if WITH_STATIC_HTML
void cgi_parse_config(char *key, char *value)
{
	entrytype *newentry;
    //    node *newnode;

	newentry->name=key;
	newentry->value=value;

    //    newnode=entries.head;

/* add this function implementation */
#if 0    
	list_append(&entries,  newentry);
#endif
}

static void cgi_parse_ret_value(char* key, void(*specialparse)(char *,void *), void *data)
{
#if 0
	int i=0;
	char *ret;
#endif

	if(specialparse != NULL)
	{
		(*specialparse)(key, data);
	}	

#if 0
	ret=cgi_value(entries,key);
	if(ret!=NULL)
	{
		while( ret[i]!='\0' )
		{
				putchar(ret[i]);
				i++;
		}
	}
#endif	
}

int cgi_create_out_html_file(char *filename,  void(*specialparse)(char *, void *), void *data )
{
	FILE *fp;
	int i, j;
	
	char key[KEY_MAX_LENGTH];
	char line[PARSE_FILE_MAX_LENGTH];
	int lineLen;

	if(( fp=fopen(filename, "r"))==NULL)
	{
		fprintf(stderr, "Open file '%s' failure:%s!\n", filename, strerror(errno) );
		return 1;
	}

	memset(line, 0, PARSE_FILE_MAX_LENGTH);
	memset(key, 0, KEY_MAX_LENGTH);
	
	/*All arguments is define as the form of  "%*xxx#%" */
	while (fgets(line,PARSE_FILE_MAX_LENGTH, fp))
	{
		lineLen = strlen(line);

		if(line[0]=='#'||line[0]=='\n'||line[0]=='\r')
			continue;
		
		for(i=0; i<lineLen-1 ; i++)
		{
			if(line[i]=='%' && line[i+1]=='*' )
			{
				for(j=i+1; j<lineLen-1; j++)
				{
					if(line[j]=='$' && line[j+1]=='%' )
					{
						
						strncpy(key, line+i+2, j-(i+2) );
						if(specialparse)
						{
							i = j+1;
							cgi_parse_ret_value(key, specialparse, data);
						}
					}
				}
			}
			else
			{
				putchar(line[i] );
			}
		}

		putchar(line[lineLen-1]);
		memset(key, 0, KEY_MAX_LENGTH);
		memset(line , 0 , PARSE_FILE_MAX_LENGTH );
	}
	
	fclose( fp );

	return 0;
}


char* cgi_change_config_value(char *line, char* newvalue)
{
	int i,count;

	i=0;
	count=0;

	if(line == NULL)
		return NULL;
    //	memset(newline,0,sizeof(newline));
    //	newline[0]='\0';
	while( line[i] != '\n')
	{
		if(line[i]=='\t')
			count++;
		i++;
		if(count==2)
			break;
	}
	
    //	CGI_SPRINTF(newline,i,"%s",line);
	if(newvalue!=NULL)
	{
		line[i]='\0';
		strcat(line,newvalue);
	}
	else
		line[i-1]='\0';/*replace '\t'*/

	return line;
}

void cgi_write_new_config_value(entrytype *entry, FILE* out)
{
	char line[PARSE_FILE_MAX_LENGTH];
	sprintf(line, "%s\t%s\t%s\n",entry->name, "string", entry->value);
	fputs(line,out);
}

void cgi_write_local_config(char *filename,int(*wirte_special_fields)(char *, node* , FILE*))
{
	FILE *fp,*tmpfp;
	char line[PARSE_FILE_MAX_LENGTH];
	char tmpfilename[1024];
	char *newline;
	node* curNode;
	int retflag,ischanged=0;

	if((fp=fopen(filename,"r"))==NULL)
	{
		perror( "Open config file failure!" );
		return;
	}

	sprintf(tmpfilename,"%s%s.%d","/tmp",strrchr(filename,'/'),(int)getpid());
	if((tmpfp=fopen(tmpfilename,"w"))==NULL)
	{
		perror( "Open tmp file failure!" );
		return;
	}
	
	while (fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		ischanged=0;
		if(line[0]=='#' || line[0]=='\n')
		{
			fputs(line,tmpfp);
			continue;
		}
		curNode=entries.head;

		while(curNode!=NULL)
		{
			if(strstr(line,curNode->entry.name)==line)
			{
				ischanged=1;
				if(wirte_special_fields)
					retflag=(*wirte_special_fields)(line, curNode,tmpfp);
				
				if( retflag)
					break;
				
				newline=cgi_change_config_value(line, curNode->entry.value);
				if(newline!=NULL)
				{
					fputs(newline, tmpfp);
					fputc('\n',tmpfp);
/* this function must be implemented */					
#if 0					
					list_delete_node(&entries,curNode);
#endif
					break;
				}
			}
			curNode=curNode->next;
		}
		
		if(!ischanged)
			fputs(line,tmpfp);
	}
	curNode=entries.head;
	
	while(curNode !=NULL)
	{
		if(!strcmp(curNode->entry.name,"Submit"))
			cgi_write_new_config_value(&curNode->entry,tmpfp);
		curNode=curNode->next;
	}
	
	fclose(fp);
	fclose(tmpfp);
}
#endif


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
	list_create(&entries);

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
	llist list;

	list_create(&list);

	if (!readconfig(CGI_DOMAIN_FILE,ITEM_EQU, "DOMAIN_NAME", &list) )
	{
		return strdup(list.head->entry.value);
	}

	cgidebug( "No DomainName is read in file of %s!" , CGI_DOMAIN_FILE );
	return "";

#if 0
	FILE *fp;
	char line[PARSE_FILE_MAX_LENGTH];
	
	if((fp=fopen(CGI_DOMAIN_FILE,"r"))==NULL)
	{
		fprintf(stderr, "Open config file %s failure!" , CGI_DOMAIN_FILE );
		return NULL;
	}

	while (fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		if(line[0]=='#' || line[0]=='\n')
		{
			continue;
		}
		break;
	}
	
	fclose(fp);

	return strdup(line);	
#endif

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
	llist list;

	list_create(&list);

	if (!readconfig(CGI_HOST_NAME_FILE,ITEM_EQU, "HOST_NAME", &list) )
	{
		return strdup(list.head->entry.value);
	}

	cgidebug( "No DomainName is read in file of %s!" , CGI_DOMAIN_FILE );
	return strdup("as600");
}

