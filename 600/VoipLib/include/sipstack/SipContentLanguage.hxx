#ifndef SipContentLanguage_HXX
#define SipContentLanguage_HXX
/*
 * $Id: SipContentLanguage.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{


enum SipContentLanguageErrorType
{
    DECODE_CONTENTLANGUAGE_FAILED


    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentLanguageParserException : public VException
{
    public:
        SipContentLanguageParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipContentLanguageParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipContentLanguageErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipContentLanguageErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentLanguageErrorType value;

};
/// data container for Subject header
class SipContentLanguage : public SipHeader
{
    public:
        /// Create one with default values
        SipContentLanguage();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipContentLanguage( const Data& srcData );
        SipContentLanguage( const SipContentLanguage& src );

        void scanSipContentLanguage(const Data& data);
        ///
        Data getPrimaryTag();
        Data getSubTag();
        

        ///
        void setPrimaryTag( const Data& newData);
        void setSubTag(const Data& newData);

        SipContentLanguage& operator=(const SipContentLanguage& src);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        ///
        bool operator==(const SipContentLanguage&) const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data primarytag;
        Data subtag;
        void decode(const Data& data);
};
 
} // namespace Vocal

#endif
