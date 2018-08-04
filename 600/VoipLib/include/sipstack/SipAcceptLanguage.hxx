#ifndef SipAcceptLanguage_HXX
#define SipAcceptLanguage_HXX
/*
 * $Id: SipAcceptLanguage.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "symbols.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{

enum SipAcceptLanguageErrorType
{
    DECODE_ACCEPTLANG_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipAcceptLanguageParserException : public VException
{
    public:
        SipAcceptLanguageParserException( const string& msg,
                                          const string& file,
                                          const int line,
                                          const int error = 0 );
        SipAcceptLanguageParserException( const string& msg,
                                          const string& file,
                                          const int line,
                                          SipAcceptLanguageErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAcceptLanguageErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAcceptLanguageErrorType value;

};
/// data container for AcceptLanguage header
class SipAcceptLanguage : public SipHeader
{
    public:
        /// Create one with default values
        SipAcceptLanguage();

      
       /// Create by decoding the data string passed in. This is the decode or parse.
        SipAcceptLanguage( const Data& srcData );
        ///
        SipAcceptLanguage( const SipAcceptLanguage& src);

        ///
        const SipAcceptLanguage& operator =(const SipAcceptLanguage& src);

        bool operator ==( const SipAcceptLanguage& src) const;
        ///
        void setData( const Data& newdata);
        ///
        void setPrimaryTag(const Data& srcPrimaryTag);
        ///
        Data getPrimaryTag() const;
        ///
        void setSubTag(const Data& srcSubTag);
        ///
        Data getSubTag() const;
        ///
        void setqValue(const Data& srcqValue);
        ///
        Data getqValue() const;

     
  ///return the encoded string version of this.
        Data encode() const;
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data primaryTag;
        Data subTag;
        Data qValue;


        void decode(const Data& data);
  
        void scanAcceptLanguage(const Data &tmpdata);
        void parseRange(const Data &tmpdata);
};
 
} // namespace Vocal

#endif
