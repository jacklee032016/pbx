/*
* $Id: IePN.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IePN.hxx"
#include "IsdnParserMode.hxx"

#include "as_isdn.h"
#include "l3ins.h"

using namespace Assist;

IePN::IePN(const unsigned char *p)  throw(IsdnIeParserException &)
	:IeCalledPN( )
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode CALLID_PN IE", __FILE__, __LINE__, res );
	}
}


IePN::IePN(int _type, int _plan, const unsigned char *_number)
	:IeCalledPN(_type, _plan, _number)
{
}

IePN::IePN( const IePN& src )
	:IeCalledPN( src)
{
}

IePN::IePN( )
	:IeCalledPN( ),
/*, following 2 fields are defined by sub-class */
	present(-1),
	screen(-1)
{
}

bool IePN::operator ==(const IePN& src) const
{
	return true;
}

bool IePN::operator <( const IePN& src ) const
{
	return true;
}

const IePN& IePN::operator=( const IePN& src )
{
/*
	if ( &src != this )
	{
		delta = src.delta;
	}
*/	
	return *this;
}


void *IePN::encode() const
{
	return 0;
}


/*
IsdnIE* IePN::duplicate() const
{
    return new IePN(*this);
}
*/

bool IePN::compareIsdnIE(IsdnIE* msg) const
{
	IePN* otherMsg = dynamic_cast<IePN*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

int IePN::validateScreen(int _screen)
{
	if(_screen ==  0 )
		return INFO_SCREEN_USER;
	else
		return INFO_SCREEN_NETWORK;
}

int IePN::validatePresent( int _present)
{
	if (_present == 1 )
		return INFO_PRESENT_RESTRICTED;
	else if(_present == 2 )
		return INFO_PRESENT_NOTAVAIL;
	else
		return INFO_PRESENT_ALLOWED;
}

const unsigned char *IePN::getPartyNumber() const
{
	return content;
}


int IePN::decode(const unsigned char *p)
{
	int len;

	if (!p)
		return ERROR_IE_IS_NULL;

#if 0
	if(p[0] != IE_CALLING_PN && p[0] !=IE_REDIR_DN&& p[0] !=IE_REDIR_NR)
	{
		cpLog(LOG_ERR, "Not CALL SubAddress IE (should be %x(CallingPN) or %x(RedirDN) or %x(RedirNR), but %x)",
			IE_CALLING_PN, IE_REDIR_DN, IE_REDIR_NR, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif	
	
	len = p[0];

	
	if (len < 1)
	{
		cpLog(LOG_ERR, "CallINGPN IE too short (%d).\n",  len);
		return ERROR_IE_TOO_SHORT;
	}

	type = (p[1]&0x70) >> 4;
	plan = p[1] & 0xf;

	memset(content, 0, ISDN_IE_CONTENT_LENGTH);
	if (!(p[1] & 0x80))
	{/* extension */
		if (len < 2)
		{
			cpLog(LOG_ERR, "IE too short (%d).\n", len );
			return ERROR_IE_TOO_SHORT;
		}
		present = (p[2]&0x60) >> 5;
		screen = p[2] & 0x3;
		strnncpy((unsigned char *)content, (unsigned char *)p+3, len-2, ISDN_IE_CONTENT_LENGTH);
	}
	else
	{
		strnncpy((unsigned char *)content, (unsigned char *)p+2, len-1, ISDN_IE_CONTENT_LENGTH);
	}
	length = strlen((char *)content);

	return AS_OK;
}
		
#if WITH_DEBUG
const char *IePN::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeCalledPN::debugInfo();

	length += sprintf( p+ length, "PN info :");
	length += sprintf( p+ length, "\tPresent : %s", getPresentName() );
	length += sprintf( p+ length, "\tScreen : %s", getScreenName() );
	length += sprintf( p+ length, "\t %s", tmp );
	free((void *)tmp);

	return p;
}

const char *IePN::getPresentName()
{
	switch( present)
	{
		case PN_PRESENT_ALLOWED:
			return "ALLOWED";
			break;
		case PN_PRESENT_RESERVED:
			return "RESERVED";
			break;
		case PN_PRESENT_RESTRICTED:
			return "RESTRICTED";
			break;
		case PN_PRESENT_UNAVAILABLE_INTERWORKING:
			return "UNAVAILABLE_INTERWORKING";
			break;
		default:
			break;
	}
	return "Unknown PRESENT TYPE";
}

const char *IePN::getScreenName()
{
	switch( screen)
	{
		case PN_SCREEN_USER_PROVIDED:
			return "USER_PROVIDED";
			break;
		case PN_SCREEN_NETWORK_PROVIDED:
			return "NETWORK_PROVIDED";
			break;
		default:
			break;
	}
	return "Unknown SCREEN TYPE";
}

#endif		


