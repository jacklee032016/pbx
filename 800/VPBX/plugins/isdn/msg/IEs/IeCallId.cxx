/*
* $Id: IeCallId.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeCallId.hxx"
#include "IsdnParserMode.hxx"
#include "l3ins.h"


using namespace Assist;

IeCallId::IeCallId(const unsigned char *p)  throw(IsdnIeParserException &) 
	:IeContent(p)
{
}

IeCallId::IeCallId()
	:IeContent()
{
}

IeCallId::IeCallId( const IeCallId& src)
	:IeContent(src)
{
}


bool IeCallId::operator == (const IeCallId& src) const
{
//	return ( data == src.data);
	return false;
}

const IeCallId& IeCallId::operator = (const IeCallId& src)
{
/*
	if (&src != this)
	{
		data = src.data;
	}
*/	
	return (*this);
}

IsdnIE* IeCallId::duplicate() const
{
	return new IeCallId(*this);
}


bool IeCallId::compareIsdnIE(IsdnIE* msg) const
{
	IeCallId* otherMsg = dynamic_cast<IeCallId*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeCallId::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (content[0]==0 || length<=0)
	{
		return 0;
	}
	if (length >8)
	{
		cpLog(LOG_ERR, "callid_len(%d) is out of range.\n", length);
		return 0;
	}

	len = length;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);

#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->call_id = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_CALL_ID;
	p[sp++] = len;
	memcpy(p+sp, content, length);

	return p;
}

#if WITH_DEBUG
const char *IeCallId::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "Call ID IE info :");
	length += sprintf( p+ length, "\tContent  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		


