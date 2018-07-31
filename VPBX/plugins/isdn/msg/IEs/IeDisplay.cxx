/*
* $Id: IeDisplay.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "IeDisplay.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeDisplay::IeDisplay( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IeContent(p)
{
}

IeDisplay::IeDisplay(const IeDisplay& src)
        : IeContent( src )
{
}

IeDisplay::IeDisplay()
	: IeContent( )
{
}

IeDisplay::~IeDisplay( )
{
}


bool IeDisplay::operator ==( const IeDisplay& src) const
{
	return IeContent::operator==(src);
}

const IeDisplay& IeDisplay::operator=(const IeDisplay& src)
{
	IeContent::operator=(src);
	return *this;
}


IsdnIE* IeDisplay::duplicate() const
{
	return new IeDisplay(*this);
}


bool IeDisplay::compareIsdnIE(IsdnIE* msg) const
{
	IeDisplay* otherMsg = dynamic_cast<IeDisplay*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeDisplay::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (!content[0])
	{
		cpLog(LOG_ERR, "display text not given.\n");
		return 0;
	}

	if (strlen((char *)content) > 80)
	{
		cpLog(LOG_ERR, "display text too long (max 80 chars), cutting.\n");
		content[80] = '\0';
	}
	
	len = strlen((char *)content);
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->display = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_DISPLAY;
	p[sp++] = len;
	
	UNCPY((char *)p+sp, (char *)content,  len);

	return p;
}
		
#if WITH_DEBUG
const char *IeDisplay::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "DISPLAY IE info :");
	length += sprintf( p+ length, "\tContent  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		

