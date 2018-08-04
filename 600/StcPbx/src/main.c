/*
* $Id: main.c,v 1.12 2007/07/25 17:49:09 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>

#include "function_declarations.h"
#include "globalVariables.h"

void	pbx_OK(void)
{
	pbx_buzzer_ctrl(TRUE);
	pbx_mdelay(200);
	pbx_buzzer_ctrl(FALSE);
}

void	pbx_2_beep(void)
{
	pbx_buzzer_ctrl(TRUE);
	pbx_mdelay(100);
	pbx_buzzer_ctrl(FALSE);
	pbx_mdelay(600);

	pbx_buzzer_ctrl(TRUE);
	pbx_mdelay(100);
	pbx_buzzer_ctrl(FALSE);
}

void pbxBuzzer( INT8U ms50)/* unit os 50 ms */
{
	if(buzzerTimeout>0)
		return;

	buzzerTimeout = ms50;
	pbx_buzzer_ctrl(TRUE);
}

void	pbx_mdelay(INT8U ms)
{
	for(ms/=5; ms>0; ms--)
		delay5ms();
}

#if 0
static void __check_state(void)/*复位矩阵通道处理	*/
{
	INT8U	data i;
	i=TEL_ZJSTUS0|TEL_ZJSTUS1;

	if(i==0)
	{
		i=track_ldstatus0|track_ldstatus1;
		if(i==0)
		{/* no any call, then reset MT8816 */
			//if( (columnFrees&0xf0) != 0 )
			columnFrees=0xff;
//			RST_MT8816();
			reset_switch();
		}
	}
}
#endif

void	pbx_ring_all(INT8U	 whichOne)
{
	pbxHwStatus.extRelayCtrl =  whichOne;
	EXT_CTRL_RINGING();
	pbx_mdelay(250);
	WATCHDOG_FEED();
	pbx_mdelay(250);
	
	pbxHwStatus.extRelayCtrl = 0;
	EXT_CTRL_RINGING();
	WATCHDOG_FEED();
}


void	pbx_factory_default(void)
{
	pbx_reset_factory_default();
	restoreFromNvram();
}


void	main(void)
{	
	pbx_init();

	pbx_ring_all(0XFF);

	while(1)
	{
		pbx_timer_task();

		pbx_statemachine_handle();
		
		uart_rx();
	};
}

