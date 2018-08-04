#ifndef SipCallId_HXX
#define SipCallId_HXX

/*
* $Log: SipCallId.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipCallId.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "HostMatch.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{

enum SipCallIdErrorType
{
    DECODE_CALLID_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipCallIdParserException : public VException
{
    public:
        SipCallIdParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  const int error = 0 );
        SipCallIdParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  SipCallIdErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
        const SipCallIdErrorType& getError() const
            {
                return value;
            }
        string getName() const ;
    private:
        SipCallIdErrorType value;

};
/// data container for CallId header
class SipCallId : public SipHeader
{
    public:
        /// Create one with default values
        SipCallId();

        ///
        SipCallId( const SipCallId& ohter );

        ///
        void setData(const Data& item);

        ///
        Data getData() const;
        Data getLocalId() const
            {
                return localid;
            }
        Data getHost() const
            {
                return host;
            }
        void setLocalId(const Data & newlocalid)
            {
                localid = newlocalid;
            }
        void setHost(const Data & newhost)
            {
                host = newhost;
            }
        ///
        bool operator == ( const SipCallId& other ) const;
        ///
        bool operator != (const SipCallId& other) const
            { return !(*this == other);}
        ///
        const SipCallId& operator = ( const SipCallId& src );

        ///
        bool operator < ( const SipCallId& other ) const;

        ///
         bool operator >( const SipCallId& other ) const;


        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipCallId( const Data& data );

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

        Data getCallIdData() const;

        size_t hashfn()const;

        /// method for copying sip headers of any type without knowing which type
        SipHeader* duplicate() const;
        /// compare two headers of (possibly) the same class
        virtual bool compareSipHeader(SipHeader* msg) const;

    private:
        Data localid;
        Data host;
        size_t hash;
        void decode(const Data& data);
        void getHash();
        void parse(const Data& data);
        void scanSipCallId(const Data& data);
        void parseLocalId(const Data& data);
        void parseHost(const Data& data);

        friend class SipMsg;
};

 ostream& operator<<(ostream& strm, const SipCallId& id);
}

#endif
