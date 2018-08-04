/*
 * $Id: as_parse_config.c,v 1.1 2007/03/16 19:30:50 lizhijie Exp $
*/
#include  <stdio.h>

#define TUPLE3_MAX_LENGTH 				1024
#define TAG_MAX_LENGTH 				96
#define TYPE_MAX_LENGTH 				32
#define VALUE_MAX_LENGTH 				TUPLE3_MAX_LENGTH - TAG_MAX_LENGTH - TYPE_MAX_LENGTH


void removeTrailingBlanks( char* line )
{
	for ( int i = strlen( line ) - 1; i > 0; i-- )
	{
		if ( line[i] == ' ' || line[i] == '\t' )
		{
			line[i] = '\0';
		}
		else
		{
			return ;
		}
	}
}


int  parse3tupleFile (char* fname, void (*parse) (char*, char*, char*) )
{
	char line [TUPLE3_MAX_LENGTH];
	char tag [TAG_MAX_LENGTH];
	char type [TYPE_MAX_LENGTH];
	int lineCnt;
	int lineLen = 0;
	int argCnt;
	int valuePos = 0;

	FILE* fd = fopen (fname , "r");

	if (fd)
	{
		for (lineCnt = 1; fgets (line, TUPLE3_MAX_LENGTH, fd); lineCnt++, valuePos = 0)
		{
			lineLen = strlen(line);
			assert( lineLen > 0 );
			line [lineLen - 1] = 0;    /* remove \n */
			if (lineLen > 1)
			{
				if (line [lineLen - 2] == '\r')
				{
					line [lineLen - 2] = 0;    /* remove \r too */
				}
			}

			removeTrailingBlanks( line );

			if ((argCnt = sscanf (line, "%s%s%*[ \t]%n", tag, type, &valuePos)) == 2)
			{
				if (line[0] != '#')
				{
					if (valuePos == 0)
					{
						cpLog (LOG_WARNING, "Line %d has too few arguments: %s", lineCnt, line);
					}
					else
					{
						(*parse) (tag, type, line + valuePos);
					}
				}
			}
			else
			{
				if (argCnt == 1 && line[0] != '#')
				{
					cpLog (LOG_WARNING, "Line %d has too few arguments: %s", lineCnt, line);
				}
			}
		}
		fclose (fd);
	}
	else
	{
		cpLog (LOG_ERR, "Cannot open file: %s", fname);
		return false;
	}
	return true;
}    /* parse3tuple */


int   strConst2i (const char* str, const char* strTable[], const int tableSize)
{
	int i;

	for (i = 0; i < tableSize; i++)
	{
		if (strcmp (str, strTable [i]) == 0)
		{
			break;
		}
	}

	return i;
}    // strConst2i

