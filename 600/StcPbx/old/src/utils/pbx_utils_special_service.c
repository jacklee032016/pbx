/*
* $Id: pbx_utils_special_service.c,v 1.1 2007/05/11 20:13:52 lizhijie Exp $
*/

INT8U	check_tefu(INT8U pdata *p)//�ж�������ĺ����Ƿ����ط�����
{	/*n=0aa,�����ǣ�n=0,��,n=0ff����*/
	INT8U data i,j,k,n=0xff;
	bit flag;
	k=p[1];	//�������
	flag=k&0x01;
	k=k/2;
	for(i=0;i<6;i++)
		{
		if(p[1]<=specialServices[i][0]&&specialServices[i][0]!=0xff)
			{
			for(j=0;j<k;j++)
				{
				if(p[j+2]!=specialServices[i][j+1])
					break;
				}
			if(j==k)
				{
				if(flag)
					{
					if((p[j+2]&0xf0)==(specialServices[i][j+1]&0xf0))
					n=0;
					}
				else
					n=0;
				if(n==0)
					{
					if(p[1]<specialServices[i][0])
						n=0xaa;
					break;
					}
				}
			}
		}
	return n;
}
INT8U	check_tefu1(INT8U xdata *p)//�ж��Ƿ����ط�����check_tefu��ڲ�����һ��
{	/*n=0aa,�����ǣ�n=0,��,n=0ff����*/
	INT8U data i,j,k,n=0xff;
	bit flag;
	k=p[0];
	flag=k&0x01;
	k=k/2;
	for(i=0;i<6;i++)
		{
		if(p[0]<=specialServices[i][0]&&specialServices[i][0]!=0xff)
			{
			for(j=0;j<k;j++)
				{
				if(p[j+2]!=specialServices[i][j+1])
					break;
				}
			if(j==k)
				{
				if(flag)
					{
					if((p[j+2]&0xf0)==(specialServices[i][j+1]&0xf0))
					n=0;
					}
				else
					n=0;
				if(n==0)
					{
					if(p[0]<specialServices[i][0])
						n=0xaa;
					break;
					}
				}
			}
		}
	return n;
}

//�ط����д���
INT8U	tefu_process(INT8U pdata *p)	/*n=0,ϵͳæ��n=1,����ʧ��,n=2����ɹ�*/
{
	INT8U data i,k,n=1;
	bit flag=0;
	if(dtmfCodingChannelsFree&0x0f)	//�Ƿ��п��е�ȥ�绺����
		{
		if(columnFrees)
			flag=1;
		else
			{
			if(boolSpecialFirst)
				{
				i=cut_ip();
				if(i!=0xff)
					{
					PBX_COLUMN_FREE(i);
					flag=1;
					}
				else
					n=0;
				}
			}
		if(flag)
			{
			i=allot_trackb();
			if(i==0xff)	//�Ƿ��п����м� û��i=0xff
				{
				if(boolSpecialFirst)
					{
					k=25;
					while(k!=0xff)//ѭ���ж��Ƿ��д���״̬25��36��26���м�
						{
						for(i=8;i<12;i++)
							{
							if(track_open&(0x80>>(i-8)))//ȷ���м��Ǵ򿪵�
								{
								if(status[i][0]==k)
									{
									if(k==25||k==36)
										{
										n=0;
										k=0xff;
										}
									else if(k==26)
										{
										pbx_line_offhook(i);
										status[i][0]=36;	//��ʱ�Ͽ��м�
										tel_jishi[i]=2;
										k=0xff;
										n=0;
										}
									break;
									}
								}
							}
						if(k!=0xff)
							{	//ѭ���ж��Ƿ��д���״̬25��36��26���м�
							if(k==25)	
								k=36;	
							else if(k==36)
								k=26;
							else if(k==26)
								{
								k=0xff;
								flag=0;
								}
							}
						}
					if(flag==0)
						{
						for(i=8;i<12;i++)
							{
							if(track_open&(0x80>>(i-8)))
								{
								if(status[i][0]==40)
									{
									for(k=0;k<4;k++)
										{
										if((qudian[k][1]&0x0f)==i)
											{
											if(check_tefu1(&qudian[k][2]))
												{
												cutresult=cutresult|(0x10<<(i-8));	/*ǿ�ƶϿ��м�*/
												n=0;
												}
											break;
											}
										}
									}
								else if(status[i][0]==44)
									{
									pbx_line_offhook(i);
									status[i][0]=36;
									tel_jishi[i]=2;
									k=status[i][1];
									if(k<8)
										{
										pbx_if_stop_ringing(k);
										stop_qudian(k);
										PBX_ROW_FREE(k);
										status[k][0]=19;
										}
									else if(k<12)
										{
										pbx_line_onhook(k);
										stop_fahao(k);
										PBX_ROW_FREE(k);
										status[k][0]=25;
										tel_jishi[k]=mask_track_time;
										n=0;
										}
									else
										send_guaji(k);
									}
								else 
									{
									cutresult=cutresult|(0x10<<(i-8));	/*ǿ�ƶϿ��м�*/
									n=0;
									}
								break;
								}
							}
						}
					}
				}
			else
				{
				k=p[0];	//���������
				p[0]=p[1];//������������ֽڣ�����start_fahao��
				if(start_fahao(i,(INT8U xdata *)p))
					{
					p[0]=k;//�ָ���������ŵ�����ͨ�����ֽڣ��Ͽ�����ͨ��ʱ��
					pbx_line_offhook(i);
					status[i][0]=40;
					status[i][1]=k;
					status[k][0]=02;
					status[k][1]=i;
					pbx_if_ringback_tone_play(k);
					n=2;
					}	
				else
					{
					p[0]=k;		
					TRACK_SET_FREE(i);
					PBX_ROW_FREE(i);
					}
				}
			}
		}
	else
		n=0;
	return n;
}

