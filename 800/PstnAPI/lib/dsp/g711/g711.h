/*
 * $Log: g711.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.1  2006/03/20 06:53:37  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.2  2006/03/16 08:37:16  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: g711.h,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
 * This source code is quick table lookup implementation of
 * convert 16 bit linear PCM and A-law u-law (ITU G.711) codings
 * Tables are generated using ITU G.711 example code from
 * Sun Microsystems, Inc.
 */

#ifndef __G711_H__
#define __G711_H__

extern	unsigned char	_l2u[4096];
extern	unsigned char	_l2A[2048];
extern	unsigned char	_u2A[256];
extern	unsigned char	_A2u[256];
extern	signed short	_u2l[256];
extern	signed short	_A2l[256];

#define AS_TONE_SILENCE_SIZE			160		/* 20 ms silence */

extern	unsigned char tone_SILENCE[AS_TONE_SILENCE_SIZE];

static	__inline__ unsigned char	linear2ulaw(signed short l)
{
	unsigned char	mask;

	mask = (l<0) ? 0x7f : 0xff;
	if (l<0)
		l = -l;
	if (l<4)
		return(0xff & mask);
	l -= 4;

//	l >>= 3;
	l = l/8;
	return(_l2u[l] & mask);
}

static	__inline__ unsigned char	linear2alaw(signed short l)
{
	unsigned char	mask;

	mask = (l<0) ? 0x7f : 0xff;
	if (l<0)
		l = -l;
	l >>= 4;
	return(_l2A[l] & mask);
}

static	__inline__ signed short		ulaw2linear(unsigned char u)
{
	return(_u2l[u]);
}

static	__inline__ signed short		alaw2linear(unsigned char a)
{
	return(_A2l[a]);
}

static	__inline__ unsigned char	ulaw2alaw(unsigned char u)
{
	return(_u2A[u]);
}

static	__inline__ unsigned char	alaw2ulaw(unsigned char a)
{
	return(_A2u[a]);
}

#endif

