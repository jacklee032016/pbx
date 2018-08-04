/*
* $Id: pbx_init.c,v 1.8 2007/07/25 17:55:22 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

/*
* Timer/Counter 0 : time gap for application
* Timer/Counter 1 : rx Caller ID of PSTN after external interrupt 0
* Timer/Counter 2 : Uart baudrate
*/
void	__reset_registers(void )
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

void	__reset_all_ICs(void )
{
	ALL_ICS_RESET();
	HT1381_RESET_OFF();
	RECORDER_RESET();
	DTMF_DECODER_DISABLE();
	reset_switch();
	
	pbxHwStatus.extToneCtrl = 0;
	EXT_CTRL_TONE();
	
	pbxHwStatus.extRelayCtrl = 0;
	EXT_CTRL_RINGING();

	pbxHwStatus.pstnFskCtrl = 0;
	PBX_SET_FLAGS(pbxHwStatus.pstnFskCtrl, PSTN_FSK_CTRL_WDI );
	PSTN_CTRL_DECODER();

	pbxHwStatus.pstnRelayCtrl = PBX_VALUE_INVALIDATE;
	PSTN_CTRL_HOOK();
	pbxHwStatus.pstnRelayCtrl = 0x10;
	PSTN_CTRL_HOOK();

#if 0	
	send_busycome(0);		/*/初始化7064时序*/
	send_busycome(0xff);
/* 初始化7064时序 （连接中继1到来电接收芯片（busucomcome=0x01时）*/
	busucomcome=0x00;	
	send_busycome(busucomcome);
#endif

	/* bug here for music and ringback tone, default is high level which made it ON */
	pbxHwStatus.musicStatus = 0x0;
	EXT_CTRL_MUSIC();

	pbxHwStatus.ringbackStatus = 0x0;
	EXT_CTRL_RINGBACK();

	pbxHwStatus.selfTestFlags = SELF_TEST_TYPE_NONE;

	EPM7064_CS = TRUE;
	
//	CONNECT(CHANNEL_ROW_IP_R_2, CHANNEL_COL_MUSIC);
}


#if 0
void	Write_Enable()
{
	INT8U data i=0;	/* 0  : enable write */
	TIME_BYTES_WRITE(0x8e,&i,1); /*万年历写使能*/
}

void	Osc_Enable()
{
	INT8U data i=0;	/* 0 :enable oscilator */
	TIME_BYTES_WRITE(0x80,&i,1);
}


void	pbx_reset_systime(void)		//配置时间
{
	Write_Enable();
	Osc_Enable();
	pbxCurrentTime[0]=00;	//秒
	pbxCurrentTime[1]=00; 	//分
	pbxCurrentTime[2]=0x12;	//时
	pbxCurrentTime[3]=0x14;	//日
	pbxCurrentTime[4]=0x10;	//月
	pbxCurrentTime[5]=0x04;	//星期
	pbxCurrentTime[6]=0x04;	//年
	pbxCurrentTime[7]=0x00;	//写保护
	TIME_BYTES_WRITE(0xbe, pbxCurrentTime,8);	//命令字节为BEh,多字节写入模式此模块为8 个
}
#endif

void	pbx_reset_systime(void)		//配置时间
{
	pbxHwStatus.sysTime.second = 0x45;
	pbxHwStatus.sysTime.minute = 0x45;
	pbxHwStatus.sysTime.hour = 0x23;
	pbxHwStatus.sysTime.date = 0x23;
	pbxHwStatus.sysTime.month = 0x12;
	pbxHwStatus.sysTime.day= 0x07;
	pbxHwStatus.sysTime.year = 0x77;
	pbxHwStatus.sysTime.protect = 0;
	
#if 0//UART_DEBUG
	uartTxCmds[uartTxInIndex].value[0] = UART_DEBUG_STSTIME;
	uartTxCmds[uartTxInIndex].value[1] = pbxHwStatus.sysTime.second;
	uartTxCmds[uartTxInIndex].value[2] = pbxHwStatus.sysTime.minute;
	uartTxCmds[uartTxInIndex].value[3] = pbxHwStatus.sysTime.hour;
	uart_tx_cmd(CMD_UART_DEBUG, 4);
#endif
	pbx_systime_write();
}

void	pbx_init(void )
{
	__reset_registers();
	ht1381_enable();
	__reset_all_ICs();	/*初始化所有外围电路*/
	
	power_on_rst();	/*;初始化IRAM、AUX寄存器*/

	pbx_reset_systime();
	
//	PBX_READ_SYS_TIME( pbxCurrentTime);
	
}


