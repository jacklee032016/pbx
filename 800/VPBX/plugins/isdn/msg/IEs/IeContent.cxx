/*
* $Id: IeContent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include <cassert>

#include "IeContent.hxx"
//#include <cmath>
#include "cpLog.h"
#include "as_isdn.h"
#include "l3ins.h"

using namespace Assist;

IeContent::IeContent( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IsdnIE(),
	length(0)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode Content IE", __FILE__, __LINE__, res );
	}
}

IeContent::IeContent()
	:IsdnIE(),
	length(0)
{
	memset(content, 0, ISDN_IE_CONTENT_LENGTH);
}

IeContent::IeContent( const IeContent& src)
	:IsdnIE(),
	length(0)
{
	length = src.length;
	memcpy(content, src.content, length);
}

IeContent::~IeContent( )
{
}

const IeContent& IeContent::operator =(const IeContent& src)
{
	if ( &src != this)
	{
		length = src.length;
		memcpy(content, src.content, length);
	}

	return *this;
}

void IeContent::setContent( const unsigned char *_content)
{
	length = strlen((char *)_content);
	memcpy(content, _content, length);
}

const unsigned char *IeContent::getContent()
{
	return content;
}

int IeContent::getLength()
{
	return length;
}

bool IeContent::operator == (const IeContent& other) const
{
	cpLog(LOG_DEBUG_STACK, "IeContent == operator");

	if (( length == other.length) && ( !strcmp((char *)content, (char *)other.content) )  )
	{
		cpLog(LOG_DEBUG_STACK, "IeContent == operator returning true");
		return true;
	}
	else
	{
		cpLog(LOG_DEBUG_STACK, "IeContent == operator returning false");
		return false;
	}
}

bool IeContent::operator < (const IeContent& other) const
{
	if ( strcmp((char *)content, (char *)other.content) < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IeContent::operator > (const IeContent& other) const
{
	if ( strcmp((char *)content, (char *)other.content) > 0 )
	{	
		return true;
	}
	else
	{
		return false;
	}
}

/*
IsdnIE* IeContent::duplicate() const
{
	return new IeContent(*this);
}
*/

int IeContent::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;

#if 0	
	if(p[0] != IE_DISPLAY && p[0] !=IE_FACILITY && p[0] !=IE_KEYPAD
		&& p[0] !=IE_USER_USER&& p[0] !=IE_CALL_ID )
	{
		cpLog(LOG_ERR, "Not Correct IE (should be %x(Display), %x(Facility), %x(Keypad), %x(UserUser), %x(CallID), but %x)", 
			IE_DISPLAY, IE_FACILITY,IE_KEYPAD , IE_USER_USER, IE_CALL_ID, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	strnncpy((unsigned char *)content, (unsigned char *)p+1, p[0], ISDN_IE_CONTENT_LENGTH);

	return AS_OK;
}

#if WITH_DEBUG
const char *IeContent::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);

	length += sprintf( p+ length, "Content : length : %d ", length );
	length += sprintf( p+ length, "\t '%s'", content );

	return p;
}
#endif		

