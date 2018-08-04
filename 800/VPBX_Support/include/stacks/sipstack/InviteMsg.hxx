#ifndef INVITEMSG_H
#define INVITEMSG_H
/*
* $Log: InviteMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: InviteMsg.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "symbols.hxx"
#include "BaseUrl.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

namespace Assist
{

/**
    Implements a SIP INVITE request.
    <p>
    <b>Example</b>
    <p>
    <pre>
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> baseurl = BaseUrl::decode(url);
    Sptr <SipUrl> tourl;
    tourl.dynamicCast(baseurl);
    int listenPort = 5060;
    int rtpPort = 3456;
    InviteMsg invite(tourl, listenPort, rtpPort);
    </pre>
*/
class InviteMsg : public SipCommand
{
    public:
        ///
        enum InviteMsgForward { inviteMsgForward };
        ///
        InviteMsg();

        /// Create a new invite based on a diversion of a prev invite
        InviteMsg( const InviteMsg& msg, Sptr <BaseUrl> uri );
        /// The listenPort is required to build the INVITE msg, in the Via field.
        InviteMsg(Sptr <BaseUrl> url, int listenPort = 5060 , int rtpPort = 3456);  
        /// for forwarding
        InviteMsg(const InviteMsg& msg, enum InviteMsgForward);

        /// UAS uses
        InviteMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        InviteMsg(const StatusMsg& statusMsg);
        /** construct a new InviteMsg based on the text of an INVITE msg. 
            this should not be called by anyone but the SipMsg class
         *  @param data  message to be parsed
         *  @exception   thrown if message fails basic parsing
         */
        InviteMsg( const Data& data ) throw (SipParserException&);
        ///
        InviteMsg& operator =(const InviteMsg&);
        ///
        bool operator ==(const InviteMsg& src) const;
        ///
        InviteMsg(const InviteMsg&);
        ///
        virtual ~InviteMsg(){}
        ///
        virtual Method getType() const;

        //--- SessionTimer headers //
        ///
        const SipSessionExpires& getSessionExpires() const;        
        ///
        const SipMinSE& getMinSE() const ;
        ///
        void setSessionExpires(const SipSessionExpires& sess);
        ///
        void setMinSE(const SipMinSE& minSe);

    private:
        void setInviteDetails(Sptr <BaseUrl> url, int listenPort, int rtpPort);
};
 
}

#endif
