/*
 * $Id: SipCallId.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "global.h"
#include "SipCallId.hxx"
#include "SipMsg.hxx"
#include "symbols.hxx"
#include "HostMatch.hxx"
#include "RandomHex.hxx"
#include "SystemInfo.hxx"

#include "support.hxx"

using namespace Vocal;

#define NUM_RANDOMNESS 16  //128 bits of randomness

string
SipCallIdParserException::getName( void ) const
{
    return "SipCallIdParserException";
}



SipCallId::SipCallId(const SipCallId& src)
        :
        localid(src.localid),
        host(src.host)
{}


SipCallId::SipCallId()
{
    localid = RandomHex::get(NUM_RANDOMNESS);
    host = theSystem.gethostAddress();
}

SipCallId::SipCallId( const Data& data )
{


    try
    {
        decode(data);
    }
    catch (SipCallIdParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipCallIdParserException("failed in Decode", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }

}

void
SipCallId::parseLocalId(const Data & ldata)
{
    setLocalId(ldata);
}
void
SipCallId::parseHost(const Data & data)
{
    setHost(data);
}
void
SipCallId::scanSipCallId( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("@", &sipdata, true);
    if (ret == FOUND)
    {
        parseLocalId(sipdata);
        parseHost(data);
    }
    else if (ret == NOT_FOUND)
    {

        parseLocalId(data);


    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item @ not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }


    }
}

void
SipCallId::parse( const Data& calliddata)
{

    Data data = calliddata;

    try
    {

        scanSipCallId(data);
    }
    catch (SipCallIdParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }


    //everything allright.


}


void
SipCallId::decode( const Data& callidstr )
{

    try
    {
        parse(callidstr);
    }
    catch (SipCallIdParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }
}

Data SipCallId::encode() const
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

void SipCallId::setData(const Data& data)
{
    decode(data);
}

Data SipCallId::getData() const
{
    Data data;
    if (getLocalId().length())
    {
        data = CALLID;
        data += SP;
        data += getCallIdData();
    }
    return data;
}
Data SipCallId::getCallIdData() const
{
    Data ret;
    if (getLocalId().length())
    {
        ret += getLocalId();
    }
    if (getHost().length())
    {
        ret += "@";
        ret += getHost();
    }
    return ret;
}
const SipCallId&
SipCallId::operator=(const SipCallId& src)
{
    if ( &src != this)
    {
        localid = src.localid;
        host = src.host;
        hash = src.hash;
    }
    return (*this);
}
///
bool
SipCallId::operator < ( const SipCallId& other ) const
{
    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

    if ( selfstr < otherstr)
    {
        return true;
    }
    return false;
}


///
bool
SipCallId::operator > ( const SipCallId& other ) const
{
    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

    if ( selfstr > otherstr )
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
bool
SipCallId::operator== ( const SipCallId& other ) const
{
    cpDebug(LOG_DEBUG_STACK, "SipCallId == operator");

    bool equal = false;

    equal = ((localid == other.localid) && (host == other.host));

    cpDebug(LOG_DEBUG_STACK, "SipCallId == operator, returning %d", equal);

    return equal;
}

#if USE_HASH_MAP
size_t SipCallId::hashfn() const
{

    return localid.hashfn() ^ host.hashfn();
}
#endif


SipHeader*
SipCallId::duplicate() const
{
    return new SipCallId(*this);
}


bool
SipCallId::compareSipHeader(SipHeader* msg) const
{
    SipCallId* otherMsg = dynamic_cast<SipCallId*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


ostream& operator<<(ostream& strm, const SipCallId& id)
{
   strm << id.encode();
   return strm;
}
