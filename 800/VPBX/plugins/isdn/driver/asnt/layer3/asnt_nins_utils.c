/*
* $Id: asnt_nins_utils.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

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


/* utils handler for L3 msg from/to Layer2, except DL_DATA_IND  */
/* #warning l2_state makes no sense in multipoint environment. shouldnt we use something like l2_state[ces] ?*/
int _asnt_l3_msg(ASNT_L3 *l3, u_int pr, int dinfo, void *arg)
{
	msg_t	*msg = arg, *lmsg = NULL;
/*#warning testing */

	int	ces = dinfo & 0xffff;

	dprint(DBGM_L3, "%s: pr(%x) di(%x) arg(%p)\n", __FUNCTION__, pr, dinfo, arg);
	switch (pr)
	{
		case (DL_UNITDATA | REQUEST):
			return(_asnt_l3_down(l3, pr, dinfo, arg));
		case (DL_DATA | REQUEST):
			if (l3->l2_state0 == ST_L3_LC_ESTAB || ces > 0)
			{
				return(_asnt_l3_down(l3, pr, dinfo, arg));
			}
			else
			{
				if (ces == 0)
				{
					AS_ISDN_addhead(pr, dinfo, msg);
					msg_queue_tail(&l3->squeue0, msg);
					l3->l2_state0 = ST_L3_LC_ESTAB_WAIT;
					_asnt_l3_down(l3, DL_ESTABLISH | REQUEST, dinfo, NULL);
					return(0);
				}
			}
			break;
			
		case (DL_DATA | CONFIRM):
			break;
			
		case (DL_ESTABLISH | REQUEST):
			if (ces == 0)
			{
				if (l3->l2_state0 != ST_L3_LC_ESTAB)
				{
					_asnt_l3_down(l3, pr, dinfo, NULL);
					l3->l2_state0 = ST_L3_LC_ESTAB_WAIT;
				}
			}
			break;
			
		case (DL_ESTABLISH | CONFIRM):
			if (ces == 0)
			{
				if (l3->l2_state0 != ST_L3_LC_REL_WAIT)
				{
					l3->l2_state0 = ST_L3_LC_ESTAB;
					_asnt_l3_send_squeue(l3, &l3->squeue0);
				}
			}
			
			if (!l3->nst->l3_2_mgr)
				break;
			
			if (!(lmsg = create_link_msg(pr, dinfo, 0, NULL, 0)))
				break;
			
			if (l3->nst->l3_2_mgr(l3->nst, lmsg))
				as_msg_free(lmsg);
			break;
			
		case (DL_ESTABLISH | INDICATION):
			if (ces == 0)
			{
				if (l3->l2_state0 == ST_L3_LC_REL)
				{
					l3->l2_state0 = ST_L3_LC_ESTAB;
					_asnt_l3_send_squeue(l3, &l3->squeue0);
				}
			}
			
			if (!l3->nst->l3_2_mgr)
				break;
			
			if (!(lmsg = create_link_msg(pr, dinfo, 0, NULL, 0)))
				break;
			
			if (l3->nst->l3_2_mgr(l3->nst, lmsg))
				as_msg_free(lmsg);
			break;
		case (DL_RELEASE | INDICATION):
/*
#warning du musst alle processe releasen CC_RELEASE!!! dies geschieht z.b. wenn man das telefon vom s0-bus abnimmt und der layer-2 dadurch zusammen bricht.
#warning geschieht dies auch im TE-mode?
#warning TODO DL_RELEASE | INDICATION handling; inclusiv special state 10 (T309)
*/
			if (ces == 0)
			{
				if (l3->l2_state0 == ST_L3_LC_ESTAB)
				{
					l3->l2_state0 = ST_L3_LC_REL;
				}
			}
			
			if (!l3->nst->l3_2_mgr)
				break;
			if (!(lmsg = create_link_msg(pr, dinfo, 0, NULL, 0)))
				break;
			if (l3->nst->l3_2_mgr(l3->nst , lmsg))
				as_msg_free(lmsg);
			
			_asnt_l3_remove_proc(&l3->proc, dinfo);
			break;
			
		case (DL_RELEASE | CONFIRM):
			if (ces == 0)
			{
				if (l3->l2_state0 == ST_L3_LC_REL_WAIT) 
				{
					l3->l2_state0 = ST_L3_LC_REL;
				}
			}
			
			if (!l3->nst->l3_2_mgr)
				break;
			
			if (!(lmsg = create_link_msg(pr, dinfo, 0, NULL, 0)))
				break;
			
			if (l3->nst->l3_2_mgr(l3->nst, lmsg))
				as_msg_free(lmsg);
			_asnt_l3_remove_proc(&l3->proc, dinfo);
			break;

		case (DL_RELEASE | REQUEST):
			if (ces == 0)
			{
				if (l3->l2_state0 == ST_L3_LC_ESTAB)
				{
					_asnt_l3_down(l3, pr, dinfo, NULL);
					l3->l2_state0 = ST_L3_LC_REL_WAIT;
				}
			}
			break;
	}
	
	if (msg)
		as_msg_free(msg);
	return(0);
}

void asnt_l3_display_NR_IE(u_char *p, char *head1, char *head2)
{
	int len;
	char	txt[128];
	char	*tp = txt;

	len = *p++;
	tp += sprintf(tp, "len(%d)", len);
	if (len)
	{
		len--;
		tp += sprintf(tp, " plan(%x)", *p);
		if (len && !(*p & 0x80))
		{
			len--;
			p++;
			tp += sprintf(tp, " pres(%x)", *p);
		}
		p++;
		tp += sprintf(tp, " ");
		while(len--)
			tp += sprintf(tp, "%c", *p++);
	}
	dprint(DBGM_L3, "%s%s %s\n", head1, head2, txt);
}

void _asnt_l3_debug(ASNT_L3 *l3, char *fmt, ...)
{
	va_list args;
	char buf[256], *p;

	va_start(args, fmt);
	p = buf;
	p += sprintf(p, "l3 ");
	p += vsprintf(p, fmt, args);
	va_end(args);
	dprint(DBGM_L3, "%s\n", buf);
}

int _asnt_l3_get_callref(u_char *p)
{
	int l, cr = 0;

	p++;			/* prot discr */
	l = 0xf & *p++;	/* callref length */
	
	if (l > 2)			/* wrong callref only 1 or 2 octet*/
		return(-2);
	if (!l)			/* dummy CallRef */
		return(-1);
	
	if (l == 1)
	{/* BRI */
		cr = *p & 0x7f;
		cr += (*p & 0x80) << 8;
	}
	else
	{/* PRI */
		cr = *p++ << 8;
		cr += *p;
	}

	return (cr);
}
                                                                                        
void _asnt_l3_new_l3_state(layer3_proc_t *pc, int state)
{
	if (pc->l3->debug & L3_DEB_STATE)
		_asnt_l3_debug(pc->l3, "newstate cr %d %d%s --> %d%s", pc->callref & 0x7FFF, 
		pc->state, pc->master ? "i" : "", state, pc->master ? "i" : "");
	pc->state = state;
}

static void asnt_l3_expire_timer(L3Timer_t *t)
{
	if (t->pc->l3->debug & L3_DEB_STATE)
		_asnt_l3_debug(t->pc->l3, "timer %p nr %x expired", t, t->nr);
	_asnt_l3_send_proc(t->pc, IMSG_TIMER_EXPIRED, &t->nr);
}

void asnt_l3_init_timer(layer3_proc_t *pc, L3Timer_t *t)
{
	t->pc = pc;
	t->tl.function = (void *) asnt_l3_expire_timer;
	t->tl.data = (long) t;
	asnt_timer_init(&t->tl, pc->l3->nst);
}

void asnt_l3_del_timer(L3Timer_t *t)
{
	asnt_timer_del(&t->tl);
}

int asnt_l3_add_timer(L3Timer_t *t, int millisec, int timer_nr)
{
	if (asnt_timer_pending(&t->tl))
	{
		dprint(DBGM_L3, "asnt_l3_add_timer: timer already active!\n");
		return -1;
	}
	
	asnt_timer_init(&t->tl, t->pc->l3->nst);
	t->nr = timer_nr;
	t->tl.expires = millisec;
	asnt_timer_add(&t->tl);
	return 0;
}

void asnt_l3_stop_all_l3_timer(layer3_proc_t *pc)
{
	asnt_l3_del_timer(&pc->timer1);
	asnt_l3_del_timer(&pc->timer2);
	dprint(DBGM_L3, "%s: pc=%p del timer2\n", __FUNCTION__, pc);
#if 0	
#warning also remove flags:
#endif
	test_and_clear_bit(FLG_L3P_TIMER303_1, &pc->Flags);
	test_and_clear_bit(FLG_L3P_TIMER308_1, &pc->Flags);
	test_and_clear_bit(FLG_L3P_TIMER312, &pc->Flags);
}

void asnt_l3_remove_all_l3_timer(layer3_proc_t *pc)
{
	int ret;
	
	ret = asnt_timer_remove(&pc->timer1.tl);
	if (ret)
		dprint(DBGM_L3, "RemoveL3Timer1: ret %d\n", ret);
			TRACE;
	
	ret = asnt_timer_remove(&pc->timer2.tl);
	dprint(DBGM_L3, "%s: pc=%p del timer2\n", __FUNCTION__, pc);
			TRACE;

	if (ret)
		dprint(DBGM_L3, "RemoveL3Timer2: ret %d\n", ret);
#if 0	
#warning also remove flags:
#endif
	test_and_clear_bit(FLG_L3P_TIMER303_1, &pc->Flags);
	test_and_clear_bit(FLG_L3P_TIMER308_1, &pc->Flags);
	test_and_clear_bit(FLG_L3P_TIMER312, &pc->Flags);
}

layer3_proc_t *_asnt_l3_create_proc(ASNT_L3 *l3, int ces, int cr, layer3_proc_t *master)
{
	layer3_proc_t	*l3p;

	l3p = malloc(sizeof(layer3_proc_t));
	if (l3p)
	{
		memset(l3p, 0, sizeof(layer3_proc_t));
		l3p->l3 = l3;
		l3p->ces = ces;
		l3p->callref = cr;
		l3p->master = master;
		asnt_l3_init_timer(l3p, &l3p->timer1);
		asnt_l3_init_timer(l3p, &l3p->timer2);
		if (master)
		{
			APPEND_TO_LIST(l3p, master->child);
		}

		dprint(DBGM_L3, "NEW PROC: ces : %x CallRef : %x\n", l3p->ces, l3p->callref );
	}	
	return(l3p);
}

layer3_proc_t *_asnt_l3_find_proc(layer3_proc_t *master, int ces, int cr)
{
	layer3_proc_t	*p = master;
	layer3_proc_t	*cp;

	dprint(DBGM_L3, "%s: ces(%x) cr(%x)\n", __FUNCTION__, ces, cr);
	
	while(p)
	{
		dprint(DBGM_L3, "%s: proc %p ces(%x) cr(%x)\n", __FUNCTION__,p, p->ces, p->callref);
		if ((p->ces == ces) && (p->callref == cr))
			break;
		if (p->child)
		{
			cp = _asnt_l3_find_proc(p->child, ces, cr);
			if (cp)
				return(cp);
		}
		
		if (((p->ces & 0xffffff00) == 0xff00) && (p->callref == cr))
			break;
		p = p->next;
	}
	return(p);
}

u_char *asnt_l3_find_IE(u_char * p, int size, u_char ie, int wanted_set)
{
	int l, codeset, maincodeset;
	u_char *pend = p + size;

	/* skip protocol discriminator, callref and message type */
	p++;
	l = (*p++) & 0xf;
	p += l;
	p++;
	codeset = 0;
	maincodeset = 0;
	/* while there are bytes left... */
	while (p < pend)
	{
		if ((*p & 0xf0) == 0x90)
		{
			codeset = *p & 0x07;
			if (!(*p & 0x08))
				maincodeset = codeset;
		}
		
		if (codeset == wanted_set)
		{
			if (*p == ie) 
			{
				/* improved length check (Werner Cornelius) */
				if (!(*p & 0x80))
				{
					if ((pend - p) < 2)
						return(NULL);
					if (*(p+1) > (pend - (p+2)))
						return(NULL);
					p++; /* points to len */
				}
				
				return (p);
			}
			else if ((*p > ie) && !(*p & 0x80))
				return (NULL);
		}
		
		if (!(*p & 0x80))
		{
			p++;
			l = *p;
			p += l;
			codeset = maincodeset;
		}
		p++;
	}
	return (NULL);
}

u_char *asnt_l3_find_and_copy_IE(u_char * p, int size, u_char ie, int wanted_set, msg_t *msg)
{
	u_char *iep, *mp;
	int	l;

	iep = asnt_l3_find_IE(p, size, ie, wanted_set);
	if (iep)
	{
		l = 1;
		if (!(ie & 0x80))
			l += *iep;
		mp = msg_put(msg, l);
		memcpy(mp, iep, l);
		iep = mp;
	}
	return(iep);
}

void _asnt_l3_msg_start(layer3_proc_t *pc, u_char mt)
{
	pc->op = &pc->obuf[0];
	*pc->op++ = 8;
	
	if (pc->callref == -1)
	{ /* dummy cr */
		*pc->op++ = 0;
	}
	else
	{
		if (pc->l3->nst->feature & FEATURE_NET_CRLEN2)
		{
			*pc->op++ = 2;
			*pc->op++ = (pc->callref >> 8) ^ 0x80;
			*pc->op++ = pc->callref & 0xff;
		}
		else
		{
			*pc->op++ = 1;
			*pc->op = pc->callref & 0x7f;
			if (!(pc->callref & 0x8000))
				*pc->op |= 0x80;
			pc->op++;
		}
	}
	*pc->op++ = mt;
}

void _asnt_l3_add_var_IE(layer3_proc_t *pc, u_char ie, u_char *iep)
{
	u_char len = *iep;

	*pc->op++ = ie;
	*pc->op++ = *iep++;
	while(len--)
		*pc->op++ = *iep++;	
}

int _asnt_l3_send_msg(layer3_proc_t *pc, int state)
{
	int l;
	int ret;
	msg_t *msg;

	l = pc->op - &pc->obuf[0];
	if (!(msg = l3_alloc_msg(l)))
		return(-ENOMEM);

	memcpy(msg_put(msg, l), &pc->obuf[0], l);
	dhexprint(DBGM_L3DATA, "l3 oframe:", &pc->obuf[0], l);
	if (state != -1)
		_asnt_l3_new_l3_state(pc, state);

	if ((ret = _asnt_l3_msg(pc->l3, DL_DATA | REQUEST, pc->ces, msg)))
		as_msg_free(msg);
	return(ret);
}

int _asnt_nins_message(layer3_proc_t *pc, u_char mt)
{
	msg_t *msg;
	u_char *p;
	int ret;

	if (!(msg = l3_alloc_msg(4)))
		return(-ENOMEM);
	p = msg_put(msg, 4);
	*p++ = 8;	/* protocol discriminator */
	
	if (pc->l3->nst->feature & FEATURE_NET_CRLEN2)
	{/* PRI */
		*p++ = 2;
		*p++ = (pc->callref >> 8) ^ 0x80;
		*p++ = pc->callref & 0xff;
	}
	else
	{/* BRI */
		*p++ = 1;
		*p = pc->callref & 0x7f;
		
		/* this msg is send to the side that originate this call reference*/
		if (!(pc->callref & 0x8000))
			*p |= 0x80;
		p++;
	}

	*p++ = mt;
	dhexprint(DBGM_L3DATA, "l3 oframe:", msg->data, 4);
	if ((ret=_asnt_l3_msg(pc->l3, DL_DATA | REQUEST, pc->ces, msg)))
		as_msg_free(msg);
	return(ret);
}

void _asnt_nins_message_cause(layer3_proc_t *pc, u_char mt, u_char cause)
{
	_asnt_l3_msg_start(pc, mt);
	if (cause)
	{
		*pc->op++ = IE_CAUSE;
		*pc->op++ = 0x2;
		*pc->op++ = 0x80 | CAUSE_LOC_PNET_LOCUSER;
		*pc->op++ = 0x80 | cause;
	}
	_asnt_l3_send_msg(pc, -1); 
}

static void _asnt_nins_status_send(layer3_proc_t *pc, u_char cause)
{

	_asnt_l3_msg_start(pc, MT_STATUS);
	*pc->op++ = IE_CAUSE;
	*pc->op++ = 2;
	*pc->op++ = 0x80 | CAUSE_LOC_USER;
	*pc->op++ = 0x80 | cause;

	*pc->op++ = IE_CALL_STATE;
	*pc->op++ = 1;
	*pc->op++ = pc->state & 0x3f;
	_asnt_l3_send_msg(pc, -1); 
}

void _asnt_nins_msg_without_setup(layer3_proc_t *pc, u_char cause)
{
	/* This routine is called if here was no SETUP made (checks in NINSup and in
	 * _asnt_nins_setup) and a RELEASE_COMPLETE have to be sent with an error code
	 * MT_STATUS_ENQUIRE in the NULL state is handled too
	 */
	switch (cause)
	{
		case 81:		/* invalid callreference */
		case 88:		/* incomp destination */
		case 96:		/* mandory IE missing */
		case 100:       /* invalid IE contents */
		case 101:	/* incompatible Callstate */
			_asnt_nins_message_cause(pc, MT_RELEASE_COMPLETE, cause);
			break;
		default:
			dprint(DBGM_L3, "ISDN _asnt_nins_msg_without_setup wrong cause %d\n", cause);
	}
	_asnt_l3_send_proc(pc, IMSG_END_PROC, NULL);
}

#if 0
static int _asnt_nins_check_messagetype_validity(layer3_proc_t *pc, int mt, void *arg)
{
	switch (mt)
	{
		case MT_ALERTING:
		case MT_CALL_PROCEEDING:
		case MT_CONNECT:
		case MT_CONNECT_ACKNOWLEDGE:
		case MT_DISCONNECT:
		case MT_INFORMATION:
		case MT_FACILITY:
		case MT_NOTIFY:
		case MT_PROGRESS:
		case MT_RELEASE:
		case MT_RELEASE_COMPLETE:
		case MT_SETUP:
		case MT_SETUP_ACKNOWLEDGE:
		case MT_RESUME_ACKNOWLEDGE:
		case MT_RESUME_REJECT:
		case MT_SUSPEND_ACKNOWLEDGE:
		case MT_SUSPEND_REJECT:
		case MT_USER_INFORMATION:
		case MT_RESTART:
		case MT_RESTART_ACKNOWLEDGE:
		case MT_CONGESTION_CONTROL:
		case MT_STATUS:
		case MT_STATUS_ENQUIRY:
		case MT_HOLD:
		case MT_RETRIEVE:
		case MT_RESUME: /* RESUME only in user->net */
		case MT_SUSPEND: /* SUSPEND only in user->net */
			if (pc->l3->debug & L3_DEB_CHECK)
				_asnt_l3_debug(pc->l3, "_asnt_nins_check_messagetype_validity mt(%x) OK", mt);
			break;
		default:
			if (pc->l3->debug & (L3_DEB_CHECK | L3_DEB_WARN))
				_asnt_l3_debug(pc->l3, "_asnt_nins_check_messagetype_validity mt(%x) fail", mt);
			_asnt_nins_status_send(pc, CAUSE_MT_NOTIMPLEMENTED);
			return(1);
	}
	return(0);
}
#endif

void _asnt_nins_std_ie_err(layer3_proc_t *pc, int ret)
{

	if (pc->l3->debug & L3_DEB_CHECK)
		_asnt_l3_debug(pc->l3, "check_infoelements ret %d", ret);

	switch(ret)
	{
		case 0: 
			break;
			
		case ERR_IE_COMPREHENSION:
			_asnt_nins_status_send(pc, CAUSE_MANDATORY_IE_MISS);
			break;
			
		case ERR_IE_UNRECOGNIZED:
			_asnt_nins_status_send(pc, CAUSE_IE_NOTIMPLEMENTED);
			break;
			
		case ERR_IE_LENGTH:
			_asnt_nins_status_send(pc, CAUSE_INVALID_CONTENTS);
			break;
			
		case ERR_IE_SEQUENCE:
		default:
			break;
	}
}

u_char *_asnt_nins_get_channel_id(layer3_proc_t *pc, msg_t *omsg, msg_t *nmsg)
{
	u_char	*sp, *p;
	int	l;

	if ((sp = p = asnt_l3_find_IE(omsg->data, omsg->len, IE_CHANNEL_ID, 0)))
	{
		l = *p++;
		if (pc->l3->nst->feature & FEATURE_NET_EXTCID)
		{ /* PRI */
			if (l < 3)
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "wrong chid len %d", *p);
				pc->err = -2;
				return (NULL);
			}

			if ((*p & 0x60) != 0x20)
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "wrong chid %x (for PRI interface)", *p);
				pc->err = -3;
				return (NULL);
			}
			p++;
			if (*p & 0x10)
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "wrong chid %x (channel map not supported)", *p);
				pc->err = -4;
				return (NULL);
			}
			p++;
			pc->bc = *p & 0x7f;
		}
		else
		{ /* BRI */
			if (l < 1)
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "wrong chid len %d", *p);
				pc->err = -2;
				return (NULL);
			}

			if (*p & 0x60)
			{
				if (pc->l3->debug & L3_DEB_WARN)
					_asnt_l3_debug(pc->l3, "wrong chid %x", *p);
				pc->err = -3;
				return (NULL);
			}
			pc->bc = *p & 3;
		}
		
		p = sp;
		sp = msg_put(nmsg, 1 + *p);
		memcpy(sp, p, 1 + *p);
	}
	else
		pc->err = -1;

	return(sp);
}

u_char *_asnt_nins_get_cause(layer3_proc_t *pc, msg_t *omsg, msg_t *nmsg)
{
	u_char l;
	u_char *p, *sp;

	if ((sp = p = asnt_l3_find_IE(omsg->data, omsg->len, IE_CAUSE, 0)))
	{
		l = *p++;
		if (l>30)
		{
			pc->err = 1;
			return(NULL);
		}

		if (l)
			l--;
		else
		{
			pc->err = 2;
			return(NULL);
		}

		if (l && !(*p & 0x80))
		{
			l--;
			p++; /* skip recommendation */
		}
		p++;

		if (l)
		{
			if (!(*p & 0x80))
			{
				pc->err = 3;
				return(NULL);
			}
			pc->err = *p & 0x7F;
		}
		else
		{
			pc->err = 4;
			return(NULL);
		}

		if (nmsg)
		{
			p = sp;
			sp = msg_put(nmsg, 1 + *p);
			memcpy(sp, p, 1 + *p);
		}
	}
	else
		pc->err = -1;

	return(sp);
}


