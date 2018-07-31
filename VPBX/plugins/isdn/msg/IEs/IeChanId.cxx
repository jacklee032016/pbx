/*
* $Id: IeChanId.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "IeChanId.hxx"

#include "IsdnAudioChannel.hxx"
#include "l3ins.h"
#include "vpbx_globals.h"

using namespace Assist;

IeChanId::IeChanId( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in Decode ChanID IE", __FILE__, __LINE__, res );
	}
}


IeChanId::IeChanId(const IeChanId& src)
	: channel(src.channel),
	exclusive(src.exclusive)
{}

IeChanId::IeChanId(int _exclusive, int _chanId)
{
	channel = _chanId;//RandomHex::get(NUM_RANDOMNESS);
	exclusive = _exclusive;
}


const IeChanId& IeChanId::operator=(const IeChanId& src)
{
	if ( &src != this)
	{
		channel = src.channel;
		exclusive = src.exclusive;
	}
	return (*this);
}

bool IeChanId::operator < ( const IeChanId& other ) const
{
	return false;
}

bool IeChanId::operator > ( const IeChanId& other ) const
{
	return false;
}

bool IeChanId::operator== ( const IeChanId& other ) const
{
	cpLog(LOG_DEBUG_STACK, "IeChanId == operator");

	bool equal = false;
	equal = (( channel == other.channel) && ( exclusive == other.exclusive) );

	cpLog(LOG_DEBUG_STACK, "IeChanId == operator, returning %d", equal);
	return equal;
}


IsdnIE* IeChanId::duplicate() const
{
	return new IeChanId(*this);
}

bool IeChanId::compareIsdnIE(IsdnIE* msg) const
{
	IeChanId* otherMsg = dynamic_cast<IeChanId*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}

unsigned char *IeChanId::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int sp = 0;
	int tLen;

	if (exclusive<0 || exclusive>1)
	{
		cpLog(LOG_ERR, "exclusive(%d) is out of range.\n", exclusive);
		return 0;
	}
	
	if (channel<CHAN_ID_B1_CHANNEL || channel>CHAN_ID_B2_CHANNEL )
	{
		cpLog(LOG_ERR, "channel(%d) is out of range.\n", channel);
		return 0;
	}

	len = 1;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->channel_id = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_CHANNEL_ID;
	p[sp++] = len;
	if (channel == 0xff)
		channel = 3;

	p[sp] = 0x80 + (exclusive<<3) + channel;

	return p;
}

int IeChanId::decode(const unsigned char *p)
{
	int len;
	
	if (!p)
		return ERROR_IE_IS_NULL;

#if 0
	if(p[0] !=  IE_CHANNEL_ID)
	{
		cpLog(LOG_ERR, "Not CALLED PN IE (should be %x, but %x)", IE_CHANNEL_ID, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	len = p[0];

	if ( len < 1)
	{
		cpLog(LOG_ERR, "IE too short (%d).\n", len );
		return ERROR_IE_TOO_SHORT;
	}

	if (p[1] & 0x40)
	{
		cpLog(LOG_ERR, "refering to channels of other interfaces is not supported.\n");
		return ERROR_IE_CHANID_ERROR;
	}

	/*D Channel Indicator */
	if (p[1] & 0x04)	
	{/* chan ID in bit0-1 is D Channel ID */
		cpLog(LOG_ERR, "using d-channel is not supported.\n");
		return ERROR_IE_CHANID_ERROR;
	}

	exclusive = (p[1]&0x08) >> 3;

	if (p[1] & 0x20)
	{
		cpLog(LOG_ERR, "extended channel ID with non PRI interface.\n");
		return ERROR_IE_CHANID_ERROR;
	}

	channel = p[1] & 0x03;
#if 0	
	if ( channel == 3)
		channel = CHAN_ID_ANY_CHANNEL;
#endif

	if( exclusive !=CHAN_ID_MODE_PREFER && exclusive != CHAN_ID_MODE_EXCLUSIVE )
	{
		cpLog(LOG_ERR, "EXCLUSIVE are not validate value");
		return ERROR_IE_CHANID_ERROR;
	}

	if( channel !=CHAN_ID_B1_CHANNEL && channel != CHAN_ID_B2_CHANNEL && channel != CHAN_ID_ANY_CHANNEL )
	{
		cpLog(LOG_ERR, "CHAN_ID are not validate value");
		return ERROR_IE_CHANID_ERROR;
	}

	if ( exclusive<0 ||channel < 0)
	{
		exclusive = 0;
		channel = CHAN_ID_ANY_CHANNEL; /* any channel */
	}
	return AS_OK;
}

		
#if WITH_DEBUG
const char *IeChanId::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	

	length += sprintf( p+ length, "ChanID IE info :");
	length += sprintf( p+ length, "\tChannel : %d",  channel );
	length += sprintf( p+ length, "\tExclusive : %s", (exclusive==CHAN_ID_MODE_EXCLUSIVE)?"Exclusive":"Preferred");

	return p;
}
#endif		

