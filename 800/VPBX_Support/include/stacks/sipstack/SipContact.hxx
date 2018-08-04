#ifndef SipContact_HXX
#define SipContact_HXX

/*
* $Log: SipContact.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContact.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include <iostream>

#include "Data.hxx"
#include "SipExpires.hxx"
#include "BaseUrl.hxx"
#include "SipDate.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"
#include "VException.hxx"

namespace Assist
{

class SipDate;

enum SipContactErrorType
{
    DECODE_CONTACT_FAILED,
    URL_PARSE_FAILED,
    MANDATORY_ERR,
    CONTACT_PARSE_ADDPRAM_FAILED
    //may need to change this to be more specific
};


/// Exception handling class 
class SipContactParserException : public VException
{
    public:
        SipContactParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipContactParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipContactErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipContactErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContactErrorType value;

};


/// data container for Contact header
class SipContact : public SipHeader
{
    public:
        class AllContactsTag {};
        static const AllContactsTag AllContacts;
        
        /// Create one with default values
        SipContact(UrlType uType = SIP_URL);

        // constructor for creasting acontact with the value '*', used only by
        // register requests
        SipContact(const AllContactsTag & c);

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipContact( const Data& srcData );
        SipContact( const SipContact& src);

        ///
        bool operator ==( const SipContact& src) const;

        ///
        bool operator<( const SipContact& src) const;

        ///
        const SipContact& operator=(const SipContact& src);
        ///
        Sptr<BaseUrl> getUrl() const;

        ///
        void setUrl( Sptr<BaseUrl> item);

        ///
        Data getDisplayName() const;

        ///
        void setDisplayName( const Data& item);

        ///
        bool isWildCard() const;

        ///
        Data getQValue() const;
        ///
        void setQValue( const Data& item);

        void setExpires( const SipExpires& item);
        ///
        SipExpires getExpires() const;

        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;

	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
        ///
        void setNullContact();
    private:

        Sptr<BaseUrl> url;
        UrlType urlType;
        Data displayName;
        Data qValue;
        SipExpires expires;
        SipParameterList myParameterList;

        bool bexpires;
        bool wildCard;

        /// Parser Methods!!
        void parse(const Data& data);
        void scanContactParam(const Data& data);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data& data);
        void decode(const Data& data);
};

ostream&
operator<<(ostream& s, const SipContact &contact);
 
}

#endif
