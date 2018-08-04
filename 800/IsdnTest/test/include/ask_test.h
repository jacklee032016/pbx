/*
$Id: ask_test.h,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
*/
#ifndef  __ASK_USER_H__
#define __ASK_USER_H__

#define ISDN_PID_L3_B_USER 		0x430000ff

#define ISDN_MODE_NT				1
#define ISDN_MODE_TE				0

#define IE_BEARER_TYPE_AUDIO		0		/* audio(64k circuit switch mode) , support by Japan TA*/
#define IE_BEARER_TYPE_64K			1		/* normal 64k circuit switch mode */

#define FLG_SEND_TONE				0x0001
#define FLG_SEND_DATA				0x0002
#define FLG_BCHANNEL_SETUP		0x0010
#define FLG_BCHANNEL_DOACTIVE		0x0020
#define FLG_BCHANNEL_ACTIVE		0x0040
#define FLG_BCHANNEL_ACTDELAYED	0x0080
#define FLG_CALL_ORGINATE			0x0100
#define FLG_BCHANNEL_EARLY			0x0200

#define MAX_REC_BUF				4000
#define MAX_DATA_BUF				1024

#define IE_BEARER_TYPE_64K			1		/* normal 64k circuit switch mode */
#define IE_BEARER_TYPE_AUDIO		0		/* audio(64k circuit switch mode) */

typedef enum
{
	TE_RX_TX_VOICE			=	0,
	TE_RX_TOUCH_TONE,
	TE_TX_TOUCH_TONE,
	TE_RX_TX_HDLC_DATA,
	TE_RX_TX_X75,
	TE_RX_TX_VOICE_B_E		/* voice early B connection */
}ftype_t;

#define PLAY_SIZE 					256


#define	MsgHead(ptr, cref, mty) \
	*ptr++ = 0x8; \
	if (cref == -1) { \
		*ptr++ = 0x0; \
	} else { \
		*ptr++ = 0x1; \
		*ptr++ = cref^0x80; \
	} \
	*ptr++ = mty

void usage(char *pname, char *usage_msg) ;


#define OUT_CALL	0
#define IN_CALL		1


typedef struct _dev_info
{
	char name[128];
	int	fd;
	int	cardnr;			/* port number : from 1 to 4 */
	ftype_t func;

	int 	ntMode;
	int 	headerLength;
	
	char	phonenr[32];
	char	display[32];
	char	msn[32];
	
	int	d_stid;
	int 	cloned;
	int	layer1;
	int	layer2;
	int	layer3;

	int	b_stid[2];
	int	b_adress[2];
	int	used_bchannel;

	int 	isRaw;
	int 	bfd[2];	/*raw device file descriptor for B channel */
	
	int	bl1_prot;
	int	bl2_prot;
	int	bl3_prot;

	int	cr;
	int	si;
	
	FILE	*fplay;
	int	save;
	int	play;
	int	flag;
	int	val;

	int 	debug;
} devinfo_t;

int dev_play_msg(devinfo_t *di);
int dev_send_data(devinfo_t  *di);

int dev_setup_bchannel(devinfo_t *di, int index);

int dev_activate_bchan(devinfo_t *di);
int dev_deactivate_bchan(devinfo_t *di);
int dev_add_dstack_layer3(devinfo_t *di, int prot);
int dev_init(devinfo_t *dev);

void dev_release(devinfo_t *dev);
int dev_send_touchtone(devinfo_t *di, int tone);


int create_raw_thread( devinfo_t *dev);


#endif

