/*
* $Id: cgiStcTxCmds.c,v 1.2 2007/05/16 18:35:40 lizhijie Exp $
*/
#include "_cgi_stc.h"


int	pbx_tx_int_value(unsigned char op, unsigned char value1, unsigned char value2)
{
	PBX_COMMAND cmd;

	cmd.op = op;
	cmd.value[0] = value1;
	cmd.value[1] = value2;
	cmd.value[2] = PBX_VALUE_INVALIDATE;

	SEND_OUT_PBX(cmd);

	return 0;
}

void __digit_2_digit(unsigned char *dest, unsigned char *src, int length)
{
	int i;
	for(i=0; i< length; i++ )
	{
		if(src[i]>= '1' && src[i]<='9' )
			dest[i] = src[i]-'0';
		else if(src[i] == '0' )
			dest[i] = DIAL_DIGIT_ZERO;
		else if(src[i] == '*' )
			dest[i] = DIAL_DIGIT_ASTREIC;
		else if(src[i] == '#' )
			dest[i] = DIAL_DIGIT_SPECIAL;
		else
			dest[i] = DIAL_DIGIT_ZERO;
	}
}

void __digit_2_compressed_digit(unsigned char *dest, unsigned char *src, int length)
{
	int i, j;
	char tmpBuf[20];
	if(length>18 )
		length = 18;
	
	memset(tmpBuf, 0, strlen(tmpBuf));
	__digit_2_digit(tmpBuf, src, length);
	
	for(i=0, j=0;i<length/2;i++,j++)
	{
		dest[i] = tmpBuf[j];
		j++;
		dest[i]= (dest[i]<<4)|(tmpBuf[j]);
	}
	
	if((length%2)) /* odd number, the low 4 bit of last charator is 0x0f*/
	{
		dest[i] = (tmpBuf[j]<<4)|0X0F;
	}
}

int pbx_tx_digit_value(unsigned char op,char *value)
{
	PBX_COMMAND cmd;
	unsigned char *pbuf;
	int	length = strlen(value);
	if(length > CMD_VALUE_MAX )
		length = CMD_VALUE_MAX-1;

	cmd.op = op;
	pbuf = cmd.value;

	__digit_2_digit(pbuf, value, length);
	pbuf[length] = PBX_VALUE_INVALIDATE;

	SEND_OUT_PBX(cmd);
	return 0;
}

int pbx_tx_index_digit_value(unsigned char op, unsigned char index, unsigned char *digits)
{
	PBX_COMMAND cmd;
	unsigned char *pbuf;
	int	length = strlen(digits);
	if(length > CMD_VALUE_MAX )
		length = CMD_VALUE_MAX-1;

	cmd.op = op;
	cmd.value[0] = index;
	pbuf = cmd.value+1;

	__digit_2_digit(pbuf, digits, length);
	pbuf[length] = PBX_VALUE_INVALIDATE;

	SEND_OUT_PBX(cmd);
	return 0;
}

int pbx_tx_index_compressed_digit_value(unsigned char op, unsigned char index, unsigned char type, unsigned char *digits)
{
	PBX_COMMAND cmd;
	unsigned char *pbuf;
	int	length = strlen(digits);

	cmd.op = op;
	cmd.value[0] = index;
	if(type == PBX_VALUE_INVALIDATE)
	{
		pbuf = cmd.value+1;
	}
	else
	{
		cmd.value[1] = type;
		pbuf = cmd.value+2;
	}

	__digit_2_compressed_digit(pbuf, digits, length);
	pbuf[length] = PBX_VALUE_INVALIDATE;

	AS_MSG_LOG(AS_LOG_ERR, "encoding digits is '%s', len=%d", digits, length);
	SEND_OUT_PBX(cmd);
	return 0;
}

int get_index(char *str)
{	
	char *p = strrchr(str, '_');
	if(p)
	{
		return atoi((p+1));
	}

	return PBX_VALUE_INVALIDATE;
}


