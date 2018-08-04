/*
 * $Author: lizhijie $
 * $Id: as_common_version.h,v 1.4 2007/04/25 09:10:44 lizhijie Exp $
*/
#ifndef __AS_COMMON_VERSION_H__
#define __AS_COMMON_VERSION_H__

#define AS_UARTD_MODULE_NAME					"uartd" 		//module name
#define AS_SELFTEST_MODULE_NAME				"selftest"
#define AS_RECORDER_MODULE_NAME				"recorder"
#define AS_RENEW_MODULE_NAME					"renew"
#define AS_SETUP_MODULE_NAME					"setdeft"
#define AS_STC_ISP_MODULE_NAME				"isp"

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
