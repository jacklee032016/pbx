/*
* $Id: pbx_uart_tx_cfgs.c,v 1.1 2007/05/11 20:14:13 lizhijie Exp $
*/

#define	PBX_SEND_BUFFER(bufferIndex, endPosition)	\
	do{	\
		if(++uartCmdSeq==0xff)	\
			++uartCmdSeq;	\
		uartTxCmds[bufferIndex][1]=uartCmdSeq;	\
		uartTxCmds[bufferIndex][endPosition]=0xff;	\
		busy_send_buf(bufferIndex);	\
		send_point--;	\
		}while(0)


BOOL pbx_tx_trunck_direct_dialing(void )
{
	INT8U data i,j;
	
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(boolTrunkMode)
	{
		uartTxCmds[i][0]=0xa3;
	}
	else 
	{
		uartTxCmds[i][0]=0xa2;
	}

	PBX_SEND_BUFFER(i, 2);
	
	return TRUE;
}

BOOL pbx_tx_no_dial_action_is_auto(void )
{
	INT8U data i,j;
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;

	if(boolNoDialAction)
	{
		uartTxCmds[i][0]=0xa4;
	}
	else 
	{
		uartTxCmds[i][0]=0xa5;
	}
	
	PBX_SEND_BUFFER(i, 2);
	return TRUE;
}

BOOL pbx_tx_forbid_line2line(void)
{
	INT8U data i,j;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(boolLine2Line)
	{
		uartTxCmds[i][0]=0xa7;
	}
	else 
	{
		uartTxCmds[i][0]=0xa6;
	}
	
	PBX_SEND_BUFFER(i, 2);
	return TRUE;
}

BOOL pbx_tx_differ_ringing(void)
{
	INT8U data i;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(boolDifferRinging)
	{
		uartTxCmds[i][0]=0xa9;
	}
	else 
	{
		uartTxCmds[i][0]=0xa8;
	}
	PBX_SEND_BUFFER(i, 2);
}

BOOL pbx_tx_callId_mode_fsk(void)
{
	INT8U data i,j;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(boolCallIdMode)
	{
		uartTxCmds[i][0]=0xaa;
	}
	else 
	{
		uartTxCmds[i][0]=0xab;
	}
	PBX_SEND_BUFFER(i, 2);

	return TRUE;
}


BOOL pbx_tx_group_call_opened(void)
{
	INT8U data  i;
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
	
	if(boolGroupCall)
	{
		uartTxCmds[i][0]=0xda;
	}
	else 
	{
		uartTxCmds[i][0]=0xdb;
	}

	PBX_SEND_BUFFER(i, 2);
	return TRUE;
}


BOOL pbx_tx_special_service_first(void)
{
	INT8U data i;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(boolSpecialFirst)
	{
		uartTxCmds[i][0]=0xd2;
	}
	else 
	{
		uartTxCmds[i][0]=0xd3;
	}
	PBX_SEND_BUFFER(i, 2);
	return TRUE;
}

BOOL pbx_tx_special_service_number(INT8U serviceIndex)
{
	INT8U data i, j, end;
	BOOL flag=0;
	i=found_send_buf();
	if(i!=0xff)
		return FALSE;
		
	if(specialServices[serviceIndex][0] != 0xff)
	{
		uartTxCmds[i][0]=0xd5;

		for(j=0;j<specialServices[serviceIndex][0];j++)
		{
			if(flag)
				uartTxCmds[i][j+3]=specialServices[j/2+1]&0x0f;
			else
				uartTxCmds[i][j+3]=specialServices[j/2+1]>>4;
			
			flag=!flag;
		}
		end = j+3;
	}
	else
	{
		uartTxCmds[i][0]=0xd6;
		end = 3;
	}
	uartTxCmds[i][2]= serviceIndex;
	PBX_SEND_BUFFER(i, end);

	return 0;
}

BOOL pbx_tx_night_service_time(void )
{
	INT8U data i,j;

	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
	
	uartTxCmds[i][0]=0xe3;
	for(j=2;j<6;j++)
		uartTxCmds[i][j] = nightServiceTime[j-2];
		
	PBX_SEND_BUFFER(i,  j);
	return TRUE;
}

BOOL pbx_tx_line_console(INT8U lineIndex )
{
	INT8U	data i;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;
	
	uartTxCmds[i][0]=0xac;
	uartTxCmds[i][2]=lineIndex;
	uartTxCmds[i][3]=virtualConsoles[lineIndex];
	
	PBX_SEND_BUFFER(i, 4);
	return 0;
}

BOOL pbx_tx_line_open(INT8U lineIndex, BOOL isOpened)
{
	INT8U	data i,j;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if(isOpened)
	{
		uartTxCmds[i][0]=0xa1;
	}
	else
	{
		uartTxCmds[i][0]=0xa0;
	}
	uartTxCmds[i][2]= lineIndex;	/* called when configed by Phone, must minus 1 */

	PBX_SEND_BUFFER(i, 3);
	return TRUE;
}


BOOL pbx_tx_line_binding_group(INT8U lineIndex, INT8U y)
{
	INT8U data i;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	uartTxCmds[i][0]=0xe2;
	uartTxCmds[i][2]= lineIndex;
	uartTxCmds[i][3]= traunkBindingExtGroup[lineIndex];
	
	PBX_SEND_BUFFER(i, 4);
	return 0;
}


BOOL pbx_tx_ext_callId_status(INT8U extIndex, BOOL isOpened)//开来电显示
{
	INT8U	data i;
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
	
	if(isOpened)
	{
		uartTxCmds[i][0]=0xb6;
	}
	else
	{
		uartTxCmds[i][0]=0xb5;
	}
	uartTxCmds[i][2] = extIndex - 1;	/* called when configed by Phone, must minus 1 */

	PBX_SEND_BUFFER(i, 3);
	return TRUE;
}


BOOL pbx_tx_ext_callwait(INT8U extIndex, BOOL isOpened)
{
	INT8U	data i;
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
	
	if(isOpened)
	{
		uartTxCmds[i][0]=0xd0;
	}
	else
	{
		uartTxCmds[i][0]=0xd1;
	}
	uartTxCmds[i][2] = extIndex;
	
	PBX_SEND_BUFFER(i, 3);
	return TRUE;
}


BOOL pbx_tx_ext_not_disturb(INT8U extIndex, BOOL isOpende)
{
	INT8U	data i;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;

	if(isOpende)
	{
		uartTxCmds[i][0]=0xde;
	}
	else
	{
		uartTxCmds[i][0]=0xdf;
	}

	uartTxCmds[i][2] = extIndex;
	PBX_SEND_BUFFER(i, 3);
	return TRUE;
}


BOOL pbx_tx_ext_call_length(INT8U extIndex )
{
	INT8U data i;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;

	uartTxCmds[i][0]=0xb0;
	uartTxCmds[i][2] = extIndex;
	uartTxCmds[i][3] = (INT8U)(phoneCallLengthes[extIndex]/600);
	
	PBX_SEND_BUFFER(i, 4);
	return	0;
}

BOOL pbx_tx_ext_authority_level(INT8U extIndex)
{
	INT8U data i;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;

	uartTxCmds[i][0]=0xaf;
	uartTxCmds[i][2]= extIndex;
	uartTxCmds[i][3]= phoneAuthorities[extIndex];

	PBX_SEND_BUFFER(i, 4);
	return TRUE;
}

BOOL pbx_tx_global_pstn_prefix(INT8U x)
{
	INT8U	data i, end;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;
	
	if(pstn_prefix)
	{
		uartTxCmds[i][0] = 0xb1;
		uartTxCmds[i][2] = pstn_prefix;
		end = 3;
	}	
	else
	{
		uartTxCmds[i][0]=0xb2;
		end = 2;
	}

	PBX_SEND_BUFFER(i, end);
	return 0;
}


BOOL pbx_tx_global_account_level(INT8U accountIndex, INT8U level)
{
	INT8U data i, end;
	i=found_send_buf();
	if(i!=0xff)
		return TRUE;
	
	if(level)
	{
		uartTxCmds[i][0]=0xad;
		uartTxCmds[i][3]= (level>>4);
		end = 4;
	}
	else
	{
		uartTxCmds[i][0]=0xae;
		end = 3;
	}
	
	uartTxCmds[i][2]= accountIndex;
	PBX_SEND_BUFFER(i, end);
	return 0;
}


BOOL pbx_tx_global_ip_prefix(void)
{
	INT8U data i,j,k;
	
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
			
	uartTxCmds[i][0]= CMD_CFG_IP_PREFIX;//0xd4;
			
	for(j=1,k=2;k<ip_prefix[0]+2;j++)
	{
		uartTxCmds[i][k++]=ip_prefix[j]>>4;
		uartTxCmds[i][k++]=ip_prefix[j]&0x0f;
	}
	
	if(k>ip_prefix[0]+2)
		k--;		//号码为奇数		
	PBX_SEND_BUFFER(i, k);
	return TRUE;
}



BOOL pbx_tx_ext_delay_to_lines_time(INT8U extIndex)
{
	INT8U	data i;
	i=found_send_buf();
	if(i==0xff)
		return FALSE;
	
	uartTxCmds[i][0]=0xbe;
	uartTxCmds[i][2]= extIndex;
	uartTxCmds[i][3]= delay_out[extIndex]/10;
	PBX_SEND_BUFFER(i, 4);
	return TRUE;
}

BOOL pbx_tx_ext_binding_group(INT8U extIndex)
{
	INT8U data i;
	i=found_send_buf();
	if(i == 0xff)
		return FALSE;

	uartTxCmds[i][0]=0xdd;
	uartTxCmds[i][2]= extIndex;
	uartTxCmds[i][3]= phone_groups[extIndex]&0x7f;
	uartTxCmds[i][4]=*(extIndex+1)>>4;

	PBX_SEND_BUFFER(i, 5);
	return TRUE;
}

BOOL pbx_tx_ext_secret_ext(INT8U extIndex)
{
	INT8U data i, end;

	i=found_send_buf();
	if(i == 0xff)
		return FALSE;
	
	if(phoneSecretBranchs[extIndex]!= 0xFF)
	{
		uartTxCmds[i][0]=0xb9;
		uartTxCmds[i][3]= phoneSecretBranchs[extIndex];
		end = 4;
	}	
	else
	{
		uartTxCmds[i][0]=0xba;
		end = 3;
	}
	
	uartTxCmds[i][2]= extIndex;
	PBX_SEND_BUFFER(i, end);
	return TRUE;
}

BOOL pbx_tx_ext_dialout_mode(INT8U extIndex)
{
	INT8U	data i;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
	
	if( phoneDialoutModes[extIndex]>10 )
	{
		uartTxCmds[i][0]=0xc2;
		uartTxCmds[i][3]= EXT_FIRST_PSTN - phoneDialoutModes[extIndex];
	}
	else
	{
		uartTxCmds[i][0]=0xc1;
		uartTxCmds[i][3]= phoneDialoutModes[extIndex];
	}
	
	uartTxCmds[i][2]= extIndex;
	PBX_SEND_BUFFER(i, 4);
	return TRUE;
}

BOOL pbx_tx_ext_left_tranfer_times(INT8U extIndex)
{
	INT8U data i;
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;

	uartTxCmds[i][0]= 0xd9;
	uartTxCmds[i][2]= extIndex;
	uartTxCmds[i][3]= phoneLeftTranferTimes[extIndex];

	PBX_SEND_BUFFER(i, 4);
	return TRUE;
}

BOOL pbx_tx_ext_busy_ext(INT8U extIndex)
{
	INT8U data i, end;

	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
		
	if(phoneBusyBranchs[extIndex] != EXT_TRANSDER_EXT_IS_NULL)
	{
		uartTxCmds[i][0]=0xbb;
		uartTxCmds[i][3] = phoneBusyBranchs[extIndex];
		end = 4;
	}	
	else
	{
		uartTxCmds[i][0]=0xbc;
		end = 3;
		return 0;
	}

	uartTxCmds[i][2]=extIndex;
	PBX_SEND_BUFFER(i, end);
	return TRUE;
}


BOOL pbx_tx_ext_phone_number(INT8U extIndex)
{
	INT8U data i,j;
	
	i=found_send_buf();
	if(i ==0xff)
		return FALSE;
				
	uartTxCmds[i][0]=0xbf;

	uartTxCmds[i][2]= extIndex;
	uartTxCmds[i][3]= phonePhoneNumbers[extIndex][1]>>4;
	j=4;
	if(phonePhoneNumbers[extIndex][0]>=2)
		uartTxCmds[i][j++]=phonePhoneNumbers[extIndex][1]&0x0f;
	if(phonePhoneNumbers[extIndex][0]>=3)
		uartTxCmds[i][j++]=phonePhoneNumbers[extIndex][2]>>4;
	if(phonePhoneNumbers[extIndex][0]==4)
		uartTxCmds[i][j++]=phonePhoneNumbers[extIndex][2]&0x0f;
				
	PBX_SEND_BUFFER(i, j);
	return TRUE;
}


bit	cancel_phoneNightServices(INT8U x)//取消夜服电话
{
	INT8U	data i;
	i=found_send_buf();
	if(i!=0xff)
		{
		if(wr24c08_single(0xff,ROM_ADDRESS_NIGHT_SERVICE+x*16+1))
			{
			phoneNightServices[x][1]=0xff;
			uartTxCmds[i][0]=0xcd;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			uartTxCmds[i][3]=0xff;
			busy_send_buf(i);
			return 1;
			}
		else
			{
			send_point--;
			return 0;
			}
		}
	else
		return 0;
}
bit	cancel_lwzy(INT8U x)//取消离位转移
{
	INT8U	data i;
	i=found_send_buf();
	if(i!=0xff)
		{
		if(wr24c08_single(0xff,ROM_ADDRESS_LEFT_TRANSFER+x*16+1))
			{
			phoneLeftTransfers[x][1]=0xff;
			uartTxCmds[i][0]=0xb8;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			uartTxCmds[i][3]=0xff;
			busy_send_buf(i);
			return 1;
			}
		else
			{
			send_point--;
			return 0;
			}
		}
	else
		return 0;
}
bit	cancel_phoneUnconditionalTransfers(INT8U x)//取消无条件转移
{
	INT8U	data i;
	i=found_send_buf();
	if(i!=0xff)
		{
		if(wr24c08_single(0xff,ROM_ADDRESS_UNCONDITIONAL_TRANSFER+x*16+1))
			{
			phoneUnconditionalTransfers[x][1]=0xff;
			uartTxCmds[i][0]=0xd8;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			uartTxCmds[i][3]=0xff;
			busy_send_buf(i);
			return 1;
			}
		else
			{
			send_point--;
			return 0;
			}
		}
	else
		return 0;
}
bit	cancel_jjld(INT8U x)//取消拒绝来电
{
	INT8U	data i;
	i=found_send_buf();
	if(i!=0xff)
		{
		if(wr24c08_single(0xff,ROM_ADDRESS_DENY_CALLIN+x*16+1))
			{
			phoneDenyCallInBuf[x][1]=0xff;
			uartTxCmds[i][0]=0xcf;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			uartTxCmds[i][3]=0xff;
			busy_send_buf(i);
			return 1;
			}
		else
			{
			send_point--;
			return 0;
			}
		}
	else
		return 0;
}


INT8U	set_lwzy(INT8U pdata* y)//设置离位转移
{
	INT8U data i,j,k,n,x;
	INT8U data p[10];
	bit flag,flag1;
	x=*y;
	k=*(y+1);
	i=found_send_buf();
	if(i!=0xff)
		{
		if(k<=7)
			{
			k=*(y+2);
			*(y+2)=*(y+1)-3;
			j=compare_haoma_p(y+2);
			if(j<8&&j!=x)
				{
				p[0]=1;
				p[1]=j;
				if(wr24c08_multi(p,ROM_ADDRESS_LEFT_TRANSFER+x*16,2))
					{
					phoneLeftTransfers[x][0]=1;
					phoneLeftTransfers[x][1]=j;
					uartTxCmds[i][0]=0xb7;
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[i][1]=uartCmdSeq;
					uartTxCmds[i][2]=x;
					uartTxCmds[i][3]=j<<4|0x0f;
					uartTxCmds[i][4]=0xff;
					busy_send_buf(i); 
					return 2;	
					}	
				else
					{
					*(y+2)=k;
					send_point--;
					return 0;
					}
				}
			else
				{
				send_point--;
				return 1;
				}
			}
		else	
			{
			flag1=check_route_when_cfg_transfer(k-3,(INT8U xdata *) y);
			if(flag1)
				{
				p[0]=k-3-ip_prefix[0];
				p[1]=0xbb;		//ip电话标志//
				flag=(bit)(ip_prefix[0]&0x01);
				k=p[0]/2;
				n=ip_prefix[0]/2+3;
				if(flag)
					{	
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n]<<4|y[j+n+1]>>4;					
					if(flag)
						p[j+2]=(y[j+n]<<4)|0x0f;	
					}
				else	
					{
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n];	
					if(flag)
						p[j+2]=y[j+n]|0x0f;	
					}
				}
			uartTxCmds[i][0]=0xb7;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			if(flag1==0)
				{
				p[0]=k-3;
				p[1]=0xaa;
				}
			flag=(bit)((y[1]-3)&0x01);
			k=(y[1]-3)/2;
			for(j=0;j<k;j++)
				{
				if(flag1==0)
					p[j+2]=y[j+3];
				uartTxCmds[i][j+3]=y[j+3];
				}
			if(flag)
				{
				if(flag1==0)
					p[j+2]=y[j+3]|0x0f;
				uartTxCmds[i][j+3]=y[j+3]|0x0f;
				j++;
				}
			uartTxCmds[i][j+3]=0xff;
			k=(p[0]+1)/2+2;
			if(wr24c08_multi(p,ROM_ADDRESS_LEFT_TRANSFER+x*16,k))
				{
				busy_send_buf(i);
				for(j=0;j<k;j++)
					phoneLeftTransfers[x][j]=p[j];
				return 2;
				}
			else
				{
				send_point--;
				return 0;
				}
			}
		}
	else
		return 0;
}
INT8U	set_jjld(INT8U pdata* y)//设置拒绝来电
{
	INT8U data i,j,k,n,x;
	INT8U data p[10];
	bit flag,flag1;
	x=*y;
	k=*(y+1);
	i=found_send_buf();
	if(i!=0xff)
		{
		if(k<=7)
			{
			k=*(y+2);
			*(y+2)=*(y+1)-3;
			j=compare_haoma_p(y+2);
			if(j<8&&j!=x)
				{
				p[0]=1;
				p[1]=j;
				if(wr24c08_multi(p,ROM_ADDRESS_DENY_CALLIN+x*16,2))
					{
					phoneDenyCallInBuf[x][0]=1;
					phoneDenyCallInBuf[x][1]=j;
					uartTxCmds[i][0]=0xce;
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[i][1]=uartCmdSeq;
					uartTxCmds[i][2]=x;
					uartTxCmds[i][3]=j<<4|0x0f;
					uartTxCmds[i][4]=0xff;
					busy_send_buf(i); 
					return 2;	
					}	
				else
					{
					*(y+2)=k;
					send_point--;
					return 0;
					}
				}
			else
				{
				send_point--;
				return 1;
				}
			}
		else	
			{
			flag1=check_route_when_cfg_transfer(k-3,(INT8U xdata *)y);
			if(flag1)
				{
				p[0]=k-3-ip_prefix[0];
				p[1]=0xbb;		//ip电话标志//
				flag=(bit)(ip_prefix[0]&0x01);
				k=p[0]/2;
				n=ip_prefix[0]/2+3;
				if(flag)
					{	
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n]<<4|y[j+n+1]>>4;					
					if(flag)
						p[j+2]=(y[j+n]<<4)|0x0f;	
					}
				else	
					{
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n];	
					if(flag)
						p[j+2]=y[j+n]|0x0f;	
					}
				}
			uartTxCmds[i][0]=0xce;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			if(flag1==0)
				{
				p[0]=k-3;
				p[1]=0xaa;
				}
			flag=(bit)((y[1]-3)&0x01);
			k=(y[1]-3)/2;
			for(j=0;j<k;j++)
				{
				if(flag1==0)
					p[j+2]=y[j+3];
				uartTxCmds[i][j+3]=y[j+3];
				}
			if(flag)
				{
				if(flag1==0)
					p[j+2]=y[j+3]|0x0f;
				uartTxCmds[i][j+3]=y[j+3]|0x0f;
				j++;
				}
			uartTxCmds[i][j+3]=0xff;
			k=(p[0]+1)/2+2;
			if(wr24c08_multi(p,ROM_ADDRESS_DENY_CALLIN+x*16,k))
				{
				busy_send_buf(i);
				for(j=0;j<k;j++)
					phoneDenyCallInBuf[x][j]=p[j];
				return 2;
				}
			else
				{
				send_point--;
				return 0;
				}
			}
		}
	else
		return 0;
}
INT8U	set_phoneNightServices(INT8U pdata* y)//设置夜服电话
{
	INT8U data i,j,k,n,x;
	INT8U data p[10];
	bit flag,flag1;
	x=*y;
	k=*(y+1);
	i=found_send_buf();
	if(i!=0xff)
		{
		if(k<=7)
			{
			k=*(y+2);
			*(y+2)=*(y+1)-3;
			j=compare_haoma_p(y+2);
			if(j<8&&j!=x)
				{
				p[0]=1;
				p[1]=j;
				if(wr24c08_multi(p,ROM_ADDRESS_NIGHT_SERVICE+x*16,2))
					{
					phoneNightServices[x][0]=1;
					phoneNightServices[x][1]=j;
					uartTxCmds[i][0]=0xcb;
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[i][1]=uartCmdSeq;
					uartTxCmds[i][2]=x;
					uartTxCmds[i][3]=j<<4|0x0f;
					uartTxCmds[i][4]=0xff;
					busy_send_buf(i); 
					return 2;	
					}	
				else
					{
					*(y+2)=k;
					send_point--;
					return 0;
					}
				}
			else
				{
				send_point--;
				return 1;
				}
			}
		else	
			{
			flag1=check_route_when_cfg_transfer(k-3,(INT8U xdata *)y);
			if(flag1)
				{
				p[0]=k-3-ip_prefix[0];
				p[1]=0xbb;		//ip电话标志
				flag=(bit)(ip_prefix[0]&0x01);
				k=p[0]/2;
				n=ip_prefix[0]/2+3;
				if(flag)
					{	
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n]<<4|y[j+n+1]>>4;					
					if(flag)
						p[j+2]=(y[j+n]<<4)|0x0f;	
					}
				else	
					{
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n];	
					if(flag)
						p[j+2]=y[j+n]|0x0f;	
					}
				}
			uartTxCmds[i][0]=0xcb;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			if(flag1==0)
				{
				p[0]=k-3;
				p[1]=0xaa;
				}
			flag=(bit)((y[1]-3)&0x01);
			k=(y[1]-3)/2;
			for(j=0;j<k;j++)
				{
				if(flag1==0)
					p[j+2]=y[j+3];
				uartTxCmds[i][j+3]=y[j+3];
				}
			if(flag)
				{
				if(flag1==0)
					p[j+2]=y[j+3]|0x0f;
				uartTxCmds[i][j+3]=y[j+3]|0x0f;
				j++;
				}
			uartTxCmds[i][j+3]=0xff;
			k=(p[0]+1)/2+2;
			if(wr24c08_multi(p,ROM_ADDRESS_NIGHT_SERVICE+x*16,k))
				{
				busy_send_buf(i);
				for(j=0;j<k;j++)
					phoneNightServices[x][j]=p[j];
				return 2;
				}
			else
				{
				send_point--;
				return 0;
				}
			}
		}
	else
		return 0;
}
INT8U	set_phoneUnconditionalTransfers(INT8U pdata* y)//设置无条件转移
{
	INT8U data i,j,k,n,x;
	INT8U data p[10];
	bit flag,flag1;
	x=*y;
	k=*(y+1);
	i=found_send_buf();
	if(i!=0xff)
		{
		if(k<=7)
			{
			k=*(y+2);
			*(y+2)=*(y+1)-3;
			j=compare_haoma_p(y+2);
			if(j<8&&j!=x)
				{
				p[0]=1;
				p[1]=j;
				if(wr24c08_multi(p,ROM_ADDRESS_UNCONDITIONAL_TRANSFER+x*16,2))
					{
					phoneUnconditionalTransfers[x][0]=1;
					phoneUnconditionalTransfers[x][1]=j;
					uartTxCmds[i][0]=0xd7;
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[i][1]=uartCmdSeq;
					uartTxCmds[i][2]=x;
					uartTxCmds[i][3]=j<<4|0x0f;
					uartTxCmds[i][4]=0xff;
					busy_send_buf(i); 
					return 2;	
					}	
				else
					{
					*(y+2)=k;
					send_point--;
					return 0;
					}
				}
			else
				{
				send_point--;
				return 1;
				}
			}
		else	
			{
			flag1=check_route_when_cfg_transfer(k-3,(INT8U xdata *)y);
			if(flag1)
				{
				p[0]=k-3-ip_prefix[0];
				p[1]=0xbb;		//ip电话标志
				flag=(bit)(ip_prefix[0]&0x01);
				k=p[0]/2;
				n=ip_prefix[0]/2+3;
				if(flag)
					{	
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n]<<4|y[j+n+1]>>4;					
					if(flag)
						p[j+2]=(y[j+n]<<4)|0x0f;	
					}
				else	
					{
					flag=(bit)(p[0]&0x01);
					for(j=0;j<k;j++)
						p[j+2]=y[j+n];	
					if(flag)
						p[j+2]=y[j+n]|0x0f;	
					}
				}
			uartTxCmds[i][0]=0xd7;
			if(++uartCmdSeq==0xff)
				uartCmdSeq++;
			uartTxCmds[i][1]=uartCmdSeq;
			uartTxCmds[i][2]=x;
			if(flag1==0)
				{
				p[0]=k-3;
				p[1]=0xaa;
				}
			flag=(bit)((y[1]-3)&0x01);
			k=(y[1]-3)/2;
			for(j=0;j<k;j++)
				{
				if(flag1==0)
					p[j+2]=y[j+3];
				uartTxCmds[i][j+3]=y[j+3];
				}
			if(flag)
				{
				if(flag1==0)
					p[j+2]=y[j+3]|0x0f;
				uartTxCmds[i][j+3]=y[j+3]|0x0f;
				j++;
				}
			uartTxCmds[i][j+3]=0xff;
			k=(p[0]+1)/2+2;
			if(wr24c08_multi(p,ROM_ADDRESS_UNCONDITIONAL_TRANSFER+x*16,k))
				{
				busy_send_buf(i);
				for(j=0;j<k;j++)
					phoneUnconditionalTransfers[x][j]=p[j];
				return 2;
				}
			else
				{
				send_point--;
				return 0;
				}
			}
		}
	else
		return 0;
}


