/*
* $Id: cgi_cfg_file.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_cfg_file.h"

int cgi_write_config_item_with_collon(char *key, char *value,  FILE*out)
{
	char line[PARSE_FILE_MAX_LENGTH];


	sprintf(line, "%s:\t%s\n",key, value);
	fputs(line,out);
	return 0;
}

/* return number of items wrote to the file */
int cgi_write_config(FILE *fp, llist *rawList, llist *modifiedList, int(*write_special_fields)(char *, char *, FILE*))
{
	node 	*currentRaw, *currentModify = NULL;
	char		*key, *value;
	char 	line[PARSE_FILE_MAX_LENGTH];
	int		count = 0;

	if(!fp || !rawList )//|| | !write_special_fields)//!modifiedList |)
	{
		cgidebug("Paramteter is NULL\n");
		return 0;
	}	
	
	currentRaw = rawList->head;
	
	while (currentRaw != 0) 
	{
		key = currentRaw->entry.name;
		value = currentRaw->entry.value;
		if(modifiedList)
		{
			currentModify = modifiedList->head;
			while(currentModify )
			{
				if(!strcasecmp(key, currentModify->entry.name ) )
				{
					value = currentModify->entry.value;
				}
				currentModify = currentModify->next;
			}
		}

		if(write_special_fields)
		{
			(write_special_fields)(key , value, fp);
		}	
		else
		{
			sprintf(line, "%s\t\t%s\n",key, value);
			fputs(line, fp);
		}

		count++;
		currentRaw = currentRaw->next;
	}

	return count;
}


