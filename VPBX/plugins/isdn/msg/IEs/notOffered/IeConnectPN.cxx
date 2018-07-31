/*
* $Id: IeConnectPN.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeConnectPN.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeConnectPN::IeConnectPN( const char  *p)  throw(IsdnIeParserException &)
	:IePN(p)
{
#if 0
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode CONNECT_PN IE", __FILE__, __LINE__, res );
	}
#endif	
}

IeConnectPN::IeConnectPN(int _type, int _plan, char *_number)
	:IePN(_type, _plan, _number)
{
}

IeConnectPN::IeConnectPN( const IeConnectPN& src )
	:IePN( src)
{
}

IeConnectPN::~IeConnectPN( )
{
}

bool IeConnectPN::operator ==(const IeConnectPN& src) const
{
	return true;
}

bool IeConnectPN::operator <( const IeConnectPN& src ) const
{
	return true;
}

const IeConnectPN& IeConnectPN::operator=( const IeConnectPN& src )
{
/*
	if ( &src != this )
	{
		delta = src.delta;
	}
*/
	return *this;
}


IsdnIE* IeConnectPN::duplicate() const
{
	return new IeConnectPN(*this);
}


bool IeConnectPN::compareIsdnIE(IsdnIE* msg) const
{
	IeConnectPN* otherMsg = dynamic_cast<IeConnectPN*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

int IeConnectPN::vlidateType(int _type)
{
	IePN::validateType( _type);
	if(_type == 0x0 )
		present = INFO_PRESENT_NULL; /* no COLP info */

	return 0;
}

char *IeConnectPN::encode(msg_t *msg)
{
	char *p;
	int len;

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
	if (length>0 && content[0] != 0 )
		len += length;//strlen((char *)number);
	if (present >= 0)
		len += 1;
	p = (char *)msg_put(msg, len+2);
	
	p[0] = IE_CONNECT_PN;
	p[1] = len;
	if (present >= 0)
	{
		p[2] = 0x00 + (type<<4) + plan;
		p[3] = 0x80 + (present<<5) + screen;
		if (length>0 && content[0] != 0 )
			UNCPY((char *)p+4, (char *)content, length );
	}
	else
	{
		p[2] = 0x80 + (type<<4) + plan;
		if (length > 0 && content[0]!= 0 )
			UNCPY((char *)p+3, (char *)content, length );
	}

	return p;
}

#if 0
int IeConnectPN::decode(const char *p)
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
const char *IeConnectPN::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IePN::debugInfo();

	length += sprintf( p+ length, "CONNECT PN IE info :");
	length += sprintf( p+ length, "\tPN info  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		

