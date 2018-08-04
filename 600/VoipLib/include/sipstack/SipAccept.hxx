#ifndef SipAccept_HXX
#define SipAccept_HXX
/*
 * $Id: SipAccept.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{

enum SipAcceptErrorType
{
    DECODE_ACCEPT_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipAcceptParserException : public VException
{
    public:
        SipAcceptParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  const int error = 0 );
        SipAcceptParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  SipAcceptErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAcceptErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAcceptErrorType value;

};

/// data container for Accept header
class SipAccept : public SipHeader
{
    public:
        /// Create one with default values
        SipAccept();
        ///
        const SipAccept& operator =(const SipAccept&);
        ///
        bool operator ==( const SipAccept& src) const;
        ///
        SipAccept( const SipAccept& src);

        SipAccept( const Data& tdata);

        Data getMediaType() const;
        ///
        void setMediaType(const Data& srcmediaType);
        ///
        Data getMediaSubtype() const;
        ///
        void setMediaSubtype(const Data& srcmediaSubtype);
        ///
        Data getqValue() const;
        ///
        void setqValue(const Data& srcqValue);
        ///
        Data getMediaToken() const;
        ///
        void setMediaToken(const Data& srcToken);
        ///
        Data getMediaTokenValue() const;
        ///
        void setMediaTokenValue(const Data& newmediatokenvalue); // {mediatokenValue=newmediatokenvalue;}
        ///
        void setTokenValue(const Data& srcTokenValue);
      
        ///
        void setAcceptExtensionParm(const Data& newaccepttoken);
        ///
        void setAcceptExtensionValue(const Data& newaccepttokenvalue);
        ///
        Data getAccpetExtensionParm() const;
        ///
        Data getAccpetExtensionValue() const;
        ///
        Data encode() const;
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;


	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        
        Data mediaType;
        Data mediaSubtype;
        Data qValue;
        Data mediatoken;
        Data mediatokenValue;
        Data accepttoken;
        Data accepttokenValue;
        /// used for parsing
        bool allmedia; 
        ///used for parsing
        bool flagmedia;
        void decode(const Data& data);

        ///Parse functions
        void scanAccept(const Data &tmpdata);
        ///
        void parseMediaParm(const Data& tmpdata);
        ///
        void parseMediaParms(const Data& data, const Data& value);
        ///
        void parseMediaExtension(const Data& extensiondata, const Data& extensionvalue);
        ///
        void parseAcceptParm(const Data & tempdata);
        ///
        void parseAcceptParms(const Data& data);
        ///
        void parseAcceptExtension(const Data& extensiondata, const Data& extensionvalue);
};
 
} // namespace Vocal

#endif
