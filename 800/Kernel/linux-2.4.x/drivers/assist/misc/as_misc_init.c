/* 
 * $Log: as_misc_init.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.4  2006/03/30 07:41:15  wangwei
 * no message
 *
 * Revision 1.3  2006/02/24 01:52:30  wangwei
 * no message
 *
 * Revision 1.2  2006/01/17 12:10:35  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 * Revision 1.4  2005/11/30 07:23:23  wangwei
 * 增加烧录93LC66程序，可以通过条件WITH_93LC66=no选择编译
 *
 * Revision 1.3  2005/09/07 10:02:00  wangwei
 * add drivers of button
 *
 * Revision 1.2  2005/06/15 02:57:27  wangwei
 * 增加了蜂鸣器用户程序接口
 *
 * Revision 1.1  2005/06/07 09:16:21  lizhijie
 * add into CVS
 *
 * $Id: as_misc_init.c,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
 * driver for Miscellious device in our IXP4xx board, such as WatchDog and beep
 * as_misc_init.c  : initial interface of different misc driver module
 * Li Zhijie, 2005.06.07
*/
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include  "as_misc_dev.h"
#include "as_misc_version.h"



#if AS_MISC_WATCHDOG
int as_watchdog_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
int as_watchdog_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
void as_ixp_watchdog_disable(void );
#endif

#if AS_MISC_EEPROM
int as_93lc66_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_93lc66_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
#endif

#if AS_MISC_BEEP
int as_beep_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
int as_beep_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
#endif

#if AS_MISC_BUTTON
int as_button_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_button_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
#endif

#if AS_MISC_ONOFFHOOK
int as_onoffhook_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_onoffhook_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
#endif

#if AS_MISC_SWITCH
int as_switch_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_switch_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
#endif

#if AS_MISC_LED
int as_138_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_138_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
#endif
int as_gpio_ctl_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
int as_gpio_ctl_proc_write(struct file *file, const char *buf, unsigned long count, void *data);

as_misc_dev  misc_devs[] = 
{
	{
		name			:	AS_MISC_PROC_WATCHDOG,
		as_proc_entry		:	NULL,
		read_handler		:	as_watchdog_proc_read,
		write_handler		:	as_watchdog_proc_write
	},
#if 0
	{
		name			:	AS_MISC_PROC_BEEP,
		as_proc_entry		:	NULL,
		read_handler		:	as_beep_proc_read,
		write_handler		:	as_beep_proc_write
	},
	{
		name			:	AS_MISC_PROC_BUTTON,
		as_proc_entry		:	NULL,
		read_handler		:	as_button_proc_read,
		write_handler		:	as_button_proc_write
	},

	{
		name			:	AS_MISC_PROC_SWITCH,
		as_proc_entry		:	NULL,
		read_handler		:	as_switch_proc_read,
		write_handler		:	as_switch_proc_write
	},
	{
		name			:	AS_MISC_PROC_ONOFFHOOK,
		as_proc_entry		:	NULL,
		read_handler		:	as_onoffhook_proc_read,
		write_handler		:	as_onoffhook_proc_write
	},
	{
		name			:	AS_MISC_PROC_138,
		as_proc_entry		:	NULL,
		read_handler		:	as_138_proc_read,
		write_handler		:	as_138_proc_write
	},
#endif
	
	{
		name			:	AS_MISC_PROC_93LC66,
		as_proc_entry		:	NULL,
		read_handler		:	as_93lc66_proc_read,
		write_handler		:	as_93lc66_proc_write

	},
	{
		name			:	AS_MISC_PROC_GPIO_CTL,
		as_proc_entry		:	NULL,
		read_handler		:	as_gpio_ctl_proc_read,
		write_handler		:	as_gpio_ctl_proc_write
	}
};


static struct proc_dir_entry *as_misc_root_dir;



static void __exit as_misc_devs_cleanup(void) 
{
	int i;
//	char proc_name[128];

#if AS_MISC_WATCHDOG
	as_ixp_watchdog_disable();
#endif

	for(i=0; i< (sizeof(misc_devs)/sizeof(as_misc_dev) ); i++ )
	{
		if(misc_devs[i].as_proc_entry)
		{
//			sprintf(proc_name, "%s/%s", AS_MISC_PROC_DIR_NAME, misc_devs[i].name);
			remove_proc_entry(misc_devs[i].name,  as_misc_root_dir );
		}
	}

	if(as_misc_root_dir)
		remove_proc_entry(AS_MISC_PROC_DIR_NAME,  NULL);
	
	printk(KERN_INFO "Assist MISC Driver Unloaded\r\n" );
}

static int __init as_misc_devs_init(void) 
{
	int res = 0;
	int i;
//	char proc_name[128];

	as_misc_root_dir = proc_mkdir(AS_MISC_PROC_DIR_NAME , NULL);
	if( as_misc_root_dir == NULL)
	{
		res = -ENOMEM;
		goto Error;
	}
	as_misc_root_dir->owner = THIS_MODULE;
		
	for(i=0; i< (sizeof(misc_devs)/sizeof(as_misc_dev) ); i++ )
	{
		printk( "%s/%s\n", AS_MISC_PROC_DIR_NAME, misc_devs[i].name );
		misc_devs[i].as_proc_entry = create_proc_entry(misc_devs[i].name, 0644, as_misc_root_dir );
		if(misc_devs[i].as_proc_entry == NULL)
		{
			res = -ENOMEM;
			goto Error;
		}
		misc_devs[i].as_proc_entry->owner = THIS_MODULE;
		misc_devs[i].as_proc_entry->read_proc = misc_devs[i].read_handler;
		misc_devs[i].as_proc_entry->write_proc = misc_devs[i].write_handler;
		misc_devs[i].as_proc_entry->data = &( misc_devs[i]) ;

	}
	
	printk(KERN_INFO  AS_VERSION_INFO("ASMISC") );
	return res;

Error:
	as_misc_devs_cleanup();
	return res;
}


MODULE_AUTHOR("Chengdu R&D <support@assistcn.com>");
MODULE_DESCRIPTION("Assist WatchDog Chip Interface");
MODULE_LICENSE("GPL");


module_init( as_misc_devs_init);
module_exit( as_misc_devs_cleanup );

