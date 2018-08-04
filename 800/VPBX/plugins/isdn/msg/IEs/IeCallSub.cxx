/*
* $Id: IeCallSub.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IsdnParserMode.hxx"
#include "IeCallSub.hxx"

#include "as_isdn.h"
#include "l3ins.h"

using namespace Assist;

IeCallSub::IeCallSub(  const unsigned char *p)  throw(IsdnIeParserException &) 
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode CALL SUBADDRESS IE", __FILE__, __LINE__, res );
	}
}


IeCallSub::IeCallSub( int _iscalling)
	:IeContent( )
{
	isCalling = _iscalling;
}

IeCallSub::IeCallSub(const IeCallSub& src)
	: IeContent(src), 
	type(src.type),
	isOdd(src.isOdd),
	isCalling(src.isCalling)
{
}

IeCallSub::~IeCallSub( )
{
}

 IeCallSub& IeCallSub::operator = (const IeCallSub& src)
{
	IeContent::operator=(src);
	type = src.type;
	isOdd = src.isOdd;
	isCalling = src.isCalling;
	return (*this);
}

bool IeCallSub::operator ==(const IeCallSub& src) const
{
	bool b = IeContent::operator==(src);
	if(b==true && type == src.type && isOdd==src.isOdd && isCalling==src.isCalling )
		return true;
	return false;
}

IsdnIE* IeCallSub::duplicate() const
{
	return new IeCallSub(*this);
}

bool IeCallSub::compareIsdnIE(IsdnIE* msg) const
{
	IeCallSub* otherMsg = dynamic_cast<IeCallSub*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeCallSub::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if( validate()==false)
		return 0;

	len = length;
	tLen = len+ 2; /* must be 2 ,lzj*/
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);

#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
#endif
	if(ntMode == ISDN_DEV_TYPE_USER)
	{
		if(isCalling==0)
		{
			p[sp++] = IE_CALLING_SUB;
//			qi->calling_sub = p - ( char *)qi - sizeof(Q931_info_t);
		}
		else
		{
			p[sp++] = IE_CALLED_SUB;
//			qi->called_sub = p - ( char *)qi - sizeof(Q931_info_t);
		}	
	}
	p[sp++] = len;
	p[sp++] = 0x80 + (type<<4) + (isOdd<<3);
	memcpy(p+sp, content, len);

	return p;
}

int IeCallSub::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
#if 0
	if(p[0] != IE_CALLING_SUB && p[0] !=IE_CALLED_SUB)
	{
		cpLog(LOG_ERR, "Not CALL SubAddress IE (should be %x(CallingSub) or %x(CalledSub), but %x)", IE_CALLING_SUB, IE_CALLED_SUB, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "Call SubAddress IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	type = p[1] &0x70;
	isOdd = p[1] &0x08;

	if(validate()==false)
	{
		return ERROR_IE_CALLSUB_ERROR;
	}
	
	length = p[0]-1;	

	strnncpy((unsigned char *)content, (unsigned char *)p+2, length, IE_CALLSUB_MAX_LENGTH );/* clip to 23 maximum */
	length = strlen((char *)content);
	
	return AS_OK;
}

#if WITH_DEBUG
const char *IeCallSub::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "CALL SubAddress IE info :");
	length += sprintf( p+ length, "\tType : %s", (type==IE_CALLSUB_TYPE_NASP)?"NASP":"User");
	length += sprintf( p+ length, "\tType : %s", (isOdd==IE_CALLSUB_IS_ODD)?"Odd":"Even");
	length += sprintf( p+ length, "\tContent  : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		

const int IeCallSub::getType() const
{
	return type;
}
		
void IeCallSub::setType(int _type)
{
	type = _type;
}
		
const int IeCallSub::getIsOdd() const
{
	return isOdd;
}
		
void IeCallSub::setIsOdd(int _isOdd)
{
	isOdd = _isOdd;
}		

bool IeCallSub::validate()
{
	if( (type== IE_CALLSUB_TYPE_NASP ||type==IE_CALLSUB_TYPE_USER)
		&&(isOdd==IE_CALLSUB_IS_EVEN || isOdd==IE_CALLSUB_IS_ODD ) )
		return true;

	return false;
}

int IeCallSub::getIeType() const
{
	if(isCalling==0)
		return ISDN_IE_CALLING_SUB;
	else
		return ISDN_IE_CALLED_SUB;
};

