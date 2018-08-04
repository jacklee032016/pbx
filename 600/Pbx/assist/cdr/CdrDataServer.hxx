/* 
	Module Name:		CdrDataServer 
	
	Function:			A server for Cdr data(it's a receiver and pbx is client of cdr data sponsor ).
	                          with capability of receiver, saver, parser and support two extended interfaces
	                          for remote in future.
	                          
	Version:			1.0.0
	
	Bulid date:		11/05/2005

	Last Modify date:	16/05/2005

	Author:			wuaiwu
*/


#ifndef 		__CDRDATASERVER_H
#define		__CDRDATASERVER_H

#include <semaphore.h>
#include <unistd.h>
#include <string>
#include "helper.hxx"
//#include <map>
using namespace std;

typedef enum 
{
 	LOG_ALERT=1,	
 	LOG_CRIT,		
 	LOG_ERR,		
 	LOG_WARNING,		
	LOG_NOTICE,	
 	LOG_INFO,	
 	LOG_DEBUG,	
 	LOG_DEBUG_STACK,	
 	LOG_DEBUG_OPER,	
 	LOG_DEBUG_HB 
 	
}LOG_LEVEL;


class CdrDataServer
{
public:
	explicit CdrDataServer(const char* inifile);
	~CdrDataServer();

public:

	void startup();
	void jion();

	static int get_loglevel();
	
private:
	static void get_file_name(char *pfn);
	static bool get_first_filen(char *pfn);
	static bool file_parse(const char* pfn);
	static void string_parse(const string& source);
	static string ip_parse(const string& ss);

	// interface no implement
	// one kind: interface invoke Vocal::CdrInterafce
	void send_remote(/*CdrClient& cdrc*/);

	// the other kind: interface to Vocal::Radius
	void send_radius_server(/*Radius& cdrc*/);
	
	
	static void* receiver(void *p);
	static void* dispatcher(void *p);
	

private:

	static char* server_pipe; 
	static char* records_dir ;
	static int file_size ;
	static char* sfile;
	static LOG_LEVEL cur_level;
	pthread_t	th_c;
	
};

#endif
