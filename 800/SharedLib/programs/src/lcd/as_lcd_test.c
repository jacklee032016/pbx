#include "aslib.h"
#include "as_lcd.h"
#include <unistd.h>
#include <stdlib.h>

const char* usage = "usage: lcdtest [-t|-p|-d] [line] [row] string... \n-t:	test time update \n-p:	test text print \n-d: default test\n";

void test_printf(char* pstring);
void default_printf();

int main(int argc, char* argv[])
{
	if (argc < 2) 
	{
		printf(usage);
		return 0;
	}

	if (argv[1][1] == 't')
	{
		if (as_lcd_time_update() < 0)
				printf("Update time error.\n");
			else
				printf("Update time OK.\n");
		return 0;
	}

	if (argv[1][1] == 'p')
	{
		int x = 1, y = 1;
		char buf[22] = {""};
		if (argc == 2)
		{
		}
		else
		{
			x = atoi(argv[2]);
			y = atoi(argv[3]);
			if (argc >= 5)
			{
				//int words = argc - 4;
				//printf("words = %d\n", words);
				int i = 4;

				int m = 0;
				for (i = 4; i < argc; ++i)
				{
					if (m >= 21) break;
					//printf("argv[%d]: %s\n", i, argv[i]);
					int n = 0;
					while (m < 21 && argv[i][n] != '\0')
					{
						buf[m] = argv[i][n];
						//printf("n : argv[%d][%d] | %d : %c\t", i, n,n,argv[i][n]);
						++n;
						++m;
					}
					//printf("\n");
					buf[m] = 0x20;
					++m;
					
					
				}
			}
			
		}

		buf[21] = 0;

		//printf("buf: %s\n", buf);
		//return 0;

		if (as_lcd_printf(x,y, buf) > 0)
			printf("Write text OK.\n");
		else printf("Write text Error.\n");

		return 0;
			
	}
	
	if (argv[1][1] == 'd')
	{
		default_printf();
		return 0;
	}

	return 0;

}

void test_printf(char* pstring)
{
	printf("test_printf with %s\n", pstring);
	int i = 2;
	for (; i < 8; ++i)
	{
		as_lcd_printf(i, 0, pstring);	
	}
}

#if 0
void default_printf()
{
	if (as_lcd_printf(1,1, "hello lcd!") > 0) printf("OK\n");
	if (as_lcd_printf(2,1, "test int: %d", 55) < 0) printf("OK\n");
	if (as_lcd_printf(3,1, "test fload: %f", 5.5) > 0) printf("OK\n");
	if (as_lcd_printf(4,1, "test char: %c", 'H') > 0) printf("OK\n");
	if (as_lcd_printf(5,1, "test string: %s","aBcD") > 0)printf("OK\n");
	if (as_lcd_printf(6,1, "goodbye lcd!") > 0) printf("OK\n");
}
#endif 

void default_printf()
{
	if (as_lcd_printf(1,1, "hello lcd!") > 0) printf("OK\n");
	sleep(1);
	if (as_lcd_printf(2,1, "test int: %d", 55) > 0) printf("OK\n");
	sleep(1);
	if (as_lcd_printf(3,1, "test float: %f", 5.5) > 0) printf("OK\n");
	sleep(1);
	if (as_lcd_printf(4,1, "test char: %c", 'H') > 0) printf("OK\n");
	sleep(1);
	if (as_lcd_printf(5,1, "test string: %s", "aBcDeF") > 0)printf("OK\n");
	sleep(1);
	if (as_lcd_printf(6,1, "goodbye lcd!") > 0) printf("OK\n");
}
