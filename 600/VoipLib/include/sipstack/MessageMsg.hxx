#ifndef MESSAGEMSG_HXX
#define MESSAGEMSG_HXX
/*
 * $Id: MessageMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
