			IXP422 README

2004.12.29
	���´���FLASH��ӳ��
		4M FLASH��128�飬ÿ��32K����ַΪ0x0000��0x8000
		
	ӳ��
		BootLoader 	(192K, 		6 blocks, 	0x0000,0000-0x0003,0000)
		Kernel		(1024+512K, 	48 blocks, 	0x0003,0000-0x001B,0000)
		JFFS2-Root	(2368K		74 blocks,	0x0014,0000-0x0040,0000)
		

2004.08.10
	Rebuild the layout of 4M FLASH Chips
		BootLoader 	(192K, 		6 blocks, 	0x0000,0000-0x0003,0000)
		Kernel		(1024+64K, 	34 blocks, 	0x0003,0000-0x0014,0000)
		JFFS2-Root	(3072-64K	88 blocks,	0x0014,0000-0x0040,0000)	2816K
	
	ICE/bootloader
		No change in different layout version 	
		
	ICE/kernel
		Burning JFFS2 root FS into FLASH with this layout
		
	ICE/jffs2-3M
		Burning JFFS2 root FS into FLASH with this layout
	

2004.08.02
	����ֱ�Ӵӷ�������д��jffs2��image
		����mtdblock�豸��Ȼ��mount -t jffs2 /dev/mtdblock2 /mntʱ��ʾ
		magic 0x8519��Ϊ��0x1985
		����ԭ��������ͨ��������д���jffs2 image����������ϵͳ������
		ͨ��"eraseall /dev/mtd0;cat jffs2.image > /dev/mtd0"�������ļ�
		ϵͳ��image������ͨ��OS��дimage����ʹ�õ�ǰOS��byte order��д��
		�������Ա�֤д���image��byte order��OS��byte orderһ��
	
	invalid compressed format (err=1)<6>Freeing initrd memory: 6144K
	VFS: Mounted root (ext2 filesystem).
	Freeing init memory: 256K
	try /sbin/init
	EXT2-fs error (device ramdisk(1,0)): ext2_check_page: bad entry in directory #92: unaligned directory entry - offset=0, inode=50070311, rec_len=807, name_len=252
	try /etc/init
	try /bin/init
	try /bin/sh
		���ϴ����ԭ����д���initrd image�ļ��Ĵ�С������832K������������û�а�
		����������д���������Խ�ѹ���ִ���

	# ls /etc/boa/
	EXT2-fs error (device ramdisk(1,0)): ext2_check_page: bad entry in directory #65: rec_len is smaller than minimal - offset=0, inode=0, rec_len=0, name_len=0
		������⣬��û���ҵ�ԭ�򣿣���

AXD����¼bootloader����Ĺ��̣�obey d:\works\ICE\setIXP4xx.ini
	AXD�г�ʼ��IXP����Ϊ��big endian����ʽ����S3C������Ҫ������
	��AXD��command line interface��ִ�С�obey d:\works\ICE\setIXP4xx.ini���ļ�
	��uboot.bin�ļ���Լ68K�������ڴ�Ϊ0x100000��5��0����λ��
	������¼�����image��ʹ�ò�ͬ��byte order�����������ĳ�����ȫ���ң�
	��ʼִ�г���

�µ���¼����
	ֻ��¼192 K �ռ��bootloader��
		�ռ����Ϊ0x50000000����0x50030000�����г���ռ��6���飬��������ռ��2����
		�ѱ������ͻ���������Ϊbig��ͬʱ��ʹ���κεı����Ż�
		�����ӳ����ro_base����Ϊ���ݴ洢��λ�ã���0x1000000��6��0����֤����ص�
		������ȫ���뿪����
		flash_bootloader
	
	ֻ��¼kernel�ĳ���
		��ַ��ΧΪ0x50030000����0x50130000����32����
		kernel��image�����u-boot��mkimage���ߴ�����image��ռ�õĿռ���1M��
	
	ֻ��¼���ļ�ϵͳ�ĳ���
		��ַ��ΧΪ��0x50130000����0x50200000����26�飬�ռ�Ϊ832K
		
	ֻ��¼��������jffs2 image�ĳ���
		��ַ��ΧΪ��0x50200000����0x50400000����64�飬2M
	
