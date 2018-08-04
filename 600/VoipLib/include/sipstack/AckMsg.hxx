#ifndef ACKMSG_H
#define ACKMSG_H
/*
 * $Id: AckMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"

namespace Vocal
{
    
class StatusMsg;

/**  \ingroup SIP
     Class to implement the ACK method of a SIP message 
     <p>
     <b>Example</b>
     <p>
     <pre>
       Data url;
       url = "sip:destination@192.168.5.12";
       Sptr <BaseUrl> tourl;
       tourl = BaseUrl::decode(url);
       int listenPort = 5060;
       int rtpPort = 3456;

       InviteMsg invite(tourl, listenPort, rtpPort);
       StatusMsg status(invite, 200);

       AckMsg ack(status);
       ack.getMutableRequestLine().setUrl(tourl);
     </pre>
*/
class AckMsg : public SipCommand
{
    public:
        enum AckMsgForward { ackMsgForward };

        ///
        AckMsg(const Data& );
        ///
        AckMsg(const AckMsg&);

        /** this interface is deprecated and will not work with loose routing.
            @deprecated
        */
        AckMsg(const AckMsg&, enum AckMsgForward);

        /// UAS uses
        AckMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        AckMsg(StatusMsg& statusMsg);

        ///
        AckMsg();
        ///
        AckMsg& operator =(const AckMsg&);

        ///
        bool operator ==(const AckMsg& src) ;

        ///
        virtual ~AckMsg();

        ///
        Method getType() const;

    private:
        /// to be deleted
        void setAckDetails(const StatusMsg& statusMsg);
};
 
}
#endif
