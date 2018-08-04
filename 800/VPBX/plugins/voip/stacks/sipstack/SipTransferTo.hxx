#ifndef SipTransferTo_HXX
#define SipTransferTo_HXX
/*
* $Log: SipTransferTo.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTransferTo.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "cpLog.h"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "SipHeader.hxx"
#include "Sptr.hxx"

namespace Assist
{
    
///
enum SipTransferToErrorType
{
    DECODE_TRANSFER_TO_FAILED,
    PARSE_TRANSFER_URL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipTransferToParserException
class SipTransferToParserException : public VException
{
    public:
        SipTransferToParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipTransferToParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipTransferToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipTransferToErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipTransferToErrorType value;

};



/// data container for TransferTo header
class SipTransferTo : public SipHeader
{
    public:
        /// Create one with default values
        SipTransferTo(UrlType uType = SIP_URL);
        ///
        SipTransferTo( Sptr <BaseUrl> url);
        ///
        SipTransferTo( const Data& data );
        ///
        SipTransferTo(const SipTransferTo&);

        ///
        Data encode() const;
        ///
        SipTransferTo& operator = (const SipTransferTo&);
        ///
        void setPort(int newport);
        ///
        int getPort();

        ///
        void setPortData(const Data& newport);
        ///
        Data getPortData();

        ///
        void setUser(const Data& newuser);
        ///
        Data getUser();
        ///
        void setDisplayName(const Data& name);
        ///
        Data getDisplayName();
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost();
        ///
        Sptr<BaseUrl> getUrl();
        ///
        void setUrl(Sptr <BaseUrl> tourl);

        ///
        bool operator ==(const SipTransferTo& srcTransferTo) const;

        ///
        void parse(const Data& data);
        void parseSipTransferTo(const Data& data);
        ///
        void scanSipTransferTo(const Data& data);
        ///
        void parseNameInfo(const Data& data);

        ///
        void parseUrl(const Data& data);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Sptr<BaseUrl> toUrl;
        UrlType urlType;
        Data displayName;
        void decode(const Data& data);
        friend class SipMsg;
};

 
}

#endif
