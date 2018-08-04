/*
* $Author: lizhijie $
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "cgi_lib.h"

char *cgi_new_string(char *str)
{
	int length = ITEM_LENGTH +1;
	if( str &&  (strlen(str)>ITEM_LENGTH) )
		length = strlen(str);
	
	char *tempstr = (char *)malloc(sizeof(char) * length );

	if (tempstr != NULL )
	{
		if( str)
			strcpy(tempstr,str);
		else
			memset(tempstr, 0, length);
	}	
	
	return tempstr;
}

#if WITH_MISC_HTML
char *cgi_sub_string(char *str, int offset, int len)
{
	int slen, start, i;
	char *nstr;

	if (str == NULL)
		return NULL;
	else
		slen = strlen(str);

	nstr = (char *)malloc(sizeof(char) * slen + 1);
	if (offset >= 0)
		start = offset;
	else
		start = slen + offset - 1;

	if ( (start < 0) || (start > slen) ) /* invalid offset */
		return NULL;

	for (i = start; i < start+len; i++)
		nstr[i - start] = str[i];

	nstr[len] = '\0';
	return nstr;
}
#endif

char *cgi_replace_ltgt(char *str)
{
	unsigned int i,j = 0;
	char *newstring;

	if (str == NULL)
		return NULL;
	newstring = (char *)malloc(sizeof(char) * (strlen(str) * 4 + 1));
	for (i = 0; i < strlen(str); i++) 
	{
		if (str[i] == '<') 
		{
			newstring[j] = '&';
			newstring[j+1] = 'l';
			newstring[j+2] = 't';
			newstring[j+3] = ';';
			j += 3;
		}
		else if (str[i] == '>') 
		{
			newstring[j] = '&';
			newstring[j+1] = 'g';
			newstring[j+2] = 't';
			newstring[j+3] = ';';
		j += 3;
	}
	else
		newstring[j] = str[i];

	j++;
	}
	newstring[j] = '\0';

	return newstring;
}

char *cgi_lower_case(char *buffer)
{
	char *tempstr = buffer;

	while (*buffer != '\0') 
	{
		if (isupper(*buffer))
			*buffer = tolower(*buffer);
		buffer++;
	}
	return tempstr;
}
