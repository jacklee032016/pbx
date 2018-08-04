#ifndef CANCELMSG_H
#define CANCELMSG_H
/*
 * $Id: CancelMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipCommand.hxx"

namespace Vocal
{
/**
   Implements CANCEL message.
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
    CancelMsg cancel(invite);
   </pre>
*/
class CancelMsg : public SipCommand
{
    public:
        ///
        enum CancelMsgForward { cancelMsgForward };

        ///
        CancelMsg(const SipCommand& sipCommand);
        ///
        CancelMsg(const Data& data);
        ///
        CancelMsg(const CancelMsg& msg, enum CancelMsgForward);
        ///
        CancelMsg& operator=(const CancelMsg&);
        ///
        bool operator ==(const CancelMsg& src); 
        ///
        CancelMsg(const CancelMsg& src);
        ///
        CancelMsg();

        ///
        virtual ~CancelMsg();
        ///
        virtual Method getType() const;
    private:
        ///
        void setCancelDetails(const SipCommand& sipCommand);
};
 
}// namespace Vocal

#endif
