/*
$Id: as_nt_mgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asnt_isdn.h"
#include "l3ins.h"
#include "asnt_nins.h"
#include "helper.h"

#include "IsdnNtDevice.hxx"
/*
* Interface between User Space NT stack and C++ environments 
*/

using namespace Assist;

/* User Space Upper layer (L3) Output interface, from here C++ is used 
* eg. NTSTACK->l3_2_mgr 
*/
int asnt_upper_output(NTSTACK *nst, msg_t *msg)
{
//	NTMGR 	*mgr = (NTMGR *)dat;
//	NTSTACK		*nst = (NTSTACK *)dat;
//	msg_t		*msg = (msg_t *)arg;
	ASNT_ISDNUSER_HEADER	*hh;
	IsdnNtDevice *ctrl = (IsdnNtDevice *)nst->priv;
	int ret = 0;

	TRACE;
	if (!nst ||!msg ||  !ctrl)
		return(-EINVAL);
	
	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
	dprint(DBGM_MAN, "%s: prim(%x) dinfo(%x) msg->len(%d) \n", __FUNCTION__, hh->prim, hh->dinfo, msg->len );

	ret = ctrl->myL3MsgHandler(msg);
	if( ret == AS_MSG_CONTINUE)
	{
		ret = -EINVAL;
	}
	else
		ret = 0;

	return ret;
}

int asnt_write_frame(NTSTACK *nst, int addr, int prim, int dinfo, int dlen, void *dbuf)
{
//	NTMGR *mgr = NULL;
	IsdnNtDevice *ctrl =NULL;

	TRACE;
	assert(nst != 0);
//	mgr = nst->manager;
//	assert( mgr != 0);
	ctrl = (IsdnNtDevice *)nst->priv;
	assert(ctrl != 0);
	return ctrl->hwSendOutFrame(addr, prim, dinfo, dlen, dbuf);
}

/*only iframe_t is keep in this msg */
int asnt_write_msg(NTSTACK *nst, msg_t *msg)
{
//	NTMGR *mgr = NULL;
	IsdnNtDevice *ctrl = NULL;

	TRACE;
	assert(nst != 0);
//	mgr = nst->manager;
//	assert( mgr != 0);
	ctrl = (IsdnNtDevice *)nst->priv;
	assert(ctrl != 0);
	return ctrl->hwSendOutMsg(msg);
}

#if 0

/* app to B Channel : select a B Channel and send App msg in workq of this Channel, just as stack2mgr*/
int _asnt_mgr_app2bc(NTMGR *mgr, int prim, void *arg)
{
	NTCtrl		*ctrl = (NTCtrl *)arg;
	msg_t		*msg;

	dprint(DBGM_MAN, "%s(%p,%x,%p)\n", __FUNCTION__,	 mgr, prim, arg);
	if (!mgr || !ctrl)
		return(-EINVAL);

#if 0	
	if (prim == PR_APP_OCHANNEL)
	{/* Retrieve Output Channel : return available NTBCH */

		pthread_mutex_lock(&mgr->ctrl->lock);
		if (mgr->bc[0].cstate == BC_CSTATE_NULL)
		{
			mgr->bc[0].cstate = BC_CSTATE_OCALL;
			pthread_mutex_unlock(&mgr->bc[0].lock);
			*bcp = &mgr->bc[0];
			return(1);
		}
		pthread_mutex_unlock(&mgr->bc[0].lock);

		pthread_mutex_lock(&mgr->bc[1].lock);
		if (mgr->bc[1].cstate == BC_CSTATE_NULL)
		{
			mgr->bc[1].cstate = BC_CSTATE_OCALL;
			pthread_mutex_unlock(&mgr->bc[1].lock);
			*bcp = &mgr->bc[1];
			return(2);
		}
		pthread_mutex_unlock(&mgr->bc[1].lock);
		
		/* No channel available */
		return(-EBUSY);
	}
	else if (prim == PR_APP_OCALL)
	{/* output a call on NTBCH : CC_SETUP_REQ with l3id */
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_SETUP | REQUEST, bc->l3id, 0,	NULL, 0);
		if (!msg)
		{
			sem_post(&bc->work);
			pthread_mutex_unlock(&bc->lock);
			return(-ENOMEM);
		}
		
		msg_queue_tail(&bc->workq, msg);
		sem_post(&bc->work);
		pthread_mutex_unlock(&bc->lock);
	}
	else if (prim == PR_APP_ALERT)
	{/* select ALERT */
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_ALERTING | REQUEST, bc->l3id, 0,	NULL, 0);
		if (!msg)
		{
			sem_post(&bc->work);
			pthread_mutex_unlock(&bc->lock);
			return(-ENOMEM);
		}
		
		msg_queue_tail(&bc->workq, msg);
		sem_post(&bc->work);
		pthread_mutex_unlock(&bc->lock);
	}
	else if (prim == PR_APP_CONNECT)
	{/* CC_CONNECT_REQ : */
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_CONNECT | REQUEST, bc->l3id, 0, NULL, 0);
		if (!msg)
		{
			sem_post(&ctrl->work);
			pthread_mutex_unlock(&ctrl->lock);
			return(-ENOMEM);
		}
		
		msg_queue_tail(&ctrl->workq, msg);
		sem_post(&ctrl->work);
		pthread_mutex_unlock(&ctrl->lock);
	}
	else if (prim == PR_APP_HANGUP)
	{/* HANGUP*/
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_DISCONNECT | REQUEST, bc->l3id, 0, NULL, 0);
		if (!msg)
		{
			sem_post(&bc->work);
			pthread_mutex_unlock(&bc->lock);
			return(-ENOMEM);
		}
		msg_queue_tail(&bc->workq, msg);
		sem_post(&bc->work);
		pthread_mutex_unlock(&bc->lock);
	}
	else if (prim == PR_APP_FACILITY)
	{
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_FACILITY | REQUEST, bc->l3id,	0, NULL, 0);
		if (!msg)
		{
			sem_post(&bc->work);
			pthread_mutex_unlock(&bc->lock);
			return(-ENOMEM);
		}
		msg_queue_tail(&bc->workq, msg);
		sem_post(&bc->work);
		pthread_mutex_unlock(&bc->lock);
	}
	else if (prim == PR_APP_USERUSER)
	{
		pthread_mutex_lock(&bc->lock);
		msg = create_link_msg(CC_USER_INFORMATION | REQUEST, bc->l3id,0, NULL, 0);
		if (!msg)
		{
			sem_post(&bc->work);
			pthread_mutex_unlock(&bc->lock);
			return(-ENOMEM);
		}
		
		msg_queue_tail(&bc->workq, msg);
		sem_post(&bc->work);
		pthread_mutex_unlock(&bc->lock);
	}
	else
#endif		
	{
		wprint("%s(%p,%x,%p) unhandled\n", __FUNCTION__, mgr, prim, arg);
	}
	return(0);
}



int asnt_mgr_match_nr(NTMGR *mgr, unsigned char *nx, nr_list_t **nrx)
{
	int		l,i,ret = 2;
	unsigned char	*p;
	nr_list_t	*nr = mgr->nrlist;

	if (!nrx)
		return(3);
	
	l = nx[0] - 1;
	if (l<=0)	/* length ?? */
		return(3);

	while(nr)
	{
		p = nx + 2;
		dprint(DBGM_MAN,"%s: cpn(%s) nr(%s)\n", __FUNCTION__, p, nr->nr);
		for(i=0;i<nr->len;i++)
		{
			if (*p != nr->nr[i])
				break;
			if ((i+1) == nr->len)
			{
				*nrx = nr;
				return(0);
			}
			
			if (l == (i+1))
			{
				ret = 1;
				break;
			}
			p++;	
		}
		nr = nr->next;
	}
	return(ret);
}

#endif

