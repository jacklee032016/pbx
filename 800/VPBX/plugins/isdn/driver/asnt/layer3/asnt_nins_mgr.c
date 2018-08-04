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

void  asnt_data_l3_state(struct _l3_msg *l3m, layer3_proc_t *proc);
void  asnt_master_l3_state(struct _l3_msg *l3m, layer3_proc_t *proc);
void  asnt_request_l3_state(struct  _l3_msg *l3m, layer3_proc_t *proc);
layer3_proc_t *_asnt_l3_find_proc(layer3_proc_t *master, int ces, int cr);
void asnt_l3_remove_all_l3_timer(layer3_proc_t *pc);

static int _asnt_l3_imsg_intrelease(layer3_proc_t *master, layer3_proc_t *child)
{
	int	cause;

	if ((!master) || (!child))
		return(-EINVAL);
	
	dprint(DBGM_L3, "%s: m/c(%x/%x) state(%d/%d) m->c(%p)\n", __FUNCTION__,
		master->ces, child->ces, master->state, child->state, master->child);

	switch (master->state)
	{
		case 0:
			if (!master->child)
			{
				_asnt_l3_send_proc(master, IMSG_END_PROC, master);
			}
			break;
		case 6:
		case 10:
		case 19:
			break;
		case 7:
		case 9:
		case 25:
			if (master->child ||test_bit(FLG_L3P_TIMER312, &master->Flags))
			{
				dprint(DBGM_L3, "%s: JOLLY child=%p, flg=%d\n", __FUNCTION__, master->child, test_bit(FLG_L3P_TIMER312, &master->Flags));
/* 
#warning TODO: save cause
#warning bedenke auch, dass vielleicht overlap sending mit information-messages praktisch wäre (später PTP)
*/
			}
			else
			{
				_asnt_l3_send_proc(master, IMSG_END_PROC, NULL);
			}
			break;
			
		case 8:
			if (master->selces == child->ces)
			{
				cause = CAUSE_NONSELECTED_USER;
				_asnt_l3_send_proc(master, IMSG_RELEASE_CHILDS, &cause);
				if (test_bit(FLG_L3P_TIMER312, &master->Flags))
				{
					_asnt_l3_new_l3_state(master, 22);
				}
				else
				{
					if (!master->child)
						_asnt_l3_send_proc(master, IMSG_END_PROC, NULL);
				}
			}
			break;

		case 22:
			if (!master->child)
			{
				_asnt_l3_send_proc(master, IMSG_END_PROC, NULL);
			}
			break;
	}
	return(0);
}

/* send out a l3_msg in this L3 transaction process : important */
int _asnt_l3_send_proc(layer3_proc_t *proc, int op, void *arg)
{
	int		i;
	layer3_proc_t	*selp;
	struct _l3_msg	*l3m = arg;
	struct _l3_msg	l3msg;

	if (proc->l3->debug & L3_DEB_PROC)
		_asnt_l3_debug(proc->l3, "%s: proc(CES:%x,CallRef:%x) op(%d)", __FUNCTION__,	proc->ces, proc->callref, op);  
	
	switch(op)
	{
		case IMSG_END_PROC:
		case IMSG_END_PROC_M:
			/* case 1:  end a PROC */
			asnt_l3_remove_all_l3_timer(proc);
			TRACE;
			if (!proc->master && !arg)
			{
				if_link(proc->l3->nst, (ifunc_t)proc->l3->nst->l3_2_mgr,
					CC_RELEASE_CR | INDICATION,	proc->ces | (proc->callref << 16), sizeof(int), &proc->err, 0);
			}
			TRACE;
			
			while (proc->child)
				_asnt_l3_send_proc(proc->child, IMSG_END_PROC, NULL);
			
			TRACE;
			if (proc->next)
				proc->next->prev = proc->prev;
			if (proc->prev)
				proc->prev->next = proc->next;
			if (proc == proc->l3->proc)
				proc->l3->proc = proc->next;

			TRACE;
			if (proc->master)
			{
				if (proc->master->child == proc)
					proc->master->child = proc->next;
				if (op == IMSG_END_PROC_M)
					_asnt_l3_imsg_intrelease(proc->master, proc);
			}
			TRACE;
			free(proc);
			break;
			
		case IMSG_L2_DATA:
			/* case 2 : L2 data */
			asnt_data_l3_state(l3m, proc);
			break;
			
		case IMSG_MASTER_L2_DATA:
			/* Case 3 : L2 Master data */
			asnt_master_l3_state(l3m, proc);
			break;
			
		case IMSG_TIMER_EXPIRED:
			/* Case 4 : Timer Expired , pass through */
			i = *((int *)arg);
			_asnt_l3_debug(proc->l3, "%s: timer %x", __FUNCTION__, i);
			l3m = &l3msg;
			l3m->mt = CC_TIMER | (i<<8);
			l3m->msg = NULL;
			
		case IMSG_L4_DATA:
			/* Case 5 : L4 data */
			asnt_request_l3_state(l3m, proc);
			break;
			
		case IMSG_CONNECT_IND:
			/* Case 6 : L4 data */
			selp = proc;
#if 0
			/* comment for not support proc->master */
			proc = proc->master;
#endif
			if (!proc)
				return(-EINVAL);
			proc->selces = selp->ces;
			_asnt_l3_new_l3_state(proc, 8);
			return(_asnt_l3_up(proc, arg));
			
		case IMSG_SEL_PROC:
			/* Case 7 : L4 data */
			selp = _asnt_l3_find_proc(proc->child, proc->selces, proc->callref);
			i = proc->selces | (proc->callref << 16);
			if_link(proc->l3->nst, (ifunc_t)proc->l3->nst->l3_2_mgr,
				CC_NEW_CR | INDICATION, proc->ces |(proc->callref << 16), sizeof(int), &i, 0);
			proc->ces = proc->selces;
			_asnt_l3_send_proc(selp, IMSG_END_PROC, NULL);
			break;
			
		case IMSG_RELEASE_CHILDS:
			/* Case 8 : L4 data */
			{
				RELEASE_t	*rel;
				char		cause[3];

				cause[0] = 2;
				cause[1] = CAUSE_LOC_PNET_LOCUSER | 0x80;
				cause[2] = *((int *)arg) | 0x80;
				l3msg.mt = CC_RELEASE | REQUEST;
				l3msg.msg = as_msg_alloc(sizeof(RELEASE_t));
				if (!l3msg.msg)
					return(-ENOMEM);
				
				rel = (RELEASE_t *)msg_put(l3msg.msg, sizeof(RELEASE_t));
				memset(rel, 0, sizeof(RELEASE_t));
				rel->CAUSE = cause;
				selp = proc->child;
				while(selp)
				{
					layer3_proc_t *next = selp->next;

					_asnt_l3_send_proc(selp, IMSG_L4_DATA, &l3msg);
					selp = next;
				}
				as_msg_free(l3msg.msg);
			}
			break;
	}
	return(0);
}

/* handler for DL_DATA_IND distribution */
static int _asnt_l3_dl_data_mux(ASNT_L3 *l3, iframe_t *frm , msg_t *msg)
{
	layer3_proc_t	*proc;
	int		ret = -EINVAL;
	int		cr;
	struct _l3_msg	l3m;

	if (!l3)
		return(ret);
	
	dprint(DBGM_L3, "%s: len(%d)\n", __FUNCTION__, msg->len);
	dhexprint(DBGM_L3DATA, "l3 iframe:", msg->data, msg->len);
	
	/************** check validation of msg ***************/
	if (msg->len < 3)
	{
		_asnt_l3_debug(l3, "NINS frame too short(%d)", msg->len);
		as_msg_free(msg);
		return(0);
	}
	
	if (msg->data[0] != PROTO_DIS_EURO)
	{ 
		if (l3->debug & L3_DEB_PROTERR)
		{
			_asnt_l3_debug(l3, "NINS%s unexpected discriminator %x message len %d", (frm->prim == (DL_DATA | INDICATION)) ? " " : "(broadcast) ", msg->data[0], msg->len);
		}
		as_msg_free(msg); 
		return(0);
	}
	
	dprint(DBGM_L3, "%s: protocol discriminator(%x)\n", __FUNCTION__, msg->data[0]);
	cr = _asnt_l3_get_callref(msg->data);
	dprint(DBGM_L3, "%s: cr(%x)\n", __FUNCTION__, cr);

	if (msg->len < ((msg->data[1] & 0x0f) + 3))
	{
		_asnt_l3_debug(l3, "NINS frame too short(%d)", msg->len);
		as_msg_free(msg);
		return(0);
	}

	l3m.msg = msg;
	l3m.mt = msg->data[msg->data[1] + 2];
	if (cr == -2)
	{  /* wrong Callref */
		if (l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(l3, "NINS wrong Callref");
		as_msg_free(msg);
		return(0);
	}
	else if (cr == -1)
	{  /* Dummy Callref */
//		if (l3m.mt == MT_FACILITY)
//			_asnt_nins_facility(l3->dummy, hh->prim, msg);
//		else if (l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(l3, "NINS dummy Callref (no facility msg)");
		as_msg_free(msg);
		return(0);
	}
	else if ((((msg->data[1] & 0x0f) == 1) && (0==(cr & 0x7f))) ||
		(((msg->data[1] & 0x0f) == 2) && (0==(cr & 0x7fff))))
	{/* Global CallRef */
		if (l3->debug & L3_DEB_STATE)
			_asnt_l3_debug(l3, "NINS Global CallRef");
//		global_handler(l3, l3m.mt, msg);
		as_msg_free(msg);
		return(0);
	}
	
	/***** handle for this msg */
	dprint(DBGM_L3, "%s: mt(%x)\n", __FUNCTION__, l3m.mt);
	proc = _asnt_l3_find_proc(l3->proc, frm->dinfo, cr);
	dprint(DBGM_L3, "%s: proc(%p)\n", __FUNCTION__, proc);

	if (!proc)
	{/* when proc is not found for this dinfo, then only MT_RESUME and MT_SETUP */
		if (l3m.mt == MT_SETUP || l3m.mt == MT_RESUME)
		{/* Setup/Resume creates a new transaction process */
			if (msg->data[2] & 0x80)
			{/* Setup/Resume with wrong CREF flag */
				if (l3->debug & L3_DEB_STATE)
					_asnt_l3_debug(l3, "NINS wrong CRef flag");
				as_msg_free(msg);
				return(0);
			}
			dprint(DBGM_L3, "%s: %s\n", __FUNCTION__, (l3m.mt==MT_SETUP)?"MT_SETUP":"MT_RESUME");

			/* Setup/Resume creates a new transaction process */
			if (!(proc = _asnt_l3_create_proc(l3, frm->dinfo, cr, NULL)))
			{	/* May be to answer with RELEASE_COMPLETE and
				 * CAUSE 0x2f "Resource unavailable", but this
				 * need a new_l3_process too ... arghh
				 */
				as_msg_free(msg);
				return(0);
			}
			
			dprint(DBGM_L3, "%s: proc(%p)\n", __FUNCTION__, proc);
			APPEND_TO_LIST(proc, l3->proc);
		}
		else
		{
			dprint(DBGM_L3, "%s: mt(%x) do not create proc\n", __FUNCTION__, l3m.mt);
/* #warning TODO: it happens that a response to an outgoing setup is received after connect of another terminal. in this case we must release.
*/
			as_msg_free(msg);
			return(0);
		}
	}

	if ((proc->ces & 0xffffff00) == 0xff00)
	{/* master of L2 */
		dprint(DBGM_L3, "%s: master state %d found\n", __FUNCTION__,	proc->state);
		msg_push(msg, ASNT_ISDNUSER_HEAD_SIZE);
		_asnt_l3_send_proc(proc, IMSG_MASTER_L2_DATA, &l3m);
	}
	else
		_asnt_l3_send_proc(proc, IMSG_L2_DATA, &l3m);

	as_msg_free(msg);
	return(0);
}

#if 0
/* handler for DL_DATA_IND distribution */
static int _asnt_l3_dl_data_mux(ASNT_L3 *l3, ASNT_ISDNUSER_HEADER *hh, msg_t *msg)
{
	layer3_proc_t	*proc;
	int		ret = -EINVAL;
	int		cr;
	struct _l3_msg	l3m;

	if (!l3)
		return(ret);
	
	dprint(DBGM_L3, "%s: len(%d)\n", __FUNCTION__, msg->len);
	dhexprint(DBGM_L3DATA, "l3 iframe:", msg->data, msg->len);
	
	/************** check validation of msg ***************/
	if (msg->len < 3)
	{
		_asnt_l3_debug(l3, "NINS frame too short(%d)", msg->len);
		as_msg_free(msg);
		return(0);
	}
	
	if (msg->data[0] != PROTO_DIS_EURO)
	{ 
		if (l3->debug & L3_DEB_PROTERR)
		{
			_asnt_l3_debug(l3, "NINS%s unexpected discriminator %x message len %d", (hh->prim == (DL_DATA | INDICATION)) ? " " : "(broadcast) ", msg->data[0], msg->len);
		}
		as_msg_free(msg); 
		return(0);
	}
	
	dprint(DBGM_L3, "%s: protocol discriminator(%x)\n", __FUNCTION__, msg->data[0]);
	cr = _asnt_l3_get_callref(msg->data);
	dprint(DBGM_L3, "%s: cr(%x)\n", __FUNCTION__, cr);

	if (msg->len < ((msg->data[1] & 0x0f) + 3))
	{
		_asnt_l3_debug(l3, "NINS frame too short(%d)", msg->len);
		as_msg_free(msg);
		return(0);
	}

	l3m.msg = msg;
	l3m.mt = msg->data[msg->data[1] + 2];
	if (cr == -2)
	{  /* wrong Callref */
		if (l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(l3, "NINS wrong Callref");
		as_msg_free(msg);
		return(0);
	}
	else if (cr == -1)
	{  /* Dummy Callref */
//		if (l3m.mt == MT_FACILITY)
//			_asnt_nins_facility(l3->dummy, hh->prim, msg);
//		else if (l3->debug & L3_DEB_WARN)
			_asnt_l3_debug(l3, "NINS dummy Callref (no facility msg)");
		as_msg_free(msg);
		return(0);
	}
	else if ((((msg->data[1] & 0x0f) == 1) && (0==(cr & 0x7f))) ||
		(((msg->data[1] & 0x0f) == 2) && (0==(cr & 0x7fff))))
	{/* Global CallRef */
		if (l3->debug & L3_DEB_STATE)
			_asnt_l3_debug(l3, "NINS Global CallRef");
//		global_handler(l3, l3m.mt, msg);
		as_msg_free(msg);
		return(0);
	}
	
	/***** handle for this msg */
	dprint(DBGM_L3, "%s: mt(%x)\n", __FUNCTION__, l3m.mt);
	proc = _asnt_l3_find_proc(l3->proc, hh->dinfo, cr);
	dprint(DBGM_L3, "%s: proc(%p)\n", __FUNCTION__, proc);

	if (!proc)
	{/* when proc is not found for this dinfo, then only MT_RESUME and MT_SETUP */
		if (l3m.mt == MT_SETUP || l3m.mt == MT_RESUME)
		{/* Setup/Resume creates a new transaction process */
			if (msg->data[2] & 0x80)
			{/* Setup/Resume with wrong CREF flag */
				if (l3->debug & L3_DEB_STATE)
					_asnt_l3_debug(l3, "NINS wrong CRef flag");
				as_msg_free(msg);
				return(0);
			}
			dprint(DBGM_L3, "%s: %s\n", __FUNCTION__, (l3m.mt==MT_SETUP)?"MT_SETUP":"MT_RESUME");

			/* Setup/Resume creates a new transaction process */
			if (!(proc = _asnt_l3_create_proc(l3, hh->dinfo, cr, NULL)))
			{	/* May be to answer with RELEASE_COMPLETE and
				 * CAUSE 0x2f "Resource unavailable", but this
				 * need a new_l3_process too ... arghh
				 */
				as_msg_free(msg);
				return(0);
			}
			
			dprint(DBGM_L3, "%s: proc(%p)\n", __FUNCTION__, proc);
			APPEND_TO_LIST(proc, l3->proc);
		}
		else
		{
			dprint(DBGM_L3, "%s: mt(%x) do not create proc\n", __FUNCTION__, l3m.mt);
/* #warning TODO: it happens that a response to an outgoing setup is received after connect of another terminal. in this case we must release.
*/
			as_msg_free(msg);
			return(0);
		}
	}

	if ((proc->ces & 0xffffff00) == 0xff00)
	{/* master of L2 */
		dprint(DBGM_L3, "%s: master state %d found\n", __FUNCTION__,	proc->state);
		msg_push(msg, ASNT_ISDNUSER_HEAD_SIZE);
		_asnt_l3_send_proc(proc, IMSG_MASTER_L2_DATA, &l3m);
	}
	else
		_asnt_l3_send_proc(proc, IMSG_L2_DATA, &l3m);

	as_msg_free(msg);
	return(0);
}
#endif

/* L2 to L3 Handler : distribute the msg to upper layer and processed by L3 */
static int _asnt_l3_muxer(NTSTACK *nst, msg_t *msg)
{
//	ASNT_ISDNUSER_HEADER	*hh;
	iframe_t	*frm;
	int		ret = -EINVAL;

//	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
	frm = (iframe_t *)msg->data;

	dprint(DBGM_L3, "%s: msg len(%d)\n", __FUNCTION__, msg->len);
//	dprint(DBGM_L3, "%s: pr(%x) di(%x)\n", __FUNCTION__, hh->prim, hh->dinfo);
	dprint(DBGM_L3, "%s: pr(%x) di(%x)\n", __FUNCTION__, frm->prim, frm->dinfo);

	/* get rid of user header */
//	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	msg_pull(msg, AS_ISDN_HEADER_LEN);
#if 0
//	if (hh->prim == (DL_DATA | INDICATION))
	{
		ret = _asnt_l3_dl_data_mux(nst->layer3, hh, msg); 
	}
	else
	{
		ret = _asnt_l3_msg(nst->layer3, hh->prim, hh->dinfo, msg);
	}
#endif	
	if (frm->prim == (DL_DATA | INDICATION))
	{
		int cr = _asnt_l3_get_callref(msg->data);
		if(cr == -2 || cr == -2)
		{
			if (nst->layer3->debug & L3_DEB_WARN)
				_asnt_l3_debug(nst->layer3, "NINS wrong Callref");
			ret = -EINVAL;
		}
		else
		{
			frm->dinfo = cr;
			ret = _asnt_l3_dl_data_mux(nst->layer3, frm, msg); 
		}
	}
	else
	{
		ret = _asnt_l3_msg(nst->layer3, frm->prim, frm->dinfo, msg);
	}
	if (ret)
		as_msg_free(msg);
	ret = 0;
	return(ret);
}

/* msg from L3 to MGR : handle CC_SETUP_REQ and feedback CONFIRM */
static int _asnt_mgr_2_l3(NTSTACK *nst, msg_t *msg)
{
	ASNT_ISDNUSER_HEADER	*hh;
	layer3_proc_t	*proc;
	struct _l3_msg	l3m;

	hh = (ASNT_ISDNUSER_HEADER *)msg->data;

TRACE;	
	dprint(DBGM_L3, "%s: msg len(%d)\n", __FUNCTION__, msg->len);
	dprint(DBGM_L3, "%s: pr(%x) di(%x)\n", __FUNCTION__, hh->prim, hh->dinfo);
	
	msg_pull(msg, ASNT_ISDNUSER_HEAD_SIZE);
	proc = _asnt_l3_find_proc(nst->layer3->proc, hh->dinfo & 0xffff,	(hh->dinfo>>16)& 0xffff);
	if (!proc)
	{
		if (hh->prim == (CC_SETUP | REQUEST))
		{/* CC_SETUP_REQ from L3 to MSG */
			int l4id;
			nst->layer3->next_cr++;
			if (nst->feature & FEATURE_NET_CRLEN2)
			{
				if (nst->layer3->next_cr>32766)
					nst->layer3->next_cr = 1;
			}
			else
			{
				if (nst->layer3->next_cr>126)
					nst->layer3->next_cr = 1;
			}

			/* replace as following for originated call 
			proc = _asnt_l3_create_proc(nst->layer3, hh->dinfo & 0xffff, nst->layer3->next_cr | 0x8000, NULL);
			*/
			proc = _asnt_l3_create_proc(nst->layer3, nst->layer3->next_cr | 0x8000, nst->layer3->next_cr | 0x8000, NULL);
			if (!proc)
			{
				dprint(DBGM_L3, "%s: pr(%x) failed to create proc.\n", __FUNCTION__, hh->prim);
				as_msg_free(msg);
				return(0);
			}
			dprint(DBGM_L3, "%s: proc(%p)\n", __FUNCTION__, proc);
#if 0
#warning testing
#endif

#if 0
printf("check for tei 0 active\n");
		l2 = nst->layer2;
		while(l2)
		{
			if (l2->tei == 0 && l2->sapi == 0)
				break;
			l2 = l2->next;
		}
		if (l2) if (l2->state == ST_L2_4)
		{
			p3i = _asnt_l3_create_proc(proc->l3, 0, proc->callref, proc);
			if (!p3i)
			{
				_asnt_l3_debug(proc->l3, "cannot create child\n");
				return(NULL);
			}
			proc = p3i;
			
			dprint(DBGM_L3, "%s: TEI 0 is active, so we created proc(%p)\n", __FUNCTION__, proc);
			
		}
#endif

			APPEND_TO_LIST(proc, nst->layer3->proc);
			l4id = proc->ces | (proc->callref << 16);
			/* feedback CC_SETUP_CONF primitive */
			if_link(nst, (ifunc_t)nst->l3_2_mgr, CC_SETUP |CONFIRM, hh->dinfo, sizeof(int), &l4id, 0);
		}
	}
	
	if (!proc)
	{
		dprint(DBGM_L3, "%s: pr(%x) no proc id %x found\n", __FUNCTION__,	hh->prim, hh->dinfo);
		as_msg_free(msg);
		return(0);
	}
	
	l3m.mt = hh->prim;
	if (msg->len)
		l3m.msg = msg;
	else
	{
		dprint(DBGM_L3, "%s: pr(%x) id(%x) zero param\n", __FUNCTION__,hh->prim, hh->dinfo);
		l3m.msg = NULL;
	}
	_asnt_l3_send_proc(proc, IMSG_L4_DATA, &l3m);
	as_msg_free(msg);
	return(0);
}

int _asnt_l3_up(layer3_proc_t *l3p, msg_t *msg)
{
	int err = -EINVAL;

	TRACE;
	if (!l3p || !l3p->l3 || !l3p->l3->nst)
		return(-EINVAL);
TRACE;	
	if (l3p->l3->nst->l3_2_mgr)
		err = l3p->l3->nst->l3_2_mgr(l3p->l3->nst, msg);
TRACE;
	if (err)
		dprint(DBGM_L3, "%s: error %d\n", __FUNCTION__, err);
	return(err);
}

int _asnt_l3_down(ASNT_L3 *l3, u_int prim, int dinfo, msg_t *msg)
{
	int err = -EINVAL;

	if(l3->nst->if_l3_2_l2 == NULL)
		dprint(DBGM_L3, "Error : not Layer down IF provided\n");
	if (!msg)
		err = if_link(l3->nst, l3->nst->if_l3_2_l2, prim, dinfo, 0, NULL, 0);
	else
		err = if_addhead(l3->nst, l3->nst->if_l3_2_l2, prim, dinfo, msg);
	return(err);
}

void _asnt_l3_send_squeue(ASNT_L3 *l3, msg_queue_t *squeue)
{
	msg_t	*msg;

	while((msg = msg_dequeue(&l3->squeue0)))
	{
		if (l3->nst->if_l3_2_l2(l3->nst, msg))
			as_msg_free(msg);
	}
}

/* #warning testing */
int _asnt_l3_remove_proc(layer3_proc_t **procp, int ces)
{
	int found = 1;
	int any = 0;
	layer3_proc_t *proc;

	if (ces > 126)
		return(0);

	while(found)
	{
		found = 0;
		proc = *procp;
		
		while(proc)
		{
			dprint(DBGM_L3, "%s: comparing %s proc(%x) ces(%x)\n", __FUNCTION__, (proc->master)?"child":"master", proc, proc->ces);

			if (proc->ces == ces)
			{
				dprint(DBGM_L3, "%s: found proc(%x)\n", __FUNCTION__, proc);
				if (proc->master)
					_asnt_l3_send_proc(proc, IMSG_END_PROC_M, NULL);
				else
					_asnt_l3_send_proc(proc, IMSG_END_PROC, NULL);
				any = 1;
				found = 1;
				break;
			}
			
			if (proc->child)
			{
				if (_asnt_l3_remove_proc(&proc->child, ces))
				{
					any = 1;
					found = 1;
					break;
				}
			}
			proc = proc->next;
			
		}
	}
	
	return(any);
}

static void _asnt_l3_release_l3(ASNT_L3 *l3)
{
	dprint(DBGM_L3, "%s(%p)\n", __FUNCTION__, l3);

	while(l3->proc)
	{
		dprint(DBGM_L3, "%s: rel_proc ces(%x)\n", __FUNCTION__, l3->proc->ces);
		_asnt_l3_send_proc(l3->proc, IMSG_END_PROC, NULL);
	}
	msg_queue_purge(&l3->squeue0);
	REMOVE_FROM_LISTBASE(l3, l3->nst->layer3);
	
	free(l3);
}

int asnt_l3_init(NTSTACK *nst)
{
	ASNT_L3 *l3;

	l3 = malloc(sizeof(ASNT_L3));
	if (!l3)
		return(-ENOMEM);
	memset(l3, 0, sizeof(ASNT_L3));

	l3->nst = nst;
	l3->debug = 0xff;

	nst->if_l2_2_l3 = _asnt_l3_muxer;
	nst->mgr_2_l3 = _asnt_mgr_2_l3;
#if 0	
#warning testing
#endif
	msg_queue_init(&l3->squeue0);
	l3->l2_state0 = ST_L3_LC_REL;
	APPEND_TO_LIST(l3, nst->layer3);

	return(0);
}

void asnt_l3_cleanup(NTSTACK *nst)
{
	if (nst->layer3)
	{
		dprint(DBGM_L3, "%s: l3 list not empty\n", __FUNCTION__);
		while(nst->layer3)
			_asnt_l3_release_l3(nst->layer3);
	}
}


