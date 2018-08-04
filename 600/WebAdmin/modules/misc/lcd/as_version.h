/*
* $Id: as_version.h,v 1.1 2007/01/25 16:50:08 lizhijie Exp $
*/

#ifndef __AS_VERSION_H__
#define __AS_VERSION_H__

#define AS_VERSION_MAJOR		"01"  /* major version number ,start from 1 */
#define AS_VERSION_MINOR		"0"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE		"08"	/* from 00 to 99 */
#define AS_VERSION_MODIFY		"23"	/* from 00 to 99 */
#define AS_MODULENAME			"wizard" //module name

#define AS_VERSION_INFO(moduleName)	 moduleName " (version: " \
			AS_VERSION_MAJOR"." AS_VERSION_MINOR"."	AS_VERSION_RELEASE \
			"-" AS_VERSION_MODIFY" " __DATE__ " " __TIME__ ")\r\n"  

#endif

