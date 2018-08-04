#include"suba_h.h"
/* C51 Control Directive */
#pragma	ot(9,speed)

//中继直拨方式 呼叫转移处理
bit	hjzy_process(INT8U  x,INT8U xdata* y)/*x主叫物理号，y话机转移目标地址*/
{
	INT8U	data z;
	bit flag;
	flag=0;
	
	if(y[1]==0xaa)
	{
		z=allot_trackb();/*分配中继返回物理号*/
		if(z!=0xff)
		{
			if(start_fahao(z,y))
			{
				pbx_line_offhook(z);
				status[z][0]=40;
				status[z][1]=x;
				status[x][1]=z;
				pbx_if_ringback_tone_play(x);
				if(x<8)
					status[x][0]=02;
				else if(x<12)
					status[x][0]=29;
				else
					status[x][0]=91;
				flag=1;
			}
			else
			{
				TRACK_SET_FREE(z);
				PBX_ROW_FREE(z);
			}
		}
	}
	else if(y[1]==0xbb)
	{
		if(x<8)
			flag=tel_target_ip(x,y);
		if(x<12)
			flag=track_target_ip(x,y);
		else
			flag=ip_target_ip(x,y);
	}
	return flag;
}
//虚拟总机方式呼叫转移处理
bit	hjzy_processx(INT8U  x,INT8U xdata* y)/*虚拟总机方式，x主叫物理号，y话机转移目标地址*/
{
	INT8U	data z;
	bit flag;
	flag=0;
	if(y[1]==0xaa)
		{
		z=allot_trackb();/*分配中继返回物理号*/
		if(z!=0xff)
			{
			if(start_fahao(z,y))
				{
				pbx_line_offhook(z);
				status[z][0]=40;
				status[z][1]=x;
				status[x][0]=44;
				status[x][1]=z;
				tel_jishi[x]=1;
				flag=1;
				}
			else
				{
				TRACK_SET_FREE(z);
				PBX_ROW_FREE(z);
				}
			}
		}
	else if(y[1]==0xbb)
		flag=track_target_ipx(x,y);
	return flag;
}

//呼叫转移处理 y是分机时的处理
bit hjzy_process1(INT8U  x,INT8U  y)/*x主叫,y被叫*/
{
	bit flag=0;
	INT8U	data z;
	flag=hj_proa(x,y);
	if(flag==0)
		{
		z=found_other_xnzj();
		if(z!=0xff)
			flag=hj_proa(x,z);
		else
			flag=hj_prob(x,y);
		}
	return flag;
}
void	is_zhanghao(INT8U x)/*是否帐号登陆，是帐号计时*/
{
	if(x==(accounts[0]&0x0f))
	{
		zhanghao_jishi[0]=200;	/*帐号计时20S*/
		accounts[0]=accounts[0]|0x08;
	}
	else if(x==(accounts[1]&0x0f))
	{
		zhanghao_jishi[1]=200;	/*帐号计时20S*/
		accounts[1]=accounts[1]|0x08;
	}
}
void	is_zhanghaob(INT8U x)/*是否帐号登陆，是注销本次登陆*/
{
	if(x==(accounts[0]&0x0f))
		accounts[0]=accounts[0]|0x8f;
	else if(x==(accounts[1]&0x0f))
		accounts[1]=accounts[1]|0x8f;
}

INT8U	is_had_zhanghao(INT8U x)/*是否帐号登陆，是返回帐号等级,激活帐号*/
{
	INT8U	data y=0xff;;
	if((accounts[0]&0x80)==0)
	{
		if(x==(accounts[0]&0x07))
		{
			accounts[0]&=0x77;
			y=accounts[0]>>4;
		}
	}
	
	if(y==0xff)
		{
		if((accounts[1]&0x80)==0)
			{
			if(x==(accounts[1]&0x07))
				{
				accounts[1]&=0x77;
				y=accounts[1]>>4;
				}
			}
		}
	return y;
}

bit	hj_proa(INT8U x,INT8U y)//呼叫处理
{
	bit flag=0;
	if(NO_DISTURB(y))
	{
		if(status[y][0]==19)
			flag=1;
		else
		{
			y=phoneBusyBranchs[y];
			if((y<8)&&(NO_DISTURB(y))&&(status[y][0]==19))
				flag=1;
		}
	}
	
	if(flag)
		{
		pbx_row_changed(y);
		PBX_ROW_BUSY(y);
		start_bella(y);
		callerid_play_start(x,y);
		status[y][1]=x;
		status[x][1]=y;
		pbx_if_ringback_tone_play(x);
		if(x<8)
			{
			status[y][0]=20;
			status[x][0]=02;
			}
		else if(x<12)
			{
			status[y][0]=21;
			status[x][0]=29;
			}
		else
			{
			status[y][0]=21;
			status[x][0]=91;
			}
		}	
	return	flag;	
}
bit	hj_proax(INT8U x,INT8U y)	//呼叫处理 虚拟总机时，中继主叫
{
	bit flag=0;
	if(NO_DISTURB(y))
		{
		if(status[y][0]==19)
			flag=1;
		else
			{
			y=phoneBusyBranchs[y];
			if((y<8)&&(NO_DISTURB(y))&&(status[y][0]==19))
				flag=1;
			}
		}
	if(flag)
		{
		pbx_row_changed(y);
		PBX_ROW_BUSY(y);
		start_bella(y);
		callerid_play_start(x,y);
		status[y][1]=x;
		status[x][1]=y;
		status[y][0]=21;
		status[x][0]=44;
		tel_jishi[x]=1;//计时0.1s
		}	
	return	flag;	
}
bit	hj_proc(INT8U x,INT8U y)//呼叫处理 拍叉转接时
{
	bit flag=0;
	if(NO_DISTURB(y))
		{
		if(status[y][0]==19)
			flag=1;
		else
			{
			y=phoneBusyBranchs[y];
			if((y<8)&&(NO_DISTURB(y))&&(status[y][0]==19))
				flag=1;
			}
		}
	if(flag)
		{
		pbx_row_changed(y);
		PBX_ROW_BUSY(y);
		start_bella(y);
		callerid_play_start(x,y);
		status[y][0]=20;
		status[y][1]=x;
		pbx_if_ringback_tone_play(x);
		status[x][0]=24;
		status[x][2]=y;
		}	
	return	flag;	
}
bit	hj_prod(INT8U x,INT8U y)//呼叫处理 三方通话时
{
	bit flag=0;
	if(NO_DISTURB(y))
		{
		if(status[y][0]==19)
			flag=1;
		else
			{
			y=phoneBusyBranchs[y];
			if((y<8)&&(NO_DISTURB(y))&&(status[y][0]==19))
				flag=1;
			}
		}
	if(flag)
		{
		pbx_row_changed(y);
		PBX_ROW_BUSY(y);
		start_bella(y);
		callerid_play_start(x,y);
		status[y][0]=20;
		status[y][1]=x;
		pbx_if_ringback_tone_play(x);
		status[x][0]=130;
		status[x][2]=y;
		}	
	return	flag;	
}
bit	hj_prom(INT8U x,INT8U y)//呼叫处理 会议时
{
	bit flag=0;
	if(NO_DISTURB(y))
	{
		if(status[y][0]==19)
			flag=1;
		else
		{
			y=phoneBusyBranchs[y];
			if((y<8)&&(NO_DISTURB(y))&&(status[y][0]==19))
				flag=1;
		}
	}
	
	if(flag)
	{
		pbx_row_changed(y);
		PBX_ROW_BUSY(y);
		start_bella(y);
		callerid_play_start(x,y);
		status[y][0]=75;
		status[y][1]=x;
		pbx_if_ringback_tone_play(x);
		status[x][0]=66;
		status[x][1]=y;
	}	
	return	flag;	
}

bit	hj_prob(INT8U x,INT8U y)/*呼叫处理 呼叫等待时*/
{
	bit flag=0;
	INT8U	data s;
	if(NO_DISTURB(y)&&callwait&(0x01<<y))
		{
		if(4<status[y][0]&&status[y][0]<10)
			{
			if(status[y][2]==0xff)
				{
				s=status[y][1];
				if(status[s][2]==0xff)
					{
					status[s][2]=status[s][0];
					if(s<8)
						status[s][0]=83;
					else if(s<12)
						status[s][0]=43;
					else
						status[s][0]=100;
					pbx_voice_channel_broken(y,s);
					start_ldts(y);
					status[y][0]=22;
					status[y][2]=x;
					pbx_if_ringback_tone_play(x);
					if(x<8)
						status[x][0]=02;
					else if(x<12)
						status[x][0]=29;
					else
						status[x][0]=91;
					status[x][1]=y;		
					flag=1;			
					}
				}
			}
		else if(status[y][0]==04)
			{
			if(status[y][2]==0xff)
				{
				STOP_DIAL_TONE(y);	/*停送拨号音*/
				start_ldts(y);
				status[y][0]=22;
				status[y][2]=x;
				pbx_if_ringback_tone_play(x);
				if(x<8)
					status[x][0]=02;
				else if(x<12)
					status[x][0]=29;
				else
					status[x][0]=91;
				status[x][1]=y;		
				flag=1;		
				}
			}
		}
	return flag;
}
void	start_ldts(INT8U x)//送来电提示音
{
	START_DIAL_TONE(x);
	tel_jishi[x]=ldts_space;
	bell_reg[x]=0;
}

void	stop_ldts(INT8U x)//停来电提示音
{
	STOP_DIAL_TONE(x);
}

 /*void	pbx_if_start_music(INT8U x)//送音乐
{
	INT8U data i,j;
	if(x<8)
		pbx_if_music_play(x, TRUE);
	else if(x<12)
		{
		i=pbx_find_free_row();
		j=pbx_find_free_column();
		if (i==0xff || j==0xff)	
			CONNECT(x,13);
		else
			{
			PBX_ROW_BUSY(i);
			PBX_COLUMN_BUSY(j);
			point[x] = (i<<4) | j;
			CONNECT(i,j);
			CONNECT(x,j);
			pbx_if_music_play(i, TRUE);
			}
		}
	else
		{
		i=pbx_find_free_row();
		if (i==0xff)
			CONNECT(15,x-12);
		else
			{
			PBX_ROW_BUSY(i);
			point[x] = (i<<4) | (x-12);
			CONNECT(i,x-12);
			pbx_if_music_play(i, TRUE);
			}
		}
}*/                  //  repaired by cz 8 11

INT8U	check_route(INT8U pdata *p)//判断是否IP路由号码
{
	INT8U data i,k,n=0xff;
	bit flag;
	k=p[1];
	flag=k&0x01;
	k=k/2;
	if(p[1]<=ip_prefix[0])
		{
		for(i=0;i<k;i++)
			{
			if(p[i+2]!=ip_prefix[i+1])
				break;
			}
		if(i==k)
			{
			if(flag)
				{
				if((p[i+2]&0xf0)==(ip_prefix[i+1]&0xf0))
				n=0;
				}
			else
				n=0;
			if(n==0)
				{
				if(p[1]<ip_prefix[0])
					n=0xaa;
				}
			}
		}
	return n;
}


void	lode_thxs(INT8U x,INT8U y)//装载通话计时时间
{
	tel_jishi[y]=phoneCallLengthes[x];
}
INT8U	found_other_xnzj()//当中继对应的虚拟总机忙时，查找其他虚拟总机
{
	INT8U data i,j;
	for(i=0;i<12;i++)
	{
		j=virtualConsoles[i];
		if(status[j][0]==19&&NO_DISTURB(j))
			break;
	}

	if(i==12)
		return 0xff;
	return j;
}

//群呼处理
bit	qunhu_pro(INT8U x,INT8U y)	/*x主叫物理号，y被叫组号*/
{
	//处理时把主叫和被叫物理号存放在group_call的空闲位置
	INT8U data i,j,k,n=8;
	INT8U	xdata *data p;
	bit flag1=0,flag2;
	if(boolGroupCall==0)
		return 0;
	for(i=0;i<8;i++)
		{
		j=phone_groups[i]&0x7f;
		flag2=(bit)(phone_groups[i]&0x80);
		if(j==y&&i!=x&&status[i][0]==19&&NO_DISTURB(i))
			{
			if(flag1==0)		//处理第一个被叫
				{
				for(k=0;k<4;k++)
					{
					if(group_call[k][0]==0xff)
						{
						group_call[k][0]=x;	//保存主叫物理号
						p=group_call[k];
						*(++p)=i;		//保存被叫物理号
						if(flag2)
						{
							start_bella(i);
							tel_jishi[i]=10;
						}
						else
							tel_jishi[i]=0;
						callerid_play_start(x,i);
						break;
						}
					}
				if(k==4)		//无空闲群呼处理空间（一般不会发生）
					return 0;
				}
			else
				{
				*++p=i;			//保存被叫物理号
				if(flag2)
				{
					start_bella(i);
					tel_jishi[i]=10;
				}
				else
					tel_jishi[i]=0;
				//bell_reg[i]=n--;	//送振铃时刻
				}
			flag1=1;
			pbx_row_changed(i);
			PBX_ROW_BUSY(i);
			status[i][0]=136;
			status[i][1]=x;
			}
		}
	return	flag1;
}

//清来电接收缓冲区

//转总机处理
void	zzj_pro(INT8U x)	/*x主叫物理号*/
{
	INT8U data k,n=0xff,i=0;
	INT8U	xdata *data ptr;
	bit flag=1;
	if(boolNoDialAction)
		{
		k=virtualConsoles[x-8];
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
					if(n==0xff||n==x)
						n=k;
					}
				}
			if(n==0xaa||n==0xbb)
				{
				if(hjzy_process(x,ptr))
					i=2;
				}
			else	//if(n!=0xff)
				{
				flag=hj_proa(x,n);
				if(flag==0)
					i=1;
				else
					i=2;
				}
			}
		else
			i=1;
		}
	if(i==1)
		{
		k=found_other_xnzj();
		if(k==0xff)
			{
			if(n!=0xff)
				{
				if(hj_prob(x,n))
					i=2;
				else
					i=0;
				}
			else	
				i=0;
			}
		else 
			{
			hj_proa(x,k);
			i=2;
			}
		}
	if(i==0)
		{
		decodingchannel_free(x);
		clr_ldbuf(x);
		pbx_line_onhook(x);
		PBX_ROW_FREE(x);
		status[x][0]=25;
		tel_jishi[x]=2;
		}
	else
		decodingchannel_free(x);
}
//拨0转总机处理
void	zzj_pro_zero(INT8U x)	/*x主叫物理号*/
{
	INT8U data k,n=0xff,i=0;
	INT8U	xdata *data ptr;
	bit flag=1;
	k=virtualConsoles[x-8];
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
				if(n==0xff||n==x)
					n=k;
				}
			}
		if(n==0xaa||n==0xbb)
			{
			if(hjzy_process(x,ptr))
				i=2;
			}
		else	//if(n!=0xff)
			{
			flag=hj_proa(x,n);
			if(flag==0)
				i=1;
			else
				i=2;
			}
		}
	else
		i=1;
	if(i==1)
		{
		k=found_other_xnzj();
		if(k==0xff)
			{
			if(n!=0xff)
				{
				if(hj_prob(x,n))
					i=2;
				else
					i=0;
				}
			else	
				i=0;
			}
		else 
			{
			hj_proa(x,k);
			i=2;
			}
		}
	if(i==0)
		{
		decodingchannel_free(x);
		clr_ldbuf(x);
		pbx_line_onhook(x);
		PBX_ROW_FREE(x);
		status[x][0]=25;
		tel_jishi[x]=2;
		}
	else
		decodingchannel_free(x);
}
//转总机处理（群呼）
void	zzj_proa(INT8U x)	/*x主叫物理号*/
{
	INT8U data k,n,i=0;
	INT8U	xdata *data ptr;
	bit flag=1;
	if(boolGroupCall&&traunkBindingExtGroup[x-8])	//群呼开，且中继绑定话机分组不为空
		{
		if(qunhu_pro(x,traunkBindingExtGroup[x-8]-1))
			status[x][0]=44;		//中继状态为44
			status[x][1]=0xff;		//用来区分是否群呼
			tel_jishi[x]=1;
			flag=0;
		}
	if(flag==1)
		{
		k=virtualConsoles[x-8];
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
					if(n==0xff||n==x)
					n=k;
					}
				}
			if(n==0xaa||n==0xbb)
				hjzy_processx(x,ptr);
			else	//if(n!=0xff)
				{
				flag=hj_proax(x,n);
				if(flag==0)
				i=1;
				}
			}
		else
			i=1;
		if(i==1)
			{
			k=found_other_xnzj();
			if(k!=0xff)
			hj_proax(x,k);
			}
		}
}
//断开与会方X与会议的连接
void	cut_meeting(INT8U x)
{
	INT8U data i,j,k,s;
	bit flag=0;
	k=meeting_reg[1];
	if(--meeting_reg[0]==1)
		{
		if(k==x)
			gjcla(k);
		else
			gjclb(k);
		i=2;
		while(meeting_reg[i]==0xff&&i<21)
			i++;
		if(i<21)
			{
			j=meeting_reg[i];
			meeting_reg[i]=0xff;
			if(j==x)
				gjcla(j);
			else
				gjclb(j);
			}
		if(k<12)
			{
			if(j<12)
				{
				DISCONNECT(k,15);
				DISCONNECT(j,15);
				}
			else
				{
				DISCONNECT(k,15);
				DISCONNECT(k,j-12);
				point[j]=0xff;
				}
			}
		else
			{
			i=point[k]>>4;
			DISCONNECT(i,j-12);
			DISCONNECT(i,k-12);
			point[k]=0xff;
			point[j]=0xff;
			}
		meeting_reg[0]=0xff;
		meeting_reg[1]=0xff;
		}
	else
		{
		if(k==x)	//主席
			{
			if(x<12)
				{
				DISCONNECT(x,15);
				flag=1;
				}
			else
				{
				i=point[x];
				point[x]=0xff;
				DISCONNECT(i>>4,x-12);
				}
			if(flag)
				{
				flag=0;
				for(j=2;j<21;j++)
					{
					k=meeting_reg[j];
					if(k<12)
						{
						flag=1;
						meeting_reg[j]=0xff;
						meeting_reg[1]=k;
						for(i=2;i<21;i++)
							{
							if(meeting_reg[i]>12&&meeting_reg[i]!=0xff)
								{
								s=meeting_reg[i];
								DISCONNECT(x,s-12);
								CONNECT(k,s-12);
								point[s]=k<<4|(s-12);
								}
							}
						break;
						}	
					}
				}
			if(flag==0)
				{
				for(j=2;j<21;j++)
					{
					k=meeting_reg[j];
					if(k!=0xff)
					meeting_reg[j]=0xff;
					meeting_reg[1]=k;
					gjcla(x);
					if(x<12)
						PBX_ROW_BUSY(x);
					}
				
				}
			else
				gjcla(x);
			}
		else
			{
			if(x<12)
				DISCONNECT(x,15);
			else
				{
				i=point[x];
				point[x]=0xff;
				DISCONNECT(i>>4,x-12);
				}
			i=2;
			while(meeting_reg[i]!=x&&i<21)
				i++;
			if(i<21)
				meeting_reg[i]=0xff;
			gjcla(x);
			}
		}
}	

//建立会议连接
void	setup_meeting()
{
	INT8U data i,j,k,s;
	j=meeting_reg[0]-1;
	s=meeting_reg[1];
	CONNECT(s,15);
	status[s][0]=69;
	for(i=2;j>0;i++)
		{
		k=meeting_reg[i];
		if(k!=0xff)
			{
			pbx_if_stop_music(k);
			j--;
			if(k<8)
				{
				CONNECT(k,15);
				status[k][0]=72;
				}
			else if(k<12)
				{
				CONNECT(k,15);
				status[k][0]=37;
				}
			else if(k<20)
				{
				CONNECT(s,k-12);
				status[k][0]=101;
				point[k]=(s<<4)|(k-12);
				}
			}
		}
}

/* 查找话机对应的帐号*/
INT8U	found_tel_zhanghao(INT8U x)
{
	INT8U data i;
	i=accounts[0]&0x07;
	if(i==x)
		return	1;
	else
	{
		i=accounts[1]&0x07;
		if(i==x)
			return	2;
		else
			return 0xff;
	}
}


