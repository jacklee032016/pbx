			IXP422 README

2004.12.29
	重新创建FLASH的映象
		4M FLASH，128块，每块32K，地址为0x0000～0x8000
		
	映象
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
	测试直接从仿真器中写入jffs2的image
		建立mtdblock设备，然后mount -t jffs2 /dev/mtdblock2 /mnt时提示
		magic 0x8519成为了0x1985
		分析原因是在于通过仿真器写入的jffs2 image，而不是在系统启动后，
		通过"eraseall /dev/mtd0;cat jffs2.image > /dev/mtd0"来传输文件
		系统的image，后者通过OS来写image，就使用当前OS的byte order来写，
		这样可以保证写入的image的byte order和OS的byte order一致
	
	invalid compressed format (err=1)<6>Freeing initrd memory: 6144K
	VFS: Mounted root (ext2 filesystem).
	Freeing init memory: 256K
	try /sbin/init
	EXT2-fs error (device ramdisk(1,0)): ext2_check_page: bad entry in directory #92: unaligned directory entry - offset=0, inode=50070311, rec_len=807, name_len=252
	try /etc/init
	try /bin/init
	try /bin/sh
		以上错误的原因是写入的initrd image文件的大小超过了832K，仿真器程序没有把
		后续的内容写完整，所以解压发现错误

	# ls /etc/boa/
	EXT2-fs error (device ramdisk(1,0)): ext2_check_page: bad entry in directory #65: rec_len is smaller than minimal - offset=0, inode=0, rec_len=0, name_len=0
		这个问题，还没有找到原因？？？

AXD中烧录bootloader程序的过程：obey d:\works\ICE\setIXP4xx.ini
	AXD中初始化IXP板子为“big endian”方式：与S3C板子主要的区别
	在AXD的command line interface中执行“obey d:\works\ICE\setIXP4xx.ini”文件
	将uboot.bin文件（约68K）载入内存为0x100000（5个0）的位置
	载入烧录程序的image（使用不同的byte order，会造成载入的程序完全混乱）
	开始执行程序

新的烧录程序
	只烧录192 K 空间的bootloader，
		空间分配为0x50000000－－0x50030000，其中程序占用6个块，环境变量占用2个块
		把编译器和汇编程序都设置为big，同时不使用任何的编译优化
		把连接程序的ro_base设置为数据存储的位置，即0x1000000（6个0，保证与加载的
		数据完全隔离开来）
		flash_bootloader
	
	只烧录kernel的程序
		地址范围为0x50030000－－0x50130000，共32个块
		kernel的image（结果u-boot的mkimage工具处理后的image）占用的空间是1M。
	
	只烧录根文件系统的程序
		地址范围为：0x50130000－－0x50200000，共26块，空间为832K
		
	只烧录最后区间的jffs2 image的程序
		地址范围为：0x50200000－－0x50400000，共64块，2M
	
