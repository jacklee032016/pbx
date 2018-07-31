/*
* $Id: IeNotify.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "IeNotify.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeNotify::IeNotify( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode NOTIFY IE", __FILE__, __LINE__, res );
	}
}

IeNotify::IeNotify(int _notify)
{
	notify = _notify;
}


IeNotify::IeNotify( const IeNotify & src)
{
}

IeNotify& IeNotify::operator=(const IeNotify& src)
{
	if ( &src != this)
	{
	}
	return (*this);
}

bool IeNotify::operator==(const IeNotify& src) const
{
	if (1   )
	{
		return true;
	}
	else
	{
		return false;
	}
}


IsdnIE* IeNotify::duplicate() const
{
	return new IeNotify(*this);
}

bool IeNotify::compareIsdnIE(IsdnIE* msg) const
{
	IeNotify* otherMsg = dynamic_cast<IeNotify*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
} 

unsigned char *IeNotify::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (validate()==false)
	{
		cpLog(LOG_ERR, "notify(%d) is out of range.\n", notify);
		return 0;
	}

	len = 1;
	tLen = len +1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen );
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->notify = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_NOTIFY;
	p[sp++] = len;
	p[sp++] = 0x80 + notify;

	return p;
}

int IeNotify::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;

	if(p[0] != IE_NOTIFY )
	{
		cpLog(LOG_ERR, "Not CALL SubAddress IE (should be %x, but %x)", IE_NOTIFY, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
	
	
	if (p[1] < 1)
	{
		cpLog(LOG_ERR, "IE NOTIFY too short (%d).\n", p[1]);
		return ERROR_IE_TOO_SHORT;
	}

	notify = p[2] & 0x7f;

	if (validate()==false)
	{
		cpLog(LOG_ERR, "notify(%d) is out of range.\n", notify);
		return ERROR_IE_NOTIFY_ERROR;
	}
	
	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeNotify::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	

	length += sprintf( p+ length, "NOTIFY IE info :");
	length += sprintf( p+ length, "\tNotify : %s", (notify==IE_NOTIFY_USER_RESUME)?"RESUME":"SUSPEND");

	return p;
}
#endif		

bool IeNotify::validate()
{
	if (notify==IE_NOTIFY_USER_RESUME || notify == IE_NOTIFY_USER_SUSPEND)
		return true;
	return false;
}

