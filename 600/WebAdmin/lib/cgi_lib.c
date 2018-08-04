/*
* cgi-lib.c - C routines that make writing CGI scripts in C a breeze
* $Id: cgi_lib.c,v 1.14 2007/09/06 09:19:58 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef WINDOWS
#include <io.h>
#endif
#include "aslib.h"
#include "cgi_lib.h"

//add dmalloc complie contition by hgp!!!!
/*****************************************/
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "cgi_syscfg.h"

FILE		*localeFp = NULL;

/*****************************************/

/* symbol table for CGI encoding */
#define _NAME 		0
#define _VALUE 		1

//global vars
char _err[100];
char *err=_err;


int writeline(entrytype *entry,int itemnum, FILE* fname)
{
	char line[LINE_MAX_LENGTH];
	int n=itemnum;

	switch(n)
	{
		case ITEM_DBL:
			sprintf(line, "%s\t%s\n",entry->name, entry->value);
			break;
		case ITEM_TRI:
			sprintf(line, "%s\t%s\t%s\t%s\n",entry->name, entry->type, entry->value, entry->rule);
			break;
		case ITEM_BLCK_LIST:
//			sprintf(line,"%s\n",entry->value);
			sprintf(line,"%s\n",entry->name);
			break;
		case ITEM_EQU:
			sprintf(line,"%s=%s\n",entry->name,entry->value);
			break;
		case ITEM_FOR:/* for chap and pap-secret */
			sprintf(line,"\"%s\"\t%s\t\"%s\"\t%s\n", entry->type, entry->name, entry->value, PPPOE_KEYWORD_SERVER_NAME_ASTERIC);//entry->rule);
			break;
		default:
			//here for expansion
			//return;
			break;
	}
	
	if(fputs(line,fname)==EOF)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int readline(entrytype *entry,int itemnum, FILE* fname)
{
	char line[LINE_MAX_LENGTH];
	int i,j,linelen,nover,vover;
	int n=itemnum;
	char *retptr;
	char string[ITEM_LENGTH+1];

	memset(entry->name,0,ITEM_LENGTH+1);
	memset(entry->value,0,ITEM_LENGTH+1);
	memset(entry->type,0,ITEM_LENGTH+1);
	memset(entry->rule,0,ITEM_LENGTH+1);
	memset(string,0,ITEM_LENGTH+1);
	//read one line from file, not include null such as #, \n, \r;
	while((retptr=fgets(line,LINE_MAX_LENGTH,fname))!=NULL)
	{
		linelen = strlen(line);
		if(linelen<1)
			continue;
		if(line[0]=='#'||line[0]=='\n'||line[0]=='\r')
			continue;
		break;
	}
	
	if(!retptr)
	{
		return -1;
	}

	switch(n)
	{
		case ITEM_ONE:
//			cgidebug("line=%s\n", line);
			i=0;
			while(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r' )//|| line[i]=='=')
				i++;
			for(j=0;line[i];i++,j++)
			{
				entry->name[j]=line[i];
				if(line[i]=='\n' || line[i]=='\r')//|| line[i]=='=')
				{
					entry->name[j]='\0';
				}
			}
			strncpy(entry->value, entry->name, (strlen(entry->name)<ITEM_LENGTH)?strlen(entry->name):ITEM_LENGTH);
			break;
			
		case ITEM_DBL:
			i=0;
			while(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r' )//|| line[i]=='=')
				i++;
			for(j=0,nover=0;line[i]&&!nover;i++,j++)
			{/* first not null field */
				entry->name[j]=line[i];
				if(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r')//|| line[i]=='=')
				{
					entry->name[j]='\0';
					nover=1;
				}
			}
			
			if(!nover)
			{
				perror("conf itemname error\n");
				return -1;
			}
			
			while(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r')
				i++;

			if(!line[i])
			{
				entry->value[0]=0;
				break;
			}
#if 0
			if(line[i] == '"')
			{
				i++;
				for(j=0; j < (linelen-i); j++)
				{
					entry->value[j]=line[i++];
				}
			}
			else
#endif				
			{
			
				for(j=0,vover=0;line[i]&&!vover;i++,j++)
				{
					entry->value[j]=line[i];
					if(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r')
					{
						entry->value[j]='\0';
						vover=1;
					}
				}

				if(!vover)
					entry->value[j]=line[i];
			}
			break;
		case ITEM_TRI:
		{
			int __count = sscanf(line, "%s %s %s %s", 
				entry->name, entry->type, entry->value,entry->rule);
//			cgidebug("count =%d : %s\n", __count, line);
			if(__count != 4)
			{
				__count =sscanf(line,"%s %s %s", entry->name, entry->type, entry->value );
				if(__count != 3)
					return 1;/* continue to next line */	
//				entry->rule[0] = '\0';
			}
			if( strchr(entry->name, '=') )
				return 1;/* this is EQL item in this config file */

			break;
		}	
		case ITEM_TRI_DHCP:
		{
			sscanf(line,"%s %s %s", entry->name, entry->type, entry->value );
			break;
		}	
		case ITEM_BLCK_LIST:
//			strcpy(entry->name,"blck_list");
//			sscanf(line,"%s\n",entry->value);
			sscanf(line,"%s\n",entry->name);
			break;
			
		case ITEM_IP_FORWARD:
			strcpy(entry->name,"ip_forward");
			sscanf(line,"%s",entry->value);
			break;
			
		case ITEM_FOR:/* for chap and pap-secret */
		{
//			int _len = 0;
			char		name[128], passwd[128];
			sscanf(line,"%s %s %s %s", name, entry->name, passwd, entry->rule);
			strncpy(entry->type, name+1, strlen(name)-2);
			strncpy(entry->value, passwd+1, strlen(passwd)-2);
#if 0			
			entry->type[strlen(entry->type)-1] = '\0';
			entry->type = entry->type+1;
			entry->value[strlen(entry->value)-1] = '\0';
			entry->value = entry->value+1;
#endif
			break;
		}	
		case ITEM_EQU:
			{	
				linelen =strlen(line);
				for(i=linelen-1;i>0;i--)
				{
					if(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r')
						line[i]='\0';
					else
						break;
				}
				char *p;
				p=strtok(line,"=");
				if(p)
					strcpy(entry->name,p);
				else
					return 1; /* continue to next line */
				p=strtok(NULL,"=");
				if(p)
					strcpy(entry->value,p);
			}
			break;
		default:
			break;
	}
	return 0;
}

int readconfig(char *filename,int itemnum,char *ctype,llist *l)
{
	FILE *cfgfp;
	entrytype tmpentry;
	int res;

	if((cfgfp=fopen(filename,"r"))==NULL)
	{
		return -1;
	}
	tmpentry.name=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	tmpentry.value=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	tmpentry.type=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	tmpentry.rule=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	
	if(l->head!=NULL)
		list_clear(l);
	list_create(l);

	cgitrace;
	while ( (res= readline(&tmpentry, itemnum, cfgfp) ) >= 0  )
	{
		if(res == 0)
		{
			if(ctype)
			{
				if(!strstr(tmpentry.name,ctype))
					continue;
			}
			list_append(l, &tmpentry);
		}
	}
	cgitrace;
	
	free(tmpentry.name);
	free(tmpentry.value);
	free(tmpentry.type);
	free(tmpentry.rule);
	fclose(cfgfp);
	return 0;
}

int writeconfig(char *filename,int itemnum,char *ctype,llist *l)
{
	FILE *cfgfp;
	node *curnode;

//	cgidebug( "writeconfig(): head %p, ctype %s\n" ,l->head,ctype);
	
	if(l->head==NULL)
	{
		return -1;
	}

	if((cfgfp=fopen(filename,"w"))==NULL)
	{
		cgidebug( "writeconfig(): open file %s error : %s\n" ,filename, strerror(errno) );
		return -1;
	}


	curnode=l->head;

	while(curnode!=NULL)
	{
		if(!strcmp(curnode->entry.name,CTYPE)||!strcmp(curnode->entry.name,VTYPE)||
			!strcmp(curnode->entry.name,SUBMIT)||!strcmp(curnode->entry.name,RESET) ||
			!strcmp(curnode->entry.name, CGI_KEYWORD_OPERATOR) ||
			!strcmp(curnode->entry.name, CGI_ACTION ) )
		{
			curnode=curnode->next;
			continue;
		}
		
		if(ctype&&!strstr(curnode->entry.name,ctype))
		{
			curnode=curnode->next;
			continue;
		}
		if(writeline(&curnode->entry, itemnum, cfgfp))
		{
			fclose(cfgfp);
			return -1;
		}
		curnode=curnode->next;
	}

	fclose(cfgfp);
	return 0;
}

int config_file_replace_field(char *filename, int itemnum,char *key, char *newValue)
{
	llist fields;
	int	ret = 0;

	list_create( &fields);
	if(readconfig(filename, itemnum, NULL, &fields)<0 )
	{
		cgidebug("read config file '%s' failed\n", filename );
		return -1;
	}

	list_replace_value_by_key( &fields, key, newValue);

	ret = writeconfig(filename, itemnum, NULL, &fields);
	list_clear( &fields);

	return ret;
}

int readconfig_raw(char *filename,llist *l)
{
	FILE *cfgfp;
	entrytype tmpentry;
	char *retptr;

	if((cfgfp=fopen(filename,"r"))==NULL)
	{
		cgidebug( "writeconfig(): open file %s error : %s\n" ,filename, strerror(errno) );
		return -1;
	}

	tmpentry.value=(char *)malloc(sizeof(char)*LINE4RAW);
	if(!tmpentry.value)
	{
		fclose(cfgfp);
		return -1;
	}

	tmpentry.name=(char *)malloc(sizeof(char)*LINE4RAW);
	if(!tmpentry.name)
	{
		fclose(cfgfp);
		free(tmpentry.value);
		return -1;
	}
	
	if(l->head!=NULL)
		list_clear(l);
	
	list_create(l);

	while((retptr=fgets(tmpentry.value,LINE4RAW,cfgfp))!=NULL)
	{
		sscanf(tmpentry.value,"%s",tmpentry.name);
		list_append(l, &tmpentry);
	}
	
	free(tmpentry.name);
	free(tmpentry.value);
	fclose(cfgfp);
	
	return 0;
}

int writeconfig_no_override(char *filename,void (*dealline)(char *line,void *data,void *refdata), llist *l)
{
	FILE *cfgfp;
	node *curnode,*rawcurnode;
	entrytype tmpentry;
	llist rawlist;
	llist *rawl=&rawlist;
	char *tmpline;
	int tmplen;

	if(l->head==NULL)
		return -1;
	
	memset(err,0,100);
	list_create(rawl);
	readconfig_raw(filename, rawl);
	#if 0
	print_entries(*l);
	print_entries(*rawl);
	#endif
	curnode=l->head;
	while(curnode!=NULL)
	{
		if(!strcmp(curnode->entry.name,CTYPE)||!strcmp(curnode->entry.name,VTYPE)||
			!strcmp(curnode->entry.name,CGI_KEYWORD_OPERATOR )||
			!strcmp(curnode->entry.name,SUBMIT)||!strcmp(curnode->entry.name,RESET)||!strcmp(curnode->entry.name,"menu1")||!strcmp(curnode->entry.name,"menu2"))
		{
			curnode=curnode->next;
			continue;
		}
		if(strlen(curnode->entry.value))//modify
		{
			rawcurnode=cgi_locatename_raw(rawl, curnode->entry.name,0);
			if(!rawcurnode)rawcurnode=cgi_locatename_raw(rawl, curnode->entry.name,1);
			tmplen=sizeof(char)*LINE4RAW;
			tmpline=(char *)malloc(tmplen);
			if(!tmpline)
			{
				list_clear(rawl);
				sprintf(err,"%s() mem alloc",__FUNCTION__);
//				cgi_error(err);
				cgidebug("%s\n", err);
				exit(-1);
			}
			memset(tmpline,0,tmplen);
			(*dealline)(tmpline,curnode,rawcurnode);
			if(!strlen(tmpline)){free(tmpline);curnode=curnode->next;continue;}
			if(rawcurnode)
			{
				free(rawcurnode->entry.value);
				rawcurnode->entry.value=tmpline;
			}
			else 
			{
				tmpentry.name="";
				tmpentry.value=tmpline;
				list_append(rawl, &tmpentry);
			}
		}
		else//note this line
		{
//			cgidebug("here: node %s",curnode->entry.name);	
			rawcurnode=cgi_locatename_raw(rawl, curnode->entry.name,0);
			if(rawcurnode)
			{
//				cgidebug(" rawnode:name %s, value %s",rawcurnode->entry.name,rawcurnode->entry.value);
				tmplen=sizeof(char)*(strlen(rawcurnode->entry.value)+1+2);
				tmpline=(char *)malloc(tmplen);
				if(!tmpline)
				{
					list_clear(rawl);
					sprintf(err,"%s() mem alloc",__FUNCTION__);
//					cgi_error(err);
					cgidebug("%s\n", err);
					exit(-1);
				}
				memset(tmpline,0,tmplen);
				strcpy(tmpline,"# ");
				strcat(tmpline,rawcurnode->entry.value);
				free(rawcurnode->entry.value);
				rawcurnode->entry.value=tmpline;
				cgidebug(" rawnode:name %s, value %s", rawcurnode->entry.name,rawcurnode->entry.value);
			}
		}
		curnode=curnode->next;
	}

	if((cfgfp=fopen(filename,"w"))==NULL)
	{
		list_clear(rawl);
		cgidebug( "%s() can't write file %s", __FUNCTION__,filename);
//		cgi_error(err);
		exit(-1);
	}
	rawcurnode=rawl->head;
	while(rawcurnode)
	{
		if(rawcurnode->entry.value==NULL)exit(0);
		fputs(rawcurnode->entry.value,cfgfp);
		rawcurnode=rawcurnode->next;
	}
	list_clear(rawl);
	fclose(cfgfp);
	return 0;
}

void *wrapper_malloc(size_t size)
{
	void *p;

	p=malloc(size);
	if(!p)
	{
		perror("malloc");
		exit(-1);
	}
	
	return p;
}

/* x2c() and unescape_url() stolen from NCSA code */
char x2c(char *what)
{
	register char digit;

	digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
	digit *= 16;
	digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
	return(digit);
}

void unescape_url(char *url)
{
    register int x,y;

    for (x=0,y=0; url[y]; ++x,++y)
    {
        if((url[x] = url[y]) == '%')
        {
            url[x] = x2c(&url[y+1]);
            y+=2;
        }
    }
    url[x] = '\0';
}

char *get_DEBUG()
{
	int bufsize = 1024;
	char *buffer = (char *)malloc(sizeof(char) * bufsize + 1);
	int i = 0;
	char ch;

	fprintf(stderr,"\n--- cgihtml Interactive Mode ---\n");
	fprintf(stderr,"Enter CGI input string.  Remember to encode appropriate ");
	fprintf(stderr,"characters.\nPress ENTER when done:\n\n");
	while ( (i<=bufsize) && ((ch = getc(stdin)) != '\n') )
	{
		buffer[i] = ch;
		i++;
        if (i>bufsize)
        {
            bufsize *= 2;
            buffer = (char *)realloc(buffer,bufsize);
        }
    }
    buffer[i] = '\0';
    fprintf(stderr,"\n Input string: %s\nString length: %d\n",buffer,i);
    fprintf(stderr,"--- end cgihtml Interactive Mode ---\n\n");
    return buffer;
}

/* return all content with char pointer */
char *get_POST()
{
	unsigned int content_length;
	char *buffer;

	if (CONTENT_LENGTH != NULL)
	{
		content_length = atoi(CONTENT_LENGTH);
		buffer = (char *)malloc(sizeof(char) * content_length + 1);

		if (fread(buffer,sizeof(char),content_length,stdin) != content_length)
		{
            /* consistency error. */
			cgidebug("caught by cgihtml: input length < CONTENT_LENGTH\n") ;
			exit(1);
		}
		buffer[content_length] = '\0';
	}
	return buffer;
}

char *get_GET()
{
	char *buffer;

	if (QUERY_STRING == NULL)
		return NULL;
	buffer = cgi_new_string(QUERY_STRING);
	return buffer;
}

/* parse content buffer into llist , return <0 when error */
int parse_CGI_encoded(llist *entries, char *buffer)
{
	int i, j, num, token;
	int len = strlen(buffer);
	char *lexeme = (char *)malloc(sizeof(char) * len + 1);
	entrytype entry;
	node *window;

//	list_create(entries);
	window = entries->head;
	entry.name = NULL;
	entry.value = NULL;
	entry.type = NULL;
	entry.rule = NULL;
	i = 0;
	num = 0;
	token = _NAME;

	while (i < len)
	{
		j = 0;
		while ( (buffer[i] != '=') && (buffer[i] != '&') && (i < len) )
		{
			lexeme[j] = (buffer[i] == '+') ? ' ' : buffer[i];
			i++;
			j++;
		}
		lexeme[j] = '\0';

		if (token == _NAME)
		{
			entry.name = cgi_new_string(lexeme);
			unescape_url(entry.name);

			if ( (buffer[i] != '=') || (i == len - 1) )
			{
				entry.value = (char *)malloc(sizeof(char));
				strcpy(entry.value,"");
				window = list_insafter(entries, window, entry);
				free(entry.name);
				entry.name = NULL;
				free(entry.value);
				entry.value = NULL;
				if (i == len - 1) /* null value at end of expression */
					num++;
				else
				{ /* error in expression */
					free(lexeme);
					return -1;
				}
			}
			else
				token = _VALUE;
		}
		else
		{
			entry.value = cgi_new_string(lexeme);
			unescape_url(entry.value);
			window = list_insafter(entries, window, entry);
			free(entry.name);
			entry.name = NULL;
			free(entry.value);
			entry.value = NULL;
			token = _NAME;
			num++;
		}
		i++;
		j = 0;
	}
	
	free(lexeme);
	if (entry.name != NULL)
		free(entry.name);
	if (entry.value != NULL)
		free(entry.value);

	return num;
}

/* stolen from k&r and seriously modified to do what I want */

int getline(char s[], int lim)
{
	int c, i=0, num;

	for (i=0; (i<lim) && ((c=getchar())!=EOF) && (c!='\n'); i++)
	{
		s[i] = c;
	}
	if (c == '\n')
	{
		s[i] = c;
	}
	if ((i==0) && (c!='\n'))
		num = 0;
	else if (i == lim)
		num = i;
	else
		num = i+1;

	return num;
}

int parse_form_encoded(llist* entries, int (*callback)(char *, void *), void *data)
{
	long content_length;
	entrytype entry;
	node	*window;
	FILE		*uploadfile;
	char *uploadfname, *tempstr, *boundary;
	char *buffer = (char *)malloc(sizeof(char) * BUFSIZ + 1);
	char *prevbuf = (char *)malloc(sizeof(char) + BUFSIZ + 1);
	short isfile,done,start;
	int i,j;
	int bytesread,prevbytesread;
	int buffersize;
	int numentries = 0;

#ifdef WINDOWS
	setmode(fileno(stdin), O_BINARY);	/* define stdin as binary */
	_fmode = BINARY;                    		/* default all file I/O as binary */
#endif

	if (CONTENT_LENGTH != NULL)
		content_length = atol(CONTENT_LENGTH);
	else
		return 0;

	/* get boundary */
	tempstr = cgi_new_string(CONTENT_TYPE);
	boundary = strstr(tempstr,"boundary=");
	boundary += (sizeof(char) * 9);
	
	/* create list */
//	list_create(entries);
	
	window = entries->head;
	/* ignore first boundary; this isn't so robust; improve it later */
	getline(buffer,BUFSIZ);
	/* now start parsing */
	
	while ((bytesread=getline(buffer,BUFSIZ)) != 0)
	{
		start = 1;
		
		/* this assumes that buffer contains no binary characters.
		if the buffer contains the first valid header, then this
		is a safe assumption.  however, it should be improved for
		robustness sake. */
		buffer[bytesread] = '\0';
		tempstr = cgi_new_string(buffer);
		tempstr += (sizeof(char) * 38); /* 38 is header up to name */
		entry.name = tempstr;
		entry.value = (char *)malloc(sizeof(char) * BUFSIZ + 1);
		buffersize = BUFSIZ;
		strcpy(entry.value,"");

		while (*tempstr != '"')
			tempstr++;

		*tempstr = '\0';
		if (strstr(buffer,"filename=\"") != NULL)
		{
			isfile = 1;
			tempstr = cgi_new_string(buffer);
			tempstr = strstr(tempstr,"filename=\"");
				tempstr += (sizeof(char) * 10);

			if (strlen(tempstr) >= BUFSIZ)
				entry.value = (char *) realloc(entry.value, sizeof(char)*strlen(tempstr)+1);

			entry.value = tempstr;
			while (*tempstr != '"')
				tempstr++;

			*tempstr = '\0';
			/* Netscape's Windows browsers handle paths differently from its
			UNIX and Mac browsers.  It delivers a full path for the uploaded
			file (which it shouldn't do), and it uses backslashes rather than
			forward slashes.  No need to worry about Internet Explorer, since
			it doesn't support HTTP File Upload at all. */

			if (strstr(cgi_lower_case(HTTP_USER_AGENT),"win") != 0)
			{
				tempstr = strrchr(entry.value, '\\');
				if (tempstr)
				{
					tempstr++;
					entry.value = tempstr;
				}
			}

			window = list_insafter(entries,window,entry);
//			cgidebug("name=%s\tvalue=%s\n", entry.name, entry.value );

			numentries++;
			uploadfname = (char *)malloc(strlen(UPLOADDIR)+strlen(entry.value)+2);
#ifdef WINDOWS
			sprintf(uploadfname,"%s\\%s",UPLOADDIR,entry.value);
#else
			sprintf(uploadfname,"%s/%s",UPLOADDIR,entry.value);
#endif
			if(callback)
			{
				if((callback)(entry.value, data) )
//					isfile = 0;
					return 1;
			}
			
			if (isfile && (uploadfile = fopen(uploadfname,"w")) == NULL)
			{/* null filename; for now, just don't save info later, save to default file */
				isfile = 0;
			}
		}
		else
			isfile = 0;

		/* ignore rest of headers and first blank line */
		while (getline(buffer, BUFSIZ) > 1)
		{
			/* DOS style blank line? */
			if ((buffer[0] == '\r') && (buffer[1] == '\n'))
				break;
		}

		done = 0;
		j = 0;

		while (!done)
		{
			bytesread = getline(buffer,BUFSIZ);
			buffer[bytesread] = '\0';
			
			if (bytesread && strstr(buffer,boundary) == NULL)
			{
				if (start)
				{
					i = 0;
					while (i < bytesread)
					{
						prevbuf[i] = buffer[i];
						i++;
					}
					prevbytesread = bytesread;
					start = 0;
				}
				else
				{
					/* flush buffer */
					i = 0;
					while (i < prevbytesread)
					{
						if (isfile)
							fputc(prevbuf[i],uploadfile);
						else
						{
							if (j > buffersize)
							{
								buffersize += BUFSIZ;
								entry.value = (char *) realloc(entry.value, sizeof(char)*buffersize+1);
							}

							entry.value[j] = prevbuf[i];
							j++;
						}
						i++;
					}

					/* buffer new input */
					i = 0;
					while (i < bytesread)
					{
						prevbuf[i] = buffer[i];
						i++;
					}
					prevbytesread = bytesread;
				}
			}
			else
			{
				done = 1;
				/* flush buffer except last two characters */
				i = 0;
				while (i < prevbytesread - 2)
				{
					if (isfile)
						fputc(prevbuf[i],uploadfile);
					else
					{
						if (j > buffersize)
						{
							buffersize += BUFSIZ;
							entry.value = (char *) realloc(entry.value, sizeof(char)*buffersize+1);
						}
						entry.value[j] = prevbuf[i];
						j++;
					}
					i++;
				}

			}
		}

		if (isfile)
			fclose(uploadfile);
		else
		{
			entry.value[j] = '\0';
			window = list_insafter(entries,window,entry);
			numentries++;
			j = 0;
		}
	}
	
	return numentries;
}

/*
 * return: <0 error in QUERY_STRING or POST string
 *            =0 no QUERY_STRING or POST string
 *            >0 normal
 */
int read_cgi_input(llist* entries, int (*callback)(char *, void *), void *data)
{
	char *input;
	int status;

	/* check for form upload.  this needs to be first, because the
	standard way of checking for POST data is inadequate.  If you
	are uploading a 100 MB file, you are unlikely to have a buffer
	in memory large enough to store the raw data for parsing.
	Instead, parse_form_encoded parses stdin directly.

	In the future, I may modify parse_CGI_encoded so that it also
	parses POST directly from stdin.  I'm undecided on this issue,
	because doing so will make parse_CGI_encoded less general. */
	if ((CONTENT_TYPE != NULL) &&	(strstr(CONTENT_TYPE,"multipart/form-data") != NULL) )
		return parse_form_encoded(entries, callback, data);

	/* get the input */
	if (REQUEST_METHOD == NULL)
		input = get_DEBUG();
	else if (!strcmp(REQUEST_METHOD,"POST"))
		input = get_POST();
	else if (!strcmp(REQUEST_METHOD,"GET"))
		input = get_GET();
	else
	{ /* error: invalid request method */
		fprintf(stderr, "caught by cgihtml: REQUEST_METHOD '%s' invalid\n", REQUEST_METHOD);
	}
	
	/* parse the input */
	if (input == NULL)
		return 0;
	status = parse_CGI_encoded(entries,input);

	free(input);
	return status;
}

//~tangr for writeconfigure by no override
//note 1: locate at notes line 0: locate at act line
node *cgi_locatename_raw(llist *l, char *name, int note)
{
	short FOUND = 0;
	node* window;

	if(!l)return NULL;
	window = l->head;
	while ( (window != 0) && (!FOUND) )
	{
		if(note)
		{
			if (window->entry.name[0]=='#'&&strstr(window->entry.value,name))
				FOUND = 1;
			else
				window = window->next;
		}
		else
		{
			if (window->entry.name[0]!='#'&&strstr(window->entry.value,name))
				FOUND = 1;			
			else
				window = window->next;
		}
	}
	
	if (FOUND)
		return window;
	else
		return NULL;
}


/***************************************
 *name: cgi_html_parse_select()
 *func: to output value from l in  <select>...</select> of  a html file
 *return: 1 - <select> found and succeed in output, the lineout[] has the line to output
 *           0 - no <select>
 *notice: <select> ... </select> must include in one line and may not write another <select> or <input>
 *           in the same line
 ***************************************/
int cgi_html_parse_select(char *linein,char *lineout, llist *l)
{
	char tag[100], name[64],value[64];
	char *pin,*ptmp,*pfind;
	int length;

	if(!(pin=strstr(linein,"<select")))
	{
		return 0;
	}
	
	length=pin-linein;
	strncpy(lineout,linein,length);
	lineout[length]=0;
	if(!(ptmp=strstr(pin,"name")))
	{
		printf("&lt;select&gt : %s", "field name not found");
		exit(-1);
	}
	
	if(cgi_searchvalue_fromname(ptmp, "name",name)<0)
	{
		printf( "search value error\n");
		exit(-1);
	}
	
	pfind = GET_VALUE(l,  name);
	while((ptmp=strstr(pin,"<option"))!=NULL)
	{
		length=ptmp-pin;
		strncat(lineout,pin,length);
		pin+=length;
		ptmp=strchr(pin,'>');
		if(!ptmp)
		{
			printf("error in &lt;option&gt;\n");
			exit(-1);
		}
		
		length=ptmp-pin;
		strncpy(tag,pin,length);
		tag[length]=0;
		pin+=length+1;
		if(cgi_searchvalue_fromname(tag,"value", value)<0)
		{
			fputs("search value error\n",stdout);
			exit(-1);
		}
		
		if(pfind)
		{
			if(strcasecmp(pfind,value))
			{
				ptmp=strstr(tag,"selected");
				if(ptmp)
				{
					memmove(ptmp,ptmp+strlen("selected"),strlen(ptmp+strlen("selected"))+1);
				}
			}
			else
			{
				if(!strstr(tag,"selected"))
				{
					strcat(tag," selected");
				}
			}
		}
		else
		{
				ptmp=strstr(tag,"selected");
				if(ptmp)
				{
					memmove(ptmp,ptmp+strlen("selected"),strlen(ptmp+strlen("selected"))+1);
				}
		}
		length=strlen(tag);
		tag[length++]='>';
		tag[length]=0;
		strcat(lineout,tag);
	}
	strcat(lineout,pin);
	
	if(!strstr(pin,"</select"))
	{
		printf( "error: &lt;select&gt; : without &lt;/select&gt;<br>");
		exit(-1);
	}
	return 1;
}

/***************************************
 *name: cgi_html_parse_input()
 *func: to output value from l in  <input> of  a html file
 *return: 1 - <input> found and succeed in output, the lineout[] has the line to output
 *           0 - no <input>
 *notice: <input> must include in one line and may not write another <select> or <input>
 *           in the same line
 ***************************************/
int cgi_html_parse_input(char *linein,char *lineout, llist *l)
{
	char tag[2000], name[64],value[64],type[32],newvalue[64];
	char *pin,*ptmp,*pfind;
	int length;
	int find_flag=0;

	if(!(pin=strstr(linein,"<input")))
	{
		return 0;
	}
	
	lineout[0]=0;
	do
	{
		length=pin-linein;
		strncpy(tag,linein,length);
		tag[length]=0;
		strcat(lineout,tag);
		if(!(ptmp=strchr(pin,'>')))
		{
			printf("&lt;input without &gt;\n");
			exit(-1);
		}
		
		length=ptmp-pin;
		if(length>2000-1)
		{
			printf("&lt;input &gt; is too long\n");
			exit(-1);
		}
		
		strncpy(tag,pin,length);
		tag[length]=0;
		pin=ptmp;
		
		if(cgi_searchvalue_fromname(tag, "type",type)<0)
		{
			printf("search type error\n");
			exit(-1);
		}
		
		if(!strcasecmp(type,"text")||!strcasecmp(type,"password"))
		{
			if(cgi_searchvalue_fromname(tag, "name",name)<0)
			{
				printf("search name error\n");
				exit(-1);
			}
			
			if(cgi_searchvalue_fromname(tag,"value", value)<0)
			{
				fputs("search value error\n",stdout);
				exit(-1);
			}
			
			pfind= GET_VALUE(l, name);
			if(pfind)
			{
				if(strcasecmp(pfind,value))
				{
					sprintf(newvalue," value=%s ",pfind);
					ptmp=strstr(tag,"value=");
					*(ptmp-1)=0;
					strcat(lineout,tag);
					strcat(lineout,newvalue);
					find_flag=0;
					while(1)
					{
					if((ptmp=strchr(ptmp,'\"'))!=NULL)
					{
						find_flag++;
						ptmp+=1;
					}
					if(find_flag==2)
					{
					find_flag=0;
					break;
					}		
					}
					strcat(lineout,ptmp);
				}
				else
				{
					strcat(lineout,tag);
				}
			}
			else
			{
				strcat(lineout,tag);
			}
		}
		else if(!strcasecmp(type,"radio"))
		{
			if(cgi_searchvalue_fromname(tag, "name",name)<0)
			{
				printf("search name error\n");
				exit(-1);
			}
			
			if(cgi_searchvalue_fromname(tag,"value", value)<0)
			{
				fputs("search value error\n",stdout);
				exit(-1);
			}
			
			pfind = GET_VALUE(l, name);
			if(pfind)
			{
				if(strcasecmp(pfind,value))
				{
					ptmp=strstr(tag,"checked");
					if(ptmp)
					{
						strncpy(ptmp,"       ",7);
					}
				}
				else
				{
					if(!strstr(tag,"checked"))
					{
						strcat(tag," checked");
					}
				}
			}
			else
			{
					ptmp=strstr(tag,"checked");
					if(ptmp)
					{
						strncpy(ptmp,"       ",7);
					}
			}
			strcat(lineout,tag);
		}
		else if(!strcasecmp(type,"checkbox"))
		{
	 		if(cgi_searchvalue_fromname(tag, "name",name)<0)
			{
				printf("search name error\n");
				exit(-1);
			}
			
			if(cgi_searchvalue_fromname(tag,"value", value)<0)
			{
				fputs("search value error\n",stdout);
				exit(-1);
			}
			
			pfind = GET_VALUE(l, name);
			if(pfind)
			{
				if(!strstr(tag,"checked"))
				{
					strcat(tag," checked");
				}
				
			}
			else
			{
					ptmp=strstr(tag,"checked");
					if(ptmp)
					{
						strncpy(ptmp,"       ",7);
					}
			}
			strcat(lineout,tag);
		}
		else
		{
			strcat(lineout,tag);
			
		}
		linein=pin;
	}while((pin=strstr(linein,"<input")));
	
	strcat(lineout,linein);
	return 1;
}

int cgi_searchvalue_fromname(char *s,char *strname,char *strvalue)
{
	char *p=s,*q,*tail;
	char *charset="\"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int length;

	if(!p)
	{
		printf( "cgi_searchvalue_fromname(): null string\n");
		return -1;
	}
	p=strstr(s,strname);
	if(!p)
	{
		printf( "cgi_searchvalue_fromname(): no this name" );
		return -1;
	}
	
	p=strchr(p,'=');
	if(!p)
	{
		printf( "cgi_searchvalue_fromname(): not a name=value string\n" );
		return -1;
	}
	
	q=strpbrk(p,charset);
	if(!q)
	{
		printf( "cgi_searchvalue_fromname():  no value found\n") ;
		return -1;
	}
	if(*q=='\"')
	{
		tail=strchr(q+1,'\"');
		if(!tail)
		{
			printf( "cgi_searchvalue_fromname():value needs double \"");
			return -1;
		}
		length=tail-q-1;
		strncpy(strvalue,q+1,length);
	}
	else
	{
		length=strspn(q,charset);
		strncpy(strvalue,q,length);
	}
	*(strvalue+length)=0;
	return 0;
}

/* miscellaneous useful CGI routines */

#if WITH_MISC_HTML


//rigor 1: in rigor 0: not in rigor
node *cgi_locatename(llist *l, char *name,int rigor)
{
	short FOUND = 0;
	node* window;

	if(!l)return NULL;
	window = l->head;
	while ( (window != 0) && (!FOUND) )
	{
		if(rigor)
		{
			if (!strcmp(window->entry.name,name))FOUND = 1;
			else window = window->next;
		}
		else
		{
			if (strstr(window->entry.name,name))FOUND = 1;
			else window = window->next;
		}
	}
	
	if (FOUND)
		return window;
	else
		return NULL;
}

int read_file_upload(llist *entries, int maxfilesize)
{
    return parse_form_encoded(entries);
}


short accept_image()
{
	char *httpaccept = getenv("HTTP_ACCEPT");

	if (strstr(httpaccept,"image") == NULL)
		return 0;
	else
		return 1;
}

char **cgi_value_all(llist l)
{
    short FOUND = 0;
    node* window;
    char **ret_val = 0;
    int num_vals = 0, i;

    window = l.head;
    while (window != 0)
    {
        FOUND = 1;
        num_vals++;
        window = window->next;
    }
    if (FOUND)
    {
        /* copy the value pointers into the returned array */
        ret_val = (char**) malloc(sizeof(char*) * (num_vals + 1));
        window = l.head;
        i = 0;
        while (window != NULL)
        {
            ret_val[i] = window->entry.value;
            i++;
            window = window->next;
        }
        /* NULL terminate the array */
        ret_val[i] = 0;
        return ret_val;
    }
    else
        return NULL;
}

/* cgi_val_multi - contributed by Mitch Garnaat <garnaat@wrc.xerox.com>;
   modified by me */
char **cgi_value_multi(llist l, char *name)
{
    short FOUND = 0;
    node* window;
    char **ret_val = 0;
    int num_vals = 0, i;

    window = l.head;
    while (window != 0)
    {
        if (!strcmp(window->entry.name,name))
        {
            FOUND = 1;
            num_vals++;
        }
        window = window->next;
    }
    if (FOUND)
    {
        /* copy the value pointers into the returned array */
        ret_val = (char**) malloc(sizeof(char*) * (num_vals + 1));
        window = l.head;
        i = 0;
        while (window != NULL)
        {
            if (!strcmp(window->entry.name,name))
            {
                ret_val[i] = window->entry.value;
                i++;
            }
            window = window->next;
        }
        /* NULL terminate the array */
        ret_val[i] = 0;
        return ret_val;
    }
    else
        return NULL;
}

/* return the value for this name */
char *cgi_value(llist l, char *name)
{
	short FOUND = 0;
	node* window;

	window = l.head;
	while ( (window != 0) && (!FOUND) )
	{
		if (!strcmp(window->entry.name,name))
			FOUND = 1;
		else
			window = window->next;
	}
	
	if (FOUND)
		return window->entry.value;
	else
		return NULL;
}

/* get name in name/value key pair*/
char *cgi_name(llist l, char *value)
{
	short FOUND = 0;
	node* window;

	window = l.head;
	while ( (window != 0) && (!FOUND) )
	{
		if (!strcmp(window->entry.value,value))
			FOUND = 1;
		else
			window = window->next;
	}	

	if (FOUND)
		return window->entry.name;
	else
		return NULL;
}

char **cgi_name_multi(llist l, char *value)
{
    short FOUND = 0;
    node* window;
    char **ret_val = 0;
    int num_vals = 0, i;

    window = l.head;
    while (window != 0)
    {
        if (!strcmp(window->entry.value,value))
        {
            FOUND = 1;
            num_vals++;
        }
        window = window->next;
    }
    if (FOUND)
    {
        /* copy the value pointers into the returned array */
        ret_val = (char**) malloc(sizeof(char*) * (num_vals + 1));
        window = l.head;
        i = 0;
        while (window != NULL)
        {
            if (!strcmp(window->entry.value,value))
            {
                ret_val[i] = window->entry.name;
                i++;
            }
            window = window->next;
        }
        /* NULL terminate the array */
        ret_val[i] = 0;
        return ret_val;
    }
    else
        return NULL;
}


int parse_cookies(llist *entries)
{
    char *cookies = getenv("HTTP_COOKIE");
    node* window;
    entrytype entry;
    int i,len;
    int j = 0;
    int numcookies = 0;
    short NM = 1;

    if (cookies == NULL)
        return 0;
    list_create(entries);
    window = entries->head;
    len = strlen(cookies);
    entry.name = (char *)malloc(sizeof(char) * len + 1);
    entry.value = (char *)malloc(sizeof(char) * len + 1);
    for (i = 0; i < len; i++)
    {
        if (cookies[i] == '=')
        {
            entry.name[j] = '\0';
            if (i == len - 1)
            {
                strcpy(entry.value,"");
                window = list_insafter(entries,window,entry);
                numcookies++;
            }
            j = 0;
            NM = 0;
        }
        else if ( (cookies[i] == '&') || (i == len - 1) )
        {
            if (!NM)
            {
                if (i == len - 1)
                {
                    entry.value[j] = cookies[i];
                    j++;
                }
                entry.value[j] = '\0';
                window = list_insafter(entries,window,entry);
                numcookies++;
                j = 0;
                NM = 1;
            }
        }
        else if ( (cookies[i] == ';') || (i == len - 1) )
        {
            if (!NM)
            {
                if (i == len - 1)
                {
                    entry.value[j] = cookies[i];
                    j++;
                }
                entry.value[j] = '\0';
                window = list_insafter(entries,window,entry);
                numcookies++;
                i++;   /* erases trailing space */
                j = 0;
                NM = 1;
            }
        }
        else if (NM)
        {
            entry.name[j] = cookies[i];
            j++;
        }
        else if (!NM)
        {
            entry.value[j] = cookies[i];
            j++;
        }
    }
    return numcookies;
}

void print_cgi_env()
{
	if (SERVER_SOFTWARE != NULL)
		printf("<p>SERVER_SOFTWARE = %s<br>\n",SERVER_SOFTWARE);
	if (SERVER_NAME != NULL)
		printf("SERVER_NAME = %s<br>\n",SERVER_NAME);
	if (GATEWAY_INTERFACE !=NULL)
		printf("GATEWAY_INTERFACE = %s<br>\n",GATEWAY_INTERFACE);

	if (SERVER_PROTOCOL != NULL)
		printf("SERVER_PROTOCOL = %s<br>\n",SERVER_PROTOCOL);
	if (SERVER_PORT != NULL)
		printf("SERVER_PORT = %s<br>\n",SERVER_PORT);
	if (REQUEST_METHOD != NULL)
		printf("REQUEST_METHOD = %s<br>\n",REQUEST_METHOD);
	if (PATH_INFO != NULL)
		printf("PATH_INFO = %s<br>\n",PATH_INFO);
	if (PATH_TRANSLATED != NULL)
		printf("PATH_TRANSLATED = %s<br>\n",PATH_TRANSLATED);
	if (SCRIPT_NAME != NULL)
		printf("SCRIPT_NAME = %s<br>\n",SCRIPT_NAME);
	if (QUERY_STRING != NULL)
		printf("QUERY_STRING = %s<br>\n",QUERY_STRING);
	if (REMOTE_HOST != NULL)
		printf("REMOTE_HOST = %s<br>\n",REMOTE_HOST);
	if (REMOTE_ADDR != NULL)
		printf("REMOTE_ADDR = %s<br>\n",REMOTE_ADDR);
	if (AUTH_TYPE != NULL)
		printf("AUTH_TYPE = %s<br>\n",AUTH_TYPE);
	if (REMOTE_USER != NULL)
		printf("REMOTE_USER = %s<br>\n",REMOTE_USER);
	if (REMOTE_IDENT != NULL)
		printf("REMOTE_IDENT = %s<br>\n",REMOTE_IDENT);

	if (CONTENT_TYPE != NULL)
		printf("CONTENT_TYPE = %s<br>\n",CONTENT_TYPE);
	if (CONTENT_LENGTH != NULL)
		printf("CONTENT_LENGTH = %s<br></p>\n",CONTENT_LENGTH);

	if (HTTP_USER_AGENT != NULL)
		printf("HTTP_USER_AGENT = %s<br></p>\n",HTTP_USER_AGENT);
}
#endif


#if DEBUG

//one pointer(with cgi_replace_ltgt()) not free,fixed.
void print_entries(llist l)
{
	node *window;
	char *pstr;

	window = l.head;
	cgidebug("<dl>\n");
	while (window != NULL)
	{
		cgidebug("  <dt> <b>%s</b>\n",window->entry.name);
		pstr = cgi_replace_ltgt(window->entry.value);
		cgidebug("  <dd> %s\n", pstr);
		free(pstr);
		window = window->next;
	}
	cgidebug("</dl>\n");
}
#endif

void kill_process(char * process_name)
{
	pid_t pid=-1;
	char cmd[128]="\0";
	while(1)
	{
		pid=cgi_get_pid_by_name(process_name);
		if(pid<0)
			break;
		sprintf(cmd,"kill -9 %d",pid);
		system(cmd);
		pid=-1;
	}
	return ;
}

void int_to_str(unsigned short d,char * str)
{
	int i,j;
	char tmp;
	memset(str,0,strlen(str));
	for(i=0;d&&( i<sizeof(str) );i++)
	{	
		str[i]=d%10+'0';
		d=d/10;
	}

	for(i=0,j=strlen(str)-1;	i<j;i++,j--)
	{
		tmp=str[i];
		str[i]=str[j];
		str[j]=tmp;
	}
}


/* return 0 : not validate IP address */
int isValidateIpAddress(char *ipAddrStr)
{
	struct in_addr ip;
	
	if(!inet_aton(ipAddrStr, &ip) )
	{
		cgidebug( "Error : %s is not valadate IP Address\n", ipAddrStr );
		return 0;
	}

	return 1;
}

int	cgi_check_pci_device(char *key, char *pciDeviceId)
{
#define	PCI_DEVICE_FILE	"/proc/pci"
#define	TMP_PCI_DEVICE		"/tmp/pci"
	char		cmd[128];
	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	int		ret = 0;

	sprintf(cmd, "cat %s |grep %s > %s", PCI_DEVICE_FILE, key, TMP_PCI_DEVICE);
	system(cmd);

	if((fp=fopen(TMP_PCI_DEVICE, "r"))==NULL)
	{
		perror( "Open config file failure!" );
		return ret;
	}

	while (fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		if( strstr(line, pciDeviceId ) )
		{
			ret =1 ;
		}
	}

	sprintf(cmd, "rm -rf %s", TMP_PCI_DEVICE );
	system(cmd);
	
	fclose(fp);

	return ret;
}


int	cgi_check_usb_device(char *vendor, char *usbDeviceId)
{
#define	USB_DEVICE_FILE		"/proc/bus/usb/devices"
#define	TMP_USB_DEVICE		"/tmp/usb"
	char		cmd[128];
	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	int		ret = 0;

	sprintf(cmd, "cat %s |grep %s > %s", USB_DEVICE_FILE, vendor, TMP_USB_DEVICE);
	system(cmd);

	if((fp=fopen(TMP_USB_DEVICE, "r"))==NULL)
	{
		perror( "Open config file failure!" );
		return ret;
	}

	while (fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		if( strstr(line, usbDeviceId ) )
		{
			ret =1 ;
		}
	}

	sprintf(cmd, "rm -rf %s", TMP_USB_DEVICE );
	system(cmd);
	
	fclose(fp);

	return ret;
}

int	cgi_check_usb_driver(char *driverName)
{
#define	USB_DRIVERS_FILE		"/proc/bus/usb/drivers"
#define	TMP_USB_DEVICE		"/tmp/usb"
	char		cmd[128];
	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	int		ret = 0;

	sprintf(cmd, "cat %s |grep %s > %s", USB_DRIVERS_FILE, driverName, TMP_USB_DEVICE);
	system(cmd);

	if((fp=fopen(TMP_USB_DEVICE, "r"))==NULL)
	{
		perror( "Open config file failure!" );
		return ret;
	}

	while (fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		if( strstr(line, driverName ) )
		{
			ret =1 ;
		}
	}

	sprintf(cmd, "rm -rf %s", TMP_USB_DEVICE );
	system(cmd);
	
	fclose(fp);

	return ret;
}

void _close_locale_fp(void )
{
	fclose(localeFp);
}

char *gettext(char *text)
{
	char fname[128];
//	FILE *fp;
	char line[LINE_MAX_LENGTH];
	char *retptr,*p,*q;

	if( !LOCALE|| strlen(LOCALE)==0 )
	{/* locale is 'C' or null */
		return text;
	}

	if(localeFp== NULL)
	{
		sprintf(fname,"%s/locale/%s_%s.po",ROOT_PATH_ADMIN,PACKAGE,LOCALE);
		localeFp = fopen(fname, "r" );
		if(!localeFp )
		{
//			cgidebug("gettext : '%s', because of '%s'\n" ,fname, strerror(errno) );
			return text;
//			exit(-1);
		}
		atexit( _close_locale_fp );
	}

	rewind(localeFp);
	while((retptr=fgets(line,LINE_MAX_LENGTH, localeFp) )!=NULL)
	{
		if(strlen(line)<1)
			continue;
		if(line[0]=='#'||line[0]=='\n'||line[0]=='\r')
			continue;
		if(!strstr(line,"msgid"))
			continue;
		p=strchr(line,'\"')+1;
		q=strrchr(line,'\"');
		if(!p||!q)
		{
			printf("locale file: double quotation expected\n");
			exit(-1);
		}
		*q=0;
		if(!strcmp(p,text))
		{
			retptr=fgets(line,LINE_MAX_LENGTH, localeFp );
			break;
		}
	}
	
//	fclose(fp);
	if(!retptr)
	{
//		cgidebug("%s is not found\n", text);
//		return TEXT_NOT_FOUND;
		return strdup(text);
	}

	if(!strstr(line,"msgstr"))
	{
		printf("locale file: no msgstr of text(%s) found\n",text);
		exit(-1);
	}
	p=strchr(line,'\"');
	q=strrchr(line,'\"');
	if(!p||!q)
	{
		printf("locale file: double quotation expected\n");
		exit(-1);
	}
	p++;
	*q=0;
	if(strlen(p))
		return strdup(p);
	else
	{
//		return TEXT_NOT_FOUND;
//		cgidebug("%s is not found 2\n", text);
		return strdup(text);
	}

#if 0
	while((retptr=fgets(line,LINE_MAX_LENGTH,fp))!=NULL)
	{
		if(strlen(line)<1)continue;
		if(line[0]=='#'||line[0]=='\n'||line[0]=='\r')continue;
		if(strstr(line,text))break;
	}
	fclose(fp);
	if(!retptr){return TEXT_NOT_FOUND;}

	p=strtok(line,"=");
	p=strtok(NULL,"=");
	if(p)return strdup(p);
	else return TEXT_NOT_FOUND;
#endif

}

int cgi_nic_refresh_net_and_broad_address(llist *nicCfgs)
{
	struct in_addr ip,mask,network,broadcast;
	char *p,*q;
	
	p = GET_VALUE(nicCfgs, NIC_KEYWORD_IP_ADDRESS);
	q = GET_VALUE(nicCfgs, NIC_KEYWORD_NETMASK);
	if(!inet_aton(p, &ip) )
	{
		cgi_error_page(gettext(CGI_STR_ERROR), gettext("Network Interface Configuration"), gettext("is not a validate IP address") );
		return 1;
	}
	if(!inet_aton(q, &mask) )
	{
		cgi_error_page(gettext(CGI_STR_ERROR), gettext("Network Interface Configuration"), gettext("is not a validate IP address") );
		return 1;
	}

	network.s_addr = ip.s_addr&mask.s_addr;
	broadcast.s_addr = ip.s_addr|~mask.s_addr;

	list_replace_value_by_key(nicCfgs, NIC_KEYWORD_BROADCAST, inet_ntoa(broadcast) );
	list_replace_value_by_key(nicCfgs, NIC_KEYWORD_NET_ADDRESS, inet_ntoa( network) );

	return 0;	
}

int	cgi_get_files_in_dir(char *dirName, llist *filesList)
{
	struct dirent	**namelist;
	struct stat 	fstats;
	char			filename[256];
	int 			n, i = 0;
	char			value[128];
	
	n = scandir( dirName, &namelist, NULL, alphasort);
	while(n--)
	{
		if( !strcasecmp(namelist[n]->d_name, ".") ||!strcasecmp(namelist[n]->d_name, "..") )
			continue;
		sprintf(filename, "%s/%s", dirName, namelist[n]->d_name );
		stat(filename, &fstats);

		sprintf(value, "%ld", fstats.st_size );
		list_append_raw(filesList, namelist[n]->d_name, value, NULL, NULL);
		
		free(namelist[n]);
		i++;
	}
		
	free(namelist);

	return i;
}

