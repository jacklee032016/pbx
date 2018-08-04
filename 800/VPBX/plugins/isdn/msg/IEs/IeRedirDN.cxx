/*
* $Id: IeRedirDN.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeRedirDN.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeRedirDN::IeRedirDN( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IePN(p)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode REDIR_DN IE", __FILE__, __LINE__, res );
	}
}

IeRedirDN::IeRedirDN(int _type, int _plan, const unsigned char *_number)
        :IePN(_type, _plan, _number)
{
}

IeRedirDN::IeRedirDN( const IeRedirDN& src)
	:IePN(src)
{
//	type = src.type;
}

IeRedirDN::~IeRedirDN()
{
}

bool IeRedirDN::operator==(const IeRedirDN& src) const
{
	cpLog(LOG_DEBUG_STACK, "IeRedirDN ::operator ==");

	bool equal = false;
	cpLog(LOG_DEBUG_STACK, "SubsNotifyEvent operator == final result: %d", equal);

	return equal;
}


IeRedirDN & IeRedirDN::operator=(const IeRedirDN& src)
{
	if (&src != this)
	{
		IePN::operator = (src);
	}
	return (*this);
}


IsdnIE* IeRedirDN::duplicate() const
{
    return new IeRedirDN(*this);
}

bool IeRedirDN::compareIsdnIE(IsdnIE* msg) const
{
    IeRedirDN* otherMsg = dynamic_cast<IeRedirDN*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


int IeRedirDN::vlidateType(int _type)
{
	IePN::validateType( _type);
	if(_type == 0x0 )
		present = INFO_PRESENT_NULL; /* no COLP info */

	return 0;
}


unsigned char *IeRedirDN::encode(msg_t *msg, int ntMode)
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

	len = 1;
	if (length>0 )
		len += length;//strlen((char *)content);
	if (present >= 0)
		len += 1;

	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen );

	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_REDIR_DN;
	p[sp++] = len;
	if (present >= 0)
	{
		p[sp++] = 0x00 + (type<<4) + plan;
		p[sp++] = 0x80 + (present<<5);
		if (length> 0)
			UNCPY((char *)p+sp, (char *)content, length );
	}
	else
	{
		p[sp++] = 0x80 + (type<<4) + plan;
		if ( length>0 )
			UNCPY((char *)p+sp , (char *)content, length );
	}

	return p;
}

#if 0
int IeRedirDN::decode(const char *p)
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
const char *IeRedirDN::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IePN::debugInfo();

	length += sprintf( p+ length, "REDIR DN IE info :");
	length += sprintf( p+ length, "\tPN info  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		


