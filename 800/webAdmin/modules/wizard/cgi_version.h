/*
* $Id$
*/

#ifndef __CGI_VERSION_H__
#define __CGI_VERSION_H__

#define CGI_VERSION_MAJOR		"01"  /* major version number ,start from 1 */
#define CGI_VERSION_MINOR		"0"	/* minor, from 0 to 9 */
#define CGI_VERSION_RELEASE		"08"	/* from 00 to 99 */
#define CGI_VERSION_MODIFY		"23"	/* from 00 to 99 */
#define CGI_MODULENAME			"wizard" //module name

#define WIZARD_VERSION_INFO(moduleName)	 moduleName " (version: " \
			CGI_VERSION_MAJOR"." CGI_VERSION_MINOR"."	CGI_VERSION_RELEASE \
			"-" CGI_VERSION_MODIFY" " __DATE__ " " __TIME__ ")\r\n"  

#endif

