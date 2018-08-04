#ifndef SipContentLength_HXX
#define SipContentLength_HXX
/*
* $Log: SipContentLength.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentLength.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Assist
{


enum SipContentLengthErrorType
{
    DECODE_CONTENTLENGTH_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentLengthParserException : public VException
{
    public:
        SipContentLengthParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );

        SipContentLengthParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         SipContentLengthErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }

        SipContentLengthErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentLengthErrorType value;
};

/// data container for ContentLength header
class SipContentLength : public SipHeader
{
    public:
        /// Create one with default values
        SipContentLength();
        ///
        SipContentLength( const SipContentLength& src);
        ///
        SipContentLength( const Data& data );
        ///
        void parse(const Data& data);
        ///
        Data encode() const;
        ///
        void setLengthData(const Data& contentlngth);
        ///
        void setLength(int contentlngth);
        ///
        Data getLength() const;
        ///
        bool operator ==( const SipContentLength& src) const;
        ///
        const SipContentLength& operator=( const SipContentLength& src);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data contentLength;
        void decode(const Data& data);
        friend class SipMsg;
};
 
}

#endif
