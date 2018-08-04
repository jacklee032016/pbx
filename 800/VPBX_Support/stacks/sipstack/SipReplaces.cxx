/*
* $Log: SipReplaces.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReplaces.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "SipParserMode.hxx"
#include "SipReplaces.hxx"
#include "symbols.hxx"

using namespace Assist;


SipReplaces::SipReplaces()
{
}

SipReplaces::SipReplaces(const SipReplaces& src)
    : callId(src.callId),
      params(src.params)
{
}

SipReplaces::SipReplaces( const Data& data )
{
    try
    {
        decode(data);
    }
    catch (VException& e)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode Replaces header: '%s'", data.logData());
            throw SipReplacesParserException("failed to decode replaces header", __FILE__, __LINE__);
        }
    }
}

void
SipReplaces::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data;
    Data sdata = tmpdata;
    int ret = sdata.match(":", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        throw SipReplacesParserException("failed in Decode", __FILE__, __LINE__);
    }
    else if (ret == FOUND) // stripped off the Replaces: header now
    {
        // !jf! should parse as a SipCallId
        int ret = sdata.match(";", &callId, true);
        if (ret == FOUND)
        {
            params.decode(sdata);
        }
        else
        {
            throw SipReplacesParserException("failed in Decode", __FILE__, __LINE__);
        }
            
        if (params.getValue("to-tag").length() == 0)
        {
            throw SipReplacesParserException("no to-tag in SipReplaces", __FILE__, __LINE__);                
        }
        if (params.getValue("from-tag").length() == 0)
        {
            throw SipReplacesParserException("no from-tag in SipReplaces", __FILE__, __LINE__);                
        }
    }
}

const Data& 
SipReplaces::getCallId() const
{
    return callId;
}

const Data 
SipReplaces::getFromTag() const
{
    return params.getValue("from-tag");
}

const Data 
SipReplaces::getToTag() const
{
    return params.getValue("to-tag");
}


void
SipReplaces::decode( const Data& data )
{
    try
    {
        Data pdata = REPLACES;
        pdata += ":";
        pdata += data;
        parse(pdata);
    }
    catch (SipReplacesParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode Replaces header: '%s'", data.logData());
            throw SipReplacesParserException("failed to decode replaces header", __FILE__, __LINE__);
        }
    }
}


SipReplaces& SipReplaces::operator = (const SipReplaces& srcReplaces)
{
    if ( &srcReplaces != this )
    {
        callId = srcReplaces.callId;
        params = srcReplaces.params;
    }
    return (*this);
}

bool SipReplaces::operator ==(const SipReplaces& srcReplaces) const
{
    // !jf! may need to do something special with from-tag and to-tag
    return srcReplaces.callId == callId && srcReplaces.params == params;
}

Data SipReplaces::encode() const
{
    Data data = REPLACES;
    data += ":";
    data += SP;
    
    data += callId;
    data += SEMICOLON;
    
    data += params.encode();

    data += CRLF;

    return data;
}

SipHeader*
SipReplaces::duplicate() const
{
    return new SipReplaces(*this);
}

bool
SipReplaces::compareSipHeader(SipHeader* msg) const
{
    SipReplaces* otherMsg = dynamic_cast<SipReplaces*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

