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
	
	AUX = 0;				/*;516AUX_RAMʹ������*/
	TMOD = 0x11;		/* timer 0 and timer 1 set as 16 bit counter */
	T2CON = 0x34;		/* timer 2 is set as UART baudrate and begin to count */

	/* timer 0 is about 25 ms */
	TH0= 76;//(65536-25000*22.1184/12)/256;
	TL0= 0;//(65536-(unsigned )(25000*22.1184/12))%256;	
	
	TH2= 0xff;		//;;9600bps	
	TL2= 0xB8;
	RCAP2H= 0xFF;		/* ��ʱ��2��ֵ��װֵ*/
	RCAP2L= 0xB8;
	PCON=0;			/* �����ʲ�����*/
	SCON=0x50;		/* ��������Ϊ8λUART */

	IE=0x9B;			/* UART, timer 0 and 1, external Interrupt 0 */
	PS=1;			/* �����жϼ�������*/
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
	send_busycome(0);		/*/��ʼ��7064ʱ��*/
	send_busycome(0xff);
/* ��ʼ��7064ʱ�� �������м�1���������оƬ��busucomcome=0x01ʱ��*/
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
	TIME_BYTES_WRITE(0x8e,&i,1); /*������дʹ��*/
}

void	Osc_Enable()
{
	INT8U data i=0;	/* 0 :enable oscilator */
	TIME_BYTES_WRITE(0x80,&i,1);
}


void	pbx_reset_systime(void)		//����ʱ��
{
	Write_Enable();
	Osc_Enable();
	pbxCurrentTime[0]=00;	//��
	pbxCurrentTime[1]=00; 	//��
	pbxCurrentTime[2]=0x12;	//ʱ
	pbxCurrentTime[3]=0x14;	//��
	pbxCurrentTime[4]=0x10;	//��
	pbxCurrentTime[5]=0x04;	//����
	pbxCurrentTime[6]=0x04;	//��
	pbxCurrentTime[7]=0x00;	//д����
	TIME_BYTES_WRITE(0xbe, pbxCurrentTime,8);	//�����ֽ�ΪBEh,���ֽ�д��ģʽ��ģ��Ϊ8 ��
}
#endif

void	pbx_reset_systime(void)		//����ʱ��
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
	__reset_all_ICs();	/*��ʼ��������Χ��·*/
	
	power_on_rst();	/*;��ʼ��IRAM��AUX�Ĵ���*/

	pbx_reset_systime();
	
//	PBX_READ_SYS_TIME( pbxCurrentTime);
	
}


