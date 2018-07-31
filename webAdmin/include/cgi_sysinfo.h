/*
* $Id: cgi_sysinfo.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/
/*
* System Information pages 
*/

#ifndef	__CGI_SYS_INFO_H__
#define	__CGI_SYS_INFO_H__

#define	CGI_SYSINFO_OP_VERSION						"version"
#define	CGI_SYSINFO_OP_NET_STATE						"NetStatus"
#define	CGI_SYSINFO_OP_SYS_PROC						"SysProc"
#define	CGI_SYSINFO_OP_LOCALE						"SysLocale"	/* language */
#define	CGI_SYSINFO_OP_TIME							"SysTime"	

#define	CGI_SYSINFO_KEYWORK_PROC_NAME				"ProcName"


typedef	struct
{
	llist 				cgiVariables;
	
	llist				globalCfgs;

	network_info		*netInfo;
	
}SYS_INFO;

int cgiSysInfo_Versions(SYS_INFO *info);
int cgiSysInfo_Tasks(SYS_INFO *info);
int cgiSysInfo_Locale(SYS_INFO *info);
int cgiSysInfo_Time(SYS_INFO *info);

#endif

