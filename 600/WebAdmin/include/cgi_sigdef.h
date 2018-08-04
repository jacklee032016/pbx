/*
* $Id: cgi_sigdef.h,v 1.3 2007/03/11 10:33:15 lizhijie Exp $
*/


#ifndef _SIGNAL_DEF_H_
#define _SIGNAL_DEF_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cgi_lib.h"

//processes name defined
#define	PROC_SIP	"pbx"
#define	PROC_PBX	"cgipbx"

//conf files defined
#define	FILE_PBX_CONF		"/etc/pbx/pbx.conf"
#define	FILE_PBX_TMP		"/etc/pbx/tmp.conf"
#define   FILE_BLCK_LIST		"/etc/sip/enemy_list.conf"


//realtime signals defined
#define	SIG_CGI_TO_PBX				2
#define	SIG_SIP			60 //SIGRTMIN
#define	SIG_PBX_CGI	59//(SIGRTMIN+1)
//#define	SIG_PBX_COM	(SIGRTMAX)
#define   SIG_SIP_GUA	61
#define   SIG_BLACKLIST	62

#if 0//opcodes defined
#define	OP_SIP_MODIFY_REQ		0
#define	OP_SIP_MODIFY_OK		1
#define	OP_SIP_STATUS_REQ		2
#define	OP_SIP_STATUS_OK		3
#define	OP_PBX_MODIFY_REQ	0
#define	OP_PBX_MODIFY_OK		1
#define	OP_PBX_STATUS_REQ	2
#define	OP_PBX_STATUS_OK		3
#endif

#define	OP_SIP_MODIFY_REQ		0
#define	OP_SIP_MODIFY_OK		1
#define	OP_SIP_STATUS_REQ		2
#define	OP_SIP_STATUS_OK		3
#define	OP_PBX_READ_REQ		0		/* READ request send from WebAdmin to CGIPBX */
#define	OP_PBX_READ_RPL		1		/* READ response(reply) from CGIPBX to WebAdmin */
#define	OP_PBX_WRITE_REQ		2		/* WRITE REQUEST */
#define	OP_PBX_WRITE_RPL		3		/* WRITE REPLY */
#define	OP_PBX_READ_OK		4		/* After process READ REPLY, then send back READ OK by WebAdmin */
#define	OP_PBX_WRITE_OK		5

int cgi_directory_filt_by_number(const struct dirent *namelist);
int  cgi_get_pid_by_name(char * appName);
int cgi_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *));
void cgi_sig_send(pid_t pid, int sigid, int sigcode);

#endif 

