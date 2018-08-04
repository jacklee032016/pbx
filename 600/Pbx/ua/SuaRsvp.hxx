/*
 * $Id: SuaRsvp.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#ifndef SUARSVP_H
#define SUARSVP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* for sockaddr_in, in_addr in rapi_lib.h */
#include "rapi_lib.h"

#include <stdio.h>
#include "NetworkAddress.h"

int upcallHandler(rapi_sid_t, rapi_eventinfo_t, int,
                  int, int, struct sockaddr *, u_char,
                  int, rapi_filter_t *, int, rapi_flowspec_t *,
                  int, rapi_adspec_t *, void *);


class SuaRsvp
{
    public:
        SuaRsvp ();

        SuaRsvp ( const SuaRsvp& src );

        SuaRsvp& operator =(const SuaRsvp& src);

        int session(NetworkAddress* destAddr);

        int sender(NetworkAddress* srcAddr);

        int reserve(NetworkAddress* rcvAddr);

        int close();

    public:

        int session_id;

        // proto
        int proto_id;

        struct sockaddr dest_addr;
        struct sockaddr src_addr;
        struct sockaddr rcv_addr;

        // reservation style
        rapi_styleid_t style;

        rapi_filter_t snd_template;
        rapi_tspec_t snd_tspec;
        rapi_adspec_t snd_adspec;

        rapi_flowspec_t flowspec_cl;
        rapi_flowspec_t flowspec_g;
        rapi_filter_t filter_spec;

        int ttl;

        int resv_flag;
        int rapi_fd;


};

#endif
