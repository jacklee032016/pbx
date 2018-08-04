#ifndef INFOMSG_H
#define INFOMSG_H
/*
 * $Id: InfoMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"

namespace Vocal
{

class BaseUrl;

class InfoMsg : public SipCommand
{
    public:
        ///
        static const char INFO_SUBJECT[];
        ///
        enum InfoMsgForward { infoMsgForward };
        ///default constructor
        InfoMsg();
        //The listenPort is required to build the INFOmsg, in the Via field.
        InfoMsg(Sptr <BaseUrl> url, int fromPort);
        ///decode operation
        InfoMsg( const Data& data );
        ///assignment operation
        InfoMsg& operator =(const InfoMsg&);
        ///comparison operation
        bool operator ==(const InfoMsg&);
        ///copy constructor
        InfoMsg(const InfoMsg&);
        ///constructor to forward the message.
        InfoMsg(const InfoMsg&, enum InfoMsgForward);
        ///return the type of the object
        virtual Method getType() const;
        ///destructor
        virtual ~InfoMsg();
    private:
        ///called by all constructors
        void setInfoDetails(Sptr <BaseUrl>  url, int fromPort);
};
 
} // namespace Vocal

#endif
