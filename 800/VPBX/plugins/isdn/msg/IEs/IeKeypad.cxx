/*
* $Id: IeKeypad.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeKeypad.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeKeypad::IeKeypad(const unsigned char *p ) throw(IsdnIeParserException &) 
	:IeContent(p)
{
}

IeKeypad::IeKeypad( )
	:IeContent()
{
}

IeKeypad::IeKeypad(const IeKeypad& src)
	:IeContent(src)
{
}

IeKeypad::~IeKeypad()
{
}

IeKeypad& IeKeypad::operator = (const IeKeypad& src)
{
	if (&src != this)
	{
		length = src.length;
		memcpy(content, src.content, length);
	}

	return *this;
}


bool IeKeypad::operator ==(const IeKeypad& src) const
{
	cpLog(LOG_DEBUG_STACK, "IeKeypad ::operator ==");
	if(!strcmp((char *)content, (char *)src.content)  && length== src.length)
		return true;
	else
		return false;
}

bool IeKeypad::operator< (const IeKeypad& src) const
{
	return false;
}

IsdnIE* IeKeypad::duplicate() const
{
	return new IeKeypad(*this);
}

bool IeKeypad::compareIsdnIE(IsdnIE* msg) const
{
	IeKeypad* otherMsg = dynamic_cast<IeKeypad*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeKeypad::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (!content[0])
	{
		cpLog(LOG_ERR, "keypad info not given.\n");
		return 0;
	}

	len = length;//strlen((char *)content);
	tLen = len +1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->keypad = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_FACILITY;
	p[sp++] = len;
	
	UNCPY((char *)p+sp, (char *)content, length );

	return p;
}

#if WITH_DEBUG
const char *IeKeypad::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "KEYPAD IE info :");
	length += sprintf( p+ length, "\tContent  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		

