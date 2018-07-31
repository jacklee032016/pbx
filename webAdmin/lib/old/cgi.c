/*	CGI library functions
 * This file includes all the neccessary functions
 * for parsing "FORM INPUT" from the web page.
 * 
 * The library is a simplified copy of libcgi-1.0,
 * if you have demands for other library functions,
 * a library function, please mail me and I will 
 * do my best to help you.
 *
 * Miranda<minlan_wang@cdmail.sonix.com.cn>
 * Mon Aug 20 13:33:06 CST 2007
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "cgi.h"

#define E_WARNING	0
#define E_FATAL		1
#define E_CAUTION 	2
#define E_INFORMATION 	3
#define E_MEMORY	4


static int hextable[256];
const char *libcgi_error_type[] = {
	"LibCGI Warning",
	"LibCGI Fatal",
	"LibCGI Caution",
	"LibCGI Information",
	"LibCGI out of memory"
};


int headers_initialized = 0;
formvars *formvars_start = NULL;
formvars *formvars_last = NULL;

/*******************Main cgi functions*****************************/

// Original idea from cgic library
void init_hex_table()
{
	memset(hextable, 0, 255);

	hextable['1'] = 1;
	hextable['2'] = 2;
	hextable['3'] = 3;
	hextable['4'] = 4;
	hextable['5'] = 5;
	hextable['6'] = 6;
	hextable['7'] = 7;
	hextable['8'] = 8;
	hextable['9'] = 9;
	hextable['a'] = 10;
	hextable['b'] = 11;
	hextable['c'] = 12;
	hextable['d'] = 13;
	hextable['e'] = 13;
	hextable['f'] = 15;
	hextable['A'] = 10;
	hextable['B'] = 11;
	hextable['C'] = 12;
	hextable['D'] = 13;
	hextable['E'] = 14;
	hextable['F'] = 15;
}

/**
*  Main cgi function. 
*  Configures all (most?) we need to  get cgi library working correctly. It MUST be called before
*  any other cgi function.
*  @see cgi_end, cgi_process_form, cgi_init_headers
**/
int cgi_init()
{
	init_hex_table();
	return 1;
}

void cgi_end()
{
	slist_free(&formvars_start);

	formvars_last = NULL;
}

/**
* Initialize HTML headers.
* You need to call this function before that any content is send to the brosert, otherwise you'll get an error (Error 500).
* @see cgi_init
**/
void cgi_init_headers()
{
	if (!headers_initialized) {
		printf("Content-type: text/html\r\n\r\n");
		headers_initialized = 1;
	}
}

/**
* Include static files.
* Function used to include static data ( normaly html files ).
* File contents will never be processed.
* Note that I don't scan for any special character. The reason
* I did it is, if the you are using this library, you have a shell
* where you can compile the cgi program. And can do much more ;-)
* @param filename Filename with full path to include
* @return If an error occurs and libcgi_debug is true, then a warning
* message is showed.
* @see libcgi_debug
*
* \code
* cgi_include("top_bar.htm");
* \endcode
*/
int cgi_include(const char *filename)
{
	FILE *inc;
	char buffer[255];

	if (!(inc = fopen(filename, "r"))) {
		cgi_init_headers();

		libcgi_error(E_WARNING, "Failed to open include file <b>%s</b>", filename);

		return 0;
	}
	
 	while (fgets(buffer, 255, inc))
		printf("%s", buffer);

	fclose(inc);

	return 1;
}

/*******************List processing functions*****************************/
/* Add a new item to the list */
void slist_add(formvars *item, formvars **start, formvars **last)
{
	// if *start is empty, then our list is also empty. So, the only
	// task to do is fill the variables *start and *last with the data pointed by item
	// *start contains the firts item in the list, and we need *last to know the 
	// end of the list
	if (!*start) {
		// item->next needs to be NULL, otherwise we never will 
		// find the end of list
		item->next = NULL;

		*start = item;
		*last = item;

		return;
	}

	// Ok, in this case, the next item will contain the
	// new data. Note that, before it, (*last)->next contains
	// NULL ( the end of the list )
	(*last)->next = item;

	// The 'next' argument of item variable need to contain
	// NULL, or in other words, it will be the end ;0
	item->next = NULL;

	// Finaly, the actual end of list will contain the new item
	*last = item;
}

// Free linked list allocated memory
void slist_free(formvars **start)
{
	while (*start) {
		void *p = *start;
		free((*start)->name);
		free((*start)->value);
		if ((*start)->file_name)
			free((*start)->file_name);

		*start = (*start)->next;

		free(p);
	}

	*start = NULL;
}

// Returns the value of the item pointed by name
char *slist_item(const char *name, formvars *start)
{
	formvars *begin;
	begin = start;

	while (begin) {
		if (!strcasecmp(begin->name, name)) 
			return (!begin->value[0] ? NULL : begin->value);

		begin = begin->next;
	}

	return NULL;
}

/*******************Form processing functions*****************************/

/**
* Transforms' URL special chars.
* Search for special chars ( like %%E1 ) in str, converting them to the ascii character correspondent.
* @param str String containing data to parse
* @return The new string
* @see cgi_escape_special_chars
**/
char *cgi_unescape_special_chars(char *str)
{
	char *tmp;
	register int i, len, pos = 0;

	len = strlen(str);
	tmp = (char *)malloc(len + 1);
	if (tmp == NULL)
		libcgi_error(E_MEMORY, "%s, line %s", __FILE__, __LINE__);

	for (i = 0; i < len; i++) {
		// If we found a '%' character, then the next two are the character
		// hexa code. Converting a hexadecimal code to their decimal is easy:
		// The first character needs to be multiplied by 16 ( << 4 ), and the another
		// one we just get the value from hextable variable
		if ((str[i] == '%') && isalnum(str[i+1]) && isalnum(str[i+2])) {
			tmp[pos] = (hextable[(unsigned char) str[i+1]] << 4) + hextable[(unsigned char) str[i+2]];
			i += 2;
		}
		else if (str[i] == '+')
			tmp[pos] = ' ';
		else
			tmp[pos] = str[i];
		
		pos++;
	}

	tmp[pos] = '\0';

	return tmp;
}

/* Separates *query in name=value pairs, then gets each piece of result of them, storing
 * the result in the linked list global variable
 */
formvars *process_data(char *query, formvars **start, formvars **last, const char delim, const char sep)
{
	register size_t position = 0, total_len = 0, i = 0;
	char *aux;
	formvars *data;

	if (query == NULL)
		return *start;

	total_len = strlen(query);
	aux = query;
	while (*query) {
		position = 0;
				
		data = (formvars *)malloc(sizeof(formvars));
		if (!data)
			libcgi_error(E_MEMORY, "%s, line %s", __FILE__, __LINE__);
			
		memset(data, 0, sizeof(formvars));							
		
		// Scans the string for the next 'delim' character
		while (*aux && (*aux != delim)) {
			position++;
			aux++;
			i++;
		}

		if (*aux) {
			aux++;
			i++;
		}

		data->name = (char *)malloc(position+1);
		if (data->name == NULL)
			libcgi_error(E_MEMORY, "%s, line %s", __FILE__, __LINE__);

		strncpy(data->name, query, position);
		data->name[position] = '\0';
		
		query = aux;
		position = 0;
		while (*aux && (*aux != sep)) {
			if ((*aux == '%') && (i + 2 <= total_len)) {
				if (isalnum(aux[1]) && isalnum(aux[2])) {
					aux += 2;
					i += 2;
					position++;
				}
			}
			else			
				position++;
				
			aux++;
			i++;
		}
				
		if (*aux) {
			aux++;
			i++;
		}
		
		data->value = (char *)malloc(position+1);
		if (data->value == NULL)
			libcgi_error(E_MEMORY, "%s, line %s", __FILE__, __LINE__);

		strncpy(data->value, cgi_unescape_special_chars(query), position);
		data->value[position] = '\0';
		
		slist_add(data, start, last);
		
		query = aux;
	}

	return *start;
}

int parse_pair(char **line, formvars *item)
{
	char *start;
	char *end;
	char *delim;
	char *pos;

	char *name;
	char *value;

	start = strchr(*line, ';');			//start point to pair delimeter ';', skip some header
	end = *line + strlen(*line) - 1;		//end point to then end of this line

	if (!start)
		return 0;

	start ++;					//Skip ';'
	end --;						/*Don't point to the '\n' at the end of the line, 
							instead point to '/r' */

	while (1) {
		if (*start == ';')
			start ++;
		while (*start == ' ')
			start ++;
		if (start == end)
			break;

//		printf("<p>start:%s</p>\n", start);
		delim = strchr(start, '=');
		if (!delim)
			return 0;
		*delim = '\0';
		name = start;

		pos = delim + 1;
		if (*pos == '"')
			delim += 1;
		while (*pos != ';') {
			if (pos == end)
				break;
			pos ++;
		}
		if (*(pos-1) == '"')
			pos -= 1;
		*pos = '\0';
		value = delim + 1;

		if (!strcmp(name, "name")) {
//			printf("<p>name:%s</p>\n", value);
			item->name = malloc(strlen(value) + 1);
			if (!item->name) {
				perror("malloc");
				return 0;
			}
			strcpy(item->name, value);
		} else if (!strcmp(name, "filename")) {
//			printf("<p>filename:%s</p>\n", value);
			item->file_name = malloc(strlen(value) + 1);
			if (!item->file_name) {
				perror("malloc");
				return 0;
			}
			strcpy(item->file_name, value);
		}
		
		start = pos + 1;
	}

	return 0;
}

int parse_content(formvars *item, char *bound)
{
	char *line = NULL;
	size_t size;

	size_t value_length;
	size_t len;

	value_length = 0;
	item->value = NULL;
	item->file_length = 0;
	while (1) {
		if ((len = cgi_getline(&line, &size, stdin)) < 0) {
			break;
		}
		if ((!strncmp(line, "--", 2)) && 
				(!strncmp(line + 2, bound, strlen(bound)))) {
//			printf("<p> Gotta break, next boundary appears</p>\n");
			break;
		}

//		printf("<p>Content:%s, length:%d<p>\n", line, len);
		value_length += len;

		item->value = realloc(item->value, value_length);
		if (!item->value)
			break;

		memcpy(item->value + value_length - len, line, len);
	}

	if (item->value)
		*(item->value + value_length - 2) = '\0'; 

	item->file_length = value_length - 2;

	if (line)
		free(line);

	return 0;
}

formvars *process_multi_data(char *bound, formvars **start, formvars **last)
{
	char *line = NULL;
	int size;
	formvars *current;

	if (cgi_getline(&line, &size, stdin) < 0)
		return NULL;
	if (strncmp(line, "--", 2) && 
			strncmp(line + 2, bound, strlen(bound)))
		return NULL;

	while (1) {
		if (cgi_getline(&line, &size, stdin) < 0)
			break;

		current = malloc(sizeof(formvars));
		memset(current, 0, sizeof(formvars));

		parse_pair(&line, current);

		while (cgi_getline(&line, &size, stdin) > 0) {
			if (!strcmp(line, "\r\n"))
				break;
		}
		parse_content(current, bound);
		slist_add(current, start, last);
	}

	if (line)
		free(line);
	return *start;
}

/*
* Process HTML form or URL data.
* Used to retrieve GET or POST data. It handles automaticaly the correct REQUEST_METHOD, so you don't need to afraid about it.
* @return Returns the contents of URL or FORM into a formvars variable, or NULL if FALSE. Most of time, you 
* don't need any variable to store the form data, because is used an internal variable to manipulate the contents. 
* @see cgi_init, cgi_init_headers
*/
formvars *cgi_process_form()
{
	char *method;
	char *post_data;
	char *tmp_data;
	int content_length;
	char *content_type;
	char *boundary;

	method = getenv("REQUEST_METHOD");
	if (strcasecmp("POST", method)) 
		return NULL;

	content_type = getenv("CONTENT_TYPE");
	if (!strncmp(content_type, "application/x-www-form-urlencoded", 33)) {

		tmp_data = getenv("CONTENT_LENGTH");
		if (tmp_data == NULL)
			return NULL;

		content_length = atoi(tmp_data);

		post_data = (char *)malloc(content_length + 1);
		if (post_data == NULL)
			libcgi_error(E_MEMORY, "%s, line %s", __FILE__, __LINE__);

		fread(post_data, content_length, 1, stdin);
		post_data[content_length] = '\0';

		return process_data(post_data, &formvars_start, &formvars_last, '=', '&');
	} else if (!strncmp(content_type, "multipart/form-data", 19)) {
		boundary = content_type + 19 + 2 + 9;
		return process_multi_data(boundary, &formvars_start, &formvars_last);
	}

	return NULL;
}

char *cgi_param(const char *var_name)
{
	return slist_item(var_name, formvars_start);
}

size_t cgi_param_length(const char *var_name)
{
	formvars *begin;
	begin = formvars_start;

	while (begin) {
		if (!strcasecmp(begin->name, var_name)) 
			return (!begin->file_name[0] ? 0: begin->file_length);

		begin = begin->next;
	}

	return 0;
}

void libcgi_error(int error_code, const char *msg, ...)
{
	va_list arguments;

	cgi_init_headers();
	va_start(arguments, msg);	

	printf("<b>%s</b>: ", libcgi_error_type[error_code]);
	vprintf(msg, arguments);
	puts("<br>");

	va_end(arguments);

	if ((error_code == E_FATAL) || (error_code == E_MEMORY)) {
		cgi_end();

		exit(EXIT_FAILURE);
	}
}

/**
* Kills the application with a message.
* Writes msg and terminate
* @param msg Message to send to the browser before killing
*/
void cgi_fatal(const char *msg)
{
	cgi_init_headers();

	puts(msg);

	cgi_end();
	exit(1);
}

/* 
 * Check User's privilege for cgi_programs
 * Added by Miranda
 * Mon Sep  3 20:47:36 CST 2007
 */
void check_prio(int priority)
{
	int authored;
	authored = atoi(getenv("AUTHORED"));
	if (authored < priority)
		cgi_fatal("You have no privilege to view this page, please clear your authentication information and re-log!");
	return;
}

/*
 * Find which language is selected by user
 * Added by Miranda
 * Wed Sep 26 11:18:14 CST 2007
 */

int cgi_find_lan()
{
	char *lan;
	lan = getenv("QUERY_STRING");
	if (!lan) {
		printf("No language selected!\n");
		return -1;
	}
	if (strncmp(lan, "lan=", 4))
		cgi_fatal("Illegal query string: not indicate language here!");
	lan += 4;
	if (!strcmp(lan, "EN"))
		return EN;
	else if (!strcmp(lan, "CH"))
		return CH;
	else if (!strcmp(lan, "ZH"))
		return ZH;
	else {
		printf("No support for this language");
		return -1;
	}
}
