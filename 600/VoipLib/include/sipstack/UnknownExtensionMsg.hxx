#ifndef UNKNOWNEXTENSIONMSG_H
#define UNKNOWNEXTENSIONMSG_H
/*
 * $Id: UnknownExtensionMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"

namespace Vocal
{
    
///
class UnknownExtensionMsg : public SipCommand
{
    public:
        ///
        enum UnknownExtensionMsgForward { unknownExtensionMsgForward };
        ///
        UnknownExtensionMsg();

        //The listenPort is required to build the INVITE msg, in the Via field.
        UnknownExtensionMsg(Sptr<BaseUrl> url, int listenPort = 5060, int rtpPort = 3456);

        /// this should bot be called by ayone but he SipMsg class
        UnknownExtensionMsg( Data& data );
        ///
        UnknownExtensionMsg& operator =(const UnknownExtensionMsg&);
        ///
        bool operator ==(const UnknownExtensionMsg& src); 
        ///
        UnknownExtensionMsg(const UnknownExtensionMsg&);

        ///
        UnknownExtensionMsg(const UnknownExtensionMsg&, enum UnknownExtensionMsgForward);

        ///
//        virtual Data encode() const;  // get details and create Message, and return.
        ///
        virtual Method getType() const;

    private:
        void setUnknownExtensionMsgDetails(Sptr <BaseUrl> url, int listenPort, int rtpPort);
};
 
} // namespace Vocal
#endif
