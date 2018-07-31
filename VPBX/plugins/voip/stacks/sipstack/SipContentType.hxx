#ifndef SipContentType_HXX
#define SipContentType_HXX

/*
* $Log: SipContentType.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipContentType.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include <map>
#include "Data.hxx"
#include "VException.hxx"
#include "cpLog.h"
#include "Sptr.hxx"
#include "symbols.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"


namespace Assist
{


enum SipContentTypeErrorType
{
    DECODE_CONTENTTYPE_FAILED,
    CONTENTTYPE_PARM_FAILED
    //may need to change this to be more specific
};


/// Exception handling class
class SipContentTypeParserException : public VException
{
    public:
        SipContentTypeParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipContentTypeParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipContentTypeErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
    
        const SipContentTypeErrorType& getError() const
            {
                return value;
            }
    
        string getName() const ;

    private:
        SipContentTypeErrorType value;
    
};

/// data container for ContentType header
class SipContentType : public SipHeader
{
    public:
    
        /// default constructor
        SipContentType();
    
        /// construct a content type from the type and subtype
        SipContentType(const Data& gtype, const Data& gsubType);

        /// copy constructor
        SipContentType( const SipContentType& src);

        /// construct content type from unparsed text
        SipContentType( const Data& data );
    
        ///
        bool operator ==(const SipContentType& other) const;
    
        ///
        const SipContentType& operator=( const SipContentType& src) ;
    
        ///
        Data encode() const;
  
        /// inline
        void setType (const Data& newtype) { 
            type = newtype; 
            type.lowercase(); 
        }
    
        /// inline
        void setSubType(const Data& newsubtype) { 
            subtype = newsubtype;
            subtype.lowercase(); 
        }
    
        ///
        void parse(const Data& data);
    
        ///
        void scanSipContentType(const Data& data);

        /// return the type of the content type
        Data getType() const;
    
        /// return the subtype of the content type
        Data getSubType() const;

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);
        ///
        void clearToken(const Data& token);

        ///
        Data getAttributeValue(const Data& token) const;

        ///
        void setMediatype(const Data& newmediatype);

        ///
        Data getMediatype();
        
        /** method for copying sip headers of any type without knowing
            which type */
        SipHeader* duplicate() const;

        /// compare two headers of (possibly) the same class
        virtual bool compareSipHeader(SipHeader* msg) const;
    private:
    
        Data type;
        Data subtype;
        Data media;
        SipParameterList myParamList;
        void decode(const Data& data);
        friend class SipMsg;    
        friend class SipContentData;
};
 
}

#endif
