/*
$Author: lizhijie $
$Log: ixdp425.h,v $
Revision 1.2  2006/12/01 14:24:45  lizhijie
dd

Revision 1.1.1.1  2006/11/29 09:09:45  lizhijie
AS600 ICE

Revision 1.10  2005/09/05 11:56:43  lizhijie
modify a bug

Revision 1.9  2005/09/05 11:15:05  lizhijie
add conditional compile for USB/CF boot root device

Revision 1.8  2005/07/04 02:56:10  lizhijie
no message

Revision 1.7  2005/06/16 02:56:11  lizhijie
update beeper from GPIO7 to GPIO6

Revision 1.6  2005/06/15 10:38:28  lizhijie
no message

Revision 1.5  2005/01/25 13:30:10  lizhijie
*** empty log message ***

Revision 1.4  2005/01/25 13:28:54  lizhijie
Add 'run install' command for using '/dev/mtdblock2' as root and at
the same time, support '/dev/hda1' as root when normally booting

Revision 1.3  2004/10/08 07:24:15  lushenghong
Cadd MAC address in the kernel command linesVS: ----------------------------------------------------------------------

Revision 1.2  2004/09/29 09:38:32  lushenghong
Cmodify some environement for IXP422 VS: ----------------------------------------------------------------------

$Revision: 1.2 $
*/

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_IXP425           1       /* This is an IXP425 CPU    */
#define CONFIG_IXDP425          1       /* on an IXDP425 Board      */

/***************************************************************
 * U-boot generic defines start here.
 ***************************************************************/

/*
 * If we are developing, we might want to start armboot from ram
 * so we MUST NOT initialize critical regs like mem-timing ...
 */
#define CONFIG_INIT_CRITICAL            /* undef for developing */

#undef CONFIG_USE_IRQ                   /* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN      (CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE         115200

#define CONFIG_COMMANDS         (CONFIG_CMD_DFL & ~CFG_CMD_NET)

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
/* These are u-boot generic parameters */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY        3
#define CONFIG_ETHADDR          	01:01:01:01:01:01

/* following is added by lizhijie 2004.09.29 */
#define CONFIG_ETH1ADDR		01:01:01:01:01:02

#if 0 
/* commentted by lizhijie 2004.09.29 */
#define CONFIG_NETMASK          255.255.0.0
#define CONFIG_IPADDR           192.168.0.21
#define CONFIG_SERVERIP         192.168.0.250
#endif

/* added by lizhijie , 2005.01.25 , support 'run install' command */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"install=setenv bootargs=console=ttyS01,115200   root=/dev/mtdblock2 rw mem=64M@0x0\;boot " \
	""

/* changed from 50040000 lizhijie 2004.06.28 */
/* change it as following in 08.09 */
/*#define CONFIG_BOOTCOMMAND      "cp 50130000 400000 40000;bootm 50030000"  
*/
#define CONFIG_BOOTCOMMAND      "bootm 50030000"  /* changed from 50040000 lizhijie 2004.06.28 */

/* for 2.4.24 kernel lizhijie 2004.06.15  */
#if 0
/* #define CONFIG_BOOTARGS         "console=ttyS01,115200 root=/dev/ram0 rw initrd=0x400000,4M mem=64M@0x0"
*/
#endif
/* add jffs2 root filesystem  */
#if  USB_BOOT
	#define CONFIG_BOOTARGS         "console=ttyS01,115200 root=/dev/mtdblock2 ro mem=64M@0x0"
#else
	#define CONFIG_BOOTARGS         "console=ttyS01,115200 root=/dev/hda1 ro mem=64M@0x0"
#endif

/* for 2.6.2 kernel  lizhijie 2004.06.14 */
/*#define CONFIG_BOOTARGS         "root=/dev/ram0 initrd=0x400000,366282 console=ttyS0,115200 mem=64M "
*/
#define CONFIG_CMDLINE_TAG


#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE    230400          /* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX   2               /* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP                            /* undef to save memory         */
#define CFG_PROMPT              "assist# "   /* Monitor Command Prompt       */
#define CFG_CBSIZE              256             /* Console I/O Buffer Size      */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS             16              /* max number of command args   */
#define CFG_BARGSIZE            CFG_CBSIZE      /* Boot Argument Buffer Size    */


/* changed  as following , lizhijie, 2005.06.15 */
/* #define CFG_MEMTEST_START       	0x00400000   */   /* memtest works on     */
/*  #define CFG_MEMTEST_END         	0x00800000     */ /* 4 ... 8 MB in DRAM   */
#define CFG_MEMTEST_START       	0x00100000      /* memtest works on     */
#define CFG_MEMTEST_END         	(0x04000000-0x400000)      /* 4 ... 60 MB in DRAM   */
/*change ended here, lizhijie, 2005.06.15 */

#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR           0x00010000      /* default load address */

#define CFG_HZ                  3333333        /* spec says 66.666 MHz, but it appears to be 33 */
						/* valid baudrates */
#define CFG_BAUDRATE_TABLE      { 9600, 19200, 38400, 57600, 115200 }

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif

/***************************************************************
 * Platform/Board specific defines start here.
 ***************************************************************/

/*
 * Hardware drivers
 */


/*
 * select serial console configuration
 */
#define CFG_IXP425_CONSOLE	IXP425_UART2   /* we use UART1 for console */

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    1          /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1            0x00000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       0x03ffffff /* 64 MB */

#define PHYS_FLASH_1            0x50000000 /* Flash Bank #1 */
#define PHYS_FLASH_SIZE         0x00400000 /* 4 MB */
#define PHYS_FLASH_BANK_SIZE    0x00400000 /* 4 MB Banks */
#define PHYS_FLASH_SECT_SIZE    0x00008000 /* 32 KB sectors (x1) */

#define CFG_DRAM_BASE           0x00000000
#define CFG_DRAM_SIZE           0x03ffffff

#define CFG_FLASH_BASE          PHYS_FLASH_1

/*
 * Expansion bus settings
 */
#define CFG_EXP_CS0				0xbcd23c42

/*
 * SDRAM settings
 */
/* lizhijie, 2005.06.28 */
/* #define CFG_SDR_CONFIG			0xd */
/* changed as this line for the 2 chips 1 bank 64M(16 bit) memory config */
#define CFG_SDR_CONFIG			0xA

#define CFG_SDRAM_REFRESH_CNT 	0x81a

/*
 * GPIO settings
 */

/*
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     1       /* max number of memory banks           */
#define CFG_MAX_FLASH_SECT      128  	/* max number of sectors on one chip    */
#define CFG_FLASH_PROTECTION
/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT    (25*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (25*CFG_HZ) /* Timeout for Flash Write */

/* FIXME */
#define CFG_ENV_IS_IN_FLASH             1
#define CFG_ENV_ADDR            (PHYS_FLASH_1 + 0x20000)
#define CFG_ENV_SIZE            0x10000 /* Total Size of Environment Sector : 64 K . lizhijie 2006.06.28 */
/* Debug */
#endif  /* __CONFIG_H */
