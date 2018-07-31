#ifndef SipServer_HXX
#define SipServer_HXX
/*
* $Log: SipServer.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipServer.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{
    
enum SipServerErrorType
{
    DECODE_SERVER_FAILED,
    SCAN_SERVER_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipServerParserException : public VException
{
    public:
        SipServerParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  const int error = 0 );
        SipServerParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  SipServerErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipServerErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipServerErrorType value;

};
/// data container for Server header
class SipServer : public SipHeader
{
    public:
        /// Create one with default values
        SipServer();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipServer( const Data& srcData );
        ///
        SipServer( const SipServer& src );
        ///
        SipServer& operator=( const SipServer& src );
        ///
        bool operator==( const SipServer& src ) const;
        ///
        void scanSipServer(const Data &tmpdata);

        ///
        Data getProduct();
        ///
        Data getComment();
        ///
        Data getVersion();
        ///
        void setProduct( const Data& newData);
        ///
        void setComment( const Data& newcomment);
        ///
        void setVersion (const Data& newversion);

        ///
        Data get();

        ///
        void set( const Data& newData);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data data;
        Data product;
        Data version;
        Data comment;
        bool flgcomment;
        bool flgproduct;
        void decode(const Data& data);
        friend class StatusMsg;
};

 
} 

#endif
