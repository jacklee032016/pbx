/*
 * modified from Linux/arch/arm/arch-ixp425/ixp-gpio.c
 * GPIO configuration for Beeper
 * Li Zhijie 2005.06.14
 * $Log: gpio.c,v $
 * Revision 1.1.1.1  2006/11/30 16:25:16  lizhijie
 * AS800 U-Boot
 *
 * Revision 1.1.1.1  2006/01/23 06:08:15  wangwei
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
 * $Id: gpio.c,v 1.1.1.1 2006/11/30 16:25:16 lizhijie Exp $
 *
 */
#include <common.h>
#include <asm/arch/ixp425.h>

#define AS_IXP425_GPIO_REG(x) 			((volatile u32 *)(IXP425_GPIO_BASE_PHYS+(x)))

#define AS_IXP425_GPIO_GPOUTR			AS_IXP425_GPIO_REG(IXP425_GPIO_GPOUTR_OFFSET)
#define AS_IXP425_GPIO_GPOER       		AS_IXP425_GPIO_REG(IXP425_GPIO_GPOER_OFFSET)


//#define IXP425_GPIO_PIN_6  				0
//#define BEEP_LINE                           		IXP425_GPIO_PIN_6
/*add,wangwei,2006.01.17  */
#define IXP425_GPIO_PIN_0  				0
#define BEEP_LINE                           		IXP425_GPIO_PIN_0

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
	gpio_line_set(BEEP_LINE, IXP425_GPIO_LOW);		
}

static void as_ixp_beep_disable(void )
{
	gpio_line_config(BEEP_LINE, IXP425_GPIO_OUT);		
	gpio_line_set(BEEP_LINE, IXP425_GPIO_HIGH);
}

extern void udelay (unsigned long usec);
void beep_ok(void )
{
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

