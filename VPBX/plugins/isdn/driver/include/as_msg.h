/*
* $Id: as_msg.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_H__
#define __MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <semaphore.h>

#define MAX_MSG_SIZE		2080
#define DEFAULT_HEADROOM	16

typedef struct _msg
{
	struct _msg			*prev;
	struct _msg			*next;
	struct _msg_queue	*list;
	int					len;
	int					size;
	unsigned char			*head;
	unsigned char			*data;
	unsigned char			*tail;
	unsigned char			*end;
	unsigned char			__data[MAX_MSG_SIZE];
} msg_t;

typedef	struct _msg_queue
{
	struct _msg		*prev;
	struct _msg		*next;
	pthread_mutex_t 	lock;
	int				len;
	int				maxlen;	
} msg_queue_t;

extern	void			as_msg_init(void);
extern	msg_t		*as_msg_alloc(int);
extern	void			as_msg_free(msg_t *);
extern	msg_queue_t	*free_queue;
extern	msg_t		*as_msg_copy(msg_t *msg);

#define	MSG_CLONE(m)	as_msg_copy(m)
 
static inline void msg_queue_init(msg_queue_t *q)
{
	pthread_mutex_init(&q->lock, NULL);
	q->len = 0;
	q->prev = (msg_t *)q;
	q->next = (msg_t *)q;
}

static inline int msg_queue_len(msg_queue_t *list_)
{
	return(list_->len);
}

static inline void msg_queue_head(msg_queue_t *list, msg_t *newm)
{
	msg_t *prev, *next;

	pthread_mutex_lock(&list->lock);
	newm->list = list;
	list->len++;
	prev = (msg_t *)list;
	next = prev->next;
	newm->next = next;
	newm->prev = prev;
	next->prev = newm;
	prev->next = newm;
	pthread_mutex_unlock(&list->lock);
}


static inline void msg_queue_tail(msg_queue_t *list, msg_t *newm)
{
	msg_t *prev, *next;

	pthread_mutex_lock(&list->lock);
	newm->list = list;
	list->len++;
	next = (msg_t *)list;
	prev = next->prev;
	newm->next = next;
	newm->prev = prev;
	next->prev = newm;
	prev->next = newm;
	pthread_mutex_unlock(&list->lock);
}


static inline msg_t *msg_dequeue(msg_queue_t *list)
{
	msg_t *next, *prev, *result;

	pthread_mutex_lock(&list->lock);
	prev = (msg_t *) list;
	next = prev->next;
	result = NULL;
	if (next != prev)
	{
		result = next;
		next = next->next;
		list->len--;
		next->prev = prev;
		prev->next = next;
		result->next = NULL;
		result->prev = NULL;
		result->list = NULL;
	}
	pthread_mutex_unlock(&list->lock);
	return result;
}

static __inline__ void msg_queue_purge(msg_queue_t *list)
{
	msg_t *msg;

	while ((msg = msg_dequeue(list))!=NULL)
		as_msg_free(msg);
}

/* shift right tail point to get more space for data at tail of msg->data */
static __inline__ unsigned char *msg_put(msg_t *msg, unsigned int len)
{
	unsigned char *tmp=msg->tail;
	msg->tail+=len;
	msg->len+=len;
	if(msg->tail>msg->end)
	{
		fprintf(stderr, "msg_over_panic msg(%p) data(%p) head(%p)\n",msg, msg->data, msg->head);
		return(NULL);
	}
	return tmp;
}

/* shift left to get more space for other header */
static __inline__ unsigned char *msg_push(msg_t *msg, unsigned int len)
{
	msg->data-=len;
	msg->len+=len;
	if(msg->data < msg->head)
	{
		fprintf(stderr, "msg_under_panic msg(%p) data(%p) head(%p)\n", msg, msg->data, msg->head);
		return(NULL);
	}
	return msg->data;
}


static __inline__ char *__msg_pull(msg_t *msg, unsigned int len)
{
	msg->len-=len;
	return 	(char *)(msg->data+=len);
}

/* shift right the msg->data pointer  */
static __inline__ unsigned char * msg_pull(msg_t *msg, unsigned int len)
{
	if (len > (unsigned int)msg->len)
		return NULL;
	return (unsigned char *)__msg_pull(msg,len);
}

static __inline__ int msg_headroom(msg_t *msg)
{
	return msg->data - msg->head;
}

static __inline__ int msg_tailroom(msg_t *msg)
{
	return msg->end - msg->tail;
}

static __inline__ void msg_reserve(msg_t *msg, unsigned int len)
{
	msg->data+=len;
	msg->tail+=len;
}

/* len : left bytes for data before tail */
static __inline__ void __msg_trim(msg_t *msg, unsigned int len)
{
	msg->len = len;
	msg->tail = msg->data+len;
}

static __inline__ void msg_trim(msg_t *msg, unsigned int len)
{
	if ((unsigned int)msg->len > len)
	{
		__msg_trim(msg, len);
	}
}

#ifdef __cplusplus
}
#endif

#endif
