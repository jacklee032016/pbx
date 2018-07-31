/*
* $Id: IeDate.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "IeDate.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"

using namespace Assist;

IeDate::IeDate( const char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in Decode DATE IE", __FILE__, __LINE__, res );
	}
}

IeDate::IeDate()
        :IsdnIE()
{
}

IeDate::IeDate( const IeDate& src)
        :IsdnIE()
{
}

bool IeDate::operator == (const IeDate& src) const
{
	cpLog(LOG_DEBUG_STACK, "IeDate == operator");
	if ( /* ( day == src.day) && ( year == src.year) &&
		( month == src.month) && ( timezonediff == src.timezonediff) */ 1 )
    	{
    		cpLog(LOG_DEBUG_STACK, "IeDate == operator returning true :)");
		return true;
	}
	else
	{
		cpLog(LOG_DEBUG_STACK, "IeDate == operator returning false:(");
		return false;
	}
}

bool IeDate::operator <(const IeDate& src) const
{
    return false;
}

const IeDate& IeDate::operator=( const IeDate& src)
{
	if (&src != this)
	{
		;
	}
	return *this;
}

IsdnIE* IeDate::duplicate() const
{
	return new IeDate(*this);
}

bool IeDate::compareIsdnIE(IsdnIE* msg) const
{
	IeDate* otherMsg = dynamic_cast<IeDate*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

char *IeDate::encode(msg_t *msg)
{
	char *p;
	int len;

	struct tm *tm;

	tm = localtime(&ti);
	if (!tm)
	{
		cpLog(LOG_ERR, "localtime() returned NULL.\n");
		return 0;
	}

	cpLog(LOG_DEBUG, "    year=%d month=%d day=%d hour=%d minute=%d\n", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);

	len = 5 + (!!seconds);
	p = (char *)msg_put(msg, len+2);
	
	p[0] = IE_DATE;
	p[1] = len;
	p[2] = tm->tm_year % 100;
	p[3] = tm->tm_mon + 1;
	p[4] = tm->tm_mday;
	p[5] = tm->tm_hour;
	p[6] = tm->tm_min;
	
	if (seconds)
		p[7] = tm->tm_sec;

	return p;
}


int IeDate::decode(const char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
	assert(p[0]==IE_DATE);
	
	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeDate::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	length += sprintf( p+ length, "DATE IE info :");
	length += sprintf( p+ length, "\tSeconds \t : %d", seconds);

	return p;
}
#endif		

