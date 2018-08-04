#ifndef NOTIFYMSG_H
#define NOTIFYMSG_H

/*
* $Log: NotifyMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: NotifyMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipCommand.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SubscribeMsg.hxx"
#include "SipCallLeg.hxx"
#include "SipStatusLine.hxx"
#include "VException.hxx"

namespace Assist
{

///
class NotifyMsg : public SipCommand
{
    public:
        ///
        enum NotifyMsgForward { notifyMsgForward };

        class NotReferException : public VException
        {
            public:
                ///
                NotReferException(const string& file,
                                  const int line,
                                  const int error = 0 )
                    : VException("", file, line, error) {}

                string getName() const { return "NotifyMsg::NotReferException"; }
        };

        ///default constructor
        NotifyMsg();
        ///
        NotifyMsg(const Data& data);
        ///Notify messages can only be constructed if a SUBSCRIBE method was first sent.  Since a NOTIFY message can only have one Event header, the event thatis being notified must be passed to the constructor.  A check is made to be sure the event exists in the SUBSCRIBE
        ///message, if not, no event header will be added to the NOTIFY message
        NotifyMsg(const SubscribeMsg& subscribeRequest, const SipSubsNotifyEvent& event);
        ///constructor so that only the call-leg of a SUBSCRIBE message can be stored, not an entire SUBSCRIBE message for each user's SUBSCRIBE message.
        NotifyMsg(const SipCallLeg& callLeg, const SipSubsNotifyEvent& event);
        ///copy constructor
        NotifyMsg(const NotifyMsg&);
        ///construct notify, to forward to the next host(used by the proxy)
        NotifyMsg(const NotifyMsg&, enum NotifyMsgForward);
        ///assignment operator
        const NotifyMsg& operator =(const NotifyMsg&);
        ///comparison operator
        bool operator ==(const NotifyMsg& src);
        ///destructor
        virtual ~NotifyMsg();
        ///called to get the type of message.
        virtual Method getType() const;

        /// the status line of the wrapped message
        SipStatusLine getReferredStatusLine() const;
    private:
        /// message is used as the same from, to, and call-id, however, the from must be used as the request-uri to send the NOTIFY message back to.
        void setNotifyDetails(Sptr <BaseUrl> fromUrl);
};
 
}

#endif
