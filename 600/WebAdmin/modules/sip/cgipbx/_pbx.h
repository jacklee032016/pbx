/*$Id*/

#ifndef ___PBX_H__
#define ___PBX_H__

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <unistd.h>
#include "cgi_lib.h"
#include "cgi_llist.h"
#include "as_version.h"

#include "pbx.h"

// global vars for pbx
extern	llist pbxentries;
extern	llist sipblck_list;

#define	PBX_INVALIDATE_STR		"0xFF"

#if 0
extern	pbx_cfg_t 	pbx_global_cfgs[];
extern	pbx_cfg_t	pbx_ext_special_template[];
extern	pbx_cfg_t	pbx_ext_basic_template[];
extern	pbx_cfg_t	pbx_lines_cfgs[];
extern	pbx_cfg_t	pbx_trunck_cfgs[];
extern	pbx_cfg_t	pbx_account_cfgs[];
extern	pbx_cfg_t	pbx_specialservice_cfgs[];
#endif

char *getendch(char *);

void read_serial_cmd_thread(void);
void pbx_handler_cgisig(int, siginfo_t *,void * );

int pbx_send_do_spec_comcmd(unsigned char cmd,char *v1,char *v2);
int pbx_send_spec_comcmd(unsigned char cmd,char *v1,char *v2);
int pbx_send_digit_comcmd(unsigned char cmd,char * v1,char * v2);
int pbx_send_teshu_comcmd(unsigned char cmd,char *v1,char *v2);
int pbx_send_comcmd(unsigned char, char *v1,char *v2);

int	pbx_setup_pbx(llist *cfgs);
int	pbx_setup_pbx_when_startup(llist *cfgs);

#endif
