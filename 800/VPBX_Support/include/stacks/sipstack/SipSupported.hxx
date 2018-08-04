#ifndef SipSupported_HXX
#define SipSupported_HXX
/*
* $Log: SipSupported.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSupported.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"

namespace Assist
{
    
enum SipSupportedErrorType
{
    DECODE_SUPPORTED_FAILED


    //may need to change this to be more specific
};

/// Exception handling class
class SipSupportedParserException : public VException
{
    public:
        SipSupportedParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipSupportedParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipSupportedErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSupportedErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSupportedErrorType value;

};
/// data container for Supported header
class SipSupported : public SipHeader
{
    public:
        /// Create one with default values
        SipSupported();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipSupported( const Data& srcData );
        SipSupported( const SipSupported& src );
        void scanSipSupported(const Data& data);
        ///
        Data get();

        ///
        void set( const Data& newData);
        ///
        SipSupported& operator=(const SipSupported& src);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        ///
        bool operator==(const SipSupported&) const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data data;
        void decode(const Data& data);
};

 
} 

#endif
