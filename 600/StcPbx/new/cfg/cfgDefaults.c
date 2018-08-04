/*
* $Id: cfgDefaults.c,v 1.7 2007/09/13 20:13:00 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

pbx_base_t			code			defaultBaseConfigs =
{
	{
		PBX_MAGIC_CHAR_1, PBX_MAGIC_CHAR_2, 
	},

	SET_BIT(TRUNK_MODE_CONSOLE, MISC_BIT_TRUNK_MODE )|
	SET_BIT(NODIAL_ACTION_AUTO, MISC_BIT_NODIAL)|
	SET_BIT(LINE2LINE_OPEN, MISC_BIT_LINE2LINE)|
	SET_BIT(TRUE, MISC_BIT_DIFFER_RINGING)|
	SET_BIT(CALLID_MODE_ON, MISC_BIT_CALL_ID)|
	SET_BIT(FALSE, MISC_BIT_GROUP_CALL)|
	SET_BIT(TRUE, MISC_BIT_SPECIAL_FIRST),

	SIGNAL_TYPE_CHINESE, //SIGNAL_TYPE_JAPANESE,	/* signal type : Japanese or Chinese */
	
	DIAL_DIGIT_ZERO, /* PSTN prefix */

	/* IP Prefix : # */
	{
		1,
		{SET_BIT(DIAL_DIGIT_SPECIAL, 4)|0x0F,},	
	},

	/* special service list */
	{
		{
			3,
			{SET_BIT(1, 4)|1, SET_BIT(DIAL_DIGIT_ZERO,4)},/* 110 */
		},
		{
			3,
			{SET_BIT(1, 4)|1, SET_BIT(9, 4)},	/* 119 */
		},
		{
			3,
			{SET_BIT(9, 4)|1, SET_BIT(1, 4)},/* 911 */
		},
		{
			3,
			{SET_BIT(1,4)|2, SET_BIT(DIAL_DIGIT_ZERO,4)},/* 120 */
		},
		{
			0,
			{SET_BIT(1, 4)|1, SET_BIT(4, 4)},	/* 114 */
		},
		{
			0,
			{SET_BIT(1, 4)|2, SET_BIT(2, 4)},	/* 122 */
		},
	},

	/* Console Phone Number : '00' */
	{
		2,
		{SET_BIT(DIAL_DIGIT_ZERO, 4)|DIAL_DIGIT_ZERO,},	
	},

	0,	/* ext Index of console */

	/*account list */
	{
		{
			SET_BIT(FALSE, ACCOUNT_BIT_FREE)|SET_BIT(FALSE, ACCOUNT_BIT_ONHOOK)|SET_BIT(EXT_AUTHOR_ALL, 0), 
			{
				4,
				{SET_BIT(8, 4)|SET_BIT(8, 0), SET_BIT(8, 4)|SET_BIT(8, 0)}, /* 8888 : account */
			},
		},
		{
			SET_BIT(FALSE, ACCOUNT_BIT_FREE)|SET_BIT(FALSE, ACCOUNT_BIT_ONHOOK)|SET_BIT(EXT_AUTHOR_ALL, 0), 
			{
				4,
				{SET_BIT(6, 4)|SET_BIT(6, 0), SET_BIT(6, 4)|SET_BIT(6, 0)}, /* 6666 : account */
			},
		},
	},

	/* system(IP) password : 77, used dialing to IP by PSTN */
	{
		2,
		{
			SET_BIT(7, 4)|SET_BIT(7, 0), SET_BIT(8, 4)|SET_BIT(8, 0), 
		}
	},

	/*line2line password : 66, used dialing to PSTN by PSTN */
	{
		2,
		{
			SET_BIT(6, 4)|SET_BIT(6, 0), SET_BIT(6, 4)|SET_BIT(6, 0), 
		}
	},

	/* night service time 20:00~08:00 */
	{
		0x14, 0x00, 0x08, 0x00 , 
	},

	/* trunk status flags : all is opened */
	/* TRUE is masked, eg. can not be used; TRUE : not masked, eg. can be used for call  */	
	SET_BIT(TRUE, 3)|SET_BIT(TRUE, 2)|SET_BIT(TRUE, 1)|SET_BIT(TRUE, 0),

	/* trunk binding group : all PSTN is in group 0 */
	{
		1, 1, 1, 1
	},

	/* trunk Virtual Consoles : virtual console of all PSTN is EXT 0  */
	{/* where 0xFF is disable */
		1, 1, 1, 1
	},

#if 0
	/* currentTime : 8 bytes */
	{
		0x11, 0x22, 
		0x33, 0x44, 
		0x55, 0x66
	},
#endif	

	/* black list */
	{
#if 0
		{
			3,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, 
				SET_BIT(1, 4)|0X0F
			},	
		},/* 801 */
		{
			4,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01},	
		},/* 8011 */
		{
			5,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, SET_BIT(2, 4)|0X0F},	
		},/* 80112 */
		{
			6,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03
			},	
		},/* 801123 */
		{
			7,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03, SET_BIT(4, 4)|0X0F
			},	
		},/* 8011234 */
		{
			12,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03, SET_BIT(4, 4)|0X05,
				SET_BIT(6,4)|0x07,  SET_BIT(8, 4)|DIAL_DIGIT_ZERO, 
			},	
		},/* 801123456780 */
#else
		{
			0,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, 
				SET_BIT(1, 4)|0X0F
			},	
		},/* 801 */
		{
			0,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01},	
		},/* 8011 */
		{
			0,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, SET_BIT(2, 4)|0X0F},	
		},/* 80112 */
		{
			0,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03
			},	
		},/* 801123 */
		{
			0,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03, SET_BIT(4, 4)|0X0F
			},	
		},/* 8011234 */
		{
			0,
			{
				SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X01, 
				SET_BIT(2, 4)|0X03, SET_BIT(4, 4)|0X05,
				SET_BIT(6,4)|0x07,  SET_BIT(8, 4)|DIAL_DIGIT_ZERO, 
			},	
		},/* 801123456780 */
#endif
	}

};

pbx_ext_base_t		code			defaultExtConfigs =
{
	/* phone number */
	{
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(1, 4)|0X0F},	
		},/* 801 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(2, 4)|0X0F},	
		},/* 802 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(3, 4)|0X0F},	
		},/* 803 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(4, 4)|0X0F},	
		},/* 804 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(5, 4)|0X0F},	
		},/* 805 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(6, 4)|0X0F},	
		},/* 806 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(7, 4)|0X0F},	
		},/* 807 */
		{
			3,
			{SET_BIT(8,4)|DIAL_DIGIT_ZERO, SET_BIT(8, 4)|0X0F},	
		},/* 808 */
	},

	/* call wait*/
	SET_BIT(TRUE,7)|SET_BIT(TRUE, 6)|SET_BIT(TRUE,5)|SET_BIT(TRUE, 4)|SET_BIT(TRUE,3)|SET_BIT(TRUE, 2)|SET_BIT(TRUE,1)|SET_BIT(TRUE, 0),	

	/* No Disturb */
	0x00,

	/* Call ID */
	0xFF,

#if WITH_EXT_DIFFER_RINGING
	/* isDifferRingFlags */
	0xFF,
#endif

	/* call length */
	{
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, 
		CALL_TIME_FOREVER, CALL_TIME_FOREVER,
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, 
		CALL_TIME_FOREVER, CALL_TIME_FOREVER,	
	},

	/* busy branch */			
	{
		PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE,
		PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE,
	},

	/* Secret branch */			
	{
		PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE,
		PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE,
	},

	/* Authorities */			
	{
		EXT_AUTHOR_ALL, EXT_AUTHOR_ALL, EXT_AUTHOR_ALL, EXT_AUTHOR_ALL,
		EXT_AUTHOR_ALL, EXT_AUTHOR_ALL, EXT_AUTHOR_ALL, EXT_AUTHOR_ALL,	
	},

	/* delayOutTime */			
	{
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER,
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER,	
	},

	/* groups , 0 : disable, others : group ID */			
	{
		SET_BIT(TRUE, EXT_GROUP_FLAG_BIT)|1, SET_BIT(TRUE, EXT_GROUP_FLAG_BIT)|1, 
		SET_BIT(TRUE, EXT_GROUP_FLAG_BIT)|1, SET_BIT(TRUE, EXT_GROUP_FLAG_BIT)|1,
		SET_BIT(FALSE, EXT_GROUP_FLAG_BIT)|2, SET_BIT(FALSE, EXT_GROUP_FLAG_BIT)|2, 
		SET_BIT(FALSE, EXT_GROUP_FLAG_BIT)|2, SET_BIT(FALSE, EXT_GROUP_FLAG_BIT)|2,	
	},

	/* dialOutMethod */			
	{
		DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO,
#if 1
		DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO, DIAL_DIGIT_ZERO,
#else
		EXT_FIRST_PSTN, 	EXT_FIRST_PSTN, 
		EXT_FIRST_IP, 	EXT_FIRST_IP,	
#endif		
	},

	/* leftTranferTimes */			
	{
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER,
		CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER, CALL_TIME_FOREVER,	
	},

};

ext_transfer_digits 		code		defaultTransferIp =
{
	EXT_TRANSFER_TYPE_IP,
	{
		18,/* length 18 */
		{/* '112131415161718191' */
			SET_BIT(1,4)|0X01, SET_BIT(2,4)|0X01, SET_BIT(3,4)|0X01, 
			SET_BIT(4,4)|0X01, SET_BIT(5,4)|0X01, 	SET_BIT(6,4)|0X01, 
			SET_BIT(7,4)|0X01, SET_BIT(8,4)|0X01, 	SET_BIT(9,4)|0X01
		},	
	}/* No. 0 */
};

ext_transfer_digits 		code		defaultTransferUnknown =
{
	EXT_TRANSFER_TYPE_UNKNOWN,
	{
		0,
		{/* No. 0 */
			SET_BIT(1,4)|0X01, SET_BIT(2,4)|0X01, SET_BIT(3,4)|0X0F
		},	
	}
};

ext_transfer_digits 		code		defaultTransferPstn =
{
	EXT_TRANSFER_TYPE_PSTN,
	{
		9,/* length 9 */
		{/* '112131415' */
			SET_BIT(1,4)|0X01, SET_BIT(2,4)|0X01, SET_BIT(3,4)|0X01, 
			SET_BIT(4,4)|0X01, SET_BIT(5,4)|0X0F
		},	
	}/* No. 0 : 112131415  */
};


pstn_param_t		code		phoneParams[2] =
{
	{/* Japanese */
#if 0		
		5*PBX_100MS,		/* Busy On Timer */
		5*PBX_100MS,		/* Busy Off Timer */
		
		10*PBX_100MS,		/* Ringback On Timer */
		20*PBX_100MS,		/* Ringback Off Timer */
#else
		10*PBX_50MS,		/* Ringer On Timer */
		20*PBX_50MS,		/* Ringer Off Timer */
		{/* tone definations */
			{/* Dial Tone , about 20 seconds for timeout */
				1,	/* step count */
				{20*PBX_50MS	/* n*100ms for every step */, 0, 0, 0}/* 2 seconds every loop */
			},
			{/* Busy Tone, about 20 seconds for timeout */
				2,	/* step count */
				{5*PBX_50MS	/* 500ms for ON */, 5*PBX_50MS/* 500ms for OFF */, 0, 0}/* 1 seconds for every loop */
			},
			{/* Ringback Tone, about 60 seconds for timeout */
				2,	/* step count */
				{10*PBX_50MS	/* 1000ms for ON */, 20*PBX_50MS/* 2000ms for OFF */, 0, 0}/* 3 seconds for every loop */
			},
			{/* CallWait Tone, about 60 seconds for timeout */
				4,	/* step count */
				{5*PBX_50MS	/* 500ms for ON */, 5*PBX_50MS/* 2000ms for OFF */, 5*PBX_50MS, 25*PBX_50MS}/* 4 seconds for every loop */
			},
			{/* Private Dial Tone, about 20 seconds for timeout */
				2,	/* step count */
				{3*PBX_50MS	/* 300ms for ON */, 2*PBX_50MS/* 200ms for OFF */, 0, 0}/* 0.5 seconds for every loop */
			},
		},
		{/* ringer definations */
			{/*Internal Ringer, about 60 seconds for timeout, same as ringback tone */
				4,	/* step count */
				{3*PBX_50MS	/* 300ms for every step */, 4*PBX_50MS, 3*PBX_50MS, 20*PBX_50MS}/* 3 seconds every loop */
			},
			{/* External Ringer, about 60 seconds for timeout, same as ringback tone */
				2,	/* step count */
				{10*PBX_50MS	/* 1000ms for ON */, 20*PBX_50MS/* 2000ms for OFF */, 0, 0}/* 3 seconds for every loop */
			},
			{/* CAR, about 3 seconds for timeout */
				2,	/* step count */
				{5*PBX_50MS	/* 5000ms for ON */, 5*PBX_50MS/* 500ms for OFF */, 0, 0}/* 1 seconds for every loop */
			},
		},
#endif		

	},
	{/* Chinese */
#if 0	
		5*PBX_100MS,		/* Busy On Timer */
		5*PBX_100MS,		/* Busy Off Timer */
		
		10*PBX_100MS,		/* Ringback On Timer */
		40*PBX_100MS,		/* Ringback Off Timer */

#else
		10*PBX_50MS,		/* Ringer On Timer */
		40*PBX_50MS,		/* Ringer Off Timer */
		{/* tone definations */
			{/* Dial Tone , about 20 seconds for timeout */
				1,	/* step count */
				{20*PBX_50MS	/* n*100ms for every step */, 0, 0, 0}/* 2 seconds every loop */
			},
			{/* Busy Tone, about 20 seconds for timeout */
				2,	/* step count */
				{5*PBX_50MS	/* 500ms for ON */, 5*PBX_50MS/* 500ms for OFF */, 0, 0}/* 1 seconds for every loop */
			},
			{/* Ringback Tone, about 60 seconds for timeout */
				2,	/* step count */
				{10*PBX_50MS	/* 1000ms for ON */, 40*PBX_50MS/* 2000ms for OFF */, 0, 0}/* 3 seconds for every loop */
			},
			{/* CallWait Tone, about 60 seconds for timeout */
				4,	/* step count */
				{5*PBX_50MS	/* 500ms for ON */, 5*PBX_50MS/* 2000ms for OFF */, 5*PBX_50MS, 25*PBX_50MS}/* 4 seconds for every loop */
			},
			{/* Private Dial Tone, about 20 seconds for timeout */
				2,	/* step count */
				{3*PBX_50MS	/* 300ms for ON */, 2*PBX_50MS/* 200ms for OFF */, 0, 0}/* 0.5 seconds for every loop */
			},
		},
		{/* ringer definations */
			{/*Internal Ringer, about 60 seconds for timeout, same as ringback tone */
				4,	/* step count */
				{3*PBX_50MS	/* 300ms for every step */, 4*PBX_50MS, 3*PBX_50MS, 40*PBX_50MS}/* 3 seconds every loop */
			},
			{/* External Ringer, about 60 seconds for timeout, same as ringback tone */
				2,	/* step count */
				{10*PBX_50MS	/* 1000ms for ON */, 40*PBX_50MS/* 2000ms for OFF */, 0, 0}/* 3 seconds for every loop */
			},
			{/* CAR, about 60 seconds for timeout */
				2,	/* step count */
				{5*PBX_50MS	/* 1000ms for ON */, 5*PBX_50MS/* 2000ms for OFF */, 0, 0}/* 1 seconds for every loop */
			},
		},
#endif
	}
};

pbx_ver_t		code				verInfo =
{
		STC_VER_MAJOR_0,
		STC_VER_MAJOR_1,

		STC_VER_MINOR_0,
		STC_VER_MINOR_1,

		{
			__DATE2__,
		},
};

