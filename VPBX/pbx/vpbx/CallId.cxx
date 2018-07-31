/*
 * $Id: CallId.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "cpLog.h"
#include "CallId.hxx"
#include "RandomHex.hxx"
#include "AudioChannel.hxx"

#include "support.hxx"

const char * const SP = " ";
const char * const EMPTY = "";
const char * const CRLF = "\r\n";

using namespace Assist;

#define NUM_RANDOMNESS 16  //128 bits of randomness

string CallIdParserException::getName( void ) const
{
	return "CallIdParserException";
}

CallId::CallId()
{
	localid = RandomHex::get(NUM_RANDOMNESS);
	audioChannel = NULL;
}

CallId::~CallId()
{
	cpLog(LOG_DEBUG, "CallId is deconstructing....." );
}

CallId::CallId(const CallId& src)
        : localid(src.localid)        
{}

CallId::CallId( const Data& data )
{
	try
	{
		decode(data);
	}
	catch (CallIdParserException&)
	{
//		if (IsdnParserMode::isdnParserMode())
		{
			cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
			throw CallIdParserException("failed in Decode", __FILE__, __LINE__, DECODE_VPBXCALLID_FAILED);
		}
	}
}

void CallId::parseLocalId(const Data & ldata)
{
	setLocalId(ldata);
}

void CallId::parse( const Data& calliddata)
{
	Data data = calliddata;

	try
	{
//		scanCallId(data);
		;
	}
	catch (CallIdParserException exception)
	{
//		if (IsdnParserMode::isdnParserMode())
		{
			cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
			throw CallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_VPBXCALLID_FAILED);
		}
	}

}

void CallId::decode( const Data& callidstr )
{
	try
	{
		parse(callidstr);
	}
	catch (CallIdParserException exception)
	{
//		if (IsdnParserMode::isdnParserMode())
		{
			cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
			throw CallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_VPBXCALLID_FAILED);
		}
	}
}

Data CallId::encode() const
{
	if (getData().length())
	{
		return (getData()+ CRLF);
	}
	else
	{
		return (getData());
	}
}

void CallId::setData(const Data& data)
{
	decode(data);
}

Data CallId::getData() const
{
	Data data;
	if (getLocalId().length())
	{
//		data = CALLID;
		data += SP;
		data += getCallIdData();
	}
	return data;
}

Data CallId::getCallIdData() const
{
	Data ret;
	if (getLocalId().length())
	{
		ret += getLocalId();
	}
/*
	if (getHost().length())
	{
		ret += "@";
		ret += getHost();
	}
*/	
	return ret;
}
const CallId& CallId::operator=(const CallId& src)
{
	if ( &src != this)
	{
		localid = src.localid;
//		host = src.host;
		hash = src.hash;
	}
	return (*this);
}

bool CallId::operator < ( const CallId& other ) const
{
	Data selfstr = (const_cast < CallId* > (this))->encode(); 
	Data otherstr = (const_cast < CallId& > (other)).encode(); 

	if ( selfstr < otherstr)
	{
		return true;
	}
	return false;
}

/*
bool CallId::operator > ( const CallId& other ) const
{
	Data selfstr = (const_cast < CallId* > (this))->encode(); 
	Data otherstr = (const_cast < CallId& > (other)).encode();

	if ( selfstr > otherstr )
	{
		return true;
	}
	else
	{
		return false;
	}
}
*/

bool CallId::operator== ( const CallId& other ) const
{
	cpLog(LOG_DEBUG_STACK, "CallId == operator");
	bool equal = false;

	equal = ((localid == other.localid)); // && (host == other.host));
	cpLog(LOG_DEBUG_STACK, "CallId == operator, returning %d", equal);

	return equal;
}


#if USE_HASH_MAP
size_t CallId::hashfn() const
{
	return localid.hashfn();// ^ host.hashfn();
}
#endif

ostream& operator<<(ostream& strm, const CallId& id)
{
	strm << id.encode();
	return strm;
}

