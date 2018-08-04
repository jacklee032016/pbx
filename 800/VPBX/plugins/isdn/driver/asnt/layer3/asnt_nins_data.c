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

static void _asnt_nins_facility(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg,*msg = arg;
	FACILITY_t	*fac;

	umsg = prep_l3data_msg(CC_FACILITY | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(FACILITY_t), msg->len, NULL);
	if (!umsg)
		return;
	fac = (FACILITY_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	
	fac->FACILITY =asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_userinfo(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t			*umsg,*msg = arg;
	USER_INFORMATION_t	*ui;

	umsg = prep_l3data_msg(CC_USER_INFORMATION | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(USER_INFORMATION_t), msg->len, NULL);
	if (!umsg)
		return;
	
	ui = (USER_INFORMATION_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	ui->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);
	
	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_setup(layer3_proc_t *pc, int pr, void *arg)
{
	u_char	*p;
	int	bcfound = 0;
	msg_t	*umsg,*msg = arg;
	int	err = 0;
	SETUP_t	*setup;
	
	umsg = prep_l3data_msg(CC_SETUP | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(SETUP_t), msg->len, NULL);
	if (!umsg)
		return;
	
	setup = (SETUP_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	/*
	 * Bearer Capabilities
	 */
	/* only the first occurence 'll be detected ! */
	if ((p = setup->BEARER = asnt_l3_find_and_copy_IE(msg->data, msg->len,
		IE_BEARER, 0, umsg)))
	{
		if ((p[0] < 2) || (p[0] > 11))
			err = 1;
		else
		{
			switch (p[1] & 0x7f)
			{
				case 0x00: /* Speech */
				case 0x10: /* 3.1 Khz audio */
				case 0x08: /* Unrestricted digital information */
				case 0x09: /* Restricted digital information */
				case 0x11:
					/* Unrestr. digital information  with 
					 * tones/announcements ( or 7 kHz audio
					 */
				case 0x18: /* Video */
					break;
				default:
					err = 2;
					break;
			}

			switch (p[2] & 0x7f)
			{
				case 0x40: /* packed mode */
				case 0x10: /* 64 kbit */
				case 0x11: /* 2*64 kbit */
				case 0x13: /* 384 kbit */
				case 0x15: /* 1536 kbit */
				case 0x17: /* 1920 kbit */
					break;
				default:
					err = 3;
					break;
			}
		}
		
		if (err)
		{
			if (pc->l3->debug & L3_DEB_WARN)
				_asnt_l3_debug(pc->l3, "setup with wrong bearer(l=%d:%x,%x)", p[0], p[1], p[2]);

			_asnt_nins_msg_without_setup(pc, CAUSE_INVALID_CONTENTS);
			as_msg_free(umsg);
			return;
		} 
	}
	else
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "setup without bearer capabilities");
		/* ETS 300-104 1.3.3 */
		_asnt_nins_msg_without_setup(pc, CAUSE_MANDATORY_IE_MISS);
		as_msg_free(umsg);
		return;
	}

	/* Channel Identification  */
	if ((setup->CHANNEL_ID = _asnt_nins_get_channel_id(pc, msg, umsg)))
	{
		if (pc->bc)
		{
			bcfound++;
		}
		else
		{
			if (pc->l3->debug & L3_DEB_WARN)
				_asnt_l3_debug(pc->l3, "setup without bchannel, call waiting");
			bcfound++;
		} 
	}
	else if (pc->err != -1)
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "setup with wrong chid ret %d", pc->err);
	}
	/* Now we are on none mandatory IEs */

	setup->COMPLETE = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_COMPLETE, 0, umsg);
	setup->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	setup->PROGRESS = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);
	setup->NET_FAC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_NET_FAC, 0, umsg);
	setup->KEYPAD = 	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_KEYPAD, 0, umsg);
	setup->SIGNAL = 	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	setup->CALLED_PN = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALLED_PN, 0, umsg);
	setup->CALLED_SUB =	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALLED_SUB, 0, umsg);
	setup->CALLING_PN = 	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALLING_PN, 0, umsg);
	setup->CALLING_SUB = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALLING_SUB, 0, umsg);
	setup->REDIR_NR = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_REDIR_NR, 0, umsg);
	setup->LLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_LLC, 0, umsg);
	setup->HLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_HLC, 0, umsg);
	setup->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);
	
	setup->ces = pc->ces;
	_asnt_l3_new_l3_state(pc, 1);
	asnt_l3_del_timer(&pc->timer2);

	dprint(DBGM_L3, "%s: pc=%p del timer2\n", __FUNCTION__, pc);
	asnt_l3_add_timer(&pc->timer2, T_CTRL, 0x31f);
	if (err) /* STATUS for none mandatory IE errors after actions are taken */
		_asnt_nins_std_ie_err(pc, err);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}


static void _asnt_nins_disconnect(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg,*msg = arg;
	DISCONNECT_t	*disc;

	umsg = prep_l3data_msg(CC_DISCONNECT | INDICATION, pc->ces |(pc->callref << 16), sizeof(DISCONNECT_t), msg->len, NULL);
	if (!umsg)
		return;
	
	disc = (DISCONNECT_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_stop_all_l3_timer(pc);
	_asnt_l3_new_l3_state(pc, 11);

	if (!(disc->CAUSE = _asnt_nins_get_cause(pc, msg, umsg)))
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "DISC get_cause ret(%d)", pc->err);
	} 

	disc->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	disc->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	disc->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_disconnect_i(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg,*msg = arg;
	DISCONNECT_t	*disc;
	u_char		cause = 0;

	umsg = prep_l3data_msg(CC_DISCONNECT | INDICATION, pc->ces |(pc->callref << 16), sizeof(DISCONNECT_t), msg->len, NULL);
	if (!umsg)
		return;

	disc = (DISCONNECT_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_stop_all_l3_timer(pc);
	
	if (!(disc->CAUSE = _asnt_nins_get_cause(pc, msg, umsg)))
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "DISC get_cause ret(%d)", pc->err);

		if (pc->err<0)
			cause = CAUSE_MANDATORY_IE_MISS;
		else if (pc->err>0)
			cause = CAUSE_INVALID_CONTENTS;
	}
	
	disc->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	disc->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	disc->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	if (cause)
		_asnt_nins_message_cause(pc, MT_RELEASE, cause);
	else
		_asnt_nins_message(pc, MT_RELEASE);

	_asnt_l3_new_l3_state(pc, 19);
	test_and_clear_bit(FLG_L3P_TIMER308_1, &pc->Flags);
	asnt_l3_add_timer(&pc->timer1, T308, 0x308);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

void _asnt_nins_information(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t 		*umsg, *msg = arg;
	INFORMATION_t	*info;

	umsg = prep_l3data_msg(CC_INFORMATION | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(INFORMATION_t), msg->len, NULL);
	if (!umsg)
		return;

	info = (INFORMATION_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	info->COMPLETE = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_COMPLETE, 0, umsg);
	info->KEYPAD = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_KEYPAD, 0, umsg);
	info->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	info->CALLED_PN = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALLED_PN, 0, umsg);

	if (pc->state == 2) 
	{ /* overlap receiving */
		asnt_l3_del_timer(&pc->timer1);
		asnt_l3_add_timer(&pc->timer1, T302, 0x302);
	}

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

void _asnt_nins_release(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	RELEASE_t	*rel;
	int 		cause=0;

	umsg = prep_l3data_msg(CC_RELEASE | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(RELEASE_t), msg->len, NULL);
	if (!umsg)
		return;

	rel = (RELEASE_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_stop_all_l3_timer(pc);

	if (!(rel->CAUSE = _asnt_nins_get_cause(pc, msg, umsg)))
	{
		if (pc->state != 12)
			if (pc->l3->debug & L3_DEB_WARN)
				_asnt_l3_debug(pc->l3, "REL get_cause ret(%d)", pc->err);
			
		if ((pc->err<0) && (pc->state != 12))
			cause = CAUSE_MANDATORY_IE_MISS;
		else if (pc->err>0)
			cause = CAUSE_INVALID_CONTENTS;
	}
	rel->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	rel->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	rel->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	if (cause)
		_asnt_nins_message_cause(pc, MT_RELEASE_COMPLETE, cause);
	else
		_asnt_nins_message(pc, MT_RELEASE_COMPLETE);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);

	_asnt_l3_new_l3_state(pc, 0);
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

static void _asnt_nins_release_i(layer3_proc_t *pc, int pr, void *arg)
{
	_asnt_nins_message(pc, MT_RELEASE_COMPLETE);
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

void _asnt_nins_release_cmpl(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t			*umsg, *msg = arg;
	RELEASE_COMPLETE_t	*relc;

	umsg = prep_l3data_msg(CC_RELEASE_COMPLETE | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(RELEASE_COMPLETE_t), msg->len, NULL);
	if (!umsg)
		return;

	relc = (RELEASE_COMPLETE_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_stop_all_l3_timer(pc);
	_asnt_l3_new_l3_state(pc, 0);

	if (!(relc->CAUSE = _asnt_nins_get_cause(pc, msg, umsg)))
	{
		if (pc->err > 0)
			if (pc->l3->debug & L3_DEB_WARN)
				_asnt_l3_debug(pc->l3, "RELCMPL get_cause err(%d)", pc->err);
	}
	
	relc->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	relc->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	relc->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);

	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

static void _asnt_nins_release_cmpl_i(layer3_proc_t *pc, int pr, void *arg)
{
	_asnt_l3_send_proc(pc, IMSG_END_PROC_M, NULL);
}

static void _asnt_nins_setup_acknowledge_i(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t			*umsg, *msg = arg;
	SETUP_ACKNOWLEDGE_t	*sa;

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
#if 0	
	if (!pc->master)
	{
		asnt_l3_del_timer(&pc->timer1);
		_asnt_l3_new_l3_state(pc, 25);
		return;
	}
	umsg = prep_l3data_msg(CC_SETUP_ACKNOWLEDGE | INDICATION, pc->master->ces |
		(pc->master->callref << 16), sizeof(SETUP_ACKNOWLEDGE_t), msg->len, NULL);
#endif

	umsg = prep_l3data_msg(CC_SETUP_ACKNOWLEDGE | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(SETUP_ACKNOWLEDGE_t), msg->len, NULL);
	if (!umsg)
		return;

	sa = (SETUP_ACKNOWLEDGE_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_del_timer(&pc->timer1);	/* T304 */
	_asnt_l3_new_l3_state(pc, 25);

	sa->CHANNEL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CHANNEL_ID, 0, umsg);
	sa->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	sa->PROGRESS =	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);

//	if (!_asnt_l3_up(pc->master, umsg))
	if (!_asnt_l3_up(pc, umsg))
		return;
	as_msg_free(umsg);
}

static void _asnt_nins_proceeding_i(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t			*umsg, *msg = arg;
	CALL_PROCEEDING_t	*proc;

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
#if 0	
	if (!pc->master)
	{
		asnt_l3_del_timer(&pc->timer1);
		_asnt_l3_new_l3_state(pc, 9);
		return;
	}
	umsg = prep_l3data_msg(CC_PROCEEDING | INDICATION, pc->master->ces |
		(pc->master->callref << 16), sizeof(CALL_PROCEEDING_t), msg->len, NULL);
#endif

	umsg = prep_l3data_msg(CC_PROCEEDING | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(CALL_PROCEEDING_t), msg->len, NULL);
	if (!umsg)
		return;

	proc = (CALL_PROCEEDING_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_del_timer(&pc->timer1);	/* T304 */
	_asnt_l3_new_l3_state(pc, 9);
	proc->CHANNEL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CHANNEL_ID, 0, umsg);
	proc->BEARER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_BEARER, 0, umsg);
	proc->FACILITY =	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	proc->PROGRESS = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);
	proc->HLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_HLC, 0, umsg);

//	if (!_asnt_l3_up(pc->master, umsg))
	if (!_asnt_l3_up(pc, umsg))
		return;

	as_msg_free(umsg);
}

static void _asnt_nins_alerting_i(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	ALERTING_t	*al;

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
#if 0	
	if (!pc->master)
	{
		asnt_l3_del_timer(&pc->timer1);
		_asnt_l3_new_l3_state(pc, 7);
		return;
	}

	umsg = prep_l3data_msg(CC_ALERTING | INDICATION, pc->master->ces |
		(pc->master->callref << 16), sizeof(ALERTING_t), msg->len, NULL);
#endif
	umsg = prep_l3data_msg(CC_ALERTING | INDICATION, pc->ces |(pc->callref << 16), sizeof(ALERTING_t), msg->len, NULL);
	if (!umsg)
		return;

	al = (ALERTING_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_del_timer(&pc->timer1);	/* T304 */
	TRACE;
	_asnt_l3_new_l3_state(pc, 7);
TRACE;
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	al->CHANNEL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CHANNEL_ID, 0, umsg);
	al->BEARER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_BEARER, 0, umsg);
	al->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	al->PROGRESS = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);
	al->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	al->HLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_HLC, 0, umsg);

	al->USER_USER =	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
#if 0	
	if (!_asnt_l3_up(pc->master, umsg))
#endif
	if (!_asnt_l3_up(pc, umsg))
		return;
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	as_msg_free(umsg);
}

#if 0
static void _asnt_nins_call_proc(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t			*umsg, *msg = arg;
	int			ret = 0;
	u_char			cause;
	CALL_PROCEEDING_t	*cp;

	umsg = prep_l3data_msg(CC_PROCEEDING | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(CALL_PROCEEDING_t), msg->len, NULL);
	if (!umsg)
		return;
	
	cp = (CALL_PROCEEDING_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);

	if ((cp->CHANNEL_ID = _asnt_nins_get_channel_id(pc, msg, umsg)))
	{
		if (!(pc->l3->nst->feature & FEATURE_NET_EXTCID))
		{ /* BRI */
			if ((0 == pc->bc) || (3 == pc->bc))
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "setup answer with wrong chid %x", pc->bc);
				_asnt_nins_status_send(pc, CAUSE_INVALID_CONTENTS);
				as_msg_free(umsg);
				return;
			}
		}
	}
	else if (1 == pc->state)
	{
		if (pc->l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(pc->l3, "setup answer wrong chid (ret %d)", pc->err);
		if (pc->err == -1)
			cause = CAUSE_MANDATORY_IE_MISS;
		else
			cause = CAUSE_INVALID_CONTENTS;
		_asnt_nins_status_send(pc, cause);
		as_msg_free(umsg);
		return;
	}
	/* Now we are on none mandatory IEs */
	cp->BEARER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_BEARER, 0, umsg);
	cp->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	cp->PROGRESS = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);
	cp->DISPLAY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_DISPLAY, 0, umsg);
	cp->HLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_HLC, 0, umsg);

	asnt_l3_del_timer(&pc->timer1);
	_asnt_l3_new_l3_state(pc, 3);
	asnt_l3_add_timer(&pc->timer1, T310, 0x310);

	if (ret) /* STATUS for none mandatory IE errors after actions are taken */
		_asnt_nins_std_ie_err(pc, ret);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}
#endif

static void _asnt_nins_connect_i(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	CONNECT_t	*conn;

#if 0	
	if (!pc->master)
	{
		asnt_l3_del_timer(&pc->timer1);
		_asnt_l3_new_l3_state(pc, 8);
		return;
	}

	umsg = prep_l3data_msg(CC_CONNECT | INDICATION, pc->master->ces |
		(pc->master->callref << 16), sizeof(CONNECT_t), msg->len, NULL);
#endif
	umsg = prep_l3data_msg(CC_CONNECT | INDICATION, pc->ces |(pc->callref << 16), sizeof(CONNECT_t), msg->len, NULL);
	if (!umsg)
		return;

	conn = (CONNECT_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	asnt_l3_del_timer(&pc->timer1);	/* T310 */
	_asnt_l3_new_l3_state(pc, 8);

	conn->BEARER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_BEARER, 0, umsg);
	conn->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);
	conn->PROGRESS = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_PROGRESS, 0, umsg);
	conn->DISPLAY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_DISPLAY, 0, umsg);
	conn->DATE = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_DATE, 0, umsg);
	conn->SIGNAL = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_SIGNAL, 0, umsg);
	conn->CONNECT_PN = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CONNECT_PN, 0, umsg);
	conn->CONNECT_SUB = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CONNECT_SUB, 0, umsg);
	conn->HLC = 	asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_HLC, 0, umsg);
	conn->LLC = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_LLC, 0, umsg);
	conn->USER_USER = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_USER_USER, 0, umsg);

	conn->ces = pc->ces;

	if (_asnt_l3_send_proc(pc, IMSG_CONNECT_IND, umsg))
		as_msg_free(umsg); 
}

static void _asnt_nins_hold(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	HOLD_t		*hold;

	if (!(pc->l3->nst->feature & FEATURE_NET_HOLD))
	{
		_asnt_nins_message_cause(pc, MT_HOLD_REJECT, CAUSE_MT_NOTIMPLEMENTED);
		return;
	}
	dprint(DBGM_L3,"%s\n", __FUNCTION__);
#if 0	
#warning TODO: global mask for supported none mandatory services, like HOLD
#endif
	if (pc->hold_state == HOLDAUX_HOLD_IND)
		return;
	if (pc->hold_state != HOLDAUX_IDLE)
	{
		_asnt_nins_message_cause(pc, MT_HOLD_REJECT, CAUSE_NOTCOMPAT_STATE);
		return;
	}
	pc->hold_state = HOLDAUX_HOLD_IND; 

	umsg = prep_l3data_msg(CC_HOLD | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(HOLD_t), msg->len, NULL);
	if (!umsg)
		return;

	hold = (HOLD_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_retrieve(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	RETRIEVE_t	*retr;

	if (!(pc->l3->nst->feature & FEATURE_NET_HOLD))
	{
		_asnt_nins_message_cause(pc, MT_RETRIEVE_REJECT, CAUSE_MT_NOTIMPLEMENTED);
		return;
	}
	dprint(DBGM_L3,"%s\n", __FUNCTION__);

	if (pc->hold_state == HOLDAUX_RETR_IND)
		return;
	if (pc->hold_state != HOLDAUX_HOLD)
	{
		_asnt_nins_message_cause(pc, MT_RETRIEVE_REJECT, CAUSE_NOTCOMPAT_STATE);
		return;
	}
	pc->hold_state = HOLDAUX_RETR_IND;

	umsg = prep_l3data_msg(CC_RETRIEVE | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(RETRIEVE_t), msg->len, NULL);
	if (!umsg)
		return;

	retr = (RETRIEVE_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	retr->CHANNEL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CHANNEL_ID, 0, umsg);

	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_suspend(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	SUSPEND_t	*susp;

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	umsg = prep_l3data_msg(CC_SUSPEND | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(SUSPEND_t), msg->len, NULL);
	if (!umsg)
		return;

	susp = (SUSPEND_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	susp->CALL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALL_ID, 0, umsg);
	susp->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);

	_asnt_l3_new_l3_state(pc, 15);
	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_resume(layer3_proc_t *pc, int pr, void *arg)
{
	msg_t		*umsg, *msg = arg;
	RESUME_t	*res;

	dprint(DBGM_L3,"%s\n", __FUNCTION__);
	umsg = prep_l3data_msg(CC_RESUME | INDICATION, pc->ces |
		(pc->callref << 16), sizeof(RESUME_t), msg->len, NULL);
	if (!umsg)
		return;

	res = (RESUME_t *)(umsg->data + ASNT_ISDNUSER_HEAD_SIZE);
	res->CALL_ID = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_CALL_ID, 0, umsg);
	res->FACILITY = asnt_l3_find_and_copy_IE(msg->data, msg->len, IE_FACILITY, 0, umsg);

	res->ces = pc->ces;
	_asnt_l3_new_l3_state(pc, 17);
	
	if (_asnt_l3_up(pc, umsg))
		as_msg_free(umsg);
}

static void _asnt_nins_status_enq(layer3_proc_t *proc, int pr, void *arg)
{
}

struct stateentry datastatelist[] =
{
	{ALL_STATES, 		MT_STATUS_ENQUIRY, 		_asnt_nins_status_enq},
	{ALL_STATES,			MT_FACILITY, 			_asnt_nins_facility},
	{SBIT(19),			MT_STATUS, 				_asnt_nins_release_cmpl},
	{SBIT(0),			MT_SETUP, 				_asnt_nins_setup},
/*		
#warning setup ack
*/
	{SBIT(6) | SBIT(7)  | SBIT(9) | SBIT(25),
						MT_SETUP_ACKNOWLEDGE,	_asnt_nins_setup_acknowledge_i},
	{SBIT(6) | SBIT(7)  | SBIT(9) | SBIT(25),
						MT_CALL_PROCEEDING, 	_asnt_nins_proceeding_i},
	{SBIT(6) | SBIT(7)  | SBIT(9) | SBIT(25),
						MT_ALERTING, 			_asnt_nins_alerting_i},
	{SBIT(6) | SBIT(7)  | SBIT(9) | SBIT(25),
						MT_CONNECT, 			_asnt_nins_connect_i},
	{SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10),
						MT_DISCONNECT, 			_asnt_nins_disconnect},
	{SBIT(7) | SBIT(8) | SBIT(9),
						MT_DISCONNECT, 			_asnt_nins_disconnect_i},
	{SBIT(2) | SBIT(3) | SBIT(4) | SBIT(7) | SBIT(8) | SBIT(9) | SBIT(10) |
	 SBIT(11) | SBIT(12) | SBIT(15) | SBIT(17) | SBIT(19) | SBIT(25),
						MT_INFORMATION, 		_asnt_nins_information},
	{SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(6) | SBIT(10) |
	 SBIT(11) | SBIT(12) | SBIT(15) | SBIT(17),
						MT_RELEASE, 				_asnt_nins_release},
	{SBIT(7) | SBIT(8) | SBIT(9) | SBIT(19) | SBIT(25),
						MT_RELEASE, 				_asnt_nins_release_i},
	{SBIT(0) | SBIT(1) | SBIT(2) | SBIT(3) | SBIT(4) | SBIT(10) |
	 SBIT(11) | SBIT(12) | SBIT(15) | SBIT(17),
						MT_RELEASE_COMPLETE, 	_asnt_nins_release_cmpl},
	{SBIT(4) | SBIT(7) | SBIT(10),
						MT_USER_INFORMATION, 	_asnt_nins_userinfo},
	{SBIT(7) | SBIT(8) | SBIT(9) | SBIT(19) | SBIT(25),
						MT_RELEASE_COMPLETE, 	_asnt_nins_release_cmpl_i},
	{SBIT(3) | SBIT(4) | SBIT(10),
						MT_HOLD, 				_asnt_nins_hold},
	{SBIT(3) | SBIT(4) | SBIT(10) | SBIT(12),
						MT_RETRIEVE, 			_asnt_nins_retrieve},
	{SBIT(10),			MT_SUSPEND, 			_asnt_nins_suspend},
	{SBIT(0),			MT_RESUME, 				_asnt_nins_resume},
};

#define DATASLLEN \
	(sizeof(datastatelist) / sizeof(struct stateentry))


void  asnt_data_l3_state(struct _l3_msg *l3m, layer3_proc_t *proc)
{
	int i;

	for (i = 0; i < DATASLLEN; i++)
	{
		if ((l3m->mt == datastatelist[i].primitive) &&
			((1 << proc->state) & datastatelist[i].state))
		break;
	}	

	if (i == DATASLLEN)
	{
		if (proc->l3->debug & L3_DEB_STATE)
		{
			_asnt_l3_debug(proc->l3, "NINS state %d mt %#x unhandled",proc->state, l3m->mt);
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
			_asnt_l3_debug(proc->l3, "NINS state %d mt %x",proc->state, l3m->mt);
		}
		datastatelist[i].rout(proc, l3m->mt, l3m->msg);
	}

}

