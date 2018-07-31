/*
* $Id: asnt_nins.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef __ASNT_L3_H__
#define __ASNT_L3_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

#include "asnt_isdn.h"

typedef struct _layer3_proc	layer3_proc_t;
typedef struct _L3Timer L3Timer_t;

struct _L3Timer 
{
	layer3_proc_t		*pc;
	itimer_t		tl;
	int			nr;
};

struct _layer3_proc
{
	layer3_proc_t	*prev;
	layer3_proc_t	*next;
	layer3_proc_t	*child;
	layer3_proc_t	*master;
	ASNT_L3		*l3;
	int			callref;
	int			ces;
	int			selces;
	int			state;
	u_long		Flags;
	L3Timer_t	timer1;
	L3Timer_t	timer2;
	int			bc;
	int             	err;
	int			cause;
	int			hold_state;
	u_char		obuf[MAX_DFRAME_LEN];
	u_char		*op;
};

#define FLG_L3P_TIMER312		1
#define FLG_L3P_TIMER303_1		2
#define FLG_L3P_TIMER308_1		3
#define FLG_L3P_GOTRELCOMP		4

struct _layer3
{
	ASNT_L3			*prev;
	ASNT_L3			*next;
	
	msg_queue_t		squeue0;
	int				l2_state0;
	int				next_cr;
	int				debug;
	
	NTSTACK			*nst;
	layer3_proc_t		*proc;
};

static inline msg_t *l3_alloc_msg(int size)
{
	msg_t	*msg;

	msg = as_msg_alloc(size+MAX_HEADER_LEN);
	if (msg)
		msg_reserve(msg, MAX_HEADER_LEN);
	return(msg);
}

extern	void	asnt_l3_display_NR_IE(u_char *, char *, char *);

/* l3 pointer arrays */

typedef struct _ALERTING
{
	u_char *BEARER;
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *SIGNAL;
	u_char *HLC;
	u_char *USER_USER;
} ALERTING_t;

typedef struct _CALL_PROCEEDING
{
	u_char *BEARER;
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *HLC;
} CALL_PROCEEDING_t;

typedef struct _CONNECT
{
	u_char *BEARER;
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *DATE;
	u_char *SIGNAL;
	u_char *CONNECT_PN;
	u_char *CONNECT_SUB;
	u_char *LLC;
	u_char *HLC;
	u_char *USER_USER;
	int ces;
} CONNECT_t;

typedef struct _CONNECT_ACKNOWLEDGE
{
	u_char *CHANNEL_ID;
	u_char *DISPLAY;
	u_char *SIGNAL;
} CONNECT_ACKNOWLEDGE_t;

typedef struct _DISCONNECT
{
	u_char *CAUSE;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *SIGNAL;
	u_char *USER_USER;

	u_char *CHARGE;			/* Advice of Charge in Codeset 6 */
} DISCONNECT_t;

typedef struct _INFORMATION
{
	u_char *COMPLETE;
	u_char *DISPLAY;
	u_char *KEYPAD;
	u_char *SIGNAL;
	u_char *CALLED_PN;
} INFORMATION_t;

typedef struct _NOTIFY
{
	u_char *BEARER;
	u_char *NOTIFY;
	u_char *DISPLAY;
	u_char *REDIR_DN;
} NOTIFY_t;

typedef struct _PROGRESS
{
	u_char *BEARER;
	u_char *CAUSE;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *HLC;
} PROGRESS_t;

typedef struct _RELEASE
{
	u_char *CAUSE;
	u_char *FACILITY;
	u_char *DISPLAY;
	u_char *SIGNAL;
	u_char *USER_USER;
} RELEASE_t;

typedef struct _RELEASE_COMPLETE
{
	u_char *CAUSE;
	u_char *FACILITY;
	u_char *DISPLAY;
	u_char *SIGNAL;
	u_char *USER_USER;
} RELEASE_COMPLETE_t;

typedef struct _RESUME
{
	u_char *CALL_ID;
	u_char *FACILITY;
	int ces;
} RESUME_t;

typedef struct _RESUME_ACKNOWLEDGE
{
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *DISPLAY;
} RESUME_ACKNOWLEDGE_t;

typedef struct _RESUME_REJECT
{
	u_char *CAUSE;
	u_char *DISPLAY;
} RESUME_REJECT_t;

typedef struct _SETUP
{
	u_char *COMPLETE;
	u_char *BEARER;
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *NET_FAC;
	u_char *DISPLAY;
	u_char *KEYPAD;
	u_char *SIGNAL;
	u_char *CALLING_PN;
	u_char *CALLING_SUB;
	u_char *CALLED_PN;
	u_char *CALLED_SUB;
	u_char *REDIR_NR;
	u_char *LLC;
	u_char *HLC;
	u_char *USER_USER;
	int ces;
} SETUP_t;

typedef struct _SETUP_ACKNOWLEDGE
{
	u_char *CHANNEL_ID;
	u_char *FACILITY;
	u_char *PROGRESS;
	u_char *DISPLAY;
	u_char *SIGNAL;
} SETUP_ACKNOWLEDGE_t;

typedef struct _STATUS
{
	u_char *CAUSE;
	u_char *CALL_STATE;
	u_char *DISPLAY;
} STATUS_t;

typedef struct _STATUS_ENQUIRY
{
	u_char *DISPLAY;
} STATUS_ENQUIRY_t;

typedef struct _SUSPEND
{
	u_char *CALL_ID;
	u_char *FACILITY;
} SUSPEND_t;

typedef struct _SUSPEND_ACKNOWLEDGE
{
	u_char *FACILITY;
	u_char *DISPLAY;
} SUSPEND_ACKNOWLEDGE_t;

typedef struct _SUSPEND_REJECT
{
	u_char *CAUSE;
	u_char *DISPLAY;
} SUSPEND_REJECT_t;

typedef struct _CONGESTION_CONTROL
{
	u_char *CONGESTION;
	u_char *CAUSE;
	u_char *DISPLAY;
} CONGESTION_CONTROL_t;

typedef struct _USER_INFORMATION
{
	u_char *MORE_DATA;
	u_char *USER_USER;
} USER_INFORMATION_t;

typedef struct _RESTART
{
	u_char *CHANNEL_ID;
	u_char *DISPLAY;
	u_char *RESTART_IND;
} RESTART_t;

typedef struct _FACILITY
{
	u_char *FACILITY;
	u_char *DISPLAY;
} FACILITY_t;

typedef struct _HOLD
{
	u_char *DISPLAY;
} HOLD_t;

typedef struct _HOLD_ACKNOWLEDGE
{
	u_char *CHANNEL_ID;
	u_char *DISPLAY;
} HOLD_ACKNOWLEDGE_t;

typedef struct _HOLD_REJECT
{
	u_char *CAUSE;
	u_char *DISPLAY;
} HOLD_REJECT_t;

typedef struct _RETRIEVE
{
	u_char *CHANNEL_ID;
} RETRIEVE_t;

typedef struct _RETRIEVE_ACKNOWLEDGE
{
	u_char *CHANNEL_ID;
	u_char *DISPLAY;
} RETRIEVE_ACKNOWLEDGE_t;

typedef struct _RETRIEVE_REJECT
{
	u_char *CAUSE;
	u_char *DISPLAY;
} RETRIEVE_REJECT_t;



int _asnt_l3_send_proc(layer3_proc_t *proc, int op, void *arg);
int _asnt_l3_up(layer3_proc_t *, msg_t *);
int _asnt_l3_down(ASNT_L3 *l3, u_int prim, int dinfo, msg_t *msg);
void _asnt_l3_send_squeue(ASNT_L3 *l3, msg_queue_t *squeue);
int _asnt_l3_remove_proc(layer3_proc_t **procp, int ces);



#define PROTO_DIS_EURO			8	/* protocol discriminator */

#define L3_DEB_WARN			1
#define L3_DEB_PROTERR			2
#define L3_DEB_STATE			4
#define L3_DEB_PROC				8
#define L3_DEB_CHECK			16

enum
{
	ST_L3_LC_REL,
	ST_L3_LC_ESTAB_WAIT,
	ST_L3_LC_REL_DELAY, 
	ST_L3_LC_REL_WAIT,
	ST_L3_LC_ESTAB,
};

enum
{
	IMSG_END_PROC,
	IMSG_END_PROC_M,
	IMSG_L2_DATA,
	IMSG_L4_DATA,
	IMSG_TIMER_EXPIRED,
	IMSG_MASTER_L2_DATA,
	IMSG_PROCEEDING_IND,
	IMSG_ALERTING_IND,
	IMSG_CONNECT_IND,
	IMSG_SEL_PROC,
	IMSG_RELEASE_CHILDS,
};

struct _l3_msg
{
	int		mt;
	msg_t	*msg;
};

struct stateentry
{
	int state;
	int primitive;
	void (*rout) (layer3_proc_t *, int, void *);
};

#define SBIT(state)	(1<<state)
#define ALL_STATES	0x03ffffff


void _asnt_l3_debug(ASNT_L3 *l3, char *fmt, ...);
void asnt_l3_del_timer(L3Timer_t *t);
int asnt_l3_add_timer(L3Timer_t *t, int millisec, int timer_nr);
void asnt_l3_stop_all_l3_timer(layer3_proc_t *pc);
void _asnt_l3_new_l3_state(layer3_proc_t *pc, int state);
int _asnt_l3_msg(ASNT_L3 *l3, u_int pr, int dinfo, void *arg);
int _asnt_nins_message(layer3_proc_t *pc, u_char mt);
u_char *_asnt_nins_get_cause(layer3_proc_t *pc, msg_t *omsg, msg_t *nmsg);
void _asnt_nins_message_cause(layer3_proc_t *pc, u_char mt, u_char cause);


int _asnt_l3_get_callref(u_char *p);
layer3_proc_t *_asnt_l3_create_proc(ASNT_L3 *l3, int ces, int cr, layer3_proc_t *master);
void _asnt_l3_add_var_IE(layer3_proc_t *pc, u_char ie, u_char *iep);
void _asnt_nins_msg_without_setup(layer3_proc_t *pc, u_char cause);
void _asnt_nins_std_ie_err(layer3_proc_t *pc, int ret);
u_char *_asnt_nins_get_channel_id(layer3_proc_t *pc, msg_t *omsg, msg_t *nmsg);

#ifdef __cplusplus
	};
#endif


#endif
