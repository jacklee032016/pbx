#ifndef __ASSIST_VERSION_H__
#define __ASSIST_VERSION_H__

#define AS_VERSION_MAJOR			"01"  /* major version number ,start from 1 */
#define AS_VERSION_MINOR			"0"	/* minor, from 0 to 9 */
#define AS_VERSION_RELEASE			"06"	/* from 00 to 99 */
#define AS_VERSION_MODIFY			"02"	/* from 00 to 99 */
#define AS_PBX_MODULENAME			"pbx" //module name

#define AS_VERSION_LOGPATH		"/var/ver/"

#if 1
#ifdef __cplusplus
extern "C"
{
#endif
#if  __KERNEL__
	#define AS_VERSION_INFO(moduleName)	printk("%s (version: %s.%s.%s-%s %s %s)\r\n", (moduleName), \
		AS_VERSION_MAJOR,AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__);
#else
	#define AS_VERSION_INFO(moduleName)  printf("%s (version: %s.%s.%s-%s %s %s)\r\n", (moduleName), \
		AS_VERSION_MAJOR,AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__);
	#define AS_VERSION_LOG(f,moduleName)  fprintf((f),"%s (version: %s.%s.%s-%s %s %s)\r\n", (moduleName), \
		AS_VERSION_MAJOR,AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__);
#endif

#ifdef __cplusplus
};
#endif
#endif

#endif

