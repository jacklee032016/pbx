/*
* $Id: IeProgress.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"

#include "IeProgress.hxx"
#include "IsdnParserMode.hxx"

#include "l3ins.h"

using namespace Assist;


IeProgress::IeProgress( const unsigned char  *p)  throw(IsdnIeParserException &)
{
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in decode PROGRESS IE", __FILE__, __LINE__, res );
	}
}

IeProgress::IeProgress(int _coding, int _location, int _progress)
{
	coding = _coding;
	location = _location;
	progress = _progress;
}

IeProgress::IeProgress( const IeProgress& src )
{
	coding = src.coding;
	location = src.location;
	progress = src.progress;
}

IeProgress::~IeProgress( )
{
}

bool IeProgress::operator == (const IeProgress& src) const
{
	if ( ( coding == src.coding) &&
		( location == src.location) &&
		( progress == src.progress) )
		return true;
	else
		return false;

}

bool IeProgress::operator<(const IeProgress& src) const
{
	return false;
}

const IeProgress& IeProgress::operator=(const IeProgress& src)
{
	if ( &src != this)
	{
		coding = src.coding;
		location = src.location;
		progress = src.progress;
	}
	
	return (*this);
}


IsdnIE* IeProgress::duplicate() const
{
	return new IeProgress(*this);
}


bool IeProgress::compareIsdnIE(IsdnIE* msg) const
{
	IeProgress* otherMsg = dynamic_cast<IeProgress*>(msg);
	if(otherMsg != 0)
	{
		return (*this == *otherMsg);
	}
	else
	{
		return false;
	}
}


unsigned char *IeProgress::encode(msg_t *msg, int ntMode)
{
	unsigned char *p;
	int len;
	int tLen;
	int sp = 0;

	if (coding<0 || coding>0x03)
	{
		cpLog(LOG_ERR, "coding(%d) is out of range.\n", coding);
		return 0;
	}
	if (location<0 || location>0x0f)
	{
		cpLog(LOG_ERR, "location(%d) is out of range.\n", location);
		return 0;
	}
	if (progress<0 || progress>0x7f)
	{
		cpLog(LOG_ERR, "progress(%d) is out of range.\n", progress);
		return 0;
	}

	len = 2;
	tLen = len+1;
	if(ntMode==ISDN_DEV_TYPE_USER)
		tLen++;
	p = msg_put(msg, tLen);
#if 0	
	Q931_info_t *qi = (Q931_info_t *)(msg->data + AS_ISDN_HEADER_LEN);
	qi->progress = p - ( char *)qi - sizeof(Q931_info_t);
#endif
	if(ntMode==ISDN_DEV_TYPE_USER)
		p[sp++] = IE_PROGRESS;
	p[sp++] = len;
	p[sp++] = 0x80 + (coding<<5) + location;
	p[sp++] = 0x80 + progress;

	return p;
}

int IeProgress::decode(const unsigned char *p)
{
	if (!p)
		return ERROR_IE_IS_NULL;
#if 0
	if(p[0] !=  IE_PROGRESS)
	{
		cpLog(LOG_ERR, "Not CALLED PN IE (should be %x, but %x)", IE_PROGRESS, p[0] );
		return ERROR_IE_DATA_ERROR;
	}
#endif

	if (p[0] < 1)
	{
		cpLog(LOG_ERR, "IE too short (%d).\n", p[0]);
		return ERROR_IE_TOO_SHORT;
	}

	coding = (p[1]&0x60) >> 5;
	location = p[1] & 0x0f;
	
	progress = p[2] & 0x7f;

	return AS_OK;
}
		
#if WITH_DEBUG
const char *IeProgress::debugInfo()
{
	int length = 0;
	char *p = (char *)malloc(DEBUG_BUFFER_SIZE) ;
	assert(p!=0);
	
	length += sprintf( p+ length, "PROGRESS IE info :");
	length += sprintf( p+ length, "\tCoding : %d", coding );
	length += sprintf( p+ length, "\tLocation : %d", location );
	length += sprintf( p+ length, "\tProgress : %d", progress );

	return p;
}
#endif		


