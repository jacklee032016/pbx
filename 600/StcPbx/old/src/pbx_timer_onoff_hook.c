/*
* $Id: pbx_timer_onoff_hook.c,v 1.1 2007/05/11 20:13:23 lizhijie Exp $
*/


void	time_end1()/* on-off hook timer task, in unit of 100ms  */
{
	INT8U	data j,k,n;
	INT8U	xdata *data ptr;
	BOOL	flag_p1;
	
	for(j=0;j<20;j++)
	{
		if(--tel_jishi[j] !=0 )
		{
			continue;
		}
		
		switch(status[j][0])
		{
		case 00:
			STOP_DIAL_TONE(j);
			decodingchannel_free(j);
			pbx_if_busytone_play(j);
			status[j][0]=03;
			break;
		case 01:
			decodingchannel_free(j);
			pbx_if_busytone_play(j);
			status[j][0]=03;
			break;
		case 02:
			break;
		case 03:
			break;
		case 04:
			k=++bell_reg[j];
			if(k!=cuigua_time)
				{
				k=k&0x01;
				if(k==1)
					{
					STOP_DIAL_TONE(j);
					tel_jishi[j]=cgy_space;
					}
				else
					{
					START_DIAL_TONE(j);
					tel_jishi[j]=cgy_space;
					}
				}
			else
				{
				STOP_DIAL_TONE(j);
				status[j][0]=9;
				if(status[j][2]!=0xff)
					tel_jishi[j]=ldts_next_time;
				k=status[j][1];
				pbx_voice_channel_connect(j,k);
				status[k][0]=32;
				}
			break;
		case 9:
			if(status[j][2]!=0xff)
				{
				k=status[j][1];
				pbx_voice_channel_broken(j,k);
				start_ldts(j);
				status[j][0]=22;
				n=status[k][0];
				if(k<8)
					status[k][0]=83;
				else if(k<12)
					status[k][0]=43;
				else 
					status[k][0]=100;
				status[k][2]=n;
				}
			break;
		case 10:
			break;
		/*case 13:
			flag_p1=0;
			for(n=0;n<4;n++)
				{
				if(codingChannels[n][0]==j)
					{
					flag_p1=1;
					break;
					}
				}
			if(flag_p1)
				tel_jishi[j]=5;
			else
				{
				status[j][0]=07;
				k=status[j][1];
				status[k][0]=32;
				lode_thxs(j,k);
				}
			break;*/
		case 13:				
			flag_p1=0;
			for(n=0;n<4;n++)
				{
				if(codingChannels[n][0]==j)
					{
					flag_p1=1;
					break;
					}
				}
			if(flag_p1)
				tel_jishi[j]=5;
			else
				{
				forbid_waixian=forbid_waixian|(0x01<<j);	//repaired by cz 9.28	拨打外线出局后若挂机直接进入挂机状态
				status[j][0]=07;
				k=status[j][1];
				status[k][0]=32;
				lode_thxs(j,k);
				}
			break;
		case 14:
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 61:
		case 123:
		case 124:
		case 125:
		case 126:
		case 127:
		case 128:
		case 132:
		case 133:
		case 141:
		case 142:
			decodingchannel_free(j);
			STOP_DIAL_TONE(j);
			pbx_if_busytone_play(j);
			status[j][0]=03;
			break;
		case 15:
			STOP_DIAL_TONE(j);
			decodingchannel_free(j);
			k=status[j][1];
			pbx_if_stop_music(k);
			pbx_voice_channel_connect(j,k);
			resume_talk(status[k],status[j]);
			break;
		case 16:
			decodingchannel_free(j);
			k=status[j][1];
			pbx_if_stop_music(k);
			pbx_voice_channel_connect(j,k);
			resume_talk(status[k],status[j]);
			break;
		case 17:
			decodingchannel_free(j);	//0728
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			status[k][2]=0xff;
			gjclb(k);
			break;
		case 18:
			decodingchannel_free(j);	//0728
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			pbx_if_stop_music(k);
			if(status[j][2]!=0xff)
				{
				n=status[j][2];
				status[j][2]=0xff;
				pbx_if_ringback_tone_play(k);
				if(status[k][2]==32)
					{
					status[k][0]=29;
					status[n][0]=10;
					}
				else if(status[k][2]==31)
					{
					status[k][0]=29;
					status[n][0]=21;
					}
				else if(status[k][2]==92||status[k][2]==97)
					{
					status[k][0]=91;
					status[n][0]=21;
					flag_paicha=0;		//repaired by cz on 19,Mar 2007 ip拨打时候设置拍叉不再发送摘机信号
					}
				else 
					{
					status[k][0]=02;
					status[n][0]=20;
					}
				status[n][1]=k;
				status[k][1]=n;
				status[k][2]=0xff;
				}
			else
				{
				gjclb(k);
				status[k][2]=0xff;
				}
			break;
		case 20:
			k=++bell_reg[j];
			if(k%3==0)
				pbx_if_start_ringing(j);
			else
				pbx_if_stop_ringing(j);
			tel_jishi[j]=10;
			if(k==phoneLeftTranferTimes[j])
				{
				k=phoneLeftTransfers[j][1];
				if(k==0xaa)
					{
					k=allot_trackb();
					if(k!=0xff)
						{
						if(start_fahao(k,phoneLeftTransfers[j]))
							{
							pbx_line_offhook(k);
							status[k][0]=40;
							n=status[j][1];/*主叫物理号*/
							status[k][1]=n;
							if(status[n][1]==j)
								status[n][1]=k;
							else
								status[n][2]=k;
							PBX_ROW_FREE(j);
							pbx_if_stop_ringing(j);
							stop_qudian(j);
							status[j][0]=19;
							}
						else
							{
							TRACK_SET_FREE(k);
							PBX_ROW_FREE(k);
							}
						}
					}
				else if(k==0xbb)
					left_transfer_to_ip(status[j][1],j);
				else if(k!=0xff)
					{
					if(status[k][0]==19)
						{
						PBX_ROW_FREE(j);
						pbx_if_stop_ringing(j);
						stop_qudian(j);
						status[j][0]=19;
						start_bella(k);
						pbx_row_changed(k);
						PBX_ROW_BUSY(k);
						n=status[j][1];		/*主叫物理号*/
						callerid_play_start(n,k);
						status[k][0]=20;
						status[k][1]=n;
						if(status[n][1]==j)
							status[n][1]=k;
						else
							status[n][2]=k;
						}
					}
				}
			break;
		case 21:
			k=++bell_reg[j];
			if(boolDifferRinging)	//是否内外线区分振铃
				{
				if(k&0x01)
					{
					pbx_if_stop_ringing(j);
					if(k&0x02)
						tel_jishi[j]=20;
					else
						tel_jishi[j]=2;
					}
				else
					{
					pbx_if_start_ringing(j);
					tel_jishi[j]=4;
					}
			
				}
			else
				{
				if(k%3==0)
					pbx_if_start_ringing(j);
				else
					pbx_if_stop_ringing(j);
				tel_jishi[j]=10;
				}
			if(k==phoneLeftTranferTimes[j])
				{
				k=phoneLeftTransfers[j][1];
				if(k==0xaa)
					{
					k=allot_trackb();
					if(k!=0xff)
						{
						if(start_fahao(k,phoneLeftTransfers[j]))
							{
							pbx_line_offhook(k);
							status[k][0]=40;
							n=status[j][1];	/*主叫物理号*/
							status[k][1]=n;
							if(status[n][1]==j)
								status[n][1]=k;
							else
								status[n][2]=k;
							PBX_ROW_FREE(j);
							pbx_if_stop_ringing(j);
							stop_qudian(j);
							status[j][0]=19;
							}
						else
							{
							TRACK_SET_FREE(k);
							PBX_ROW_FREE(k);
							}
						}
					}
				else if(k==0xbb)
					left_transfer_to_ip(status[j][1],j);
				else if(k!=0xff)
					{
					if(status[k][0]==19)
						{
						PBX_ROW_FREE(j);
						pbx_if_stop_ringing(j);
						stop_qudian(j);
						status[j][0]=19;
						start_bella(k);
						pbx_row_changed(k);
						PBX_ROW_BUSY(k);
						n=status[j][1];/*主叫物理号*/
						callerid_play_start(n,k);
						status[k][0]=21;
						status[k][1]=n;
						if(status[n][1]==j)
							status[n][1]=k;
						else
							status[n][2]=k;
						}
					}
				}
			break;
		case 22:
			k=++bell_reg[j];
			if(k!=ldts_time)
				{
				k=k&0x01;
				if(k==1)
					{
					STOP_DIAL_TONE(j);
					tel_jishi[j]=ldts_space;
					}
				else
					{
					START_DIAL_TONE(j);
					tel_jishi[j]=ldts_space;
					}
				}
			else
				{
				stop_ldts(j);
				k=status[j][1];
				pbx_voice_channel_connect(j,k);
				resume_talk(status[k],status[j]);
				status[j][0]=9;
				tel_jishi[j]=ldts_next_time;
				}
			break;
		case 24:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				k=status[j][1];
				n=status[j][2];
				status[j][2]=0xff;
				pbx_if_stop_ringing(n);
				stop_qudian(n);
				PBX_ROW_FREE(n);
				status[n][0]=19;
				pbx_if_stop_music(k);
				pbx_voice_channel_connect(j,k);
				resume_talk(status[k],status[j]);
				}
			break;

			
		case 25:
			if(pbxHwStatus.trackFreeStatus&(0x01<<(j-8)))	
				{/*/ 若中继已取消屏蔽，为第n（n>=2)次进入*/
				TRACK_SET_FREE(j);
				}
			else
				{
				TRACK_MASK_SET(j);
				tel_jishi[j]=10;	//定时20/0.1s 后取消中继中继忙标志
				}
			break;
		case 26:
			break;

			
		case 27:
			pbx_if_record_stop_play(j);
			status[j][0]=28;
			tel_jishi[j]=DIALING_TIMER_LENGTH;
			break;
		case 28:
			decodingchannel_free(j);
			flag_p1=0;
			if(boolNoDialAction)
				{
				k=virtualConsoles[j-8];
				if(NO_DISTURB(k))
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
							if(n==0xff)
								n=k;
							}
						}
					if(n==0xaa||n==0xbb)
						flag_p1=hjzy_process(j,ptr);
					else //if(n!=0xff)
						flag_p1=hjzy_process1(j,n);
					}
				else
					{
					n=found_other_xnzj();
					if(n!=0xff)
						flag_p1=hj_proa(j,n);
					}
				}
			if(flag_p1==0)
				{	
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				}
			break;
		case 29:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				decodingchannel_free(j);	//0728
				pbx_if_ringback_tone_stop(j);
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				k=status[j][1];
				if(k<8)
					{
					if(status[k][0]==21)
						{
						pbx_if_stop_ringing(k);
						stop_qudian(k);
						PBX_ROW_FREE(k);
						status[k][0]=19;
						}
					else if(status[k][0]==22)
						{
						stop_ldts(k);
						n=status[k][1];
						pbx_voice_channel_connect(k,n);
						resume_talk(status[n],status[k]);
						}
					else if(status[k][0]==9)
						{
						n=status[k][1];
						if(status[n][0]==05)
							status[k][0]=06;
						else if(status[n][0]==06)
							status[k][0]=05;
						else if(status[n][0]==31||status[n][0]==92)
							status[k][0]=8;
						else 
							status[k][0]=07;
						}
					status[k][2]=0xff;
					}
				else if(k<12)
					{
					stop_fahao(k);
					pbx_line_onhook(k);
					PBX_ROW_FREE(k);
					status[k][0]=25;
					tel_jishi[k]=mask_track_time;
					}
				else
					send_guaji(k);
				}
			break;
		case 30:
			decodingchannel_free(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		case 32:
			k=status[j][1];
			if(k<8&&phoneCallLengthes[k])
				{
				pbx_voice_channel_broken(j,k);
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				if(status[k][2]==0xff)
					{
					status[k][0]=03;
					pbx_if_busytone_play(k);
					}
				else
					{
					if(status[k][2]==j)
						{
						status[k][2]=0xff;
						n=status[k][1];
						pbx_if_stop_music(n);
						pbx_voice_channel_connect(k,n);
						resume_talk(status[n],status[k]);
						}
					else
						{
						n=status[k][2];
						status[k][2]=0xff;
						pbx_if_ringback_tone_stop(n);
						status[k][1]=n;
						pbx_voice_channel_connect(k,n);
						if(n<8)
							{
							status[n][0]=05;
							status[n][1]=06;
							}
						else if(n<12)
							{
							status[n][0]=31;
							status[n][1]=8;
							}
						else
							{
							status[n][0]=92;
							status[n][1]=8;
							}
						}
					}
				}
			break;
		case 33:
			k=status[j][1];
			if(status[j][2]==32&&k<8&&phoneCallLengthes[k])
				{
				pbx_if_stop_music(j);
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				k=status[j][1];
				status[j][2]=0xff;
				n=status[k][2];
				status[k][2]=0xff;
				status[k][1]=n;
				if(status[k][0]==15)
					status[k][0]=00;
				else if(status[k][0]==16)
					status[k][0]=01;
				else if(status[k][0]==24||status[k][0]==130)
					status[k][0]=02;
				else if(status[k][0]==18)
					{
					PBX_ROW_FREE(k);
					status[k][0]=19;
					if(n!=0xff)
						{
						pbx_if_stop_ringing(n);
						stop_qudian(n);
						PBX_ROW_FREE(n);
						status[n][0]=19;
						}
					}
				else if(status[k][0]==10)
					{
					pbx_if_stop_ringing(k);
					stop_qudian(k);
					PBX_ROW_FREE(k);
					status[k][0]=19;
					}
				}	
			break;
		case 35:
			decodingchannel_free(status[j][1]);
			break;
		case 36:
			pbx_line_onhook(j);
			PBX_ROW_FREE(k);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		case 42:
			k=status[j][1];
			if(status[j][2]==32&&phoneCallLengthes[k])
				{
				pbx_if_stop_music(j);
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				status[j][2]=0xff;
				k=status[j][1];
				if(status[k][0]==80)
					{
					n=status[k][2];
					status[k][2]=0xff;
					status[k][1]=n;
					if(n<8)
						{
						status[n][0]=05;
						status[k][0]=06;
						}
					else if(n<12)
						{
						status[n][0]=31;
						status[k][0]=8;
						}
					else
						{
						status[n][0]=92;
						status[k][0]=8;
						}
					}
				else
					{
					if(status[k][2]==0xff)
						{
						status[k][0]=19;
						PBX_ROW_FREE(k);
						}
					}
				}
			break;
		case 43:
			k=status[j][1];
			if(status[j][2]==32&&phoneCallLengthes[k])
				{
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				status[j][2]=0xff;
				k=status[j][1];
				if(status[k][0]==04)
					STOP_DIAL_TONE(k);
				else 
					stop_ldts(k);
				if(status[k][2]!=0xff)
					{
					n=status[k][2];
					status[k][2]=0xff;
					status[k][1]=n;
					pbx_if_ringback_tone_stop(n);
					pbx_voice_channel_connect(k,n);
					if(n<8)
						{
						status[n][0]=05;
						status[k][0]=06;
						}
					else if(n<12)
						{
						status[n][0]=31;
						status[k][0]=8;
						}
					else 
						{
						status[n][0]=92;
						status[k][0]=8;
						}
					}
				else
					{
					pbx_if_busytone_play(k);
					status[k][0]=03;
					}
				}
			break;
		case 44:
			if(--zjld_reg[j-8][1]==0)
				{
				status[j][0]=25;
				PBX_ROW_FREE(j);
				TRACK_SET_FREE(j);
				TRACK_MASK_SET(j);
				k=status[j][1];
				if(k<8)
					{
					pbx_if_stop_ringing(k);
					stop_qudian(k);
					PBX_ROW_FREE(k);
					status[k][0]=19;
					}
				else if(k<12)
					{
					stop_fahao(k);
					pbx_line_onhook(k);
					PBX_ROW_FREE(k);
					status[k][0]=25;
					tel_jishi[k]=mask_track_time;
					}
				else if(k<20)
					send_guaji(k);
				else	//群呼
					{
					for(k=0;k<4;k++)
						{
						if(group_call[k][0]==j)
							{
							INT8U data tel;
							group_call[k][0]=0xff;	//清空以备其他用
							for(tel=1;tel<9;tel++)
								{
								n=group_call[k][tel];
								group_call[k][tel]=0xff;//清空以备其他用
								if(n<8)
									{
									pbx_if_stop_ringing(n);
									stop_qudian(n);
									PBX_ROW_FREE(n);
									status[n][0]=19;
									}
								else
									break;
								}
							break;
							}
						}
					}
				}
			else
				tel_jishi[j]=1;
			break;
		case 60:
			STOP_RECORD();
			isd1420Free=1;
			pbx_if_busytone_play(j);
			status[j][0]=03;
			break;
		case 62:
			STOP_DIAL_TONE(j);
			k=is_had_zhanghao(j);
			if(k==0xff)
				k=phoneAuthorities[j];
			if(k<5)
				{
				n=pbx_line_allocate_channel(j);
				if(n!=0xff)
					{
					status[j][0]=13;
					status[j][1]=n;
					status[n][0]=35;
					status[n][1]=j;
					tel_jishi[j]=100;
					tel_jishi[n]=220;
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
			break;
		case 63:
			STOP_DIAL_TONE(j);
			pbx_if_busytone_play(j);
			status[j][0]=65;
			break;
		case 64:
			pbx_if_busytone_play(j);
			status[j][0]=65;
			break;
		case 65:
			k=++bell_reg[j];
			if(k!=busysound_time1)
				{
				k=k&0x01;
				if(k==0)
					START_DIAL_TONE(j);
				else 
					STOP_DIAL_TONE(j);
				tel_jishi[j]=busysound_space;
				}
			else 
				{
				START_DIAL_TONE(j);
				decodingchannel_clear(j);
				status[j][0]=63;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				}
			break;
		case 66:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				pbx_if_busytone_play(j);
				status[j][0]=65;
				k=status[j][1];
				if(k<8)
					{
					pbx_if_stop_ringing(k);
					stop_qudian(k);
					PBX_ROW_FREE(k);
					status[k][0]=19;
					}
				else
					send_guaji(k);
				}
			break;
		case 68:
			decodingchannel_free(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			meeting_reg[0]=0xff;
			meeting_reg[1]=0xff;
			for(n=2;n<21;n++)
				{
				if(meeting_reg[n]!=0xff)
					{
					k=meeting_reg[n];
					meeting_reg[n]=0xff;
					pbx_if_stop_music(k);
					gjclb(k);
					}
				}
			break;
		case 75:
			k=++bell_reg[j];
			k=k%3;
			if(k==0)
				pbx_if_start_ringing(j);
			else
				pbx_if_stop_ringing(j);
			tel_jishi[j]=10;
			break;
		case 77:
			k=status[j][1];
			if(status[k][0]==95)
				{
				decodingchannel_free(j);
				pbx_if_busytone_play(j);
				status[j][0]=03;
				}
			else
				{
				pbx_if_busytone_play(j);
				status[j][0]=65;
				}
			send_guaji(k);
			break;
		case 78:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				decodingchannel_free(j);	//0728
				pbx_if_ringback_tone_stop(j);
				pbx_if_busytone_play(j);
				status[j][0]=03;
				k=status[j][1];
				send_guaji(k);
				}
			break;
		case 79:
			k=status[j][1];
			n=status[j][2];
			if(n==0xff)
				{
				PBX_ROW_FREE(j);
				status[j][0]=19;
				pbx_if_stop_music(k);
				gjclb(k);
				status[k][2]=0xff;
				}
			else
				{
				status[j][1]=n;
				status[j][2]=0xff;
				pbx_if_stop_music(k);
				gjclb(k);
				status[k][2]=0xff;
				start_bella(j);
				if(k<8)
					status[j][0]=20;
				else
					status[j][0]=21;
				}
			break;
		case 84:
			k=bell_reg[j];
			if(++k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				pbx_if_busytone_play(j);
				status[j][0]=03;
				k=status[j][1];
				status[k][2]=0xff;
				if(status[k][0]==22)
					{
					stop_ldts(k);
					n=status[k][1];
					pbx_voice_channel_connect(k,n);
					resume_talk(status[n],status[k]);
					}
				else if(status[k][0]==9)
					{
					n=status[k][1];
					if(status[n][0]==05)
						status[k][0]=06;
					else if(status[n][0]==06)
						status[k][0]=05;
					else if(status[n][0]==31||status[n][0]==92)
						status[k][0]=8;
					else
						status[k][0]=07;
					}
				}
			break;
		case 85:
			pbx_if_record_stop_play(j);
			if(decodingchannel_allocate(j))
				{
				START_DIAL_TONE(j);
				status[j][0]=00;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				}
			else
				{
				pbx_if_busytone_play(j);
				status[j][0]=03;
				}
			break;
		case 87:
			send_guaji(j);	//发送挂机信号
		case 91:
			k=++bell_reg[j];
			if(k!=ip_backsound_time) //一般由IP控制，转接时由PBX控制
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				k=status[j][1];
				send_guaji(j);
				status[k][2]=0xff;
				if(k<8)
					{
					if(status[k][0]==21)
						{
						pbx_if_stop_ringing(k);
						stop_qudian(k);
						PBX_ROW_FREE(k);
						status[k][0]=19;
						}
					else if(status[k][0]==22)
						{
						stop_ldts(k);
						n=status[k][1];
						pbx_voice_channel_connect(k,n);
						resume_talk(status[n],status[k]);					
						}
					else if(status[k][0]==9)
						{
						n=status[k][1];
						if(status[n]==05)
							status[k][0]=06;
						else if(status[n]==06)
							status[k][0]=05;
						else if(status[n]==31||status[n][0]==92)
							status[k][0]=8;
						else
							status[k][0]=07;
						}
					}
				else if(k<12)
					{
					stop_fahao(k);
					pbx_line_onhook(k);
					PBX_ROW_FREE(k);
					status[k][0]=25;
					tel_jishi[k]=mask_track_time;
					}
				else
					send_guaji(k);
				}
			break;
		case 108:
			send_guaji(j);
			break;
		case 112:
			send_guajia(j);
			break;
		case 113:			//PCM channel check  overtime
			decodingchannel_free(j);
			status[j][0]=86;
			columnFrees=0xff;
			break;
		case 118:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				pbx_line_onhook(j);
				PBX_ROW_FREE(j);
				status[j][0]=25;
				tel_jishi[j]=2;
				for(k=0;k<4;k++)
					{
					if(group_call[k][0]==j)
						{
						INT8U data tel;
						group_call[k][0]=0xff;
						for(tel=1;tel<9;tel++)
							{
							n=group_call[k][tel];
							group_call[k][tel]=0xff;
							if(n<8)
								{
								pbx_if_stop_ringing(n);
								stop_qudian(n);
								PBX_ROW_FREE(n);
								status[n][0]=19;
								}
							else
								break;
							}
						break;
						}
					}
				}
			break;
		case 129:
			STOP_DIAL_TONE(j);
			decodingchannel_free(j);
			k=status[j][1];
			pbx_if_stop_music(k);
			pbx_voice_channel_connect(j,k);
			resume_talk(status[k],status[j]);
			break;
		case 130:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				k=status[j][2];
				status[j][2]=0xff;
				pbx_if_stop_ringing(k);
				stop_qudian(k);
				PBX_ROW_FREE(k);
				status[k][0]=19;
				k=status[j][1];
				pbx_if_stop_music(k);
				pbx_voice_channel_connect(j,k);
				resume_talk(status[k],status[j]);
				}
			break;
		/*case 134:
			decodingchannel_free(j);
			pbx_if_busytone_play(j);
			status[j][0]=03;
			break;*/
		/*case 135:
			k=++bell_reg[j];
			if(k!=RINGBACK_LOOP_COUNT)
				{
				k=k&0x01;
				if(k==1)
					{
					pbx_if_ringback_tone_stop(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_OFF;
					}
				else
					{
					pbx_if_ringback_tone_play(j);
					tel_jishi[j]=TONE_TIME_RINGBACK_ON;
					}
				}
			else
				{
				pbx_if_ringback_tone_stop(j);
				pbx_if_busytone_play(j);
				status[j][0]=03;
				k=status[j][1];
				n=k>>4;
				if(n!=0x0f)
					{
					pbx_if_stop_ringing(n);
					stop_qudian(n);
					PBX_ROW_FREE(n);
					status[n][0]=19;
					}
				n=k&0x0f;
				if(n!=0x0f)
					{
					pbx_if_stop_ringing(n);
					stop_qudian(n);
					PBX_ROW_FREE(n);
					status[n][0]=19;
					}
				k=status[j][2];
				status[j][2]=0xff;
				n=k>>4;
				if(n!=0x0f)
					{
					pbx_if_stop_ringing(n);
					stop_qudian(n);
					PBX_ROW_FREE(n);
					status[n][0]=19;
					}
				n=k&0x0f;
				if(n!=0x0f)
					{
					pbx_if_stop_ringing(n);
					stop_qudian(n);
					PBX_ROW_FREE(n);
					status[n][0]=19;
					}
				}
			break;*/
		case 136:
			k=++bell_reg[j];
			if(boolDifferRinging)	//是否内外线区分振铃
				{
				if(k&0x01)
					{
					pbx_if_stop_ringing(j);
					if(k&0x02)
						tel_jishi[j]=20;
					else
						tel_jishi[j]=2;
					}
				else
					{
					pbx_if_start_ringing(j);
					tel_jishi[j]=4;
					}
			
				}
			else
				{
				k=k%3;
				if(k==0)
					pbx_if_start_ringing(j);
				else
					pbx_if_stop_ringing(j);
				tel_jishi[j]=10;
				}
			break;
		case 137:
			k=++bell_reg[j];
			if(k!=busysound_time1)
				{
				k=k&0x01;
				if(k==1)
					{
					STOP_DIAL_TONE(j);
					tel_jishi[j]=busysound_space;
					}
				else
					{
					START_DIAL_TONE(j);
					tel_jishi[j]=busysound_space;
					}
				}
			else
				{
				if(decodingchannel_allocate(j))
					{
					START_DIAL_TONE(j);
					status[j][0]=00;
					tel_jishi[j]=DIALING_TIMER_LENGTH;
					}

			else
					status[j][0]=03;
				}
			break;
		case 139:
			pbx_if_stop_ringing(j);
			status[j][0]=19;
			break;
		default: 
			break;
		}
		}
	}
}

