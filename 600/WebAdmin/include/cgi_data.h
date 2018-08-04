/*
* $Id: cgi_data.h,v 1.2 2006/12/19 17:32:07 lizhijie Exp $
*/
/*
* Backup and restore 
*/

#ifndef	__CGI_DATA_H__
#define	__CGI_DATA_H__

#define	CGIDATA_DEBUG		1

#define	TFTP_EXE										"/usr/bin/tftp"

#define	CGIDATA_CFG_FILE								"/etc/sys/data.conf"

#define	CGIDATA_OP_AUTO_UPDATE						"AutoUpdate"
#define	CGIDATA_OP_BACK_SERVER						"BackServer"		/* back to server */
#define	CGIDATA_OP_BACK_LOCAL						"BackLocal"		/* back to client */
#define	CGIDATA_OP_RESTORE_LOCAL					"RestoreLocal"		/* restore from client local */
#define	CGIDATA_OP_RESTORE_SERVER					"RestoreServer"	/* restore from server */
#define	CGIDATA_OP_RESTORE_INIT						"RestoreInit"		/* restore to init state */

#define	CGIDATA_KEYWORD_SERVER_IP					"DATA_SERVER"
#define	CGIDATA_KEYWORD_RESET_FLAG					"RESET_FLAG"

#define	CGIDATA_KEYWORD_RESET_TFTP					"TFTP"
#define	CGIDATA_KEYWORD_RESET_FLASH				"FLASH"
#define	CGIDATA_KEYWORD_RESET_NO					"NO"

#define	CGIDATA_DATA_BACKUP_PREFIX					"assist_back"          /* prefix of backup data file name */
#define	CGIDATA_SOFT_UPLOAD_PREFIX					"assist_soft"            /* prefix of auto upload software file name */

#define	CGIDATA_BACKUP_DATA_DIR						"data"
#define	CGIDATA_BACKUP_DIRS							"/etc/* /usr/etc/*"          /* 需要tar的路径 */

#define	CGIDATA_TRY_MAX							3

typedef	struct
{
	llist 				cgiVariables;
	
	llist				globalCfgs;
#if 0
	interface_info		*wanInfo;
	interface_info		*lanInfo;
#else
	network_info		*netInfo;
#endif
}DATA_INFO;

int cgidata_get_package_filename(char *filename, const char *command, char *devID);
char *cgidata_init_filename(char *opName);
char *cgidata_serverip_button(DATA_INFO *info, char *opName);
int cgidata_write_config_file(DATA_INFO *info, char *key, char *value);

int cgiData_BackupLocal(DATA_INFO *info);
int cgiData_BackupServer(DATA_INFO *info);
int cgiData_RestoreServer(DATA_INFO *info);

extern	char		dataFilename[];

#endif

