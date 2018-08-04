/*
* $Id: main.c,v 1.2 2007/08/05 21:09:59 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>

#include "declarations.h"
#include "globalVariables.h"

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
			//if( (pbxHwStatus.columnFrees&0xf0) != 0 )
			pbxHwStatus.columnFrees=0xff;
//			RST_MT8816();
			reset_switch();
		}
	}
}
#endif


/*
* Timer/Counter 0 : time gap for application
* Timer/Counter 1 : rx Caller ID of PSTN after external interrupt 0
* Timer/Counter 2 : Uart baudrate
*/
void	__stcInit(void )
{
	AUX = 0x87;
	AUX = 0x59;
	AUX1= AUX1|0x10;

	AUXR = 0x01;
	
	AUX = 0;				/*;516AUX_RAM使能设置*/
	TMOD = 0x11;		/* timer 0 and timer 1 set as 16 bit counter */
	T2CON = 0x34;		/* timer 2 is set as UART baudrate and begin to count */

	/* timer 0 is about 25 ms */
	TH0= 76;//(65536-25000*22.1184/12)/256;
	TL0= 0;//(65536-(unsigned )(25000*22.1184/12))%256;	
	
	TH2= 0xff;		//;;9600bps	
	TL2= 0xB8;
	RCAP2H= 0xFF;		/* 定时器2初值重装值*/
	RCAP2L= 0xB8;
	PCON=0;			/* 波特率不倍增*/
	SCON=0x50;		/* 串口设置为8位UART */

	IE=0x9B;			/* UART, timer 0 and 1, external Interrupt 0 */
	PS=1;			/* 串口中断级别优先*/
	TR0=1;
}

void	__resetExternICs(void )
{
	ht1381_enable();
	DTMF_ENCODER_CS = FALSE;
	HT1381_RESET_OFF();
	RECORDER_RESET();
	DTMF_DECODER_DISABLE();
	reset_switch();
	
	extDevices.toneCtrl = 0;
	EXT_CTRL_TONE();
	
	extDevices.relayCtrl = 0;
	EXT_CTRL_RINGING();

	pstnDevices.fskCtrl = 0;
	PBX_SET_FLAGS(pstnDevices.fskCtrl, PSTN_FSK_CTRL_WDI );
	PSTN_CTRL_DECODER();

	pstnDevices.relayCtrl = PBX_VALUE_INVALIDATE;
	PSTN_CTRL_HOOK();
	pstnDevices.relayCtrl = 0x10;
	PSTN_CTRL_HOOK();

	/* bug here for music and ringback tone, default is high level which made it ON */
	extDevices.musicStatus = 0x0;
	EXT_CTRL_MUSIC();

	extDevices.ringbackStatus = 0x0;
	EXT_CTRL_RINGBACK();

	pbxHwStatus.selfTestFlags = SELF_TEST_TYPE_NONE;

	EPM7064_CS = TRUE;
	
//	CONNECT(CHANNEL_ROW_IP_R_2, CHANNEL_COL_MUSIC);

	PX0=1;		/*;设置中断优先级INT0、INT1，T1为高优先级*/
	PT1=1;
	EX0=1;		/*;设置中断允许INT0、INT1允许中断*/
	EA=1;	
	
#if WITH_FSK_MT88E39	
	IT0 = FALSE;		/* trigger with falling edge for MT88E39 */
	MT88E39_DCLK = FALSE;
#else
	IT0 = FALSE;		/* trigger in low level for SM8823AP */
#endif
	CTS = UART_CTS_ENABLE;
}

void	_pbx_init(void )
{
	__stcInit();
	__resetExternICs();
	resetAllDataStructures();
}


void	main(void)
{	
	_pbx_init();

	pbx_ring_all(0XFF);

	while(1)
	{
		pbxTimerTasks();
		
		uart_rx();

		pbx_statemachine_handle();
	};
}

