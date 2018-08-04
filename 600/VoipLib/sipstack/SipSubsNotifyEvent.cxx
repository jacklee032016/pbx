/*
 * $Id: SipSubsNotifyEvent.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipSubsNotifyEvent.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipSubsNotifyEventParserException::getName( void ) const
{
    return "SipSubsNotfiyEventParserException";
}

SipSubsNotifyEvent::SipSubsNotifyEvent()
        :
        eventType()
{
}


SipSubsNotifyEvent::SipSubsNotifyEvent( const Data& srcData )
{
    try
    {
        decode(srcData);
        eventType = srcData;
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }
    }
}


SipSubsNotifyEvent::SipSubsNotifyEvent( const SipSubsNotifyEvent& src)
{
    eventType = src.getEventType();
}


SipSubsNotifyEvent::~SipSubsNotifyEvent()
{
}


bool
SipSubsNotifyEvent::operator==(const SipSubsNotifyEvent& src) const
{
    cpDebug(LOG_DEBUG_STACK, "SipSubsNotifyEvent ::operator ==");

    bool equal = false;
    cpDebug(LOG_DEBUG_STACK, "this.event type = %s", eventType.logData());
    cpDebug(LOG_DEBUG_STACK, "other.event type = %s", src.getEventType().logData());
    equal = (eventType == src.getEventType());

    cpDebug(LOG_DEBUG_STACK, "SubsNotifyEvent operator == final result: %d", equal);

    return equal;
}


SipSubsNotifyEvent &
SipSubsNotifyEvent::operator=(const SipSubsNotifyEvent& src)
{
    if (&src != this)
    {
        eventType = src.eventType;
    }
    return (*this);
}


Data
SipSubsNotifyEvent::getEventType() const
{
    return eventType;
}


void
SipSubsNotifyEvent::setEventType( const Data& item)
{
    eventType = item;
}


Data
SipSubsNotifyEvent::encode() const
{
    Data sipSubsNotifyEvent = EVENT;

    sipSubsNotifyEvent += SP;
    sipSubsNotifyEvent += eventType;

    sipSubsNotifyEvent += CRLF;

    return sipSubsNotifyEvent;
}


void
SipSubsNotifyEvent::decode( const Data& data )
{
    try
    {
        parse(data);
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");

            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }
    }
}


void
SipSubsNotifyEvent::parse( const Data& data)
{
    try
    {
        scanSipSubsNotifyEvent(data);
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");

            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }

    }
    //No parse problems.
}


void
SipSubsNotifyEvent::scanSipSubsNotifyEvent(const Data& data)
{
    eventType = data;
}


SipHeader*
SipSubsNotifyEvent::duplicate() const
{
    return new SipSubsNotifyEvent(*this);
}


bool
SipSubsNotifyEvent::compareSipHeader(SipHeader* msg) const
{
    SipSubsNotifyEvent* otherMsg = dynamic_cast<SipSubsNotifyEvent*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
