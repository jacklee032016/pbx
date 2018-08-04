#ifndef SipSdp_HXX
#define SipSdp_HXX
/*
 * $Id: SipSdp.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "SipContentData.hxx"

namespace Vocal
{
    
/// data container for Sdp object in the body of a SIP message.
class SipSdp : public SipContentData
{
    public:
        /// Create one with default values
        SipSdp();
        ///
        SipSdp(const Vocal::SDP::SdpSession& descriptor);
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
        Vocal::SDP::SdpSession&
        getSdpDescriptor() ;
        ///
        void
        setSdpDescriptor(const Vocal::SDP::SdpSession& sdp);
        ///
        void
        setConnAddress(const Vocal::SDP::SdpConnection& connAddr);
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
        Vocal::SDP::SdpSession sdpDescriptor;
};

ostream&
operator<<(ostream& s, SipSdp &msg);
 
} // namespace Vocal

#endif
