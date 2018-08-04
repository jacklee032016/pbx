/*
* $Id$
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "cgiLib.h"

FILE		*localeFp = NULL;

void _close_locale_fp(void )
{
	fclose(localeFp);
	localeFp = NULL;
}

char *gettext(char *text)
{
	char fname[128];
//	FILE *fp;
	char line[LINE_MAX_LENGTH];
	char *retptr,*p,*q;
	char	*localeName= NULL;

	if(!HTTP_ACCEPT_LANGUAGE || strlen(HTTP_ACCEPT_LANGUAGE)==0)
	{
		if( !LOCALE|| strlen(LOCALE)==0 )
		{/* locale is 'C' or null */
			return text;
		}
		else
			localeName = LOCALE;
	}
	else
	{
		localeName = HTTP_ACCEPT_LANGUAGE;
	}

//	cgidebug("LOCALE NAME = %s\n", localeName );
//	cgidebug("HTTP_ACCEPT_LANGUAGE = %s\n", HTTP_ACCEPT_LANGUAGE );
	
	if(localeFp== NULL)
	{
		sprintf(fname,"%s/locale/%s_%s.po",ROOT_PATH_ADMIN,PACKAGE, localeName );
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
		return strdup(text);
	}

}

