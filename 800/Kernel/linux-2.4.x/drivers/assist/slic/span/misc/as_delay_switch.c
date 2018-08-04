
#include "asstel.h"
#include "as_fxs.h"
static int  as_ixp_switch_enable(void )
{
	gpio_line_config(GPIO_SWITCH_PHONE,IXP425_GPIO_OUT);	
	gpio_line_set(GPIO_SWITCH_PHONE,IXP425_GPIO_LOW);	
	return 0;
}

static int as_ixp_switch_disable(void)
{
	gpio_line_config(GPIO_SWITCH_PHONE,IXP425_GPIO_OUT);	
	gpio_line_set(GPIO_SWITCH_PHONE,IXP425_GPIO_HIGH);
	return 0;
}

int  as_realy_switch_phone(struct as_dev_chan *chan,  unsigned long data)
{

		int res;
		copy_from_user(&res, (int *)data, sizeof(res));
		switch(res)
		{
			case AS_SWITCH_2_LINE:
				return as_ixp_switch_enable();
				
			case AS_SWITCH_2_FXS:
				return as_ixp_switch_disable();
				
			default:
				return -1;
		}
	return 0;
}



