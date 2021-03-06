/*
 * $Id: dchannel.c,v 1.1.1.1 2006/11/29 08:55:13 lizhijie Exp $
 */

#include <linux/module.h>
#include "asISDN.h"
#include "layer1.h"
#include "helper.h"
#include "dchannel.h"


static void dchannel_bh(dchannel_t *dch)
{
	struct sk_buff	*skb;
	int		err;
	as_isdn_head_t	*hh;

	if (!dch)
		return;

#if AS_ISDN_DEBUG_CORE_CTRL
	if (dch->debug)
		printk(KERN_ERR  "%s: event %lx\n", __FUNCTION__, dch->event);
#endif

//	TRACE();
#if 0
	if (test_and_clear_bit(D_CLEARBUSY, &dch->event))
	{
#if AS_ISDN_DEBUG_CORE_CTRL
		if (dch->debug)
		{
//			AS_ISDN_debugprint(&dch->inst, "D-Channel Busy cleared");
			printk(KERN_DEBUG "D-Channel Busy cleared\n");
		}
#endif
		
		stptr = dch->stlist;
		while (stptr != NULL)
		{
			stptr->l1.l1l2(stptr, PH_PAUSE | CONFIRM, NULL);
			stptr = stptr->next;
		}
	}
#endif
/* in D channel, data received must be transfered layer by layer, so primitives are all PH_DATA_CONFIRM/IND */
//	TRACE();
	if (test_and_clear_bit(D_XMTBUFREADY, &dch->event))
	{
		if ((skb = dch->next_skb))
		{
			hh = AS_ISDN_HEAD_P(skb);
			dch->next_skb = NULL;
			skb_trim(skb, 0);
//			TRACE();
			if (if_newhead(&dch->inst.up, PH_DATA_CNF, hh->dinfo, skb))
			{
//				TRACE();
				dev_kfree_skb(skb);
			}
		}
	}

//	TRACE();
	if (test_and_clear_bit(D_RCVBUFREADY, &dch->event))
	{
		while ((skb = skb_dequeue(&dch->rqueue)))
		{

//			TRACE();
			err = if_newhead(&dch->inst.up, PH_DATA_IND, AS_ISDN_ID_ANY, skb);
//				TRACE();
			if (err < 0)
			{
//				TRACE();
#if AS_ISDN_DEBUG_CORE_CTRL
				printk(KERN_WARNING "%s: deliver err %d\n", __FUNCTION__, err);
#endif
				dev_kfree_skb(skb);
			}
//			TRACE();
		}
	}

//	TRACE();
	if (dch->hw_bh)
		dch->hw_bh(dch);

//	TRACE();
}

int  as_isdn_init_dch(dchannel_t *dch)
{
	if (!(dch->dlog = kmalloc(MAX_DLOG_SPACE, GFP_ATOMIC)))
	{
		printk(KERN_WARNING	"AS_ISDN: No memory for dlog\n");
		return(-ENOMEM);
	}
	
	if (!(dch->tx_buf = kmalloc(MAX_DFRAME_LEN_L1, GFP_ATOMIC)))
	{
		printk(KERN_WARNING	"AS_ISDN: No memory for dchannel tx_buf\n");
		kfree(dch->dlog);
		dch->dlog = NULL;
		return(-ENOMEM);
	}
	
	dch->hw = NULL;
	dch->rx_skb = NULL;
	dch->tx_idx = 0;
	dch->next_skb = NULL;
	dch->event = 0;
//	INIT_WORK(&dch->work, (void *)(void *)dchannel_bh, dch);
	INIT_TQUEUE(&dch->work, (void *)(void *)dchannel_bh, dch);
	dch->hw_bh = NULL;
	skb_queue_head_init(&dch->rqueue);
	return(0);
}

int as_isdn_free_dch(dchannel_t *dch)
{
#ifdef HAS_WORKQUEUE
	if (dch->work.pending)
		printk(KERN_ERR "as_isdn_free_dch work:(%lx)\n", dch->work.pending);
#else
	if (dch->work.sync)
		printk(KERN_ERR "as_isdn_free_dch work:(%lx)\n", dch->work.sync);
#endif
	discard_queue(&dch->rqueue);
	if (dch->rx_skb)
	{
		dev_kfree_skb(dch->rx_skb);
		dch->rx_skb = NULL;
	}
	if (dch->tx_buf)
	{
		kfree(dch->tx_buf);
		dch->tx_buf = NULL;
	}
	if (dch->next_skb)
	{
		dev_kfree_skb(dch->next_skb);
		dch->next_skb = NULL;
	}
	if (dch->dlog)
	{
		kfree(dch->dlog);
		dch->dlog = NULL;
	}
	return(0);
}

EXPORT_SYMBOL(as_isdn_init_dch);
EXPORT_SYMBOL(as_isdn_free_dch);
