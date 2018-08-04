/*
* $Id: wizard.h,v 1.4 2007/01/25 18:52:29 lizhijie Exp $
*/

#ifndef	__WIZARD_H__
#define	__WIZARD_H__

#include "cgi_lib.h"
#include "as_disk_info.h"

#define trace		cgidebug("%s[%d]\r\n", __FUNCTION__, __LINE__);

typedef	struct
{
	llist				cgiVariables;

	
	llist				wanCfgs;
	llist				pppoeCfgs;
	llist				pppSecretCfgs;
	
	llist				lanCfgs;

	llist				defaultCfgs;			/* content of /etc/sys/data.conf */

	network_info 		*netInfo;

	as_disk_t 		*disks;

}WIZARD_INFO;


#define	WIZARD_STR_TITLE			_("Install Wizard")
#define	WIZARD_STR_PREPARE		_("Prepare")
#define	WIZARD_STR_HARDWARE		_("Hardware Check")
#define	WIZARD_STR_PARTITION		_("Disk Partition")
#define	WIZARD_STR_FORMAT		_("Disk Format")
#define	WIZARD_STR_UPDATE		_("Update")

#define	WIZARD_OP_WANCONFIG		"WanConfig"	
#define	WIZARD_OP_HARDWARE		"HardwareCheck"	
#define	WIZARD_OP_PARTITION		"Partition"	
#define	WIZARD_OP_FORMAT			"Format"	
#define	WIZARD_OP_UPLOAD			"UpLoad"	
#define	WIZARD_OP_UPDATE			"Update"	
#define	WIZARD_OP_REBOOT			"Reboot"	

#define	WIZARD_KEY_UPLOAD_FILE		"uploadfiles"

#define	WIZARD_FS_MOUNT_POINT		"/opt"

#define	WIZARD_FS_FILESYSTEM_IMAGE_PREFIX	"AS600_FileSystem"
#define	WIZARD_FS_FILESYSTEM_IMAGE_SURFIX	"tar.gz"

#define	WIZARD_FS_TRANSFER_DELAY	120		/* seconds */

typedef	enum
{
	WIZARD_ERROR_NO = 0,
	WIZARD_ERROR_UNKWON_TYPE,
	WIZARD_ERROR_NO_DISK,
	WIZARD_ERROR_MAX_NUMBER
}WIZARD_ERROR_CODE;


char	*wizard_page_begin(char *title, WIZARD_INFO *info);
char *wizard_page_alert(char *status, WIZARD_INFO *info);
char *wizard_page_end(char *leftButton, char *rightButton, WIZARD_INFO *info);
int	wizard_page(char *title, char *status, char *alert, char *leftButton, char *rightButton, WIZARD_INFO *info);
char *wizard_page_status_begin( WIZARD_INFO *info);
char *wizard_page_status(char *status, WIZARD_INFO *info);
char *wizard_page_status_end( WIZARD_INFO *info);

int  wizard_cgi_hardware_check(WIZARD_INFO *info );
int  wizard_cgi_hd_partition(WIZARD_INFO *info );
int  wizard_cgi_hd_format(WIZARD_INFO *info );

as_disk_t  *wizard_find_disk(WIZARD_INFO *info);

int  wizard_cgi_upload(WIZARD_INFO *info );
int  wizard_cgi_update(WIZARD_INFO *info );

int wizard_cgi_reboot(WIZARD_INFO *info);

int	wizard_upload_callback(char *uploadFileName, void *data);

int wizard_cgi_wan(WIZARD_INFO *info);

#endif

