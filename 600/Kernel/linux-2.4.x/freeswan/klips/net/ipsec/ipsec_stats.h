/*
 * @(#) definition of ipsec_stats structure
 *
 * Copyright (C) 2001  Richard Guy Briggs  <rgb@freeswan.org>
 *                 and Michael Richardson  <mcr@freeswan.org>
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
 * RCSID $Id: ipsec_stats.h,v 1.1.1.1 2006/11/29 08:55:38 lizhijie Exp $
 *
 */

/* 
 * This file describes the errors/statistics that FreeSWAN collects.
 */

#ifndef _IPSEC_STATS_H_

struct ipsec_stats {
	__u32		ips_alg_errs;	       /* number of algorithm errors */
	__u32		ips_auth_errs;	       /* # of authentication errors */
	__u32		ips_encsize_errs;      /* # of encryption size errors*/
	__u32		ips_encpad_errs;       /* # of encryption pad  errors*/
	__u32		ips_replaywin_errs;    /* # of pkt sequence errors */
};

#define _IPSEC_STATS_H_
#endif /* _IPSEC_STATS_H_ */

/*
 * $Log: ipsec_stats.h,v $
 * Revision 1.1.1.1  2006/11/29 08:55:38  lizhijie
 * AS600 Kernel
 *
 * Revision 1.1.1.1  2005/07/08 09:46:23  lizhijie
 * Linux-2.4.24 integrated Intel Access Library 2.0
 *
 * Revision 1.1.1.1  2004/08/17 02:50:02  lizhijie
 * linux-2.4.24 for IXP422
 *
 * Revision 1.2  2001/11/26 09:16:16  rgb
 * Merge MCR's ipsec_sa, eroute, proc and struct lifetime changes.
 *
 * Revision 1.1.2.1  2001/09/25 02:27:00  mcr
 * 	statistics moved to seperate structure.
 *
 *
 *
 * Local variables:
 * c-file-style: "linux"
 * End:
 *
 */
