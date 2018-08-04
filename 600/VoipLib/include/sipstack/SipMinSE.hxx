#ifndef SipMinSE_HXX
#define SipMinSE_HXX
/*
 * $Id: SipMinSE.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipMinSEErrorType
{
    DECODE_MIN_SE_FAILED

    //may need to change this to be more specific
};


/// Exception handling class
class SipMinSEParserException : public VException
{
    public:
        SipMinSEParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipMinSEParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipMinSEErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipMinSEErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipMinSEErrorType value;
};


/// data container for MinSE header
class SipMinSE : public SipHeader
{
    public:
        /// Create one with default values
        SipMinSE();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipMinSE( const Data& srcData );
        SipMinSE( const SipMinSE& src );

        ///
        bool operator ==( const SipMinSE& src ) const;
        ///
        const SipMinSE& operator =(const SipMinSE& src);
        ///
        void scanSipMinSE(const Data& data);

        ///
        void setDelta( const Data& data);
        ///
        const Data& getDelta() const;

        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;
        ///
        void decode(const Data& data);

        ///
        Data getData() const;


	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;

        ///
        void parseError(const string& context);

    private:
        Data delta;
        bool bDelta;

        friend class SipMsg;
};
 
} // namespace Vocal

#endif
