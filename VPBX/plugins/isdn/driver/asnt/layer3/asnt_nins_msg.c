#include <stdlib.h>

#if __ARM_IXP__
#include <arm/bitops.h>
#else
#include <asm/bitops.h>
#endif

#include "ask_isdn.h"
#include "asnt_nins.h"
#include "l3ins.h"
#include "helper.h"

void _asnt_nins_release_cmpl(layer3_proc_t *pc, int pr, void *arg);
void _asnt_nins_information(layer3_proc_t *pc, int pr, void *arg);
void _asnt_nins_release(layer3_proc_t *pc, int pr, void *arg);

static layer3_proc_t *create_child_proc(layer3_proc_t *pc, int mt, msg_t *msg, int state)
{
	ASNT_ISDNUSER_HEADER	*hh;
	struct _l3_msg	l3m;
	layer3_proc_t	*p3i;

	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	p3i = _asnt_l3_create_proc(pc->l3, hh->dinfo, pc->callref, pc);
	if (!p3i)
	{
		_asnt_l3_debug(pc->l3, "cannot create child\n");
		return(NULL);
	}

	p3i->state = pc->state;
	if (pc->state != -1)
		_asnt_l3_new_l3_state(pc, state);
	l3m.mt = mt;
	l3m.msg = msg;

	_asnt_l3_send_proc(p3i, IMSG_L2_DATA, &l3m);
	return(p3i);
}                                                   

static void _asnt_nins_setup_acknowledge_m(layer3_proc_t *pc, int pr, void *arg)
{
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	asnt_l3_del_timer(&pc->timer1);
	create_child_proc(pc, pr, arg, 25);
}

static void _asnt_nins_proceeding_m(layer3_proc_t *pc, int pr, void *arg)
{
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	asnt_l3_del_timer(&pc->timer1);
	create_child_proc(pc, pr, arg, 9);
}

static void _asnt_nins_alerting_m(layer3_proc_t *pc, int pr, void *arg)
{
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	asnt_l3_del_timer(&pc->timer1);
	create_child_proc(pc, pr, arg, 7);
}

static void _asnt_nins_connect_m(layer3_proc_t *pc, int pr, void *arg)
{
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	asnt_l3_del_timer(&pc->timer1);
	create_child_proc(pc, pr, arg, 8);
}

static void _asnt_nins_release_m(layer3_proc_t *pc, int pr, void *arg)
{
	_asnt_nins_message(pc, MT_RELEASE_COMPLETE);
}

static void _asnt_nins_release_mx(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t	*msg = arg;

	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	_asnt_nins_release(pc, pr, msg);
}

static void _asnt_nins_release_cmpl_m(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t	*msg = arg;
	u_char	*p;

	if (pc->state == 6)
	{
		msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
		if ((p = _asnt_nins_get_cause(pc, msg, NULL)))
		{
			dprint(DBGM_L3,"%s cause (%d/%d)\n", __FUNCTION__,pc->cause, pc->err);
			switch(pc->cause) 
			{
				case CAUSE_USER_BUSY:
					break;

				case CAUSE_CALL_REJECTED:
					if (pc->err == CAUSE_USER_BUSY)
						pc->cause = pc->err;
					break;

				default:
					pc->cause = pc->err;
			}
		}
		test_and_set_bit(FLG_L3P_GOTRELCOMP, &pc->Flags);
	}
}

void _asnt_nins_release_cmpl_mx(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t	*msg = arg;

	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	_asnt_nins_release_cmpl(pc, pr, msg);
}

static void _asnt_nins_information_mx(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t	*msg = arg;

	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	_asnt_nins_information(pc, pr, msg);
}

struct stateentry mdatastatelist[] =
{
/*
#warning setup acknowledge
*/
	{SBIT(6) | SBIT(7) | SBIT(9) | SBIT(25),
		MT_SETUP_ACKNOWLEDGE, 		_asnt_nins_setup_acknowledge_m},
	{SBIT(6) | SBIT(7) | SBIT(9) | SBIT(25),
		MT_CALL_PROCEEDING, 		_asnt_nins_proceeding_m},
	{SBIT(6) | SBIT(7) | SBIT(9) | SBIT(25),
		MT_ALERTING, 				_asnt_nins_alerting_m},
	{SBIT(6) | SBIT(7) | SBIT(9) | SBIT(25),
		MT_CONNECT, 				_asnt_nins_connect_m},
	{SBIT(2) | SBIT(3) | SBIT(4) | SBIT(7) | SBIT(8) | SBIT(9) | SBIT(10) |
	 SBIT(11) | SBIT(12) | SBIT(15) | SBIT(17) | SBIT(19) | SBIT(25),
		MT_INFORMATION, 			_asnt_nins_information_mx},
	{SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10) | SBIT(11) |
	 SBIT(12) | SBIT(15) | SBIT(17),
		MT_RELEASE, 					_asnt_nins_release_mx},
	{SBIT(6) | SBIT(7) | SBIT(8) | SBIT(9) | SBIT(22) | SBIT(25),
		MT_RELEASE, 					_asnt_nins_release_m},
	{SBIT(19),  MT_RELEASE, 			_asnt_nins_release_cmpl},
	{SBIT(0) | SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10) |
	 SBIT(11) | SBIT(12) | SBIT(15) | SBIT(17) | SBIT(19),
		MT_RELEASE_COMPLETE, 		_asnt_nins_release_cmpl_mx},
	{SBIT(6) | SBIT(7) | SBIT(8) | SBIT(9) | SBIT(22) | SBIT(25),
		MT_RELEASE_COMPLETE, 		_asnt_nins_release_cmpl_m},
};

#define MDATASLLEN \
	(sizeof(mdatastatelist) / sizeof(struct stateentry))
                                                   

void  asnt_master_l3_state(struct _l3_msg *l3m, layer3_proc_t *proc)
{
	int i;
	for (i = 0; i < MDATASLLEN ; i++)
	{
		if ((l3m->mt == mdatastatelist[i].primitive) &&
			((1 << proc->state) & mdatastatelist[i].state))
		break;
	}	

	if (i == MDATASLLEN)
	{
		if (proc->l3->debug & L3_DEB_STATE)
		{
			_asnt_l3_debug(proc->l3, "NINS state %d mt %#x unhandled", proc->state, l3m->mt);
		}
		if ((MT_RELEASE_COMPLETE != l3m->mt) && (MT_RELEASE != l3m->mt))
		{
//			_asnt_nins_status_send(proc, CAUSE_NOTCOMPAT_STATE);
		}
	}
	else
	{
		if (proc->l3->debug & L3_DEB_STATE)
		{
			_asnt_l3_debug(proc->l3, "NINS state %d mt %x", proc->state, l3m->mt);
		}
		mdatastatelist[i].rout(proc, l3m->mt, l3m->msg);
	}
	return ;
}

