/* conn.c 
 *
 * Copyright 2004,Philippe Stellwag <linux@mp3s.name>, Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Publice License,
 * version 2 or any later.
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "conn.h"

#ifndef NONETDB
    #include <netdb.h>
#endif

#ifdef hpux
    #define bcopy(a,b,c) memcpy(b,a,c)
#endif

#ifndef NONETDB
    extern char *inet_ntoa();
    extern u_long inet_addr();
#else

/* inet_addr for EXCELAN (which does not have it!) */
u_long inet_addr(register char *cp)
{
    u_long val,base,n;
    register char c;
    u_long octet[4],*octetptr = octet;

    #ifndef htonl
        extern u_long htonl();
    #endif

again:
    /* Collect number up to '.'
     * Values are specified as for C:
     * 0x=hex,0=octal,other=decimal.
     */
    val = 0; base = 10;
    if (*cp == '0')
        base = 8,cp++;
    if (*cp == 'x' || *cp == 'X')
        base = 16,cp++;
    while (c = *cp) {
        if (isdigit(c)) {
            val = (val * base) + (c - '0');
            cp++;
            continue;
        }
        if (base == 16 && isxdigit(c)) {
            val = (val << 4) + (c + 10 - (islower(c) ? 'a' : 'A'));
            cp++;
            continue;
        }
        break;
    }
    if (*cp == '.') {
        /* Internet format:
         *    a.b.c.d
         *    a.b.c   (with c treated as 16-bits)
         *    a.b     (with b treated as 24 bits)
         */
        if (octetptr >= octet + 4)
            return (-1);
        *octetptr++ = val,cp++;
        goto again; /* dirty but must be (: */
    }

    /* Check for trailing characters. */
    if (*cp && !isspace(*cp))
        return (-1);
    *octetptr++ = val;

    /* Concoct the address according to
     * the number of octet specified.
     */
    n = octetptr - octet;
    switch (n) {

    case 1:                /* a -- 32 bits */
        val = octet[0];
        break;

    case 2:                /* a.b -- 8.24 bits */
        val = (octet[0] << 24) | (octet[1] & 0xffffff);
        break;

    case 3:                /* a.b.c -- 8.8.16 bits */
        val = (octet[0] << 24) | ((octet[1] & 0xff) << 16) |
            (octet[2] & 0xffff);
        break;

    case 4:                /* a.b.c.d -- 8.8.8.8 bits */
        val = (octet[0] << 24) | ((octet[1] & 0xff) << 16) |
              ((octet[2] & 0xff) << 8) | (octet[3] & 0xff);
        break;

    default:
        return (-1);
    }
    val = htonl(val);
    return (val);
}

char *inet_ntoa(struct in_addr in)
{
    static char address[20];

    sprintf(address,"%u.%u.%u.%u",
             (in.s_addr>>24)&0xff,
             (in.s_addr>>16)&0xff,
             (in.s_addr>>8 )&0xff,
             (in.s_addr    )&0xff);
    return(address);
}
#endif

#ifdef USG
void bcopy(register caddr_t s,register caddr_t d,register int l)
{
    while (l-- > 0) *d++ = *s++;
}
#endif

/* Take the name of an internet host in ASCII (this may either be its
 * official host name or internet number (with or without enclosing
 * backets [])), and return a list of internet addresses.
 *
 * returns NULL for failure to find the host name in the local database,
 * or for a bad internet address spec.
 */
u_long **name_to_address(char *host)
{
    static u_long *host_addresses[2];
    static u_long haddr;

    if (host == (char *)NULL) {
        return((u_long **)NULL);
    }

    host_addresses[0] = &haddr;
    host_addresses[1] = (u_long *)NULL;

    /* Is this an ASCII internet address? (either of [10.0.0.65] or
     * 10.0.0.65). We get away with the second test because hostnames
     * and domain labels are not allowed to begin in numbers.
     * (cf. RFC952, RFC882).
     */
    if (*host == '[' || isdigit(*host)) {
        char    namebuf[128];
        register char    *cp = namebuf;

        /* strip brackets [] or anything else we don't want. */
        while(*host != '\0' && cp < &namebuf[sizeof(namebuf)]) {
            if (isdigit(*host) || *host == '.')
                *cp++ = *host++;    /* copy */
            else
                host++;            /* skip */
        }
        *cp = '\0';
        haddr = inet_addr(namebuf);
        return(&host_addresses[0]);
    } else {
        #ifdef NONETDB
            extern u_long rhost();

            /* lint is gonna bitch about this (comparing an unsigned?!) */
            if ((haddr = rhost(&host)) == FAIL)
                return((u_long **)NULL);    /* no such host */
            return(&host_addresses[0]);
        #else
            struct hostent    *hstp = gethostbyname(host);

            if (hstp == NULL) {
                return((u_long **)NULL);    /* no such host */
            }

            if (hstp->h_length != sizeof(u_long))
                abort();    /* this is fundamental */

            #ifndef h_addr
                /* alignment problems (isn't dbm wonderful?) */
                bcopy((caddr_t)hstp->h_addr,(caddr_t)&haddr,sizeof(haddr));
                return(&host_addresses[0]);
            #else
                return((u_long **)hstp->h_addr_list);
            #endif
        #endif
    }
}

/* Get a service port number from a service name (or ASCII number)
 * Return zero if something is wrong (that's a reserved port)
 */
#ifdef NONETDB
static struct Services {
    char *name;
    u_short port;
} Services[] = {
    {"nntp",   IPPORT_NNTP},       /* RFC977 */
    {"smtp",   IPPORT_SMTP},       /* RFC821 */
    {"name",   IPPORT_NAMESERVER}, /* RFC881,RFC882,RFC883 */
    {"time",   IPPORT_TIMESERVER}, /* RFC868 */
    {"echo",   IPPORT_ECHO},       /* RFC862 */
    {"discard",IPPORT_DISCARD},    /* RFC863 */
    {"daytime",IPPORT_DAYTIME},    /* RFC867 */
    {"login",  IPPORT_LOGINSERVER},/* N/A - 4BSD specific */
};
#endif

u_short gservice(char *serv,char *proto)
{
    if (serv == (char *)NULL || proto == (char *)NULL)
        return((u_short)0);

    if (isdigit(*serv)) {
        return(htons((u_short)(atoi(serv))));
    } else {
        #ifdef NONETDB
            register int i;

            for(i = 0; i < (sizeof(Services) / sizeof(struct Services)); i++) {
                if (strcmp(serv,Services[i].name) == 0)
                    return(htons(Services[i].port));
            }
            return((u_short)0);
        #else
            struct servent    *srvp = getservbyname(serv,proto);

            if (srvp == (struct servent *)NULL)
                return((u_short)0);
            return((u_short)srvp->s_port);
        #endif
    }
}

/* given a host name (either name or internet address) and service name
 * (or port number) (both in ASCII), give us a TCP connection to the
 * requested service at the requested host (or give us FAIL).
 */
int get_tcp_conn(char *host,char *serv)
{
    register int sock;
    u_long **addrlist;
    struct sockaddr_in sadr;


    if ((addrlist = name_to_address(host)) == (u_long **)NULL) {
        return(NOHOST);
    }

    sadr.sin_family = (u_short)AF_INET;    /* Only internet for now */
    if ((sadr.sin_port = gservice(serv,"tcp")) == 0)
        return(NOSERVICE);

    for(; *addrlist != (u_long *)NULL; addrlist++) {
        bcopy((caddr_t)*addrlist,(caddr_t)&sadr.sin_addr,sizeof(sadr.sin_addr));

				if ((sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
						return(FAIL);

				if (connect(sock,(struct sockaddr *)&sadr,sizeof(sadr)) < 0) {
						perror("connect");
						(void) close(sock);    /* dump descriptor */
						return errno;
				} else
						return(sock);
    }
    return(FAIL);
}
