#ifndef  __AS_DISK_INFO_H__
#define __AS_DISK_INFO_H__

#define 	DISK_MODEL					"/proc/ide/hda/model"
#define 	DISK_GEOMETRY   			"/proc/ide/hda/geometry"

#define 	DISK_PARTITIONS			"/proc/partitions"

#define	DISK_MOUNT_TABLE			"/proc/mounts"	//	"/etc/mtab"

#define	DEVICE_HARD_DISK			"/dev/hd"
#define	DEVICE_USB_DISK			"/dev/sd"

#define 	DISK_TEMP_FILE				"/tmp/disks.tmp"

#define	HD_DISK_CONFIG_FILE		"/etc/sys/hddisk.conf"

#define	DISK_PARTITION_NUMBER	2

#define	SUBDIRS_OF_FILE_SERVER	"share"
#define	SUBDIRS_OF_SPOOL			"'cups/tmp cron/crontabs mail smtp postfix saslauthd'"

typedef	enum
{
	PART_SIZE_PERCENT = 0,		/* percentage */
	PART_SIZE_ABSOLUTE,		/* in unit of MB*/
	PART_SIZE_UNKWNOW
}part_size_type_t;

typedef struct  _part_info
{	
	part_size_type_t		sizeType;
	int					ratio;
	
	char					*filesystem;
	char					*mountpoint;
	char					*subdirs;
}part_info_t;


typedef enum
{
		AS_DISK_HARD_DISK			= 0,
		AS_DISK_USB_DISK,
		AS_DISK_CF_CARD_DISK,
		AS_DISK_CDROM,
		AS_DISK_CDRW,
		AS_DISK_NOT_SUPPORT,
		AS_DISK_NO_DEVICE,
		AS_DISK_NO_PARTITION,
		AS_DISK_OS_ERROR
}disk_state_t;

typedef enum
{
		AS_PART_UNFORMAT			= 0,
		AS_PART_FORMATED	,				/* has formatted now */
		AS_PART_FORMATING,				/* is formatting now */
		AS_PART_MOUNTED					/* mountted in the file system */
}partition_state_t;

#define 	AS_STRING_LENGTH			128

struct _as_disk;

struct _as_partition;
struct _as_partition
{
	int					index;
	char					name[AS_STRING_LENGTH];
	partition_state_t		status;

	int					isRootFs;
	
	int 					startCylinder;
	int 					endCylinder;
	char					blocks[AS_STRING_LENGTH];

	int 					id;							/* file system ID */
	char					system[AS_STRING_LENGTH];

	/* info get from /proc/mounts or /etc/mtab */
	char					mountPoint[AS_STRING_LENGTH];
	char					fileSystemType[AS_STRING_LENGTH];	

	struct _as_disk		*disk;
	
	struct _as_partition	*next;
};
typedef	struct _as_partition as_partition_t;


struct _as_disk;
struct _as_disk
{
	int					index;
	char					name[AS_STRING_LENGTH];
	disk_state_t 			status;
	
	unsigned char 		descrition[AS_STRING_LENGTH];
	int 					cylinders;
	int 					headers;
	int 					sectors;

	int					hasRootFs;

	int 					capacity;							/* in unit of MB or GB */
	char					unit[AS_STRING_LENGTH];
	char					capacityB[AS_STRING_LENGTH];		/* in unit of bytes */

	as_partition_t			*parts;

	part_info_t			*partConfig;

	struct _as_disk		*next;
};
typedef	struct _as_disk as_disk_t;

#define	DISK_KEYWORD_DISK						"HDDISK"
#define	DISK_KEYWORD_PART						"part"
#define	DISK_KEYWORD_PART1_SIZE					"part1Size"

#define	DISK_KEYWORD_HD_NUM						"HD_NUM"	/* number of partition in this HD */
#define	DISK_KEYWORD_HD_STATUS					"HD_STATUS"	
#define	DISK_KEYWORD_PART_DEV					"PART"
#define	DISK_KEYWORD_SIZE							"SIZE"
#define	DISK_KEYWORD_FS_TYPE						"TYPE"
#define	DISK_KEYWORD_PART_STATUS				"STATUS"
#define	DISK_KEYWORD_PART_MOUNTPOINT			"MT"
#define	DISK_KEYWORD_PART_MT_SUBDIRS			"SUBDIRS"

#define	DISK_OP_DISK_INSTALL						"install"		/* format a partition of disk */
#define	DISK_OP_DISK_FDISK						"fdisk"		/* format a partition of disk */
#define	DISK_OP_DISK_FDISKING						"fdisking"		/* format a partition of disk */
#define	DISK_OP_DISK_LOOKUP						"lookup"

#define	DISK_OP_DISK_MOUNT						"mount"		/* only mount the first HD disk */
#define	DISK_OP_DISK_UNMOUNT						"unmount"	/* only mount the first HD disk */

#define	DISK_OP_PART_INFO							"partinfo"		/* lookup partition info of disk */
#define	DISK_OP_PART_FORMAT						"format"		/* format a partition of disk */
#define	DISK_OP_PART_FORMATING					"formating"	/* format a partition of disk */

#define	DISK_OP_MGMT								"management"
#define	DISK_OP_MGMT_UNMOUNT					"mgmt_unomunt"
#define	DISK_OP_MGMT_MOUNT						"mgmt_mount"

#define	DISK_FILESYSTEM_EXT2						"ext2"
#define	DISK_FILESYSTEM_EXT3						"ext3"
#define	DISK_FILESYSTEM_REISERFS					"reiserfs"



#define	DISK_BACKEND_E2_FORMAT					"/sbin/mke2fs %s > /dev/null  2>&1"
#define	DISK_BACKEND_E3_FORMAT					"/sbin/mke2fs %s > /dev/null ; /sbin/tune2fs -c 0 -j %s > /dev/null  2>&1"
#define	DISK_BACKEND_REISERFS_FORMAT			"echo -e  \"y\n\" | /sbin/mkreiserfs %s > /dev/null  2>&1"


#define	DISK_PART_1_RATION						0.3
#define	DISK_PART_2_RATION						0.7


#define	DISK_FORMAT_REFRESH_TIMEOUT				55	/* seconds */

unsigned char  *as_cgi_disk_info(as_disk_t *disk);
as_disk_t *as_cgi_get_disks();

as_disk_t *as_cgi_disk_find_disk(as_disk_t *_disks,  char *name);
as_partition_t *as_cgi_disk_find_part(as_disk_t *_disk,  int index);

int	sys_partition_format(as_partition_t *part);
int	sys_disk_fdisk(as_disk_t *disk, char *command);
int	sys_disk_install(as_disk_t *disks);
int 	sys_disk_format(as_disk_t *disk, char *cmd);

int	disk_check_root_disk(as_disk_t *disk);

extern	llist		diskCgiVariables;
extern	llist 		hdDiskCfgs;

int sys_disk_management(as_disk_t *disk, llist *diskConfig);
int sys_disk_mgmt_remount(as_disk_t *disk, llist *diskConfig);
int sys_disk_mgmt_unmount(as_disk_t *disk, llist *diskConfig);

char *as_cgi_disk_parts_info(as_disk_t *disk);
char *as_cgi_disk_disks_info(as_disk_t *disks, char *url , char *op);

#endif


