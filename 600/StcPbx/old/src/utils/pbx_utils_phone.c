/*
* $Id: pbx_utils_phone.c,v 1.1 2007/05/11 20:13:52 lizhijie Exp $
*/

//话机挂机处理		
void	gjcla(INT8U x)
{
	if( x< STATE_FXO_OFFSET)
	{
		status[x][0]=19;
		PBX_ROW_FREE(x);
	}
	else if(x < STATE_IP_OFFSET)
	{
		status[x][0]=25;
		pbx_line_onhook(x);
		PBX_ROW_FREE(x);
		tel_jishi[x]=2;
	}
	else
	{
		send_guaji(x);
	}	
}

/*话机挂机处理		*/
void	gjclb(INT8U x)
{
	if(x<STATE_FXO_OFFSET)
	{
		status[x][0]=03;
		pbx_if_busytone_play(x);
	}
	else if(x<STATE_IP_OFFSET)
	{
		status[x][0]=25;
		pbx_line_onhook(x);
		PBX_ROW_FREE(x);
		tel_jishi[x]=2;
	}
	else
	{
		send_guaji(x);
	}	
}

