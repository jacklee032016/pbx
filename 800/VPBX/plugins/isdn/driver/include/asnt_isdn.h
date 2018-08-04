/*
* $Id: asnt_isdn.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
/*
* Assist NT Mode user library
* Li Zhijie, 2005.08.15¡¡
*/

#ifndef __ASNT_ISDN_H__
#define __ASNT_ISDN_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include "ibuffer.h"

#include "vpbx_globals.h"

#include "ask_isdn.h"
#include "as_msg.h"

#include "asnt_debug.h"

#define MSN_LEN						32
#define SUBADR_LEN					24
#define UUS_LEN						256
#define FAC_LEN						132
#ifndef ASNT_ISDN_FRAME_MIN
#define ASNT_ISDN_FRAME_MIN		8
#endif

/* ASNT_L2 is DL(SAPI+TEI) for L2 communication */
typedef struct _layer3			ASNT_L3;
typedef struct _layer4			ASNT_L4;


typedef struct _AS_ISDNif			AS_ISDNif_t;
typedef struct _AS_ISDNinstance	AS_ISDNinstance_t;
typedef struct _nt_stack			NTSTACK;				/* NT stack */
typedef struct _nr_list				nr_list_t;

typedef int (*ifunc_t)(NTSTACK *, msg_t *);				/* Info Frame Function oriented to STACK */

//typedef int (*hwSendOutFrame_t)(NTSTACK *, int addr, int prim, int  dinfo, int dlen, void *dbuf );
//typedef int (*hwSendOutMsg_t)(NTSTACK *nst, msg_t *msg);

#define	MAX_BDATA_SIZE	2048


/* App Layer (L4) primitive */
#define PR_APP_CHECK_NR		1
#define PR_APP_ICALL			2				/* input a call NTBCH  */
#define PR_APP_OCHANNEL		3				/* get available channel */
#define PR_APP_OCALL			4				/* output a call in NTBCH */
#define PR_APP_ALERT			5
#define PR_APP_CONNECT			6
#define PR_APP_HANGUP			7
#define PR_APP_CLEAR			8
#define PR_APP_USERUSER		9
#define PR_APP_FACILITY			10
#define PR_APP_OPEN_RECFILES	11
#define PR_APP_CLOSE_RECFILES	12

#define FLG_NST_READER_ABORT	1
#define FLG_NST_TERMINATION	2

#define FEATURE_NET_HOLD		0x00000001
#define FEATURE_NET_PTP		0x00000002
/* used in PRI */
#define FEATURE_NET_CRLEN2		0x00000004
#define FEATURE_NET_EXTCID		0x00000008

/* NT stack(L2/L3 in user space and L1/L0 in kernel space)
 * a NT stack instance is for a ST interface(port) determined by cardnr  */
struct _nt_stack
{
	int					device;				/* file descriptor*/
	int					cardnr;				/* Card (port) No. which support NT L0/L1 in one Card */
	int					d_stid;				/* D Channel Stack ID in kernel space */

	int					l2_id;				/* layer2_t id in kernel space */
	int					l3_id;				/* layer3_t id in kernel space */

//	msg_t				*phd_down_msg;
	/* hold the dinfo of PH_DATA_REQ iframe_t last send out to driver which wait to confirm
	* when it is 0, no PH_DATA_REQ frame wait to confirm
	*/
	int					phd_downmsg_dinfo;	

	ASNT_L3				*layer3;

	ifunc_t				if_l2_2_l3;	/* called by IsdnNtDevice to send data RX from driver */
	ifunc_t				l3_2_mgr;	/* send data to IsdnNtDevice after protocol stack */

	ifunc_t				if_l3_2_l2;	/* stack send data to device command thread */
	ifunc_t				mgr_2_l3;	/* EndPoint state machine send data to stack */
	
	msg_queue_t			down_queue;		/* msg queue to down PHY */
//	msg_queue_t			rqueue;				/* msg queue construct from frames which read from fd */
//	msg_queue_t			wqueue;
//	sem_t				work;
//	pthread_mutex_t		lock;

	u_long				flag;
//	struct _itimer			*tlist;

	u_long				feature;

	/* pointer to IsdnNtDevice */
	void 				*priv;
	
	/* send out frame destinated to driver */
//	hwSendOutFrame_t 	hwSendFrame;
//	hwSendOutMsg_t		hwSendMsg;

	nr_list_t				*nrlist;
};

struct _nr_list
{
	nr_list_t			*prev;
	nr_list_t			*next;
	unsigned char		len;
	unsigned char		nr[MSN_LEN];
	unsigned char		name[64];
	int				typ;
	int				flags;
};

#define NR_TYPE_INTERN		1		/* between 2 B Channels in a port*/
#define NR_TYPE_AUDIO		2		/* between B ch and sound card */
#define NR_TYPE_VOIP		3

typedef struct _itimer
{
	struct _itimer		*prev;
	struct _itimer		*next;
	NTSTACK			*nst;
	
	int				id;
	int				expires;
	u_long			Flags;
	
	unsigned long		data;
	int				(*function)(unsigned long);

	void 			*queueTimer;		/* timer implemented in FIFO queue */
} itimer_t;

#define FLG_TIMER_RUNING		1
#define FLG_TIMER_INITED		2

/* B Channel Call Flags */
#define FLG_BC_USE				0x00000001
#define FLG_BC_SENT_CID		0x00000002
#define FLG_BC_CALL_ORGINATE	0x00000004
#define FLG_BC_PROGRESS		0x00000008
#define FLG_BC_APPLICATION		0x00000010
#define FLG_BC_TONE_DIAL		0x00000100
#define FLG_BC_TONE_BUSY		0x00000200
#define FLG_BC_TONE_ALERT		0x00000400
#define FLG_BC_TONE_SILENCE	0x00000800
#define FLG_BC_TONE_NONE		0x00000000
#define FLG_BC_TONE				0x00000F00
#define FLG_BC_RECORD			0x00010000
#define FLG_BC_RECORDING		0x00020000
#define FLG_BC_RAWDEVICE		0x01000000
#define FLG_BC_KEEP_SEND		0x02000000
#define FLG_BC_TERMINATE		0x08000000

#define MSG_L1_PRIM				0x010000
#define MSG_L2_PRIM				0x020000
#define MSG_L3_PRIM				0x030000


extern	u_char		*asnt_l3_find_IE(u_char *, int, u_char, int);
extern	u_char		*asnt_l3_find_and_copy_IE(u_char *, int, u_char, int, msg_t *);
extern	void		asnt_l3_display_NR_IE(u_char *, char *, char *);

typedef struct _AS_ISDNuser_head
{
	u_int	prim;
	int		dinfo;
} ASNT_ISDNUSER_HEADER;

#define ASNT_ISDNUSER_HEAD_SIZE	sizeof(ASNT_ISDNUSER_HEADER)

/* interface msg help routines */

static inline void AS_ISDN_newhead(u_int prim, int dinfo, msg_t *msg)
{
	ASNT_ISDNUSER_HEADER *hh = (ASNT_ISDNUSER_HEADER *)msg->data;

	hh->prim = prim;
	hh->dinfo = dinfo;
}

static inline int if_newhead(void *arg, ifunc_t func, u_int prim, int dinfo, msg_t *msg)
{
	if (!msg)
		return(-ENXIO);
	AS_ISDN_newhead(prim, dinfo, msg);
	return(func((NTSTACK *)arg, msg));
}

static inline void AS_ISDN_addhead(u_int prim, int dinfo, msg_t *msg)
{
	ASNT_ISDNUSER_HEADER *hh = (ASNT_ISDNUSER_HEADER *)msg_push(msg, ASNT_ISDNUSER_HEAD_SIZE);

	hh->prim = prim;
	hh->dinfo = dinfo;
}


static inline int if_addhead(void *arg, ifunc_t func, u_int prim, int dinfo,msg_t *msg)
{
	if (!msg)
		return(-ENXIO);
	AS_ISDN_addhead(prim, dinfo, msg);
	return(func((NTSTACK *)arg, msg));
}


static inline msg_t *create_link_msg(u_int prim, int dinfo, int len, void *arg, int reserve)
{
	msg_t	*msg;

	if (!(msg = as_msg_alloc(len + ASNT_ISDNUSER_HEAD_SIZE + reserve)))
	{
		wprint("%s: no msg size %d+%d+%d\n", __FUNCTION__,len, ASNT_ISDNUSER_HEAD_SIZE, reserve);
		return(NULL);
	}
	else
		msg_reserve(msg, reserve + ASNT_ISDNUSER_HEAD_SIZE);

	if (len)
		memcpy(msg_put(msg, len), arg, len);
	AS_ISDN_addhead(prim, dinfo, msg);
	return(msg);
}

static inline int if_link(void *farg, ifunc_t func, u_int prim, int dinfo, int len,void *arg, int reserve)
{
	msg_t	*msg;
	int	err;

	if (!(msg = create_link_msg(prim, dinfo, len, arg, reserve)))
		return(-ENOMEM);
	err = func((NTSTACK *)farg, msg);
	if (err)
		as_msg_free(msg);
	return(err);
}

static inline msg_t *prep_l3data_msg(u_int prim, int dinfo, int ssize, int dsize, msg_t *old)
{
	if (!old)
	{
		old = as_msg_alloc(ssize + dsize + ASNT_ISDNUSER_HEAD_SIZE + DEFAULT_HEADROOM);
		if (!old)
		{
			wprint("%s: no msg size %d+%d+%d\n", __FUNCTION__,ssize, dsize, ASNT_ISDNUSER_HEAD_SIZE + DEFAULT_HEADROOM);
			return(NULL);
		}
	}
	else
	{
		old->data = old->head + DEFAULT_HEADROOM;
		old->tail = old->data;
		old->len = 0;
	}
	memset(msg_put(old, ssize + ASNT_ISDNUSER_HEAD_SIZE), 0,	ssize + ASNT_ISDNUSER_HEAD_SIZE);
	AS_ISDN_newhead(prim, dinfo, old);
	return(old);
}

/* bchannel.h */
/* B Channel Call State */
enum
{
	BC_CSTATE_NULL,
	BC_CSTATE_ICALL,				/* In Call state */
	BC_CSTATE_OCALL,				/* Out Call state */
	BC_CSTATE_OVERLAP_REC,
	BC_CSTATE_PROCEED,
	BC_CSTATE_ALERTING,
	BC_CSTATE_ACTIV,
	BC_CSTATE_DISCONNECT,
	BC_CSTATE_DISCONNECTED,
	BC_CSTATE_RELEASE,
};

enum
{
	BC_BSTATE_NULL,
	BC_BSTATE_SETUP,
	BC_BSTATE_ACTIVATE,
	BC_BSTATE_ACTIV,
	BC_BSTATE_DEACTIVATE,
	BC_BSTATE_CLEANUP,
};


extern	void			as_msg_init(void);
extern	msg_t		*as_msg_alloc(int);
extern	void			as_msg_free(msg_t *);
extern	msg_queue_t	*free_queue;
extern	msg_t		*as_msg_copy(msg_t *msg);

#define	MSG_CLONE(m)	as_msg_copy(m)


extern	int		asnt_l3_init(NTSTACK *);
extern	void		asnt_l3_cleanup(NTSTACK *);

extern	int		asnt_timer_init(itimer_t *, NTSTACK  *);
extern	int		asnt_timer_add(itimer_t *);
extern	int		asnt_timer_del(itimer_t *);
extern	int		asnt_timer_remove(itimer_t *it);
extern	int		asnt_timer_pending(itimer_t *);
extern	int 		asnt_timer_handle(itimer_t *timer);

extern	int 		asnt_upper_output(NTSTACK *nst, msg_t *msg);

extern	int 		asnt_write_frame(NTSTACK *nst, int addr, int prim, int dinfo, int dlen, void *dbuf);
extern	int 		asnt_write_msg(NTSTACK *nst, msg_t *msg);

#ifdef __cplusplus
	};
#endif


#endif
