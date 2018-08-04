/*
* $Id: _ddns.h,v 1.2 2007/05/31 20:38:05 lizhijie Exp $
*/
#ifndef	___DDNS_H__
#define	___DDNS_H__

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <errno.h>
#include <time.h>
#include <signal.h>     /* for singal handle */

#define UINT unsigned int
#define MAXKEYBYTES 56 // 448 bits
#define ROUNDCOUNT  16

#define	DDNS_DEBUG		1

#include "ddnsCodec.h"
#include "aslib.h"
#include "cgi_lib.h"
#include "cgiDdns.h"

#define SEND_BUFFER_SIZE				512
#define SEND_BUFFER_DATA_SIZE			504
#define SIZE_CMDPACKET					128 // Shake packet size
#define SESSION_FRAME_BUFFER_SIZE		65536

#define CMD_REQUEST					0x9101
#define CMD_AFFIRM						0x9102
#define CMD_EXIT						0x9103
#define CMD_ACCEPT						0x9104
#define CMD_CONNECTINFO				0x9105
#define CMD_STREAMHEAD				0x9106

#define COMMAND_REGI					"regi a"
#define COMMAND_CNFM					"cnfm\r\n"


typedef	struct 
{
	long nReq;
	long nOperateCode;
	long sessionCode;
	long nCheckSum;
	long nFill;
}UpdatePack;

typedef	struct 
{
	UpdatePack 		base;
	long 			ip;
}UpdatePackEx;

typedef struct
{
	char				serverIp[DDNS_DEFAULT_BUFFER_LENGTH];
	struct hostent 	*serverHost;

	int				tcpShakePort;
	int				shakeHandSocket;
	
	int				udpMyPort;
	int				udpDestPort;
	int				udpSocket;

	char				userName[DDNS_DEFAULT_BUFFER_LENGTH];
	char				passwd[DDNS_DEFAULT_BUFFER_LENGTH];

	blowfish_t		blowfish;

	/*rx from TCP shakehand Port , eg. cipher key stored as plain text */
	unsigned char 	plainChallenge[DDNS_DEFAULT_BUFFER_LENGTH];	
	int				challengeLength;
	
	long				sequence;
	long				sessionCode;

	int				quitFlags;
	
	long				interruptCount;
	long				lastInterruptTime;

	PEANUT_INFO		info;

	int				registerCount;
}ddns_task_t;

#define	DDNS_PID_FILE				"/var/run/ddns.pid"

#define	DDNS_SERVER_PORT			6060
#define	DDNS_MY_UDP_PORT			6070

#if 1
#define	DDNS_SERVER_IP				"hphwebservice.oray.net"//"hph021.oray.net"		/* "219.136.252.79" */
#define	DDNS_MY_NAME				"assistcn"
#define	DDNS_MY_PASSWD			"assist"
#define	DDNS_CLIENT_INFO			0x25E69767		//
#define	DDNS_KEY_INFO				0x0CDC2F23
#else
#define	DDNS_SERVER_IP			"ph031pro.oray.net"	/* */
#define	DDNS_MY_NAME				"user"
#define	DDNS_MY_PASSWD			"user"
#define	DDNS_CLIENT_INFO			0x88888888
#define	DDNS_KEY_INFO				0x88888888
#endif

#define	DDNS_NAME					"DDNS Daemon"

int GenerateCrypt(ddns_task_t *task, long clientinfo, long embkey, char *szResult);
void CryptBlow(ddns_task_t *task, char *crypted);
void DecryptBlow(ddns_task_t *task, char *decodings);

int	ddnsHeartBeatLoop(ddns_task_t *task);
int	ddnsShakeHand(ddns_task_t *task);

char *asInetNtoa(long ip);

#if		DDNS_DEBUG
void ddnsDebugBuffer(char *title, unsigned char *buf, int len);
#endif

long  as_Big2Little(long raw);

#define CIPHER_ALGO_BLOWFISH		4  /* blowfish 128 bit key */

#define BLOWFISH_BLOCKSIZE			8
#define BLOWFISH_ROUNDS			16

typedef struct
{
	unsigned long		s0[256];
	unsigned long		s1[256];
	unsigned long		s2[256];
	unsigned long		s3[256];
	unsigned long		p[BLOWFISH_ROUNDS+2];
} BLOWFISH_context;


int  bf_setkey( BLOWFISH_context *c, char *key, unsigned keylen );
void encrypt_block( BLOWFISH_context *bc, char *inbuf, char *outbuf );
void decrypt_block( BLOWFISH_context *bc, char *outbuf, char *inbuf );


#endif

