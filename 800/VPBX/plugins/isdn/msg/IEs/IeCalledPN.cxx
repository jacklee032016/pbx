/*
* $Id: IeCalledPN.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "Sptr.hxx"
#include "cpLog.h"
#include "IsdnParserMode.hxx"
#include "IeCalledPN.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeCalledPN::IeCalledPN( const unsigned char  *p)  throw(IsdnIeParserException &)
	:IeContent()
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode CALLID_PN IE", __FILE__, __LINE__, res );
	}
}

IeCalledPN::IeCalledPN(int _type, int _plan, const unsigned char *number)
        : IeContent(number),
        type(_type),
        plan(_plan)
{
}

IeCalledPN::IeCalledPN(const IeCalledPN& src)
        : IeContent(src),
        type(src.type),
        plan(src.plan)
{
}

IeCalledPN::IeCalledPN( )
        : IeContent()
{
	/* default value when parse msg from ISDN */
	type = PN_TYPE_UNKNOWN;
	plan = PN_PLAN_UNKNOWN;
}

IeCalledPN::~IeCalledPN( )
{
}

IeCalledPN& IeCalledPN::operator = (const IeCalledPN& srcReferTo)
{
	return (*this);
}


bool IeCalledPN::operator ==(const IeCalledPN& srcReferTo) const
{
    cpLog(LOG_DEBUG_STACK, "SipAlso ::operator ==");

    return false;//equal;
}


IsdnIE* IeCalledPN::duplicate() const
{
	return new IeCalledPN(*this);
}

bool IeCalledPN::compareIsdnIE(IsdnIE* msg) const
{
	IeCalledPN* otherMsg = dynamic_cast<IeCalledPN*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeCalledPN::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (type<0 || type>7)
	{
		cpLog(LOG_ERR, "type(%d) is out of range.\n", type);
		return 0;
	}
	if (plan<0 || plan>15)
	{
		cpLog(LOG_ERR, "plan(%d) is out of range.\n", plan);
		return 0;
	}
	if (!content[0])
	{
		cpLog(LOG_ERR, "number is not given.\n");
		return 0;
	}

	len = 1+ length;//strlen((char *)number);
	if(len>ISDN_IE_CONTENT_LENGTH) 
		len = ISDN_IE_CONTENT_LENGTH;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
	
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->called_nr = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_CALLED_PN;
	p[sp++] = len;
	p[sp++] = 0x80 + (type<<4) + plan;
	UNCPY((char *)p+sp, (char *)content, length );
	
	return p;
}

int IeCalledPN::decode(const unsigned char *p)
{
	int msgLen;

	if (!p)
		return ERROR_IE_IS_NULL;

	msgLen = p[0];
	if ( msgLen < 2)
	{
		cpLog(LOG_ERR, "CalledPN IE too short (%d).\n", msgLen );
		return ERROR_IE_TOO_SHORT;
	}
	type = (p[1]&0x70) >> 4;	/* bit 4-6 */
	plan = p[1] & 0xf;			/* bit 0-3 */

	memset( content, 0, ISDN_IE_CONTENT_LENGTH);
	strnncpy((unsigned char *)content, (unsigned char *)p+2, msgLen-1, ISDN_IE_CONTENT_LENGTH);
	length = strlen((char *)content);

	return AS_OK;
}

const unsigned char *IeCalledPN::getPartyNumber()
{
	return getContent();
}

void IeCalledPN::setPartyNumber( const unsigned char *_number)
{
	return setContent(_number);
}

int IeCalledPN::getType()
{
	return type;
}

void IeCalledPN::setType(int _type)
{
	type = _type;
}

int IeCalledPN::getPlan()
{
	return plan;
}

void IeCalledPN::setPlan(int _plan)
{
	plan = _plan;
}

int IeCalledPN::validateType(int _type)
{
	if (_type == 0x1 )
		type = INFO_NTYPE_INTERNATIONAL;
	else if (_type == 0x2 )
		type = INFO_NTYPE_NATIONAL;
	else if(_type == 0x4 )
		type = INFO_NTYPE_SUBSCRIBER;
	else
		type = INFO_NTYPE_UNKNOWN;

	return 0;
}

		
#if WITH_DEBUG
const char *IeCalledPN::debugInfo()
{
	int length = 0;
//	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
//	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "PN IE info :");
	length += sprintf( p+ length, "\r\n\tPhone Number Length : %d('%s')", getLength(), getContent());
//	free((void *)tmp);
	length += sprintf( p+ length, "\ttype : %s", getTypeName());
	length += sprintf( p+ length, "\tplan : %s", getPlanName());

	return p;
}

const char *IeCalledPN::getTypeName()
{
	switch(type)
	{
		case PN_TYPE_INTERNATIONAL:
			return "INTERNATIONAL";
			break;
		case PN_TYPE_UNKNOWN:
			return "UNKNOWN";
			break;
		case PN_TYPE_NATIONAL:
			return "NATIONAL";
			break;
		case PN_TYPE_NETWORK:
			return "NETWORK";
			break;
		case PN_TYPE_SUBSCRIBE:
			return "SUBSCRIBE";
			break;
		default:
			break;
	}
	return "Unknown PN TYPE";
}

const char *IeCalledPN::getPlanName()
{
	switch(plan)
	{
		case PN_PLAN_E164:
			return "E164";
			break;
		case PN_PLAN_PRIVTAE:
			return "PRIVTAE";
			break;
		case PN_PLAN_UNKNOWN:
			return "UNKNOWN";
			break;
		default:
			break;
	}
	return "Unkonwn PN PLAN";
}

#endif

