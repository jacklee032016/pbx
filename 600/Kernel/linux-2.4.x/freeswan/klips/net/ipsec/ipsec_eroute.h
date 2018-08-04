/*
 * @(#) declarations of eroute structures
 *
 * Copyright (C) 1996, 1997  John Ioannidis.
 * Copyright (C) 1998, 1999, 2000, 2001  Richard Guy Briggs <rgb@freeswan.org>
 * Copyright (C) 2001                    Michael Richardson <mcr@freeswan.org>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: ipsec_eroute.h,v 1.1.1.1 2006/11/29 08:55:38 lizhijie Exp $
 *
 * derived from ipsec_encap.h 1.15 on 2001/9/18 by mcr.
 *
 */

#ifndef _IPSEC_EROUTE_H_

#include "radij.h"
#include "ipsec_encap.h"
#include "ipsec_radij.h"

/*
 * The "type" is really part of the address as far as the routing
 * system is concerned. By using only one bit in the type field
 * for each type, we sort-of make sure that different types of
 * encapsulation addresses won't be matched against the wrong type.
 */

/*
 * An entry in the radix tree 
 */

struct rjtentry
{
	struct	radij_node rd_nodes[2];	/* tree glue, and other values */
#define	rd_key(r)	((struct sockaddr_encap *)((r)->rd_nodes->rj_key))
#define	rd_mask(r)	((struct sockaddr_encap *)((r)->rd_nodes->rj_mask))
	short	rd_flags;
	short	rd_count;
};

struct ident
{
	__u16	type;	/* identity type */
	__u64	id;	/* identity id */
	__u8	len;	/* identity len */
	caddr_t	data;	/* identity data */
};

/*
 * An encapsulation route consists of a pointer to a 
 * radix tree entry and a SAID (a destination_address/SPI/protocol triple).
 */

struct eroute
{
	struct rjtentry er_rjt;
	struct sa_id er_said;
	uint32_t er_pid;
	uint32_t er_count;
	uint64_t er_lasttime;
	struct sockaddr_encap er_eaddr; /* MCR get rid of _encap, it is silly*/
	struct sockaddr_encap er_emask;
        struct ident er_ident_s;
        struct ident er_ident_d;
	struct sk_buff* er_first;
	struct sk_buff* er_last;
};

#define er_dst er_said.dst
#define er_spi er_said.spi
#define er_proto er_said.proto

#define _IPSEC_EROUTE_H_
#endif /* _IPSEC_EROUTE_H_ */

/*
 * $Log: ipsec_eroute.h,v $
 * Revision 1.1.1.1  2006/11/29 08:55:38  lizhijie
 * AS600 Kernel
 *
 * Revision 1.1.1.1  2005/07/08 09:46:23  lizhijie
 * Linux-2.4.24 integrated Intel Access Library 2.0
 *
 * Revision 1.1.1.1  2004/08/17 02:50:02  lizhijie
 * linux-2.4.24 for IXP422
 *
 * Revision 1.2  2001/11/26 09:16:13  rgb
 * Merge MCR's ipsec_sa, eroute, proc and struct lifetime changes.
 *
 * Revision 1.1.2.1  2001/09/25 02:18:54  mcr
 * 	struct eroute moved to ipsec_eroute.h
 *
 *
 * Local variables:
 * c-file-style: "linux"
 * End:
 *
 */
