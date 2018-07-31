/*
 * Parameter Processing Functions for 8120.ini
 * Miranda Wang
 * Wed Sep 19 16:04:24 CST 2007
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "param.h"

#if 0
#define CONFIG_FILE_NAME	"/tmp/jffs2/8120.cfg"
#else
#define CONFIG_FILE_NAME	"/etc/8120.cfg"
#endif
#define MAX_LINE_LENGTH		100
struct param *param_start = NULL;
struct param *param_end = NULL;

void plist_add(struct param *item)
{
	if (!param_start) {
		param_start = item;
		param_end = item;
		item->next = NULL;
	}	

	param_end->next = item;
	param_end = item;
	item->next = NULL;
}

void plist_free()
{
	struct param *current;
	struct param *tmp;

	current = param_start;
	while (current) {
		tmp = current;

		free(current->name);
		free(current->value);

		current = current->next;
		free(tmp);
	}

	param_start = NULL;
	param_end = NULL;
}

struct param *plist_item(char *name)
{
	struct param *current;
	current = param_start;

	while (current) {
		if (!strcmp(current->name, name))
			return current;

		current = current->next;
	}

	return NULL;
}

int plist_update_item(char *name, char *value)
{
	struct param *item;
	if (!value)
		value = "";
	
	item = plist_item(name);
	if (!item)
		return -1;

	if (strlen(item->value) < strlen(value)) {
		free(item->value);
		if (! (item->value = malloc(strlen(value) + 1)))
			return -1;
	}

	strcpy(item->value, value);
	return 0;
}



int plist_init()
{
	FILE *file;
	struct param *item = NULL;
	char *buffer = NULL;
	char *delem;
	int n;

	file = fopen(CONFIG_FILE_NAME, "r");
	if (!file) {
		perror("Open configuration file error");
		return -1;
	}

	while (cgi_getline(&buffer, &n, file) != -1) {
		if (*buffer == '#')
			continue;	

		buffer[strlen(buffer)-1] = '\0';

		delem = strchr(buffer, '=');
		/* Skip malformed line */
		if (!delem) 
			continue;

		*delem = '\0';
		item = malloc(sizeof(struct param));
		if (!item) {
			perror("alloc memory for new item");
			exit(1);
		}
		item->name = malloc(strlen(buffer) + 1);
		if (!item->name) {
			perror("alloc memory for new item");
			free(item);
			free(buffer);
			fclose(file);
			exit(1);
		}

		item->value = malloc(strlen(delem+1) + 1);
		if (!item->value){
			perror("alloc memory for new item");
			free(item);
			free(item->name);
			free(buffer);
			fclose(file);
			exit(1);
		}

		strcpy(item->name, buffer);
		strcpy(item->value, delem+1);

		plist_add(item);
	}

	free(buffer);
	fclose(file);
	return 0;
}

int plist_update()
{
	int fd;
	struct flock lock; 
	struct param *current;
	char *buffer;
	
	fd = open(CONFIG_FILE_NAME, O_WRONLY|O_TRUNC);
	if (fd < 0)
		return -1;

	buffer = malloc(MAX_LINE_LENGTH);
	if (!buffer)
	{close(fd);return -1;}

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	while (fcntl(fd, F_SETLK, &lock) == -1) {
		if ((errno == EBUSY) || (errno == EAGAIN)) {
			continue;
		} else {
			{free(buffer);close(fd);return -1;}
		}
	}

	current = param_start;
	while (current) {	
		sprintf(buffer, "%s=%s\n", current->name, current->value);
		if (write(fd, buffer, strlen(buffer)) < 0)
		{free(buffer);close(fd);return -1;}
		current = current->next;
	}

	free(buffer);
	close(fd);
	return 0;
}
