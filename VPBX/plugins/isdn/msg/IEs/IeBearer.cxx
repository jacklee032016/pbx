/*
* $Id: IeBearer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IeBearer.hxx"
#include "IsdnParserMode.hxx"
#include "cpLog.h"
#include "l3ins.h"

using namespace Assist;

IeBearer::IeBearer( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode BEARER IE", __FILE__, __LINE__, res );
	}
}


IeBearer::IeBearer(int _codec, int _capacity, int _mode)
{
	codec = validateCodec(_codec);
	capability = validateCapabilty(_capacity);
	mode = validateMode( _mode);

	rate = -1;
	multi = -1;
	user = -1;
}

IeBearer::IeBearer( const IeBearer& src)
{
	codec = src.codec;
	capability = src.capability;
	mode = src.mode;

	rate = -1;
	multi = -1;
	user = -1;
}

/* construct a default bearer IE, eg. bearer used behild PBX */
IeBearer::IeBearer( )
{
	codec = BEARER_INFO_CODING_TTC;
	capability = BEARER_INFO_CAPABILITY_31KHZ_AUDIO;
	mode = BEARER_INFO_MODE_CIRCUIT;

	rate = BEARER_INFO_RATE_64KPS;
	multi = -1;
	user = BEARER_INFO_L1_PROTOCOL_G711_ULAW;
}


const IeBearer& IeBearer::operator =(const IeBearer& src)
{
	if (&src != this)
	{
	}
	return (*this);
}

bool IeBearer::operator ==( const IeBearer& src) const
{
	return false;
}

IeBearer* IeBearer::duplicate() const
{
	return new IeBearer(*this);
}


bool IeBearer::compareIsdnIE(IsdnIE* msg) const
{
    IeBearer* otherMsg = dynamic_cast<IeBearer*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

int IeBearer::validateCodec(int _codec)
{
	return BEARER_INFO_CODING_TTC;
}

int IeBearer::validateCapabilty(int _cap)
{
	if(_cap == -1)
		return BEARER_INFO_CAPABILITY_31KHZ_AUDIO;
	else if( _cap == 0)
		return BEARER_INFO_CAPABILITY_SPEECH;
	else if(_cap ==8 )
		return BEARER_INFO_CAPABILITY_DIGITAL_UNRESTRICTED;
//	else if(_cap == 9 )
//		return BEARER_INFO_CAPABILITY_DIGITAL_RESTRICTED;
//	else if(_cap == 17)
//		return BEARER_INFO_CAPABILITY_DIGITAL_UNRESTRICTED_TONES;
//	else if(_cap == 24 )
//		return BEARER_INFO_CAPABILITY_VIDEO;
	else/* 11 */
		return BEARER_INFO_CAPABILITY_31KHZ_AUDIO;
}

int IeBearer::validateMode(int _mode)
{
	if(_mode == 2)
		return BEARER_INFO_MODE_PACKET;
	else
		return BEARER_INFO_MODE_CIRCUIT;
}

int IeBearer::validateUser(int _user)
{
	if(_user== -1)
		// codec = INFO_CODEC_ALAW;
		codec = BEARER_INFO_CODING_TTC;
	else if(_user == BEARER_INFO_L1_PROTOCOL_X30)
		codec = BEARER_INFO_CODING_TTC;//INFO_CODEC_V110;
	else if(_user == BEARER_INFO_L1_PROTOCOL_G711_ULAW )
		codec = BEARER_INFO_CODING_TTC;
/*
	else if(_user == 3 )
		codec = INFO_CODEC_ALAW;
	else if(_user == 4 )
		codec = INFO_CODEC_G721;
 	else if(_user == 5 )
		codec = INFO_CODEC_H221H242;
	else if(_user == 7 )
		codec = INFO_CODEC_NONCCITT;
	else if(_user == 8 )
		codec = INFO_CODEC_V120;
*/
	else if(_user == BEARER_INFO_L1_PROTOCOL_X31 )
		codec = BEARER_INFO_CODING_TTC;
		
	else /* 2 */
		codec = BEARER_INFO_CODING_TTC;

	return 0;
}

int IeBearer::validateMulti(int _multi)
{
	return _multi;
}

int IeBearer::validateRate(int _rate)
{
	return _rate;
}

void IeBearer::setUser(int _user)
{
	user = validateUser( _user);
};

void IeBearer::setMulti(int _multi)
{
	multi = validateMulti( _multi) ;
};

void IeBearer::setRate(int _rate)
{
	rate = validateRate(_rate);
};

unsigned char *IeBearer::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int sp = 0;	/*start position */
	int tLen = 0;	/* total Length */	

	if (codec<0 || codec>3)
	{
		cpLog(LOG_ERR, "coding(%d) is out of range.\n", codec);
		return 0;
	}
	
	if (capability<0 || capability>31)
	{
		cpLog(LOG_ERR, "capability(%d) is out of range.\n", capability);
		return 0;
	}

	if (mode<0 || mode>3)
	{
		cpLog(LOG_ERR, "mode(%d) is out of range.\n", mode);
		return 0;
	}
	
	if (rate<0 || rate>31)
	{
		cpLog(LOG_ERR, "rate(%d) is out of range.\n", rate);
		return 0;
	}
	
	if (multi>127)
	{
		cpLog(LOG_ERR, "multi(%d) is out of range.\n", multi);
		return 0;
	}
	if (user>31)
	{
		cpLog(LOG_ERR, "user L1(%d) is out of range.\n", rate);
		return 0;
	}
	if (rate!=24 && multi>=0)
	{
		cpLog(LOG_ERR, "multi(%d) is only possible if rate(%d) would be 24.\n", multi, rate);
		multi = -1;
	}

	len = 2 + (multi>=0) + (user>=0);
	tLen = len+1;
	if( ntMode == ISDN_DEV_TYPE_USER )
		tLen++;
	p = msg_put(msg, tLen );

#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->bearer_capability = p - ( char *)qi - sizeof(Q931_info_t);
#endif

	if(ntMode == ISDN_DEV_TYPE_USER )
	{
		p[sp++] = IE_BEARER;
	}
	p[sp++] = len;
	p[sp++] = 0x80 + (codec<<5) + capability;
	p[sp++] = 0x80 + (mode<<5) + rate;

	if (multi >= 0)
		p[sp++] = 0x80 + multi;
	if (user >= 0)
		p[sp+(multi>=0)] = 0xa0 + user;
	
	return p;
}


int IeBearer::decode(const unsigned char *p)
{
	int len;
	if (!p)
		return ERROR_IE_IS_NULL;

	len = p[0];

	if ( len < 2)
	{
		cpLog(LOG_ERR, "Bearer IE too short (%d).\n", len );
		return ERROR_IE_TOO_SHORT;
	}

	codec = (p[1]&0x60) >> 5;
	capability = p[1] & 0x1f;		/* last 5 bits */
	
	if (len>=2)
	{
		mode = (p[2]&0x60) >> 5;
		rate = p[2] & 0x1f;
	}
	
	if (len>=3 && rate==0x18)
	{
		multi = p[3] & 0x7f;
		if (len >= 4)
			user = p[4] & 0x1f;
	} 
	else
	{
		if (len >=3)
			user = p[3] & 0x1f;
	}

	return AS_OK;
}

#if WITH_DEBUG
const char *IeBearer::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);

	length += sprintf( p+ length, "Bearer IE info :");
	length += sprintf( p+ length, "\tCodec : %s", (codec==BEARER_INFO_CODING_TTC)?"TTC":"Not offered");
	length += sprintf( p+ length, "\tCapacity : %s", (capability==BEARER_INFO_CAPABILITY_31KHZ_AUDIO)?"3.1KHz Audio" :"Other");

	length += sprintf( p+ length, "\tMode : %s", (mode==BEARER_INFO_MODE_CIRCUIT)?"Circuit":"Packet");
	length += sprintf( p+ length, "\tRate : %s", (rate==BEARER_INFO_RATE_64KPS)?"64Kps":"Not support");
	
	length += sprintf( p+ length, "\tMulti : %s", (multi==0x01)?"OK":"Not support");
	length += sprintf( p+ length, "\tUser : %s", (user==BEARER_INFO_L1_PROTOCOL_G711_ULAW)?"G711-U":"Other not support");

	return p;
}
#endif		

