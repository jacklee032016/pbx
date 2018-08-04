/*
* $Id: te_call.c,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
* test B channel as raw device
*/
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "ask_isdn.h"

#include "l3ins.h"

#include "ask_test.h"
#include <sys/ioctl.h>


int TE_send_SETUP(devinfo_t *dev, int SI, char *PNr)
{
	unsigned char *np, *p, *msg, buf[1024];
	int len, ret;
	int i=0;

	unsigned char facility[32] = 
		{ 
			0x1c, 0x14, 0x91, 0xa1, 0x11,
			0x02, 0x01, 0x0a, 0x06, 0x07,
			0x03, 0xa2, 0x31, 0x87, 0x69,
			0x01, 0x05, 0x31, 0x03, 0x81,
			0x01, 0x01 
		};
			
	p = msg = buf + AS_ISDN_HEADER_LEN;
	MsgHead(p, dev->cr, MT_SETUP);
//	*p++ = 0xa1; /* complete indicator */
	*p++ = IE_BEARER;
	if (SI == IE_BEARER_TYPE_AUDIO)
	{ /* Audio */
		*p++ = 0x3;	/* Length                               */
		*p++ = 0x90;	/* Coding Std. CCITT, 3.1 kHz audio     */
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  */
		*p++ = 0xa2;	/* 0xa3: A-Law Audio,Euro; 0xa2:u-law(Japan)   */
	}
	else
	{ /* default Datatransmission 64k */
		*p++ = 0x2;	/* Length                               */
		*p++ = 0x88;	/* Coding Std. CCITT, unrestr. dig. Inf */
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  */
	}

	for(i=0; i<22;i++)
		*p++ = *(facility + i);
		
	*p++ = IE_CALLED_PN;
	np = PNr;
	*p++ = strlen(np) + 1;

	/* Classify as AnyPref. */
	*p++ = 0x80;		/* Ext = '1'B, Type = '000'B, Plan = '0001'B. */
	while (*np)
		*p++ = *np++ & 0x7f;

	len = p - msg;
	ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN,
		DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
	return(ret);
}


int TE_read_mutiplexer( devinfo_t *dev)
{
	unsigned char	*p, *msg, buf[MAX_REC_BUF];
	iframe_t	*rfrm;
	int		timeout = TIMEOUT_10SEC;
	int		ret = 0;
	int		len;

	rfrm = (iframe_t *)buf;
	/* Main loop */

	while ((ret = ask_isdn_read(dev->fd, buf, MAX_REC_BUF, timeout)))
	{
		if (ret >= 16)
		{
			fprintf(stdout,"readloop addr(%x) prim(%x) len(%d)\n", rfrm->addr, rfrm->prim, rfrm->len);

			if (rfrm->addr == (dev->b_adress[dev->used_bchannel] | IF_DOWN))
			{/* B-Channel related messages */
				fprintf(stdout,"Error : No B Channel msg should be used in this stage\n" );
			}
			
			/****************** D-Channel related messages ****************/ 
			else if ((ret > 19) && (buf[19] == MT_CONNECT)  )
			{/* We got connect, so bring B-channel up */
//				rawdev_activate_bchan(dev);

				/* send a CONNECT_ACKNOWLEDGE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_CONNECT_ACKNOWLEDGE);
				len = p - msg;
				ret = ask_isdn_write_frame(dev->fd, buf,	dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);

				/* if here is outgoing data, send first part */
				create_raw_thread( dev);

				fprintf(stdout,"send out CONNECT_ACK now\n" );

			}

			else if ((ret > 19) && (buf[19] == MT_DISCONNECT))
			{/* send a RELEASE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_RELEASE);
				len = p - msg;
				ret = ask_isdn_write_frame(dev->fd, buf,	dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE))
			{/* on a devsconnecting msg leave loop */
				/* send a RELEASE_COMPLETE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, dev->cr, MT_RELEASE_COMPLETE);
				len = p - msg;
				ret = ask_isdn_write_frame(dev->fd, buf,	dev->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
				return(2);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE_COMPLETE))
			{
				/* on a disconnecting msg leave loop */
				return(1);
			}
		}
	}
	
	return(0);
}


int TE_do_connection( devinfo_t *dev)
{
	unsigned char *p, *msg, buf[1024];
	iframe_t *rfrm;
	int len, idx, ret = 0;
	int bchannel;

	rfrm = (iframe_t *)buf;

	if (strlen(dev->phonenr))
	{
		dev->cr = 0x81;
		TE_send_SETUP(dev, dev->si, dev->phonenr);
		fprintf(stdout,"send SETUP\r\n");
	}
	
	bchannel= -1;
	/* Wait for a SETUP message or a CALL_PROCEEDING */
//	while ((ret = ask_isdn_read(dev->fd, buf, 1024, 3*TIMEOUT_10SEC))) 
	while ((ret = ask_isdn_read(dev->fd, buf, 1024, TIMEOUT_INFINIT ))) 
	{
		if (ret >= 20)
		{
			if ( buf[19] == MT_CALL_PROCEEDING )
			{
				dev->cr = buf[18];
				
	 			idx = 20;
				while (idx<ret)
				{
					if (buf[idx] == IE_CHANNEL_ID)
					{
						bchannel=buf[idx+2] & 0x3;
						break;
					}
					else if (!(buf[idx] & 0x80))
					{/* variable len IE */
						idx++;
						idx += buf[idx];
					}
					idx++;
				}
				break;
			}
		}
	}
	
	fprintf(stdout,"bchannel %d\n", bchannel);

	if (bchannel > 0)
	{/* setup a B-channel stack */
		int x = 0;
		int res;
		dev->used_bchannel = bchannel -1;

		if (!TE_read_mutiplexer(dev))
		{ /* timed out */
			/* send a RELEASE_COMPLETE */
			fprintf(stdout,"read_mutiplexer timed out sendevng RELEASE_COMPLETE\n");
			p = msg = buf + AS_ISDN_HEADER_LEN;;
			MsgHead(p, dev->cr, MT_RELEASE_COMPLETE);
			len = p - msg;
			ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN, 
				DL_DATA | REQUEST,0, len, msg, TIMEOUT_1SEC);
		}
		
		res = ioctl(dev->bfd[dev->used_bchannel],  AS_ISDN_CTL_DEACTIVATE, &x);
	}
	else
	{
		fprintf(stdout,"no channel or no connection\n");
	}
	

	return(0);
}

devinfo_t  rawDev =
{
	cardnr	:	1,
	func		:	TE_RX_TX_VOICE,	
	phonenr	:	"8901"	,
	name	:	"TE-C",

	cloned	:	0,

	isRaw	:	1,
	used_bchannel 	:	0,
	si		:	IE_BEARER_TYPE_AUDIO,
	debug	:	0xff
};

int main(int argc,char *argv[])
{
	devinfo_t  *dev = &rawDev;	
	fprintf(stderr,"Test Assist ISDN TA 1.0\r\n\tUsage : traw [cardno :1-4]\n");
	if(argc>=2)
		dev->cardnr = atoi(argv[1]);

	dev_init(&rawDev);

	TE_do_connection(&rawDev);

	dev_release(&rawDev);
	
	return(0);
}

