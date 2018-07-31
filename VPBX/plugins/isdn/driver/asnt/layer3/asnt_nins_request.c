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
 int _asnt_l3_send_msg(layer3_proc_t *pc, int state);
void _asnt_l3_msg_start(layer3_proc_t *pc, u_char mt);

static void _asnt_nins_setup_ack_req(layer3_proc_t *pc, int pr, void *arg)
{
	SETUP_ACKNOWLEDGE_t *setup_ack = arg;

	if (setup_ack)
	{
		_asnt_l3_msg_start(pc, MT_SETUP_ACKNOWLEDGE);
		if (setup_ack->CHANNEL_ID)
		{
			if (setup_ack->CHANNEL_ID[0] == 1)
				pc->bc = setup_ack->CHANNEL_ID[1] & 3;
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, setup_ack->CHANNEL_ID);
		}

		if (setup_ack->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, setup_ack->FACILITY);

		if (setup_ack->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, setup_ack->PROGRESS);

		if (setup_ack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, setup_ack->DISPLAY);

		_asnt_l3_send_msg(pc, 2);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 2);
		_asnt_nins_message(pc, MT_SETUP_ACKNOWLEDGE);
	}
	asnt_l3_del_timer(&pc->timer1);
	asnt_l3_add_timer(&pc->timer1, T302, 0x302);
}

static void _asnt_nins_proceed_req(layer3_proc_t *pc, int pr, void *arg)
{
	CALL_PROCEEDING_t *cproc = arg;

	asnt_l3_del_timer(&pc->timer1);
	if (cproc)
	{
		_asnt_l3_msg_start(pc, MT_CALL_PROCEEDING);
		if (cproc->BEARER)
			_asnt_l3_add_var_IE(pc, IE_BEARER, cproc->BEARER);

		if (cproc->CHANNEL_ID)
		{
			if (cproc->CHANNEL_ID[0] == 1)
				pc->bc = cproc->CHANNEL_ID[1] & 3;
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, cproc->CHANNEL_ID);
		}

		if (cproc->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, cproc->FACILITY);

		if (cproc->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, cproc->PROGRESS);

		if (cproc->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, cproc->DISPLAY);

		if (cproc->HLC)
			_asnt_l3_add_var_IE(pc, IE_HLC, cproc->HLC);
		_asnt_l3_send_msg(pc, 3);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 3);
		_asnt_nins_message(pc, MT_CALL_PROCEEDING);
	}
}

static void _asnt_nins_alert_req(layer3_proc_t *pc, int pr, void *arg)
{
	ALERTING_t *alert = arg;

	if (alert)
	{
		_asnt_l3_msg_start(pc, MT_ALERTING);

		if (alert->BEARER)
			_asnt_l3_add_var_IE(pc, IE_BEARER, alert->BEARER);

		if (alert->CHANNEL_ID)
		{
			if (alert->CHANNEL_ID[0] == 1)
				pc->bc = alert->CHANNEL_ID[1] & 3;
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, alert->CHANNEL_ID);
		}

		if (alert->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, alert->FACILITY);

		if (alert->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, alert->PROGRESS);

		if (alert->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, alert->DISPLAY);

		if (alert->HLC)
			_asnt_l3_add_var_IE(pc, IE_HLC, alert->HLC);

		if (alert->USER_USER)
			_asnt_l3_add_var_IE(pc, IE_USER_USER, alert->USER_USER);

		_asnt_l3_send_msg(pc, 4);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 4);
		_asnt_nins_message(pc, MT_ALERTING);
	}
	asnt_l3_del_timer(&pc->timer1);
}

static void _asnt_nins_setup_req(layer3_proc_t *pc, int pr, void *arg)
{
	SETUP_t	*setup = arg;
	msg_t	*msg;
	int	l;

	_asnt_l3_msg_start(pc, MT_SETUP);
	if (setup->COMPLETE)
		*pc->op++ = IE_COMPLETE;

	if (setup->BEARER)
		_asnt_l3_add_var_IE(pc, IE_BEARER, setup->BEARER);

	if (setup->CHANNEL_ID)
	{
		if (setup->CHANNEL_ID[0] == 1)
			pc->bc = setup->CHANNEL_ID[1] & 3;
		_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, setup->CHANNEL_ID);
	}

	if (setup->FACILITY)
		_asnt_l3_add_var_IE(pc, IE_FACILITY, setup->FACILITY);

	if (setup->PROGRESS)
		_asnt_l3_add_var_IE(pc, IE_PROGRESS, setup->PROGRESS);

	if (setup->NET_FAC)
		_asnt_l3_add_var_IE(pc, IE_NET_FAC, setup->NET_FAC);

	if (setup->DISPLAY)
		_asnt_l3_add_var_IE(pc, IE_DISPLAY, setup->DISPLAY);

	if (setup->KEYPAD)
		_asnt_l3_add_var_IE(pc, IE_KEYPAD, setup->KEYPAD);

	if (setup->CALLING_PN)
		_asnt_l3_add_var_IE(pc, IE_CALLING_PN, setup->CALLING_PN);

	if (setup->CALLING_SUB)
		_asnt_l3_add_var_IE(pc, IE_CALLING_SUB, setup->CALLING_SUB);

	if (setup->CALLED_PN)
		_asnt_l3_add_var_IE(pc, IE_CALLED_PN, setup->CALLED_PN);

	if (setup->CALLED_SUB)
		_asnt_l3_add_var_IE(pc, IE_CALLED_SUB, setup->CALLED_SUB);

	if (setup->LLC)
		_asnt_l3_add_var_IE(pc, IE_LLC, setup->LLC);

	if (setup->HLC)
		_asnt_l3_add_var_IE(pc, IE_HLC, setup->HLC);

	if (setup->USER_USER)
		_asnt_l3_add_var_IE(pc, IE_USER_USER, setup->USER_USER);
	
	l = pc->op - &pc->obuf[0];
	if (!(msg = l3_alloc_msg(l)))
		return;

	memcpy(msg_put(msg, l), &pc->obuf[0], l);
	_asnt_l3_new_l3_state(pc, 6);

	dhexprint(DBGM_L3DATA, "l3 oframe:", &pc->obuf[0], l);
#if 0	
#warning testing
#endif
	if (pc->l3->l2_state0 && (pc->l3->nst->feature & FEATURE_NET_PTP))
	{
		dprint(DBGM_L3, "%s: proc(%p) sending SETUP to CES 0\n", __FUNCTION__, pc);
		if (_asnt_l3_msg(pc->l3, DL_DATA | REQUEST, 0, msg))
			as_msg_free(msg);
	}
	else
	{
		dprint(DBGM_L3, "%s: proc(%p) sending SETUP to broadcast CES\n", __FUNCTION__, pc);
		if (_asnt_l3_msg(pc->l3, DL_UNITDATA | REQUEST, 127, msg))
			as_msg_free(msg);
	}

	asnt_l3_del_timer(&pc->timer1);
	test_and_clear_bit(FLG_L3P_TIMER303_1, &pc->Flags);
	asnt_l3_add_timer(&pc->timer1, T303, 0x303);
	test_and_set_bit(FLG_L3P_TIMER312, &pc->Flags);
	asnt_l3_del_timer(&pc->timer2);
	asnt_l3_add_timer(&pc->timer2, T312, 0x312);
}

static void _asnt_nins_disconnect_req(layer3_proc_t *pc, int pr, void *arg);

static void _asnt_nins_connect_req(layer3_proc_t *pc, int pr, void *arg)
{
	CONNECT_t *conn = arg;

	asnt_l3_del_timer(&pc->timer1);
	if (conn && conn->CHANNEL_ID)
	{
		if (conn->CHANNEL_ID[0] == 1)
			pc->bc = conn->CHANNEL_ID[1] & 3;
	}
#if 0	
#warning pc->bc is nice, but a task of the application. if you change anything, please let me know. 
#endif

#if 0
	if (!pc->bc)
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "D-chan connect for waiting call");
		_asnt_nins_disconnect_req(pc, pr, NULL);
		return;
	}
#endif

	if (conn)
	{
		_asnt_l3_msg_start(pc, MT_CONNECT);
		if (conn->BEARER)
			_asnt_l3_add_var_IE(pc, IE_BEARER, conn->BEARER);

		if (conn->CHANNEL_ID)
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, conn->CHANNEL_ID);

		if (conn->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, conn->FACILITY);

		if (conn->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, conn->PROGRESS);

		if (conn->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, conn->DISPLAY);

		if (conn->DATE)
			_asnt_l3_add_var_IE(pc, IE_DATE, conn->DATE);

		if (conn->CONNECT_PN)
			_asnt_l3_add_var_IE(pc, IE_CONNECT_PN, conn->CONNECT_PN);

		if (conn->CONNECT_SUB)
			_asnt_l3_add_var_IE(pc, IE_CONNECT_SUB, conn->CONNECT_SUB);

		if (conn->LLC)
			_asnt_l3_add_var_IE(pc, IE_LLC, conn->LLC);

		if (conn->HLC)
			_asnt_l3_add_var_IE(pc, IE_HLC, conn->HLC);

		if (conn->USER_USER)
			_asnt_l3_add_var_IE(pc, IE_USER_USER, conn->USER_USER);

		_asnt_l3_send_msg(pc, 10);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 10);
		_asnt_nins_message(pc, MT_CONNECT);
	}
}

static void _asnt_nins_connect_res(layer3_proc_t *pc, int pr, void *arg)
{
	CONNECT_ACKNOWLEDGE_t	*connack = arg;
//	int			cause;

//	asnt_l3_del_timer(&pc->timer1);

	/* reply CC_NEW_CR to L4 */
//	_asnt_l3_send_proc(pc, IMSG_SEL_PROC, NULL);

	if (connack && connack->CHANNEL_ID)
	{
		if (connack->CHANNEL_ID[0] == 1)
			pc->bc = connack->CHANNEL_ID[1] & 3;
	}
#if 0
	if (!pc->bc)
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "D-chan connect for waiting call");
		_asnt_nins_disconnect_req(pc, pr, NULL);
		return;
	}
#endif
	if (connack)
	{
		_asnt_l3_msg_start(pc, MT_CONNECT_ACKNOWLEDGE);

		if (connack->CHANNEL_ID)
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, connack->CHANNEL_ID);

		if (connack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, connack->DISPLAY);

		if (connack->SIGNAL)
			_asnt_l3_add_var_IE(pc, IE_SIGNAL, connack->SIGNAL);
		_asnt_l3_send_msg(pc, 10);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 10);
		_asnt_nins_message(pc, MT_CONNECT_ACKNOWLEDGE);
	}
	//cause = CAUSE_NONSELECTED_USER;
	//_asnt_l3_send_proc(pc, IMSG_RELEASE_CHILDS, &cause);
}

static void _asnt_nins_disconnect_req(layer3_proc_t *pc, int pr, void *arg)
{
	DISCONNECT_t *disc = arg;

	asnt_l3_stop_all_l3_timer(pc);
	if (disc)
	{
		_asnt_l3_msg_start(pc, MT_DISCONNECT);
		if (disc->CAUSE)
		{ 
			_asnt_l3_add_var_IE(pc, IE_CAUSE, disc->CAUSE);
		}
		else
		{
			*pc->op++ = IE_CAUSE;
			*pc->op++ = 2;
			*pc->op++ = 0x80 | CAUSE_LOC_PNET_LOCUSER;
			*pc->op++ = 0x80 | CAUSE_NORMALUNSPECIFIED;
		}

		if (disc->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, disc->FACILITY);

		if (disc->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, disc->PROGRESS);

		if (disc->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, disc->DISPLAY);

		if (disc->USER_USER)
			_asnt_l3_add_var_IE(pc, IE_USER_USER, disc->USER_USER);

		if (disc->CHARGE )/* charge info in codeset 6 , 2006.06.12 */
		{
			*pc->op++ = IE_CODESET_6;
			_asnt_l3_add_var_IE(pc, IE_ADVISE_CHARGE, disc->CHARGE );
		}	
		
		_asnt_l3_send_msg(pc, 12);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 12);
		_asnt_nins_message_cause(pc, MT_DISCONNECT, CAUSE_NORMALUNSPECIFIED);
	}
	asnt_l3_add_timer(&pc->timer1, T305, 0x305);
}

static void _asnt_nins_facility_req(layer3_proc_t *pc, int pr, void *arg)
{
	FACILITY_t *fac = arg;

	if (fac)
	{
		_asnt_l3_msg_start(pc, MT_FACILITY);

		if (fac->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, fac->FACILITY);
		else
			return;
		
		if (fac->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, fac->DISPLAY);

		_asnt_l3_send_msg(pc, -1);
	}
}

static void _asnt_nins_userinfo_req(layer3_proc_t *pc, int pr, void *arg)
{
	USER_INFORMATION_t *ui = arg;

	if (ui)
	{
		_asnt_l3_msg_start(pc, MT_USER_INFORMATION);

		if (ui->USER_USER)
			_asnt_l3_add_var_IE(pc, IE_USER_USER, ui->USER_USER);
		else
			return;
		
		_asnt_l3_send_msg(pc, -1);
	}
}

static void _asnt_nins_information_req(layer3_proc_t *pc, int pr, void *arg)
{
	INFORMATION_t *info = arg;

	if (info)
	{
		_asnt_l3_msg_start(pc, MT_INFORMATION);

		if (info->COMPLETE)
			*pc->op++ = IE_COMPLETE;

		if (info->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, info->DISPLAY);

		if (info->KEYPAD)
			_asnt_l3_add_var_IE(pc, IE_KEYPAD, info->KEYPAD);

		if (info->SIGNAL)
			_asnt_l3_add_var_IE(pc, IE_SIGNAL, info->SIGNAL);

		if (info->CALLED_PN)
			_asnt_l3_add_var_IE(pc, IE_CALLED_PN, info->CALLED_PN);

		_asnt_l3_send_msg(pc, -1);
	}
}

static void _asnt_nins_progress_req(layer3_proc_t *pc, int pr, void *arg)
{
	PROGRESS_t *prog = arg;

	if (prog)
	{
		_asnt_l3_msg_start(pc, MT_INFORMATION);

		if (prog->BEARER)
			_asnt_l3_add_var_IE(pc, IE_BEARER, prog->BEARER);

		if (prog->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, prog->CAUSE);

		if (prog->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, prog->FACILITY);

		if (prog->PROGRESS)
			_asnt_l3_add_var_IE(pc, IE_PROGRESS, prog->PROGRESS);
		else
			return;

		if (prog->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, prog->DISPLAY);

		if (prog->HLC)
			_asnt_l3_add_var_IE(pc, IE_HLC, prog->HLC);
//#warning ETSI 300286-1 only define USER_USER for USER_INFORMATION SETUP ALERTING PROGRESS CONNECT DISCONNECT RELEASE*
//		if (prog->USER_USER)
//			_asnt_l3_add_var_IE(pc, IE_USER_USER, prog->USER_USER);

		_asnt_l3_send_msg(pc, -1);
	}
}

static void _asnt_nins_notify_req(layer3_proc_t *pc, int pr, void *arg)
{
	NOTIFY_t *noti = arg;

	if (noti)
	{
		_asnt_l3_msg_start(pc, MT_INFORMATION);
		
		if (noti->BEARER)
			_asnt_l3_add_var_IE(pc, IE_BEARER, noti->BEARER);
		
		if (noti->NOTIFY)
			_asnt_l3_add_var_IE(pc, IE_NOTIFY, noti->NOTIFY);
		else
			return;
		
		if (noti->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, noti->DISPLAY);
		
		if (noti->REDIR_DN)
			_asnt_l3_add_var_IE(pc, IE_REDIR_DN, noti->REDIR_DN);
		
		_asnt_l3_send_msg(pc, -1);
	}
}

static void _asnt_nins_disconnect_req_out(layer3_proc_t *pc, int pr, void *arg)
{
	DISCONNECT_t	*disc = arg;
	int		cause;

	if (pc->master)
	{ /* child */
		_asnt_nins_disconnect_req_out(pc->master, pr, arg);
		return;
	}
	asnt_l3_del_timer(&pc->timer1);

	if (disc)
	{
		if (disc->CAUSE)
		{
			cause = disc->CAUSE[2] & 0x7f;
		}
		else
		{
			cause = CAUSE_NORMALUNSPECIFIED;
		}
	}
	
	_asnt_l3_send_proc(pc, IMSG_RELEASE_CHILDS, &cause);
	if (test_bit(FLG_L3P_TIMER312, &pc->Flags))
	{
		_asnt_l3_new_l3_state(pc, 22);
	}
	else
	{
		if_link(pc->l3->nst, (ifunc_t)pc->l3->nst->l3_2_mgr,
			CC_RELEASE | CONFIRM, pc->ces |(pc->callref << 16), 0, NULL, 0);

		_asnt_l3_new_l3_state(pc, 0);
		if (!pc->child)
			_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
	}
}

static void _asnt_nins_release_req(layer3_proc_t *pc, int pr, void *arg)
{
	RELEASE_t *rel = arg;

	asnt_l3_stop_all_l3_timer(pc);
	if (rel)
	{
		_asnt_l3_msg_start(pc, MT_RELEASE);

		if (rel->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, rel->CAUSE);

		if (rel->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, rel->FACILITY);

		if (rel->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rel->DISPLAY);

		if (rel->USER_USER)
			_asnt_l3_add_var_IE(pc, IE_USER_USER, rel->USER_USER);

		_asnt_l3_send_msg(pc, 19);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 19);
		_asnt_nins_message(pc, MT_RELEASE);
	}
	test_and_clear_bit(FLG_L3P_TIMER308_1, &pc->Flags);
	asnt_l3_add_timer(&pc->timer1, T308, 0x308);
}

static void _asnt_nins_release_cmpl_req(layer3_proc_t *pc, int pr, void *arg)
{
	RELEASE_COMPLETE_t *rcmpl = arg;

	asnt_l3_stop_all_l3_timer(pc);
	
	if (rcmpl)
	{
		_asnt_l3_msg_start(pc, MT_RELEASE_COMPLETE);

		if (rcmpl->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, rcmpl->CAUSE);

		if (rcmpl->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, rcmpl->FACILITY);

		if (rcmpl->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rcmpl->DISPLAY);

		if (rcmpl->USER_USER) 
			_asnt_l3_add_var_IE(pc, IE_USER_USER, rcmpl->USER_USER);
		_asnt_l3_send_msg(pc, 0);
	}
	else
	{
		_asnt_l3_new_l3_state(pc, 0);
		_asnt_nins_message(pc, MT_RELEASE_COMPLETE);
	}
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

static void _asnt_nins_t302(layer3_proc_t *pc, int pr, void *arg)
{
	{
		int t = 0x302;

		asnt_l3_stop_all_l3_timer(pc);
		if_link(pc->l3->nst, (ifunc_t)pc->l3->nst->l3_2_mgr,
			CC_TIMEOUT | INDICATION,pc->ces | (pc->callref << 16), sizeof(int), &t, 0);
	}
}

static void _asnt_nins_t303(layer3_proc_t *pc, int pr, void *arg)
{
	int			l;
	msg_t			*msg;
	RELEASE_COMPLETE_t	*relc;

	asnt_l3_del_timer(&pc->timer1);
	
	if (test_bit(FLG_L3P_GOTRELCOMP, &pc->Flags))
	{
		asnt_l3_stop_all_l3_timer(pc);
		msg = prep_l3data_msg(CC_RELEASE_COMPLETE | INDICATION,
			pc->ces | (pc->callref << 16), sizeof(RELEASE_COMPLETE_t), 3, NULL);

		if (!msg)
			return;

		relc = (RELEASE_COMPLETE_t *)(msg->data + ASNT_ISDNUSER_HEAD_SIZE);
		_asnt_l3_new_l3_state(pc, 0);
		relc->CAUSE = msg_put(msg, 3);
		relc->CAUSE[0] = 2;
		relc->CAUSE[1] = 0x80;

		if (pc->cause)
			relc->CAUSE[2] = pc->cause | 0x80;
		else
			relc->CAUSE[2] = CAUSE_NORMALUNSPECIFIED | 0x80;

		if (_asnt_l3_up(pc, msg))
			as_msg_free(msg);
		_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
		return;
	}
	
	if (!test_and_set_bit(FLG_L3P_TIMER303_1, &pc->Flags))
	{
		if (pc->obuf[3] == MT_SETUP)
		{
			l = pc->op - &pc->obuf[0];
			dhexprint(DBGM_L3DATA, "l3 oframe:", &pc->obuf[0], l);

			if ((msg = l3_alloc_msg(l)))
			{
				memcpy(msg_put(msg, l), &pc->obuf[0], l);
				if (_asnt_l3_msg(pc->l3, DL_UNITDATA | REQUEST, 127, msg))
					as_msg_free(msg);
			}

			asnt_l3_del_timer(&pc->timer2);
			dprint(DBGM_L3, "%s: pc=%p del timer2\n", __FUNCTION__, pc);
			asnt_l3_add_timer(&pc->timer2, T312, 0x312);

			test_and_set_bit(FLG_L3P_TIMER312, &pc->Flags);
			asnt_l3_add_timer(&pc->timer1, T303, 0x303);
			return;
		}
	}
	
	msg = prep_l3data_msg(CC_RELEASE_COMPLETE | INDICATION,
		pc->ces | (pc->callref << 16), sizeof(RELEASE_COMPLETE_t), 3, NULL);
	if (!msg)
		return;
	
	relc = (RELEASE_COMPLETE_t *)(msg->data + ASNT_ISDNUSER_HEAD_SIZE);
	relc->CAUSE = msg_put(msg, 3);
	relc->CAUSE[0] = 2;
	relc->CAUSE[1] = 0x85;
	relc->CAUSE[2] = CAUSE_NOUSER_RESPONDING | 0x80;

	if (_asnt_l3_up(pc, msg))
		as_msg_free(msg);
	_asnt_l3_new_l3_state(pc, 22);
}

static void _asnt_nins_t308(layer3_proc_t *pc, int pr, void *arg)
{
	if (!test_and_set_bit(FLG_L3P_TIMER308_1, &pc->Flags))
	{
		_asnt_l3_new_l3_state(pc, 19);
		asnt_l3_del_timer(&pc->timer1);
		_asnt_nins_message(pc, MT_RELEASE);
		asnt_l3_add_timer(&pc->timer1, T308, 0x308);
	}
	else
	{
		int t = 0x308;

		asnt_l3_stop_all_l3_timer(pc);
		_asnt_l3_new_l3_state(pc, 0);
		if_link(pc->l3->nst, (ifunc_t)pc->l3->nst->l3_2_mgr,
			CC_TIMEOUT | INDICATION,pc->ces | (pc->callref << 16), sizeof(int), &t, 0);
		_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
	}
}

static void _asnt_nins_t312(layer3_proc_t *pc, int pr, void *arg)
{
	int t = 0x312;

	test_and_clear_bit(FLG_L3P_TIMER312, &pc->Flags);
	asnt_l3_del_timer(&pc->timer2);
	dprint(DBGM_L3, "%s: pc=%p del timer2\n", __FUNCTION__, pc);
	_asnt_l3_debug(pc->l3, "%s: state %d", __FUNCTION__, pc->state);
	
	if (pc->state == 22)
	{
		asnt_l3_stop_all_l3_timer(pc);
		if (!pc->child)
		{
			if_link(pc->l3->nst, (ifunc_t)pc->l3->nst->l3_2_mgr,
				CC_TIMEOUT | INDICATION,pc->ces |(pc->callref << 16), sizeof(int), &t, 0);
			_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
		}
	}
}

static void _asnt_nins_holdack_req(layer3_proc_t *pc, int pr, void *arg)
{
	HOLD_ACKNOWLEDGE_t *hack = arg;

	if (pc->hold_state != HOLDAUX_HOLD_IND)
		return;
	pc->hold_state = HOLDAUX_HOLD; 
	if (hack)
	{
		_asnt_l3_msg_start(pc, MT_HOLD_ACKNOWLEDGE);
		if (hack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, hack->DISPLAY);
		_asnt_l3_send_msg(pc, -1);
	}
	else
	{
		_asnt_nins_message(pc, MT_HOLD_ACKNOWLEDGE);
	}
}

static void _asnt_nins_holdrej_req(layer3_proc_t *pc, int pr, void *arg)
{
	HOLD_REJECT_t *hrej = arg;

	if (pc->hold_state != HOLDAUX_HOLD_IND)
		return;
	pc->hold_state = HOLDAUX_IDLE; 
	_asnt_l3_msg_start(pc, MT_HOLD_REJECT);

	if (hrej)
	{
		if (hrej->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, hrej->CAUSE);
		else
		{
			*pc->op++ = IE_CAUSE;
			*pc->op++ = 2;
			*pc->op++ = 0x80;
			*pc->op++ = 0x80 | 0x47;
		}
		if (hrej->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, hrej->DISPLAY);
	}
	else
	{
		*pc->op++ = IE_CAUSE;
		*pc->op++ = 2;
		*pc->op++ = 0x80;
		*pc->op++ = 0x80 | 0x47;
	}
	_asnt_l3_send_msg(pc, -1);
}

static void _asnt_nins_retrack_req(layer3_proc_t *pc, int pr, void *arg)
{
	RETRIEVE_ACKNOWLEDGE_t *rack = arg;

	if (pc->hold_state != HOLDAUX_RETR_IND)
		return;
	pc->hold_state = HOLDAUX_IDLE;
	
	if (rack)
	{
		_asnt_l3_msg_start(pc, MT_RETRIEVE_ACKNOWLEDGE);

		if (rack->CHANNEL_ID)
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, rack->CHANNEL_ID);

		if (rack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rack->DISPLAY);

		_asnt_l3_send_msg(pc, -1);
	}
	else
	{
		_asnt_nins_message(pc, MT_RETRIEVE_ACKNOWLEDGE);
	}
}

static void _asnt_nins_retrrej_req(layer3_proc_t *pc, int pr, void *arg)
{
	RETRIEVE_REJECT_t *rrej = arg;

	if (pc->hold_state != HOLDAUX_RETR_IND)
		return;
	pc->hold_state = HOLDAUX_HOLD; 
	_asnt_l3_msg_start(pc, MT_RETRIEVE_REJECT);
	
	if (rrej)
	{
		if (rrej->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, rrej->CAUSE);
		else
		{
			*pc->op++ = IE_CAUSE;
			*pc->op++ = 2;
			*pc->op++ = 0x80;
			*pc->op++ = 0x80 | 0x47;
		}

		if (rrej->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rrej->DISPLAY);
	}
	else
	{
		*pc->op++ = IE_CAUSE;
		*pc->op++ = 2;
		*pc->op++ = 0x80;
		*pc->op++ = 0x80 | 0x47;
	}
	_asnt_l3_send_msg(pc, -1);
}

static void _asnt_nins_suspack_req(layer3_proc_t *pc, int pr, void *arg)
{
	SUSPEND_ACKNOWLEDGE_t *sack = arg;

	asnt_l3_stop_all_l3_timer(pc);
	if (sack)
	{
		_asnt_l3_msg_start(pc, MT_SUSPEND_ACKNOWLEDGE);

		if (sack->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, sack->FACILITY);

		if (sack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, sack->DISPLAY);

		_asnt_l3_send_msg(pc, 0);
	}
	else
	{
		_asnt_nins_message(pc, MT_SUSPEND_ACKNOWLEDGE);
	}
	_asnt_l3_new_l3_state(pc, 0);
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

static void _asnt_nins_susprej_req(layer3_proc_t *pc, int pr, void *arg)
{
	SUSPEND_REJECT_t *srej = arg;

	_asnt_l3_msg_start(pc, MT_SUSPEND_REJECT);
	if (srej)
	{
		if (srej->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, srej->CAUSE);
		else
		{
			*pc->op++ = IE_CAUSE;
			*pc->op++ = 2;
			*pc->op++ = 0x80;
			*pc->op++ = 0x80 | 0x47;
		}
		if (srej->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, srej->DISPLAY);
	}
	else
	{
		*pc->op++ = IE_CAUSE;
		*pc->op++ = 2;
		*pc->op++ = 0x80;
		*pc->op++ = 0x80 | 0x47;
	}
	_asnt_l3_send_msg(pc, -1);
	_asnt_l3_new_l3_state(pc, 10);
}

static void _asnt_nins_resack_req(layer3_proc_t *pc, int pr, void *arg)
{
	RESUME_ACKNOWLEDGE_t *rack = arg;

	asnt_l3_stop_all_l3_timer(pc);
	if (rack)
	{
		_asnt_l3_msg_start(pc, MT_RESUME_ACKNOWLEDGE);
		if (rack->CHANNEL_ID)
			_asnt_l3_add_var_IE(pc, IE_CHANNEL_ID, rack->CHANNEL_ID);
		if (rack->FACILITY)
			_asnt_l3_add_var_IE(pc, IE_FACILITY, rack->FACILITY);
		if (rack->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rack->DISPLAY);
		_asnt_l3_send_msg(pc, 0);
	}
	else
	{
		_asnt_nins_message(pc, MT_RESUME_ACKNOWLEDGE);
	}
	_asnt_l3_new_l3_state(pc, 10);
}

static void _asnt_nins_resrej_req(layer3_proc_t *pc, int pr, void *arg)
{
	RESUME_REJECT_t *rrej = arg;

	_asnt_l3_msg_start(pc, MT_RESUME_REJECT);
	if (rrej)
	{
		if (rrej->CAUSE)
			_asnt_l3_add_var_IE(pc, IE_CAUSE, rrej->CAUSE);
		else
		{
			*pc->op++ = IE_CAUSE;
			*pc->op++ = 2;
			*pc->op++ = 0x80;
			*pc->op++ = 0x80 | 0x47;
		}
		if (rrej->DISPLAY)
			_asnt_l3_add_var_IE(pc, IE_DISPLAY, rrej->DISPLAY);
	}
	else
	{
		*pc->op++ = IE_CAUSE;
		*pc->op++ = 2;
		*pc->op++ = 0x80;
		*pc->op++ = 0x80 | 0x47;
	}
	_asnt_l3_send_msg(pc, -1);
	_asnt_l3_new_l3_state(pc, 0);
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

/* *INDENT-OFF* */
struct stateentry downstatelist[] =
{
#if 0
	{SBIT(0),
	 CC_RESUME | REQUEST, _asnt_nins_resume_req},
	{SBIT(12),
	 CC_RELEASE | REQUEST, _asnt_nins_release_req},
	{ALL_STATES,
	 CC_RESTART | REQUEST, _asnt_nins_restart},
	{SBIT(6) | SBIT(7) | SBIT(9) | SBIT(25),
	 CC_CONNECT | REQUEST, _asnt_nins_connect_req},
	{SBIT(10),
	 CC_SUSPEND | REQUEST, _asnt_nins_suspend_req},
#endif
	{ALL_STATES, 	CC_RELEASE_COMPLETE | REQUEST, 		_asnt_nins_release_cmpl_req},
	{SBIT(0),		CC_SETUP | REQUEST, 					_asnt_nins_setup_req},
	{SBIT(1),		CC_SETUP_ACKNOWLEDGE | REQUEST, 	_asnt_nins_setup_ack_req},
	{SBIT(1) | SBIT(2),
					CC_PROCEEDING | REQUEST, 			_asnt_nins_proceed_req},
	{SBIT(2) | SBIT(3),
					CC_ALERTING | REQUEST, 				_asnt_nins_alert_req},
	{SBIT(2) | SBIT(3) | SBIT(4),
					CC_CONNECT | REQUEST, 				_asnt_nins_connect_req},
#if 0
	/* comment this line, and changed as following */
	{SBIT(8),		CC_CONNECT | RESPONSE, 				_asnt_nins_connect_res},
#endif
	{SBIT(8),		CC_CONNECT_ACKNOWLEDGE | REQUEST, 				_asnt_nins_connect_res},
	{SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10),
					CC_DISCONNECT | REQUEST, 			_asnt_nins_disconnect_req},
	{SBIT(6) | SBIT(7) | SBIT(8) | SBIT(9) | SBIT(25),
	 				CC_DISCONNECT | REQUEST, 			_asnt_nins_disconnect_req_out},
	{SBIT(11)
/*
#warning bitte beachte folgendes:
*/
/*
es ist nur erlaubt, im state 11 einen release zu schicken!
dennoch verwende der stack den release scheinbar, um einen prozess
zu releasen, wie es z.b. in _asnt_nins_disconnect_req_out geschieht.
der process befindet sich zu diesem zeitpunk noch im state 7, 9 oder 25.
wenn man den (Layer 4) state auf 11 ändern würde, braucht mann die folgende
zeile nicht: (bitte nachdenken, ob dies korrekt ist)
Nein glaube ich nicht. CC_RELEASE |= CC_RELEASE_CR muss aber mal ein paar Tests
machen
*/
	| SBIT(12) | SBIT(7) | SBIT(9) | SBIT(25),
					CC_RELEASE | REQUEST, 			_asnt_nins_release_req},
/* 
#warning noch ein bug: wenn ein CC_DISCONNECT gesendet wird (state 7 = klingeling), dann bekommt man nur einen RELEASE_CR, aber keinen vorherigen RELEASE 
*/
/* muss ich auch testen, keine Zeit */
	{ALL_STATES,		CC_FACILITY | REQUEST, 			_asnt_nins_facility_req},
	{SBIT(4) | SBIT(7) | SBIT(8) | SBIT(10),
					CC_USER_INFORMATION | REQUEST, 	_asnt_nins_userinfo_req},
	{SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10) | SBIT(11) | SBIT(12) | SBIT(25),
					CC_INFORMATION | REQUEST, 		_asnt_nins_information_req},
	{SBIT(2) | SBIT(3) | SBIT(4),
					CC_PROGRESS | REQUEST, 			_asnt_nins_progress_req},
	{SBIT(10) | SBIT(15),
					CC_NOTIFY | REQUEST, 			_asnt_nins_notify_req},
	{SBIT(2),
					CC_T302, 						_asnt_nins_t302},
	{SBIT(6),		CC_T303, 						_asnt_nins_t303},
	{SBIT(19),		CC_T308, 						_asnt_nins_t308},
	{ALL_STATES,		CC_T312, 						_asnt_nins_t312},
	{SBIT(3) | SBIT(4) | SBIT(10) | SBIT(12),
					CC_HOLD_ACKNOWLEDGE | REQUEST, _asnt_nins_holdack_req},
	{SBIT(3) | SBIT(4) | SBIT(10) | SBIT(12),
					CC_HOLD_REJECT | REQUEST, 		_asnt_nins_holdrej_req},
	{SBIT(3) | SBIT(4) | SBIT(10) | SBIT(12),
					CC_RETRIEVE_ACKNOWLEDGE | REQUEST, _asnt_nins_retrack_req},
	{SBIT(3) | SBIT(4) | SBIT(10) | SBIT(12),
					CC_RETRIEVE_REJECT | REQUEST, 		_asnt_nins_retrrej_req},
	{SBIT(15),
					CC_SUSPEND_ACKNOWLEDGE | REQUEST, _asnt_nins_suspack_req},
	{SBIT(15),
					CC_SUSPEND_REJECT | REQUEST, 		_asnt_nins_susprej_req},
	{SBIT(17),		CC_RESUME_ACKNOWLEDGE | REQUEST, 	_asnt_nins_resack_req},
	{SBIT(17),		CC_RESUME_REJECT | REQUEST, 		_asnt_nins_resrej_req},
};

#define DOWNSLLEN \
	(sizeof(downstatelist) / sizeof(struct stateentry))

void  asnt_request_l3_state(struct  _l3_msg *l3m, layer3_proc_t *proc)
{
	int i;

	for (i = 0; i <DOWNSLLEN; i++)
	{
		if ((l3m->mt == downstatelist[i].primitive) && ((1 << proc->state) & downstatelist[i].state))
			break;
	}	

	if (i == DOWNSLLEN)
	{
		if (proc->l3->debug & L3_DEB_STATE)
		{
			_asnt_l3_debug(proc->l3, "NINS state %d L4 %#x unhandled", proc->state, l3m->mt);
		}
		
	}
	else
	{
		if (proc->l3->debug & L3_DEB_STATE)
		{
			_asnt_l3_debug(proc->l3, "NINS state %d L4 %x", proc->state, l3m->mt);
		}
		
		if (l3m->msg)
			downstatelist[i].rout(proc, l3m->mt, l3m->msg->data);
		else
			downstatelist[i].rout(proc, l3m->mt, NULL);
	}

}

