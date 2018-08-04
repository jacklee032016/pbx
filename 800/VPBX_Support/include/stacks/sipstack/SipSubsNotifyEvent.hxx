#ifndef SipSubsNotifyEvent_HXX
#define SipSubsNotifyEvent_HXX
/*
* $Log: SipSubsNotifyEvent.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSubsNotifyEvent.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"



#include "Data.hxx"
#include "VException.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
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

 
} // namespace Assist


#endif
