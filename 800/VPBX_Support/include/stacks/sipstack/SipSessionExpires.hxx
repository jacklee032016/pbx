#ifndef SipSessionExpires_HXX
#define SipSessionExpires_HXX
/*
* $Log: SipSessionExpires.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSessionExpires.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{
    
enum SipSessionExpiresErrorType
{
    DECODE_SESSION_EXPIRE_FAILED

    //may need to change this to be more specific
};


/// Exception handling class
class SipSessionExpiresParserException : public VException
{
    public:
        SipSessionExpiresParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipSessionExpiresParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipSessionExpiresErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSessionExpiresErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSessionExpiresErrorType value;
};


/// data container for SessionExpires header
class SipSessionExpires : public SipHeader
{
    public:
        /// Create one with default values
        SipSessionExpires();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipSessionExpires( const Data& srcData );
        SipSessionExpires( const SipSessionExpires& src );

        ///
        bool operator ==( const SipSessionExpires& src ) const;
        ///
        const SipSessionExpires& operator =(const SipSessionExpires& src);
        ///
        void scanSipSessionExpires(const Data& data);

        ///
        void setDelta( const Data& data);
        ///
        const Data& getDelta() const;
        ///
        void setRefreshParam ( const Data& data);
        ///
        const Data& getRefreshParam() const;

        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;
        ///
        void decode(const Data& data);

        ///
        Data getData() const;


	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;

        ///
        void parseError(const string& context);

    private:
        Data delta;
        bool bDelta;
        Data refresher;
        bool bRefresher;

        friend class SipMsg;
};
 
} 

#endif
