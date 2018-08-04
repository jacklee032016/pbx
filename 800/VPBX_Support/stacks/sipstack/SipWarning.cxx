/*
* $Log: SipWarning.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipWarning.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipWarning.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipWarningParserException::getName( void ) const
{
    return "SipWarningParserException";
}


SipWarning::SipWarning()
        : code(),
        agent(),
        text()
{}



SipWarning::SipWarning( const Data& srcData )
        : code(),
        agent(),
        text()
{
    try
    {
        decode( srcData );
    }
    catch ( SipWarningParserException&)
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
}


void
SipWarning::decode( const Data& retryData )
{
    Data data = retryData;

    try
    {
        scanWarning( data );
    }
    catch ( SipWarningParserException& exception)
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_WARNING_FAILED );
        }
    }
}


void
SipWarning::scanWarning( const Data &tmpData )

{
    Data warnData = tmpData;
    Data warnValue;
    int retn = warnData.match( " ", &warnValue, true );
    if ( retn == FOUND)
    {
        setCode( warnValue.convertInt() );
        Data wAgent = warnData;
        Data wAgentValue;
        int test = wAgent.match( " ", &wAgentValue, true );
        if ( test == FOUND )
        {
            setAgent( wAgentValue );
            setText( wAgent );
        }
        else if ( test == NOT_FOUND )
        {
            if ( SipParserMode::sipParserMode() )
            {
                throw SipWarningParserException(
                    "failed to decode the Retry After string",
                    __FILE__,
                    __LINE__, DECODE_WARNING_FAILED );
            }
        }
        else if ( test == FIRST )
        {

            if ( SipParserMode::sipParserMode() )
            {
                throw SipWarningParserException(
                    "failed to decode the Retry After string",
                    __FILE__,
                    __LINE__, DECODE_WARNING_FAILED );
            }
        }
    }
    else if ( retn == NOT_FOUND )
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
    else if ( retn == FIRST )
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
}

SipWarning::SipWarning( const SipWarning& src )
{
    //copy all member variables
    code = src.code;
    agent = src.agent;
    text = src.text;
}


const SipWarning& SipWarning::operator=( const SipWarning& src )
{
    if ( &src != this )
    {
        code = src.code;
        agent = src.agent;
        text = src.text;
    }
    return *this;
}

/// 
bool SipWarning::operator ==(const SipWarning & src) const
{
  return (
	  ( code == src.code ) &&
	  ( agent == src.agent ) &&
	  ( text == src.text ) 
	  );
}
///
int SipWarning::getCode()
{
    return code.convertInt();
}

///
void SipWarning::setCode( const int& newCode )
{
    code = Data(newCode);
}

///
Data SipWarning::getCodeData()
{
    return code;
}

///
void SipWarning::setCodeData( const Data& newCode )
{
    code = newCode;
}

///
Data SipWarning::getAgent()
{
    return agent;
}

///
void SipWarning::setAgent( const Data& newAgent )
{
    agent = newAgent;
}

///
Data SipWarning::getText()
{
    return text;
}

///
void SipWarning::setText( const Data& newText )
{
    text = newText;
}

/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data SipWarning::encode() const
{
    Data ret;
    ret += SIP_WARNING;
    ret += code;
    ret += SP;
    ret += agent;
    ret += SP;
    ret += text;

    return ret;
}


SipHeader*
SipWarning::duplicate() const
{
    return new SipWarning(*this);
}


bool
SipWarning::compareSipHeader(SipHeader* msg) const
{
    SipWarning* otherMsg = dynamic_cast<SipWarning*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
