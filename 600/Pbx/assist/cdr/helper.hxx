#ifndef		__HELPER_H
#define		__HELPER_H

//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include "CdrDataServer.hxx"

void cdr_debug_info(int pri, const char* file, int line, const char* frmt,...);

#define cpLog(pri,frmt, args...)\
if (pri <= 10 && CdrDataServer::get_loglevel() >= pri)\
cdr_debug_info(pri, __FILE__, __LINE__,frmt,##args);\

int get_processid(const char* proname);

#endif
