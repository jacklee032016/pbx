#ifndef SipMimeVersion_HXX
#define SipMimeVersion_HXX

/*
* $Log: SipMimeVersion.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMimeVersion.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipMimeVersionErrorType
{
    DECODE_MIMEVERSION_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipMimeVersionParserException : public VException
{
    public:
        SipMimeVersionParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipMimeVersionParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipMimeVersionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipMimeVersionErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipMimeVersionErrorType value;
};


/// data container for MimeVersion header
class SipMimeVersion : public SipHeader
{
    public:
        /// Create one with default values
        SipMimeVersion();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipMimeVersion( const Data& srcData );
        ///
        SipMimeVersion( const SipMimeVersion& src );

        const SipMimeVersion& operator =(const SipMimeVersion& src);
        bool operator ==(const SipMimeVersion&) const ;
       
        ///
        Data get() const;
        ///
        void set( Data newValue);
        void scanSipMimeVersion(const Data& data);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;

	/// decode the data
        void decode(const Data& data);

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data value;
};

 
}

#endif
