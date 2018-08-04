/*
* $Id: pbx_hw_ctl.c,v 1.3 2007/07/22 20:48:24 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


void cfg_cpld_cmd(INT8U cmdWrite)
{
	NOP();
	NOP();

	P2 = cmdWrite;
	NOP();
	NOP();
	EPM7064_CS = FALSE;
	NOP();
	NOP();
	EPM7064_CS = TRUE;
	NOP();
	NOP();
}

void cfg_cpld_value_cmd(INT8U cmdWrite, INT8U valueP0)
{
	P0 = valueP0;

	cfg_cpld_cmd(cmdWrite);

	P0 = PBX_VALUE_INVALIDATE;
}

void	dtfm_encoder_digit(INT8U index, INT8U digit)
{
	if(index != 0)
	{
		DTMT_ENCODER1_DISBALE();
		digit = (digit<<4)|0xF0;
	}
	else
	{
		DTMT_ENCODER0_DISBALE();
	}
	P0 = digit;

	DTMF_ENCODER_CS = TRUE;
	NOP();
	NOP();

	DTMF_ENCODER_CS = FALSE;
	NOP();
	NOP();
	
	if(index!=0)
	{
		cfg_cpld_cmd(CPLD_CMD_DTMF1_ENABLE);
	}
	else
	{
		cfg_cpld_cmd(CPLD_CMD_DTMF0_ENABLE);
	}

	P0 = PBX_VALUE_INVALIDATE;
}

/*CS signal of music, ringback and 8 dtmf decoder */
void hc138_enable(INT8U which)
{
	HC138IN	&= 0XF0;
	HC138IN	|= (which&0x0F);	/*when ON, HC138 Y4 output 0; OFF Y4 output 1 */
	
	HC138EN = TRUE;
	NOP();
	NOP();
}

void hc138_value_cmd(INT8U which, INT8U valueP0)
{
	P0 = valueP0;

	hc138_enable(which);
	NOP();
	NOP();
	HC138_DISABLE();
	
	P0 = PBX_VALUE_INVALIDATE;
}


void pbx_buzzer_ctrl(BOOL status)
{
	if(status == TRUE)
	{
		PBX_SET_FLAGS(pbxHwStatus.pstnRelayCtrl, PSTN_RELAY_CTRL_BUZZER);
	}
	else
	{
		PBX_CLEAR_FLAGS(pbxHwStatus.pstnRelayCtrl, PSTN_RELAY_CTRL_BUZZER);
	}

	PSTN_CTRL_HOOK();
}

void		reset_switch(void)
{
	MT8816_CS = FALSE;
	MT8816_ST0 = FALSE;
	MT8816_ST1 = FALSE;

	/* send high level to reset pin of 8816 */
	PBX_CLEAR_FLAGS(pbxHwStatus.pstnRelayCtrl, PSTN_RELAY_CTRL_RESET_8816);
	PSTN_CTRL_HOOK();

	/* send low level to reset pin of 8816 */
	PBX_SET_FLAGS(pbxHwStatus.pstnRelayCtrl, PSTN_RELAY_CTRL_RESET_8816);
	PSTN_CTRL_HOOK();
}

/*CS is connect to STC directly and read status from 74HC245 */
INT8U pbx_read_hw_status(INT8U type)
{
	INT8U data value;
	
	P0 = PBX_VALUE_INVALIDATE;
	
	if(type == HW_STATUS_EXT_HOOK)
	{/* refer to schema 4.D and 12.D */
		READ_EXT_HOOK_CS = FALSE;
	}
	else if(type== HW_STATUS_PSTN_HOOK_BUSY )
	{/* refer to schema 4.D and 8.B */
		READ_PSTN_HOOK_CS = FALSE;
	}
	else if(type== HW_STATUS_DTMF_DECODER)
	{
		READ_DTMF_CS = FALSE;
	}
	else
	{
		READ_OTHERS_CS = FALSE;
	}

	NOP();
	NOP();
	value = P0;
	
	if(type == HW_STATUS_EXT_HOOK)
	{
		READ_EXT_HOOK_CS = TRUE;
	}
	else if(type== HW_STATUS_PSTN_HOOK_BUSY )
	{
		READ_PSTN_HOOK_CS = TRUE;
	}
	else if(type== HW_STATUS_DTMF_DECODER)
	{
		READ_DTMF_CS = TRUE;
	}
	else
	{
		READ_OTHERS_CS = TRUE;
	}

	P0 = PBX_VALUE_INVALIDATE;
	return value;
}


void switch_connect(INT8U row, INT8U col, BOOL isConnect)
{
	INT8U data value = row;

	value = (value<<3);
	if(isConnect == TRUE)
		value |= SET_BIT(1, 7);

	MT8816_CS = TRUE;

	if(col>7)
	{
		P0 = value|(col%8);
		NOP();
		NOP();
		MT8816_ST1 = TRUE;
		NOP();
		NOP();
		MT8816_ST1 = FALSE;
	}
	else
	{
		P0 = value|col;
		NOP();
		NOP();
		MT8816_ST0 = TRUE;
		NOP();
		NOP();
		MT8816_ST0 = FALSE;
	}

	MT8816_CS = FALSE;

	P0 = PBX_VALUE_INVALIDATE;
}

