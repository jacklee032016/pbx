#ifndef _sig_def_def_
#define _sig_def_def_

/// #include "sip_set.h"

#define	SIG_SIP_MODIFY_REQ	SIGRTMIN
#define	SIG_SIP_MODIFY_OK		(SIGRTMIN+1)
#define	SIG_SIP_STATUS_REQ	(SIGRTMIN+2)
#define	SIG_SIP_STATUS_OK		(SIGRTMIN+3)
#define	SIG_CGI_PBX			60

#define	SHM_SIP_KEY	(key_t)0x010
#define	SEM_SIP_KEY	(key_t)0x015

#define	IFLAGS	(IPC_CREAT | IPC_EXCL)

int sigmnt(int signo, void (*sig_handler)(int, siginfo_t *, void *));
void sigsend(pid_t pid, int signo);
void sighandler(int signo, siginfo_t *info,void * val);
void sighandler_status(int signo, siginfo_t *info, void *val);
int set_ok();
int get_ok();
int clr_ok();




#endif
