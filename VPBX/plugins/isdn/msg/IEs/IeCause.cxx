/*
* $Id: IeCause.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IeCause.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"

using namespace Assist;

IeCause::IeCause( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in Decode Cause IE", __FILE__, __LINE__, res );
	}
}

IeCause::IeCause(int _location, int _cause)
{
	location = _location;
	cause = _cause;
}

IeCause::IeCause( const IeCause& src)
{
	location = src.location;
	cause = src.cause;
}

const IeCause& IeCause::operator =(const IeCause& src)
{
	if (&src != this)
	{
		location = src.location;
		cause = src.cause;
	}
	return (*this);
}

bool IeCause::operator ==( const IeCause& src) const
{
	return ( (location == src.location) && (cause == src.cause) );
 }



IsdnIE* IeCause::duplicate() const
{
	return new IeCause(*this);
}

bool IeCause::compareIsdnIE(IsdnIE* msg) const
{
	IeCause* otherMsg = dynamic_cast<IeCause*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeCause::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	cpLog(LOG_DEBUG, "CAUSE IE encode");
	if (location<0 || location>7)
	{
		cpLog(LOG_ERR, "location(%d) is out of range.\n", location);
		return 0;
	}
	
	if (cause<0 || cause>127)
	{
		cpLog(LOG_ERR, "cause(%d) is out of range.\n", cause);
		return 0;
	}

	len = 2;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen ); 	/* added total 4 bytes at tail */
#if 0
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->cause = p - ( char *)qi - sizeof(Q931_info_t);
#endif

	if(ntMode==ISDN_DEV_TYPE_USER )
		p[sp++] = IE_CAUSE;
	p[sp++] = len;
	p[sp++] = 0x80 + location;
	p[sp++] = 0x80 + cause;

	return p;
}

int IeCause::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
#if 0
	if(p[0] !=  IE_CAUSE)
	{
		cpLog(LOG_ERR, "Not CALLED PN IE (should be %x, but %x)", IE_CAUSE, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 2)
	{
		cpLog(LOG_ERR, "IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	location = p[1] & 0x0f;
	cause = p[2] & 0x7f;
	if(cause < 0)
		cause = 16;
	
	return AS_OK;
}

		
#if WITH_DEBUG
const char *IeCause::debugInfo()
{
	int length = 0;
//	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
//	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "Cause IE info :");
//	length += sprintf( p+ length, "\tContent  : %s", tmp );
//	free((void *)tmp);
	length += sprintf( p+ length, "\tLocation : %d", location );
	length += sprintf( p+ length, "\tCause : %d", cause );

	return p;
}
#endif		


