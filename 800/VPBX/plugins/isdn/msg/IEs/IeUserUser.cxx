/*
* $Id: IeUserUser.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IsdnParserMode.hxx"
#include "IeUserUser.hxx"

#include "l3ins.h"

using namespace Assist;

IeUserUser::IeUserUser(  const unsigned char *p)  throw(IsdnIeParserException &) 
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode USERUSER IE", __FILE__, __LINE__, res );
	}
}


IeUserUser::IeUserUser( int _protocol, const unsigned char *_content)
	:IeContent(_content)
{
	protocol = _protocol;
	length = strlen((char *)_content);
}

IeUserUser::IeUserUser(const IeUserUser& src)
    : IeContent(src), 
    protocol(src.protocol)
{
}

IeUserUser::~IeUserUser( )
{
}


#if 0
IeUserUser::IeUserUser( const Data& data )
{
    try
    {
        decode(data);
    }
    catch (VException& e)
    {
        if (IsdnParserMode::isdnParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode Replaces header: '%s'", data.logData());
            throw IeUserUserParserException("failed to decode replaces header", __FILE__, __LINE__);
        }
    }
}
#endif

IeUserUser& IeUserUser::operator = (const IeUserUser& src)
{
	IeContent::operator=(src);
	protocol = src.protocol;
	return (*this);
}

bool IeUserUser::operator ==(const IeUserUser& src) const
{
	bool b = IeContent::operator==(src);
	if(b==true && protocol == src.protocol )
		return true;
	return false;
}

IsdnIE* IeUserUser::duplicate() const
{
	return new IeUserUser(*this);
}

bool IeUserUser::compareIsdnIE(IsdnIE* msg) const
{
	IeUserUser* otherMsg = dynamic_cast<IeUserUser*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeUserUser::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (protocol<0 || protocol>127)
	{
		cpLog(LOG_ERR, "protocol(%d) is out of range.\n", protocol);
		return 0;
	}

	if (!strlen((char *)content) || length<=0)
	{
		return NULL;
	}

	len = length;
	tLen = len +1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	
	p = msg_put(msg, tLen+1 );
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->useruser = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_USER_USER;
	p[sp++] = len;
	p[sp++] = 0x80 + protocol;
	memcpy(p+sp, content, len);

	return p;
}

int IeUserUser::decode(const unsigned char *p)
{
	int user_len =0;

	if (!p)
		return ERROR_IE_IS_NULL;

#if 0	
	if(p[0] !=  IE_USER_USER)
	{
		cpLog(LOG_ERR, "Not CALLED PN IE (should be %x, but %x)", IE_USER_USER, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	user_len = p[0]-1;
//	if (p[0] < 1)
//		return;
	protocol = p[1];
	memset(content, 0, ISDN_IE_CONTENT_LENGTH);
	memcpy(content, p+2, (user_len<=ISDN_IE_CONTENT_LENGTH)?(user_len):ISDN_IE_CONTENT_LENGTH); /* clip to 128 maximum */

	length = strlen((char *)content);
	
	return AS_OK;
}

#if WITH_DEBUG
const char *IeUserUser::debugInfo()
{
	int length = 0;
	const char *tmp;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	tmp = IeContent::debugInfo();

	length += sprintf( p+ length, "USER-USER IE info :");
	length += sprintf( p+ length, "\tProtocol : %d", protocol );
	length += sprintf( p+ length, "\tContent : %s", tmp );
	free((void *)tmp);

	return p;
}
#endif		


