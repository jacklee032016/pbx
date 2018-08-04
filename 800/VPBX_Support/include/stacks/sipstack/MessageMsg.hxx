#ifndef MESSAGEMSG_HXX
#define MESSAGEMSG_HXX

/*
* $Log: MessageMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: MessageMsg.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipCommand.hxx"

namespace Assist
{

class BaseUrl;

class MessageMsg : public SipCommand
{
    public:
        ///
        static const char INFO_SUBJECT[];
        ///
        enum MessageMsgForward { messageMsgForward };
        ///default constructor
        MessageMsg();
        //The listenPort is required to build the INFOmsg, in the Via field.
        MessageMsg(Sptr <BaseUrl> url, int fromPort);
        ///decode operation
        MessageMsg( const Data& data );
        ///assignment operation
        MessageMsg& operator =(const MessageMsg&);
        ///comparison operation
        bool operator ==(const MessageMsg&);
        ///copy constructor
        MessageMsg(const MessageMsg&);
        ///constructor to forward the message.
        MessageMsg(const MessageMsg&, enum MessageMsgForward);
        ///return the type of the object
        virtual Method getType() const;
        ///destructor
        virtual ~MessageMsg();
    private:
        ///called by all constructors
        void setInfoDetails(Sptr <BaseUrl>  url, int fromPort);
};
 
}

#endif
