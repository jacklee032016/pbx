/*
* $Id: IeCallingPN.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "IeCallingPN.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeCallingPN::IeCallingPN( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IePN(p)
{
#if 0
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode CALLING_PN IE", __FILE__, __LINE__, res );
	}
#endif	
}

IeCallingPN::IeCallingPN(int _type, int _plan, const unsigned char *_number)
	:IePN(_type, _plan, _number)
{
}
 
IeCallingPN::IeCallingPN( const IeCallingPN& src )
        : IePN(src)
{
}

IeCallingPN::IeCallingPN(  )
        : IePN()
{
	/* following 2 fields construct 0x83 in the second byte of CallingPN */
	present = PN_PRESENT_ALLOWED;
	screen = PN_SCREEN_NETWORK_PROVIDED;
}

IeCallingPN::~IeCallingPN( )
{
}

const IeCallingPN& IeCallingPN::operator=(const IeCallingPN& src)
{
//	if (&src != this)
//	{
//		value = src.value;
//	}
	return (*this);
}


bool IeCallingPN::operator ==(const IeCallingPN& srcmv) const
{
	bool equal = false;
//	equal = (value == srcmv.value);
	return equal;
}

IsdnIE* IeCallingPN::duplicate() const
{
	return new IeCallingPN(*this);
}

bool IeCallingPN::compareIsdnIE(IsdnIE* msg) const
{
	IeCallingPN* otherMsg = dynamic_cast<IeCallingPN*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

int IeCallingPN::vlidateType(int _type)
{
	IePN::validateType( _type);
	if(_type == 0x0 )
	{
		present = INFO_PRESENT_NULL; /* no COLP info */
		screen = INFO_SCREEN_NETWORK;
	}

	return 0;
}


unsigned char *IeCallingPN::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (type<0 || type>7)
	{
		cpLog(LOG_ERR, "type(%d) is out of range.\n", type);
		return 0;
	}
	if (plan<0 || plan>15)
	{
		cpLog(LOG_ERR, "plan(%d) is out of range.\n", plan);
		return 0;
	}
	if (present>3)
	{
		cpLog(LOG_ERR, "present(%d) is out of range.\n", present);
		return 0;
	}
	if( (present >= 0) && (screen<0 || screen>3) )
	{
		cpLog(LOG_ERR, "screen(%d) is out of range.\n", screen);
		return 0;
	}

	len = 1;
	if (length>0 && content[0]!= 0 )
		len += length;//strlen((char *)number);
	if (present >= 0)
		len += 1;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
#if 0
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->calling_nr = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode == ISDN_DEV_TYPE_USER)
		p[sp++] = IE_CALLING_PN;
	p[sp++] = len;
	if (present >= 0)
	{
		p[sp++] = 0x00 + (type<<4) + plan;
		p[sp++] = 0x80 + (present<<5) + screen;
		if (length > 0 && content[0] != 0 )
			UNCPY((char *)p+sp, (char *)content, length );
	}
	else
	{
		p[sp++] = 0x80 + (type<<4) + plan;
		if (length >0 && content[0] != 0 )
			UNCPY((char *)p+sp, (char *)content, length );
	}
	
	return p;
}

#if 0
int IeCallingPN::decode(const char *p)
{
	char number[ISDN_IE_CONTENT_LENGTH];
	int _length;

	if (!p)
		return ERROR_IE_IS_NULL;
	
	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "CallINGPN IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	type = (p[1]&0x70) >> 4;
	plan = p[1] & 0xf;

	if (!(p[1] & 0x80))
	{
		if (p[0] < 2)
		{
			cpLog(LOG_ERR, "IE too short (%d).\n", p[0]);
			return ERROR_IE_TOO_SHORT;
		}
		present = (p[2]&0x60) >> 5;
		screen = p[2] & 0x3;
		strnncpy((unsigned char *)number, (unsigned char *)p+3, p[0]-2, _length);
	}
	else
	{
		strnncpy((unsigned char *)number, (unsigned char *)p+2, p[0]-1, _length);
	}

	return AS_OK;
}
#endif

		
#if WITH_DEBUG
const char *IeCallingPN::debugInfo()
{
/*
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
*/	
	return IePN::debugInfo();

/*
	length += sprintf( p+ length, "Called PN IE info :");
	length += sprintf( p+ length, "\tPN info  : %s", tmp );
	free((void *)tmp);

	return p;
*/	
}
#endif		

