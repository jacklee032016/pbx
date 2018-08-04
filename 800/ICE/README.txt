				IXP425 README

2006.01.17   wangwei
FLASH烧录程序关于TE28F128J3C150

		采用FLASH-INTEL TE28F128J3C150(16MB)，共128个扇区，每个扇区128KB
	
	烧录程序的空间分配：
	
		BOOTLOADER
			扇区：		2个扇区	
			地址分配：	0x50000000-0x5003ffff
			使用空间：	2*128KB=256KB
		
		KERNEL
			扇区：		12个扇区	
			地址分配：	0x50040000-0x501bffff
			使用空间：	12*128KB=1536KB
		
		JFFS2
			扇区：		114个扇区	
			地址分配：	0x501c0000-0x50ffffff
			使用空间：	114*128KB=14592KB
		

	
