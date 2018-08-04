#ifndef SipUserAgent_HXX
#define SipUserAgent_HXX
/*
* $Log: SipUserAgent.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUserAgent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"

namespace Assist
{
    
///
enum SipUserAgentErrorType
{
    DECODE_USERAGENT_FAILED,
    SCAN_USERAGENT_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipUserAgentParserException : public VException
{
    public:
        ///
        SipUserAgentParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     const int error = 0 );
        ///
        SipUserAgentParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     SipUserAgentErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipUserAgentErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        SipUserAgentErrorType value;

};


/// data container for UserAgent header
class SipUserAgent: public SipHeader
{
    public:
        /// Create one with default values
        SipUserAgent();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipUserAgent( const Data& srcData );
        ///
        SipUserAgent( const SipUserAgent& src );

        ///
        const SipUserAgent& operator=(const SipUserAgent& rhs);

	/// comparison operator
	bool operator==(const SipUserAgent& src) const;

        ///
        const Data& getProduct() const;
        ///
        const Data& getComment() const;
        ///
        const Data& getVersion() const;
        ///
        void setProduct( const Data& newData);
        ///
        void setComment( const Data& newcomment);
        ///
        void setVersion (const Data& newversion);
        ///
        void scanSipUserAgent(const Data &tmpdata);
        /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
        ///
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

        friend class SipMsg; 
};
 
} // namespace Assist

#endif
