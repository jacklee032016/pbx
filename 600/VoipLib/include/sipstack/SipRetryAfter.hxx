#ifndef SipRetryAfter_HXX
#define SipRetryAfter_HXX
/*
 * $Id: SipRetryAfter.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipDate.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipRetryAfterErrorType
{
    DECODE_RETRYAFTER_FAILED,
    SCAN_RETRYAFTER_FAILED
    //may need to change this to be more specific
};

class SipRetryAfterParserException : public VException
{
    public:
        SipRetryAfterParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipRetryAfterParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipRetryAfterErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipRetryAfterErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipRetryAfterErrorType value;

};
/// data container for RetryAfter header
class SipRetryAfter : public SipHeader
{
    public:
        /// Create one with default values
        SipRetryAfter();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipRetryAfter( const Data& srcData );
        ///
        SipRetryAfter& operator=(const SipRetryAfter& rhs);
        ///
        SipRetryAfter( const SipRetryAfter& src );
       
        ///
        bool operator ==(const SipRetryAfter& src) const;
        ///
        SipDate getDate() const;
        ///
        void parseDuration(const Data& datdata);
        ///
        void setDate( const SipDate& newDate);
        ///
        void parseDate(const Data& dadata);
        ///
        Data getComment() const;
        ///
        void scanRetryAfter(const Data &tmpdata);
        ///
        void setComment( const Data& newComment);
        ///
        int getDuration() const;
        ///
        void setDuration( const int newDuration);
        ///
        void setDurationData( const Data& newDuration);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Data getDurationData() const;
        ///
        SipDate date;  // if delta-seconds, convert and store as SipDate
        ///
        Data datedata;
        ///
        Data comment;
        ///
        Data duration;  // provide integer and Data accessorrs
        ///
        bool flagcomment;
        ///
        bool flagduration;
        ///
        void decode(const Data& retrydata);
        friend class SipMsg;
};

 
} // namespace Vocal

#endif
