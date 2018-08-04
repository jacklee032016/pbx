#ifndef SipTimestamp_HXX
#define SipTimestamp_HXX
/*
 * $Id: SipTimestamp.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "SipHeader.hxx"

#include "Data.hxx"
#include "symbols.hxx"

namespace Vocal
{
    
enum SipTimestampErrorType
{
    DECODE_TIMESTAMP_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipTimestampParserException : public VException
{
    public:
        SipTimestampParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     const int error = 0 );
        SipTimestampParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     SipTimestampErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipTimestampErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipTimestampErrorType value;

};
/// data container for Timestamp header
class SipTimestamp : public SipHeader
{
    public:
        /// Create one with default values
        SipTimestamp();

        // TODO - add more constructors that take the parameters needed to build it
        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipTimestamp( const Data& srcData );
        SipTimestamp( const SipTimestamp& src );

        const SipTimestamp& operator=(const SipTimestamp& rhs)
        {
            if (this != &rhs)
            {
                time = rhs.time;
                delay = rhs.delay;
                ;

            }
            return *this;
        }

     
        ///
        bool operator ==( const SipTimestamp& src ) const;

        ///
        int getTime();

        ///
        void setTime( const int& newTime);
        void scanSipTimestamp(const Data& data);
        ///
        int getDelay();

        ///
        void setDelay( const int& newDelay);

        ///
        Data getTimeData();

        ///
        void setTimeData( const Data& newTime);

        ///
        Data getDelayData();

        ///
        void setDelayData( const Data& newDelay);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data time;
        Data delay;
        void decode(const Data& data);
        friend class SipMsg;
};

 
} // namespace Vocal

#endif
