#ifndef SipSession_HXX
#define SipSession_HXX
/*
* $Log: SipSession.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipSession.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "VException.hxx"
#include "SipHeader.hxx"

namespace Assist
{
    
enum SipSessionErrorType
{
    DECODE_SESSION_FAILED
};


/// Exception handling class
class SipSessionParserException : public VException
{
    public:
        SipSessionParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipSessionParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipSessionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSessionErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSessionErrorType value;

};

/// data container for Session header
class SipSession : public SipHeader
{
    public:
    ///
    SipSession();
    ///
    SipSession( const SipSession& src);
    ///
    SipSession( const Data& data );
    ///
    bool operator ==(const SipSession& other) const;
    ///
    SipSession& operator=( const SipSession& src);
    ///
    Data encode() const;
    ///
    bool isSessionMedia();
    ///
    bool isSessionQos();
    ///
    bool isSessionSecurity();
    ///
    void setSessionMedia(const bool&);
    ///
    void setSessionQos(const bool&);
    ///
    void setSessionSecurity(const bool&);
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
private:
    bool sessionMedia;
    bool sessionQos;
    bool sessionSecurity;
    
    void decode(const Data& data);
    friend class StatusMsg;
};

 
}

#endif
