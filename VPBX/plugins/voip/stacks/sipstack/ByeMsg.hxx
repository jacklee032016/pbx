#ifndef BYEMSG_H
#define BYEMSG_H

/*
* $Log: ByeMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: ByeMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipCommand.hxx"

namespace Assist
{

class StatusMsg;

/**
    Impements SIP BYE message.
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
    SipContact myContact(url);
    status.setContact(myContact);

    AckMsg ack(status);
    ack.getMutableRequestLine().setUrl(tourl);

    //construct BYE on status. (If you were UAC for initial INVITE)
    ByeMsg byeMsg(status);
    //Reverse the record route list and append contact at the end
    //if RecordRoute list contains A,B,C,D
    //reverse it an create a list 
    //D,C,B,A, see UaBase::saveRouteList() for details
    SipRecordRouteList rrList = status.getrecordrouteList();
    vector<SipRoute*> newRoutes; //Assume newRoutes contains the processed route list(rrList)

    ///ProcessRoutes
    if(newRoutes.size())
    {
        byeMsg.setRouteList(newRoutes);
        SipRoute siproute = byeMsg.getRoute(0);
        byeMsg.removeRoute(0);
        SipRequestLine& reqLine = byeMsg.getMutableRequestLine();
        reqLine.setUrl( siproute.getUrl() );
    }
    //Set the via 
    byeMsg.flushViaList();
    SipVia via;
    via.setHost(localHostAddr);
    via.setPort(localSipPort);
    byeMsg.setVia(via);

    //Set the seq number to be oldSeq+1, say oldSeq=101
    unsigned int cseq = 102;
    SipCSeq sipCSeq;
    sipCSeq.setCSeq( cseq );
    byeMsg->setCSeq( sipCSeq );
    ///End Process Routes

    //construct BYE on Ack.(if you were UAS for initial INVITE)
    ByeMsg byeAck(ack);
    //Get the record route list from the original request received
    //if RecordRoute list contains A,B,C,D
    //an create a list as A,B,C,D
    //see UaBase::saveRouteList() for details
    vector<SipRoute*> newRoutes; //Assume newRoutes contains the processed route list(rrList)
    //Follow the processing of the route and setting via, same as above

    </pre>
*/
class ByeMsg : public SipCommand
{
    public:
        ///
        enum ByeMsgForward { byeMsgForward };

        ///
        ByeMsg(const SipCommand& sipMessage);
        ///
        ByeMsg(const Data& data);

        ///
        ByeMsg(const ByeMsg&);

        /** this interface is deprecated and will not work with loose routing.
            @deprecated 
        */
        ByeMsg(const ByeMsg&, enum ByeMsgForward);

        /// UAS uses
        ByeMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        ByeMsg(const StatusMsg& statusMsg);

        ///
        ByeMsg& operator =(const ByeMsg&);
        ///
        bool operator ==(const ByeMsg& src); 
        ///        
        ByeMsg();
        ///
        virtual ~ByeMsg();
        ///
        virtual Method getType() const;
    private:
        /// to be deleted
        void setByeDetails(const SipMsg& sipMessage);
};
 
}

#endif
