/******************************************************************************
 *
 * Name:	skgehwt.c
 * Project:	Gigabit Ethernet Adapters, Common Modules
 * Version:	$Revision: 1.1.1.1 $
 * Date:	$Date: 2006/11/30 16:28:26 $
 * Purpose:	Hardware Timer.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *	(C)Copyright 1998-2002 SysKonnect GmbH.
 *	(C)Copyright 2002-2003 Marvell.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	The information in this file is provided "AS IS" without warranty.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * History:
 *
 *	$Log: skgehwt.c,v $
 *	Revision 1.1.1.1  2006/11/30 16:28:26  lizhijie
 *	AS800 Kernel
 *	
 *	Revision 1.1.1.1  2006/01/10 06:01:07  lizhijie
 *	kernel and driver for DUX project
 *	
 *	Revision 1.1.1.1  2005/07/08 09:46:11  lizhijie
 *	Linux-2.4.24 integrated Intel Access Library 2.0
 *	
 *	Revision 1.1.1.1  2004/08/17 02:49:55  lizhijie
 *	linux-2.4.24 for IXP422
 *	
 *	Revision 1.14  2003/05/13 18:01:58  mkarl
 *	Editorial changes.
 *	
 *	Revision 1.13  1999/11/22 13:31:12  cgoos
 *	Changed license header to GPL.
 *	
 *	Revision 1.12  1998/10/15 15:11:34  gklug
 *	fix: ID_sccs to SysKonnectFileId
 *	
 *	Revision 1.11  1998/10/08 15:27:51  gklug
 *	chg: correction factor is host clock dependent
 *	
 *	Revision 1.10  1998/09/15 14:18:31  cgoos
 *	Changed more BOOLEANs to SK_xxx
 *
 *	Revision 1.9  1998/09/15 14:16:06  cgoos
 *	Changed line 107: FALSE to SK_FALSE
 *	
 *	Revision 1.8  1998/08/24 13:04:44  gklug
 *	fix: typo
 *	
 *	Revision 1.7  1998/08/19 09:50:49  gklug
 *	fix: remove struct keyword from c-code (see CCC) add typedefs
 *	
 *	Revision 1.6  1998/08/17 09:59:02  gklug
 *	fix: typos
 *	
 *	Revision 1.5  1998/08/14 07:09:10  gklug
 *	fix: chg pAc -> pAC
 *	
 *	Revision 1.4  1998/08/10 14:14:52  gklug
 *	rmv: unneccessary SK_ADDR macro
 *	
 *	Revision 1.3  1998/08/07 12:53:44  gklug
 *	fix: first compiled version
 *	
 *	Revision 1.2  1998/08/07 09:19:29  gklug
 *	adapt functions to the C coding conventions
 *	rmv unneccessary functions.
 *	
 *	Revision 1.1  1998/08/05 11:28:36  gklug
 *	first version: adapted from SMT/FDDI
 *	
 *	
 *	
 *
 ******************************************************************************/


/*
	Event queue and dispatcher
*/
#if (defined(DEBUG) || ((!defined(LINT)) && (!defined(SK_SLIM))))
static const char SysKonnectFileId[] =
	"$Header: /CVS/CVS/ixp_works/as800_Kernel/linux-2.4.x/drivers/net/sk98lin/skgehwt.c,v 1.1.1.1 2006/11/30 16:28:26 lizhijie Exp $" ;
#endif

#include "h/skdrv1st.h"		/* Driver Specific Definitions */
#include "h/skdrv2nd.h"		/* Adapter Control- and Driver specific Def. */

#ifdef __C2MAN__
/*
	Hardware Timer function queue management.

	General Description:

 */
intro()
{}
#endif

/*
 * Prototypes of local functions.
 */
#define	SK_HWT_MAX	(65000)

/* correction factor */
#define	SK_HWT_FAC	(1000 * (SK_U32)pAC->GIni.GIHstClkFact / 100)

/*
 * Initialize hardware timer.
 *
 * Must be called during init level 1.
 */
void	SkHwtInit(
SK_AC	*pAC,	/* Adapters context */
SK_IOC	Ioc)	/* IoContext */
{
	pAC->Hwt.TStart = 0 ;
	pAC->Hwt.TStop	= 0 ;
	pAC->Hwt.TActive = SK_FALSE ;

	SkHwtStop(pAC,Ioc) ;
}

/*
 *
 * Start hardware timer (clock ticks are 16us).
 *
 */
void	SkHwtStart(
SK_AC	*pAC,	/* Adapters context */
SK_IOC	Ioc,	/* IoContext */
SK_U32	Time)	/* Time in units of 16us to load the timer with. */
{
	SK_U32	Cnt ;

	if (Time > SK_HWT_MAX)
		Time = SK_HWT_MAX ;

	pAC->Hwt.TStart = Time ;
	pAC->Hwt.TStop = 0L ;

	Cnt = Time ;

	/*
	 * if time < 16 us
	 *	time = 16 us
	 */
	if (!Cnt) {
		Cnt++ ;
	}

	SK_OUT32(Ioc, B2_TI_INI, Cnt * SK_HWT_FAC) ;
	SK_OUT16(Ioc, B2_TI_CRTL, TIM_START) ;	/* Start timer. */

	pAC->Hwt.TActive = SK_TRUE ;
}

/*
 * Stop hardware timer.
 * and clear the timer IRQ
 */
void	SkHwtStop(
SK_AC	*pAC,	/* Adapters context */
SK_IOC	Ioc)	/* IoContext */
{
	SK_OUT16(Ioc, B2_TI_CRTL, TIM_STOP) ;
	SK_OUT16(Ioc, B2_TI_CRTL, TIM_CLR_IRQ) ;

	pAC->Hwt.TActive = SK_FALSE ;
}


/*
 *	Stop hardware timer and read time elapsed since last start.
 *
 * returns
 *	The elapsed time since last start in units of 16us.
 *
 */
SK_U32	SkHwtRead(
SK_AC	*pAC,	/* Adapters context */
SK_IOC	Ioc)	/* IoContext */
{
	SK_U32	TRead ;
	SK_U32	IStatus ;

	if (pAC->Hwt.TActive) {
		SkHwtStop(pAC,Ioc) ;

		SK_IN32(Ioc, B2_TI_VAL, &TRead);
		TRead /= SK_HWT_FAC;

		SK_IN32(Ioc, B0_ISRC, &IStatus);

		/* Check if timer expired (or wraparound). */
		if ((TRead > pAC->Hwt.TStart) || (IStatus & IS_TIMINT)) {
			SkHwtStop(pAC,Ioc) ;
			pAC->Hwt.TStop = pAC->Hwt.TStart ;
		} else {
			pAC->Hwt.TStop = pAC->Hwt.TStart - TRead ;
		}
	}
	return (pAC->Hwt.TStop) ;
}

/*
 * interrupt source= timer
 */
void	SkHwtIsr(
SK_AC	*pAC,	/* Adapters context */
SK_IOC	Ioc)	/* IoContext */
{
	SkHwtStop(pAC,Ioc);
	pAC->Hwt.TStop = pAC->Hwt.TStart;
	SkTimerDone(pAC,Ioc) ;
}

/* End of file */
