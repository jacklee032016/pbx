/*
$Id: ask_test_l2.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
/*  L2 B-channel  L3 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "ask_isdn.h"
#include "l3ins.h"

#include "ask_user.h"

static char _l2_usage[] = 
{
	"Call with %s [options] [filename]\r\n\r\n" \
	"       filename   filename.in  incoming data\r\n" \
	"                  filename.out outgoing data\r\n" \
	"                  data is alaw for voice\r\n\r\n" \
	"\n     Valid options are:\r\n\r\n" \
	"  -?              Usage ; printout this information\r\n" \
	"  -c<n>           use card number n (default 1)\r\n" \
	"  -F<n>           use function n (default 0)\r\n" \
	"                    0 send and recive voice\r\n" \
	"                    1 send touchtones\r\n" \
	"                    2 recive touchtones\r\n" \
	"                    3 send and recive hdlc data\r\n" \
	"                    4 send and recive X75 data\r\n" \
	"                    5 send and recive voice early B connect\r\n" \
	"  -n <phone nr>   Phonenumber to dial\r\n" \
	"  -vn             Printing debug info level n\r\n\r\n"
};

typedef struct _devinfo
{
	int	device;
	int	cardnr;
	int	func;
	char	phonenr[32];
	int	d_stid;
	int	layer1;
	int	layer2;
	int	layer3;
	int	b_stid[2];
	int	b_adress[2];
	int	used_bchannel;
	int	save;
	int	play;
	FILE	*fplay;
	int	flag;
	int	val;
	int	cr;
	int	si;					/* service ID, used in BEAR IE */
	int	bl1_prot;
	int	bl2_prot;
	int	bl3_prot;
} devinfo_t;


#define ISDN_PID_L2_B_USER		0x420000ff

static int l2VerifyOn=0xFF;

char tt_char[]="0123456789ABCD*#";

#define PLAY_SIZE 64


int l2_play_msg(devinfo_t *di)
{
	unsigned char buf[PLAY_SIZE+AS_ISDN_HEADER_LEN];
	iframe_t *frm = (iframe_t *)buf;
	int len, ret;
	
	if (di->play<0)
		return(0);
	
	len = read(di->play, buf + AS_ISDN_HEADER_LEN, PLAY_SIZE);
	if (len<0)
	{
		printf("play_msg err %d: \"%s\"\n", errno, strerror(errno));
		close(di->play);
		di->play = -1;
	}
	
	frm->addr = di->b_adress[di->used_bchannel] | IF_DOWN;
	frm->prim = PH_DATA | REQUEST;
	frm->dinfo = 0;
	frm->len = len;
	ret = ask_isdn_write(di->device, buf, len+AS_ISDN_HEADER_LEN, 8000);
	if (ret < 0)
		fprintf(stdout,"play write error %d %s\n", errno, strerror(errno));
	else if (l2VerifyOn>3)
		fprintf(stdout,"play write ret=%d\n", ret);

	return(ret);
}

int l2_send_data(devinfo_t *di)
{
	unsigned char buf[MAX_DATA_BUF+AS_ISDN_HEADER_LEN];
	iframe_t *frm = (iframe_t *)buf;
	unsigned char *data;
	int len, ret;
	
	if (di->play<0 || !di->fplay)
		return(0);

	if (!(data = fgets(buf + AS_ISDN_HEADER_LEN, MAX_DATA_BUF, di->fplay)))
	{
		close(di->play);
		di->play = -1;
		data = buf + AS_ISDN_HEADER_LEN;
		data[0] = 4; /* ctrl-D */
		data[1] = 0;
	}

	len = strlen(data);
	if (len==0)
	{
		close(di->play);
		di->play = -1;
		data[0] = 4; /* ctrl-D */
		len = 1;
	}
	
	frm->addr = di->b_adress[di->used_bchannel] | IF_DOWN;
	frm->prim = PH_DATA | REQUEST;
	frm->dinfo = 0;
	frm->len = len;
	
	ret = ask_isdn_write(di->device, buf, len+AS_ISDN_HEADER_LEN, 100000);
	if (ret < 0)
		fprintf(stdout,"send_data write error %d %s\n", errno, strerror(errno));
	else if (l2VerifyOn>3)
		fprintf(stdout,"send_data write ret=%d\n", ret);

	return(ret);
}

int l2_setup_bchannel(devinfo_t *di)
{
	AS_ISDN_pid_t pid;
	int ret;
	layer_info_t li;


	if ((di->used_bchannel<0) || (di->used_bchannel>1))
	{
		fprintf(stdout, "wrong channel %d\n", di->used_bchannel);
		return(0);
	}
	memset(&li, 0, sizeof(layer_info_t));

	strcpy(&li.name[0], "B L2");
	li.object_id = -1;
	li.extentions = 0;
	li.pid.protocol[2] = di->bl2_prot;
	li.pid.layermask = ISDN_LAYER(2);
	li.st = di->b_stid[di->used_bchannel];

	ret = ask_isdn_new_layer(di->device, &li);
	if (ret<0)
	{
		fprintf(stdout, "new_layer ret(%d)\n", ret);
		return(0);
	}

	if (ret)
	{
		di->b_adress[di->used_bchannel] = ret;
		if (l2VerifyOn>2)
			fprintf(stdout,"b_adress%d %08x\n", di->used_bchannel+1, ret);
		
		memset(&pid, 0, sizeof(AS_ISDN_pid_t));

		pid.protocol[1] = di->bl1_prot;
		pid.protocol[2] = di->bl2_prot;
//		pid.protocol[3] = di->bl3_prot;
		pid.layermask = ISDN_LAYER(1) | ISDN_LAYER(2); // | ISDN_LAYER(3);
		if (di->flag & FLG_CALL_ORGINATE)
			pid.global = 1;

		ret = ask_isdn_set_stack(di->device, di->b_stid[di->used_bchannel], &pid);
		if (ret)
		{
			fprintf(stdout, "set_stack ret(%d)\n", ret);
			return(0);
		}
		ret = di->b_adress[di->used_bchannel];
	}
	return(ret);
}

int l2_send_SETUP(devinfo_t *di, int SI, char *PNr)
{
	unsigned char *np, *p, *msg, buf[1024];
	int len, ret;

	p = msg = buf + AS_ISDN_HEADER_LEN;
	MsgHead(p, di->cr, MT_SETUP);
	*p++ = 0xa1; /* complete indicator */
	*p++ = IE_BEARER;
	
	if (SI == IE_BEARER_TYPE_AUDIO )
	{ /* Audio */
		*p++ = 0x3;	/* Length                               */
		*p++ = 0x90;	/* Coding Std. CCITT, 3.1 kHz audio     */
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  */
		*p++ = 0xa3;	/* A-Law Audio                          */
	}
	else
	{ /* default Data transmission 64k */
		*p++ = 0x2;	/* Length                               */
		*p++ = 0x88;	/* Coding Std. CCITT, unrestr. dig. Inf */
		*p++ = 0x90;	/* Circuit-Mode 64kbps                  */
	}

	*p++ = IE_CALLED_PN;
	np = PNr;
	*p++ = strlen(np) + 1;
	/* Classify as AnyPref. */
	*p++ = 0x81;		/* Ext = '1'B, Type = '000'B, Plan = '0001'B. */

	while (*np)
		*p++ = *np++ & 0x7f;

	len = p - msg;

	ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN,
		DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);

	return(ret);
}

int l2_activate_bchan(devinfo_t *di)
{
	unsigned char buf[128];
	iframe_t *rfrm;
	int ret;

	ret = ask_isdn_write_frame(di->device, buf,
		di->b_adress[di->used_bchannel] | IF_DOWN,	PH_ACTIVATE | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (l2VerifyOn>3)
		fprintf(stdout,"PH_ACTIVATE write ret=%d\n", ret);
	
	ret = ask_isdn_read(di->device, buf, 128, TIMEOUT_10SEC); 	

	if (l2VerifyOn>3)
		fprintf(stdout,"PH_ACTIVATE read ret=%d\n", ret);

	rfrm = (iframe_t *)buf;
	if (ret>0)
	{
		if (rfrm->prim == (PH_ACTIVATE | CONFIRM))
		{
			di->flag |= FLG_BCHANNEL_ACTIVE;
		}
	}
	return(ret);
}

int l2_deactivate_bchan(devinfo_t *di)
{
	unsigned char buf[128];
	int ret;

	ret = ask_isdn_write_frame(di->device, buf,	di->b_adress[di->used_bchannel] | IF_DOWN,
		DL_RELEASE | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (l2VerifyOn>3)
		fprintf(stdout,"DL_RELEASE write ret=%d\n", ret);

	ret = ask_isdn_read(di->device, buf, 128, TIMEOUT_10SEC); 	
	if (l2VerifyOn>3)
		fprintf(stdout,"DL_RELEASE read ret=%d\n", ret);

	di->flag &= ~FLG_BCHANNEL_ACTIVE;
	di->flag &= ~FLG_BCHANNEL_SETUP;

	ret = ask_isdn_clear_stack(di->device, di->b_stid[di->used_bchannel]);
	if (l2VerifyOn>3)
		fprintf(stdout,"clear_stack ret=%d\n", ret);

	return(ret);
}

int l2_send_touchtone(devinfo_t *di, int tone)
{
	iframe_t frm;
	int tval, ret;

	if (l2VerifyOn>1)
		fprintf(stdout,"send_touchtone %c\n", DTMF_TONE_MASK & tone);

	tval = DTMF_TONE_VAL | tone;
	ret = ask_isdn_write_frame(di->device, &frm,di->b_adress[di->used_bchannel] | IF_DOWN,
		PH_CONTROL | REQUEST, 0, 4, &tval, TIMEOUT_1SEC);
	
	if (l2VerifyOn>3)
		fprintf(stdout,"tt send ret=%d\n", ret);

	return(ret);
}

int l2_read_mutiplexer(devinfo_t *di)
{
	unsigned char	*p, *msg, buf[MAX_REC_BUF];
	iframe_t	*rfrm;
	int		timeout = TIMEOUT_10SEC;
	int		ret = 0;
	int		len;

	rfrm = (iframe_t *)buf;
	
	/* Main loop */
start_again:
	while ((ret = ask_isdn_read(di->device, buf, MAX_REC_BUF, timeout)))
	{
		if (l2VerifyOn>3)
			fprintf(stdout,"readloop ret=%d\n", ret);

		if (ret >= 16)
		{
			if (l2VerifyOn>4)
				fprintf(stdout,"readloop addr(%x) prim(%x) len(%d)\n", rfrm->addr, rfrm->prim, rfrm->len);
			
			if (rfrm->addr == (di->b_adress[di->used_bchannel] | IF_DOWN))
			{/* B-Channel related messages */
				if (rfrm->prim == (PH_DATA | INDICATION))
				{/* received data, save it */
					write(di->save, &rfrm->data.i, rfrm->len);
				}
				else if (rfrm->prim == (PH_DATA | CONFIRM))
				{/* get ACK of send data, so we can send more */
					if (l2VerifyOn>5)
						fprintf(stdout,"PH_DATA_CNF\n");
					
					switch (di->func)
					{
						case TE_RX_TX_VOICE:
						case TE_TX_TOUCH_TONE:
							if (di->play > -1)
								l2_play_msg(di);
							break;
					}
				}
				else if (rfrm->prim == (PH_CONTROL | INDICATION))
				{
					if ((rfrm->len == 4) && ((rfrm->data.i & ~DTMF_TONE_MASK)
						== DTMF_TONE_VAL))
					{
						fprintf(stdout,"GOT TT %c\n", DTMF_TONE_MASK & rfrm->data.i);
					}
					else
						fprintf(stdout,"unknown PH_CONTROL len %d/val %x\n", rfrm->len, rfrm->data.i);
					
				}
			}
			
			/*******  D-Channel related messages ************/  
			else if ((ret > 19) && (buf[19] == MT_CONNECT) && (di->flag & FLG_CALL_ORGINATE))
			{ /* We got connect, so bring B-channel up */
				if (!(di->flag & FLG_BCHANNEL_EARLY))
				{
					if (!(di->flag & FLG_BCHANNEL_ACTDELAYED))
						l2_activate_bchan(di);
					else
						di->flag |= FLG_BCHANNEL_DOACTIVE;
				}

				/* send a CONNECT_ACKNOWLEDGE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, di->cr, MT_CONNECT_ACKNOWLEDGE);
				len = p - msg;
				
				ret = ask_isdn_write_frame(di->device, buf,  di->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);

				/* if here is outgoing data, send first part */
				switch (di->func)
				{
					case 0:
					case 2:
					case 5:
						if (di->play > -1)
							l2_play_msg(di);
						break;
						
					case 1:
						/* send next after 2 sec */
						timeout = 2*TIMEOUT_1SEC;
						di->flag |= FLG_SEND_TONE;
						break;
						
					case 3:
					case 4:
						/* setup B after 1 sec */
						timeout = 1*TIMEOUT_1SEC;
						break;
						
				}
			}
			else if ((ret > 19) && (buf[19] == MT_CONNECT_ACKNOWLEDGE) &&
				(!(di->flag & FLG_CALL_ORGINATE)))
			{/* We got connect ack, so bring B-channel up */
				if (!(di->flag & FLG_BCHANNEL_EARLY))
				{
					if (!(di->flag & FLG_BCHANNEL_ACTDELAYED))
						l2_activate_bchan(di);
					else
						di->flag |= FLG_BCHANNEL_DOACTIVE;
				}
				
				/* if here is outgoing data, send first part */
				switch (di->func)
				{
					case 0:
					case 2:
					case 5:
						if (di->play > -1)
							l2_play_msg(di);
						break;
						
					case 1:
						/* send next after 2 sec */
						timeout = 2*TIMEOUT_1SEC;
						di->flag |= FLG_SEND_TONE;
						break;
						
					case 3:
					case 4:
						/* setup B after 1 sec */
						timeout = 1*TIMEOUT_1SEC;
						break;
						
				}
			}
			else if ((ret > 19) && (buf[19] == MT_DISCONNECT))
			{/* send a RELEASE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, di->cr, MT_RELEASE);
				len = p - msg;
				ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN,
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE))
			{/* on a disconnecting msg leave loop */
				/* send a RELEASE_COMPLETE */
				p = msg = buf + AS_ISDN_HEADER_LEN;
				MsgHead(p, di->cr, MT_RELEASE_COMPLETE);
				len = p - msg;
				ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN, 
					DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
				return(2);
			}
			else if ((ret > 19) && (buf[19] == MT_RELEASE_COMPLETE))
			{/* on a disconnecting msg leave loop */
				return(1);
			}
		}
	}
	

	if (di->flag & FLG_SEND_TONE)
	{
		if (di->val)
		{
			di->val--;
			l2_send_touchtone(di, tt_char[di->val]);
		}
		else
		{
			/* After last tone disconnect */
			p = msg = buf + AS_ISDN_HEADER_LEN;
			MsgHead(p, di->cr, MT_DISCONNECT);
			len = p - msg;
			ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN, 
				DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
			di->flag &= ~FLG_SEND_TONE;
			
		}
		goto start_again;
		
	}
	else if (di->flag & FLG_SEND_DATA)
	{
		if (di->play > -1)
			l2_send_data(di);
		else
			di->flag &= ~FLG_SEND_DATA;
		goto start_again;
	}
	else if (di->flag & FLG_BCHANNEL_DOACTIVE)
	{
		ret = l2_activate_bchan(di);
		if (!ret)
		{
			fprintf(stdout,"error on activate_bchan\n");
			return(0);
		}
		di->flag &= ~FLG_BCHANNEL_DOACTIVE;
		/* send next after 1 sec */
		timeout = 1*TIMEOUT_1SEC;
		di->flag |= FLG_SEND_DATA;
		goto start_again;
	}
	return(0);
}

int l2_do_connection(devinfo_t *di)
{
	unsigned char *p, *msg, buf[1024];
	iframe_t *rfrm;
	int len, idx, ret = 0;
	int bchannel;

	rfrm = (iframe_t *)buf;

	if (strlen(di->phonenr))
	{/* if phone numner defined */
		di->flag |= FLG_CALL_ORGINATE;
		di->cr = 0x81;
		l2_send_SETUP(di, di->si, di->phonenr);
	}
	bchannel= -1;
	
	/* Wait for a SETUP message or a CALL_PROCEEDING */
	while ((ret = ask_isdn_read(di->device, buf, 1024, 3*TIMEOUT_10SEC)))
	{
		if (l2VerifyOn>3)
			fprintf(stdout,"readloop ret=%d\n", ret);

		if (ret >= 20)
		{
			if (((!(di->flag & FLG_CALL_ORGINATE)) &&(buf[19] == MT_SETUP)) ||
				((di->flag & FLG_CALL_ORGINATE) &&(buf[19] == MT_CALL_PROCEEDING)))
			{
				if (!(di->flag & FLG_CALL_ORGINATE)) /*as callee */
					di->cr = buf[18];

				idx = 20;
				while (idx<ret)
				{
					if (buf[idx] == IE_CHANNEL_ID)
					{/* find B channel used both as caller and callee */
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
		di->used_bchannel = bchannel -1;
		
		switch (di->func)
		{
			case TE_RX_TX_VOICE_B_E:
				di->flag |= FLG_BCHANNEL_EARLY;
			case TE_RX_TX_VOICE:
			case TE_RX_TOUCH_TONE:
			case TE_TX_TOUCH_TONE:
			case TE_RX_TX_HDLC_DATA:
			case TE_RX_TX_X75:
				ret = l2_setup_bchannel(di);
				if (ret)
					di->flag |= FLG_BCHANNEL_SETUP;
				else
				{
					fprintf(stdout,"error on setup_bchannel\n");
					goto clean_up;
				}

				if (di->flag & FLG_BCHANNEL_EARLY)
				{
					ret = l2_activate_bchan(di);
					if (!ret)
					{
						fprintf(stdout,"error on activate_bchan\n");
						goto clean_up;
					}
				}
				break;
		}
		
		if (!(di->flag & FLG_CALL_ORGINATE))
		{
			p = msg = buf + AS_ISDN_HEADER_LEN;
			MsgHead(p, di->cr, MT_CONNECT);
			len = p - msg;
			ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN, 
				DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
		}
		
		if (!l2_read_mutiplexer(di))
		{ /* timed out */
			/* send a RELEASE_COMPLETE */
			fprintf(stdout,"l2_read_mutiplexer timed out sending RELEASE_COMPLETE\n");
			
			p = msg = buf + AS_ISDN_HEADER_LEN;;
			MsgHead(p, di->cr, MT_RELEASE_COMPLETE);
			len = p - msg;

			ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN, 
				DL_DATA | REQUEST, 0, len, msg, TIMEOUT_1SEC);
		}
		
		l2_deactivate_bchan(di);
	}
	else
	{
		fprintf(stdout,"no channel or no connection\n");
	}
	
clean_up:
	sleep(1);
	ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN,
		DL_RELEASE | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	
	if (l2VerifyOn>3)
		fprintf(stdout,"ret=%d\n", ret);
	
	ret = ask_isdn_read(di->device, buf, 1024, TIMEOUT_10SEC);
	if (l2VerifyOn>3)
		fprintf(stdout,"read ret=%d\n", ret);
	
	sleep(1);
	ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN,
		MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (l2VerifyOn>3)
		fprintf(stdout,"ret=%d\n", ret);
	
	ret = ask_isdn_read(di->device, buf, 1024, TIMEOUT_10SEC);
	if (l2VerifyOn>3)
		fprintf(stdout,"read ret=%d\n", ret);
	
	return(0);
}



int l2_add_dlayer3(devinfo_t *di, int prot)
{
	layer_info_t li;
	stack_info_t si;
	interface_info_t ii;
	int lid, ret;

	if (di->layer3)
	{/* if L3 for D channel has exist, then clone a new stack from it */
		memset(&si, 0, sizeof(stack_info_t));
		
		si.extentions = EXT_STACK_CLONE;
		si.mgr = -1;
		si.id = di->d_stid;
		ret = ask_isdn_new_stack(di->device, &si);
		if (ret <= 0)
		{
			fprintf(stdout, "clone stack failed ret(%d)\n", ret);
			return(11);
		}

		di->d_stid = ret;
	}
	
	memset(&li, 0, sizeof(layer_info_t));

	strcpy(&li.name[0], "userspace-L3U");
	li.object_id = -1;
	li.extentions = 0;
	li.pid.protocol[3] = prot;
	li.pid.layermask = ISDN_LAYER(3);
	li.st = di->d_stid;

	lid = ask_isdn_new_layer(di->device, &li);
	if (lid<0)
		return(12);

	di->layer3 = lid;

	if (!di->layer3)
		return(13);
	
	ii.extentions = EXT_IF_CREATE | EXT_IF_EXCLUSIV;
	ii.owner = di->layer3;
	ii.peer = di->layer2;
	ii.stat = IF_DOWN;

	ret = ask_isdn_connect(di->device, &ii);
	if (ret)
		return(13);

	ii.owner = di->layer3;
	ii.stat = IF_DOWN;

	ret = ask_isdn_get_interface_info(di->device, &ii);
	if (ret != 0)
		return(14);

	if (ii.peer == di->layer2)
		fprintf(stdout, "Layer 2 not cloned\n");
	else
		fprintf(stdout, "Layer 2 %08x cloned from %08x\n", ii.peer, di->layer2);
	
	di->layer2 = ii.peer;

	return(0);
}

/* check D Channel L1/L2/L3 layer info, add L3 DSS1 USER, 
and send/wait DL_ESTABLISH_REQ/IND,eg. create a link in DL  */
int l2_do_setup(devinfo_t *di)
{
	unsigned char buf[1024];
	iframe_t *frm = (iframe_t *)buf;
	int i, ret = 0;
	stack_info_t *stinf;
	status_info_t *si;

	di->bl2_prot = ISDN_PID_L2_B_USER;
	di->bl3_prot = ISDN_PID_L3_B_TRANS;
	switch (di->func)
	{
		case TE_RX_TX_VOICE:
		case TE_RX_TX_VOICE_B_E :
			di->bl1_prot = ISDN_PID_L1_B_64TRANS;
			di->si = IE_BEARER_TYPE_AUDIO;
			break;
		case TE_RX_TOUCH_TONE:
			di->bl1_prot = ISDN_PID_L1_B_64TRANS;
			di->bl2_prot = ISDN_PID_L2_B_TRANSDTMF;
			di->si = IE_BEARER_TYPE_AUDIO;
			di->val= 8; /* send  8 touch tons (7 ... 0) */
			break;
		case TE_TX_TOUCH_TONE :
			di->bl1_prot = ISDN_PID_L1_B_64TRANS;
			di->bl2_prot = ISDN_PID_L2_B_TRANSDTMF;
			di->si = IE_BEARER_TYPE_AUDIO;
			break;
		case TE_RX_TX_HDLC_DATA:
			di->bl1_prot = ISDN_PID_L1_B_64HDLC;
			di->si = 7;
			break;
		case TE_RX_TX_X75 :
			fprintf(stdout,"X.75 not supported with L2 user\n");
			return(1);
		default:
			fprintf(stdout,"unknown program function %d\n", di->func);
			return(1);
	}

	ret = ask_isdn_get_stack_count(di->device);
	if (l2VerifyOn>1)
		fprintf(stdout,"%d stacks found\n", ret);
	if (ret < di->cardnr)
	{
		fprintf(stdout,"cannot config card nr %d only %d cards\n",	di->cardnr, ret);
		return(2);
	}
	
	ret = ask_isdn_get_stack_info(di->device, di->cardnr, buf, 1024);
	if (ret<=0)
	{
		fprintf(stdout,"cannot get stackinfo err: %d\n", ret);
		return(3);
	}
	stinf = (stack_info_t *)&frm->data.p;
	if (l2VerifyOn>1)
		ask_isdn_print_stack_info(stdout, stinf);

	di->d_stid = stinf->id;
	for (i=0;i<2;i++)
	{
		if (stinf->childcnt>i)
			di->b_stid[i] = stinf->child[i];
		else
			di->b_stid[i] = 0;
	}

	di->layer1 = ask_isdn_get_layerid(di->device, di->d_stid, 1);
	if (di->layer1<0)
	{
		fprintf(stdout,"cannot get layer1\n");
		return(4);
	}
	if (l2VerifyOn>1)
		fprintf(stdout,"layer1 id %08x\n", di->layer1);

	di->layer2 = ask_isdn_get_layerid(di->device, di->d_stid, 2);
	if (di->layer2<0)
	{
		fprintf(stdout,"cannot get layer2\n");
		return(5);
	}
	if (l2VerifyOn>1)
		fprintf(stdout,"layer2 id %08x\n", di->layer2);

	di->layer3 = ask_isdn_get_layerid(di->device, di->d_stid, 3);
	if (di->layer3<0)
	{
		fprintf(stdout,"cannot get layer3\n");
		di->layer3 = 0;
	}
	if (l2VerifyOn>1)
		fprintf(stdout,"layer3 id %08x\n", di->layer3);

	ret = l2_add_dlayer3(di, ISDN_PID_L3_DSS1USER);
	if (ret)
		return(ret);
	
	ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN,
		DL_ESTABLISH | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (l2VerifyOn>3)
		fprintf(stdout,"dl_etablish write ret=%d\n", ret);

	ret = ask_isdn_read(di->device, buf, 1024, TIMEOUT_10SEC);
	if (l2VerifyOn>3)
		fprintf(stdout,"dl_etablish read ret=%d\n", ret);
	if (ret>0)
	{
		if (frm->prim != (DL_ESTABLISH | CONFIRM))
			return(6);
	}
	else
	{
		fprintf(stdout,"DL_ESTABLISH | REQUEST return(%d)\n", ret);
		return(7);
	}
	
	ret = ask_isdn_get_status_info(di->device, di->layer1, buf, 1024);
	if (ret > AS_ISDN_HEADER_LEN)
	{
		si = (status_info_t *)&frm->data.p;
		ask_isdn_print_status(stdout, si);
	}
	else
		fprintf(stdout,"ask_isdn_get_status_info ret(%d)\n", ret);
	
	ret = ask_isdn_get_status_info(di->device, di->layer2, buf, 1024);
	if (ret > AS_ISDN_HEADER_LEN)
	{
		si = (status_info_t *)&frm->data.p;
		ask_isdn_print_status(stdout, si);
	}
	else
		fprintf(stdout,"ask_isdn_get_status_info ret(%d)\n", ret);
	
	sleep(1);
	return(0);
}

int main(int argc, char *argv[])
{
	char FileName[200],FileNameOut[200];
	int aidx=1,para=1, idx;
	char sw;
	devinfo_t AS_ISDN;
	int err;

	fprintf(stderr,"Test ISDN 1.0\n");
	strcpy(FileName, "test_file");
	memset(&AS_ISDN, 0, sizeof(AS_ISDN));
	AS_ISDN.cardnr = 1;
	AS_ISDN.func = 0;
	AS_ISDN.phonenr[0] = 0;
	if (argc<1)
	{
		fprintf(stderr,"Error: Not enough arguments please check\n");
		usage(argv[0], _l2_usage);
		exit(1);
	}

	do
	{
		if (argv[aidx] && argv[aidx][0]=='-')
		{
			sw=argv[aidx][1];
			switch (sw)
			{
				case 'c':
					if (argv[aidx][2])
					{
						AS_ISDN.cardnr=atol(&argv[aidx][2]);
					}
					break;
						
				case 'F':
					if (argv[aidx][2])
					{
						AS_ISDN.func=atol(&argv[aidx][2]);
					}
					break;
						
				case 'n':
					if (!argv[aidx][2])
					{
				        	idx = 0;
						aidx++;
					}
					else
					{
						idx=2;
					}

					if (aidx<=argc)
					{/* phone number */
						strcpy(AS_ISDN.phonenr, &argv[aidx][idx]);
					}
					else
					{
						fprintf(stderr," Switch %c without value\n",sw);
						exit(1);
					}
					break;
						
				case '?' :
					usage(argv[0], _l2_usage);
					exit(1);
					break;
						
				default  : 
					fprintf(stderr,"Unknown Switch %c\n",sw);
					usage(argv[0], _l2_usage);
					exit(1);
					break;
			}
		}
		else
		{
			if (para==1)
			{
				if (argc > 1)
					strcpy(FileName,argv[aidx]);
				para++;
			}
			else
			{
				fprintf(stderr,"Undefined argument %s\n",argv[aidx]);
				usage(argv[0], _l2_usage);
				exit(1);
			}
		}

		aidx++;
	}while (aidx<argc);
	
	if (0>(AS_ISDN.device = ask_isdn_open()))
	{
		printf("Test ISDN cannot open AS_ISDN due to %s\n", strerror(errno));
		return(1);
	}
	
	sprintf(FileNameOut,"%s.out",FileName);
	sprintf(FileName,"%s.in",FileName);
	if (0>(AS_ISDN.save = open(FileName, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU))) 
	{
		printf("TestAS_ISDN cannot open %s due to %s\n",FileName, strerror(errno));
		close(AS_ISDN.device);
		return(1);
	}
	
	if (0>(AS_ISDN.play = open(FileNameOut, O_RDONLY)))
	{
		printf("TestAS_ISDN cannot open %s due to %s\n",FileNameOut,strerror(errno));
		AS_ISDN.play = -1;
	}
	else 
		AS_ISDN.fplay = fdopen(AS_ISDN.play, "r");
	
	if (l2VerifyOn>8)
		fprintf(stdout,"fileno %d/%d/%d\n",AS_ISDN.save, AS_ISDN.play,	AS_ISDN.device);

	err = l2_do_setup(&AS_ISDN);
	if (err)
		fprintf(stdout,"do_setup error %d\n", err);
	else
		l2_do_connection(&AS_ISDN);	
	
	close(AS_ISDN.save);
	if (AS_ISDN.play>=0)
		close(AS_ISDN.play);

	err=ask_isdn_close(AS_ISDN.device);
	if (err)
		fprintf(stdout,"ask_isdn_close: error(%d): %s\n", err, strerror(err));
	
	return(0);
}

