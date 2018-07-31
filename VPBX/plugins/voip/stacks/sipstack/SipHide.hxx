#ifndef SIP_HIDE_HXX_
#define SIP_HIDE_HXX_

/*
* $Log: SipHide.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipHide.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{

enum SipHideErrorType
{
    DECODE_HIDE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipHideParserException
class SipHideParserException : public VException
{
    public:
        SipHideParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipHideParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipHideErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipHideErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipHideErrorType value;
};

/// data container for Hide header
class SipHide : public SipHeader
{
    public:
        /// Create one with default values
        SipHide();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipHide( const Data& srcData );
        SipHide( const SipHide& src );

        const SipHide& operator =(const SipHide& src);
        ///
        bool operator==(const SipHide& src) const;

        ///
        Data get() const;

        ///
        void set( const Data& newData);
        void scanHide(const Data & scandata);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data data;

        void decode(const Data& data);
        friend class SipCommand;
};

 
} 

#endif
