#ifndef SipSdp_HXX
#define SipSdp_HXX
/*
* $Log: SipSdp.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipSdp.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "SipContentData.hxx"

namespace Assist
{
    
/// data container for Sdp object in the body of a SIP message.
class SipSdp : public SipContentData
{
    public:
        /// Create one with default values
        SipSdp();
        ///
        SipSdp(const Assist::SDP::SdpSession& descriptor);
        ///
        SipSdp(const SipSdp& src);
        ///
        SipSdp& operator =(const SipSdp&);

#if 0
        /// compare equality -- xxx not implemented
        bool const operator==(const SipSdp& rhs) const;
#endif
        ///
        SipSdp( const Data& data );

	///
	~SipSdp();
    
        ///
        Assist::SDP::SdpSession&
        getSdpDescriptor() ;
        ///
        void
        setSdpDescriptor(const Assist::SDP::SdpSession& sdp);
        ///
        void
        setConnAddress(const Assist::SDP::SdpConnection& connAddr);
        ///
        Data
        getConnAddress();
        ///
        void
        setUserName(const Data& name);
        ///
        Data
        getUserName();
        ///
        void
        setAddressType(SDP::AddressType);
        ///
	SDP::AddressType
        getAddressType();
        ///
        void
        setAddress(const Data& address);
        ///
        Data
        getAddress();
        ///
        void
        setRtpPortData(const Data& port);
        ///
        Data
        getRtpPortData();
        ///
        void
        setRtpPort(int port);
        /** get the RTP port requested in this SDP message. if none
            can be found, return -1 */
        int
        getRtpPort() const;
        ///
        void
        setSessionName(const Data& session);
        ///
        Data
        getSessionName();
        ///
        void
        setSessionId(unsigned int sessionId);
        ///
        unsigned int
        getSessionId();

        ///
        Data
        encodeHeaders() const;
        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);
	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;

	///
	bool doCodecsMatch() const;

    private:
        Assist::SDP::SdpSession sdpDescriptor;
};

ostream&
operator<<(ostream& s, SipSdp &msg);
 
} 

#endif
