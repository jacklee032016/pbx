/*
* $Id: pbx_main_offhook.c,v 1.1 2007/05/11 20:13:23 lizhijie Exp $
*/


/* 0x15 and 0x16 and called by Main Loop 
a EXT is on-hook or off-hook, param is EXT|PSTN|IP index, from 0 to 19 */
void	ext_on_off_hook_process(INT8U j)
{
	INT8U	data n,k;
	
	switch(status[j][0])
	{	
		case 00:
		case 62:
			is_zhanghao(j);			
			STOP_DIAL_TONE(j);
			decodingchannel_free(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 01:
			is_zhanghao(j);
			decodingchannel_free(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 02:
			is_zhanghao(j);
			pbx_if_ringback_tone_stop(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			if(k<8)
				{
				if(status[k][0]==20)
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
					resume_talk(status[n],status[k]);	/*恢复原通话状态*/
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
			break;
		case STATE_EXT_PLAY_BUSY://03:
			is_zhanghao(j);
			STOP_DIAL_TONE(j);
			PBX_ROW_FREE(j);
			status[j][0]=STATE_EXT_IDLE;//19;
			
			if(status[j][2]!=0xff)	//060308修改
			{
				send_guaji(status[j][2]);
				status[j][2]=0xff;
			}
			
			break;
		case 04:
			is_zhanghao(j);
			STOP_DIAL_TONE(j);/*停送催挂音*/
			if(status[j][2]==0xff)		
				{/*第三字节空*/
				PBX_ROW_FREE(j);
				status[j][0]=19;
				k=status[j][1];
				pbx_line_onhook(k);
				PBX_ROW_FREE(k);
				status[k][0]=25;
				tel_jishi[k]=mask_track_time;	
				}
			else	
				{/*第三字节不空*/
				status[j][0]=79;
				tel_jishi[j]=paicha_time;
				k=status[j][1];
				pbx_if_start_music(k);
				status[k][0]=42;
				}
			break;
		case 05:
			is_zhanghao(j);
			if(status[j][2]==0xff)
				{
				k=status[j][1];
				pbx_voice_channel_broken(j,k);
				if(status[k][2]==0xff)
					{
					status[j][0]=17;
					status[k][0]=23;
					status[k][2]=06;
					tel_jishi[j]=paicha_time;
					}
				else				//对方送来电提示情况
					{
					status[j][0]=19;
					PBX_ROW_FREE(j);
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
			else
				{
				k=status[j][2];			//现通话方
				status[j][2]=0xff;
				pbx_voice_channel_broken(j,k);
				n=status[j][1];			//原通话方
				pbx_if_stop_music(n);
				pbx_voice_channel_connect(k,n);
				status[j][0]=19;
				PBX_ROW_FREE(j);
				if(status[n][2]==05||status[n][2]==06)
					{
					status[n][0]=05;
					status[n][1]=k;
					status[n][2]=0xff;
					status[k][1]=n;
					}
				else if(status[n][2]==31||status[n][2]==92)
					{
					status[n][0]=status[n][2];
					status[n][1]=k;
					status[n][2]=0xff;
					status[k][0]=8;
					status[k][1]=n;
					}
				else if(status[n][2]==32||status[n][2]==97)
					{
					status[n][0]=status[n][2];
					status[n][1]=k;
					status[n][2]=0xff;
					status[k][0]=07;
					status[k][1]=n;
					if(k<12)
						lode_thxs(k,n);
					}
				}
			break;
		case 06:
			is_zhanghao(j);
			k=status[j][1];		//对方，现通话方
			if(status[k][2]==0xff)
				{
				pbx_voice_channel_broken(j,k);
				status[j][0]=17;
				status[k][0]=23;
				status[k][2]=05;
				tel_jishi[j]=paicha_time;
				}
			else
				{
				pbx_voice_channel_broken(j,k);
				status[j][0]=19;
				PBX_ROW_FREE(j);
				if(status[k][1]==j)		//K送来电提示情况
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
				else
					{
					status[k][2]=0xff;
					n=status[k][1];			//原通话方
					pbx_if_stop_music(n);
					pbx_voice_channel_connect(k,n);
					resume_talk(status[n],status[k]);					
					}
				}
			break;
		/*case 07:
			is_zhanghao(j);
			if(status[j][2]==0xff)
				{
				status[j][0]=17;
				tel_jishi[j]=paicha_time;
				k=status[j][1];
				pbx_voice_channel_broken(j,k);
				if(k<12)
					{
					status[k][0]=34;
					status[k][2]=32;
					}
				else	
					{
					status[k][0]=93;
					status[k][2]=97;
					}
				}
			else	
				{
				k=status[j][1];//原通话方
				n=status[j][2];//现通话方
				status[j][2]=0xff;
				pbx_voice_channel_broken(j,n);
				pbx_if_stop_music(k);
				pbx_voice_channel_connect(k,n);
				status[j][0]=19;
				PBX_ROW_FREE(j);
				if(status[k][2]==05||status[k][2]==06)
					{
					status[k][0]=07;
					status[n][0]=32;
					lode_thxs(k,n);
					}
				else if(status[k][2]==31)
					{
					status[k][0]=31;
					status[n][0]=32;
					}					
				else if(status[k][2]==92)
					{
					status[k][0]=92;
					status[n][0]=32;
					}
				else if(status[k][2]==32)
					{
					status[k][0]=32;
					status[n][0]=31;
					}
				else if(status[k][2]==97)
					{
					status[k][0]=97;
					status[n][0]=31;
					}
				status[k][1]=n;
				status[k][2]=0xff;
				status[n][1]=k;		
				}
			break;*/
		case 07:				/* repaired by cz 9.1	拨打外线出局后若挂机直接进入挂机状态			*/
			is_zhanghao(j);
			if(status[j][2]==0xff)
				{
				if((forbid_waixian)&(0x01<<j))	//repaired by cz 9.28	拨打外线出局后若挂机直接进入挂机状态
					{
					forbid_waixian=forbid_waixian&~(0x01<<j);	//repaired by cz 9.28	拨打外线出局后若挂机直接进入挂机状态
					status[j][0]=19;
					k=status[j][1];
					pbx_voice_channel_broken(j,k);
					status[k][0]=25;
					pbx_line_onhook(k);
					PBX_ROW_FREE(k);
					tel_jishi[k]=2;
					}
				else	
					{
					status[j][0]=17;
					tel_jishi[j]=paicha_time;
					k=status[j][1];
					pbx_voice_channel_broken(j,k);
					if(k<12)
						{
						status[k][0]=34;
						status[k][2]=32;
						}
					else	
						{
						status[k][0]=93;
						status[k][2]=97;
						}
					}
				}
			else	
				{
				k=status[j][1];//原通话方
				n=status[j][2];//现通话方
				status[j][2]=0xff;
				pbx_voice_channel_broken(j,n);
				pbx_if_stop_music(k);
				pbx_voice_channel_connect(k,n);
				status[j][0]=19;
				PBX_ROW_FREE(j);
				if(status[k][2]==05||status[k][2]==06)
					{
					status[k][0]=07;
					status[n][0]=32;
					lode_thxs(k,n);
					}
				else if(status[k][2]==31)
					{
					status[k][0]=31;
					status[n][0]=32;
					}					
				else if(status[k][2]==92)
					{
					status[k][0]=92;
					status[n][0]=32;
					}
				else if(status[k][2]==32)
					{
					status[k][0]=32;
					status[n][0]=31;
					}
				else if(status[k][2]==97)
					{
					status[k][0]=97;
					status[n][0]=31;
					}
				status[k][1]=n;
				status[k][2]=0xff;
				status[n][1]=k;		
				}
			break;
		case 8:
			status[j][0]=17;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			tel_jishi[j]=paicha_time;	/*拍叉时间*/
			if(k<12)
				{
				status[k][0]=34;
				status[k][2]=31;
				}
			else
				{
				status[k][0]=93;
				status[k][2]=92;
				}
				
			break;
		case 9:
			k=status[j][1];
			pbx_voice_channel_broken(k,j);
			if(status[j][2]==0xff)
				{
				status[j][0]=17;
				tel_jishi[j]=paicha_time;	/*拍叉时间*/
				status[k][0]=34;
				status[k][2]=32;
				}
			else
				{
				status[j][0]=79;
				tel_jishi[j]=paicha_time;	/*拍叉时间*/
				pbx_if_start_music(k);
				status[k][2]=status[k][0];
				if(k<8)
					status[k][0]=82;
				else if(k<12)
					status[k][0]=42;
				else
					status[k][0]=98;
				}
			break;
		case 10:
			pbx_if_stop_ringing(j);
			stop_qudian(j);
			status[j][0]=07;
			k=status[j][1];
			pbx_if_stop_music(k);
			status[k][0]=32;
			pbx_voice_channel_connect(k,j);
			lode_thxs(j,k);		
			break;
		case 11:
			is_zhanghao(j);
			pbx_if_stop_music(j);
			status[j][0]=19;
			status[j][2]=0xff;
			k=status[j][1];
			status[k][1]=status[k][2];
			status[k][2]=0xff;
			if(status[k][0]==15)
				status[k][0]=0;
			else if(status[k][0]==16)
				status[k][0]=01;
			else if(status[k][0]==24||status[k][0]==130)
				status[k][0]=02;
			else if(status[k][0]==18)
				{
				status[k][0]=19;
				PBX_ROW_FREE(j);
				if(status[k][2]!=0xff)
					{
					n=status[k][2];
					pbx_if_stop_ringing(n);
					stop_qudian(n);
					PBX_ROW_FREE(n);
					status[n][0]=19;
					}
				}
			break;	
		case 12:
			status[j][0]=19;
			PBX_ROW_FREE(j);	
			break;
		case 13:
			if(status[j][2]==0xff)
				{
				is_zhanghao(j);
				decodingchannel_free(j);
				status[j][0]=19;
				k=status[j][1];
				pbx_line_onhook(k);
				pbx_voice_channel_broken(j,k);
				status[k][0]=25;
				tel_jishi[k]=mask_track_time;
				PBX_ROW_FREE(j);
				PBX_ROW_FREE(k);
				}
			else	
				{
				k=status[j][1];//原通话方
				n=status[j][2];//现通话方
				status[j][2]=0xff;
				pbx_voice_channel_broken(j,n);
				pbx_if_stop_music(k);	//
				pbx_voice_channel_connect(k,n);	//因不知是否已经通话，所以不可拍差返回，直接建立通话
				status[j][0]=19;
				decodingchannel_free(j);
				PBX_ROW_FREE(j);
				if(k<8)
					status[k][0]=07;
				else if(k<12)
					status[k][0]=31;
				else
					status[k][0]=92;
				status[k][1]=n;
				status[k][2]=0xff;
				status[n][0]=32;
				status[n][1]=k;
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
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;			
		case 15:
#if	FEATURE_STAY
			if(status[j][1]>7)		//repaired by cz 07年3月8号
				{
				is_zhanghao(j);
				PBX_ROW_FREE(j);
				STOP_DIAL_TONE(j);
				decodingchannel_free(j);
				
				stay_flag|=1<<(status[j][1]-8);	//置留守标志对应外线或ip
				stay_object=status[j][1];	//保存留守对象外线或ip
			
				status[j][0]=19;
				status[j][1]=0xff;
				status[j][2]=0xff;
				break;
				}
			else
#endif	
				{
				is_zhanghao(j);
				STOP_DIAL_TONE(j);
				decodingchannel_free(j);
				status[j][0]=18;
				tel_jishi[j]=paicha_time;
				break;
				}
		case 16:
			is_zhanghao(j);
			decodingchannel_free(j);
			status[j][0]=18;
			tel_jishi[j]=paicha_time;
			break;
		case 17:
			if(decodingchannel_allocate(j))
				{
				START_DIAL_TONE(j);
				status[j][0]=15;
				tel_jishi[j]=DIALING_TIMER_LENGTH;
				k=status[j][1];
				pbx_if_start_music(k);
				if(k<8)
					status[k][0]=11;
				else if(k<12)
					status[k][0]=33;
				else 
					status[k][0]=94;
				}
				
			else
				{
				k=status[j][1];
				pbx_voice_channel_connect(k,j);
				resume_talk(status[k],status[j]);
				}
			break;
		case 18:
			k=status[j][1];
			pbx_if_stop_music(k);
			pbx_voice_channel_connect(k,j);
			resume_talk(status[k],status[j]);
			if(status[j][2]!=0xff)
				{
				n=status[j][2];
				status[j][2]=0xff;
				pbx_if_stop_ringing(n);
				stop_qudian(n);
				status[n][0]=19;
				PBX_ROW_FREE(n);
				}
			break;
		
		case STATE_EXT_IDLE:
		case 139:
			{
				ext_state_idle(j);
			}
			break;

	
		case 20:
			pbx_if_stop_ringing(j);
			stop_qudian(j);
			k=status[j][1];
			if(status[k][0]==18)
				{
				status[k][0]=19;
				n=status[k][1];
				status[k][2]=0xff;
				PBX_ROW_FREE(k);
				pbx_if_stop_music(n);
				pbx_voice_channel_connect(j,n);
				if(status[n][2]==05||status[n][2]==06)
					{
					status[n][0]=05;
					status[j][0]=06;
					}
				else if(status[n][2]==31)
					{
					status[n][0]=31;
					status[j][0]=8;
					}
				else if(status[n][2]==32)
					{
					status[n][0]=31;
					status[j][0]=07;
					lode_thxs(j,n);
					}
				else if(status[n][2]==92||status[n][2]==97)
					{
					status[n][0]=92;
					status[j][0]=8;
					}
				status[n][1]=j;
				status[n][2]=0xff;
				status[j][1]=n;
				}
			else
				{
				pbx_if_ringback_tone_stop(k);
				if(status[k][0]==130)
					{
					n=status[k][1];
					pbx_if_stop_music(n);
					status[n][2]=j;
					status[j][2]=n;
					status[j][0]=131;
					status[k][0]=131;
					if(n<8)
						status[n][0]=131;
					else if(n<12)
						status[n][0]=45;
					else
						status[n][0]=109;
					pbx_voice_channel_connect(n,k);
					n=point[k];
					n=n&0x0f;
					CONNECT(j,n);/*建立三方通话*/
					n=n|(j<<4);
					point[j]=n;
					}
				else 
					{
					pbx_voice_channel_connect(j,k);
					status[j][0]=06;
					status[k][0]=05;
					}
				}
			break;
		case 21:
			pbx_if_stop_ringing(j);
			stop_qudian(j);
			status[j][0]=8;
			k=status[j][1];
			if((k>=12)&&flag_paicha)
				{
				n=found_send_buf();
				if(n!=0xff)
					{
					uartTxCmds[n][0]=0x85;
					if(++uartCmdSeq==0xff)
						uartCmdSeq++;
					uartTxCmds[n][1]=uartCmdSeq;
					uartTxCmds[n][2]=k-12;
					uartTxCmds[n][3]=peerOfIP[k-12];
					uartTxCmds[n][4]=0xff;
					busy_send_buf(n);
					}
				}
			else 
				{
				flag_paicha=1;
				}
			pbx_if_ringback_tone_stop(k);
			pbx_voice_channel_connect(j,k);
			if(status[k][0]==44)
				{
				pbx_line_offhook(k);
				status[k][0]=31;
				}
			else
				{
				if(k<12)
					{status[k][0]=31;}
				else
					{
					status[k][0]=92;					
					}
				}
			break;
		case 22:
			is_zhanghao(j);
			stop_ldts(j);
			status[j][0]=79;
			k=status[j][1];
			tel_jishi[j]=paicha_time;
			pbx_if_start_music(k);
			if(k<8)
				status[k][0]=82;
			else if(k<12)
				status[k][0]=42;
			else
				status[k][0]=98;
			break;
		
		case 23:
			is_zhanghao(j);
			status[j][0]=19;
			k=status[j][1];
			PBX_ROW_FREE(j);
			PBX_ROW_FREE(k);
			status[j][2]=0xff;
			status[k][0]=19;
			break;
		case 24:
			is_zhanghao(j);
			pbx_if_ringback_tone_stop(j);
			status[j][0]=18;
			tel_jishi[j]=paicha_time;
			break;	
		case 27:
			pbx_if_record_stop_play(j);/*停送语音提示*/
			decodingchannel_free(j);
			pbx_line_onhook(j);
			clr_ldbuf(j);/*清来电缓冲区*/
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		case 28:
			decodingchannel_free(j);
			pbx_line_onhook(j);
			clr_ldbuf(j);/*清来电缓冲区*/
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		case 29:
			decodingchannel_free(j);	//0728
			pbx_if_ringback_tone_stop(j);
			pbx_line_onhook(j);
			clr_ldbuf(j);/*清来电缓冲区*/
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
			break;
		case 30:
			decodingchannel_free(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		case 31:
			decodingchannel_free(j);	//0728
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			if(k<8&&status[k][2]!=0xff)
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
			else
				gjclb(k);
			break;
		case 32:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			if(k<8)
				{
				if(status[k][2]==0xff)
					{
					pbx_if_busytone_play(k);
					status[k][0]=03;
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
			else if(k<12)
				{
				pbx_line_onhook(k);
				PBX_ROW_FREE(k);
				status[k][0]=25;
				tel_jishi[k]=mask_track_time;
				}
			else
				send_guaji(k);
			break;
		case 33:
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
				PBX_ROW_FREE(k);
				status[k][0]=19;
				}
			break;		
		case 34:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			status[j][2]=0xff;
			PBX_ROW_FREE(k);
			status[k][0]=19;
			break;
		case 35:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			if(status[k][2]==0xff)
				{
				pbx_if_busytone_play(k);
				status[k][0]=03;
				}
			else 
				{
				status[k][2]=0xff;
				n=status[k][1];
				pbx_if_stop_music(n);
				pbx_voice_channel_connect(k,n);
				resume_talk(status[n],status[k]);			
				}
			break;
		case 37:
			cut_meeting(j);
			break;
		case 38:
			pbx_if_stop_music(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			meeting_reg[0]--;
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==j)
					{
					meeting_reg[n]=0xff;
					break;
					}
				}
			break;
		case 39:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			pbx_if_busytone_play(j);
			status[k][0]=65;
			break;
		case 40:
			stop_fahao(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			if(status[k][0]==02)
				{
				pbx_if_ringback_tone_stop(k);
				pbx_if_busytone_play(k);
				status[k][0]=03;
				}
			else if(status[k][0]==29)
				{
				pbx_if_ringback_tone_stop(k);
				pbx_line_onhook(k);
				PBX_ROW_FREE(k);
				status[k][0]=25;
				tel_jishi[k]=mask_track_time;
				}
			else if(status[k][0]==44)
				{
				status[k][0]=26;
				zjld_reg[j-8][0]=0;
				}
			else if(status[k][0]==91)
				{
				pbx_if_ringback_tone_stop(k);
				send_guaji(k);
				}
			break;
		case 41:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			status[k][2]=0xff;
			n=status[k][1];
			pbx_if_stop_music(n);
			pbx_voice_channel_connect(k,n);
			resume_talk(status[n],status[k]);
			break;
		case 42:
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
			break;
		case 43:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			status[j][2]=0xff;
			k=status[j][1];
			if(status[k][0]==04)
				STOP_DIAL_TONE(k);/*停送催挂音*/
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
			break;
		case 45:
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=point[j]&0x0f;
			point[j]=0xff;
			DISCONNECT(j,k);
			k=status[j][1];
			n=status[j][2];
			status[j][2]=0xff;
			if(status[k][1]==j)
				status[k][1]=status[k][2];
			status[k][2]=0xff;
			if(status[n][1]==j)
				status[n][1]=status[n][2];
			status[n][2]=0xff;
			status[k][0]=05;
			status[n][0]=06;
			break;
		case 60:
			STOP_RECORD();
			isd1420Free=1;
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 61:
			is_zhanghao(j);
			decodingchannel_free(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 63:
			STOP_DIAL_TONE(j);
			status[j][0]=68;
			tel_jishi[j]=paicha_time;
			break;
		case 64:
			status[j][0]=68;
			tel_jishi[j]=paicha_time;
			break;
		case 65:
			STOP_DIAL_TONE(j);
			status[j][0]=68;
			tel_jishi[j]=paicha_time;
			break;
		case 66:
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
			pbx_if_ringback_tone_stop(j);
			status[j][0]=68;
			tel_jishi[j]=paicha_time;
			break;
		case 67:
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			status[j][0]=68;
			tel_jishi[j]=paicha_time;
			pbx_if_start_music(k);
			status[k][0]=71;
			++meeting_reg[0];
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==0xff)
					{
					meeting_reg[n]=k;
					break;
					}
				}
			break;
		case 68:
			START_DIAL_TONE(j);
			decodingchannel_clear(j);
			status[j][0]=63;
			tel_jishi[j]=DIALING_TIMER_LENGTH;
			break;
		case 69:
			cut_meeting(j);
			break;
		case 70:
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			pbx_if_busytone_play(k);
			status[k][0]=65;
			break;
		case 71:
			pbx_if_stop_music(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			--meeting_reg[0];
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==j)
					{
					meeting_reg[n]=0xff;
					break;
					}
				}
			break;
		case 72:
			cut_meeting(j);
			break;
		case 73:
			status[j][0]=68;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			pbx_if_start_music(k);
			status[k][0]=38;
			tel_jishi[j]=paicha_time;
			++meeting_reg[0];
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==0xff)
					{
					meeting_reg[n]=k;
					break;
					}
				}
			break;
		case 74:
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			pbx_line_onhook(k);
			PBX_ROW_FREE(k);
			status[k][0]=25;
			tel_jishi[k]=mask_track_time;
			break;
		case 75:
			pbx_if_stop_ringing(j);
			stop_qudian(j);
			k=status[j][1];
			pbx_if_ringback_tone_stop(k);
			pbx_voice_channel_connect(j,k);
			status[j][0]=70;
			status[k][0]=67;
			break;
		case 76:
			break;
			/////////////
		case 77:
			k=status[j][1];
			if(status[k][0]==95)
				{
				decodingchannel_free(j);
				PBX_ROW_FREE(j);
				status[j][0]=19;
				}
			else
				{
				status[j][0]=68;
				tel_jishi[j]=paicha_time;
				}
			send_guaji(k);
			break;
		case 78:
			decodingchannel_free(j);	//0728
			pbx_if_ringback_tone_stop(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			send_guaji(k);
			break;
		case 79:
			if(status[j][2]==0xff)
				{
				k=status[j][1];
				pbx_if_stop_music(k);
				pbx_voice_channel_connect(j,k);
				resume_talk(status[k],status[j]);
				}
			else
				{
				k=status[j][1];
				if(status[k][0]==42||status[k][0]==82||status[k][0]==98)
					{
					n=status[j][2];
					pbx_if_ringback_tone_stop(n);
					pbx_voice_channel_connect(j,n);
					if(n<8)
						status[n][0]=81;
					else if(n<12)
					  	status[n][0]=41;
					else 
					      	status[n][0]=99;
					status[j][0]=80;
					}
				else 
					{
					n=status[j][2];
					status[j][1]=n;
					status[j][2]=0xff;
					pbx_if_ringback_tone_stop(n);
					pbx_voice_channel_connect(j,n);
					if(n<8)
						{
						status[n][0]=05;
						status[j][0]=06;
						}
					else if(n<12)
						{
					  	status[n][0]=31;
					  	status[j][0]=8;
					  	}
					else 
					    	{
					      	status[n][0]=92;
						status[j][0]=8;
						}
					}
				}
			break;
		case 80:
			status[j][0]=79;
			n=status[j][2];
			status[j][2]=0xff;
			pbx_voice_channel_broken(j,n);
			gjclb(n);
			tel_jishi[j]=paicha_time;
			break;
		case 81:
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			status[k][2]=0xff;
			n=status[k][1];
			pbx_if_stop_music(n);
			pbx_voice_channel_connect(k,n);
			resume_talk(status[n],status[k]);
			break;
		case 82:
			pbx_if_stop_music(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			status[j][2]=0xff;
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
			break;
		case 83:
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=status[j][1];
			status[j][2]=0xff;
			stop_ldts(k);
			n=status[k][2];
			status[k][1]=n;
			status[k][2]=0xff;
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
			break;
		case 84:
			pbx_if_ringback_tone_stop(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
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
			break;
		case 85:
			pbx_if_record_stop_play(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 87:
			send_guaji(j);
		case 91:
			pbx_if_ringback_tone_stop(j);
			send_guaji(j);
			k=status[j][1];
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
			break;
		case 92:
			k=status[j][1];
			if(k>=8)			//060308 chang
				send_guaji(j);
			else
				status[k][2]=j;
			pbx_voice_channel_broken(j,k);
			gjclb(k);
			break;
		case 93:
			send_guaji(j);
			k=status[j][1];
			status[j][2]=0xff;
			status[k][0]=19;
			break;
		case 94:
			send_guaji(j);
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
				PBX_ROW_FREE(k);
				status[k][0]=19;
				}
			break;		
		case 96:
			k=status[j][1];
			pbx_voice_channel_connect(j,k);
			status[j][0]=97;
			if(k<8)
				{
				pbx_if_ringback_tone_stop(k);
				status[k][0]=07;
				}
			else if(k<12)
				{
				if(status[k][0]==29)
					pbx_if_ringback_tone_stop(k);
				else 
					pbx_line_offhook(k);
				status[k][0]=31;
				}
			else
				{
				pbx_if_ringback_tone_stop(k);
				status[k][0]=92;
				}
			break;
		
		case 97:
			k=status[j][1];
			if(k>=8)			//060308 chang
				send_guaji(j);
			else
				status[k][2]=j;
			pbx_voice_channel_broken(j,k);
			decodingchannel_free(k);	//0728
			gjclb(k);
			break;
		case 98:
			pbx_if_stop_music(j);
			send_guaji(j);
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
			break;
		case 99:
			send_guaji(j);
			k=status[j][1];
			status[k][2]=0xff;
			n=status[k][1];
			pbx_if_stop_music(n);
			pbx_voice_channel_connect(k,n);
			resume_talk(status[n],status[k]);
			break;
		case 100:
			send_guaji(j);
			status[j][2]=0xff;
			k=status[j][1];
			stop_ldts(k);
			n=status[k][2];
			status[k][2]=0xff;
			status[k][1]=n;
			pbx_if_ringback_tone_stop(n);
			pbx_voice_channel_connect(k,n);
			if(k<8)
				{
				status[n][0]=05;
				status[k][0]=06;
				}
			else if(k<12)
				{
				status[n][0]=31;
				status[k][0]=8;
				}
			else
				{
				status[n][0]=92;
				status[k][0]=8;
				}
			break;
		case 101:
			cut_meeting(j);
			break;
		case 105:
			status[j][0]=106;
			k=status[j][1];
			pbx_if_ringback_tone_stop(k);
			pbx_voice_channel_connect(j,k);
			status[k][0]=122;
			break;
		case 106:
			send_guaji(j);
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			pbx_if_busytone_play(k);
			status[k][0]=65;
			break;
		case 107:
			pbx_if_stop_music(j);
			send_guaji(j);
			meeting_reg[0]--;
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==j)
					{
					meeting_reg[n]=0xff;
					break;
					}
				}
			break;
		case 109:
			send_guajia(j);;
			k=status[j][1];
			n=status[j][2];
			status[j][2]=0xff;
			if(status[k][1]==j)
				status[k][1]=status[k][2];
			status[k][2]=0xff;
			if(status[n][1]==j)
				status[n][1]=status[n][2];
			status[n][2]=0xff;
			status[k][0]=05;
			status[n][0]=06;
			break;
		case 110:
			decodingchannel_free(j);
			send_guaji(j);
			break;
		case 111:
			pbx_if_ringback_tone_stop(j);
			send_guaji(j);
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
			break;
		case 114:
			status[j][0]=86;
			PBX_COLUMN_FREE(k-12);
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			pbx_if_busytone_play(k);
			status[k][0]=03;
			break;
		//case 116:
		//	pbx_line_onhook(j);
		//	PBX_ROW_FREE(j);
		//	status[j][0]=25;
		//	tel_jishi[j]=2;
		//	k=status[j][1];
		//	send_guaji(k);
		//	break;
		case 117:	
			decodingchannel_free(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			k=status[j][1];
			send_guaji(k);
			break;
		case 118:	
			pbx_if_ringback_tone_stop(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			for(k=0;k<4;k++)
				{
				if(group_call[k][0]==j)
					{
					INT8U	data tel;
					group_call[k][0]=0xff;
					for(tel=1;tel<9;tel++)
						{
						n=group_call[k][tel];
						if(n<8)
							{
							group_call[k][tel]=0xff;
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
			break;
		case 119:
			decodingchannel_free(j);
			pbx_line_onhook(j);
			PBX_ROW_FREE(j);
			status[j][0]=25;
			tel_jishi[j]=2;
			break;
		//case 120:
		//	PBX_ROW_FREE(j);
		//	status[j][0]=19;
		//	k=status[j][1];
		//	send_guaji(k);
		//	break;
		//case 121:
		//	status[j][0]=68;
		//	k=status[j][1];
		//	send_guaji(k);
		//	tel_jishi[j]=paicha_time;
		//	break;
		case 122:
			status[j][0]=68;
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			pbx_if_start_music(k);
			status[k][0]=107;
			tel_jishi[j]=paicha_time;
			meeting_reg[0]++;
			for(n=1;n<21;n++)
				{
				if(meeting_reg[n]==0xff)
					{
					meeting_reg[n]=k;
					break;
					}
				}
			break;
		case 129:
			is_zhanghao(j);
			decodingchannel_free(j);
			status[j][0]=18;
			tel_jishi[j]=paicha_time;
			break;
		case 130:
			is_zhanghao(j);
			pbx_if_ringback_tone_stop(j);
			status[j][0]=18;
			tel_jishi[j]=paicha_time;
			break;
		case 131:
			PBX_ROW_FREE(j);
			status[j][0]=19;
			k=point[j]&0x0f;
			point[j]=0xff;
			DISCONNECT(j,k);
			k=status[j][1];
			if(status[k][1]==j)
				status[k][1]=status[k][2];
			status[k][2]=0xff;
			n=status[j][2];
			status[j][2]=0xff;
			if(status[n][1]==j)
				status[n][1]=status[n][2];
			status[n][2]=0xff;
			if(k<8&&n<8)
				{
				status[k][0]=05;
				status[n][0]=06;
				}
			else
				{
				if(k<8)
					status[k][0]=8;
				else if(k<12)
					status[k][0]=31;
				else
					status[k][0]=92;	
				if(n<8)
					status[n][0]=8;
				else if(n<12)
					status[n][0]=31;
				else
					status[n][0]=92;	
				}
			break;
		/*case 134:
			decodingchannel_free(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;*/
		/*case 135:
			pbx_if_ringback_tone_stop(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
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
			break;*/
		case 136:
			pbx_if_stop_ringing(j);
			stop_qudian(j);
			k=status[j][1];
			pbx_if_ringback_tone_stop(k);
			pbx_voice_channel_connect(j,k);
			if(status[k][0]==44)
				pbx_line_offhook(k);
			status[k][0]=31;
			status[k][1]=j;
			status[j][0]=8;	
			for(n=0;n<4;n++)
				{
				if(group_call[n][0]==k)	//群呼数据
					{
					INT8U data tel;
					group_call[n][0]=0xff;
					for(tel=1;tel<9;tel++)
						{
						k=group_call[n][tel];
						group_call[n][tel]=0xff;
						if(k>=8)
							break;
						if(k!=j)
							{
							pbx_if_stop_ringing(k);
							stop_qudian(k);
							PBX_ROW_FREE(k);
							status[k][0]=19;
							}
						}
					break;
					}
				}
			break;
		case 137:
			is_zhanghao(j);
			STOP_DIAL_TONE(j);
			PBX_ROW_FREE(j);
			status[j][0]=19;
			break;
		case 138:
			pbx_if_start_ringing(j);
			status[j][0]=139;
			tel_jishi[j]=20;
			break;
		case 140:
			status[j][0]=19;
			PBX_ROW_FREE(j);
			k=status[j][1];
			pbx_voice_channel_broken(j,k);
			send_guaji(k);
			break;
		default: 
			break;
		
	}		
}



