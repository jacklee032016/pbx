 /*
 $Author: lizhijie $
 $Log: armlinux.c,v $
 Revision 1.1.1.1  2006/11/30 16:25:18  lizhijie
 AS800 U-Boot

 Revision 1.1.1.1  2006/01/23 06:08:32  wangwei
 no message

 Revision 1.5  2005/01/25 10:00:15  lizhijie
 remove MAC address config

 Revision 1.4  2004/10/08 07:36:50  lizhijie
 Cadd CVS log into file repositoryVS: ----------------------------------------------------------------------

 $Revision: 1.1.1.1 $
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <zlib.h>
#include <asm/byteorder.h>
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

/*cmd_boot.c*/
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

#if 0
static void setup_ramdisk_tag (bd_t *bd);
#endif
# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD)
static void setup_videolfb_tag (gd_t *gd);
# endif


static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

extern image_header_t header;	/* from cmd_bootm.c */


void do_bootm_linux (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		     ulong addr, ulong *len_ptr, int verify)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong len = 0, checksum;
	ulong initrd_start, initrd_end;
	ulong data;
	void (*theKernel)(int zero, int arch, uint params);
	image_header_t *hdr = &header;
	bd_t *bd = gd->bd;

	#ifdef CONFIG_CMDLINE_TAG
	/* following is added by lizhjie 2004.10.06 , in order to set MAC address in OS */
	#if 0
	/* remove MAC address config in u-boot , lizhijie 2005.01.25 */
	static char my_cmdline[256];
	char *mac1, *mac2;
	char *s;
	char *commandline;/* = getenv ("bootargs");*/
	#endif

	char *commandline = getenv ("bootargs");

	#if  0
	/* remove MAC address config in u-boot, lizhijie 2005.01.25 */
	if ((s = getenv("bootargs")) == NULL)
		s = "";

	
/* other MAC validation check should be added here */	
	if ((mac1 = getenv("ethaddr")) == NULL)
		mac1 = "01:01:01:01:01:01";
	if(strlen(mac1) != 17)
		mac1 = "01:01:01:01:01:01";
		
	if ((mac2 = getenv("eth1addr")) == NULL)
		mac2 = "01:01:01:01:01:02";
	if(strlen(mac2) != 17)
		mac2 = "01:01:01:01:01:02";
	
	sprintf(my_cmdline, "%s mac1=%s mac2=%s", s, mac1, mac2);

	commandline = my_cmdline;
/* end of modification in 2005.01.25 */	
	#endif

	/* end of added by lizhijie */	
	#endif

	theKernel = (void (*)(int, int, uint))ntohl(hdr->ih_ep);

	/*
	 * Check if there is an initrd image
	 */
	if (argc >= 3) {
		SHOW_BOOT_PROGRESS (9);

		addr = simple_strtoul (argv[2], NULL, 16);

		printf ("## Loading Ramdisk Image at %08lx ...\n", addr);

		/* Copy header so we can blank CRC field for re-calculation */

		#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (addr, sizeof (image_header_t),
					(char *) &header);
		} else
		#endif
			memcpy (&header, (char *) addr,
				sizeof (image_header_t));

		if (ntohl (hdr->ih_magic) != IH_MAGIC) {
			printf ("Bad Magic Number\n");
			SHOW_BOOT_PROGRESS (-10);
			do_reset (cmdtp, flag, argc, argv);
		}

		data = (ulong) & header;
		len = sizeof (image_header_t);

		checksum = ntohl (hdr->ih_hcrc);
		hdr->ih_hcrc = 0;

		if (crc32 (0, (char *) data, len) != checksum) {
			printf ("Bad Header Checksum\n");
			SHOW_BOOT_PROGRESS (-11);
			do_reset (cmdtp, flag, argc, argv);
		}

		SHOW_BOOT_PROGRESS (10);

		print_image_hdr (hdr);

		data = addr + sizeof (image_header_t);
		len = ntohl (hdr->ih_size);

		#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (data, len, (char *) CFG_LOAD_ADDR);
			data = CFG_LOAD_ADDR;
		}
		#endif

		if (verify) {
			ulong csum = 0;

			printf ("   Verifying Checksum ... ");
			csum = crc32 (0, (char *) data, len);
			if (csum != ntohl (hdr->ih_dcrc)) {
				printf ("Bad Data CRC\n");
				SHOW_BOOT_PROGRESS (-12);
				do_reset (cmdtp, flag, argc, argv);
			}
			printf ("OK\n");
		}

		SHOW_BOOT_PROGRESS (11);

		if ((hdr->ih_os != IH_OS_LINUX) ||
		    (hdr->ih_arch != IH_CPU_ARM) ||
		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
			printf ("No Linux ARM Ramdisk Image\n");
			SHOW_BOOT_PROGRESS (-13);
			do_reset (cmdtp, flag, argc, argv);
		}

		#ifdef CONFIG_B2
		/*
		 *we need to copy the ramdisk to SRAM to let Linux boot
		 */
		memmove ((void *) ntohl(hdr->ih_load), (uchar *)data, len);
		data = ntohl(hdr->ih_load);
		#endif /* CONFIG_B2 */

		/*
		 * Now check if we have a multifile image
		 */
	} else if ((hdr->ih_type == IH_TYPE_MULTI) && (len_ptr[1])) {
		ulong tail = ntohl (len_ptr[0]) % 4;
		int i;

		SHOW_BOOT_PROGRESS (13);

		/* skip kernel length and terminator */
		data = (ulong) (&len_ptr[2]);
		/* skip any additional image length fields */
		for (i = 1; len_ptr[i]; ++i)
			data += 4;
		/* add kernel length, and align */
		data += ntohl (len_ptr[0]);
		if (tail) {
			data += 4 - tail;
		}

		len = ntohl (len_ptr[1]);

	} else {
		/*
		 * no initrd image
		 */
		SHOW_BOOT_PROGRESS (14);

		len = data = 0;
	}

	#ifdef	DEBUG
	if (!data) {
		printf ("No initrd\n");
	}
	#endif

	if (data) {
		initrd_start = data;
		initrd_end = initrd_start + len;
	} else {
		initrd_start = 0;
		initrd_end = 0;
	}

	SHOW_BOOT_PROGRESS (15);

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

	#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    	defined (CONFIG_CMDLINE_TAG) || \
    	defined (CONFIG_INITRD_TAG) || \
    	defined (CONFIG_SERIAL_TAG) || \
    	defined (CONFIG_REVISION_TAG) || \
    	defined (CONFIG_VFD)
	setup_start_tag (bd);
	#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params);
	#endif
	#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
	#endif
	#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
	#endif
	#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
	#endif
	#ifdef CONFIG_INITRD_TAG
	if (initrd_start && initrd_end)
		setup_initrd_tag (bd, initrd_start, initrd_end);
	#endif
	#if defined (CONFIG_VFD)
	setup_videolfb_tag ((gd_t *) gd);
	#endif
	setup_end_tag (bd);
	#endif

	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ...   \n\n");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();

	theKernel (0, bd->bi_arch_number, bd->bi_boot_params);
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD)
static void setup_videolfb_tag (gd_t *gd)
{
	/* An ATAG_VIDEOLFB node tells the kernel where and how large
	 * the framebuffer for video was allocated (among other things).
	 * Note that a _physical_ address is passed !
	 *
	 * We only use it to pass the address and size, the other entries
	 * in the tag_videolfb are not of interest.
	 */
	params->hdr.tag = ATAG_VIDEOLFB;
	params->hdr.size = tag_size (tag_videolfb);

	params->u.videolfb.lfb_base = (u32) gd->fb_base;
	/* 7168 = 256*4*56/8 - actually 2 pages (8192 bytes) are allocated */
	params->u.videolfb.lfb_size = 7168;

	params = tag_next (params);
}
#endif

static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
