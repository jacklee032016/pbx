/*
* $Id: pbx_phone_rx_cfg.c,v 1.1 2007/05/11 20:14:30 lizhijie Exp $
*/


/* x is 6 bytes length */
BOOL	pbx_phone_cfg_ip_prefix(INT8U pdata *x)
{
	INT8U data p[4];
	p[0]=x[1]-3;/* length of IP Prefix */
	p[1]=x[3];
	p[2]=x[4];
	p[3]=x[5];
	return pbx_cfg_global_ip_prefix( p);
}

INT8U pbx_phone_cfg_ext_phone_number(INT8U x,INT8U pdata *y)
{
	INT8U data i,j,n,k;
	INT8U data p[3];

	k=*(y+1);
	*(y+1)=*y-3;
	n=compare_haoma_p(y+1);
	if(n>=8)
	{
		p[0]=*y-3;
		p[1]=*(y+2);
		p[2]=*(y+3);
		if( pbx_cfg_ext_phone_number(x, p) )
		{
			n=2;
		}
		else
		{
			n=0;
		}
	}
	else
	{
		n=1;
	}

	return n;
}

BOOL pbx_phone_cfg_ext_binding_groups(INT8U pdata *x)
{
	INT8U data j,k=0;
	k=(*x>>4)-1;
	j=( (*x&0x0f)-1)|( ( *(x+1)&0xf0 )<<3 );
	
	return pbx_cfg_ext_binding_group(k, j);
}

BOOL pbx_phone_cfg_ext_secret_ext(INT8U x, INT8U pdata* y)
{
	INT8U data i,j,k;
	k=*(y+1);
	*(y+1)=*y-3;
	j=compare_haoma_p(y+1);

	if( j<PBX_FXS_COUNT && j!=x )
		return pbx_cfg_ext_secret_ext(x, j);
	return FALSE;
}

BOOL pbx_phone_cfg_ext_busy_ext(INT8U x,INT8U pdata* y)
{
	INT8U data i,j,k;
	k=*(y+1);
	*(y+1)=*y-3;
	j=compare_haoma_p(y+1);

	if(j<PBX_FXS_COUNT && j!=x)
		return pbx_cfg_ext_busy_ext(x, j);
	return FALSE;
}


BOOL pbx_phone_cfg_special_service_number(INT8U pdata *x)
{
	INT8U data k;
	INT8U data p[4];

	k=(x[3]>>4)-1;
	p[0]=x[1]-5;
	p[1]=x[4];
	p[2]=x[5];
	p[3]=x[6];
	return pbx_cfg_special_service_number(k, p);
}


BOOL pbx_phone_cfg_night_service_time(INT8U pdata *t)
{
	INT8U data i,j,p[4];
	p[0]=*t++;
	p[1]=*t++;
	t++;
	p[2]=*t++;
	p[3]=*t++;

	return pbx_cfg_night_service_time( p);
}

INT8U pbx_phone_cfg_transfer_left(INT8U pdata* y)
{
	INT8U data length = 0;
	INT8U data p[10];
	INT8U data extIndex = *y;

	length = transfer_phone_config_data(p, y);

	return pbx_cfg_ext_transfer_left(extIndex, length, p);
}

INT8U pbx_phone_cfg_transfer_deny_callin(INT8U pdata* y)
{
	INT8U data length = 0;
	INT8U data p[10];
	INT8U data extIndex = *y;

	length = transfer_phone_config_data(p, y);
	return pbx_cfg_ext_transfer_deny_callin(extIndex, length, p);
}

INT8U pbx_phone_cfg_transfer_night_services(INT8U pdata* y)
{
	INT8U data length = 0;
	INT8U data p[10];
	INT8U data extIndex = *y;

	length = transfer_phone_config_data(p, y);
	return pbx_cfg_ext_transfer_neight_service(extIndex, length, p);
}

INT8U pbx_phone_cfg_transfer_unconditional(INT8U pdata* y)
{
	INT8U data length = 0;
	INT8U data p[10];
	INT8U data extIndex = *y;

	length = transfer_phone_config_data(p, y);
	return pbx_cfg_ext_transfer_unconditional(extIndex, length, p);
}


