/* 
 * $Log: as_version.h,v $
 * Revision 1.1.1.1  2006/11/29 08:35:30  lizhijie
 * AS600 Shared Library
 *
 * Revision 1.1.1.1  2006/07/12 09:12:53  lizhijie
 * rebuild as600 shared library
 *
 * Revision 1.6  2005/09/16 09:30:22  wangwei
 * no message
 *
 * Revision 1.5  2005/09/05 07:15:40  wangwei
 * no message
 *
 * Revision 1.4  2005/06/24 05:25:28  wangwei
 * no message
 *
 * Revision 1.3  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.2  2005/06/03 04:01:50  lizhijie
 * add defination for NPE test
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_version.h,v 1.1.1.1 2006/11/29 08:35:30 lizhijie Exp $
 * Version Info defination for every module (program)
 * as_version.c in self-test program 
 * Li Zhijie, 2005.05.23
*/

#ifndef __AS_VERSION_H__
#define __AS_VERSION_H__

#define AS_VERSION_MAJOR		"01"  /* major version number ,start from 1 */
#define AS_VERSION_MINOR		"0"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE		"03"	/* from 00 to 99 */
#define AS_VERSION_MODIFY		"01"	/* from 00 to 99 */

#include "as_common_version.h"

#endif

