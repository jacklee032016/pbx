/*
* $Id: IeCallState.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "IeCallState.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"
#include "as_isdn.h"

using namespace Assist;

IeCallState::IeCallState( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode NOTIFY IE", __FILE__, __LINE__, res );
	}
}

#if 0
IeCallState::IeCallState(int _notify)
{
	notify = _notify;
}
#endif

IeCallState::IeCallState( const IeCallState & src)
{
}

IeCallState& IeCallState::operator=(const IeCallState& src)
{
	if ( &src != this)
	{
		coding = src.coding;
		callStateValue = src.callStateValue;
	}
	return (*this);
}

bool IeCallState::operator==(const IeCallState& src) const
{
	if (coding==src.coding && callStateValue == src.callStateValue )
	{
		return true;
	}
	else
	{
		return false;
	}
}


IsdnIE* IeCallState::duplicate() const
{
	return new IeCallState(*this);
}

bool IeCallState::compareIsdnIE(IsdnIE* msg) const
{
	IeCallState* otherMsg = dynamic_cast<IeCallState*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
} 

unsigned char *IeCallState::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (validate()==false)
	{
		cpLog(LOG_ERR, "notify(%d) is out of range.\n", callStateValue);
		return 0;
	}

	len = 1;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen );

#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->call_state = p - ( char *)qi - sizeof(Q931_info_t);
#endif

	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_CALL_STATE;
	p[sp++] = len;
	p[sp++] = (0xC0&coding)|(0x3f&callStateValue) ;

	return p;
}

int IeCallState::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
#if 0
	if(p[0] !=  IE_CALL_STATE)
	{
		cpLog(LOG_ERR, "Not CALL STATE IE (should be %x, but %x)", IE_CALL_STATE, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "IE NOTIFY too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	coding = (p[1]&0xC0) >> 6;		/* bit 6-7 */
	callStateValue = p[2] & 0x3f;		/* last 6 bits */

	if (validate()==false)
	{
		cpLog(LOG_ERR, "CallState(%d) is out of range.\n", callStateValue );
		return ERROR_IE_NOTIFY_ERROR;
	}
	
	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeCallState::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	

	length += sprintf( p+ length, "CALLSTATE IE info :");
	length += sprintf( p+ length, "\tCoding : %s", (coding==IE_CALLSTATE_CODING_TTC)?"TTC":"Invalidate");

	length += sprintf( p+ length, "\tCall State : %d", callStateValue );

	return p;
}
#endif		

bool IeCallState::validate()
{
	if (coding == IE_CALLSTATE_CODING_TTC && callStateValue<=IE_CALLSTATE_VALUE_N_CALL_ABORT
		&& callStateValue>=IE_CALLSTATE_VALUE_NULL)
		return true;
	return false;
}

