#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

/*
 * Add a new DNS to the system
 * Description:
 *	You must check if the dns_name is valid.
 * Return value:
 *	0  - success
 *	-1 - failure 
 */
int set_dns(char *dns)
{
	FILE *fp;
	char line[80];
	int flag = 0;
	fp = fopen("/etc/resolv.conf", "a+");
	if (fp == NULL)
		return -1;
	if (dns == NULL)
		return -1;

	while (fscanf(fp, "%s", line) != -1)
	{
		if (!strncmp(line, "nameserver", 10))
		{
			flag = 1;
			continue;
		}
		if (flag)
		{
			if (!strncmp(line, dns, strlen(dns)))
				return -1;
			flag = 0;
		}
	}
	fprintf(fp, "nameserver %s\n", dns);

	fclose(fp);
	return 0;
}
