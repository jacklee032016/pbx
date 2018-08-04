/*
 * $Id: SipSession.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipSession.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipSessionParserException::getName( void ) const
{
    return "SipSessionParserException";
}

SipSession::SipSession( const SipSession& src)
        :
        sessionMedia(src.sessionMedia),
        sessionQos(src.sessionQos),
        sessionSecurity(src.sessionSecurity)
{}



bool
SipSession::operator ==(const SipSession& other) const
{
    if ( (sessionMedia == other.sessionMedia) &&
	 (sessionQos == other.sessionQos) &&
	 (sessionSecurity == other.sessionSecurity)
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}



SipSession&
SipSession::operator=( const SipSession& src)
{
    if (&src != this)
    {
        sessionMedia = src.sessionMedia;
        sessionQos = src.sessionQos;
        sessionSecurity = src.sessionSecurity;
    }

    return *this;
}


SipSession::SipSession()
    :sessionMedia(false),
     sessionQos(false),
     sessionSecurity(false)
{
}



SipSession::SipSession( const Data& data )
    :sessionMedia(false),
     sessionQos(false),
     sessionSecurity(false)
{
    try
    {
        decode(data);
    }
    catch (SipSessionParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipSessionParserException("failed in Decode", __FILE__, __LINE__, DECODE_SESSION_FAILED);

        }


    }
}


void
SipSession::decode( const Data& ctypestr )
{
    if (ctypestr == SESSION_MEDIA)
    {
	setSessionMedia(true);
    }
    else if (ctypestr == SESSION_QOS)
    {
	setSessionQos(true);
    }
    else if (ctypestr == SESSION_SECURITY)
    {
	setSessionSecurity(true);
    }
    else
    {
	//throw --
	 cpLog(LOG_ERR, "Not any of Media, Qos, Security ");
            throw SipSessionParserException("failed in Decode", __FILE__, __LINE__, DECODE_SESSION_FAILED);
    }
}


Data SipSession::encode() const
{
    Data session;
    if ( (sessionMedia) || (sessionQos) || (sessionSecurity) )
    {
        session = SESSION;
        session+= SP;
        if (sessionMedia)
        {
	    session+= SESSION_MEDIA;
        }
        if (sessionQos)
        {
	    session+= SESSION_QOS;
        }
        if (sessionSecurity)
        {
	    session+= SESSION_SECURITY;
        }
        session += CRLF;
    }
    return session;
}


void SipSession::setSessionMedia(const bool& newsessionMedia)
{
    sessionMedia = newsessionMedia;
}


void SipSession::setSessionQos(const bool& newsessionQos)
{
    sessionQos = newsessionQos; 
}
    
void SipSession::setSessionSecurity(const bool& newsessionSecurity)
{
    sessionSecurity = newsessionSecurity;
}
    
bool SipSession::isSessionMedia()
{
    return sessionMedia;
}
    
    
bool SipSession::isSessionSecurity()
{
    return sessionSecurity;
}
    
    
bool SipSession::isSessionQos()
{
    return sessionQos;
}    


SipHeader*
SipSession::duplicate() const
{
    return new SipSession(*this);
}


bool
SipSession::compareSipHeader(SipHeader* msg) const
{
    SipSession* otherMsg = dynamic_cast<SipSession*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
