#ifndef __SONIX_NETWORK_LIB_H__
#define __SONIX_NETWORK_LIB_H__

/*
 * Add a new DNS to the system
 * Note:
 *	You must check if the dns_name is valid.
 *	If the dns_name is null, -1 will be return.
 * Return value:
 *	0  - success
 *	-1 - failure 
 */
int set_dns(char *dns_name);

int get_ip(char *ip_addr);

/*
 * Set IP and net mask of eth0
 * Note:
 *	If the ip_address is NULL, -2 will be return.
 *  If the set_mask is NULL, "255.255.255.0" will be set.
 * return value: 
 *	 0 - success
 *  -1 - socket error
 *	-2 - set ip failure
 *  -3 - set netmask failure
 */
int set_ip_netmask(char *ip_address, char *net_mask);

#endif
