/* 
	Module Name:		main 
	Function:			launch all                         
	Version:			1.0.0
	Bulid date:		12/05/2005
	Last Modify date:	12/05/2005
	Author:			wuaiwu
*/

#include "CdrDataServer.hxx"
#include "helper.hxx"
#include <unistd.h>
#include <fcntl.h>
#include <error.h>

int main (int argc, char* argv[])
{
	int nerror = 0;
	if ((nerror = get_processid("cdr")) > 0)
	{
		//cpDebug(LOG_DEBUG, "cdrServer is existen, this will exit.");
		return 0;
	}
	else if (nerror == -1)
	{
		//cpDebug(LOG_DEBUG, "I don't know whether cdrServer was existent, since something error occured.");
		//cpDebug(LOG_DEBUG, "Press y continue else exit.");
		char some = '0';
		scanf("%c", &some);
		if (some != 'y' && some != 'Y') 
		{
			//cpDebug(LOG_DEBUG, "exit\n");
			return 0;
		}
	}
	
	CdrDataServer cdr_server("/etc/sip/cdr/cdr.cfg");
	cdr_server.startup();

 	return 0;
}

 
