#ifndef CGI_H
#define CGI_H

// general purpose linked list. Actualy isn't very portable
// because uses only 'name' and 'value' variables to store data.
// Problably, in a future release, this will be replaced by 
// another type of struct
typedef struct formvarsA {
        char *name;
        char *value;
	char *file_name;
	size_t file_length;
        struct formvarsA *next;
} formvars;

enum LAN {
	EN,
	CH,
	ZH	
};

int cgi_init();
void cgi_end();

void cgi_init_headers();
int cgi_include(const char *filename);

void slist_add(formvars *item, formvars **start, formvars **last);
void slist_free(formvars **start);
char *slist_item(const char *name, formvars *start);

formvars *process_data(char *query, formvars **start, formvars **last, const char delim, const char sep);
formvars *cgi_process_form();
char *cgi_param(const char *var_name);
size_t cgi_param_length(const char *var_name);

void libcgi_error(int error_code, const char *msg, ...);
void cgi_fatal(const char *msg);

void check_prio(int priority);
int cgi_find_lan();
#endif
