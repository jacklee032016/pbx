#ifndef CANCELMSG_H
#define CANCELMSG_H
/*
* $Log: CancelMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: CancelMsg.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipCommand.hxx"


namespace Assist
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
 
}

#endif
