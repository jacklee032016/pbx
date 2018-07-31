/*
* $Id: IeCharge.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeCharge.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"

using namespace Assist;


IeCharge::IeCharge( const unsigned char  *p)  throw(IsdnIeParserException &)
{
TRACE;
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode PROGRESS IE", __FILE__, __LINE__, res );
	}
}

IeCharge::IeCharge(unsigned char * _indicator, int _infoType)
{
	infoType = _infoType;
	setIndicator(_indicator);
}

IeCharge::IeCharge( const IeCharge& src )
{
	infoType = src.infoType;
	setIndicator(src.chargeIndicator);
}

IeCharge::~IeCharge( )
{
}

bool IeCharge::operator == (const IeCharge& src) const
{
	if ( ( infoType == src.infoType) &&
		( strcmp((const char *)chargeIndicator, (const char *)src.chargeIndicator)==0 )  )
		return true;
	else
		return false;

}

bool IeCharge::operator<(const IeCharge& src) const
{
	return false;
}

const IeCharge& IeCharge::operator=(const IeCharge& src)
{
	if ( &src != this)
	{
		infoType = src.infoType;
		setIndicator(src.chargeIndicator );
	}
	
	return (*this);
}


IsdnIE* IeCharge::duplicate() const
{
	return new IeCharge(*this);
}


bool IeCharge::compareIsdnIE(IsdnIE* msg) const
{
	IeCharge* otherMsg = dynamic_cast<IeCharge*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

void IeCharge::setIndicator( const unsigned char *_indicator)
{
	length = strlen((char *)_indicator);
	length = (length > ISDN_IE_CONTENT_LENGTH)?ISDN_IE_CONTENT_LENGTH:length;
	memcpy(chargeIndicator, _indicator, length);
}

const unsigned char *IeCharge::getIndicator()
{
	return chargeIndicator;
}

int IeCharge::getLength()
{
	return length;
}


unsigned char *IeCharge::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if ( infoType != IE_CHARGE_TOTAL_CHARGE )
	{
		cpLog(LOG_ERR, "Information Type (%d) is out of range.\n", infoType );
		return 0;
	}
	if (length > 12 )
	{
		cpLog(LOG_ERR, "length (%d) is too big\n", length );
		return 0;
	}

	if(ntMode == ISDN_DEV_TYPE_USER) /* in TE mode, no charge info is send out by ourself */
		return 0;

	len = length +1;
	tLen = len;
	if(ntMode==ISDN_DEV_TYPE_USER )
		tLen++;/* need nore 1 byte for USER mode : only length field are encoded in IE for NT mode*/
	p = msg_put(msg, tLen);

	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_ADVISE_CHARGE;
	p[sp++] = len;
	p[sp++] = 0x80 + infoType;
	
	UNCPY((char *)p+sp, (char *)chargeIndicator, length );

	return p;
}

int IeCharge::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;

	if (p[0] < 2 || p[0] > 12 )
	{
		cpLog(LOG_ERR, "IE Advice of Charge is too short or too big(%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}
TRACE;

	infoType = p[1]&0x0F;

	strnncpy((unsigned char *)chargeIndicator, (unsigned char *)p+1, p[0], ISDN_IE_CONTENT_LENGTH);

	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeCharge::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	length += sprintf( p+ length, "Advice of Charge IE info :");
	length += sprintf( p+ length, "\tInformation Type : %d", infoType );
	length += sprintf( p+ length, "\tCharge Indicator : %s", chargeIndicator );

	return p;
}
#endif		


