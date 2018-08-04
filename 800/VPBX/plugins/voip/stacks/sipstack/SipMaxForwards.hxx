#ifndef SipMaxForwards_HXX
#define SipMaxForwards_HXX

/*
* $Log: SipMaxForwards.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipMaxForwards.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipMaxForwardsErrorType
{
    DECODE_MAXFORWARDS_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipMaxForwardsParserException : public VException
{
    public:
        SipMaxForwardsParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipMaxForwardsParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipMaxForwardsErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipMaxForwardsErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipMaxForwardsErrorType value;
};


/// data container for MaxForwards header
class SipMaxForwards : public SipHeader
{
    public:
        /// Create one with default values
        SipMaxForwards();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipMaxForwards( const Data& srcData );
        ///
        SipMaxForwards( const SipMaxForwards& src );

        const SipMaxForwards& operator =(const SipMaxForwards& src);
        bool operator==(const SipMaxForwards& src) const;
        ///
        int get() const;
        ///
        void set( int newValue);
        ///
        void set( const Data& newValue);

        // return the new value, return -1 if the value is already 0, but don't
        // change it internally
        int decrement();
        
        void scanSipMaxForwards(const Data& data);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        int value;
        void decode(const Data& data);
        friend class SipCommand;
};

}

ostream&
operator<<(ostream& s, const Assist::SipMaxForwards &smf);

#endif
