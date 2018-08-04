#ifndef SipExpires_HXX
#define SipExpires_HXX
/*
 * $Id: SipExpires.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{

enum SipExpiresErrorType
{
    DECODE_EXPIRE_FAILED

    //may need to change this to be more specific
};


/// Exception handling class SipExpiresParserException
class SipExpiresParserException : public VException
{
    public:
        SipExpiresParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipExpiresParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipExpiresErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipExpiresErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipExpiresErrorType value;
};


/// data container for Expires header
class SipExpires : public SipHeader
{
    public:
        /// Create one with default values
        SipExpires();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipExpires( const Data& srcData );
        SipExpires( const SipExpires& src );

        ///
        bool operator ==( const SipExpires& src ) const;

        bool operator <( const SipExpires& src ) const;

        ///
        const SipExpires& operator =(const SipExpires& src);
        ///
        void scanSipExpires(const Data& data);

        ///
        void setDelta( const Data& data);
        ///
        Data getDelta() const;
        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;
        ///
        void decode(const Data& data);

        ///
        Data getData() const;
        ///

	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;


    private:
        // always store as SipDate, if delta convert to date
        Data delta;

        friend class SipMsg;
};
 
} // namespace Vocal

#endif
