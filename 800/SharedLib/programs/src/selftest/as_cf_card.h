
#define __u32 unsigned int
#define __u16 unsigned short int
#define __u8 unsigned char

typedef struct  {

/*00*/ 	__u32 s_inodes_count;			/* inodes ���� */
		__u32 s_blocks_count;			/* blocks ���� */
		__u32 s_r_blocks_count;			/* ������ blocks ���� */
		__u32 s_free_blocks_count;		/* ���е� blocks ���� */
/*10*/ 	__u32 s_free_inodes_count;		/* ���е� inodes ���� */
		__u32 s_first_data_block;			/* ��һ������ block */
		__u32 s_log_block_size;			/* block �Ĵ�С */
		__u32 s_log_frag_size;			/* ���Ժ��� */
/*20*/ 	__u32 s_blocks_per_group;		/* ÿ block group �� block ���� */
		__u32 s_frags_per_group;		/* ���Ժ��� */
		__u32 s_inodes_per_group;		/* ÿ block group �� inode ���� */
		__u32 s_mtime;					/* Mount time */
/*30*/ 	__u32 s_wtime;					/* Write time */
		__u16 s_mnt_count;				/* Mount count */
		__u16 s_max_mnt_count;			/* Maximal mount count */	
		__u16 s_magic;					/* Magic ǩ�� */
		__u16 s_state;					/* File system state */
		__u16 s_errors;					/* Behaviour when detecting errors */
		__u16 s_minor_rev_level;			/* minor revision level */
/*40*/ 	__u32 s_lastcheck;				/* time of last check */
		__u32 s_checkinterval;			/* max. time between checks */
		__u32 s_creator_os;				/* ���Ժ��� */
		__u32 s_rev_level;				/* Revision level */
/*50*/ 	__u16 s_def_resuid;				/* Default uid for reserved blocks */
		__u16 s_def_resgid;				/* Default gid for reserved blocks */
		__u32 s_first_ino;				/* First non-reserved inode */
		__u16 s_inode_size;				/* size of inode structure */
		__u16 s_block_group_nr;			/* block group # of this superblock */
		__u32 s_feature_compat;			/* compatible feature set */
/*60*/ 	__u32 s_feature_incompat;		/* incompatible feature set */
		__u32 s_feature_ro_compat;		/* readonly-compatible feature set */
/*68*/ 	__u8 s_uuid[16];					/* 128-bit uuid for volume */
/*78*/ 	char s_volume_name[16];			/* volume name */
/*88*/ 	char s_last_mounted[64];			/* directory where last mounted */
/*C8*/ 	__u32 s_algorithm_usage_bitmap;	/* ���Ժ��� */
		__u8 s_prealloc_blocks;			/* ���Ժ��� */
		__u8 s_prealloc_dir_blocks;		/* ���Ժ��� */
		__u16 s_padding1;				/* ���Ժ��� */
/*D0*/ 	__u8 s_journal_uuid[16];			/* uuid of journal superblock */
/*E0*/ 	__u32 s_journal_inum;			/* ��־�ļ��� inode ���� */
		__u32 s_journal_dev;				/* ��־�ļ����豸�� */
		__u32 s_last_orphan;				/* start of list of inodes to delete */
/*EC*/ 	__u32 s_reserved[197];			/* ���Ժ��� */

}ext3_super_block;

#define __AS_STR_LENTH			256
#define __AS_RCF_LENTH			2048

#define __AS_DIR_NAME			"dev"
#define __AS_DIR_CF_CARD		"hda1"

#define S_MAIGC_VALU  			0xEF53

