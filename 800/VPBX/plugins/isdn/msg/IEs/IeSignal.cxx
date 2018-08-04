/*
* $Id: IeSignal.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"
#include "IeSignal.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"

using namespace Assist;


IeSignal::IeSignal( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode SIGNAL IE", __FILE__, __LINE__, res );
	}
}

IeSignal::IeSignal(int _signal)
{
	signal = _signal;
}

IeSignal::IeSignal( const IeSignal& src )
{
	signal = src.signal;
}

bool IeSignal::operator ==(const IeSignal& src) const
{
	if(signal==src.signal)
		return true;
	return false;
}

const IeSignal& IeSignal::operator=( const IeSignal& src )
{
	if (this != &src)
	{
		signal = src.signal;
	}
	return *this;
}

IsdnIE* IeSignal::duplicate() const
{
	return new IeSignal(*this);
}

bool IeSignal::compareIsdnIE(IsdnIE* msg) const
{
	IeSignal* otherMsg = dynamic_cast<IeSignal*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeSignal::encode(msg_t *msg, int ntMode)
{
	return 0;
}

int IeSignal::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
	signal =1;
	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeSignal::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);

	length += sprintf( p+ length, "SIGNAL IE info :");
	length += sprintf( p+ length, "\tLSignal \t : %d", signal );

	return p;
}
#endif		


