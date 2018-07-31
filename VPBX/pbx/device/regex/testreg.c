#include <stdio.h>
#include <sys/types.h>
#include "regex.h"

#define DEBUG
int main()
{
	int i;
	const char *buf = "[3]#([0-9])+[#]";
	const char *test = "3#9945";
	
	regex_t *preg;
	preg = (regex_t *) malloc(sizeof(regex_t));
//	memset (preg, 0, sizeof(regex_t));

	i = regcomp(preg, buf, REG_EXTENDED);
	
	if (i != 0)
	{
		perror ("Regcomp error,:");
		exit (1);
	}

	printf ("Buf regcomp completed\n");

	printf ("Length of test is %d\n", strlen(test));
	i = strlen(test);
	i = re_match(preg, test, i, 0, 0);

	if (i < 0)
		printf ("No match!\n");
	else if (i == 0)
		printf ("Full matched!\n");
	else
		printf ("Partial matched %d!\n", i);

	return 0;
}
