/*
* $Id: as_version.h,v 1.1 2007/03/26 09:07:45 lizhijie Exp $
*/

#ifndef __AS_VERSION_H__
#define __AS_VERSION_H__

#define AS_VERSION_MAJOR		"01"  /* major version number ,start from 1 */
#define AS_VERSION_MINOR		"0"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE		"01"	/* from 00 to 99 */
#define AS_VERSION_MODIFY		"01"	/* from 00 to 99 */
#define AS_MODULENAME			"alert" //module name

#define AS_VERSION_INFO(moduleName)	 moduleName " (version: " \
			AS_VERSION_MAJOR"." AS_VERSION_MINOR"."	AS_VERSION_RELEASE \
			"-" AS_VERSION_MODIFY" " __DATE__ " " __TIME__ ")\r\n"  

#endif

