/*
 * $Log: as_version.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/04/29 05:55:25  lizhijie
 * dd
 *
 * Revision 1.4  2006/03/23 05:22:05  wangwei
 * no message
 *
 * Revision 1.3  2006/03/17 09:44:22  wangwei
 * no message
 *
 * Revision 1.2  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_version.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#ifndef __AS_VERSION_H__
#define __AS_VERSION_H__

#define AS_VERSION_MAJOR		"00" /* major version number ,start from 1 */
#define AS_VERSION_MINOR		"95"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE		"00"	/* from 01 to 99 */
#define AS_VERSION_MODIFY		"00"	/* from 00 to 99 */

#if  __KERNEL__
	#define AS_VERSION_INFO(moduleName)	"%s( version: %s.%s.%s-%s %s %s)\r\n", (moduleName), \
		AS_VERSION_MAJOR,AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__
#else
	#define AS_VERSION_INFO(moduleName) 	"%s( version: %s.%s.%s-%s %s %s)\r\n", (moduleName), \
		AS_VERSION_MAJOR,AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__
#endif

#endif
