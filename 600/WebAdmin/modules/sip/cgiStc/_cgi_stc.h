/*
* $Id: _cgi_stc.h,v 1.5 2007/09/14 09:11:09 lizhijie Exp $
*/

#ifndef	___CGI_STC_H__
#define	___CGI_STC_H__

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

#include "uart_cmd.h"

#include "pbx.h"

extern	llist pbxentries;

#define	STC_CFG_TYPE_NONE 					0x00000001
#define	STC_CFG_TYPE_BOOL 					0x00000002
#define	STC_CFG_TYPE_VALUE					0x00000004
#define	STC_CFG_TYPE_VALUE_COMPRESSED		0x00000008

#define	STC_CFG_TYPE_KEY						0x00000010
#define	STC_CFG_TYPE_VALUE_TRANSFER			0x00000020
#define	STC_CFG_TYPE_VALUE_TIME				0x00000040
#define	STC_CFG_TYPE_INTEGER					0x00000080
#define	STC_CFG_TYPE_BLACKLIST				0x00001000

#define	STC_CFG_TYPE_VALUE_DIGIT				0x10000000

void  stc_read_serial_cmd_thread(void );
int	stc_setup_pbx_when_startup(llist *cfgs);

int	stc_setup_pbx(llist *cfgs);
pbx_cfg_t *stc_find_cfg_item(PBX_COMMAND *cmd);

void stc_handler_cgisig(int signo, siginfo_t *info,void * val);

int	pbx_tx_int_value(unsigned char cmd, unsigned char value1, unsigned char value2);
int pbx_tx_digit_value(unsigned char cmd,char *value);
int pbx_tx_index_digit_value(unsigned char cmd, unsigned char index, unsigned char *digits);
int pbx_tx_index_compressed_digit_value(unsigned char cmd, unsigned char index, unsigned char type, unsigned char *digits);

int get_index(char *str);

#define	SEND_OUT_PBX(cmd)	\
	do{	if(as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_CGI)< 0) {	\
		AS_MSG_LOG(AS_LOG_ERR, "write to UART message queue failed");	\
		return -1;}	\
		}while(0)


#endif

