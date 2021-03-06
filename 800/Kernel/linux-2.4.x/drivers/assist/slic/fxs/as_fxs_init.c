/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Log: as_fxs_init.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.5  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.4  2006/04/19 12:31:48  lizhijie
 * no message
 *
 * Revision 1.3  2006/01/14 11:05:35  lizhijie
 * dd
 *
 * Revision 1.2  2006/01/14 04:07:59  lizhijie
 * add conditional compile in integrated environments and wc structure optimization
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.3  2005/04/26 06:03:01  lizhijie
 * add version info for drivers
 *
 * Revision 1.2  2005/04/20 02:35:26  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.3  2004/12/11 05:35:51  lizhijie
 * add Tiger320 debug info
 *
 * Revision 1.2  2004/11/25 07:59:24  lizhijie
 * recommit all
 *
*/
#include "as_fxs_common.h"

#include "as_version.h"

int debug = 1;
int robust = 1;
int timingonly = 1;
int lowpower = 0;
int boostringer = 1;
int _opermode = 2;
int fxshonormode = 1;


#if  AS_WITH_FXO_MODULE
char *opermode = "JAPAN";

struct fxo_mode  fxo_modes[] =
{
	{ "FCC", 0, 0, 0, 0, 0, 0x3, 0, 0 }, 	/* US, Canada */
	{ "TBR21", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },/* Austria, Belgium, Denmark, Finland, France, Germany, 
										   Greece, Iceland, Ireland, Italy, Luxembourg, Netherlands,
										   Norway, Portugal, Spain, Sweden, Switzerland, and UK */
	/* */
	{ "JAPAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },

	{ "ARGENTINA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "AUSTRALIA", 1, 0, 0, 0, 0, 0, 0x3, 0x3 },
	{ "AUSTRIA", 0, 1, 0, 0, 1, 0x3, 0, 0x3 },
	{ "BAHRAIN", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "BELGIUM", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "BRAZIL", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "BULGARIA", 0, 0, 0, 0, 1, 0x3, 0x0, 0x3 },
	{ "CANADA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CHILE", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CHINA", 0, 0, 0, 0, 0, 0, 0x3, 0xf },
	{ "COLUMBIA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "CROATIA", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "CYRPUS", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "CZECH", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "DENMARK", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ECUADOR", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "EGYPT", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "ELSALVADOR", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "FINLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "FRANCE", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "GERMANY", 0, 1, 0, 0, 1, 0x3, 0, 0x3 },
	{ "GREECE", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "GUAM", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "HONGKONG", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "HUNGARY", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "ICELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "INDIA", 0, 0, 0, 0, 0, 0x3, 0, 0x4 },
	{ "INDONESIA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "IRELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ISRAEL", 0, 0, 0, 0, 1, 0x3, 0, 0x2 },
	{ "ITALY", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	
	
	{ "NEWZEALAND", 0, 0, 0, 0, 0, 0x3, 0, 0x4 },
	{ "NIGERIA", 0, 0, 0, 0, 0x1, 0x3, 0, 0x2 },
	{ "NORWAY", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "OMAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "SOUTHKOREA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "SPAIN", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SWEDEN", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SWITZERLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2 },
	{ "SYRIA", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "TAIWAN", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "THAILAND", 0, 0, 0, 0, 0, 0, 0x3, 0 },
	{ "UAE", 0, 0, 0, 0, 0, 0x3, 0, 0 },
	{ "UK", 0, 1, 0, 0, 1, 0x3, 0, 0x5 },
	{ "USA", 0, 0, 0, 0, 0, 0x3, 0, 0 },
};
#endif


static struct wcfxs_desc as_fxse = { "Assist SLIC/DAA Card", 0 };

static struct pci_device_id as_fxs_pci_tbl[] = 
{
	{ 0xe159, 0x0001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &as_fxse },	
	{ 0xe159, 0x0001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &as_fxse },	
	{ 0xe159, 0x0001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &as_fxse },	
	{ 0xe159, 0x0001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &as_fxse },	
	{ 0 }
};

MODULE_DEVICE_TABLE(pci,   as_fxs_pci_tbl);

static struct pci_driver as_fxs_driver = 
{
	name: 	"asfxs",
	probe: 	as_fxs_init_one,
	remove:	as_fxs_remove_one,
	suspend: NULL,
	resume:	NULL,
	id_table: as_fxs_pci_tbl,
};

static int __init as_fxs_init(void)
{
	int res;
#if 0//AS_WITH_FXO_MODULE
	int x;

	printk("count=%d\r\n" ,sizeof(fxo_modes) / sizeof(fxo_modes[0]) );
	for (x=0;x<(sizeof(fxo_modes) / sizeof(fxo_modes[0])); x++) 
	{
		printk("X=%d\r\n" ,x );
		if (!strcmp(fxo_modes[x].name, opermode))
		{
			trace
			break;
		}
	}
trace

	if (x < sizeof(fxo_modes) / sizeof(fxo_modes[0])) 
	{
	trace
		_opermode = x;
	} 
	else 
	{
	trace
		printk("Invalid/unknown operating mode '%s' specified.  Please choose one of:\n", opermode);
		for (x=0;x<sizeof(fxo_modes) / sizeof(fxo_modes[0]); x++)
			printk("  %s\n", fxo_modes[x].name);
		printk("Note this option is CASE SENSITIVE!\n");
		return -ENODEV;
	}
#endif

trace;
	res = pci_module_init(&as_fxs_driver);

	trace;
	if (res)
	{
		trace;
		return -ENODEV;
	}
	
	printk(AS_VERSION_INFO("slic") );
	return 0;
}

static void __exit as_fxs_cleanup(void)
{
	pci_unregister_driver(&as_fxs_driver);
}


MODULE_PARM(debug, "i");
MODULE_PARM(robust, "i");

#if  AS_WITH_FXO_MODULE
MODULE_PARM(opermode, "s");
#endif

MODULE_PARM(_opermode, "i");
MODULE_PARM(timingonly, "i");
MODULE_PARM(lowpower, "i");
MODULE_PARM(boostringer, "i");
MODULE_PARM(fxshonormode, "i");

MODULE_DESCRIPTION("Asttel telephone driver");
MODULE_AUTHOR("Chengdu R&D <supports@assistcn.com>");
MODULE_LICENSE("GPL");

module_init(as_fxs_init);
module_exit(as_fxs_cleanup);

