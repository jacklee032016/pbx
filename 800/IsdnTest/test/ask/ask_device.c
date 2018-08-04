/*
$Id: ask_device.c,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
*/
#include "ask_isdn.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#if WITH_VPBX
#include "cpLog.h"
#endif

typedef struct _askISDNdev
{
	struct _askISDNdev		*prev;
	struct _askISDNdev		*next;
	pthread_mutex_t		rmutex;
	pthread_mutex_t		wmutex;
	int					Flags;
	int					fid;
	int					isize;
	unsigned char			*inbuf;
	unsigned char           	*irp;
	unsigned char			*iend;
} askISDNdev_t;

#define FLG_KISDN_WRRD_ATOMIC	1

askISDNdev_t *devlist = NULL;

static	pthread_mutex_t	devlist_lock = PTHREAD_MUTEX_INITIALIZER;

#define AS_ISDN_DEVICE		"/dev/astel/isdn"

#if 0
void
xxxxxxxxxx(void) {
if (devlist)
	fprintf(stderr, "xxxxxxxxxx dev %p prev %p next %p\n", devlist, devlist->prev, devlist->next);
else
	fprintf(stderr, "xxxxxxxxxx devlist %p\n", devlist);
}
#endif

#if ASK_DEBUG
void ask_hex_dump(void *buf, int len)
{
	int i;
	for (i = 0 ; i < len; i++)
	{
		printf("%02x", ((unsigned char *)buf)[i]);
		if (i%2)
			printf(" ");
		if (15 == i%16)
			printf("\n");
	}
	printf("\n");
}
#endif

int ask_isdn_open(void)
{
	int		fid;
	askISDNdev_t	*dev;

	
	if (0>(fid = open(AS_ISDN_DEVICE, O_RDWR | O_NONBLOCK)))
//	if (0>(fid = open(AS_ISDN_DEVICE, O_RDWR  )))
		return(fid);

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev)
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (dev)
	{
		fprintf(stderr, "%s: device %d (%p) has allready fid(%d)\n", __FUNCTION__, dev->fid, dev, fid);
		close(fid);
		errno = EBUSY;
		return(-1); 
	}
	dev = malloc(sizeof(askISDNdev_t));
	if (!dev)
	{
		close(fid);
		errno = ENOMEM;
		return(-1);
	}
	memset(dev, 0, sizeof(askISDNdev_t));
	dev->fid = fid;
	dev->isize = AS_ISDN_INBUFFER_SIZE;
	dev->inbuf = malloc(dev->isize);
	if (!dev->inbuf)
	{
		free(dev);
		close(fid);
		errno = ENOMEM;
		return(-1);
	}
	dev->irp = dev->inbuf;
	dev->iend = dev->inbuf;
	pthread_mutex_init(&dev->rmutex, NULL);
	pthread_mutex_init(&dev->wmutex, NULL);
	
	pthread_mutex_lock(&devlist_lock);
	dev->prev = devlist;
	while(dev->prev && dev->prev->next)
		dev->prev = dev->prev->next;
	if (devlist)
		dev->prev->next = dev;
	else
		devlist = dev;
	pthread_mutex_unlock(&devlist_lock);

	return(fid);
}

int ask_isdn_close(int fid)
{
	askISDNdev_t	*dev = devlist;
	int		ret;

	pthread_mutex_lock(&devlist_lock);
	while(dev)
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}

	if (!dev)
	{
		pthread_mutex_unlock(&devlist_lock);
		errno = ENODEV;
		return(-1);
	}

	if (dev->prev)
		dev->prev->next = dev->next;
	if (dev->next)
		dev->next->prev = dev->prev;
	if (devlist==dev)
		devlist=dev->next;

	pthread_mutex_lock(&dev->rmutex);
	if (dev->inbuf)
		free(dev->inbuf);
	dev->inbuf = NULL;
	pthread_mutex_unlock(&dev->rmutex);

	ret = pthread_mutex_destroy(&dev->rmutex);
	if (ret)
		fprintf(stderr, "%s: rmutex destroy returns %d\n", __FUNCTION__, ret);
	pthread_mutex_lock(&dev->wmutex);
	pthread_mutex_unlock(&dev->wmutex);
	ret = pthread_mutex_destroy(&dev->wmutex);
	
	if (ret)
		fprintf(stderr, "%s: wmutex destroy returns %d\n", __FUNCTION__, ret);
	pthread_mutex_unlock(&devlist_lock);
	free(dev);

	return(close(fid));
}

static int _ask_isdn_remove_iframe(askISDNdev_t *dev, iframe_t *frm)
{
	u_char	*ep;
	int	len;

	if (frm->len > 0)
		len = AS_ISDN_HEADER_LEN + frm->len;
	else
		len = AS_ISDN_HEADER_LEN;
	
	ep = (u_char *)frm;
	ep += len;
	if (ep >= dev->iend)
		dev->iend = (u_char *)frm;
	else
	{
		memcpy(frm, ep, dev->iend - ep);
		dev->iend -= len; 
	}
	
	return(dev->iend - dev->irp);
}

/* read raw frame from dev->inbuf into 'buf', this frame is not remove from dev->inbuf */
int ask_isdn_read(int fid, void *buf, size_t count, int utimeout) 
{
	askISDNdev_t	*dev;
	int		ret = 0, len, sel;
	fd_set		in;
	iframe_t	*ifr;
	struct timeval	tout;
#ifdef MUTEX_TIMELOCK
	struct timespec	ts;
#endif

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev) 
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev) 
	{
		errno = ENODEV;
		return(-1);
	}
	
	if (utimeout != -1) 
	{
		tout.tv_sec = utimeout/1000000;
		tout.tv_usec = utimeout%1000000;
#ifdef MUTEX_TIMELOCK
		if (utimeout == 0) 
		{
			ret = pthread_mutex_trylock(&dev->rmutex);
			if (ret)
			{
				fprintf(stderr, "%s: mutex_trylock (%d)\n",	__FUNCTION__, ret);
				errno = ret;
				return(-1);
			}
		}
		else
		{
#ifdef CLOCK_REALTIME
			clock_gettime(CLOCK_REALTIME, &ts);
#else
			{
				struct timeval  tv;
				struct timezone tz;

				gettimeofday(&tv,&tz);
				TIMEVAL_TO_TIMESPEC(&tv,&ts);
			}
#endif
			ts.tv_sec += tout.tv_sec;
			ts.tv_nsec += 1000*tout.tv_usec;
			if (ts.tv_nsec > 1000000000L)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000L;
			}
			ret = pthread_mutex_timedlock(&dev->rmutex, &ts);
			if (ret)
			{
				fprintf(stderr, "%s: mutex_timedlock (%d)\n", __FUNCTION__, ret);
				errno = ret;
				return(-2);
			}
		}
#else
		pthread_mutex_lock(&dev->rmutex);
#endif
	} 
	else
		pthread_mutex_lock(&dev->rmutex);

	if (dev->Flags & FLG_KISDN_WRRD_ATOMIC) 
	{
//		fprintf(stderr, "%s: WRRD_ATOMIC try again\n", __FUNCTION__);
		errno = EAGAIN;
		ret = -3;
		goto out;
	}
	len = dev->iend - dev->irp;

	/* read only when no data available in dev->inbuf */
	if (!len)
	{
		dev->irp = dev->iend = dev->inbuf;
		pthread_mutex_unlock(&dev->rmutex);
		FD_ZERO(&in);
		FD_SET(fid, &in);
		if (utimeout != -1)
		{
			sel = select(fid + 1, &in, NULL, NULL, &tout);
		}
		else
			sel = select(fid + 1, &in, NULL, NULL, NULL);

		if (sel<0)
		{
//			fprintf(stderr, "%s: select err(%d)\n", __FUNCTION__, errno);
			return(sel);
		}
		else if (sel==0)
		{
//			fprintf(stderr, "%s: select timed out\n", __FUNCTION__);
			return(0);
		}

		if (FD_ISSET(fid, &in))
		{
#ifdef MUTEX_TIMELOCK
			if (!utimeout)
			{
				ret = pthread_mutex_trylock(&dev->rmutex);
				if (ret)
				{
					fprintf(stderr, "%s: mutex_trylock (%d)\n", __FUNCTION__, ret);
					errno = ret;
					return(-4);
				}
			}
			else
#endif
				pthread_mutex_lock(&dev->rmutex);

			len = dev->isize  - (dev->iend - dev->irp);
			if (len<=0)
			{
				fprintf(stderr, "%s-%d :buffer error\n", __FUNCTION__, __LINE__);
				errno = ENOSPC;
				ret = -5;
				goto out;
			}
			if (dev->Flags & FLG_KISDN_WRRD_ATOMIC)
			{
				fprintf(stderr, "%s: WRRD_ATOMIC try again\n", __FUNCTION__);
				errno = EAGAIN;
				ret = -6;
				goto out;
			}
			len = read(fid, dev->iend, len);
//			fprintf(stderr, "%s: read %d\n", __FUNCTION__, len);
			if (len <= 0)
			{
				ret = len;
				goto out;
			}
#if ASK_DEBUG
			printf("Data read from File descriptor :\r\n");
			ask_hex_dump(dev->iend,  len);
#endif
			dev->iend += len;
			len = dev->iend - dev->irp;
		}
		else
		{
			return(0);
		}
	}

	if (len < AS_ISDN_HEADER_LEN)
	{
		dev->iend = dev->irp;
		fprintf(stderr, "%s: frame too short:%d\n",	__FUNCTION__, len);
		errno = EINVAL;
		ret = -7;
		goto out;
	}
	
	ifr = (iframe_t *)dev->irp;
	if (ifr->len > 0)
	{
		if ((ifr->len + AS_ISDN_HEADER_LEN) > len)
		{
			dev->iend = dev->irp;
			errno = EINVAL;
			ret = -8;
			goto out;
		}
		len = ifr->len + AS_ISDN_HEADER_LEN;
	} 
	else
		len = AS_ISDN_HEADER_LEN;

	if (len>count)
	{
		errno = ENOSPC;
		ret = -9;
		goto out;
	}
	memcpy(buf, dev->irp, len);
	dev->irp += len;
	ret = len;
out:
	pthread_mutex_unlock(&dev->rmutex);
	return(ret);
}

static iframe_t *_ask_isdn_find_iframe(askISDNdev_t *dev, u_int addr, u_int prim) 
{
	iframe_t	*frm;
	u_char		*rp;
	
	rp = dev->irp;
	while(rp<dev->iend) 
	{
		if ((dev->iend - rp) < AS_ISDN_HEADER_LEN) 
		{
			return(NULL);
		}
		
		frm = (iframe_t *)rp;
#if ASK_DEBUG		
		printf("Address %x(search %x); Prim %x(search %x)\n", frm->addr, addr, frm->prim, prim );
#endif
		if ((frm->addr == addr) && (frm->prim == prim))
			return(frm);
		
		if (frm->len > 0)
			rp += AS_ISDN_HEADER_LEN + frm->len;
		else
			rp += AS_ISDN_HEADER_LEN;
		
	}
	return(NULL);
}

/* read from fd into dev->buf, and find the frame with 'addr' and 'msgtype', 
 * then put this frame into param 'buf', finally remove this frame from dev->inbuf 
*/
int ask_isdn_read_frame(int fid, void *buf, size_t count, u_int addr, u_int msgtype, int utimeout)
{
	askISDNdev_t	*dev;
	int		len=0, sel, first, ret = 0;
	fd_set		in, exp;
	iframe_t	*ifr;
	struct timeval	tout;
#ifdef MUTEX_TIMELOCK
	struct timespec	ts;
#endif

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev) 
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev) 
	{
		errno = ENODEV;
		return(-1);
	}
	
	if (utimeout != -1) 
	{
		tout.tv_sec = utimeout/1000000;
		tout.tv_usec = utimeout%1000000;
#ifdef MUTEX_TIMELOCK
		if (utimeout == 0) 
		{
			ret = pthread_mutex_trylock(&dev->rmutex);
			if (ret) 
			{
				fprintf(stderr, "%s: mutex_trylock (%d)\n",	__FUNCTION__, ret);
				errno = ret;
				return(-1);
			}
		} 
		else 
		{
#ifdef CLOCK_REALTIME
			clock_gettime(CLOCK_REALTIME, &ts);
#else
			{
				struct timeval  tv;
				struct timezone tz;

				gettimeofday(&tv,&tz);
				TIMEVAL_TO_TIMESPEC(&tv,&ts);
			}
#endif
			ts.tv_sec += tout.tv_sec;
			ts.tv_nsec += 1000*tout.tv_usec;
			if (ts.tv_nsec > 1000000000L) 
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000L;
			}
			ret = pthread_mutex_timedlock(&dev->rmutex, &ts);
			if (ret) 
			{
				fprintf(stderr, "%s: mutex_timedlock (%d)\n", __FUNCTION__, ret);
				errno = ret;
				return(-1);
			}
		}
#else
		pthread_mutex_lock(&dev->rmutex);
#endif
	} 
	else
		pthread_mutex_lock(&dev->rmutex);

	first = 1;
	while((utimeout == -1) || tout.tv_sec || tout.tv_usec || first) 
	{
		if (!first || !(dev->iend - dev->irp)) 
		{
			FD_ZERO(&in);
			FD_SET(fid, &in);
			FD_ZERO(&exp);
			FD_SET(fid, &exp);

			if (utimeout != -1)
				sel = select(fid + 1, &in, NULL, &exp, &tout);
			else
				sel = select(fid + 1, &in, NULL, &exp, NULL);
			if (sel<0)
			{
//				fprintf(stderr, "%s: select err(%d)\n", __FUNCTION__, errno);
				ret = sel;
				goto out;
			}
			else if (sel==0)
			{
//				fprintf(stderr, "%s: select timed out\n", __FUNCTION__);
				ret = 0;
				goto out;
			}

			if (FD_ISSET(fid, &in) || FD_ISSET(fid, &exp))
			{
#if ASK_DEBUG		
				int i;
#endif
				len = dev->isize - (dev->iend - dev->irp);
				if (len<=0) 
				{
					errno = ENOSPC;
					ret = -1;
					goto out;
				}
				len = read(fid, dev->iend, len);
#if ASK_DEBUG		
//				fprintf(stderr, "%s: read %d\n", __FUNCTION__, len);
				printf("Raw data from fd (%d)\"", len);
				for(i=0;i<AS_ISDN_HEADER_LEN;i++)
				{
					printf("%x ", (int )*(dev->iend+i) );
				}
				printf("\"\n");
#endif
				if (len <= 0) 
				{
					ret = len;
					goto out;
				}
				dev->iend += len;
			} 
			else
				continue;
		}

		if (dev->iend - dev->irp) 
		{
			ifr = _ask_isdn_find_iframe(dev, addr, msgtype);
			if (ifr)
			{
				if (ifr->len > 0)
				{
#if 0
					if ((ifr->len + AS_ISDN_HEADER_LEN) > len) 
					{
						dev->irp = dev->iend;
						errno = EINVAL;
						ret = -1;
						goto out;
					}
#endif

#if ASK_DEBUG		

					fprintf(stderr, "%s--%d: read %d\n", __FUNCTION__, __LINE__, len);
#endif
					len = ifr->len + AS_ISDN_HEADER_LEN;
				}
				else
				{
#if 0//ASK_DEBUG		
					fprintf(stderr, "%s--%d: read %d\n", __FUNCTION__, __LINE__, len);
#endif
					len = AS_ISDN_HEADER_LEN;
				}
				
				if (len > count)
				{
					fprintf(stderr, "%s--%d: read %d\n", __FUNCTION__, __LINE__, len);
					errno = ENOSPC;
					ret = -1;
					goto out;
				}
				memcpy(buf, ifr, len);
#if 0//ASK_DEBUG		
				fprintf(stderr, "%s--%d: read %d\n", __FUNCTION__, __LINE__, len);
#endif
				_ask_isdn_remove_iframe(dev, ifr);
				ret = len;
				goto out;
			}
#if 0			
			else
			{
				fprintf(stderr, "%s--%d: Frame No found\n", __FUNCTION__, __LINE__);
			}
#endif			
		}
		first = 0;
	}
out:
	pthread_mutex_unlock(&dev->rmutex);
	return(ret);
}

/* wait fd writable and trylock device, then write buf into fd */
int  ask_isdn_write(int fid, void *buf, size_t count, int utimeout)
{
	askISDNdev_t	*dev;
	int		len, sel;
	fd_set		out;
	struct timeval	tout;
#ifdef MUTEX_TIMELOCK
	struct timespec	ts;
	int		ret;
#endif

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev)
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev)
	{
		errno = ENODEV;
		return(-1);
	}

	FD_ZERO(&out);
	FD_SET(fid, &out);
	if (utimeout != -1)
	{
		tout.tv_sec = utimeout/1000000;
		tout.tv_usec = utimeout%1000000;
		sel = select(fid + 1, NULL, &out, NULL, &tout);
	}
	else
		sel = select(fid + 1, NULL, &out, NULL, NULL);
	if (sel<=0)
		return(sel);
	if (!FD_ISSET(fid, &out))
		return(0);

	if (utimeout != -1)
	{
#ifdef MUTEX_TIMELOCK
		if (utimeout == 0)
		{
			ret = pthread_mutex_trylock(&dev->wmutex);
			if (ret)
			{
				fprintf(stderr, "%s: mutex_trylock (%d)\n",	__FUNCTION__, ret);
				errno = ret;
				return(-1);
			}
		}
		else
		{
#ifdef CLOCK_REALTIME
			clock_gettime(CLOCK_REALTIME, &ts);
#else
			{
				struct timeval  tv;
				struct timezone tz;

				gettimeofday(&tv,&tz);
				TIMEVAL_TO_TIMESPEC(&tv,&ts);
			}
#endif
			ts.tv_sec += tout.tv_sec;
			ts.tv_nsec += 1000*tout.tv_usec;
			if (ts.tv_nsec > 1000000000L)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000L;
			}
			ret = pthread_mutex_timedlock(&dev->wmutex, &ts);
			if (ret)
			{
				fprintf(stderr, "%s: mutex_timedlock (%d)\n", __FUNCTION__, ret);
				errno = ret;
				return(-1);
			}
		}
#else
		pthread_mutex_lock(&dev->wmutex);
#endif
	}
	else
		pthread_mutex_lock(&dev->wmutex);

	len = write(fid, buf, count);
	pthread_mutex_unlock(&dev->wmutex);
	return(len);
}

/* construct a frame and call ask_isdn_write : return 0 : success, others : failed */
int ask_isdn_write_frame(int fid, void *fbuf, u_int addr, u_int msgtype,
		int dinfo, int dlen, void *dbuf, int utimeout)
{
	iframe_t        *ifr = fbuf;
	int		len = AS_ISDN_HEADER_LEN;
	int		ret;

	if (!fbuf)
	{
		errno = EINVAL;
		return(-1);
	}
	if ((dlen > 0) && !dbuf) 
	{
		errno = EINVAL;
		return(-1);
	}

	ifr->addr = addr;
	ifr->prim = msgtype;
	ifr->dinfo= dinfo;
	ifr->len  = dlen;
	if (dlen>0) 
	{
		len += dlen;
		memcpy(&ifr->data.i, dbuf, dlen);	/* overflow ??? */
	}
	ret = ask_isdn_write(fid, ifr, len, utimeout);
	
	if (ret == len)
		ret = 0;
	else if (ret>=0)
	{
		errno = ENOSPC;
		ret = -1;
	}
	return(ret);
}

int ask_isdn_select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	askISDNdev_t	*dev = devlist;

	if (readfds)
	{
		pthread_mutex_lock(&devlist_lock);
		while(dev)
		{
			if (FD_ISSET(dev->fid, readfds))
			{
				if (dev->iend - dev->irp)
				{
					pthread_mutex_unlock(&devlist_lock);
					FD_ZERO(readfds);
					FD_SET(dev->fid, readfds);
					if (writefds)
						FD_ZERO(writefds);
					if (exceptfds)
						FD_ZERO(exceptfds);
					return(1);
				}
			}
			dev = dev->next;
		}
		pthread_mutex_unlock(&devlist_lock);
	}
	
	return(select(n, readfds, writefds, exceptfds, timeout));
}

/* automic operation for write and read */
int set_wrrd_atomic(int fid)
{
	askISDNdev_t	*dev;
	int		ret;

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev)
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev)
	{
		return(-1);
	}

	pthread_mutex_lock(&dev->rmutex);
	if (dev->Flags & FLG_KISDN_WRRD_ATOMIC)
		ret = 1;
	else
	{
		ret = 0;
		dev->Flags |= FLG_KISDN_WRRD_ATOMIC;
	}
	pthread_mutex_unlock(&dev->rmutex);

	return(ret);
}

int clear_wrrd_atomic(int fid)
{
	askISDNdev_t	*dev;
	int		ret;

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev)
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev)
	{
		return(-1);
	}

	if (dev->Flags & FLG_KISDN_WRRD_ATOMIC)
	{
		dev->Flags &= ~FLG_KISDN_WRRD_ATOMIC;
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	return(ret);
}


/* read raw frame from dev->inbuf into 'buf', this frame is not remove from dev->inbuf */
int ask_isdn_read_without_lock(int fid, void *buf, size_t count, int utimeout) 
{
	askISDNdev_t	*dev;
	int		ret = 0, len, sel;
	fd_set		in;
	iframe_t	*ifr;
	struct timeval	tout;

	pthread_mutex_lock(&devlist_lock);
	dev = devlist;
	while(dev) 
	{
		if (dev->fid==fid)
			break;
		dev = dev->next;
	}
	pthread_mutex_unlock(&devlist_lock);

	if (!dev) 
	{
		errno = ENODEV;
		return(-1);
	}
	
	if (utimeout != -1) 
	{
		tout.tv_sec = utimeout/1000000;
		tout.tv_usec = utimeout%1000000;
	} 

	if (dev->Flags & FLG_KISDN_WRRD_ATOMIC) 
	{
//		fprintf(stderr, "%s: WRRD_ATOMIC try again\n", __FUNCTION__);
		errno = EAGAIN;
		ret = -3;
		goto out;
	}
	len = dev->iend - dev->irp;

	/* no frame is available in dev->inbuf, so read data from fd; else read data from dev->inbuf */
	if (!len)
	{
		dev->irp = dev->iend = dev->inbuf;
		FD_ZERO(&in);
		FD_SET(fid, &in);
		if (utimeout != -1)
		{
			sel = select(fid + 1, &in, NULL, NULL, &tout);
		}
		else
			sel = select(fid + 1, &in, NULL, NULL, NULL);

		if (sel<0)
		{
//			fprintf(stderr, "%s: select err(%d)\n", __FUNCTION__, errno);
			return(sel);
		}
		else if (sel==0)
		{
//			fprintf(stderr, "%s: select timed out\n", __FUNCTION__);
			return(0);
		}

		if (FD_ISSET(fid, &in))
		{
			len = dev->isize  - (dev->iend - dev->irp);
			if (len<=0)
			{
				fprintf(stderr, "%s-%d :buffer error\n", __FUNCTION__, __LINE__);
				errno = ENOSPC;
				ret = -5;
				goto out;
			}
			if (dev->Flags & FLG_KISDN_WRRD_ATOMIC)
			{
				fprintf(stderr, "%s: WRRD_ATOMIC try again\n", __FUNCTION__);
				errno = EAGAIN;
				ret = -6;
				goto out;
			}
			len = read(fid, dev->iend, len);
//			fprintf(stderr, "%s: read %d\n", __FUNCTION__, len);
			if (len <= 0)
			{
				ret = len;
				goto out;
			}
#if ASK_DEBUG
			printf("Data read from File descriptor :\r\n");
			ask_hex_dump(dev->iend,  len);
#endif
			dev->iend += len;
			len = dev->iend - dev->irp;
		}
		else
		{
			return(0);
		}
	}

	/* read a frame from dev->inbuf */
	if (len < AS_ISDN_HEADER_LEN)
	{/* len is too small, so overwrite it and continue(no data is copied) */
		dev->iend = dev->irp;
		fprintf(stderr, "%s: frame too short:%d\n",	__FUNCTION__, len);
#if 0		
		errno = EINVAL;
		ret = -7;
		goto out;
#endif
		errno = EAGAIN;
		return 0;
	}
	
	ifr = (iframe_t *)dev->irp;
	if (ifr->len > 0)
	{
		if ((ifr->len + AS_ISDN_HEADER_LEN) > len)
		{
			fprintf(stderr, "%s: frame too longer : data length %d, frame length %d\n",	__FUNCTION__, len, ifr->len );

#if ASK_DEBUG		
			ask_hex_dump(dev->irp, len);
#endif
#if 0
			dev->iend = dev->irp;
			errno = EINVAL;
			ret = -8;
			goto out;
#endif			
			memcpy(buf, dev->irp, len);
			dev->irp = dev->iend = dev->inbuf;
			return len;
		}
		len = ifr->len + AS_ISDN_HEADER_LEN;
	} 
	else
		len = AS_ISDN_HEADER_LEN;

	if (len>count)
	{
		errno = ENOSPC;
		ret = -9;
		goto out;
	}
	memcpy(buf, dev->irp, len);
	dev->irp += len;
	ret = len;
out:

	return(ret);
}


/* wait fd writable, then write buf into fd */
int  ask_isdn_write_without_lock(int fid, void *buf, size_t count, int utimeout)
{
	int		len, sel;
	fd_set		out;
	struct timeval	tout;

	FD_ZERO(&out);
	FD_SET(fid, &out);
	if (utimeout != -1)
	{
		tout.tv_sec = utimeout/1000000;
		tout.tv_usec = utimeout%1000000;
		sel = select(fid + 1, NULL, &out, NULL, &tout);
	}
	else
		sel = select(fid + 1, NULL, &out, NULL, NULL);
	if (sel<=0)
		return(sel);
	if (!FD_ISSET(fid, &out))
		return(0);

	len = write(fid, buf, count);

	return(len);
}

