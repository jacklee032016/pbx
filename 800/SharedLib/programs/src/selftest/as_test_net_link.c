#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>

#include <netinet/in.h>

#include <errno.h>
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "as_test.h"

#define AS_FAIL 	-1

#define __BUF_SIZE		2048

#define TIMEOUT_5_SECONDS		3

typedef enum
{
	DLT_NULL =	0,	/* no link-layer encapsulation */
	DLT_EN10MB ,	/* Ethernet (10Mb) */
	DLT_EN3MB,		/* Experimental Ethernet (3Mb) */
	DLT_AX25,		/* Amateur Radio AX.25 */
	DLT_PRONET,	/* Proteon ProNET Token Ring */
	DLT_CHAOS,		/* Chaos */
	DLT_IEEE802,	/* IEEE 802 Networks */
	DLT_ARCNET,	/* ARCNET */
	DLT_SLIP,		/* Serial Line IP */
	DLT_PPP,		/* Point-to-point Protocol */
	DLT_FDDI,		/* FDDI */
	DLT_ATM_RFC1483,	/* LLC/SNAP encapsulated atm */
	DLT_RAW,			/* raw IP */
	DLT_SLIP_BSDOS,	/* BSD/OS Serial Line IP */
	DLT_PPP_BSDOS		/* BSD/OS Point-to-point Protocol */
}as_link_type;


static char *as_link_types[] = 
{
	"no link-layer encapsulation",
	"Ethernet (100Mb)",
	"Experimental Ethernet (3Mb)",
	"Amateur Radio AX.25",
	"Proteon ProNET Token Ring",
	"Chaos",
	"IEEE 802 Networks",
	"ARCNET",
	"Serial Line IP",
	"Point-to-point Protocol",
	"FDDI",
	"LLC/SNAP encapsulated atm",
	"raw IP",
	"BSD/OS Serial Line IP",
	"BSD/OS Point-to-point Protocol"
};

typedef struct
{
	char		dev[64];
	int fd;
}as_link_device;

static as_link_device dev;

static void __as_link_type_debug(as_link_type type)
{
	printf("\tLink Layer type \t: %s\r\n", as_link_types[type]);
}

static void __as_link_flags_debug(short int flags)
{
	printf("\tStatus \t\t\t: %s\r\n", (flags&IFF_UP)? "Up" : "Down");
	printf("\tBroadcast mode \t\t: %s\r\n", (flags&IFF_BROADCAST)? "Enable" : "Disable");
	printf("\tLoopback Device \t: %s\r\n", (flags&IFF_LOOPBACK)? "Yes" : "No");
	printf("\tP2P Device \t\t: %s\r\n", (flags&IFF_POINTOPOINT)? "Yes" : "No");
	printf("\tResource Allocate \t: %s\r\n", (flags&IFF_RUNNING)? "Yes" : "No");
	printf("\tARP Support \t\t: %s\r\n", (flags&IFF_NOARP)? "No" : "Yes");
	printf("\tPromisc mode \t\t: %s\r\n", (flags&IFF_PROMISC)? "Enable" : "Disable");
	printf("\tAuto Negotiate \t\t: %s\r\n", (flags&IFF_AUTOMEDIA)? "Yes" : "No");
}

static int __as_link_open( as_link_device *dev)
{
	struct ifreq ifr;
	struct sockaddr sa;
	as_link_type type;
	unsigned char *address;

	assert(dev);
	dev->fd = socket(PF_INET, SOCK_PACKET, htons(ETH_P_ALL));
	if (dev->fd < 0) 
	{
		printf( "Link Layer socket Failed: %s\r\n", strerror(errno));
		return AS_FAIL;
	}

	/* Bind to the interface name */
	memset(&sa, 0, sizeof(sa));
	sa.sa_family = AF_INET;
	(void)strncpy(sa.sa_data, dev->dev, sizeof(sa.sa_data));
	if (bind( dev->fd, &sa, sizeof(sa))) 
	{
		printf( "Link Layer socket bind: %s: %s\r\n", dev->dev, strerror(errno));
		goto bad;
	}

	printf("Network Dvice '%s' Info : \r\n", dev->dev);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev->dev, sizeof(ifr.ifr_name));
	if (ioctl( dev->fd, SIOCGIFHWADDR, &ifr) < 0 ) 
	{
		printf( "SIOCGIFHWADDR: %s", strerror(errno));
		goto bad;
	}

	switch (ifr.ifr_hwaddr.sa_family) 
	{
		case ARPHRD_ETHER:
		case ARPHRD_METRICOM:
			type= DLT_EN10MB;
			break;
		case ARPHRD_EETHER:
			type = DLT_EN3MB;
			break;
		case ARPHRD_AX25:
			type = DLT_AX25;
			break;
		case ARPHRD_PRONET:
			type = DLT_PRONET;
			break;
		case ARPHRD_CHAOS:
			type = DLT_CHAOS;
			break;
		case ARPHRD_IEEE802:
			type = DLT_IEEE802;
			break;
		case ARPHRD_ARCNET:
			type = DLT_ARCNET;
			break;
		case ARPHRD_SLIP:
		case ARPHRD_CSLIP:
		case ARPHRD_SLIP6:
		case ARPHRD_CSLIP6:
		case ARPHRD_PPP:
			type = DLT_RAW;
			break;
		case ARPHRD_LOOPBACK:
			type = DLT_NULL;
			break;
		default:
			printf( "\tunknown physical layer type 0x%x\r\n",  ifr.ifr_hwaddr.sa_family);
			type = DLT_NULL;
	}
	__as_link_type_debug( type);
	address = ifr.ifr_hwaddr.sa_data;
	printf("\tHardware Address \t: %2x:%2x:%2x:%2x:%2x:%2x\r\n", 
		*address, *(address+1), *(address+2),*(address+3),*(address+4),*(address+5)  );

	/* Base the buffer size on the interface MTU */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev->dev, sizeof(ifr.ifr_name));
	if (ioctl( dev->fd, SIOCGIFMTU, &ifr) < 0 ) 
	{
		printf( "SIOCGIFMTU: %s\r\n", strerror(errno));
		goto bad;
	}
	printf("\tMTU(MaxiumTransmitUnit)\t: %d\r\n", ifr.ifr_mtu);
	/* Leave room for link header (which is never large under linux...) */

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, dev->dev);
	if (ioctl( dev->fd, SIOCGIFFLAGS, &ifr) < 0 ) 
	{
		printf( "SIOCGIFFLAGS: %s\r\n", strerror(errno));
		goto bad;
	}
	__as_link_flags_debug(ifr.ifr_flags );

	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl( dev->fd, SIOCSIFFLAGS, &ifr) < 0 ) 
	{
		printf( "SIOCSIFFLAGS: %s", strerror(errno));
		goto bad;
	}

	return 0;
bad:
	close(dev->fd);
	dev->fd = -1;
	
	return AS_FAIL;
}

static int  __as_link_read( as_link_device *dev)
{
	int cc;
	int res;
	unsigned char bp[__BUF_SIZE];
	int bufsize = __BUF_SIZE;
	struct sockaddr from;
	int fromlen;
	fd_set	fs_read;
	struct	timeval tv_timeout;

	assert(dev);
	
	FD_ZERO (&(fs_read) );
	FD_SET ( dev->fd, &(fs_read) );
	fromlen = sizeof(from);
	tv_timeout.tv_sec = TIMEOUT_5_SECONDS;
	tv_timeout.tv_usec = 0; /*millseconds/1000;*/
	
again:
	do 
	{
		res = select ( dev->fd + 1, &(fs_read), NULL, NULL, &(tv_timeout) );
		if(res == 0 )
		{
			printf("Timeout about %ld seconds when read network device \"%s\"\r\n", tv_timeout.tv_sec, dev->dev);
			return AS_FAIL;
		}
		else  if(res <0 )
		{
			printf("Failed when read network device \"%s\", error is %s\r\n", dev->dev, strerror(errno) );
			return AS_FAIL;
		}
		cc = recvfrom( dev->fd, bp, bufsize, 0, &from, &fromlen);
		if (cc < 0) 
		{
			/* Don't choke when we get ptraced */
			switch (errno) 
			{
				case EINTR:
					goto again;

				case EWOULDBLOCK:
					return (0);		/* XXX */
			}
			printf( "read: %s", strerror(errno));
			close(dev->fd);
			dev->fd = -1;
			return AS_FAIL;
		}
//	printf("read %d bytes from %s\r\n", cc, from.sa_data);	
	} while (strcmp(dev->dev, from.sa_data));

//	printf("read %d bytes from %s\r\n Buf is \"%s\"\r\n", cc, dev->dev, bp);	
	printf("Test Result \t: \"%s is successful!\"\r\n",  bp);	

	return cc;
}

static int  __as_link_write(as_link_device *dev)
{
	int cc;
	unsigned char bp[__BUF_SIZE];
	struct sockaddr dest;
	int addrlen;

	assert(dev);
	memset(&dest, 0, sizeof(dest));
	dest.sa_family = AF_INET;
	(void)strncpy(dest.sa_data, dev->dev, sizeof(dest.sa_data));

	sprintf(bp, "Assist network device test on %s", dev->dev);

	addrlen = sizeof(dest);
	cc = sendto(dev->fd, bp, strlen(bp), 0, &dest, addrlen);
	if (cc < 0) 
	{
		printf( "write: %s\r\n", strerror(errno));
		close(dev->fd);
		dev->fd = -1;
		return AS_FAIL;
	};

	return (0);
}

static int __as_test_eth_device( char *device_name)
{
	sprintf(dev.dev, "%s", device_name);
	if( __as_link_open(&dev)<0 )
		return AS_FAIL;

	if( __as_link_write(&dev)<0 )
		return AS_FAIL;

	if( __as_link_read(&dev)<0 )
		return AS_FAIL;
	close(dev.fd);
	dev.fd = -1;

	return 0;
}

#if 0
int main(int argc, char *argv[])
{
	__as_test_eth_device("eth0");
	__as_test_eth_device("eth1");
	__as_test_eth_device("lo");
	return 0;
}
#endif

int as_test_dev_eth0(void *data)
{
	int res = __as_test_eth_device("eth0");
	if(res < 0)
		return -AS_TEST_ETH0_FAIL;
	return res;
}

int as_test_dev_eth1(void *data)
{
	int res =  __as_test_eth_device("eth1");
	if(res <0 )
		return -AS_TEST_ETH1_FAIL;
	return res;
}
#if ETH2_SUPPORT
int as_test_dev_eth2(void *data)
{
	int res =  __as_test_eth_device("eth2");
	if(res <0 )
		return -AS_TEST_ETH2_FAIL;
	return res;
}
#endif

