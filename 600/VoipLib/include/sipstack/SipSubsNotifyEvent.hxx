#ifndef SipSubsNotifyEvent_HXX
#define SipSubsNotifyEvent_HXX
/*
 * $Id: SipSubsNotifyEvent.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "VException.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{
    
enum SipSubsNotifyEventErrorType
{
    DECODE_EVENT_FAILED
};

/// Exception handling class
class SipSubsNotifyEventParserException : public VException
{
    public:
        SipSubsNotifyEventParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           const int error = 0 );
        SipSubsNotifyEventParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           SipSubsNotifyEventErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSubsNotifyEventErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSubsNotifyEventErrorType value;

};

/// data container for Event header
class SipSubsNotifyEvent : public SipHeader
{
    public:
        /// Create one with default values
        SipSubsNotifyEvent();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipSubsNotifyEvent( const Data& srcData );
        SipSubsNotifyEvent( const SipSubsNotifyEvent& src);

        virtual ~SipSubsNotifyEvent();

        ///
        bool operator ==( const SipSubsNotifyEvent& src) const;
        ///
        SipSubsNotifyEvent& operator = (const SipSubsNotifyEvent& src);

        ///
        Data getEventType() const;
        ///
        void setEventType( const Data& item);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data eventType;

        void decode(const Data& data);
        void parse(const Data& data);
        void scanSipSubsNotifyEvent(const Data& data);
};

 
} // namespace Vocal

#endif
