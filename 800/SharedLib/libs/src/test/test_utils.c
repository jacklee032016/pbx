#include "aslib.h"
#include "stdlib.h"

const char* usage = "usage: test_log -f facility -d level -s|-c\n";
const char* facility[5] =
{
	"facility current value from 0to 3\n",
	"[0]AS_LOG_0: facility for cdr\n",
	"[1]AS_LOG_1: facility for sipserver\n",
	"[2]AS_LOG_2: facility for cgi\n",
	"[3]AS_LOG_3: facility for pbx\n",
};

const char* level[9] = 
{
	"debug level current value form 0 to 7\n",
	"[0]LOG_EMERG\n",
	"[1]LOG_ALERT\n",
	"[2]LOG_CRIT\n",
	"[3]LOG_ERR\n",
	"[4]LOG_WARNING\n",
	"[5]LOG_NOTICE\n",
	"[6]LOG_INFO\n",
	"[7]LOG_DEBUG\n",
};

const char* style = "-c: messages to console\n-s: messages to syslog file\n";


int main(int argc, char* argv[])
{
	if (argc != 6 ||
	    argv[1][1] != 'f' ||
	    argv[3][1] != 'd' ||
	    (argv[5][1] != 's' && argv[5][1] != 'c'))
		goto USAGE;
	
	log_stru_t testlog;
	
	switch (argv[5][1])
	{
	case 's':
		testlog.lstyle = USE_SYSLOG;
		//testlog.llevel = LOG_DEBUG;
		//testlog.lfacility = AS_LOG_6;
		break;
		
	case 'c':
		testlog.lstyle = USE_CONSOLE;
		//testlog.llevel = LOG_DEBUG;
		//testlog.lfacility = AS_LOG_6;
		break;

	default:
		goto USAGE;
	}


	int ifaci = atoi(argv[2]) + 16;
	if (!(ifaci >=20 && ifaci <=23))goto USAGE;

	int ileve = atoi(argv[4]) ;
	if (!(ileve >=0 && ileve <=7))goto USAGE;

	testlog.llevel = ileve;
	testlog.lfacility = ifaci << 3;	

	if (as_log_init(testlog) < 0)
	{
		printf("syslog init failed.\n");
		return 0;
	}
	
	as_cplog(ileve, "hello! log messages test start...");
	as_cplog(ileve, "test int: %d", 1010);
	//as_cplog(LOG_DEBUG, "test fload: %f", 1010.0);
	as_cplog(ileve, "test char: %c", 'X');
	as_cplog(ileve, "test string: %s", "THIS is OnLy a test.");
	as_cplog(ileve, "test all: %d-%c-%s", 9999, 'M', "This is only a test");
        as_cplog(ileve, "goodby! log test messaes end.");

	return 0;

USAGE:
	
	printf(usage);
	int i = 0;
	for (i = 0; i < 5; ++i)
		printf(facility[i]);
	for (i = 0; i < 9; ++i)
		printf(level[i]);
	printf(style);

	return 0;	
	
}



