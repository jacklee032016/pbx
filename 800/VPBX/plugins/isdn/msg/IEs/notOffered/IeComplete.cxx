/*
* $Id: IeComplete.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "IeComplete.hxx"

#include "IsdnParserMode.hxx"
#include "l3ins.h"

using namespace Assist;

IeComplete::IeComplete( const char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in Decode COMPLETE IE", __FILE__, __LINE__, res );
	}
}

IeComplete::IeComplete( int _complete )
	: complete(_complete)
{
}

IeComplete& IeComplete::operator=(const IeComplete& rhs)
{
	if ( &rhs != this )
	{
		this->complete = rhs.complete;
	}
	return *this;
}

bool IeComplete::operator==( IeComplete& src) const
{
	return (complete == src.getComplete());
}


IeComplete::IeComplete( const IeComplete& src )
        : complete(src.complete)
{}

IeComplete::~IeComplete()
{
}



IsdnIE* IeComplete::duplicate() const
{
	return new IeComplete(*this);
}

bool IeComplete::compareIsdnIE(IsdnIE* msg) const
{
	IeComplete *otherMsg = dynamic_cast<IeComplete *>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

char *IeComplete::encode(msg_t *msg)
{
	char *p;

	if (complete<0 || complete>1)
	{
		cpLog(LOG_ERR, "complete(%d) is out of range.\n", complete);
		return 0;
	}

	p = (char *)msg_put(msg, 1);
	p[0] = IE_COMPLETE;

	return p;
}


int IeComplete::decode(const char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
	assert(p[0] ==  IE_COMPLETE);
	complete = 1;

	return AS_OK;
}

int IeComplete::getComplete()
{
	return complete;
}

void IeComplete::setComplete(int _complete)
{
	this->complete = _complete;
}
		
		
#if WITH_DEBUG
const char *IeComplete::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	length += sprintf( p+ length, "COMPLETE IE info :");
	length += sprintf( p+ length, "\tComplete \t : %d",  complete);

	return p;
}
#endif		


