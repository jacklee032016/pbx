#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>

int get_ip(char *ip_addr)
{
	struct ifreq ifr;
	struct sockaddr_in *addr;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		return -1;

	memset(&ifr, 0, sizeof(ifr));
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	
	addr->sin_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
	if (ioctl(sfd, SIOCGIFADDR, &ifr) < 0)
		return -1;
	strcpy(ip_addr, inet_ntoa(addr->sin_addr));
	return 0;
}

/*
 * Set IP and net mask of eth0
 * return value: 
 *	 0 - success
 *  -1 - socket error
 *	-2 - set ip failure
 *  -3 - set netmask failure
 */
int set_ip_netmask(char *ip_address, char *net_mask)
{
	struct ifreq ifr;
	struct sockaddr_in *addr;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		return -1;

	memset(&ifr, 0, sizeof(ifr));
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	
	addr->sin_family = AF_INET;
	if (ip_address == NULL)
		return -2;
	addr->sin_addr.s_addr = inet_addr(ip_address);
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
	if (ioctl(sfd, SIOCSIFADDR, &ifr) < 0)
		return -2;

	if (net_mask)
		addr->sin_addr.s_addr = inet_addr(net_mask);
	else
		addr->sin_addr.s_addr = inet_addr("255.255.255.0");
	if (ioctl(sfd, SIOCSIFNETMASK, &ifr) < 0)
		return -3;
	return 0;
}
