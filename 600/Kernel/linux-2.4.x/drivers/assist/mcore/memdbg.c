/*
* $Id: memdbg.c,v 1.1.1.1 2006/11/29 08:55:13 lizhijie Exp $
*/

#include <linux/stddef.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/skbuff.h>
#include "asISDN.h"
#include <asm/bugs.h>
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

static struct list_head AS_ISDN_memdbg_list = LIST_HEAD_INIT(AS_ISDN_memdbg_list);
static struct list_head AS_ISDN_skbdbg_list = LIST_HEAD_INIT(AS_ISDN_skbdbg_list);
static spinlock_t	memdbg_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	skbdbg_lock = SPIN_LOCK_UNLOCKED;
static kmem_cache_t	*mid_sitem_cache;

char	_mid_fn[256];
u_int	_mid_line;
u_int	__mid_debug_1;

#define MAX_FILE_STRLEN		(64 - 3*sizeof(u_int) - sizeof(struct list_head))

#define MID_ITEM_TYP_KMALLOC	1
#define MID_ITEM_TYP_VMALLOC	2

typedef struct _mid_item {
	struct list_head	head;
	u_int			typ;
	u_int			size;
	u_int			line;
	char			file[MAX_FILE_STRLEN];
} _mid_item_t;

typedef struct _mid_sitem {
	struct list_head	head;
	struct sk_buff		*skb;
	unsigned int		size;
	int			line;
	char			file[MAX_FILE_STRLEN];
} _mid_sitem_t;

void *
__mid_kmalloc(size_t size, int ord, char *fn, int line)
{
	_mid_item_t	*mid;
	u_long		flags;

	mid = kmalloc(size + sizeof(_mid_item_t), ord);
	if (mid) {
		INIT_LIST_HEAD(&mid->head);
		mid->typ  = MID_ITEM_TYP_KMALLOC;
		mid->size = size;
		mid->line = line;
		memcpy(mid->file, fn, MAX_FILE_STRLEN);
		mid->file[MAX_FILE_STRLEN-1] = 0; 
		spin_lock_irqsave(&memdbg_lock, flags);
		list_add_tail(&mid->head, &AS_ISDN_memdbg_list);
		spin_unlock_irqrestore(&memdbg_lock, flags);
		return((void *)&mid->file[MAX_FILE_STRLEN]);
	} else
		return(NULL);
}

void
__mid_kfree(const void *p, char *fn, int line)
{
	struct list_head	*item, *next;
	_mid_item_t	*mid;
	u_long		flags;

	if (!p) {
		printk(KERN_ERR "zero pointer kfree at %p", __builtin_return_address(0));
		return;
	}
#if 0
	mid = (_mid_item_t *)((u_char *)p - sizeof(_mid_item_t));
	spin_lock_irqsave(&memdbg_lock, flags);
	list_del(&mid->head);
	spin_unlock_irqrestore(&memdbg_lock, flags);
	kfree(mid);
#endif

	mid = (_mid_item_t *)((u_char *)p - sizeof(_mid_item_t));
	spin_lock_irqsave(&memdbg_lock, flags);
	list_for_each_safe(item, next, &AS_ISDN_memdbg_list) {
		if (mid == (_mid_item_t *)item) {
			list_del(&mid->head);
			spin_unlock_irqrestore(&memdbg_lock, flags);
			kfree(mid);
			return;
		}
	}
	spin_unlock_irqrestore(&memdbg_lock, flags);
	printk(KERN_ERR  "\n\n********\n****\n**** %s: item(%p) not in list\n****\n********\n\n\n", __FUNCTION__, p);
//	WARN_ON(1);
}

void *
__mid_vmalloc(size_t size, char *fn, int line)
{
	_mid_item_t	*mid;
	u_long		flags;

	mid = vmalloc(size + sizeof(_mid_item_t));
	if (mid) {
		INIT_LIST_HEAD(&mid->head);
		mid->typ  = MID_ITEM_TYP_VMALLOC;
		mid->size = size;
		mid->line = line;
		memcpy(mid->file, fn, MAX_FILE_STRLEN);
		mid->file[MAX_FILE_STRLEN-1] = 0; 
		spin_lock_irqsave(&memdbg_lock, flags);
		list_add_tail(&mid->head, &AS_ISDN_memdbg_list);
		spin_unlock_irqrestore(&memdbg_lock, flags);
		return((void *)&mid->file[MAX_FILE_STRLEN]);
	} else
		return(NULL);
}

void
__mid_vfree(const void *p, char *fn, int line)
{
	_mid_item_t	*mid;
	u_long		flags;

	if (!p) {
		printk(KERN_ERR "zero pointer vfree at %p", __builtin_return_address(0));
		return;
	}
	mid = (_mid_item_t *)((u_char *)p - sizeof(_mid_item_t));
	spin_lock_irqsave(&memdbg_lock, flags);
	list_del(&mid->head);
	spin_unlock_irqrestore(&memdbg_lock, flags);
	vfree(mid);
}

void
__mid_kfree_skb(const void *p, char *fn, int line)
{
//	_mid_item_t	*mid;

	if (!p) {
		printk(KERN_ERR "zero pointer vfree at %p", __builtin_return_address(0));
		return;
	}
// jolly debug start
	if (_mid_line) {
		printk(KERN_ERR "kfree_skb (line %d file %s) during kfree_skb (line %d file %s)\n", line, fn, _mid_line, _mid_fn);
	}
	strncpy(_mid_fn, fn, sizeof(_mid_fn)-1);
	_mid_line = line;
	kfree_skb((struct sk_buff *)p);
	_mid_fn[0] = 0;
	_mid_line = 0;
// jolly debug stop

}

void
__mid_dev_kfree_skb(const void *p, char *fn, int line)
{
//	_mid_item_t	*mid;

	if (!p) {
		printk(KERN_ERR "zero pointer vfree at %p", __builtin_return_address(0));
		return;
	}
// jolly debug start
	if (_mid_line) {
		printk(KERN_ERR "kfree_skb (line %d file %s) during kfree_skb (line %d file %s)\n", line, fn, _mid_line, _mid_fn);
//		while((volatile u_int)_mid_line)
//			;
	}
	strncpy(_mid_fn, fn, sizeof(_mid_fn)-1); 
	_mid_line = line;
	dev_kfree_skb((struct sk_buff *)p);
	_mid_fn[0] = 0;
	_mid_line = 0;
// jolly debug stop
}

static void
__mid_skb_destructor(struct sk_buff *skb)
{
	struct list_head	*item;
	_mid_sitem_t		*sid;
	u_long		flags;

	spin_lock_irqsave(&skbdbg_lock, flags);
	list_for_each(item, &AS_ISDN_skbdbg_list) {
 		sid = (_mid_sitem_t *)item;
 		if (sid->skb == skb) {
 			list_del(&sid->head);
			spin_unlock_irqrestore(&skbdbg_lock, flags);
 			kmem_cache_free(mid_sitem_cache, sid);
 			return;
 		}
 	}
	spin_unlock_irqrestore(&skbdbg_lock, flags);
	printk(KERN_ERR  "\n\n********\n****\n**** %s: item(%p) not in list\n****\n********\n\n\n", __FUNCTION__, skb);
// jolly debug start
	printk(KERN_ERR  "-> line %d file '%s'  debug_1 %d\n", _mid_line, _mid_fn, __mid_debug_1);
// jolly debug stop
//	WARN_ON(1);
}

void
__mid_skb_check(struct sk_buff *skb, char *text)
{
	struct list_head	*item, *next;
	_mid_sitem_t		*sid;
	u_long		flags;

	spin_lock_irqsave(&skbdbg_lock, flags);
	list_for_each_safe(item, next, &AS_ISDN_skbdbg_list) {
		sid = (_mid_sitem_t *)item;
		if (sid->skb == skb) {
			spin_unlock_irqrestore(&skbdbg_lock, flags);
			return;
		}
	}
	spin_unlock_irqrestore(&skbdbg_lock, flags);
	printk(KERN_ERR  "CHECK FAILED %s: item(%p) not in list\n", __FUNCTION__, skb);
	printk(KERN_ERR  "CHECK FAILED -> text '%s'\n", text);
}

static __inline__ void
__mid_sitem_setup(struct sk_buff *skb, unsigned int size, char *fn, int line)
{
	_mid_sitem_t	*sid;
	u_long		flags;

	sid = kmem_cache_alloc(mid_sitem_cache, GFP_ATOMIC);
	if (!sid)
		return;
	INIT_LIST_HEAD(&sid->head);
	sid->skb = skb;
	sid->size = size;
	sid->line = line;
	memcpy(sid->file, fn, MAX_FILE_STRLEN);
	sid->file[MAX_FILE_STRLEN-1] = 0; 
	skb->destructor = __mid_skb_destructor;
	spin_lock_irqsave(&skbdbg_lock, flags);
	list_add_tail(&sid->head, &AS_ISDN_skbdbg_list);
	spin_unlock_irqrestore(&skbdbg_lock, flags);
}

struct sk_buff *
__mid_alloc_skb(unsigned int size, int gfp_mask, char *fn, int line)
{
	struct sk_buff	*skb = alloc_skb(size, gfp_mask);

	if (!skb)
		return(NULL);
	__mid_sitem_setup(skb, size, fn, line);
	return(skb);
}

struct sk_buff *
__mid_dev_alloc_skb(unsigned int size, char *fn, int line)
{
	struct sk_buff	*skb = dev_alloc_skb(size);

	if (!skb)
		return(NULL);
	__mid_sitem_setup(skb, size, fn, line);
	return(skb);
}

struct sk_buff
*__mid_skb_clone(struct sk_buff *skb, int gfp_mask, char *fn, int line)
{
	struct sk_buff	*nskb = skb_clone(skb, gfp_mask);

	if (!nskb)
		return(NULL);
	__mid_sitem_setup(nskb, (nskb->end - nskb->head), fn, line);
	return(nskb);
}

struct sk_buff
*__mid_skb_copy(struct sk_buff *skb, int gfp_mask, char *fn, int line)
{
	struct sk_buff	*nskb = skb_copy(skb, gfp_mask);

	if (!nskb)
		return(NULL);
	__mid_sitem_setup(nskb, (nskb->end - nskb->head), fn, line);
	return(nskb);
}

struct sk_buff
*__mid_skb_realloc_headroom(struct sk_buff *skb, unsigned int headroom, char *fn, int line)
{
	struct sk_buff	*nskb = skb_realloc_headroom(skb, headroom);

	if (!nskb || (nskb == skb))
		return(nskb);
	__mid_sitem_setup(nskb, (nskb->end - nskb->head), fn, line);
	return(nskb);
}

void
__mid_cleanup(void)
{
	struct list_head	*item, *next;
	_mid_item_t		*mid;
	_mid_sitem_t		*sid;
	as_isdn_head_t		*hh;
	int			n = 0;

	list_for_each_safe(item, next, &AS_ISDN_memdbg_list) {
		mid = (_mid_item_t *)item;
		switch(mid->typ) {
			case MID_ITEM_TYP_KMALLOC:
				printk(KERN_ERR "not freed kmalloc size(%d) from %s:%d\n",
					mid->size, mid->file, mid->line);
				kfree(mid);
				break;
			case MID_ITEM_TYP_VMALLOC:
				printk(KERN_ERR "not freed vmalloc size(%d) from %s:%d\n",
					mid->size, mid->file, mid->line);
				vfree(mid);
				break;
			default:
				printk(KERN_ERR "unknown mid->typ(%d) size(%d) from %s:%d\n",
					mid->typ, mid->size, mid->file, mid->line);
				break;
		}
		n++;
	}
	printk(KERN_ERR  "%s: %d kmalloc item(s) freed\n", __FUNCTION__, n);
	n = 0;
	list_for_each_safe(item, next, &AS_ISDN_skbdbg_list) {
		sid = (_mid_sitem_t *)item;
		hh = AS_ISDN_HEAD_P(sid->skb);
		printk(KERN_ERR "not freed skb(%p) size(%d) prim(%x) dinfo(%x) allocated at %s:%d\n",
			sid->skb, sid->size, hh->prim, hh->dinfo, sid->file, sid->line);
		/*maybe the skb is still aktiv */
		sid->skb->destructor = NULL;
		list_del(&sid->head);
		kmem_cache_free(mid_sitem_cache, sid);
		n++;
	}

	if (mid_sitem_cache)
		kmem_cache_destroy(mid_sitem_cache);
	printk(KERN_ERR  "%s: %d sk_buff item(s) freed\n", __FUNCTION__, n);
}

int
__mid_init(void)
{
	mid_sitem_cache = kmem_cache_create("AS_ISDN_skbdbg",
				sizeof(_mid_sitem_t),
				0, 0, NULL, NULL);
	if (!mid_sitem_cache)
		return(-ENOMEM);
	return(0);
}

#ifdef MODULE
EXPORT_SYMBOL(__mid_kmalloc);
EXPORT_SYMBOL(__mid_kfree);
EXPORT_SYMBOL(__mid_vmalloc);
EXPORT_SYMBOL(__mid_vfree);
EXPORT_SYMBOL(__mid_kfree_skb);
EXPORT_SYMBOL(__mid_dev_kfree_skb);
EXPORT_SYMBOL(__mid_alloc_skb);
EXPORT_SYMBOL(__mid_dev_alloc_skb);
EXPORT_SYMBOL(__mid_skb_clone);
EXPORT_SYMBOL(__mid_skb_copy);
EXPORT_SYMBOL(__mid_skb_realloc_headroom);
EXPORT_SYMBOL(__mid_debug_1);
EXPORT_SYMBOL(__mid_skb_check);
#endif

