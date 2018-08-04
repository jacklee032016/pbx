/*
* $Id: pbx_cfg_items.c,v 1.1 2007/05/11 20:14:46 lizhijie Exp $
*/

BOOL pbx_cfg_sys_passwd(INT8U pdata *passwd)
{
	INT8U data p1[2];
	p1[0] = passwd[0];
	p1[1] = passwd[1];
	return wr24c08_multi(p1, ROM_ADDRESS_SYS_PASSWD, 2);
}

/* 0x22: trunck is virtual console: 0x23 : is direct dialing */
BOOL pbx_cfg_trunck_direct_dialing(BOOL isDirectDialing )
{
	INT8U data i;
	BOOL res;
	
	i=sign2;
	boolTrunkMode = isDirectDialing;
	WRITE_GLOBAL_MISC(res, i);
	return res;
}

/* 0x24 : No Dial Action is Auto; 0x25 : is broken */
BOOL pbx_cfg_no_dial_action_is_auto(BOOL isAuto)
{
	INT8U data i;
	BOOL res;
	
	i=sign2;
	boolNoDialAction = isAuto;
	WRITE_GLOBAL_MISC(res, i);
	return res;
}

/* 0x26 : assign value as 0; 0x27 : 1 */
BOOL pbx_cfg_forbid_line2line(BOOL isForbid)
{
	INT8U data i;
	BOOL res;
	
	i=sign2;
	boolLine2Line = isForbid;
	WRITE_GLOBAL_MISC(res, i);

	return res;
}


/*0x28: disable; 0x29 : enable*/
BOOL pbx_cfg_differ_ringing(BOOL isTrue)		//内外线区分振铃
{
	INT8U data i;
	BOOL res;
	
	i=sign2;
	boolDifferRinging = isTrue;
	WRITE_GLOBAL_MISC(res, i);

	return res;
}

/*0x2a : FSK; 0x2b : DTMF */
BOOL pbx_cfg_callId_mode_fsk(BOOL isTrue)
{
	INT8U data i;
	BOOL res;
	
	i=sign2;
	boolCallIdMode=isTrue;
	WRITE_GLOBAL_MISC(res, i);

	return res;
}


/* 0x58 : Open Group Call(TRUE); 0x59 : close group call(FALSE) */
BOOL pbx_cfg_group_call_opened(BOOL isOpen)
{
	INT8U data i;
	BOOL res;
	i=sign2;
	
	boolGroupCall= isOpen;
	WRITE_GLOBAL_MISC(res, i);

	return res;
}

/* 0x50 : sepcial service first is enabled; 0x51 : is disabled */
BOOL pbx_cfg_special_service_first(BOOL isEnabled)		//设置紧急电话优先
{
	INT8U data old;
	BOOL res;
	
	old = sign2;
	boolSpecialFirst= isEnabled;
	WRITE_GLOBAL_MISC(res, old);

	return res;
}

BOOL pbx_cfg_special_service_number(INT8U extIndex, INT8U xdata *p)
{
	INT8U data i=0;

	if(p==NULL)
	{
		if(wr24c08_single(0xff, ROM_ADDRESS_SPECIAL_SERVICE+extIndex*4))
		{
			specialServices[extIndex][0]=0xff;
			return 1;
		}
		else
			return 0;
	}
	
	if(wr24c08_multi(p,ROM_ADDRESS_SPECIAL_SERVICE+extIndex*4,4))
	{
		for(i=0;i<4;i++)
			specialServices[extIndex][i]=p[i];
		return	1;
	}
	else
		return 0;
}

/* 0x5f */
BOOL pbx_cfg_night_service_time(INT8U pdata *p)
{
	INT8U data i;
	
	if(wr24c08_multi(p,ROM_ADDRESS_NIGHT_SERVICE_TIME,4))
	{
		for(i=0;i<4;i++)
			nightServiceTime[i]=p[i];
		return 1;
	}
	else
		return 0;
}

/* 0x31 and 0x32(cancel PSTN prefix : write 0 into ROM )*/
BOOL pbx_cfg_global_pstn_prefix(INT8U _psntPrefix/* 0 : cancel*/)
{
	if(wr24c08_single(_psntPrefix,ROM_ADDRESS_PSTN_PREFIX))
	{
		pstn_prefix=_psntPrefix;
		return 1;
	}
	else 
		return 0;
}

/* 0x2d : authority level of account; 0x2e : close an account 
* when autohority is 0xff, close this account */
BOOL pbx_cfg_global_account_authority_level(INT8U accoutIndex, INT8U authority)	//设置帐号等级
{
	if(authority!= 0xff)
		authority=authority<<4;
	if(wr24c08_single(authority,ROM_ADDRESS_ACCOUNT+accoutIndex*3))
	{
		accounts[accoutIndex]=authority|(accounts[accoutIndex]&0x8f);
		return 1;
	}
	else 
		return 0;
}

/* called by PHONE and UART interface : p must be 4 bytes length array */
BOOL pbx_cfg_global_ip_prefix(INT8U xdata *p)
{
	INT8U data length =0;
	if(wr24c08_multi(p,ROM_ADDRESS_IP_PREFIX,4))
	{
		for(length =0; length<4; length++)
			ip_prefix[length]=p[length];
		return	1;
	}
	else
		return 0;
}


/* 0x20 : disable trunk; 0x21 : enabled */
BOOL pbx_cfg_line_open(INT8U lineIndex, BOOL isEnabled)
{
	INT8U data old;
	
	old = track_open;
	if(isEnabled)
		track_open=track_open|(0x80>>(lineIndex));
	else	
		track_open=track_open&~(0x80>>(lineIndex));
	
	if(wr24c08_single(track_open,track_open1))
		return 1;
	else 
	{
		track_open = old;
		return 0;
	}	
}

/* 0x2c : virtual console for PSTN line */
BOOL pbx_cfg_line_virtual_console(INT8U lineIndex, INT8U consoleId)	//设置虚拟总机
{
	if(wr24c08_single(consoleId,xnzj_rom+lineIndex))
	{
		virtualConsoles[lineIndex] = consoleId;
		return 1;
	}
	else 
		return 0;
}

/* 0x5e */
BOOL pbx_cfg_line_binding_group(INT8U x/*trunk no*/,INT8U y/*EXT index*/)
{
	if(wr24c08_single(y,track_telgroup_rom+x))
	{
		traunkBindingExtGroup[x]=y;
		return 1;
	}
	else
		return 0;
}

/* 0x30 : call pbxCurrentTime length of an EXT; when timeLength is 0, cancel time length limit */
BOOL pbx_cfg_ext_call_length(INT8U extIndex, INT8U timeLength/* unit of minutes */)	//设置通话限时
{
	unsigned int data j;
	INT8U	data p[2];

	j=(unsigned int)timeLength*600;	
	p[0]=(INT8U)(j&0x00ff);
	p[1]=(INT8U)(j>>8);
	
	if(wr24c08_multi(p,ROM_ADDRESS_CALL_LENGTH_LIMIT+extIndex*16,2))
	{
		phoneCallLengthes[extIndex]=j;
		return 1;
	}
	else 
		return 0;
}


/* 0x35 : close CALLID of EXT; 0x36 : open it */
BOOL pbx_cfg_ext_callId_status(INT8U extIndex, BOOL isEnabled)
{
	INT8U data old;
	old = isCallerIdOn;
	if(isEnabled)
		isCallerIdOn=isCallerIdOn|(0x80>>(extIndex));	/* when config by PHONE, must minus 1 */
	else
		isCallerIdOn=isCallerIdOn&~(0x80>>(extIndex));
	
	if(wr24c08_single(isCallerIdOn, ROM_ADDRESS_PHONE_CALLID))
		return 1;
	else 
	{
		isCallerIdOn = old;
		return 0;
	}	
}

/* 0x4e : open CallWait; 0x4f : Close CallWait */
BOOL pbx_cfg_ext_callwait_open(INT8U extIndex, BOOL isEnable)		//开通呼叫等待
{
	INT8U	data old;
	old = callwait;
	if(isEnable)
		callwait=callwait|(0x01<<extIndex);
	else
		callwait=callwait&~(0x01<<extIndex);
		
	if(wr24c08_single(callwait, ROM_ADDRESS_PHONE_CALLWAIT) )
		return 1;
	else
	{
		callwait = old;
		return 0;
	}
}


/* 0x5c : Open; 0x5d : close */
BOOL pbx_cfg_ext_notdisturb_opened(INT8U extIndex, BOOL isEnable/**/ )
{
	INT8U	data i;
	
	i=not_disturb;
	if(isEnable)/* 0x5c */
		not_disturb=not_disturb&~(0x01<<extIndex);
	else
		not_disturb=not_disturb|(0x01<<extIndex);
		
	if(wr24c08_single(not_disturb,ROM_ADDRESS_PHONE_NOT_DISTURB))
		return 1;
	else
	{
		not_disturb=i;
		return 0;
	}
}


/* 0x2f : EXT authority level */
BOOL pbx_cfg_ext_authority_level(INT8U extIndex, INT8U authorLevel)	//设置分机等级
{
	if(wr24c08_single(authorLevel,tel_qx+extIndex*16))
	{
		phoneAuthorities[extIndex] = authorLevel;
		return 1;
	}
	else 
		return 0;
}

/* 0x3e */
BOOL pbx_cfg_ext_delay_to_lines_time(INT8U x/*EXT index*/, INT8U y/*unit is seconds, 0~9, 0 is disable*/)
{
	if(wr24c08_single(y*10, delay_out_rom+x*16))
	{
		delay_out[x]=y*10;
		return 1;
	}
	else
		return 0;
}

BOOL pbx_cfg_ext_binding_group(INT8U extIndex, INT8U groupId)
{
	if(wr24c08_single(groupId, ROM_ADDRESS_PHONE_GROUP+extIndex))
	{
		phone_groups[extIndex] = groupId;
		return 1;
	}
	else
		return 0;
}

/* 0x39 : Secret EXT; 0x3a : close it (when referExt is 0xff) )*/
BOOL pbx_cfg_ext_secret_ext(INT8U extIndex,INT8U referExt)	//设置秘书分机
{
	if(wr24c08_single(referExt,ROM_ADDRESS_SECRET_TRANSFER+extIndex*16))
	{
		phoneSecretBranchs[extIndex] = referExt;
		return 1;
	}
	else
		return 0;
}


/* 0x3c: disable Busy Transfer; 0x3b : Busy Transfer  */
BOOL pbx_cfg_ext_busy_ext(INT8U extIndex, INT8U referExt/* 0xFF is null, eg. cancel busy transfer*/)
{
	if(wr24c08_single(referExt,ROM_ADDRESS_BUSY_TRANSFER+extIndex*16))
	{
		phoneBusyBranchs[extIndex]= referExt;
		return 1;
	}
	else
		return 0;
}


/* 0x41: normal setup; 0x42 : PSTN first, store 0xff-x(may be 0xfe) 
* mode : 0XFF, pstn first; 0xFe, IP prefix
*/
BOOL pbx_cfg_ext_dialout_mode(INT8U extIndex, INT8U mode )
{
	if(wr24c08_single(mode,ROM_ADDRESS_PHONE_DIALOUT_MODE+extIndex))
	{
		phoneDialoutModes[extIndex] = mode;
		return 1;
	}
	else
		return	0;
}

/* 0x57 : left transfer time */
BOOL pbx_cfg_ext_left_transfer_time(INT8U extIndex, INT8U length )	//设置离位转移时间
{
	if(wr24c08_single(length, ROM_ADDRESS_LEFT_TRANSFER_TIME+extIndex))
	{
		phoneLeftTranferTimes[extIndex] = length;
		return 1;
	}
	else
		return 0;
}


BOOL pbx_cfg_ext_phone_number(INT8U extIndex, INT8U data *p)
{
	if(wr24c08_multi(p, ROM_ADDRESS_EXT_PHONE_NUMBER+extIndex*16, 3))
	{
		phonePhoneNumbers[extIndex][0] = p[0];
		phonePhoneNumbers[extIndex][1] = p[1];
		phonePhoneNumbers[extIndex][2] = p[2];
		return 1;
	}
	else
		return	0;
}

BOOL pbx_cfg_ext_transfer_left(INT8U extIndex, int length, INT8U pdata *p )
{
	INT8U data i;

	if(length ==0 || p==NULL)
	{
		if(wr24c08_single(0xff,ROM_ADDRESS_LEFT_TRANSFER+extIndex*16+1))
		{
			phoneLeftTransfers[extIndex][1]=0xff;
			return 1;
		}
		else
			return 0;
	}

	if(wr24c08_multi(p,ROM_ADDRESS_LEFT_TRANSFER+extIndex*16, length) )
	{
		for(i=0;i<length;i++)
			phoneLeftTransfers[extIndex][i]=p[i];
		return 1;
	}
	return 0;
}

BOOL pbx_cfg_ext_transfer_unconditional(INT8U extIndex, int length, INT8U pdata *p)
{
	INT8U data i;

	if(length ==0 || p==NULL)
	{
		if(wr24c08_single(0xff,ROM_ADDRESS_UNCONDITIONAL_TRANSFER+extIndex*16+1))
		{
			phoneUnconditionalTransfers[extIndex][1]=0xff;
			return 1;
		}
		else
			return 0;
	}

	if(wr24c08_multi(p,ROM_ADDRESS_UNCONDITIONAL_TRANSFER+extIndex*16, length))
	{
		for(i=0;i<length ;i++)
			phoneUnconditionalTransfers[extIndex][i]=p[i];
		return 1;
	}
	return 0;
}

BOOL pbx_cfg_ext_transfer_neight_service(INT8U extIndex, int length, INT8U pdata *p)
{
	INT8U data i;

	if(length ==0 || p==NULL)
	{
		if(wr24c08_single(0xff,ROM_ADDRESS_NIGHT_SERVICE+extIndex*16+1))
		{
			phoneNightServices[extIndex][1]=0xff;
			return 1;
		}
		else
			return 0;
	}
	
	if(wr24c08_multi(p, ROM_ADDRESS_NIGHT_SERVICE+extIndex*16, length))
	{
		for(i=0; i<length; i++)
			phoneNightServices[extIndex][i]=p[i];
		return 1;
	}
	return 0;
}

BOOL pbx_cfg_ext_transfer_deny_callin(INT8U extIndex, int length, INT8U pdata *p)
{
	INT8U data i;

	if(length ==0 || p==NULL)
	{
		if(wr24c08_single(0xff,ROM_ADDRESS_DENY_CALLIN+extIndex*16+1))
		{
			phoneDenyCallInBuf[extIndex][1]=0xff;
			return 1;
		}
		else
			return 0;
	}
	
	if(wr24c08_multi(p,ROM_ADDRESS_DENY_CALLIN+extIndex*16, length))
	{
		for(i=0; i<length; i++)
			phoneDenyCallInBuf[extIndex][i]=p[i];
		return 1;
	}	
	return 0;
}


