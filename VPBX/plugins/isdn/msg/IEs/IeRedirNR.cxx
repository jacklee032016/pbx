/*
* $Id: IeRedirNR.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeRedirNR.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeRedirNR::IeRedirNR( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IePN(p)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode REDIR_NR IE", __FILE__, __LINE__, res );
	}
}

IeRedirNR::IeRedirNR(int type, int plan, const unsigned char *number)
	:IePN(type, plan, number)
{}

IeRedirNR::IeRedirNR( const IeRedirNR & src)
	:IePN(src)
{
}

IeRedirNR::~IeRedirNR()
{
}

IeRedirNR& IeRedirNR::operator=(const IeRedirNR& src)
{
	if ( &src != this)
	{
		IePN::operator=(src);
	}
	return (*this);
}


bool IeRedirNR::operator ==(const IeRedirNR& src) const
{
	bool b;
	b = IePN::operator==(src); 
	if (b==true &&  reason == src.reason)
	{
		return true;
	}
	else
	{
		return false;
	}
}

IsdnIE* IeRedirNR::duplicate() const
{
	return new IeRedirNR(*this);
}

bool IeRedirNR::compareIsdnIE(IsdnIE* msg) const
{
	IeRedirNR* otherMsg = dynamic_cast<IeRedirNR*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

void IeRedirNR::setReason(int _reason)
{
	reason = validateReason( _reason);
};

int IeRedirNR::validateReason( int _reason)
{
	if(_reason == 1 )
		return INFO_REDIR_BUSY;
	else if( _reason == 2 )
		return INFO_REDIR_NORESPONSE;
	else if( _reason == 15 )
		return INFO_REDIR_UNCONDITIONAL;
	else if( _reason == 10 )
		return INFO_REDIR_CALLDEFLECT;
	else if( _reason ==  9 )
		return INFO_REDIR_OUTOFORDER;
	else 
		return INFO_REDIR_UNKNOWN;
}

int IeRedirNR::vlidateType(int _type)
{
	IePN::validateType( _type);
	if(_type == 0x0 )
	{
		present = INFO_PRESENT_NULL; /* no COLP info */
		reason = INFO_REDIR_UNKNOWN;
	}

	return 0;
}

unsigned char *IeRedirNR::encode(msg_t *msg, int ntMode)
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
	if (present > 3)
	{
		cpLog(LOG_ERR, "present(%d) is out of range.\n", present);
		return 0;
	}
	if( (present >= 0) &&  (screen<0 || screen>3) )
	{
		cpLog(LOG_ERR, "screen(%d) is out of range.\n", screen);
		return 0;
	}
	if (reason > 0x0f)
	{
		cpLog(LOG_ERR, "reason(%d) is out of range.\n", reason);
		return 0;
	}

	len = 1;
	if ( length )
		len += length;//strlen((char *)content);
	if (present >= 0)
	{
		len += 1;
		if (reason >= 0)
			len += 1;
	}
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	
	p = msg_put(msg, tLen);

	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_REDIR_NR;
	p[sp++] = len;
	if (present >= 0)
	{
		if (reason >= 0)
		{
			p[sp++] = 0x00 + (type<<4) + plan;
			p[sp++] = 0x00 + (present<<5) + screen;
			p[sp++] = 0x80 + reason;
			if ( length >0 )
				UNCPY( (char *)p+sp, (char *)content, length ); /**/
		}
		else
		{
			p[sp++] = 0x00 + (type<<4) + plan;
			p[sp++] = 0x80 + (present<<5) + screen;
			if ( length >0 )
				UNCPY((char *)p+sp, (char *)content, length );
		}
	}
	else
	{
		p[sp++] = 0x80 + (type<<4) + plan;
		if (length >0 && content[0]!=0 )
			UNCPY((char *)p+sp, (char *)content, length );
	}

	return p;
}

int IeRedirNR::decode(const unsigned char *p)
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
		
		if (!(p[2] & 0x80))
		{
			reason = p[3] & 0x0f;
			strnncpy((unsigned char *)number, (unsigned char *)p+4, p[0]-3, _length);
		}
		else
		{
			reason = -1;
			strnncpy((unsigned char *)number, (unsigned char *)p+3, p[0]-2, _length );
		}
	}
	else
	{
		strnncpy((unsigned char *)number, (unsigned char *)p+2, p[0]-1, _length );
	}

	return AS_OK;
}

#if WITH_DEBUG
const char *IeRedirNR::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IePN::debugInfo();

	length += sprintf( p+ length, "REDIR NR IE info :");
	length += sprintf( p+ length, "\tPN info  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif

