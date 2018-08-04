/*
* $Id: pbx_cfg_utils.c,v 1.1 2007/05/11 20:14:46 lizhijie Exp $
*/


/* compare whether this phone number is a IP dialing. TRUE : yes
* call by Phone Config IF and Uart Config IF
* k : length of compared phone number 
*/
BOOL check_route_when_cfg_transfer(INT8U dataLength, INT8U xdata *p)
{
	INT8U data i, length;
	BOOL	flag;
	
	if(dataLength < ip_prefix[0])
		return FALSE;
	
	p+=3;
	
	length = ip_prefix[0];
	flag=(BOOL)( length&0x01);	/*odd flags */
	length = length/2;
	
	for(i=0;i<length;i++)
	{
		if(p[i]!= ip_prefix[i+1])
			break;
	}
	
	if(i== length)
	{
		if(flag)
		{
			if((p[i]&0xf0)==(ip_prefix[i+1]&0xf0))
				return TRUE;
		}
		else
			return TRUE;
	}

	return FALSE;
}

/* return the length coded in dest which length is 10; raw data is come from Phone cfg and Uart cfg IF */
INT8U transfer_uart_config_data(INT8U data *dest, INT8U xdata *rawdata)
{
	INT8U data i,j,k,n,length;
	BOOL isOdd;

	if(rawdata[4]==0xff)
	{/* it is refer to a local EXT ? */
		dest[0]=1;
		dest[1]=rawdata[3]>>4;
		length = 2;
	}
	else	
	{
		i=3;
		j=0;
		while(rawdata[i]!=0xff&&i<15)
		{
			i++;/* length of used bytes */
			j++;/* 3 digits in one byte */
			j++;
		}
		
		if((rawdata[i-1]&0x0f)==0x0f)/* odd number */
			j--;
		
		isOdd = check_route_when_cfg_transfer(j, rawdata);
		if(isOdd)
		{/* IP is this Tranfer dest */
			dest[0]=j-ip_prefix[0];
			dest[1]= EXT_TRANSFER_TYPE_PSTN; /* 0xbb;	ip电话标志*/
			
			isOdd=(BOOL)(ip_prefix[0]&0x01);
			k=dest[0]/2;
			n=ip_prefix[0]/2+3;
			
			if(isOdd)
			{	
				isOdd=(BOOL)(dest[0]&0x01);
				for(i=0;i<k;i++)
					dest[i+2]=rawdata[i+n]<<4|rawdata[i+n+1]>>4;
				
				if(isOdd)
					dest[i+2]=(rawdata[i+n]<<4)|0x0f;	
			}
			else	
			{
				isOdd=(BOOL)(dest[0]&0x01);
				for(i=0;i<k;i++)
					dest[i+2] = rawdata[i+n];	
				if(isOdd)
					dest[i+2] = rawdata[i+n]|0x0f;	
			}
		}
		else
		{
			dest[0]= j;
			dest[1]= EXT_TRANSFER_TYPE_PSTN;//0xaa;
			isOdd=(BOOL)(j&0x01);
			k=j/2;
			for(i=0;i<k;i++)
				dest[i+2]= rawdata[i+3];
			if(isOdd)
				dest[i+2]= rawdata[i+3]|0x0f;
		}
		
		j=(j+1)/2+2;
		
		length = j;
	}

	return length;
}

INT8U transfer_phone_config_data(INT8U data *dest, INT8U pdata *rawdata)
{
	INT8U data i,j,k,n,x;
	INT8U data length = 0;
	BOOL	flag,flag1;
	
	x=*rawdata;
	k=*(rawdata+1);
	
	if(k<=7)
	{
		k=*(rawdata+2);
		*(rawdata+2)=*(rawdata+1)-3;
		j=compare_haoma_p(rawdata+2);
		if(j<8&&j!=x)
		{/* local EXT */
			dest[0]=1;
			dest[1]=j;
			
			length = 2;
		}
	}
	else	
	{
		flag1=check_route_when_cfg_transfer(k-3,(INT8U xdata *)rawdata);
		if(flag1)
		{/* IP transfer */
			dest[0]=k-3-ip_prefix[0];
			dest[1] = EXT_TRANSFER_TYPE_IP;//0xbb;		//ip电话标志
			flag=(BOOL)(ip_prefix[0]&0x01);
			k=dest[0]/2;
			n=ip_prefix[0]/2+3;
			
			if(flag)
			{	
				flag=(BOOL)(dest[0]&0x01);
				for(j=0;j<k;j++)
					dest[j+2]=rawdata[j+n]<<4|rawdata[j+n+1]>>4;					
				if(flag)
					dest[j+2]=(rawdata[j+n]<<4)|0x0f;	
			}
			else	
			{
				flag=(BOOL)(dest[0]&0x01);
				for(j=0;j<k;j++)
					dest[j+2]=rawdata[j+n];	
				if(flag)
					dest[j+2]=rawdata[j+n]|0x0f;	
			}
		}
		
		if(flag1==0)
		{
			dest[0]=k-3;
			dest[1]=EXT_TRANSFER_TYPE_PSTN;//0xaa;
		}
		
		flag=(BOOL)((rawdata[1]-3)&0x01);
		k=(rawdata[1]-3)/2;
		for(j=0;j<k;j++)
		{
			if(flag1==0)
				dest[j+2]=rawdata[j+3];
		}
		
		if(flag)
		{
			if(flag1==0)
				dest[j+2]=rawdata[j+3]|0x0f;
			j++;
		}
		k=(dest[0]+1)/2+2;
		length = k;
		
	}

	return length;
}


