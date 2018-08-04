/*
* $Id: cgi_cdr.h,v 1.1 2007/03/30 09:10:35 lizhijie Exp $
*/
#ifndef	__CGI_CDR_H__
#define	__CGI_CDR_H__

#include "cgi_lib.h"

#define		CDR_OP_CONFIG					"cdrConfig"
#define		CDR_OP_VIEW					"cdrView"

typedef		struct
{
	llist		cgiVariables;
	
}CDR_INFO;

#endif

