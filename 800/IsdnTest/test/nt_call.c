/*
$Id: nt_call.c,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
*/
/* test L3 DSS1 NET */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "ask_isdn.h"
#include "l3ins.h"

#include "ask_test.h"
#include <sys/ioctl.h>


#include "as_thread.h"

int NT_send_SETUP(devinfo_t *di, int SI, char *PNr)
{
	unsigned char *np, *p, *msg, buf[1024];
	int len, ret;

	p = msg = buf + AS_ISDN_HEADER_LEN;
	MsgHead(p, di->cr, MT_SETUP);
/*	*p++ = 0xa1; *//* complete indicator, not support in INS  */
	*p++ = IE_BEARER;

	if (SI == IE_BEARER_TYPE_AUDIO)
	{ /* Audio */
		*p++ = 0x3;	/* Length                               				*/
		*p++ = 0x90;	/* Coding Std. CCITT, 3.1 kHz audio     		*/
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  			*/
		*p++ = 0xa2;	/* 0xa3:A-Law Audio; 0xa2 :u-law for japan  	*/
	}
	else
	{ /* default Datatransmission 64k */
		*p++ = 0x2;	/* Length                               */
		*p++ = 0x88;	/* Coding Std. CCITT, unrestr. dig. Inf */
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  */
	}

	*p++ = IE_CHANNEL_ID;
	*p++ = 0x1;	/* Length */
	*p++ = 0x80 | (1 + di->used_bchannel);

/*	
	if (strlen(di->display))
	{
		*p++ = IE_DISPLAY;
		*p++ = strlen(di->display);
		np = di->display;
		while(*np)
			*p++ = *np++ & 0x7f;
	}
*/
	if (strlen(di->msn))
	{
		*p++ = IE_CALLING_PN;
		*p++ = strlen(PNr) +2;
		*p++ = 0x00; /*00:unkwon, 01:international */
		*p++ = 0x80;
		np = PNr;
		while(*np)
			*p++ = *np++ & 0x7f;
	}

#if 0
	if (PNr && strlen(PNr))
	{
		*p++ = IE_CALLED_PN;
		np = di->msn;
		*p++ = strlen(np) + 1;
		/* Classify as AnyPref. */
		*p++ = 0x81;		/* Ext = '1'B, Type = '000'B, Plan = '0001'B. */

		while (*np)
			*p++ = *np++ & 0x7f;
	}
#endif

	len = p - msg;
	ret = ask_isdn_write_frame(di->fd, buf, di->layer3 | IF_DOWN,
		DL_UNITDATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);

	return(ret);
}



int NT_read_mutiplexer(devinfo_t *dev)
{
	unsigned char	*p, *msg, buf[MAX_REC_BUF];
	iframe_t	*rfrm;
//	int		timeout = TIMEOUT_10SEC;
	int		timeout = TIMEOUT_INFINIT;
	int		ret = 0;
	int		len;

	rfrm = (iframe_t *)buf;
	/* Main loop */

	sleep(1);

	while ((ret = ask_isdn_read(dev->fd, buf, MAX_REC_BUF, timeout)))
	{
//		if (dev->debug>3)
//			fprintf(stdout,"readloop in multiplexer ret=%d\n", ret);
		
		if (ret >= 16)
		{
			if (dev->debug>4)
				fprintf(stdout,"readloop addr(%x) prim(%x) len(%d)\n",rfrm->addr, rfrm->prim, rfrm->len);

			/******************* D-Channel related messages **********/  
			if ((ret > 19) && (buf[19] == MT_CONNECT) &&
				(dev->flag & FLG_CALL_ORGINATE))
			{/* We got connect, so bring B-channel up */

				printf("send a CONNECT_ACKNOWLEDGE \n");
				create_raw_thread( dev);

				/* send a CONNECT_ACKNOWLEDGE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_CONNECT_ACKNOWLEDGE);
				len = p - msg;
				
				ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST,0, len, msg, TIMEOUT_1SEC);

			}
			else if ((ret > 19) && (buf[19] == MT_DISCONNECT))
			{/* send a RELEASE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_RELEASE);
				len = p - msg;

				ret = ask_isdn_write_frame(dev->fd, buf,	dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST,0, len, msg, TIMEOUT_1SEC);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE))
			{/* on a disconnecting msg leave loop */
				/* send a RELEASE_COMPLETE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_RELEASE_COMPLETE);
				len = p - msg;
				ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
				
				return(2);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE_COMPLETE))
			{/* on a disconnecting msg leave loop */
				return(1);
			}
		}
	}

	return(0);
}


int NT_do_connection(devinfo_t *di)
{
	unsigned char *p, *msg, buf[1024];
	iframe_t *rfrm;
	int len, idx, ret = 0;
	int bchannel;
	int isContinue = 0;
	int x = 0;
	int res;

	rfrm = (iframe_t *)buf;

	if (di->func == OUT_CALL)
	{
		di->flag |= FLG_CALL_ORGINATE;
		di->cr = 0x81;
		NT_send_SETUP(di, di->si, di->phonenr);
		printf("After SETUP\n");
	}


	bchannel= di->used_bchannel + 1;
	
	/* Wait for a SETUP message or a CALL_PROCEEDING */
//	while ((ret = ask_isdn_read(di->fd, buf, 1024,  TIMEOUT_10SEC)))
	while ((ret = ask_isdn_read(di->fd, buf, 1024,  TIMEOUT_INFINIT)))
	{
		if (di->debug>3)
			fprintf(stdout,"readloop addr(%x) prim(%x) len(%d)\n",rfrm->addr, rfrm->prim, rfrm->len);

		if (ret >= 20)
		{
			if ( buf[19] == MT_ALERTING )
			{		
				di->cr = buf[18];
	 			idx = 20;
				isContinue = 1 ;
				break;
			}
		}
	}

	if(!isContinue)
		return 0;

	/* setup a B-channel stack */
	
	if (!NT_read_mutiplexer(di))
	{ /* timed out */
		/* send a RELEASE_COMPLETE */
		fprintf(stdout,"read_mutiplexer timed out sending RELEASE_COMPLETE\n");
		p = msg = buf + AS_ISDN_HEADER_LEN;;
		MsgHead(p, di->cr, MT_RELEASE_COMPLETE);
		len = p - msg;
		
		ret = ask_isdn_write_frame(di->fd, buf,	di->layer3 | IF_DOWN, 
			DL_DATA | REQUEST,0, len, msg, TIMEOUT_1SEC);
	}

	res = ioctl(di->bfd[di->used_bchannel],  AS_ISDN_CTL_DEACTIVATE, &x);
	
	return(0);
}


int ntdev_init(devinfo_t *dev)
{

	dev->bl1_prot = ISDN_PID_L1_B_64TRANS;
	dev->bl2_prot = ISDN_PID_L2_B_TRANS;
	dev->bl3_prot = ISDN_PID_L3_B_USER;//ISDN_PID_L3_B_TRANS;

	dev_init(dev);

	return(0);
}

devinfo_t  ntDev =
{
	name	:	"NT",
	cardnr	:	3,
	func		:	TE_RX_TX_VOICE,	
	
	phonenr	:	"8901"	,
	display	:	"Test Display",
	msn		:	"789",

	cloned	:	0,

	isRaw	:	1,
	si		:	IE_BEARER_TYPE_AUDIO,

	used_bchannel	:	1,

	debug	:	0xff
};

static void *ntCallingThread(void *data)
{
	dev_init(&ntDev);

	NT_do_connection(&ntDev);

	dev_release(&ntDev);
	printf("Quit from Call thread\n");
	pthread_exit(NULL);
	return NULL;
}

static as_thread_t  callingThread =
{
	name	:	"NT-RAW",
	handler	:	ntCallingThread,
	log		: 	printf,
	
	private	:	NULL
};


int main(int argc,char *argv[])
{
	pid_t pid;
	devinfo_t  *dev = &ntDev;	
	
	fprintf(stderr,"Test L3 INS NET 1.0\n\tUsage : nraw [cardno : 1-4]\n");
	if(argc>=2)
		dev->cardnr = atoi(argv[1]);
	
	pid = as_thread_create(&callingThread);

	fprintf(stderr,"running.....\n");
	pthread_join(pid, NULL);
	fprintf(stderr,"Ended\n");

	return(0);
}

