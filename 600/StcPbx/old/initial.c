#include"initial_h.h"

#pragma	ot(9,speed)


void	zjld_process() //中继来电处理
{
	INT8U	data i,j,h=0x01;
	
	i=track_ldstatus0|~pbxHwStatus.trackFreeStatus;
	
	for(j=8;j<12;j++)
	{
		if((i&h)==0)
		{
			if(status[j][0]==25)
			{
			}
			else if(status[j][0]==26)
			{
				if(zjld_reg[j-8][0]>=track_zj_time)
				{
					if(boolTrunkMode)
					{
						if(isd1420Free)
						{
							if(decodingchannel_allocate(j))
							{
								pbx_line_offhook(j);
								pbx_if_record_start_play(j);
								status[j][0]=27;
								tel_jishi[j]=record_time;
							}
							else
								zjld_reg[j-8][1]=50;
						}
						else
							zjld_reg[j-8][1]=50;
					}
					else
					{
						zzj_proa(j);	
						zjld_reg[j-8][1]=50;
					}
				}
				else
					zjld_reg[j-8][1]=50;
			}
			else
				zjld_reg[j-8][1]=50;
		}
		h=h<<1;
	}
}


//发送复位信号
void	send_reset()
{
	INT8U data i;
	i=found_send_buf();
	if(i==0xFF)
		return;
	
	uartTxCmds[i][0]=0x94;
	uartTxCmds[i][1]=0;
	uartTxCmds[i][2]=0;
	uartTxCmds[i][3]=0;
	uartTxCmds[i][4]=0xff;
	busy_send_buf(i);
}


