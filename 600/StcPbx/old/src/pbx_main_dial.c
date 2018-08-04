/*
* $Id: pbx_main_dial.c,v 1.1 2007/05/11 20:13:23 lizhijie Exp $
*/



void	dial_process()
{
	INT8U	data i,j,k,m,n,h=0x01;
	bit flag_p1,flag_p2;
	i=hasDecodedNumberFlags;
	
	for(m=0;m<8;m++)
	{
		if(!(h&i))
		{
			h=h<<1;
			continue;
		}
		
		hasDecodedNumberFlags=hasDecodedNumberFlags&(~h);
		j=codingChannels[m][0];
		
		switch(status[j][0])
		{
			case 00:
			case 62:
				ext_dialing_state(j);
				break;
				
			case 01:
				#ifdef DIAL_X
				if(codingChannels[m][1]==2&&(codingChannels[m][2]&0x0f)==11)
					{
					is_zhanghaob(j);
					status[j][0]=14;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					codingChannels[m][1]=0;
					break;
					}
				#endif
				flag_p1=1;
				flag_p2=0;
				k=pbx_find_ext_by_number(&codingChannels[m][1]);
				if(k<8)
					{
					INT8U	xdata *data ptr;
					if(k==j)
						{
						decodingchannel_free(j);
						pbx_if_busytone_play(j);
						status[j][0]=03;
						}
					else
						{
						if(NO_DISTURB(k)==0)
							{
							decodingchannel_free(j);
							pbx_if_busytone_play(j);
							status[j][0]=03;
							}
						else
							{
							n=phoneUnconditionalTransfers[k][1];
							ptr=phoneUnconditionalTransfers[k];
							if(n==0xff)
								{
								if(nowIsNightServiceTime)
									{
								n=phoneNightServices[k][1];
								ptr=phoneNightServices[k];
									}
								if(n==0xff)
									{
									n=phoneSecretBranchs[k];
									if(n==0xff||n==j)
									n=k;
									}
								}
							if(n==0xaa||n==0xbb)
								flag_p1=hjzy_process(j,ptr);
							else //if(n!=0xff)
								{
								flag_p1=hj_proa(j,n);
								if(flag_p1==0)
									flag_p1=hj_prob(j,n);
								}
							if(flag_p1==0)
								{
								decodingchannel_free(j);
								pbx_if_busytone_play(j);
								status[j][0]=03;
								}
							else
								decodingchannel_free(j);
							}
						}
					}
				else
					{
					n=check_tefu(codingChannels[m]);
					if(n==0)
						{
						flag_p1=0;
						n=tefu_process(codingChannels[m]);	/*n=0,系统忙，n=1,处理失败,n=2处理成功*/
						if(n==0)
							{
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							decodingchannel_broken(j);
							}
						else if(n==1)
							{
							decodingchannel_free(j);
							pbx_if_busytone_play(j);
							status[j][0]=03;
							}
						else
							decodingchannel_free(j);
						}
					if(flag_p1)
						{
						k=check_route(codingChannels[m]);
						if(k==0)
							{
							flag_p1=0;
							n=pbx_ip_find_free_device();
							if(n==0xff)
								{
								decodingchannel_free(j);
								pbx_if_busytone_play(j);
								status[j][0]=03;
								}
							else
								{
								k=found_send_buf();
								if(k==0xff)
									{
									hasDecodedNumberFlags=hasDecodedNumberFlags|h;
									decodingchannel_broken(j);
									tel_jishi[j]=DIALING_TIMER_LENGTH;
									}
								else
									{
									if(++uartCmdSeq==0xff)
										uartCmdSeq++;
									uartTxCmds[k][0]=0x91;
									uartTxCmds[k][1]=uartCmdSeq;
									uartTxCmds[k][2]=n;
									uartTxCmds[k][3]=j;
									uartTxCmds[k][4]=0xff;
									peerOfIP[n]=j;
									PBX_COLUMN_BUSY(n);
									busy_send_buf(k);
									status[j][0]=77;
									status[j][1]=n+12;
									status[n+12][0]=95;
									status[n+12][1]=j;
									tel_jishi[j]=DIALING_TIMER_LENGTH;
									codingChannels[m][1]=0;
									decodingchannel_resume(j);
									}
								}
							}
						}
					if(flag_p1)
						{
						flag_p2=(n==0xaa)||(k==0xaa);
						k=codingChannels[m][1];
						if(k==3)
							{
							if((codingChannels[m][2]==0x18)&&n==0)
								{
								decodingchannel_free(j);
								status[j][0]=138;
								}
								else
									tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#ifdef DIAL_X
						else if(k>=5)
						#else
						else if(k>=4)
						#endif
							{
							if(flag_p2==0)
								{
								decodingchannel_free(j);
								pbx_if_busytone_play(j);
								status[j][0]=03;
								}
							else
								tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						}
					}
				break;
			case 13:
				flag_p1=0;
				if(codingChannels[m][1]==1)
					{
					k=codingChannels[m][2]>>4;
					if(k==0)
						{
						k=is_had_zhanghao(j);
						if(k==0xff)
							k=phoneAuthorities[j];
						if(k<3)
							{
							k=status[j][1];
							tel_jishi[k]=220;	/*中继计时22秒*/
							flag_p1=1;
							}
						}
					else if((k==11)&&(phoneDialoutModes[j]==0xff))
						{
						k=status[j][1];
						pbx_line_onhook(k);
						pbx_voice_channel_broken(j,k);
						PBX_ROW_FREE(k);
						status[k][0]=25;
						tel_jishi[k]=mask_track_time;
						//START_DIAL_TONE(j);
						#ifdef DIAL_X
						status[j][0]=01;
						#else
						codingChannels[m][1]=0;	/*清号码个数*/
						status[j][0]=00;
						#endif
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					else 
						{
						for(n=0;n<6;n++)
							{
							if(specialServices[n][0]!=0xff&&k==specialServices[n][1]>>4)
								{
								flag_p1=1;
								break;
								}
							}
						flag_p1=flag_p1||k==pstn_prefix;
						if(flag_p1)
							{
							k=status[j][1];
							tel_jishi[k]=220;	/*中继计时22秒*/
							}
						else
							{
							k=is_had_zhanghao(j);
							if(k==0xff)
								k=phoneAuthorities[j];
							if(k<4)
								{
								k=status[j][1];
								tel_jishi[k]=220;	/*中继计时22秒*/
								flag_p1=1;
								}
							}
						}	
					}
				else if(codingChannels[m][1]==2)
					{
					decodingchannel_free(j);
					k=codingChannels[m][2];
					if(k==0)
						{
						k=is_had_zhanghao(j);
						if(k==0xff)
							k=phoneAuthorities[j];
						if(k==1)
							{
							k=status[j][1];
							tel_jishi[k]=220;	/*中继计时22秒*/
							flag_p1=1;
							}
						}
					else
						{
						k=status[j][1];
						tel_jishi[k]=220;	/*中继计时22秒*/
						flag_p1=1;
						}
					}
				else
					{
					decodingchannel_free(j);
					k=status[j][1];
					tel_jishi[k]=220;	/*中继计时22秒*/
					flag_p1=1;
					}
				if(flag_p1==0)			/*错误情况*/
					{
					k=status[j][1];
					pbx_line_onhook(k);
					pbx_voice_channel_broken(j,k);
					PBX_ROW_FREE(k);
					status[k][0]=25;
					tel_jishi[k]=mask_track_time;
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 14:
				k=codingChannels[m][1];
				flag_p1=0;
				if(k==1)
					{
					k=codingChannels[m][2]>>4;
					if(k==0)
						{
						if(j==0)
							{
							codingChannels[m][1]=0;
							status[j][0]=54;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							flag_p1=1;
							}
						}
					else if(k==4)
						{
						codingChannels[m][1]=0;
						status[j][0]=55;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					else if(k==6)
						{
						codingChannels[m][1]=0;
						status[j][0]=57;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					else if(k==8)
						{
						codingChannels[m][1]=0;
						status[j][0]=59;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					else if(k==9)
						{
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					}
				else if(k==2)
					{
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					flag_p1=1;
					}
				else
					{
					k=codingChannels[m][2];
					if(k==0x99)
						{
						k=codingChannels[m][3]&0xf0;
						if(k==0xb0)
							{
							codingChannels[m][1]=0;
							status[j][0]=63;
							START_DIAL_TONE(j);
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							meeting_reg[0]=1;
							meeting_reg[1]=j;
							flag_p1=1;
							}
						}
					else if(k==0x98)
						{
						k=codingChannels[m][3]&0xf0;
						if(k==0xb0)
							{
							status[j][0]=126;
							
							#ifdef DIAL_X
							//使缓冲区有一位号码"*",为了以后调用compare_haoma()方便 
							codingChannels[m][1]=1;//号码个数=1；
							codingChannels[m][2]=0xb0;
							#else
							codingChannels[m][1]=0;
							#endif
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							flag_p1=1;
							}
						}
					}
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 15:
				STOP_DIAL_TONE(j);
				#ifdef DIAL_X
				k=codingChannels[m][2]>>4;
				//if(k==phoneDialoutModes[j])
				if((k==phoneDialoutModes[j]) || ((k==0)&&(phoneDialoutModes>10)))//k==0 防止出局方式为外线时转接到外线出错	
					{
					k=is_had_zhanghao(j);
					if(k==0xff)
						k=phoneAuthorities[j];
					if(k<5)
						{
						k=pbx_line_allocate_channel(j);
						if(k!=0xff)
							{
							status[j][0]=13;
							status[j][2]=k;
							status[k][0]=35;
							status[k][1]=j;
							tel_jishi[j]=100;
							tel_jishi[k]=220;
							codingChannels[m][1]=0;
							}
						else
							{
							decodingchannel_free(j);
							n=status[j][1];
							pbx_if_stop_music(n);
							pbx_voice_channel_connect(j,n);
							resume_talk(status[n],status[j]);
							}
						}
					else
						{
						decodingchannel_free(j);
						n=status[j][1];
						pbx_if_stop_music(n);
						pbx_voice_channel_connect(j,n);
						resume_talk(status[n],status[j]);
						}
					}
				else if(k==11)
					{
					status[j][0]=16;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				else if(k==12)//#
					{
					////使缓冲区有一位号码"*"以便以后调用compare_haoma
					codingChannels[m][2] = 0xb0;
					
					status[j][0]=129;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				/*else if(k<10)
					{
					status[j][0]=16;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}*/
				else 
					{
					decodingchannel_free(j);
					n=status[j][1];
					pbx_if_stop_music(n);
					pbx_voice_channel_connect(j,n);
					resume_talk(status[n],status[j]);
					}
				#else
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
								flag_p1=hj_proc(j,n);
							else 	
								flag_p1=0;
							}
						}
						if(flag_p1==0)
							{
							k=status[j][1];
							pbx_if_stop_music(k);
							pbx_voice_channel_connect(j,k);
							resume_talk(status[k],status[j]);
							}
					}
				else
					{
					k=codingChannels[m][2]>>4;
					if((k==phoneDialoutModes[j])||((k==0)&&(phoneDialoutModes[j]>10)))
						{
						k=is_had_zhanghao(j);
						if(k==0xff)
							k=phoneAuthorities[j];
						if(k<5)
							{
							k=pbx_line_allocate_channel(j);
							if(k!=0xff)
								{
								status[j][0]=13;
								status[j][2]=k;
								status[k][0]=35;
								status[k][1]=j;
								tel_jishi[j]=100;
								tel_jishi[k]=220;
								codingChannels[m][1]=0;
								}
							else
								{
								decodingchannel_free(j);
								n=status[j][1];
								pbx_if_stop_music(n);
								pbx_voice_channel_connect(j,n);
								resume_talk(status[n],status[j]);
								}
							}
						else
							{
							decodingchannel_free(j);
							n=status[j][1];
							pbx_if_stop_music(n);
							pbx_voice_channel_connect(j,n);
							resume_talk(status[n],status[j]);
							}
						}
					else if(k==11)
						{
						codingChannels[m][1]=0;
						status[j][0]=129;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if(k<10)
						{
						status[j][0]=16;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else 
						{
						decodingchannel_free(j);
						n=status[j][1];
						pbx_if_stop_music(n);
						pbx_voice_channel_connect(j,n);
						resume_talk(status[n],status[j]);
						}
					}
				#endif
				break;
			case 16:
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
								flag_p1=hj_proc(j,n);
							else 	
								flag_p1=0;
							}
						}
					}
				#ifdef DIAL_X	
				else  if(codingChannels[m][1]<5)
				#else
				else  if(codingChannels[m][1]<4)
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
				break;
			case 27:
				pbx_if_record_stop_play(j);
				flag_p1=1;
				#ifndef DIAL_X
				k=pbx_find_ext_by_number(&codingChannels[m][1]);
				if(k<8)
					{
					INT8U xdata *data ptr;
					flag_p1=0;
					if(k==j)
						zzj_pro(j);
					else
						{
						if(NO_DISTURB(k)==0)
							{
							decodingchannel_free(j);
							pbx_line_onhook(j);
							PBX_ROW_FREE(j);
							status[j][0]=25;
							tel_jishi[j]=2;
							}
						else
							{
							n=phoneUnconditionalTransfers[k][1];
							ptr=phoneUnconditionalTransfers[k];
							if(n==0xff)
								{
								if(nowIsNightServiceTime)
									{
									n=phoneNightServices[k][1];
									ptr=phoneNightServices[k];
									}
								if(n==0xff)
									{
									n=phoneSecretBranchs[k];
									if(n==0xff||n==j)
									n=k;
									}
								}
							if(n==0xaa||n==0xbb)
								flag_p1=hjzy_process(j,ptr);
							else //if(n!=0xff)
								{
								flag_p1=hj_proa(j,n);
								if(flag_p1==0)
								flag_p1=hj_prob(j,n);
								}
							if(flag_p1==0)
								{
								decodingchannel_free(j);
								pbx_line_onhook(j);
								PBX_ROW_FREE(j);
								status[j][0]=25;
								tel_jishi[j]=2;
								}
							else
								decodingchannel_free(j);
							}
						}
					}
				#endif
				if(flag_p1)
					{
					n=check_route(codingChannels[m]);
					if(n==0)
						{
						n=pbx_ip_find_free_device();
						if(n==0xff)
							{
							decodingchannel_free(j);
							pbx_line_onhook(j);
							PBX_ROW_FREE(j);
							status[j][0]=25;
							tel_jishi[j]=2;
							}
						else	
							{
							k=found_send_buf();
							if(k==0xff)
								{
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
								decodingchannel_broken(j);
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							else
								{
								if(++uartCmdSeq==0xff)
									uartCmdSeq++;
								uartTxCmds[k][0]=0x91; //send offhook signle
								uartTxCmds[k][1]=uartCmdSeq;
								uartTxCmds[k][2]=n;
								uartTxCmds[k][3]=j+0x78;
								uartTxCmds[k][4]=0xff;
								peerOfIP[n]=j+0x78;
								PBX_COLUMN_BUSY(n);
								busy_send_buf(k);
								status[j][0]=117;	//修改中继状态为117
								status[j][1]=n+12;
								status[n+12][0]=95;
								status[n+12][1]=j;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								codingChannels[m][1]=0;
								decodingchannel_resume(j);
								}
							}
						}
					else
						{
						k=(codingChannels[m][2])>>4;
						if(k==0)
							zzj_pro_zero(j);
						#ifndef DIAL_X
						else if(k==11&&boolLine2Line)
							{
							codingChannels[m][1]=0;
							status[j][0]=30;
							clr_ldbuf(j);	/*清来电缓冲区*/
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#else
						else if(k==11)
							{
							status[j][0]=28;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#endif
						else if(k==12)
							{
							status[j][0]=119;
							clr_ldbuf(j);	/*清来电缓冲区*/
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#ifndef DIAL_X
						else if(k<10)
							{
							status[j][0]=28;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#endif
						else
							zzj_pro(j);
						}
					}
				break;
			case 28:
				k=pbx_find_ext_by_number(&codingChannels[m][1]);
				if(k<8)
					{
					INT8U	xdata *data ptr;
					if(k==j)
						zzj_pro(j);
					else
						{
						if(NO_DISTURB(k)==0)
							{
							decodingchannel_free(j);
							pbx_line_onhook(j);
							PBX_ROW_FREE(j);
							status[j][0]=25;
							tel_jishi[j]=2;
							}
						else
							{
							n=phoneUnconditionalTransfers[k][1];
							ptr=phoneUnconditionalTransfers[k];
							if(n==0xff)
								{
								if(nowIsNightServiceTime)
									{
									n=phoneNightServices[k][1];
									ptr=phoneNightServices[k];
									}
								if(n==0xff)
									{
									n=phoneSecretBranchs[k];
									if(n==0xff||n==j)
									n=k;
									}
								}
							if(n==0xaa||n==0xbb)
								flag_p1=hjzy_process(j,ptr);
							else //if(n!=0xff)
								{
								flag_p1=hj_proa(j,n);
								if(flag_p1==0)
									flag_p1=hj_prob(j,n);
								}
							if(flag_p1==0)
								{
								decodingchannel_free(j);
								pbx_line_onhook(j);
								PBX_ROW_FREE(j);
								status[j][0]=25;
								tel_jishi[j]=2;
								}
							else
								decodingchannel_free(j);
							}
						}
					}
					
				else
					{			
					flag_p2=0;
					n=check_route(codingChannels[m]);
					if(n==0)
						{
						n=pbx_ip_find_free_device();
						if(n==0xff)
							{
							decodingchannel_free(j);
							pbx_line_onhook(j);
							PBX_ROW_FREE(j);
							status[j][0]=25;
							tel_jishi[j]=2;
							}
						else
							{
							k=found_send_buf();
							if(k==0xff)
								{
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
								decodingchannel_broken(j);
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							else
								{
								if(++uartCmdSeq==0xff)
									uartCmdSeq++;
								uartTxCmds[k][0]=0x91; //send offhook signle
								uartTxCmds[k][1]=uartCmdSeq;
								uartTxCmds[k][2]=n;
								uartTxCmds[k][3]=j+0x78;
								uartTxCmds[k][4]=0xff;
								peerOfIP[n]=j+0x78;
								PBX_COLUMN_BUSY(n);
								busy_send_buf(k);
								status[j][0]=117;	//修改中继状态为117
								status[j][1]=n+12;
								status[n+12][0]=95;
								status[n+12][1]=j;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								codingChannels[m][1]=0;
								decodingchannel_resume(j);
								}
							}
						}
					else
						{
						k=codingChannels[m][1];
						flag_p2=n==0xaa;
						if(k==1)
							{
							k=codingChannels[m][2]>>4;
							if(k==0)
								zzj_pro_zero(j);
							#ifndef DIAL_X
							else if(k==11)
								{
								codingChannels[m][1]=0;
								clr_ldbuf(j);
								status[j][0]=30;
								}
							#endif
							else if(k==12)
								{
								clr_ldbuf(j);
								status[j][0]=119;
								}
							else	
								tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						#ifdef DIAL_X
						else if(k == 2 && (codingChannels[m][2] == 0xbb) )
							{
							codingChannels[m][1]=0;
							clr_ldbuf(j);
							status[j][0]=30;
							}
						else if(k>=5)
						#else
						else if(k>=4)
						#endif
							{
							if(flag_p2)
								tel_jishi[j]=DIALING_TIMER_LENGTH;
							else
								zzj_pro(j);
							}
						else
							tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					}
				break;
			case 30:
				if(codingChannels[m][1]>=5)
					{
					k=codingChannels[m][4]&0xf0;	/*#键*/
					flag_p1=1;
					if(k==0xc0)
						{
						INT8U data ptr[2];
						if(rd24c08_multi(ptr,ROM_ADDRESS_LINE2LINE_PASSWD,2))
							{
							if(ptr[0]==codingChannels[m][2]&&ptr[1]==codingChannels[m][3])
								{
								k=pbx_line_allocate_channel(j);
								if(k!=0xff)
									{
									decodingchannel_free(j);
									status[j][0]=31;
									status[j][1]=k;
									status[k][0]=32;
									status[k][1]=j;
									}
								else 
									{
									decodingchannel_free(j);
									pbx_line_onhook(j);
									PBX_ROW_FREE(j);
									status[j][0]=25;
									tel_jishi[j]=2;
									}
								}
							else
								{
								decodingchannel_free(j);
								pbx_line_onhook(j);
								PBX_ROW_FREE(j);
								status[j][0]=25;
								tel_jishi[j]=2;
								}
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
						pbx_line_onhook(j);
						PBX_ROW_FREE(j);
						status[j][0]=25;
						tel_jishi[j]=2;
						}
					}
				else
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				break;
			case 46:
				k=codingChannels[m][1];
				flag_p1=0;
				if(k==1)
					flag_p1=(codingChannels[m][2]&0xf0)==0xb0;
				else if(k==2)
					flag_p1=(codingChannels[m][2]&0x0f)<10;
				else if(k==3)
					flag_p1=codingChannels[m][3]>>4<10;
				else if(k==4)
					flag_p1=(codingChannels[m][3]&0x0f)<10;
				else if(k==5)
					flag_p1=codingChannels[m][4]>>4<10;
				else
					flag_p1=(codingChannels[m][4]&0x0f)==12;
				if(flag_p1)
					{
					if(k<=5)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else	
						{
						INT8U data ptr[2];
						if(rd24c08_multi(ptr,ROM_ADDRESS_SYS_PASSWD,2))
							{
							k=(codingChannels[m][2]<<4)|(codingChannels[m][3]>>4);
							n=(codingChannels[m][3]<<4)|(codingChannels[m][4]>>4);
							if(ptr[0]==k&&ptr[1]==n)
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								status[j][0]=47;
								codingChannels[m][1]=0;
								}
							else
								{
								decodingchannel_free(j);
								pbx_if_busytone_play(j);
								status[j][0]=03;
								}
							}
						else
							{
							decodingchannel_broken(j);
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						}
					}
				else
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 47:
				k=codingChannels[m][1];
				flag_p1=0;
				if(k==1)
					{
					STOP_DIAL_TONE(j);
					if((codingChannels[m][2]&0xf0)==0xb0)
						{
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					}
				else if(k==2)
					{
					k=codingChannels[m][2]&0x0f;
					n=0xff;
					if(k==1)
						n=48;
					else if(k==2)
						n=49;
					else if(k==3)
						n=51;
					else if(k==5)
						n=52;
					else if(k==7)
						n=53;
					else if(k==8)
						flag_p1=1;
					if(n!=0xff)
						{
						status[j][0]=n;
						codingChannels[m][1]=0;
						flag_p1=1;
						}
					}
				else if(k==3)
					{
					if((codingChannels[m][3]&0xf0)==0x80)
						{
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					}
				else if(k==4)
					{
					if((codingChannels[m][3]&0x0f)==0x0b)
						{
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					}
				else if(k<9)
					{
					flag_p2=(bit)(k&0x01);
					n=++k/2+1;
					k=codingChannels[m][n];
					if(flag_p2)
						k=k>>4;
					else
						k=k&0x0f;
					if(k<10)
						{
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						flag_p1=1;
						}
					}
				else
					{
					if((codingChannels[m][6]&0xf0)==0xc0)
						{
						if(pbx_cfg_sys_passwd(&codingChannels[m][4]) )	/*设置系统密码*/
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=47;
							codingChannels[m][1]=0;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else
							{
							decodingchannel_broken(j);
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						flag_p1=1;
						}
					}
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 48:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if(codingChannels[m][2]>>4>0&&codingChannels[m][2]>>4<7)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else	
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else 
						flag_p1=0;
					}
				else if(k==3)
					{
					if(codingChannels[m][3]>>4<2)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else	
						flag_p1=0;
					}
				else
					{
					if((codingChannels[m][3]&0x0f)==12)
						{
						if((codingChannels[m][2]&0xf0)==0x10)
							flag_p2=pbx_cfg_trunck_direct_dialing(codingChannels[m][3]>>4);
						else if((codingChannels[m][2]&0xf0)==0x20)
							flag_p2=pbx_cfg_no_dial_action_is_auto(codingChannels[m][3]>>4);
						else if((codingChannels[m][2]&0xf0)==0x30)
							flag_p2=pbx_cfg_forbid_line2line(codingChannels[m][3]>>4);
						else if((codingChannels[m][2]&0xf0)==0x40)
							flag_p2=pbx_cfg_differ_ringing(codingChannels[m][3]>>4);
						else if((codingChannels[m][2]&0xf0)==0x50)
							flag_p2=pbx_cfg_callId_mode_fsk(codingChannels[m][3]>>4);
						else
							flag_p2=pbx_cfg_group_call_opened(codingChannels[m][3]>>4);
						if(flag_p2==0)
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
				break;
			case 49:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					k=codingChannels[m][2]>>4;
					if( (1<k&&k<=4) || k==8 )
						{
						status[j][0]=50;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if(k<8)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else	
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==3)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					{
					if((codingChannels[m][3]&0x0f)==12)
						{
						k=codingChannels[m][2]>>4;
						n=codingChannels[m][3]>>4;
						if(k==0)
							{
							if(n>0&&n<5)
								flag_p2=pbx_cfg_line_open(n-1, FALSE);//close_track(n);
							else
								flag_p1=0;
							}
						else if(k==1)
							{
							if(n>0&&n<5)
								flag_p2=pbx_cfg_line_open(n-1, TRUE);//open_track(n);
							else
								flag_p1=0;
							}
						else if(k==5)
							{
							if(n<10)
								flag_p2 = pbx_cfg_global_pstn_prefix(n);//set_bendi_head(n);
							else
								flag_p1=0;
							}
						else if(k==6)
						{
							if(0<n&&n<9)
								flag_p2=pbx_cfg_ext_callId_status(n-1, TRUE);//open_ldxs(n);
							else
								flag_p1=0;
						}
						else 
						{
							if(0<n&&n<9)
								flag_p2=pbx_cfg_ext_callId_status(n-1, FALSE);//close_ldxs(n);
							else
								flag_p1=0;
						}
						
						if(flag_p1)
							{
							if(flag_p2)
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=47;
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
				break;
			case 50:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<5)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					{
					if((codingChannels[m][4]&0xf0)==0xc0)
						{
						k=codingChannels[m][2]>>4;
						n=codingChannels[m][3]&0x0f;
						if(k==2)
							{
							k=codingChannels[m][3]>>4;
							if((0<k&&k<5)&&(0<n&&n<9))
								flag_p2=pbx_cfg_line_virtual_console(k-1, n-1);//set_xnzj(k,n);
							else
								flag_p1=0;
							}
						else if(k==3)
							{
							k=codingChannels[m][3]>>4;
							if((0<k&&k<3)&&(n<6))
								flag_p2 = pbx_cfg_global_account_authority_level(k-1,n);
							else
								flag_p1=0;
							}
						else if(k==4)
							{
							k=codingChannels[m][3]>>4;
							if((0<k&&k<9)&&(0<n&&n<6))
								flag_p2 = pbx_cfg_ext_authority_level(k-1, n);//set_phoneAuthorities(k,n);
							else
								flag_p1=0;
							}
						else
							{
							k=codingChannels[m][3]>>4;
							if((0<k&&k<5)&&(n<5))
								flag_p2=pbx_cfg_line_binding_group(k-1, n);//set_track_telgroup(k,n);
							else
								flag_p1=0;
							}
						if(flag_p1)
							{
							if(flag_p2)
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=47;
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
				break;
			case 51:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if((0<(codingChannels[m][2]&0xf0)&&(codingChannels[m][2]&0xf0)<0x90))
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
							tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==3)
					{
					if((codingChannels[m][3]&0xf0)<0xa0)
							tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==4)
					{
					if((codingChannels[m][3]&0x0f)<0x0a)
							tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else
					{
					if((codingChannels[m][4]&0xf0)==0xc0)
						{
						k=codingChannels[m][2]>>4;
						n=(codingChannels[m][3]>>4)*10+(codingChannels[m][3]&0x0f);
						
						if(pbx_cfg_ext_call_length(k-1, n ) ) //set_thxs(k,n))
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=47;
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
				break;
			case 52:
				flag_p1=1;
				if(codingChannels[m][1]==1)
					{
					k=codingChannels[m][2]>>4;
					if(k>8)
						flag_p1=0;
					else if(k==2)
						status[j][0]=123;
					else if(k==3)
						status[j][0]=124;
					else if(k==4||k==5)
						status[j][0]=125;
					else if(k==6)
						status[j][0]=133;
					else if(k==7)
						status[j][0]=141;
					else if(k==8)
						status[j][0]=142;	//设置夜服时间
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}
				else 
					{
					if((codingChannels[m][2]&0x0f)==12)
						{
						if(isd1420Free)
							{
							if((codingChannels[m][2]&0xf0)==0x00)
								{
								if(j!=0)
									flag_p1=0;
								else
									{
									START_RECORD();
									isd1420Free=0;
									decodingchannel_free(j);
									status[j][0]=60;
									tel_jishi[j]=record_time;
									}					
								}
							else
								{
								pbx_if_record_start_play(j);
								decodingchannel_free(j);
								status[j][0]=85;
								tel_jishi[j]=record_time;
								}
							}
						else
							flag_p1=0;
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
				break;
			case 53:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if((codingChannels[m][2]&0xf0)<0x90&&(codingChannels[m][2]&0xf0)!=0)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<8)
					{
					flag_p2=(bit)(k&0x01);
					n=++k/2+1;
					if(flag_p2)
						k=codingChannels[m][n]>>4;
					else
						k=codingChannels[m][n]&0x0f;
					if(k<10&&codingChannels[m][1]!=7)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else if(k==12&&codingChannels[m][1]>3)
						{
						k=codingChannels[m][2]>>4;
						n=pbx_phone_cfg_ext_phone_number(k -1,&codingChannels[m][1]);
						if(n==2)
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=47;
							codingChannels[m][1]=0;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else if(n==0)
							{
							decodingchannel_broken(j);
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
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
				break;
			case 54:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if((codingChannels[m][2]&0xf0)==0)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<6)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					{
					if(codingChannels[m][3]==0x12&&codingChannels[m][4]==0x3c)
						{
						reset_chuchang();	//恢复出厂设置
						rom_to_ram();
						k=found_send_buf();	
						if(k==0xff)
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
						else
							{
							START_DIAL_TONE(j);
							codingChannels[m][1]=0;	//号码个数复位
							status[j][0]=0;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							uartTxCmds[k][0]=0xbd;	//传送复位命令
							if(++uartCmdSeq==0xff)
								uartCmdSeq++;
							uartTxCmds[k][1]=uartCmdSeq;
							uartTxCmds[k][2]=0xff;
							busy_send_buf(k);
							}
						}
					else if(codingChannels[m][3]==0x11&&codingChannels[m][4]==0x9c)//repaired by cz 8 31
						initial();		//repaired by cz 8 31
					else
						 flag_p1=0;
					}
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 55:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					k=codingChannels[m][2]>>4;
					if(k==0||k==2||k==4||k==6||k==8)
						{
						status[j][0]=56;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else 
					{
					if((codingChannels[m][2]&0x0f)==12)
						{
						k=codingChannels[m][2]>>4;
						if(k==1)
							flag_p2= pbx_cfg_ext_busy_ext(j, EXT_TRANSDER_EXT_IS_NULL);
						else if(k==3)
							flag_p2 = pbx_cfg_ext_secret_ext(j, EXT_TRANSDER_EXT_IS_NULL);//cancel_msfj(j);
						else if(k==5)
							flag_p2= pbx_cfg_ext_transfer_left(j, 0, NULL);//cancel_lwzy(j);
						else if(k==7)
							flag_p2= pbx_cfg_ext_transfer_neight_service(j, 0, NULL);//cancel_phoneNightServices(j);
						else
							flag_p2= pbx_cfg_ext_transfer_deny_callin(j, 0, NULL);//cancel_jjld(j);
						if(flag_p2)
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
				break;
			case 56:
				flag_p1=1;
				if(codingChannels[m][1]==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(codingChannels[m][1]==3)
					{
					if((codingChannels[m][3]>>4)<10)	//
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(codingChannels[m][1]<=7)
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=(k+1)/2+1;
					k=flag_p2?codingChannels[m][k]>>4:codingChannels[m][k]&0x0f;
					if(k==12)
						{
						k=codingChannels[m][2]>>4;
						if(k==0)
							n=pbx_phone_cfg_ext_busy_ext(j,&codingChannels[m][1]); /*n=0,系统忙，n=1,处理失败,n=2处理成功*/
						else if(k==2)
							n=pbx_phone_cfg_ext_secret_ext(j,&codingChannels[m][1]);
						else if(k==4)
							n= pbx_phone_cfg_transfer_left(codingChannels[m]);//set_lwzy(codingChannels[m]);
						else if(k==6)
							n=pbx_phone_cfg_transfer_night_services(codingChannels[m]);//set_phoneNightServices(codingChannels[m]);
						else
							n= pbx_phone_cfg_transfer_deny_callin(codingChannels[m]);//set_jjld(codingChannels[m]);
						if(n==0)
							{
							decodingchannel_broken(j);
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else if(n==2)
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=00;
							codingChannels[m][1]=0;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else
							flag_p1=0;
						}
					else if(k<10)
						{
						k=codingChannels[m][2]>>4;
						if((k==0||k==2)&&codingChannels[m][1]==7)
							flag_p1=0;
						else
							tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else
						flag_p1=0;
					}
				else if(codingChannels[m][1]<20)
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=++k/2+1;
					if(flag_p2)
						k=codingChannels[m][k]>>4;
					else
						k=codingChannels[m][k]&0x0f;
					if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else 
						{
						if(k==12)
							{
							k=codingChannels[m][2]>>4;
							if(k==4)
								n=set_lwzy(codingChannels[m]);	
							else if(k==6)
								n= pbx_phone_cfg_transfer_night_services(codingChannels[m]);//set_phoneNightServices(codingChannels[m]);
							else	
								n=set_jjld(codingChannels[m]);
							if(n==0)
								{
								decodingchannel_broken(j);
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}	
							else
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=00;
								codingChannels[m][1]=0;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							}
						else
							flag_p1=0;
						}
					}
				else
					flag_p1=0;
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 57:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					k=codingChannels[m][2]>>4;
					if(k==0)
						{
						codingChannels[m][1]=0;
						status[j][0]=58;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if(k==2||k==3)
						{
						status[j][0]=132;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if(k==1)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==7)
					{
					if(codingChannels[m][5]>>4==12)
						{
						INT8U	data ptr[2];
						if(rd24c08_multi(ptr,ROM_ADDRESS_LINE2LINE_PASSWD,2))
							{
							if(codingChannels[m][3]==ptr[0]&&codingChannels[m][4]==ptr[1])
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							else
								flag_p1=0;
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
				else if(k==8)
					{
					STOP_DIAL_TONE(j);
					if((codingChannels[m][5]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==9)
					{
					if(codingChannels[m][6]>>4==6)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==10)
					{
					if((codingChannels[m][6]&0x0f)==1)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==11)
					{
					if(codingChannels[m][7]>>4==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==16)
					{
					if((codingChannels[m][9]&0x0f)==12)
						{
						INT8U data ptr[2];
						ptr[0]=(codingChannels[m][7]<<4)|(codingChannels[m][8]>>4);
						ptr[1]=(codingChannels[m][8]<<4)|(codingChannels[m][9]>>4);
						if(wr24c08_multi(ptr,ROM_ADDRESS_LINE2LINE_PASSWD,2))
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							codingChannels[m][1]=0;
							status[j][0]=00;
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
				else
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=++k/2+1;
					if(flag_p2)
						k=codingChannels[m][k]>>4;
					else
						k=codingChannels[m][k]&0x0f;
					if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else 
						flag_p1=0;				
					}
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;	
			case 58:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if((codingChannels[m][2]&0xf0)==0xb0)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					k=codingChannels[m][2]&0x0f;
					if(k==1||k==2)
						{
						flag_p2=(accounts[--k]&0x80)!=0;//最高位为1帐号未使用
						flag_p2|=((accounts[k]&0x07)==j);// 使用方为己方
						flag_p2&=((accounts[k]&0x70)!=0);//高半字节其余位为0，帐号注销
						if(flag_p2)
							tel_jishi[j]=DIALING_TIMER_LENGTH;
						else
							flag_p1=0;	/*帐号在使用*/
						}
					else
						flag_p1=0;
					}
				else if(k==7)
					{
					if((codingChannels[m][5]&0xf0)==0xc0)
						{
						INT8U data ptr[2];
						k=codingChannels[m][2]&0x0f;
						if(rd24c08_multi(ptr,--k*3+1+ROM_ADDRESS_ACCOUNT,2))
							{
							if(codingChannels[m][3]==ptr[0]&&codingChannels[m][4]==ptr[1])
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=61;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								codingChannels[m][1]=0;
								k=(codingChannels[m][2]&0x0f)-1;
								accounts[k]=accounts[k]&0x70|j;	/*做登陆标记*/
								}
							else
								flag_p1=0;
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
				else
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=++k/2+1;
					if(flag_p2)
						k=codingChannels[m][k]>>4;
					else
						k=codingChannels[m][k]&0x0f;
					if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else 
						flag_p1=0;	
					}
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;	
			case 59:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					if((codingChannels[m][2]&0xf0)==0x80)
						{
						codingChannels[m][1]=0;
						status[j][0]=46;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if((codingChannels[m][2]&0xf0)==0x50)
						{
						status[j][0]=127;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if((codingChannels[m][2]&0xf0)==0x70)
						{
						status[j][0]=128;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else if((codingChannels[m][2]&0xf0)<0x90)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==12)
						{
						//if((codingChannels[m][2]&0xf0)==0)
							//flag_p2=set_wxfirst(j);
						if((codingChannels[m][2]&0xf0)==0x30)
							flag_p2=pbx_cfg_ext_callwait_open(j, TRUE);//open_callwait(j);
						else if((codingChannels[m][2]&0xf0)==0x40)
							flag_p2=pbx_cfg_ext_callwait_open(j, FALSE);//close_callwait(j);
						else if((codingChannels[m][2]&0xf0)==0x60)
							flag_p2= pbx_cfg_ext_transfer_unconditional(j, 0, NULL);//cancel_phoneUnconditionalTransfers(j);
						else
							flag_p1=0;
						if(flag_p1)
							{
							if(flag_p2)
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=00;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								codingChannels[m][1]=0;
								}
							else
								{
								decodingchannel_broken(j);
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							}
						}
					else if((codingChannels[m][2]&0x0f)==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==3)
					{
					if((codingChannels[m][3]&0xf0)<0xa0)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else
					{
					if((codingChannels[m][3]&0x0f)==12)
						{
						k=codingChannels[m][2]>>4;
						if(k==0&&(codingChannels[m][3]>>4)<2)
							flag_p2= pbx_cfg_ext_dialout_mode(j, EXT_FIRST_PSTN-(codingChannels[m][3]>>4) );//set_wx_ip_first(j,codingChannels[m][3]>>4);
						else if(k==1)
							flag_p2= pbx_cfg_ext_dialout_mode(j, codingChannels[m][3]>>4);//set_bxcj(j,codingChannels[m][3]>>4);
						else if(k==2)
							flag_p2= pbx_cfg_ext_delay_to_lines_time(j, codingChannels[m][3]>>4);//set_delay_out(j,codingChannels[m][3]>>4);
						else
							flag_p1=0;
						if(flag_p1)
							{
							if(flag_p2)
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=00;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								codingChannels[m][1]=0;
								}
							else
								{
								decodingchannel_broken(j);
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
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
				break;	
			case 61:
				k=codingChannels[m][1];
				flag_p1=1;
				if(k==1)
					{
					STOP_DIAL_TONE(j);
					k=codingChannels[m][2]>>4;
					if( (k==phoneDialoutModes[j]) || ((k==0)&&(phoneDialoutModes[j]>10) ) )
						{
						k=is_had_zhanghao(j);
						if(k==0xff)
						k=phoneAuthorities[j];
						if(k<5)
							{
							k=pbx_line_allocate_channel(j);
							if(k!=0xff)
								{
								status[j][0]=13;
								status[j][1]=k;
								status[k][0]=35;
								status[k][1]=j;
								tel_jishi[j]=100;
								tel_jishi[k]=220;
								codingChannels[m][1]=0;
								}
							else
								flag_p1=0;
							}
						else
							flag_p1=0;
						}
					else if(k==11)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else if(k<10)
						{
						status[j][0]=01;
						tel_jishi[j]=DIALING_TIMER_LENGTH;
						}
					else
						flag_p1=0;
					}
				else if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==6)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k==3)
					{
					if((codingChannels[m][3]&0xf0)==0)
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
				else if(k==5)
					{
					k=codingChannels[m][4]>>4;
					if(k==found_tel_zhanghao(j))
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<10)
					{
					flag_p2=(bit)(k&0x01);
					k=++k/2+1;
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
					{
					if((codingChannels[m][6]&0x0f)==12)
						{
						INT8U	data	ptr[2];
						ptr[0]=(codingChannels[m][4]<<4)|(codingChannels[m][5]>>4);
						ptr[1]=(codingChannels[m][5]<<4)|(codingChannels[m][6]>>4);
						k=codingChannels[m][4]>>4;
						if(wr24c08_multi(ptr,--k*3+1+ROM_ADDRESS_ACCOUNT,2))
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=61;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							codingChannels[m][1]=0;
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
				break;	
			case 63:
				flag_p1=1;
				flag_p2=1;
				STOP_DIAL_TONE(j);
				#ifndef DIAL_X
				k=pbx_find_ext_by_number(&codingChannels[m][1]);
				if(k<8)
					{
					flag_p2=0;
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
				#endif
				if(flag_p2)
					{
					n=check_route(codingChannels[m]);
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
								uartTxCmds[k][0]=0x91;
								uartTxCmds[k][1]=uartCmdSeq;
								uartTxCmds[k][2]=n;
								uartTxCmds[k][3]=j;
								uartTxCmds[k][4]=0xff;
								peerOfIP[n]=j;
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
						k=codingChannels[m][2]>>4;
						if((k==phoneDialoutModes[j])||((k==0)&&(phoneDialoutModes>10)))
							{
							k=is_had_zhanghao(j);
							if(k==0xff)
								k=phoneAuthorities[j];
							if(k<5)
								{
								k=pbx_line_allocate_channel(j);
								if(k!=0xff)
									{
									status[j][0]=73;
									status[j][1]=k;
									status[k][0]=39;
									status[k][1]=j;
									tel_jishi[j]=100;
									tel_jishi[k]=220;
									codingChannels[m][1]=0;
									}
								else
									flag_p1=0;
								}
							else
								flag_p1=0;
							}	
						else if(k==12)
							{
							decodingchannel_free(j);
							if(meeting_reg[0]<2)
								{
								meeting_reg[0]=0xff;
								meeting_reg[1]=0xff;
								pbx_if_busytone_play(j);
								status[j][0]=03;
								}
							else
								setup_meeting();
							}
						else 
							{
							status[j][0]=64;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						}
					if(flag_p1==0)
						{
						//decodingchannel_free(j);
						pbx_if_busytone_play(j);
						status[j][0]=65;
						}
					}
				break;	
			case 64:
				bohao64_process(j,m);
				break;
			case 65:
			case 66:
			case 67:
			case 68:
				codingChannels[m][1]=0;
				break;
			case 07:	//0728
			case 31:	//0728
			case 77:
			case 117:
				bohao117_process(j,m,h);
				break;
			case 113:		//PCM channel check
				n=found_send_buf();
				if(n!=0xff&&buzzer_status==0)
					{
					k=codingChannels[m][1];
					if(k==10)
						{
						status[j][0]=86;
						if(j==19)
							{
							uartTxCmds[n][0]=0x98;
							if(++uartCmdSeq==0xff)
								uartCmdSeq++;
							uartTxCmds[n][1]=uartCmdSeq;
							uartTxCmds[n][2]=0x55;//自检结束
							uartTxCmds[n][3]=codingChannels[m][6]&0x0f;
							uartTxCmds[n][4]=0xff;
							busy_send_buf(n);
							decodingchannel_free(19);
							columnFrees=0xff;
							}
						else
							{
							uartTxCmds[n][0]=0x98;
							if(++uartCmdSeq==0xff)
								uartCmdSeq++;
							uartTxCmds[n][1]=uartCmdSeq;
							uartTxCmds[n][2]=++j-12;
							uartTxCmds[n][3]=codingChannels[m][6]&0x0f;
							uartTxCmds[n][4]=0xff;
							busy_send_buf(n);
							status[j][0]=113;	//check	next channel
							DISCONNECT(14,j-13);
							CONNECT(14,j-12);
							codingChannels[m][0]=j;
							codingChannels[m][1]=0;
							tel_jishi[j]=100;;
							}
						}
					else
						{
						flag_p2=(bit)(k&0x01);
						k=++k/2+1;
						if(flag_p2)
							k=codingChannels[m][k]>>4;
						else
							k=codingChannels[m][k]&0x0f;
						uartTxCmds[n][0]=0x98;
						if(++uartCmdSeq==0xff)
							uartCmdSeq++;
						uartTxCmds[n][1]=uartCmdSeq;
						uartTxCmds[n][2]=j-12;
						uartTxCmds[n][3]=k;
						uartTxCmds[n][4]=0xff;
						busy_send_buf(n);
						tel_jishi[j]=100;
						}
					BUZZER_ON();
					buzzer_status=1;	//蜂鸣器状态
					buzzer_reg=0;	//控制蜂鸣器时间间隔
					buzzing=1;	//蜂鸣器响停标志
					buzzer_jishi=2;//响2*50ms
					}
				else
					{
					if(n!=0xff)
						send_point--;
					hasDecodedNumberFlags=hasDecodedNumberFlags|h;
					tel_jishi[j]=100;
					}
				break;
			 case 119:
				bohao119_process(j,m);
				break;
			case 123:
				bohao123_process(j,m,h);
				break;
			case 124:
				bohao124_process(j,m,h);
				break;
			case 125:
				bohao125_process(j,m,h);
				break;
			case 126:
				bohao126_process(j,m);
				break;
			case 127:
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
					if((codingChannels[m][3]>>4)<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<=7)
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=(k+1)/2+1;
					k=flag_p2?codingChannels[m][k]>>4:codingChannels[m][k]&0x0f;
					if(k==12)
					{
						n= pbx_phone_cfg_transfer_unconditional(codingChannels[m]);//set_phoneUnconditionalTransfers(codingChannels[m]);	
						if(n==0)
							{
							decodingchannel_broken(j);
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else if(n==2)
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=00;
							codingChannels[m][1]=0;
							tel_jishi[j]=DIALING_TIMER_LENGTH;
							}
						else
							flag_p1=0;

						}
					else if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else
						flag_p1=0;
					}
				else if(k<20)
					{
					k=codingChannels[m][1];
					flag_p2=(bit)(k&0x01);
					k=++k/2+1;
					if(flag_p2)
						k=codingChannels[m][k]>>4;
					else
						k=codingChannels[m][k]&0x0f;
					if(k<10)
						tel_jishi[j]=DIALING_TIMER_LENGTH;
					else 
						{
						if(k==12)
							{
							n=set_phoneUnconditionalTransfers(codingChannels[m]);	
							if(n==0)
								{
								decodingchannel_broken(j);
								hasDecodedNumberFlags=hasDecodedNumberFlags|h;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}	
							else
								{
								decodingchannel_resume(j);
								START_DIAL_TONE(j);
								status[j][0]=00;
								codingChannels[m][1]=0;
								tel_jishi[j]=DIALING_TIMER_LENGTH;
								}
							}
						else
							flag_p1=0;
						}
					}
				else
					flag_p1=0;
				if(flag_p1==0)
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;	
			case 128:
				bohao128_process(j,m,h);
				break;
			case 129:
				bohao129_process(j,m);
				break;
			case 132:
				bohao132_process(j,m,h);
				break;
			case 133:
				bohao133_process(j,m,h);
				break;
			//case 134:
				//bohao134_process(j,m);
				//break;
			case 141:
				k=codingChannels[m][1];
				flag_p1=0;
				if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						flag_p1=1;
					}
				else if(k==4)
					flag_p1=((codingChannels[m][3]&0x0f)<10)&&(codingChannels[m][3]<0x9a); //yerh
				else if(k==6)
					{
					if(((codingChannels[m][4]&0x0f)<10)&&(codingChannels[m][4]!=0)&&(codingChannels[m][4]<0x13)) //month
						flag_p1=1;
					}
				else if(k==8)
					{
					if(((codingChannels[m][5]&0x0f)<10)&&(codingChannels[m][5]!=0)&&(codingChannels[m][5]<0x32)) //day
						flag_p1=1;
					}
				else if(k==10)
					{
					if(((codingChannels[m][6]&0x0f)<10)&&(codingChannels[m][6]<0x24)) //hour
						flag_p1=1;
					}
				else if(k==12)
					{
					if(((codingChannels[m][7]&0x0f)<10)&&(codingChannels[m][7]<0x60)) //minute
						flag_p1=1;
					}
				else if(k==13)
					{
					if((codingChannels[m][8]>>4)!=0&&(codingChannels[m][8]>>4)<8)//week
						flag_p1=1;
					}
				else if(k==14)
					{
					if((codingChannels[m][8]&0x0f)==12)
						{
						Write_Enable();
						Osc_Enable();
						pbxCurrentTime[0]=0;			//second
						pbxCurrentTime[1]=codingChannels[m][7];		//minute
						pbxCurrentTime[2]=codingChannels[m][6];		//hour
						pbxCurrentTime[3]=codingChannels[m][5];		//day
						pbxCurrentTime[4]=codingChannels[m][4];		//month
						pbxCurrentTime[5]=codingChannels[m][8]>>4;		//week
						pbxCurrentTime[6]=codingChannels[m][3];		//year
						pbxCurrentTime[7]=0;			//写保护
						TIME_BYTES_WRITE(0xbe,pbxCurrentTime,8);	//命令字节为BEh,多字节写入模式此模块为8 个
						flag_p1=1;
						codingChannels[m][1]=0;		//清号码个数
						START_DIAL_TONE(j);
						status[j][0]=47;
						}
					}
				else 
					flag_p1=1;	//号码位数等于 3,5....
				if(flag_p1)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
			case 142:
				k=codingChannels[m][1];
				flag_p1=0;
				if(k==2)
					{
					if((codingChannels[m][2]&0x0f)==11)
						flag_p1=1;
					}
				else if(k==4)
					flag_p1=((codingChannels[m][3]&0x0f)<10)&&(codingChannels[m][3]<0x24); //hour
				else if(k==6)
					{
					if(((codingChannels[m][4]&0x0f)<10)&&(codingChannels[m][4]<0x60)) //min
						flag_p1=1;
					}
				else if(k==7)
					{
					flag_p1=codingChannels[m][5]>>4==11;
					codingChannels[m][1]++;
					}
				else if(k==10)
					{
					flag_p1=((codingChannels[m][6]&0x0f)<10)&&(codingChannels[m][6]<0x24); //hour
					}
				else if(k==12)
					{
					if(((codingChannels[m][7]&0x0f)<10)&&(codingChannels[m][7]<0x60)) //min
						flag_p1=1;
					}
				else if(k==13)
					{
					if(codingChannels[m][8]>>4==12)
						{
						flag_p1=1;
						if(set_yefu_time(&codingChannels[m][3]))	
							{
							decodingchannel_resume(j);
							START_DIAL_TONE(j);
							status[j][0]=47;
							codingChannels[m][1]=0;
							}
						else
							{
							hasDecodedNumberFlags=hasDecodedNumberFlags|h;
							decodingchannel_broken(j);
							}
						}
					}
				else
					flag_p1=1;
				if(flag_p1)
					tel_jishi[j]=DIALING_TIMER_LENGTH;
				else
					{
					decodingchannel_free(j);
					pbx_if_busytone_play(j);
					status[j][0]=03;
					}
				break;
				
			default: 
				break;
			}
		
		h=h<<1;
	}
}

