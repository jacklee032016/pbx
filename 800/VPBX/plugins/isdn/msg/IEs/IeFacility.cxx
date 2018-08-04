/*
* $Id: IeFacility.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "cpLog.h"
//#include <cassert>

#include "IeFacility.hxx"

#include "l3ins.h"

using namespace Assist;

IeFacility::IeFacility(const unsigned char *p) throw(IsdnIeParserException &) 
	:IeContent( p)
{
}

IeFacility::IeFacility()
	:IeContent( )
{
}

IeFacility::IeFacility( const IeFacility& src)
	:IeContent( src )
{
}

IeFacility::~IeFacility( )
{
}

const IeFacility& IeFacility::operator =(const IeFacility& src)
{
	IeContent::operator=(src);
	return *this;
}

bool  IeFacility::operator == (const IeFacility& other) const
{
	cpLog(LOG_DEBUG_STACK, "IeFacility == operator");

	return IeContent::operator==(other);
}


bool  IeFacility::operator < (const IeFacility& other) const
{
	if ( 1) //cseq < other.cseq)
	{
		return true;
	}
	else// if (cseq > other.cseq)
	{
		return false;
	}
}

bool IeFacility::operator > (const IeFacility& other) const
{
	return false;
}

IsdnIE* IeFacility::duplicate() const
{
	return new IeFacility(*this);
}

bool IeFacility::compareIsdnIE(IsdnIE* msg) const
{
	IeFacility* otherMsg = dynamic_cast<IeFacility*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeFacility::encode( msg_t *msg, int ntMode)
{
	unsigned char *p;
	int sp = 0;
	int len = FACILITY_LENGTH + 1;

	if(ntMode == ISDN_DEV_TYPE_USER)
	{
		len = (ntMode==ISDN_DEV_TYPE_USER)?(FACILITY_LENGTH+2):(FACILITY_LENGTH+1);
		p = msg_put(msg, len);

		if(ntMode==ISDN_DEV_TYPE_USER)
			p[sp++] = IE_FACILITY;
		p[sp++] = FACILITY_LENGTH;

		p[sp++] = 0x80+ SUPPLEMENTARY_SERVICE;
		p[sp++] = FACILITY_INVOKE;
		p[sp++] = FACILITY_INVOKE_LENGTH;

		p[sp++] = FACILITY_ID_INTERGER;
		p[sp++] = FACILITY_ID_LENGTH;
		p[sp++] = FACILITY_ID_VALUE;

		p[sp++] = FACILITY_OPERATION_OBJECT;
		p[sp++] = FACILITY_OPERATION_LENGTH;
		p[sp++] = FACILITY_OPERATION_OID_CCITT +FACILITY_OPERATION_OID_NETWORK;
		p[sp++] = FACILITY_OPERATION_OID_NTT_FIRST;
		p[sp++] = FACILITY_OPERATION_OID_NTT_SECOND;
		p[sp++] = FACILITY_OPERATION_OID_ISDN_SERVICE_FIRST;
		p[sp++] = FACILITY_OPERATION_OID_ISDN_SERVICE_SECOND;
		p[sp++] = FACILITY_OPERATION_OID_OPERATION;
		p[sp++] = FACILITY_OPERATION_OID_UNKNOWN;

		p[sp++] = FACILITY_SET_TYPE;
		p[sp++] = FACILITY_SET_LENGTH;
		p[sp++] = 0x81;
		p[sp++] = 0x01;
	//	p[sp++] = 0x01;
		p[sp++] = 0x02;
		
	//	memcpy(p+2, content, length);
		}
	else
	{/* 0x1c 0x09 0x91 0xa4 0x06 0x02 0x01 0x03 0x81 0x01 0x01 */
		len = 9+2 ;
		p = msg_put(msg, len);

		if(ntMode==ISDN_DEV_TYPE_USER)
			p[sp++] = IE_FACILITY;
		p[sp++] = 9;

		p[sp++] = 0x80+ SUPPLEMENTARY_SERVICE;
		p[sp++] = FACILITY_REJECT;
		p[sp++] = 0x06;

		p[sp++] = FACILITY_ID_INTERGER;
		p[sp++] = 01;//FACILITY_ID_LENGTH;
		p[sp++] = FACILITY_SET_LENGTH;
		p[sp++] = 0x81;
		p[sp++] = 0x01;
		p[sp++] = 0x01;
		
	}
	return p;
}

#if WITH_DEBUG
const char *IeFacility::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "FACILITY IE info :");
	length += sprintf( p+ length, "\tContent  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		

