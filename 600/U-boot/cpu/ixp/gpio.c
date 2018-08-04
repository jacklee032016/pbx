/*
 * modified from Linux/arch/arm/arch-ixp425/ixp-gpio.c
 * GPIO configuration for Beeper
 * Li Zhijie 2005.06.14
 * $Log: gpio.c,v $
 * Revision 1.2  2006/12/01 14:24:45  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/11/29 09:09:44  lizhijie
 * AS600 ICE
 *
 * Revision 1.4  2006/06/06 07:19:24  wangwei
 * no message
 *
 * Revision 1.3  2005/07/04 02:55:04  lizhijie
 * no message
 *
 * Revision 1.2  2005/06/16 02:56:11  lizhijie
 * update beeper from GPIO7 to GPIO6
 *
 * Revision 1.1  2005/06/15 10:36:42  lizhijie
 * no message
 *
 * $Id: gpio.c,v 1.2 2006/12/01 14:24:45 lizhijie Exp $
 *
 */
#include <common.h>
#include <asm/arch/ixp425.h>

#define AS_IXP425_GPIO_REG(x) 			((volatile u32 *)(IXP425_GPIO_BASE_PHYS+(x)))

#define AS_IXP425_GPIO_GPOUTR			AS_IXP425_GPIO_REG(IXP425_GPIO_GPOUTR_OFFSET)
#define AS_IXP425_GPIO_GPOER       		AS_IXP425_GPIO_REG(IXP425_GPIO_GPOER_OFFSET)


#define IXP425_GPIO_PIN_6  					0		/* 0 : Beep */

#define BEEP_LINE                           			IXP425_GPIO_PIN_6

/* added PCI reset into bootloader and remove PCI reset from kernel arm/ixp425/ixdp425_pci.c 
Li Zhijie, 2006.12.01 */
#define IXP425_GPIO_PIN_13  				13	/* 13 : PCI reset */
#define	PCI_RESET_LINE						IXP425_GPIO_PIN_13

/* GPIO pin types */
#define IXP425_GPIO_OUT 				0
#define IXP425_GPIO_IN  					1

/* GPIO signal types */
#define IXP425_GPIO_LOW				0
#define IXP425_GPIO_HIGH				1


void gpio_line_set(u8 line, int value)
{
	if (value == IXP425_GPIO_HIGH)
	    *AS_IXP425_GPIO_GPOUTR |= BIT(line);
	else if (value == IXP425_GPIO_LOW)
	    *AS_IXP425_GPIO_GPOUTR &= ~BIT(line);
}

void gpio_line_config(u8 line, u32 outOrIn)
{
	u32 enable;

	enable = *AS_IXP425_GPIO_GPOER;

	if (outOrIn & IXP425_GPIO_IN) 
	{
		enable |= BIT(line);
	}
	else
	{
		enable &= ~BIT(line);
	} 

	*AS_IXP425_GPIO_GPOER = enable;
}

static void  as_ixp_beep_enable(void )
{	
	gpio_line_config(BEEP_LINE, IXP425_GPIO_OUT);		
	gpio_line_set(BEEP_LINE, IXP425_GPIO_HIGH);		
}

static void as_ixp_beep_disable(void )
{
	gpio_line_config(BEEP_LINE, IXP425_GPIO_OUT);		
	gpio_line_set(BEEP_LINE, IXP425_GPIO_LOW);
}

extern void udelay (unsigned long usec);
void beep_ok(void )
{
	/* added PCI reset. Li Zhijie, 2006.12.01 */
	gpio_line_config(PCI_RESET_LINE, IXP425_GPIO_OUT);
	gpio_line_set(PCI_RESET_LINE, IXP425_GPIO_LOW);
	udelay(1000);
	gpio_line_set(PCI_RESET_LINE, IXP425_GPIO_HIGH);
	/* End of added */

	as_ixp_beep_enable();
	udelay(200000);
	as_ixp_beep_disable();
}

void beep_fail(void )
{
	as_ixp_beep_enable();
	udelay(600000);
	as_ixp_beep_disable();
	
	as_ixp_beep_enable();
	udelay(600000);
	as_ixp_beep_disable();
}

