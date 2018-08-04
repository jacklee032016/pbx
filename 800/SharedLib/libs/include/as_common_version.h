/*
 * $Author: lizhijie $
 * $Log: as_common_version.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.4  2005/09/01 08:59:58  tangr
 * deal version
 *
 * Revision 1.3  2005/07/15 07:43:04  wuaiwu
 * no message
 *
 * Revision 1.2  2005/07/08 01:36:29  lijie
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_common_version.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/
#ifndef __AS_COMMON_VERSION_H__
#define __AS_COMMON_VERSION_H__

#define AS_UARTD_MODULE_NAME					"uartd" 		//module name
#define AS_SELFTEST_MODULE_NAME				"selftest"
#define AS_RECORDER_MODULE_NAME                     "recorder"


#define AS_VERSION_LOGPATH					"/var/ver"

#if 0
#define AS_VERSION_INFO(moduleName)	 "%s (version: %s.%s.%s-%s %s %s)\r\n", \
			moduleName, AS_VERSION_MAJOR,AS_VERSION_MINOR, \
			AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__  
#endif

#define AS_VERSION_INFO(moduleName)	 moduleName " (version: " \
			AS_VERSION_MAJOR"." AS_VERSION_MINOR"."	AS_VERSION_RELEASE \
			"-" AS_VERSION_MODIFY" " __DATE__ " " __TIME__ ")\r\n"  


#if  __KERNEL__
	#define AS_VERSION_DEBUG					printk
#else
	#define AS_VERSION_DEBUG  					printf
	#define AS_VERSION_LOG  					fprintf
		
	//int ver_log(int argc, char **argv) ;
	//int ver_opt (int argc, char **argv) ;
#endif

#endif
