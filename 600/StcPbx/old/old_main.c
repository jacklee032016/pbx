#include"main_h.h"
#pragma	ot(9,speed)
//#include <stdlib.h>
//void	send_pbx_cmd();
//void	writetoserai(INT8U *p,INT8U x);
// 标记0728 为与IP通话不断开译码通道所增加 或删去的程序

#define	FEATURE_STAY		0	/*配置留守功能*/

#if		FEATURE_STAY
xdata	INT8U stay_flag=0;
xdata	INT8U stay_object;
#endif

INT8U	forbid_waixian=0;		//repaired by cz 9.1	拨打外线出局后若挂机直接进入挂机状态
bit	flag_paicha=1;
void judge_state();

void	bohao64_process(INT8U j, INT8U m)
{
	INT8U data k,n;
	bit flag_p1,flag_p2;
	flag_p1=1;
	
	k = pbx_find_ext_by_number(&codingChannels[m][1]);
	
	if(k<8)
	{
		if(k==j)
			flag_p1=0;
		else
		{
			if(NO_DISTURB(k)==0)
				flag_p1=0;
			else
			{
				if(hj_prom(j,k)==0)
					flag_p1=0;
			}
		}

		if(flag_p1==0)
		{
			pbx_if_busytone_play(j);
			status[j][0]=65;
		}
	}
	else
	{
		flag_p1=1;
		flag_p2=1;
		n= check_route(codingChannels[m]);

		if(n==0)
		{
			flag_p2=0;
			n=pbx_ip_find_free_device();
			if(n==0xff)
				flag_p1=0;
			else
			{
				k=found_send_buf();
				if(k!=0xff)
				{
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[k][0]= CMD_CALL_PBX_IP_DIALING;//0x91;
					uartTxCmds[k][1]=uartCmdSeq;
					uartTxCmds[k][2]=n;
					uartTxCmds[k][3]=j;
					uartTxCmds[k][4]=0xff;

					peerOfIP[n] = j;
					PBX_COLUMN_BUSY(n);
					busy_send_buf(k);
					status[j][0]=77;
					status[j][1]=n+12;
					status[n+12][0]=103;
					status[n+12][1]=j;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					codingChannels[m][1]=0;

				}
				else
					flag_p1=0;
			}
		}

		if(flag_p2)
		{
			k=codingChannels[m][1];
			if(k==2)
			{
				if((codingChannels[m][2]&0x0f)<10)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					flag_p1=0;
			}
			else if(k==3)
			{
				if(codingChannels[m][3]>>4<10)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					flag_p1=0;
			}
#ifdef DIAL_X
			else if(k>=5)
#else
			else if(k>=4)
#endif
			{
				if(n==0xaa)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					flag_p1=0;
			}
		}

		if(flag_p1==0)
		{
			pbx_if_busytone_play(j);
			status[j][0]=65;
		}
	}
}

void	bohao117_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k,i;
	bit	flag;
	
	if(codingChannels[m][1]==1)
	{/* LENGTH IS 1 */
		STOP_DIAL_TONE(j);
		if((codingChannels[m][2]>>4)==DIAL_DIGIT_ASTREIC && j<PBX_FXS_COUNT)//*键
		{
			if(phoneDialoutModes[j]== EXT_FIRST_IP )//0xfe)
			{
#ifdef DIAL_X
				status[j][0]=01;
#else
				status[j][0]=00;
				codingChannels[m][1]=0;
#endif
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				send_guaji(status[j][1]);
			}
			else
				i=codingChannels[m][2]>>4;
		}
		else
			i=codingChannels[m][2]>>4;
	}
	else
	{
		k=codingChannels[m][1];
		flag=(bit)(k&0x01);
		k=++k/2+1;
		if(flag)
			i=codingChannels[m][k]>>4;
		else
			i=codingChannels[m][k]&0x0f;
	}
	
	k=found_send_buf();
	if(k==0xff)
	{
		hasDecodedNumberFlags=hasDecodedNumberFlags|h;
		decodingchannel_broken(j);
		tel_jishi[j] = DIALING_TIMER_LENGTH;
	}                                                                                                               
	else
	{
		if(++uartCmdSeq==0xff)
			uartCmdSeq++;
		uartTxCmds[k][0]= CMD_CALL_PBX_IP_PHONENUMBER;//0x92;
		uartTxCmds[k][1]= uartCmdSeq;
		uartTxCmds[k][2]= status[j][1]-12;
		if(j<8)
			uartTxCmds[k][3]=j;
		else
			uartTxCmds[k][3]=j+0x78;

		uartTxCmds[k][4] = i;
		uartTxCmds[k][5] = 0xff;
		busy_send_buf(k);
		tel_jishi[j]=bohao_time_ip;
		decodingchannel_resume(j);
	}
}

void	bohao119_process(INT8U j,INT8U m)
{
	INT8U data k;
	k= codingChannels[m][2]&0x0f;

	if(0<k&&k<5)
	{
		if(qunhu_pro(j,k-1))
		{
			decodingchannel_free(j);
			pbx_if_ringback_tone_play(j);
			status[j][0]=118;
		}
		else
			zzj_pro(j);	//拨错号转总机处理
	}
	else
		zzj_pro(j);
}

void	bohao123_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k,n;
	bit	flag_p1,flag_p2;
	k=codingChannels[m][1];
	flag_p1=1;

	if(k==2)
	{
		if((codingChannels[m][2]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
	}
	else if(k<10)
	{
		flag_p2=(bit)(k&0x01);
		n=(k+1)/2+1;
		if(flag_p2)
			n=codingChannels[m][n]>>4;
		else
			n=codingChannels[m][n]&0x0f;

		if(n!=12&&n<10&&k!=9)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else if(n==12)
		{
			if(k>3)
			{
				if(set_ip_prefix(codingChannels[m]))
				{
					decodingchannel_resume(j);
					START_DIAL_TONE(j);
					status[j][0]=47;
					codingChannels[m][1]=0;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				}
				else
				{
					hasDecodedNumberFlags=hasDecodedNumberFlags|h;
					decodingchannel_broken(j);
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				}

			}
			else
				flag_p1=0;
		}
		else
			flag_p1=0;
	}
	else
		flag_p1=0;

	if(flag_p1==0)
	{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
	}
}

void	bohao124_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k,n;
	bit flag_p1,flag_p2;
	k=codingChannels[m][1];
	flag_p1=1;
	if(k==2)
		{
		if((codingChannels[m][2]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
		}
	else if(k==3)
		{
		if(0<(codingChannels[m][3]&0xf0)&&(codingChannels[m][3]&0xf0)<0x70)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
		}
	else if(k==4)
		{
		if((codingChannels[m][3]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
		}
	else if(k<12)
		{
		flag_p2=(bit)(k&0x01);
		n=(k+1)/2+1;
		if(flag_p2)
			n=codingChannels[m][n]>>4;
		else
			n=codingChannels[m][n]&0x0f;
		if(n!=12&&n<10&&k!=11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else if(n==12)
			{
			if(k>5)
				{
				if(pbx_phone_cfg_special_service_number(codingChannels[m]))
					{
					decodingchannel_resume(j);
					START_DIAL_TONE(j);
					status[j][0]=47;
					codingChannels[m][1]=0;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				else
					{
					hasDecodedNumberFlags=hasDecodedNumberFlags|h;
					decodingchannel_broken(j);
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				}
			else
				flag_p1=0;
			}
		else
			flag_p1=0;
		}
	else
		flag_p1=0;
	if(flag_p1==0)
		{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
		}
}
void	bohao125_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k;
	bit flag_p1;
	k=codingChannels[m][1];
	flag_p1=1;
	if(k==2)
		{
		if((codingChannels[m][2]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
		}
	else if(k==3)
		{
		if((codingChannels[m][2]&0xf0)==0x40)
			{
			if(0<(codingChannels[m][3]&0xf0)&&(codingChannels[m][3]&0xf0)<0x70)
				tel_jishi[j]=DIALING_TIMER_LENGTH;
			else
				flag_p1=0;
			}
		else
			{
			if((codingChannels[m][3]&0xf0)<0x20)
				tel_jishi[j]=DIALING_TIMER_LENGTH;
			else
				flag_p1=0;
			}
		}
	else
		{				
		if((codingChannels[m][3]&0x0f)==12)
			{
			if((codingChannels[m][2]&0xf0)==0x40)
				{
				if(pbx_cfg_special_service_number( (codingChannels[m][3]>>4) -1, NULL))//cancel_tefu(codingChannels[m][3]>>4))
					{
					decodingchannel_resume(j);
					START_DIAL_TONE(j);
					status[j][0]=47;
					codingChannels[m][1]=0;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				else
					{
					hasDecodedNumberFlags=hasDecodedNumberFlags|h;
					decodingchannel_broken(j);
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				}
			else
				{
				if(pbx_cfg_special_service_first(codingChannels[m][3]>>4))
					{
					decodingchannel_resume(j);
					START_DIAL_TONE(j);
					status[j][0]=47;
					codingChannels[m][1]=0;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				else
					{
					hasDecodedNumberFlags=hasDecodedNumberFlags|h;
					decodingchannel_broken(j);
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}								
				}
			}
		else
			flag_p1=0;
		}
	if(flag_p1==0)
		{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
		}
}
void	bohao126_process(INT8U j,INT8U m)
{
	INT8U data k,n;
	bit flag_p1,flag_p2;
	flag_p1=1;
	k=pbx_find_ext_by_number(&codingChannels[m][1]);
	if(k<8)
		{
		if(k==j)
			flag_p1=0;
		else
			{
			decodingchannel_free(j);
			if(status[k][0]==10)
				{
				n=status[k][1];
				pbx_if_ringback_tone_stop(n);
				pbx_voice_channel_connect(j,n);
				status[j][0]=07;
				status[j][1]=n;
				status[n][0]=32;
				status[n][1]=j;
				lode_thxs(j,n);							
				}
			else if(status[k][0]==20)
				{
				n=status[k][1];			/*主叫方B*/
				if(status[n][0]==18)
					{
					PBX_ROW_FREE(n);
					status[n][0]=19;
					status[n][2]=0xff;
					n=status[n][1];	/*B的原通话方C*/
					pbx_voice_channel_connect(j,n);
					if(status[n][2]==05||status[n][2]==06)
						{
						status[n][2]=0xff;
						status[n][0]=05;
						status[n][1]=j;
						status[j][0]=06;
						status[j][1]=n;
						}
					else if(status[n][2]==31||status[n][2]==92)
						{
						status[n][0]=status[n][2];
						status[n][2]=0xff;
						status[n][1]=j;
						status[j][0]=8;
						status[j][1]=n;
						}
					else if(status[n][2]==32||status[n][2]==97)
						{
						status[n][0]=status[n][2];
						status[n][2]=0xff;
						status[n][1]=j;
						status[j][0]=07;
						status[j][1]=n;
						lode_thxs(j,n);
						}
					}
				else
					{
					pbx_if_ringback_tone_stop(n);
					pbx_voice_channel_connect(j,n);
					status[n][0]=05;
					if(status[n][1]==k)
						status[n][1]=j;
					else
						status[n][2]=j;
					status[j][0]=06;
					status[j][1]=n;
					}
				}
			else if(status[k][0]==21)
				{
				n=status[k][1];
				pbx_voice_channel_connect(j,n);
				status[j][0]=8;
				status[j][1]=n;
				if(status[n][0]==44)
					{
					pbx_line_offhook(n);
					status[n][0]=31;
					status[n][1]=j;
					}
				else
					{
					pbx_if_ringback_tone_stop(n);
					if(n<12)
						status[n][0]=31;
					else
						status[n][0]=92;
					status[n][1]=j;
					}
				}
			else if(status[k][0]==75)
				{
				pbx_if_ringback_tone_stop(n);
				status[n][0]=67;
				status[n][1]=j;
				status[j][0]=70;
				status[j][1]=n;
				CONNECT(n,15);
				CONNECT(j,15);
				point[j]=(j<<4)|15;
				point[n]=(n<<4)|15;
				}
			else
				flag_p1=0;
			if(flag_p1)
				{
				pbx_if_stop_ringing(k);
				stop_qudian(k);
				PBX_ROW_FREE(k);
				status[k][0]=19;
				}
			}
		}
	#ifdef DIAL_X
	else if(codingChannels[m][1]<5)
	#else
	else if(codingChannels[m][1]<4)
	#endif
		{
		flag_p2=(bit)(codingChannels[m][1]&0x01);
		k=((codingChannels[m][1]+1)>>1)+1;
		if(flag_p2)
			k=codingChannels[m][k]>>4;
		else
			k=codingChannels[m][k]&0x0f;
		if(k<10)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
		}
	else
		flag_p1=0;
	if(flag_p1==0)
		{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
		}
}
void	bohao128_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k;
	bit	flag_p1;
	flag_p1=1;
	k=codingChannels[m][1];
	if(k==2)
		{
		if((codingChannels[m][2]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else 
			flag_p1=0;
		}
	else if(k==3)
		{
		if((codingChannels[m][3]&0xf0)<0x40)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else 
			flag_p1=0;
		}
	else if(k==4)
		{
		if((codingChannels[m][3]&0x0f)<10 && codingChannels[m][3]!=0)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else 
			flag_p1=0;
		}
	else
		{
		if((codingChannels[m][4]&0xf0)==0xc0)
			{
			k=codingChannels[m][3]>>4;
			k=k*10+(codingChannels[m][3]&0x0f);
			if(pbx_cfg_ext_left_transfer_time(j, k) )//set_phoneLeftTranferTimes(j,k))
				{
				decodingchannel_resume(j);
				START_DIAL_TONE(j);
				status[j][0]=00;
				codingChannels[m][1]=0;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				}
			else
				{
				decodingchannel_broken(j);
				hasDecodedNumberFlags=hasDecodedNumberFlags|h;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				}	
			}
		else 
			flag_p1=0;
		}
	if(flag_p1==0)
		{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
		}
}
void	bohao129_process(INT8U j,INT8U m)
{
 	INT8U data k,n;
 	bit flag_p1;
 	flag_p1=1;

 	k=pbx_find_ext_by_number(&codingChannels[m][1]);
	if(k<8)
	{
		decodingchannel_free(j);
		if(k==j)
			flag_p1=0;
		else
		{
			if(NO_DISTURB(k)==0)
				flag_p1=0;
			else
			{
				n=phoneUnconditionalTransfers[k][1];
				if(n>=8)
				{
					if(nowIsNightServiceTime)
						n=phoneNightServices[k][1];
					if(n>=8)
					{
						n=phoneSecretBranchs[k];
						if(n==0xff||n==j)
							n=k;
					}
				}

				if(j!=n)
					flag_p1=hj_prod(j,n);
				else
					flag_p1=0;
			}
		}							
	}
#ifdef DIAL_X
	else if(codingChannels[m][1]<=4)
#else
	else if(codingChannels[m][1]<4)
#endif
		tel_jishi[j]=DIALING_TIMER_LENGTH;
	else
		flag_p1=0;

	if(flag_p1==0)
	{
		decodingchannel_free(j);
		k=status[j][1];
		pbx_if_stop_music(k);
		pbx_voice_channel_connect(j,k);
		resume_talk(status[k],status[j]);
	}
}

void	bohao132_process(INT8U j,INT8U m,INT8U h)
{
	bit flag_p2;

	if((codingChannels[m][2]&0x0f)==12)
	{
		if((codingChannels[m][2]&0xf0)==0x20)
			flag_p2 = pbx_cfg_ext_notdisturb_opened(j, TRUE);// set_not_disturb(j);
		else
			flag_p2 = pbx_cfg_ext_notdisturb_opened(j, FALSE);//set_disturb(j);		

		if(flag_p2)
		{
			decodingchannel_resume(j);
			START_DIAL_TONE(j);
			status[j][0]=00;
			codingChannels[m][1]=0;
			tel_jishi[j] = DIALING_TIMER_LENGTH;
		}
		else
		{
			decodingchannel_broken(j);
			hasDecodedNumberFlags=hasDecodedNumberFlags|h;
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		}	
	}
	else
	{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
	}
}

void 	bohao133_process(INT8U j,INT8U m,INT8U h)
{
	INT8U data k;
	bit flag_p1,flag;
	flag_p1=1;
	k=codingChannels[m][1];

	if(k==2)
	{
		if((codingChannels[m][2]&0x0f)==11)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
	}
	else if(k==3)
	{
		if(0<(codingChannels[m][3]&0xf0)&&(codingChannels[m][3]&0xf0)<0x90)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
	}
	else if(k==4)
	{
		if(0<(codingChannels[m][3]&0x0f)&&(codingChannels[m][3]&0x0f)<0x05)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
	}
	else if(k==5)
	{
		if((codingChannels[m][4]&0xf0)==0||(codingChannels[m][4]&0xf0)==0x10)
			tel_jishi[j]=DIALING_TIMER_LENGTH;
		else
			flag_p1=0;
	}
	else 
	{
		if((codingChannels[m][4]&0x0f)==12)
		{
			flag= pbx_phone_cfg_ext_binding_groups(&codingChannels[m][3]);

			if(flag==0)
			{
				decodingchannel_broken(j);
				hasDecodedNumberFlags=hasDecodedNumberFlags|h;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
			}
			else 
			{
				decodingchannel_resume(j);
				START_DIAL_TONE(j);
				status[j][0]=47;
				codingChannels[m][1]=0;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
			}
		}
		else
			flag_p1=0;
	}
	
	if(flag_p1==0)
	{
		decodingchannel_free(j);
		pbx_if_busytone_play(j);
		status[j][0]=03;
	}	
}


/*void 	bohao134_process(INT8U j,INT8U m)
{
	INT8U data k;
	decodingchannel_free(j);
	k=codingChannels[m][2]&0x0f;
	if(0<k&&k<5)
		{
		if(qunhu_pro(j,k-1))
			{
			pbx_if_ringback_tone_play(j);
			status[j][0]=135;
			}
		else
			{
			pbx_if_busytone_play(j);
			status[j][0]=03;
			}
		}
	else
		{
		pbx_if_busytone_play(j);
		status[j][0]=03;
		}
}*/

/*typedef enum
{//cmd number
	AS_PBX_CMD_TYPE_CALLER_OFFHOOK 				= 0x91, 	//PBX caller offhook 
	AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT 			= 0x92,		// PBX send caller number to IP 
	AS_PBX_CMD_TYPE_CALLER_ONHOOK 				= 0x93, 	//PBX onhook first when in-call 
	AS_PBX_CMD_TYPE_PCM_RESET					= 0x94,	
	AS_PBX_CMD_TYPE_CALLEE_OFFHOOK 				= 0x85,		// PBX offhook as a callee 
	AS_PBX_CMD_TYPE_CALLEE_SEND_PCM_CHANNEL 	= 0x86,
	AS_PBX_CMD_TYPE_CALLEE_BUSY_HERE 			= 0x87
}AS_PBX_CMD_TYPE;

#define  CMD_VALUE_MAX   14

unsigned char opS[] = 
{
	AS_PBX_CMD_TYPE_CALLER_OFFHOOK,
	AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT,
	AS_PBX_CMD_TYPE_CALLER_ONHOOK,
	AS_PBX_CMD_TYPE_PCM_RESET,
	AS_PBX_CMD_TYPE_CALLEE_OFFHOOK,
	AS_PBX_CMD_TYPE_CALLEE_SEND_PCM_CHANNEL,
	AS_PBX_CMD_TYPE_CALLEE_BUSY_HERE

};
struct _ip_pbx_cmd
{
	unsigned char op;
	unsigned char uartCmdSeq;
	unsigned char value[CMD_VALUE_MAX];
};
typedef struct _ip_pbx_cmd PBX_COMMAND;



void send_pbx_cmd( )
{
	PBX_COMMAND   cmd;	
	
	cmd.uartCmdSeq = 0;
	cmd.op = opS[rand()%7];

	cmd.value[0] = rand()%4;
	cmd.value[1] = rand()%4;

	if(cmd.op == AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT)
	{
		cmd.value[2] = rand()%12;
		if(cmd.value[2] == 10)
			cmd.value[2] = 0;
		cmd.value[3] = 0xFF;
	}
	else cmd.value[2] = 0xFF;

	writetoserai((INT8U *)&cmd, 6);
	//send this cmd to searil port
}
void	writetoserai(INT8U *p,INT8U x)
{
	INT8U data i;
	i=found_send_buf();
	if(i==0xff)
		return;
	uartTxCmds[i][0]=*p++;
	uartTxCmds[i][1]=*p++;
	uartTxCmds[i][2]=*p++;
	uartTxCmds[i][3]=*p++;
	uartTxCmds[i][4]=*p++;
	uartTxCmds[i][5]=*p++;
	busy_send_buf(i);
}*/

