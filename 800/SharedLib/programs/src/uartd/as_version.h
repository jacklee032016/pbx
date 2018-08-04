/* 
 * $Log: as_version.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.2  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_version.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
 * Version Info defination for every module (program)
 * as_version.c in uartd program 
 * Li Zhijie, 2005.05.26
*/

#ifndef __AS_VERSION_H__
#define __AS_VERSION_H__

#define AS_VERSION_MAJOR		"01"  /* major version number ,start from 1 */
#define AS_VERSION_MINOR		"0"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE		"00"	/* from 00 to 99 */
#define AS_VERSION_MODIFY		"01"	/* from 00 to 99 */

#include "as_common_version.h"

#endif

